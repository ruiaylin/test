@echo off&&color 1f&chcp 437 >nul&graftabl 936 >nul
set "str=MainBoard: BIOS: CPU: Monitor: HardDisk: Pardition: LAN_card: Printer: Sound_Card: Memory: Graphic_card:"
set "code=abcdefghijklmnopqrstuvwxyz"&title %str%

:start
rem cls
setlocal ENABLEDELAYEDEXPANSION
set "var=!str::= !"&set /a mm=96
for %%a in (!var!) do (
   set /a mm+=1
   set ".!mm!=%%a"
   echo  !code:~0,1!.%%a&echo.
   set "code=!code:~1!"
)
echo  Please input a key to check your sys: 1, search all; 2, exit
echo exit|cmd/kprompt $_e 100 CD 16 B4 4C CD 21$_g$_|debug>nul
set "choice=%errorlevel%"&set "exit="&cls
if !choice! equ 49 goto MainBoard
if !choice! equ 50 goto end
set "exit=endlocal&echo.&echo preass any key to continue&pause>null&goto start"
if not defined .!choice! endlocal&goto start
goto !.%choice%!

:MainBoard
cls&echo MainBoard:
for /f "tokens=1* delims==" %%a in (
'wmic BASEBOARD get Manufacturer^,Product^,Version^,SerialNumber /value'
) do (
     set /a tee+=1
     if "!tee!" == "3" echo       Manufacturer		= %%b
     if "!tee!" == "4" echo       Model			= %%b
     if "!tee!" == "5" echo       Serial No.   		= %%b
     if "!tee!" == "6" echo       Version		   	= %%b
)
%exit%

:BIOS
set /a tee=0&echo.&echo BIOS:
for /f "tokens=1* delims==" %%a in (
'wmic bios  get  CurrentLanguage^,Manufacturer^,SMBIOSBIOSVersion^,SMBIOSMajorVersion^,SMBIOSMinorVersion^,ReleaseDate /value'
) do (
     set /a tee+=1
     if "!tee!" == "3" echo       Language 			= %%b
     if "!tee!" == "4" echo       Manufacturer 		= %%b
     if "!tee!" == "5" echo       Delivery Date 		= %%b
     if "!tee!" == "6" echo       Version 			= %%b
     if "!tee!" == "7" echo       SMBIOSMajorVersion	= %%b
     if "!tee!" == "8" echo       SMBIOSMinorVersion	= %%b 
)
%exit%

:CPU
set /a tee=0&echo.&echo CPU:
for /f "tokens=1* delims==" %%a in (
'wmic cpu get name^,ExtClock^,CpuStatus^,Description /value'
) do (
     set /a tee+=1
     if "!tee!" == "3" echo       CPU No.   		= %%b
     if "!tee!" == "4" echo       Processor Ver.   		= %%b
     if "!tee!" == "5" echo       External Frequency  	= %%b
     if "!tee!" == "6" echo       Name and Basic Frequency 	= %%b
)
%exit%

:Monitor
set /a tee=0&echo.&echo Monitor:
for /f "tokens=1* delims==" %%a in (
'wmic DESKTOPMONITOR  get name^,ScreenWidth^,ScreenHeight^,PNPDeviceID /value'
) do (
     set /a tee+=1
     if "!tee!" == "3" echo       Type		= %%b
     if "!tee!" == "4" echo       Other Info	= %%b
     if "!tee!" == "5" echo       Screen Height	= %%b
     if "!tee!" == "6" echo       Screen Width	= %%b
)
%exit%

:HardDisk
set /a tee=0&echo.&echo Hard Dist:
for /f "tokens=1* delims==" %%a in (
'wmic DISKDRIVE get model^,interfacetype^,size^,totalsectors^,partitions /value'
) do (
     set /a tee+=1
     if "!tee!" == "3" echo       Interface Type	= %%b
     if "!tee!" == "4" echo       HardDisk Modes	= %%b
     if "!tee!" == "5" echo       Pardition No.	= %%b
     if "!tee!" == "6" echo       Disk Capacity	= %%b
     if "!tee!" == "7" echo       All Sector NO.	= %%b
)
%exit%

:Pardition
echo.&echo Pardition:
for /f "delims=" %%a in (
'wmic LOGICALDISK where "mediatype='12'" get description^,deviceid^,filesystem^,size^,freespace'
) do echo       %%a
%exit%

:LAN_card
set /a tee=0&echo.&echo LAN_card:
for /f "tokens=1* delims==" %%a in (
'wmic NICCONFIG where "index='1'" get ipaddress^,macaddress^,description /value'
) do (
     set /a tee+=1
     if "!tee!" == "3" echo       LAN_card  Type	= %%b
     if "!tee!" == "4" echo       LAN_card  IP	= %%b
     if "!tee!" == "5" echo       LAN_card MAC	= %%b
)
%exit%

:Printer
set /a tee=0&echo.&echo Printer:
for /f "tokens=1* delims==" %%a in ('wmic PRINTER get caption /value') do (
     set /a tee+=1
     if "!tee!" == "3" echo       Printer Name	= %%b
)
%exit%

:Sound_Card
set /a tee=0&echo.&echo Sound_Card:
for /f "tokens=1* delims==" %%a in ('wmic SOUNDDEV get name^,deviceid /value') do (
     set /a tee+=1
     if "!tee!" == "3" echo       Other Info	= %%b
     if "!tee!" == "4" echo       Model		= %%b
)
%exit%

:Memory
set /a tee=0&echo.&echo Memory: 
for /f "delims=" %%i in ('systeminfo 2^>nul^|findstr "ÄÚ´æ"')do echo	%%i
%exit%

:Graphic_card
echo.&echo Graphic_card2:
for /l %%a in (1 1 20) do set tg=    !tg!&set "ko=    !ko!"
set /p=      I need about 30 seconds ......%tg%<nul
del /f "%TEMP%\temp.txt" 2>nul
dxdiag /t %TEMP%\temp.txt
:Graphic_card2  I need about 30 seconds!
if EXIST "%TEMP%\temp.txt" (
set /p=!ko!!tg!<nul
for /f "tokens=1,2,* delims=:" %%a in (
'findstr /c:" Card name:" /c:"Display Memory:" /c:"Current Mode:" "%TEMP%\temp.txt"'
) do (
        set /a tee+=1
        if !tee! == 1 echo	Graphic_card Model:	%%b
        if !tee! == 2 echo       Graphic_card Memory Capacity:	%%b
        if !tee! == 3 echo       Cur Settings:		%%b
      )) else (
        ping /n 2 127.1>nul
        goto Graphic_card2
)
if !choice! equ 49 set "exit=echo Press any key to continue...&pause>null"
echo.
%exit%
del /f "%TEMP%\temp.txt"
:end
cls
window.bat
del null&@echo.
