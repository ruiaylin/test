@echo off&&color 1f&chcp 437 >nul&graftabl 936 >nul
:: code 伤脑筋 硬件检测  @bbs.verybat.cn
:: sc config  winmgmt start= auto >nul 2<&1
:: net start winmgmt 2>nul
set "str=主版: BIOS: CPU: 显示器: 硬盘: 分区信息: 网卡: 打印机: 声卡: 内存: 显卡:"
set "code=abcdefghijklmnopqrstuvwxyz"&title %str%

:start
rem cls&setlocal ENABLEDELAYEDEXPANSION
setlocal ENABLEDELAYEDEXPANSION
set "var=!str::= !"&set /a mm=96
for %%a in (!var!) do (
   set /a mm+=1
   set ".!mm!=%%a"
   echo  !code:~0,1!.%%a&echo.
   set "code=!code:~1!"
)
echo  请输入相应的序号选择需查询的内容、按 1 全部查询、按 2 退出
echo exit|cmd/kprompt $_e 100 CD 16 B4 4C CD 21$_g$_|debug>nul
set "xuanz=%errorlevel%"&set "exit="&cls
if !xuanz! equ 49 goto 主版
if !xuanz! equ 50 pause
set "exit=endlocal&echo.&pause&goto start"
if not defined .!xuanz! endlocal&goto start
goto !.%xuanz%!

:主版
cls&echo 主版:
for /f "tokens=1* delims==" %%a in (
'wmic BASEBOARD get Manufacturer^,Product^,Version^,SerialNumber /value'
) do (
     set /a tee+=1
     if "!tee!" == "3" echo       制造商   = %%b
     if "!tee!" == "4" echo       型  号   = %%b
     if "!tee!" == "5" echo       序列号   = %%b
     if "!tee!" == "6" echo       版  本   = %%b
)
%exit%

:BIOS
set /a tee=0&echo.&echo BIOS:
for /f "tokens=1* delims==" %%a in (
'wmic bios  get  CurrentLanguage^,Manufacturer^,SMBIOSBIOSVersion^,SMBIOSMajorVersion^,SMBIOSMinorVersion^,ReleaseDate /value'
) do (
     set /a tee+=1
     if "!tee!" == "3" echo       当前语言 = %%b
     if "!tee!" == "4" echo       制 造 商 = %%b
     if "!tee!" == "5" echo       发行日期 = %%b
     if "!tee!" == "6" echo       版    本 = %%b
     if "!tee!" == "7" echo       SMBIOSMajorVersion = %%b
     if "!tee!" == "8" echo       SMBIOSMinorVersion = %%b 
)
%exit%

:CPU
set /a tee=0&echo.&echo CPU:
for /f "tokens=1* delims==" %%a in (
'wmic cpu get name^,ExtClock^,CpuStatus^,Description /value'
) do (
     set /a tee+=1
     if "!tee!" == "3" echo       CPU  个 数   = %%b
     if "!tee!" == "4" echo       处理器版本   = %%b
     if "!tee!" == "5" echo       外      频   = %%b
     if "!tee!" == "6" echo       名称及主频率 = %%b
)
%exit%

:显示器
set /a tee=0&echo.&echo 显示器:
for /f "tokens=1* delims==" %%a in (
'wmic DESKTOPMONITOR  get name^,ScreenWidth^,ScreenHeight^,PNPDeviceID /value'
) do (
     set /a tee+=1
     if "!tee!" == "3" echo       类    型  = %%b
     if "!tee!" == "4" echo       其他信息  = %%b
     if "!tee!" == "5" echo       屏 幕 高  = %%b
     if "!tee!" == "6" echo       屏 幕 宽  = %%b
)
%exit%

:硬盘
set /a tee=0&echo.&echo 硬  盘:
for /f "tokens=1* delims==" %%a in (
'wmic DISKDRIVE get model^,interfacetype^,size^,totalsectors^,partitions /value'
) do (
     set /a tee+=1
     if "!tee!" == "3" echo       接口类型  = %%b
     if "!tee!" == "4" echo       硬盘型号  = %%b
     if "!tee!" == "5" echo       分 区 数  = %%b
     if "!tee!" == "6" echo       容    量  = %%b
     if "!tee!" == "7" echo       总 扇 区  = %%b
)
%exit%

:分区信息
echo.&echo 分区信息:
for /f "delims=" %%a in (
'wmic LOGICALDISK where "mediatype='12'" get description^,deviceid^,filesystem^,size^,freespace'
) do echo       %%a
%exit%

:网卡
set /a tee=0&echo.&echo 网  卡:
for /f "tokens=1* delims==" %%a in (
'wmic NICCONFIG where "index='1'" get ipaddress^,macaddress^,description /value'
) do (
     set /a tee+=1
     if "!tee!" == "3" echo       网卡类型  = %%b
     if "!tee!" == "4" echo       网卡  IP  = %%b
     if "!tee!" == "5" echo       网卡 MAC  = %%b
)
%exit%

:打印机
set /a tee=0&echo.&echo 打印机:
for /f "tokens=1* delims==" %%a in ('wmic PRINTER get caption /value') do (
     set /a tee+=1
     if "!tee!" == "3" echo       打印机名字  = %%b
)
%exit%

:声卡
set /a tee=0&echo.&echo 声 卡:
for /f "tokens=1* delims==" %%a in ('wmic SOUNDDEV get name^,deviceid /value') do (
     set /a tee+=1
     if "!tee!" == "3" echo       其他信息  = %%b
     if "!tee!" == "4" echo       型    号  = %%b
)
%exit%

:内存
set /a tee=0&echo.&echo 内  存: 
for /f "delims=" %%i in ('systeminfo 2^>nul^|findstr "内存"')do echo       %%i
%exit%

:显卡
echo.&echo 显  卡:
for /l %%a in (1 1 20) do set tg=    !tg!&set "ko=    !ko!"
set /p=      这里需要 30 秒左右 ......%tg%<nul
del /f "%TEMP%\temp.txt" 2>nul
dxdiag /t %TEMP%\temp.txt
:显卡2  这里需要30秒左右!
if EXIST "%TEMP%\temp.txt" (
set /p=!ko!!tg!<nul
for /f "tokens=1,2,* delims=:" %%a in (
'findstr /c:" Card name:" /c:"Display Memory:" /c:"Current Mode:" "%TEMP%\temp.txt"'
) do (
        set /a tee+=1
        if !tee! == 1 echo       显卡型号: %%b
        if !tee! == 2 echo       显存大小: %%b
        if !tee! == 3 echo       当前设置: %%b
      )) else (
        ping /n 2 127.1>nul
        goto 显卡2
)
if !xuanz! equ 49 set "exit=pause"
echo.
%exit%
del /f "%TEMP%\temp.txt" 2>nul
