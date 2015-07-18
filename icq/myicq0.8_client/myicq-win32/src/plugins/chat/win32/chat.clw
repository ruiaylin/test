; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
ClassCount=3
Class1=CChatApp
LastClass=CChatDlg
NewFileInclude2=#include "chat.h"
ResourceCount=1
NewFileInclude1=#include "stdafx.h"
Class2=CChatDlg
LastTemplate=CEdit
Class3=CChatEdit
Resource1=IDD_CHAT

[CLS:CChatApp]
Type=0
HeaderFile=chat.h
ImplementationFile=chat.cpp
Filter=N
LastObject=CChatApp

[CLS:CChatDlg]
Type=0
HeaderFile=ChatDlg.h
ImplementationFile=ChatDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CChatDlg
VirtualFilter=dWC

[DLG:IDD_CHAT]
Type=1
Class=CChatDlg
ControlCount=13
Control1=IDC_SEND_TEXT,edit,1351684100
Control2=IDC_RECV_TEXT,edit,1351684100
Control3=IDC_STATIC,button,1342177287
Control4=IDC_MY_UIN,static,1342308353
Control5=IDC_CONTACT_UIN,static,1342308353
Control6=IDC_TOTAL_TIME,static,1342308353
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,button,1342177287
Control9=IDC_STATIC,button,1342177287
Control10=IDC_VOLUME_RECV,msctls_progress32,1350565888
Control11=IDC_VOLUME_SEND,msctls_progress32,1350565888
Control12=IDC_MY_FACE,static,1342179331
Control13=IDC_CONTACT_FACE,static,1342179331

[CLS:CChatEdit]
Type=0
HeaderFile=ChatEdit.h
ImplementationFile=ChatEdit.cpp
BaseClass=CEdit
Filter=W
VirtualFilter=WC
LastObject=CChatEdit

