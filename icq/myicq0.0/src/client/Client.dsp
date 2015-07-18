# Microsoft Developer Studio Project File - Name="Client" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Client - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Client.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Client.mak" CFG="Client - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Client - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Client - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Client - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"..\уе╫э\Client.exe"

!ELSEIF  "$(CFG)" == "Client - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"..\уе╫э\Clientd.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Client - Win32 Release"
# Name "Client - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AddFriendDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BmpButton.cpp
# End Source File
# Begin Source File

SOURCE=.\BmpComBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ChangePwd.cpp
# End Source File
# Begin Source File

SOURCE=.\Client.cpp
# End Source File
# Begin Source File

SOURCE=.\Client.rc
# End Source File
# Begin Source File

SOURCE=.\ClientSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\common.cpp
# End Source File
# Begin Source File

SOURCE=.\Data.cpp
# End Source File
# Begin Source File

SOURCE=.\FriendDetail.cpp
# End Source File
# Begin Source File

SOURCE=.\GfxGroupEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\GfxOutBarCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\GfxPopupMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\LoginDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LookDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ModifyPIDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Msg.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiSendDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RegisterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RequestFriendDetail.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SendBroadcastDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SendToAllDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupServerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ShowAddMsgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ShowBroadcastDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ShowOnlineDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TalkDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TrayIcon.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AddFriendDlg.h
# End Source File
# Begin Source File

SOURCE=.\BmpButton.h
# End Source File
# Begin Source File

SOURCE=.\BmpComBox.h
# End Source File
# Begin Source File

SOURCE=.\ChangePwd.h
# End Source File
# Begin Source File

SOURCE=.\Client.h
# End Source File
# Begin Source File

SOURCE=.\ClientSocket.h
# End Source File
# Begin Source File

SOURCE=.\Common.h
# End Source File
# Begin Source File

SOURCE=.\Data.h
# End Source File
# Begin Source File

SOURCE=.\FriendDetail.h
# End Source File
# Begin Source File

SOURCE=.\GfxGroupEdit.h
# End Source File
# Begin Source File

SOURCE=.\GfxOutBarCtrl.h
# End Source File
# Begin Source File

SOURCE=.\GfxPopupMenu.h
# End Source File
# Begin Source File

SOURCE=.\LoginDlg.h
# End Source File
# Begin Source File

SOURCE=.\LookDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\ModifyPIDlg.h
# End Source File
# Begin Source File

SOURCE=.\Msg.h
# End Source File
# Begin Source File

SOURCE=.\MultiSendDlg.h
# End Source File
# Begin Source File

SOURCE=.\RegisterDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SearchDlg.h
# End Source File
# Begin Source File

SOURCE=.\SendBroadcastDlg.h
# End Source File
# Begin Source File

SOURCE=.\SendToAllDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetupServerDlg.h
# End Source File
# Begin Source File

SOURCE=.\ShowAddMsgDlg.h
# End Source File
# Begin Source File

SOURCE=.\ShowBroadcastDlg.h
# End Source File
# Begin Source File

SOURCE=.\ShowOnlineDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TalkDlg.h
# End Source File
# Begin Source File

SOURCE=.\TrayIcon.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\aaa.ico
# End Source File
# Begin Source File

SOURCE=.\res\bmp382.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp682.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp683.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp684.bmp
# End Source File
# Begin Source File

SOURCE=.\res\check_la.ico
# End Source File
# Begin Source File

SOURCE=.\res\Checkmrk.ico
# End Source File
# Begin Source File

SOURCE=.\res\Client.ico
# End Source File
# Begin Source File

SOURCE=.\res\Client.rc2
# End Source File
# Begin Source File

SOURCE=.\res\cur1320.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur268.cur
# End Source File
# Begin Source File

SOURCE=.\res\find.bmp
# End Source File
# Begin Source File

SOURCE=.\res\find1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\find2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\GlobalDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\help.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon4.ico
# End Source File
# Begin Source File

SOURCE=.\res\icr_hand.cur
# End Source File
# Begin Source File

SOURCE=.\res\idr_chat.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_find.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_frie.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_home.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_mdis.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_mess.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_noti.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_onhi.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_onli.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_qq.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_sear.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_sj.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_sj1.ico
# End Source File
# Begin Source File

SOURCE=.\res\LargeIcon.bmp
# End Source File
# Begin Source File

SOURCE=.\res\mess_man.bmp
# End Source File
# Begin Source File

SOURCE=.\res\message.bmp
# End Source File
# Begin Source File

SOURCE=".\res\Network Neighbourhood.ico"
# End Source File
# Begin Source File

SOURCE=.\res\person_s.bmp
# End Source File
# Begin Source File

SOURCE=.\res\quit.bmp
# End Source File
# Begin Source File

SOURCE=.\res\red.ico
# End Source File
# Begin Source File

SOURCE=.\res\SmallIcon.bmp
# End Source File
# Begin Source File

SOURCE=.\res\sys_setu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\up1.ico
# End Source File
# Begin Source File

SOURCE=.\res\User.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
