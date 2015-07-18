# Microsoft Developer Studio Project File - Name="myicqd" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=myicqd - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "myicqd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "myicqd.mak" CFG="myicqd - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "myicqd - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "myicqd - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "myicqd - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".." /I "../include" /I "../des/" /I "../getopt" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 ws2_32.lib libmysql.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"../../myicqd.exe" /libpath:"d:\mysql\lib\opt"

!ELSEIF  "$(CFG)" == "myicqd - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".." /I "../include" /I "../des/" /I "../getopt" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib mysqlclient.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../myicqd.exe" /pdbtype:sept /libpath:"d:\mysql\lib\debug"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "myicqd - Win32 Release"
# Name "myicqd - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\dbmanager.cpp
# End Source File
# Begin Source File

SOURCE=..\des\des.c
# End Source File
# Begin Source File

SOURCE=..\dnsmanager.cpp
# End Source File
# Begin Source File

SOURCE=..\getopt\getopt.c
# End Source File
# Begin Source File

SOURCE=..\groupplugin.cpp
# End Source File
# Begin Source File

SOURCE=..\icqgroup.cpp
# End Source File
# Begin Source File

SOURCE=..\icqpacket.cpp
# End Source File
# Begin Source File

SOURCE=..\list.cpp
# End Source File
# Begin Source File

SOURCE=..\log.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=..\myicq.cpp
# End Source File
# Begin Source File

SOURCE=..\profile.cpp
# End Source File
# Begin Source File

SOURCE=..\queue.cpp
# End Source File
# Begin Source File

SOURCE=..\server.cpp
# End Source File
# Begin Source File

SOURCE=.\service.cpp
# End Source File
# Begin Source File

SOURCE=..\sessionhash.cpp
# End Source File
# Begin Source File

SOURCE=..\slab.cpp
# End Source File
# Begin Source File

SOURCE=..\tcppacket.cpp
# End Source File
# Begin Source File

SOURCE=..\udppacket.cpp
# End Source File
# Begin Source File

SOURCE=..\udpsession.cpp
# End Source File
# Begin Source File

SOURCE=..\utils.cpp
# End Source File
# Begin Source File

SOURCE=.\win32.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\dbmanager.h
# End Source File
# Begin Source File

SOURCE=..\dnsmanager.h
# End Source File
# Begin Source File

SOURCE=..\getopt\getopt.h
# End Source File
# Begin Source File

SOURCE=..\include\group.h
# End Source File
# Begin Source File

SOURCE=..\groupplugin.h
# End Source File
# Begin Source File

SOURCE=..\include\groupsession.h
# End Source File
# Begin Source File

SOURCE=..\icqgroup.h
# End Source File
# Begin Source File

SOURCE=..\icqpacket.h
# End Source File
# Begin Source File

SOURCE=..\icqsocket.h
# End Source File
# Begin Source File

SOURCE=..\include\icqtypes.h
# End Source File
# Begin Source File

SOURCE=..\list.h
# End Source File
# Begin Source File

SOURCE=..\log.h
# End Source File
# Begin Source File

SOURCE=..\myicq.h
# End Source File
# Begin Source File

SOURCE=..\des\NDES.H
# End Source File
# Begin Source File

SOURCE=..\include\packet.h
# End Source File
# Begin Source File

SOURCE=..\profile.h
# End Source File
# Begin Source File

SOURCE=..\queue.h
# End Source File
# Begin Source File

SOURCE=..\refobject.h
# End Source File
# Begin Source File

SOURCE=..\server.h
# End Source File
# Begin Source File

SOURCE=.\service.h
# End Source File
# Begin Source File

SOURCE=..\session.h
# End Source File
# Begin Source File

SOURCE=..\sessionhash.h
# End Source File
# Begin Source File

SOURCE=..\slab.h
# End Source File
# Begin Source File

SOURCE=..\sync.h
# End Source File
# Begin Source File

SOURCE=..\tcppacket.h
# End Source File
# Begin Source File

SOURCE=..\udppacket.h
# End Source File
# Begin Source File

SOURCE=..\udpsession.h
# End Source File
# Begin Source File

SOURCE=..\utils.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\myicq.sql
# End Source File
# Begin Source File

SOURCE=..\..\myicqd.cnf
# End Source File
# End Target
# End Project
