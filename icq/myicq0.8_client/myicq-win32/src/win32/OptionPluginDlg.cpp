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

// OptionPluginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myicq.h"
#include "OptionPluginDlg.h"
#include "icqplugin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionPluginDlg property page

IMPLEMENT_DYNCREATE(COptionPluginDlg, CPropertyPage)

COptionPluginDlg::COptionPluginDlg() : CPropertyPage(COptionPluginDlg::IDD)
{
	//{{AFX_DATA_INIT(COptionPluginDlg)
	//}}AFX_DATA_INIT
}

COptionPluginDlg::~COptionPluginDlg()
{
}

void COptionPluginDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionPluginDlg)
	DDX_Control(pDX, IDC_PLUGIN_LIST, m_pluginList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionPluginDlg, CPropertyPage)
	//{{AFX_MSG_MAP(COptionPluginDlg)
	ON_BN_CLICKED(IDC_CONFIG, OnConfig)
	ON_BN_CLICKED(IDC_ABOUT, OnAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionPluginDlg message handlers

BOOL COptionPluginDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	PtrList &l = PluginFactory::getPluginList();
	imageList.Create(32, 32, ILC_MASK, l.size(), 0);
	m_pluginList.SetImageList(&imageList, LVSIL_NORMAL);

	PtrList::iterator it;
	int i = 0;
	for (it = l.begin(); it != l.end(); ++it) {
		IcqPlugin *plugin = (IcqPlugin *) *it;
		if (plugin->type == ICQ_PLUGIN_TOY) {
			NetPlugin *p = (NetPlugin *) plugin;
			string name;
			HICON icon = (HICON) p->getNameIcon(name);
			int image = imageList.Add(icon);
			int item = m_pluginList.InsertItem(i++, name.c_str(), image);
			m_pluginList.SetItemData(item, (DWORD) p);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionPluginDlg::OnConfig() 
{
	POSITION pos = m_pluginList.GetFirstSelectedItemPosition();
	if (pos) {
		int i = m_pluginList.GetNextSelectedItem(pos);
		ToyPlugin *p = (ToyPlugin *) m_pluginList.GetItemData(i);
		if (p->configure)
			p->configure(getApp()->getProfile(p->name.c_str()));
	}
}

void COptionPluginDlg::OnAbout() 
{
	POSITION pos = m_pluginList.GetFirstSelectedItemPosition();
	if (pos) {
		int i = m_pluginList.GetNextSelectedItem(pos);
		ToyPlugin *p = (ToyPlugin *) m_pluginList.GetItemData(i);
		if (p->about)
			p->about();
	}
}
