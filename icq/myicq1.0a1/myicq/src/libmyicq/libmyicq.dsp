# Microsoft Developer Studio Project File - Name="libmyicq" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libmyicq - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libmyicq.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libmyicq.mak" CFG="libmyicq - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libmyicq - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libmyicq - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libmyicq - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "include/" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libmyicq - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "include/" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libmyicq - Win32 Release"
# Name "libmyicq - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\base64.c
# End Source File
# Begin Source File

SOURCE=.\des.c
# End Source File
# Begin Source File

SOURCE=.\groupplugin.cpp
# End Source File
# Begin Source File

SOURCE=.\httpproxy.cpp
# End Source File
# Begin Source File

SOURCE=.\icqclient.cpp
# End Source File
# Begin Source File

SOURCE=.\icqconfig.cpp
# End Source File
# Begin Source File

SOURCE=.\icqdb.cpp
# End Source File
# Begin Source File

SOURCE=.\icqgroup.cpp
# End Source File
# Begin Source File

SOURCE=.\icqlink.cpp
# End Source File
# Begin Source File

SOURCE=.\icqpacket.cpp
# End Source File
# Begin Source File

SOURCE=.\icqplugin.cpp
# End Source File
# Begin Source File

SOURCE=.\icqprofile.cpp
# End Source File
# Begin Source File

SOURCE=.\icqwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\msgsession.cpp
# End Source File
# Begin Source File

SOURCE=.\proxy.cpp
# End Source File
# Begin Source File

SOURCE=.\qid.cpp
# End Source File
# Begin Source File

SOURCE=.\socksproxy.cpp
# End Source File
# Begin Source File

SOURCE=.\tcppacket.cpp
# End Source File
# Begin Source File

SOURCE=.\tcpsession.cpp
# End Source File
# Begin Source File

SOURCE=.\udppacket.cpp
# End Source File
# Begin Source File

SOURCE=.\udpsession.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\base64.h
# End Source File
# Begin Source File

SOURCE=.\include\contactinfo.h
# End Source File
# Begin Source File

SOURCE=.\include\group.h
# End Source File
# Begin Source File

SOURCE=.\groupplugin.h
# End Source File
# Begin Source File

SOURCE=.\include\groupsession.h
# End Source File
# Begin Source File

SOURCE=.\httpproxy.h
# End Source File
# Begin Source File

SOURCE=.\icqclient.h
# End Source File
# Begin Source File

SOURCE=.\icqconfig.h
# End Source File
# Begin Source File

SOURCE=.\icqdb.h
# End Source File
# Begin Source File

SOURCE=.\icqgroup.h
# End Source File
# Begin Source File

SOURCE=.\icqlink.h
# End Source File
# Begin Source File

SOURCE=.\include\icqlinkbase.h
# End Source File
# Begin Source File

SOURCE=.\icqpacket.h
# End Source File
# Begin Source File

SOURCE=.\icqplugin.h
# End Source File
# Begin Source File

SOURCE=.\icqprofile.h
# End Source File
# Begin Source File

SOURCE=.\icqsocket.h
# End Source File
# Begin Source File

SOURCE=.\include\icqtypes.h
# End Source File
# Begin Source File

SOURCE=.\icqwindow.h
# End Source File
# Begin Source File

SOURCE=.\msgsession.h
# End Source File
# Begin Source File

SOURCE=.\ndes.h
# End Source File
# Begin Source File

SOURCE=.\include\packet.h
# End Source File
# Begin Source File

SOURCE=.\include\plugin.h
# End Source File
# Begin Source File

SOURCE=.\include\profile.h
# End Source File
# Begin Source File

SOURCE=.\proxy.h
# End Source File
# Begin Source File

SOURCE=.\socksproxy.h
# End Source File
# Begin Source File

SOURCE=.\tcppacket.h
# End Source File
# Begin Source File

SOURCE=.\tcpsession.h
# End Source File
# Begin Source File

SOURCE=.\include\tcpsessionbase.h
# End Source File
# Begin Source File

SOURCE=.\udppacket.h
# End Source File
# Begin Source File

SOURCE=.\udpsession.h
# End Source File
# End Group
# End Target
# End Project
