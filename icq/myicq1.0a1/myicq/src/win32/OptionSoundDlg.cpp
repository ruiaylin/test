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

// OptionSoundDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "OptionSoundDlg.h"
#include "icqlink.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionSoundDlg property page

IMPLEMENT_DYNCREATE(COptionSoundDlg, CPropertyPage)

COptionSoundDlg::COptionSoundDlg() : CPropertyPage(COptionSoundDlg::IDD)
{
	//{{AFX_DATA_INIT(COptionSoundDlg)
	//}}AFX_DATA_INIT

	curSel = 0;

	bitset<NR_USER_FLAGS> &f = icqLink->options.flags;
	m_soundOn = (f.test(UF_SOUND_ON) ? 0 : 1);
}

COptionSoundDlg::~COptionSoundDlg()
{
}

void COptionSoundDlg::enableControls(BOOL enable)
{
	GetDlgItem(IDC_SOUND_EVENT)->EnableWindow(enable);
	GetDlgItem(IDC_SOUND_FILE)->EnableWindow(enable);
	GetDlgItem(IDC_PLAY)->EnableWindow(enable);
	GetDlgItem(IDC_BROWSE)->EnableWindow(enable);
}

void COptionSoundDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionSoundDlg)
	DDX_Control(pDX, IDC_SOUND_EVENT, m_cmbSoundEvent);
	DDX_Radio(pDX, IDC_SOUND_ON, m_soundOn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionSoundDlg, CPropertyPage)
	//{{AFX_MSG_MAP(COptionSoundDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_CBN_SELCHANGE(IDC_SOUND_EVENT, OnSelchangeSoundEvent)
	ON_BN_CLICKED(IDC_SOUND_ON, OnSoundOn)
	ON_BN_CLICKED(IDC_SOUND_OFF, OnSoundOff)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionSoundDlg message handlers

void COptionSoundDlg::OnBrowse() 
{
	CFileDialog dlg(TRUE, "wav", NULL, OFN_HIDEREADONLY, "*.wav|*.wav||", this);
	if (dlg.DoModal() == IDOK)
		SetDlgItemText(IDC_SOUND_FILE, dlg.GetPathName());	
}

void COptionSoundDlg::OnPlay() 
{
	CString file;
	GetDlgItemText(IDC_SOUND_FILE, file);
	PlaySound(file, NULL, SND_ASYNC);
}

void COptionSoundDlg::OnSelchangeSoundEvent() 
{
	GetDlgItemText(IDC_SOUND_FILE, soundFiles[curSel]);
	curSel = m_cmbSoundEvent.GetCurSel();
	SetDlgItemText(IDC_SOUND_FILE, soundFiles[curSel]);
}

BOOL COptionSoundDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	IcqOption &options = icqLink->options;

	for (int i = 0; i < NR_SOUNDS; i++)
		soundFiles[i] = options.soundFiles[i].c_str();
	m_cmbSoundEvent.SetCurSel(curSel);
	SetDlgItemText(IDC_SOUND_FILE, soundFiles[curSel]);

	enableControls(m_soundOn == 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionSoundDlg::OnOK() 
{
	icqLink->options.flags.set(UF_SOUND_ON, m_soundOn == 0);

	GetDlgItemText(IDC_SOUND_FILE, soundFiles[curSel]);
	for (int i = 0; i < NR_SOUNDS; i++)
		icqLink->options.soundFiles[i] = soundFiles[i];	
	
	CPropertyPage::OnOK();
}

void COptionSoundDlg::OnSoundOn() 
{
	enableControls();
}

void COptionSoundDlg::OnSoundOff() 
{
	enableControls(FALSE);
}
