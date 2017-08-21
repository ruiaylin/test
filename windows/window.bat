@echo off
title AlexStocks CMD
echo Please input a key to choose the display mode. 
echo Those available options are as follows.
echo 	f, fullscreen;  
echo		t, tradition; 
echo		d, dos; 
echo		e, eye; 
echo		m, matrix;
echo exit|cmd/kprompt $_e 100 CD 16 B4 4C CD 21$_g$_|debug>nul
set "mode=%errorlevel%"
if %mode% == 102 goto fullscreen
if %mode% == 116 goto tradition 
if %mode% == 100 goto dos
if %mode% == 101 goto eye
if %mode% == 109 goto matrix
rem the default mode is tradition
:tradition
mode con lines=36
mode con cols=145
color 0f
goto last
:fullscreen
fullscreen.bat
goto last
:dos
mode con lines=36
mode con cols=145
color 1f
goto last
:eye
mode con lines=36
mode con cols=145
color 60
goto last
:matrix
mode con lines=36
mode con cols=145
color a0
goto last
:last
cls
