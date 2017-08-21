@echo off
if "%1" == "-a" goto sysinfo else goto wininfo
:wininfo
@echo for each uname in getobject _ >uname.vbs 
@echo ("winmgmts:\\.\root\cimv2:win32_operatingsystem").instances_ >>uname.vbs 
@echo wscript.echo uname.caption^&" "^&uname.version:next >>uname.vbs 
cscript //nologo ./uname.vbs & del uname.vbs 
goto end
:sysinfo
.\sysinfo.bat
:end