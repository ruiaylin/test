# Microsoft Developer Studio Project File - Name="myicq" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=myicq - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "myicq.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "myicq.mak" CFG="myicq - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "myicq - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "myicq - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "myicq - Win32 Release"

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
# ADD CPP /nologo /MD /W2 /GX /O2 /I "../libmyicq" /I "../libmyicq/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib libmyicq.lib libdb40.lib /nologo /subsystem:windows /machine:I386 /out:"../../MyICQ.exe" /libpath:"../libmyicq/release" /libpath:"d:\db-4.0.14\build_win32\Release"

!ELSEIF  "$(CFG)" == "myicq - Win32 Debug"

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
# ADD CPP /nologo /MDd /W2 /Gm /GX /ZI /Od /I "../libmyicq" /I "../libmyicq/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib libmyicq.lib libdb40d.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../MyICQ.exe" /pdbtype:sept /libpath:"../libmyicq/debug" /libpath:"d:\db-4.0.14\build_win32\Debug"

!ENDIF 

# Begin Target

# Name "myicq - Win32 Release"
# Name "myicq - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimFace.cpp
# End Source File
# Begin Source File

SOURCE=.\BtnST.cpp
# End Source File
# Begin Source File

SOURCE=.\CaptionBtnDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\COLORLIS.CPP
# End Source File
# Begin Source File

SOURCE=.\DelFriendDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DetailAuthDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DetailBasicDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DetailCommDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DetailCustomDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DetailMiscDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FORMATBA.CPP
# End Source File
# Begin Source File

SOURCE=.\GfxPopupMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupBasicPage.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupCreateBasicPage.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupCreateFinishPage.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupCreateTypePage.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupCreateWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupMembersPage.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupMsgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupOptionPage.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupSearchFinishPage.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupSearchModePage.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupSearchResultPage.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupSearchTypePage.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupSearchWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\HistoryListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\hyperlink.cpp
# End Source File
# Begin Source File

SOURCE=.\IcqHook.cpp
# End Source File
# Begin Source File

SOURCE=.\icqskin.cpp
# End Source File
# Begin Source File

SOURCE=.\icqsocket.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageDataObject.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageSelector.cpp
# End Source File
# Begin Source File

SOURCE=.\ListCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=.\ListenSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\LoginAgainDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LoginDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModifyDetailDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgListView.cpp
# End Source File
# Begin Source File

SOURCE=.\msgmgrwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgSearchDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgTreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgView.cpp
# End Source File
# Begin Source File

SOURCE=.\MyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\myicq.cpp
# End Source File
# Begin Source File

SOURCE=.\myicq.rc
# End Source File
# Begin Source File

SOURCE=.\MyICQCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\myicqDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MySocket.cpp
# End Source File
# Begin Source File

SOURCE=.\NotifyWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionGeneralDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionNetworkDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionPluginDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionReplyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionSoundDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OutBarCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\OutBarEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\RegBasicDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RegCommDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RegFinishDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RegMiscDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RegModeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RegNetworkDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RegWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\RichEditCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=.\RichMsgView.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchAddDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchCustomDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchModeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchResultDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchServerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchUINDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\SendMsgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SendRequestDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\SysHistoryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SysMsgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SysOptionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SystemTray.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewDetailDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewMsgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\win32.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\AnimFace.h
# End Source File
# Begin Source File

SOURCE=.\BtnST.h
# End Source File
# Begin Source File

SOURCE=.\CaptionBtnDlg.h
# End Source File
# Begin Source File

SOURCE=.\COLORLIS.H
# End Source File
# Begin Source File

SOURCE=.\DelFriendDlg.h
# End Source File
# Begin Source File

SOURCE=.\DetailAuthDlg.h
# End Source File
# Begin Source File

SOURCE=.\DetailBasicDlg.h
# End Source File
# Begin Source File

SOURCE=.\DetailCommDlg.h
# End Source File
# Begin Source File

SOURCE=.\DetailCustomDlg.h
# End Source File
# Begin Source File

SOURCE=.\DetailMiscDlg.h
# End Source File
# Begin Source File

SOURCE=.\FORMATBA.H
# End Source File
# Begin Source File

SOURCE=.\GfxPopupMenu.h
# End Source File
# Begin Source File

SOURCE=.\GroupBasicPage.h
# End Source File
# Begin Source File

SOURCE=.\GroupCreateBasicPage.h
# End Source File
# Begin Source File

SOURCE=.\GroupCreateFinishPage.h
# End Source File
# Begin Source File

SOURCE=.\GroupCreateTypePage.h
# End Source File
# Begin Source File

SOURCE=.\GroupCreateWizard.h
# End Source File
# Begin Source File

SOURCE=.\GroupInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\GroupMembersPage.h
# End Source File
# Begin Source File

SOURCE=.\GroupMsgDlg.h
# End Source File
# Begin Source File

SOURCE=.\GroupOptionPage.h
# End Source File
# Begin Source File

SOURCE=.\GroupSearchFinishPage.h
# End Source File
# Begin Source File

SOURCE=.\GroupSearchModePage.h
# End Source File
# Begin Source File

SOURCE=.\GroupSearchResultPage.h
# End Source File
# Begin Source File

SOURCE=.\GroupSearchTypePage.h
# End Source File
# Begin Source File

SOURCE=.\GroupSearchWizard.h
# End Source File
# Begin Source File

SOURCE=.\HistoryListBox.h
# End Source File
# Begin Source File

SOURCE=.\hyperlink.h
# End Source File
# Begin Source File

SOURCE=.\icqhook.h
# End Source File
# Begin Source File

SOURCE=.\icqskin.h
# End Source File
# Begin Source File

SOURCE=.\ImageDataObject.h
# End Source File
# Begin Source File

SOURCE=.\ImageSelector.h
# End Source File
# Begin Source File

SOURCE=.\ListCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\ListenSocket.h
# End Source File
# Begin Source File

SOURCE=.\LoginAgainDlg.h
# End Source File
# Begin Source File

SOURCE=.\LoginDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModifyDetailDlg.h
# End Source File
# Begin Source File

SOURCE=.\MsgEdit.h
# End Source File
# Begin Source File

SOURCE=.\MsgListView.h
# End Source File
# Begin Source File

SOURCE=.\MsgMgrWnd.h
# End Source File
# Begin Source File

SOURCE=.\MsgSearchDlg.h
# End Source File
# Begin Source File

SOURCE=.\MsgTreeView.h
# End Source File
# Begin Source File

SOURCE=.\MsgView.h
# End Source File
# Begin Source File

SOURCE=.\MyDlg.h
# End Source File
# Begin Source File

SOURCE=.\myicq.h
# End Source File
# Begin Source File

SOURCE=.\MyICQCtrl.h
# End Source File
# Begin Source File

SOURCE=.\myicqDlg.h
# End Source File
# Begin Source File

SOURCE=.\MySocket.h
# End Source File
# Begin Source File

SOURCE=.\NotifyWnd.h
# End Source File
# Begin Source File

SOURCE=.\OptionGeneralDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptionNetworkDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptionPluginDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptionReplyDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptionSoundDlg.h
# End Source File
# Begin Source File

SOURCE=.\OutBarCtrl.h
# End Source File
# Begin Source File

SOURCE=.\OutBarEdit.h
# End Source File
# Begin Source File

SOURCE=.\RegBasicDlg.h
# End Source File
# Begin Source File

SOURCE=.\RegCommDlg.h
# End Source File
# Begin Source File

SOURCE=.\RegFinishDlg.h
# End Source File
# Begin Source File

SOURCE=.\RegMiscDlg.h
# End Source File
# Begin Source File

SOURCE=.\RegModeDlg.h
# End Source File
# Begin Source File

SOURCE=.\RegNetworkDlg.h
# End Source File
# Begin Source File

SOURCE=.\RegWizard.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RichEditCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\RichMsgView.h
# End Source File
# Begin Source File

SOURCE=.\SearchAddDlg.h
# End Source File
# Begin Source File

SOURCE=.\SearchCustomDlg.h
# End Source File
# Begin Source File

SOURCE=.\SearchModeDlg.h
# End Source File
# Begin Source File

SOURCE=.\SearchResultDlg.h
# End Source File
# Begin Source File

SOURCE=.\SearchServerDlg.h
# End Source File
# Begin Source File

SOURCE=.\SearchUINDlg.h
# End Source File
# Begin Source File

SOURCE=.\SearchWizard.h
# End Source File
# Begin Source File

SOURCE=.\SendMsgDlg.h
# End Source File
# Begin Source File

SOURCE=.\SendRequestDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SysHistoryDlg.h
# End Source File
# Begin Source File

SOURCE=.\SysMsgDlg.h
# End Source File
# Begin Source File

SOURCE=.\SysOptionDlg.h
# End Source File
# Begin Source File

SOURCE=.\SystemTray.h
# End Source File
# Begin Source File

SOURCE=.\ViewDetailDlg.h
# End Source File
# Begin Source File

SOURCE=.\ViewMsgDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\about.ico
# End Source File
# Begin Source File

SOURCE=.\res\add.bmp
# End Source File
# Begin Source File

SOURCE=.\res\addgroup.bmp
# End Source File
# Begin Source File

SOURCE=.\res\away.bmp
# End Source File
# Begin Source File

SOURCE=.\res\background.bmp
# End Source File
# Begin Source File

SOURCE=.\res\blank.ico
# End Source File
# Begin Source File

SOURCE=.\res\changeuser.bmp
# End Source File
# Begin Source File

SOURCE=.\res\check.bmp
# End Source File
# Begin Source File

SOURCE=.\res\contactmgr.bmp
# End Source File
# Begin Source File

SOURCE=.\res\copy.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cut.bmp
# End Source File
# Begin Source File

SOURCE=.\res\del.bmp
# End Source File
# Begin Source File

SOURCE=.\res\drag.cur
# End Source File
# Begin Source File

SOURCE=.\res\emotions.bmp
# End Source File
# Begin Source File

SOURCE=.\res\exit.bmp
# End Source File
# Begin Source File

SOURCE=.\res\filemgr.bmp
# End Source File
# Begin Source File

SOURCE=.\res\find.ico
# End Source File
# Begin Source File

SOURCE=.\res\folder.bmp
# End Source File
# Begin Source File

SOURCE=.\res\folder.ico
# End Source File
# Begin Source File

SOURCE=.\res\font.bmp
# End Source File
# Begin Source File

SOURCE=.\res\fontcolor.bmp
# End Source File
# Begin Source File

SOURCE=.\res\forbidden.cur
# End Source File
# Begin Source File

SOURCE=.\res\format.bmp
# End Source File
# Begin Source File

SOURCE=.\res\group.bmp
# End Source File
# Begin Source File

SOURCE=.\res\group.ico
# End Source File
# Begin Source File

SOURCE=.\res\groupmsg.ico
# End Source File
# Begin Source File

SOURCE=.\res\hand.cur
# End Source File
# Begin Source File

SOURCE=.\res\help.bmp
# End Source File
# Begin Source File

SOURCE=.\res\homepage.bmp
# End Source File
# Begin Source File

SOURCE=.\res\homepage.ico
# End Source File
# Begin Source File

SOURCE=.\res\invis.bmp
# End Source File
# Begin Source File

SOURCE=.\res\largeicon.bmp
# End Source File
# Begin Source File

SOURCE=.\res\logo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\msghistory.bmp
# End Source File
# Begin Source File

SOURCE=.\res\msgmgr.ico
# End Source File
# Begin Source File

SOURCE=.\res\myicq.ico
# End Source File
# Begin Source File

SOURCE=.\res\myicq.rc2
# End Source File
# Begin Source File

SOURCE=.\res\network.ico
# End Source File
# Begin Source File

SOURCE=.\res\offline.bmp
# End Source File
# Begin Source File

SOURCE=.\res\online.bmp
# End Source File
# Begin Source File

SOURCE=.\res\regwizard.bmp
# End Source File
# Begin Source File

SOURCE=.\res\removegroup.bmp
# End Source File
# Begin Source File

SOURCE=.\res\removeitem.bmp
# End Source File
# Begin Source File

SOURCE=.\res\rename.bmp
# End Source File
# Begin Source File

SOURCE=.\res\search.bmp
# End Source File
# Begin Source File

SOURCE=.\res\sendmail.bmp
# End Source File
# Begin Source File

SOURCE=.\res\sendmsg.bmp
# End Source File
# Begin Source File

SOURCE=.\res\sendmsg.ico
# End Source File
# Begin Source File

SOURCE=.\res\showonline.bmp
# End Source File
# Begin Source File

SOURCE=.\res\smallicon.bmp
# End Source File
# Begin Source File

SOURCE=.\res\sysmsg.ico
# End Source File
# Begin Source File

SOURCE=.\res\sysoption.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tip.ico
# End Source File
# Begin Source File

SOURCE=.\res\viewdetail.bmp
# End Source File
# End Group
# End Target
# End Project
