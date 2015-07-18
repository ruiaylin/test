#if !defined(AFX_TRAYICON_H__185BF3A6_33BD_11D5_806D_00E04C41C3AD__INCLUDED_)
#define AFX_TRAYICON_H__185BF3A6_33BD_11D5_806D_00E04C41C3AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrayIcon.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon command target

class CTrayIcon : public CCmdTarget
{
	DECLARE_DYNCREATE(CTrayIcon)
	          

// Attributes
public:
	CTrayIcon();
	~CTrayIcon();

// Operations
public:
	void SetNotificationWnd(CWnd* pNotifyWnd,UINT uCbMsg);
	BOOL SetIcon(UINT uID,LPCSTR lpTip); 	
	LRESULT OnTrayNotification(WPARAM uID, LPARAM lEvent);	

protected:
	NOTIFYICONDATA m_nid;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAYICON_H__185BF3A6_33BD_11D5_806D_00E04C41C3AD__INCLUDED_)
