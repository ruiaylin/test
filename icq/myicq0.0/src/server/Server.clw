; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "server.h"
LastPage=0

ClassCount=8
Class1=CMainFrame
Class2=CServerApp
Class3=CAboutDlg
Class4=CServerDoc
Class5=CServerView
Class6=CSetupDlg
Class7=CRecvSocket
Class8=CSendSocket

ResourceCount=4
Resource1=IDD_SETUP
Resource2=IDD_ABOUTBOX
Resource3=IDR_MAINFRAME
Resource4=IDR_POPUP

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
VirtualFilter=fWC
LastObject=CMainFrame

[CLS:CServerApp]
Type=0
BaseClass=CWinApp
HeaderFile=Server.h
ImplementationFile=Server.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=Server.cpp
ImplementationFile=Server.cpp
LastObject=CAboutDlg

[CLS:CServerDoc]
Type=0
BaseClass=CDocument
HeaderFile=ServerDoc.h
ImplementationFile=ServerDoc.cpp

[CLS:CServerView]
Type=0
BaseClass=CEditView
HeaderFile=ServerView.h
ImplementationFile=ServerView.cpp

[CLS:CSetupDlg]
Type=0
BaseClass=CDialog
HeaderFile=SetupDlg.h
ImplementationFile=SetupDlg.cpp

[CLS:CRecvSocket]
Type=0
BaseClass=CAsyncSocket
HeaderFile=Socket.h
ImplementationFile=Socket.cpp

[CLS:CSendSocket]
Type=0
BaseClass=CAsyncSocket
HeaderFile=Socket.h
ImplementationFile=Socket.cpp

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_SETUP]
Type=1
Class=CSetupDlg
ControlCount=24
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_IP,edit,1350633600
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_PORT,edit,1350631552
Control7=IDC_LISTPOST,listbox,1352728835
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_SENDPORT,edit,1350762624
Control11=IDC_STATIC,static,1342308352
Control12=IDC_DATASOURCE,edit,1350631552
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_UID,edit,1350631552
Control16=IDC_PWD,edit,1350631552
Control17=IDC_ADD,button,1342242816
Control18=IDC_CONFIG_DS,button,1342242816
Control19=IDC_STATIC,button,1342177287
Control20=IDC_STATIC,button,1342177287
Control21=IDC_DELETE,button,1342242816
Control22=IDC_STATIC,static,1342308352
Control23=IDC_BROADCASTPWD,edit,1350631552
Control24=IDC_DEFAULT_SETUP,button,1342242816

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_START_SERVER
Command9=ID_CLOSE_SERVER
Command10=ID_SETUP_SERVER
Command11=ID_APP_ABOUT
CommandCount=11

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_SAVE
Command3=ID_FILE_SAVE_AS
Command4=ID_APP_EXIT
Command5=ID_USER_ONLINE
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_START_SERVER
Command9=ID_CLOSE_SERVER
Command10=ID_SETUP_SERVER
Command11=ID_APP_ABOUT
CommandCount=11

[MNU:IDR_POPUP]
Type=1
Class=?
Command1=ID_SHOWWINDOW
Command2=ID_START_SERVER
Command3=ID_CLOSE_SERVER
Command4=ID_SETUP_SERVER
Command5=ID_APP_ABOUT
Command6=ID_APP_EXIT
CommandCount=6

