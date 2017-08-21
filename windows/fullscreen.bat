@echo off
setlocal ENABLEDELAYEDEXPANSION
set /a tee=0
for /f "tokens=1* delims==" %%a in (
'wmic DESKTOPMONITOR  get ScreenWidth^,ScreenHeight^ /value'
) do (
     set /a tee+=1
	 if "!tee!" == "3" set screenheight=%%b
	 if "!tee!" == "4" set screenwidth=%%b
)
set /a screenheight-=100
set /a screenwidth-=100
rem echo %screenheight%
rem echo %screenwidth%
mode con lines=%screenheight%
mode con cols=%screenwidth%
color 60
cls
@echo.
