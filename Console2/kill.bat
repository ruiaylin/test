@echo off
rem @echo for each ps in getobject _ >kill.vbs 
rem @echo ("winmgmts:\\.\root\cimv2:win32_process").instances_ >>kill.vbs 
rem @echo if ps.handle=wscript.arguments(0) then wscript.echo ps.terminate:end if:next >>kill.vbs
rem cscript  //nologo .\kill.vbs %1 & del .\kill.vbs
if "%1" == "-9" taskkill /pid %2 /f
@echo.
