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

// ChatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "contactinfo.h"
#include "icqlinkbase.h"
#include "ChatDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_TOTAL_TIME		1001

static CChatDlg *pChatDlg;

/////////////////////////////////////////////////////////////////////////////
// CChatDlg dialog

static void CALLBACK onTimer(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	KillTimer(hwnd, idEvent);

	pChatDlg->destroy();
}

CChatDlg::CChatDlg(TcpSessionBase *tcp, CWnd* pParent /*=NULL*/)
: CDialog(CChatDlg::IDD, pParent), ChatSession(tcp)
{
	//{{AFX_DATA_INIT(CChatDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	icqLink = tcp->getLink();
}

CChatDlg::~CChatDlg()
{
}

void CChatDlg::destroy()
{
	tcp->destroy();

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DestroyWindow();
}

BOOL CChatDlg::initSpeech()
{
	waveIn.open(1, 8000, getBitsPerSample(), m_hWnd);
	int n = getFrameSize() * 10;
	waveIn.addBuffer(n);
	waveIn.addBuffer(n);

	waveOut.open(1, 8000, getBitsPerSample(), m_hWnd);

	m_sendVol.SetRange32(0, 32768);
	m_recvVol.SetRange32(0, 32768);

	waveIn.start();

	return TRUE;
}

void CChatDlg::onChatText(const char *text)
{
	SetDlgItemText(IDC_RECV_TEXT, text);
}

void CChatDlg::onSpeechData(const char *frame, int n)
{
	short val = *(short *) frame;
	m_recvVol.SetPos(abs(val));
	waveOut.play(frame, n);
}

void CChatDlg::onClose(bool prompt)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (prompt) {
		CString text;
		text.LoadString(IDS_REMOTE_CLOSE);
		MessageBox(text);
	}

	DestroyWindow();
}

void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChatDlg)
	DDX_Control(pDX, IDC_VOLUME_SEND, m_sendVol);
	DDX_Control(pDX, IDC_VOLUME_RECV, m_recvVol);
	DDX_Control(pDX, IDC_SEND_TEXT, m_sendEdit);
	DDX_Control(pDX, IDC_CONTACT_FACE, m_contactFace);
	DDX_Control(pDX, IDC_MY_FACE, m_myFace);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChatDlg, CDialog)
	//{{AFX_MSG_MAP(CChatDlg)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_SEND_TEXT, OnChangeSendText)
	//}}AFX_MSG_MAP
	ON_MESSAGE(MM_WIM_DATA, onWaveInData)
	ON_MESSAGE(MM_WOM_DONE, onWaveOutDone)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChatDlg message handlers

BOOL CChatDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	HICON icon = AfxGetApp()->LoadIcon(IDI_CHAT);
	SetIcon(icon, FALSE);

	ContactInfo *my = icqLink->getContactInfo(NULL);
	CString str;
	str.Format("MyICQ#: %lu", my->qid.uin);
	m_myFace.SetIcon((HICON) icqLink->getFaceIcon(my->face, STATUS_ONLINE));
	SetDlgItemText(IDC_MY_UIN, str);

	QID qid;
	tcp->getRemoteQID(qid);
	ContactInfo *c = icqLink->getContactInfo(&qid);
	str.Format("MyICQ#: %lu", c->qid.uin);
	m_contactFace.SetIcon((HICON) icqLink->getFaceIcon(c->face, STATUS_ONLINE));
	SetDlgItemText(IDC_CONTACT_UIN, str);

	str.Format(IDS_CHAT_TITLE, my->nick.c_str(), c->nick.c_str());
	SetWindowText(str);

	startTime = CTime::GetCurrentTime();
	SetTimer(IDT_TOTAL_TIME, 1000, NULL);

	initSpeech();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChatDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == IDT_TOTAL_TIME) {
		CTime now = CTime::GetCurrentTime();
		CTimeSpan span = now - startTime;
		SetDlgItemText(IDC_TOTAL_TIME, span.Format("%H:%M:%S"));
	} else
		CDialog::OnTimer(nIDEvent);
}

void CChatDlg::OnChangeSendText() 
{
	CString str;
	GetDlgItemText(IDC_SEND_TEXT, str);
	sendChatText(str);
}

void CChatDlg::OnCancel()
{
	CString str;
	str.LoadString(IDS_PROMPT_CLOSE);
	if (MessageBox(str, NULL, MB_YESNO) == IDYES) {
		pChatDlg = this;
		::SetTimer(NULL, 0, 0, onTimer);
	}
}

LRESULT CChatDlg::onWaveInData(WPARAM wParam, LPARAM lParam)
{
	HWAVEIN hwi = (HWAVEIN) wParam;
	LPWAVEHDR pwh = (LPWAVEHDR) lParam;

	waveInUnprepareHeader(hwi, pwh, sizeof(WAVEHDR));

	short val = *(short *) pwh->lpData;
	m_sendVol.SetPos(abs(val));
	sendSpeechData(pwh->lpData, pwh->dwBytesRecorded);

	waveInPrepareHeader(hwi, pwh, sizeof(WAVEHDR));
	waveInAddBuffer(hwi, pwh, sizeof(WAVEHDR));

	return 0;
}

LRESULT CChatDlg::onWaveOutDone(WPARAM wParam, LPARAM lParam)
{
	HWAVEOUT hwo = (HWAVEOUT) wParam;
	LPWAVEHDR pwh = (LPWAVEHDR) lParam;

	waveOutUnprepareHeader(hwo, pwh, sizeof(WAVEHDR));
	delete []pwh->lpData;
	delete pwh;

	return 0;
}

void CChatDlg::PostNcDestroy() 
{
	delete this;
}
