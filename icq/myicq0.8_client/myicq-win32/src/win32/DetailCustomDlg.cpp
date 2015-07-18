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

// DetailCustomDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "DetailCustomDlg.h"
#include "ViewDetailDlg.h"
#include <mmsystem.h>
#include "icqdb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDS_OPTION_FIRST	IDS_OPTION_INVISIBLE
#define IDS_OPTION_LAST		IDS_OPTION_IGNORE

/////////////////////////////////////////////////////////////////////////////
// CDetailCustomDlg property page

IMPLEMENT_DYNCREATE(CDetailCustomDlg, CPropertyPage)

CDetailCustomDlg::CDetailCustomDlg() : CPropertyPage(CDetailCustomDlg::IDD)
{
	//{{AFX_DATA_INIT(CDetailCustomDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	curSel = 0;
}

CDetailCustomDlg::~CDetailCustomDlg()
{
}

void CDetailCustomDlg::enableGreeting()
{
	int i = IDS_OPTION_GREETING - IDS_OPTION_FIRST;
	GetDlgItem(IDC_GREETING)->EnableWindow(m_lstOptions.GetCheck(i) == 1);
}

void CDetailCustomDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDetailCustomDlg)
	DDX_Control(pDX, IDC_SOUND, m_cmbSound);
	DDX_Control(pDX, IDC_OPTIONS, m_lstOptions);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDetailCustomDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CDetailCustomDlg)
	ON_LBN_SELCHANGE(IDC_OPTIONS, OnSelchangeOptions)
	ON_BN_CLICKED(IDC_CUSTOM_SOUND, OnCustomSound)
	ON_CBN_SELCHANGE(IDC_SOUND, OnSelchangeSound)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_MODIFY, OnModify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDetailCustomDlg message handlers

BOOL CDetailCustomDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	DWORD style = GetClassLong(m_lstOptions, GCL_STYLE);
	SetClassLong(m_lstOptions, GCL_STYLE, style & ~CS_DBLCLKS);
	
	IcqContact *contact = ((CViewDetailDlg *) GetParent())->contact;
	bitset<NR_CONTACT_FLAGS> &f = contact->flags;
	for (int id = IDS_OPTION_FIRST; id <= IDS_OPTION_LAST; id++) {
		CString str;
		str.LoadString(id);
		int i = m_lstOptions.AddString(str);
		m_lstOptions.SetCheck(i, f.test(i));
	}
	CheckDlgButton(IDC_CUSTOM_SOUND, contact->flags.test(CF_CUSTOMSOUND));

	enableGreeting();
	OnCustomSound();
	m_cmbSound.SetCurSel(curSel);
	for (int i = 0; i < NR_CUSTOM_SOUNDS; i++)
		soundFiles[i] = contact->soundFiles[i].c_str();
	SetDlgItemText(IDC_FILE, soundFiles[curSel]);
	SetDlgItemText(IDC_GREETING, contact->greeting.c_str());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDetailCustomDlg::OnSelchangeOptions() 
{
	int i = m_lstOptions.GetCurSel();
	if (i == (IDS_OPTION_GREETING - IDS_OPTION_FIRST))
		enableGreeting();
}

void CDetailCustomDlg::OnCustomSound() 
{
	BOOL enable = IsDlgButtonChecked(IDC_CUSTOM_SOUND);
	GetDlgItem(IDC_SOUND)->EnableWindow(enable);
	GetDlgItem(IDC_PLAY)->EnableWindow(enable);
	GetDlgItem(IDC_FILE)->EnableWindow(enable);
	GetDlgItem(IDC_BROWSE)->EnableWindow(enable);
}

void CDetailCustomDlg::OnSelchangeSound() 
{
	CString str;
	GetDlgItemText(IDC_FILE, str);
	soundFiles[curSel] = str;

	curSel = m_cmbSound.GetCurSel();
	SetDlgItemText(IDC_FILE, soundFiles[curSel]);
}

void CDetailCustomDlg::OnPlay() 
{
	CString fileName;
	GetDlgItemText(IDC_FILE, fileName);
	PlaySound(fileName, NULL, SND_FILENAME);
}

void CDetailCustomDlg::OnBrowse() 
{
	CFileDialog dlg(TRUE, "wav", NULL, OFN_HIDEREADONLY, "*.wav|*.wav||", this);
	if (dlg.DoModal() == IDOK)
		SetDlgItemText(IDC_FILE, dlg.GetPathName());
}

void CDetailCustomDlg::OnModify() 
{
	IcqContact *contact = ((CViewDetailDlg *) GetParent())->contact;
	bitset<NR_CONTACT_FLAGS> &f = contact->flags;

	int count = m_lstOptions.GetCount();
	for (int i = 0; i < count; i++)
		f.set(i, m_lstOptions.GetCheck(i) == 1);

	f.set(CF_CUSTOMSOUND, IsDlgButtonChecked(IDC_CUSTOM_SOUND));

	GetDlgItemText(IDC_FILE, soundFiles[curSel]);
	for (i = 0; i < NR_CUSTOM_SOUNDS; i++)
		contact->soundFiles[i] = soundFiles[i];

	CString str;
	GetDlgItemText(IDC_GREETING, str);
	contact->greeting = str;

	IcqDB::saveContact(*contact);
}

void CDetailCustomDlg::OnCancel() 
{
	GetParent()->DestroyWindow();
}
