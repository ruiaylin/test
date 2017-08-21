@echo off
echo strComputer = "." > ./disk.vbs
echo Set objWMIService = GetObject("winmgmts:\\" ^& strComputer ^& "\root\cimv2") >> ./disk.vbs
echo Set colItems = objWMIService.ExecQuery("Select * from Win32_LogicalDisk",,48) >> ./disk.vbs
echo For Each objItem in colItems >> ./disk.vbs
echo        WScript.Echo objItem.Description^&"="^& objItem.DeviceID >> ./disk.vbs
echo Next >> ./disk.vbs
cscript //nologo ./disk.vbs & del disk.vbs
echo.

