/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright            : (C) 2002 by Zhang Yong                         *
 *   email                : z-yong163@163.com                              *
 ***************************************************************************/

// SendFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SendFile.h"
#include "SendFileDlg.h"
#include "icqlinkbase.h"
#include "contactinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_BROWSE_FILE		1001
#define IDT_ANIM			1002

static CWnd *pWnd;

static VOID CALLBACK onTimer(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	KillTimer(hwnd, idEvent);
	delete pWnd;
}

/////////////////////////////////////////////////////////////////////////////
// CSendFileDlg dialog

CSendFileDlg::CSendFileDlg(FileSession *s, CWnd* pParent /*=NULL*/)
	: CDialog(CSendFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendFileDlg)
	//}}AFX_DATA_INIT

	session = s;
	icqLink = s->getLink();

	uint32 uin = s->getUIN();
	contact = icqLink->getContactInfo(uin);
	m_uin.Format("%lu", uin);
	m_nick = contact->nick.c_str();

	bytesSent = 0;
	lastBytes = 0;
	lastTime = 0;
	frame = 0;
}

CSendFileDlg::~CSendFileDlg()
{
	if (session)
		delete session;
}

const char *CSendFileDlg::getPathName(const char *name, uint32 size)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	fileName = name;
	fileSize = size;

	CString str;
	str.Format(IDS_FILESIZE, fileSize);
	SetDlgItemText(IDC_FILESIZE, str);
	str.LoadString(IDS_RECEIVING_FILE);
	SetDlgItemText(IDC_STATUS, str);

	CFileDialog dlg(FALSE, NULL, fileName,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "*.*|*.*||", this);
	if (dlg.DoModal() != IDOK)
		return NULL;

	pathName = dlg.GetPathName();
	str.Format(IDS_TITLE_RECVFILE, (LPCTSTR) pathName);
	SetWindowText(str);

	m_ctlProgress.SetRange32(0, fileSize);

	return pathName;
}

void CSendFileDlg::onFileReceive()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString str;
	str.LoadString(IDS_SENDING_FILE);
	SetDlgItemText(IDC_STATUS, str);
}

void CSendFileDlg::onFileProgress(uint32 bytes)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	DWORD thisTime = GetTickCount();
	bool isSend = session->isSend;

	if (bytesSent == 0) {
		lastBytes = 0;
		lastTime = thisTime;
		CString str;
		str.LoadString(isSend ? IDS_SENDING_FILE : IDS_RECEIVING_FILE);
		SetDlgItemText(IDC_STATUS, str);
	}
	bytesSent = bytes;
	m_ctlProgress.SetPos(bytesSent);

	CString str;
	int percent = (int) (bytesSent * 100.0f / fileSize);
	str.Format(isSend ? IDS_SENDFILE_BYTES : IDS_RECVFILE_BYTES,
		bytesSent, percent);
	SetDlgItemText(IDC_STATUS_DETAIL, str);

	if (thisTime - lastTime >= 500) {
		float speed = (float) (bytesSent - lastBytes) / (thisTime - lastTime) / 1.024f;
		lastTime = thisTime;
		lastBytes = bytesSent;

		str.Format("%.1fKB/S", speed);
		SetDlgItemText(IDC_SPEED, str);
	}
}

void CSendFileDlg::onFileFinished()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CString text, title;
	if (bytesSent == fileSize) {
		title.LoadString(IDS_FINISHED);
		text.LoadString(session->isSend ? IDS_SUCCESS_SENDFILE : IDS_SUCCESS_RECVFILE);
	} else {
		title.LoadString(IDS_FAILED);
		text.LoadString(IDS_ERROR_SENDFILE);
	}
	MessageBox(text, title);

	DestroyWindow();
}

void CSendFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendFileDlg)
	DDX_Control(pDX, IDC_FACE, m_faceStatic);
	DDX_Control(pDX, IDC_PROGRESS, m_ctlProgress);
	DDX_Text(pDX, IDC_UIN, m_uin);
	DDX_Text(pDX, IDC_NICK, m_nick);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendFileDlg, CDialog)
	//{{AFX_MSG_MAP(CSendFileDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendFileDlg message handlers

BOOL CSendFileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString str;
	if (session->isSend)
		SetTimer(IDT_BROWSE_FILE, 0, NULL);
	else {
		str.LoadString(IDS_ACCEPT_FILE);
		SetWindowText(str);
	}

	str.LoadString(IDS_PLEASE_WAIT);
	SetDlgItemText(IDC_STATUS, str);

	HICON icon = AfxGetApp()->LoadIcon(IDI_SENDFILE);
	SetIcon(icon, TRUE);
	SetIcon(icon, FALSE);

	m_faceStatic.SetIcon((HICON) icqLink->getFaceIcon(contact->face, STATUS_ONLINE));

	SetTimer(IDT_ANIM, 500, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSendFileDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == IDT_BROWSE_FILE) {
		KillTimer(nIDEvent);	// one shot trigger

		CFileDialog dlg(TRUE, NULL, NULL,
			OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, "*.*|*.*||", this);
		if (dlg.DoModal() != IDOK) {
			DestroyWindow();
			return;
		}

		pathName = dlg.GetPathName();
		fileName = dlg.GetFileName();
		CFile file(pathName, CFile::modeRead);
		fileSize = file.GetLength();
		m_ctlProgress.SetRange32(0, fileSize);
		file.Close();

		CString str;
		str.Format(IDS_TITLE_SENDFILE, (LPCTSTR) pathName);
		SetWindowText(str);
		str.Format(IDS_FILESIZE, fileSize);
		SetDlgItemText(IDC_FILESIZE, str);

		session->sendFileInfo(pathName, fileName, fileSize);

	} else if (nIDEvent == IDT_ANIM) {
		frame ^= 1;
		m_faceStatic.SetIcon(
			(HICON) icqLink->getFaceIcon(contact->face, frame ? STATUS_AWAY : STATUS_ONLINE));

	} else
		CDialog::OnTimer(nIDEvent);
}

void CSendFileDlg::OnCancel() 
{
	CString title, text;
	title.LoadString(IDS_WARNING);
	text.LoadString(IDS_PROMPT_STOP);
	
	if (MessageBox(text, title, MB_YESNO) == IDYES)
		DestroyWindow();
}

void CSendFileDlg::PostNcDestroy() 
{
	session->setListener(NULL);
	pWnd = this;
	::SetTimer(NULL, 0, 0, onTimer);
}
