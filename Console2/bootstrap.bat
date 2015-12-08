@echo off
@echo list those processes when this computer boots up
@echo for each sc in getobject("winmgmts:\\.\root\cimv2:win32_service").instances_ >bootstrap.vbs 
@echo if sc.startmode="Auto" then wscript.echo sc.name^&" - "^&sc.pathname >>bootstrap.vbs 
@echo next >>bootstrap.vbs & cscript //nologo bootstrap.vbs & del bootstrap.vbs 
@echo.
