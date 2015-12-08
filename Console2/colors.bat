@echo off
echo Please input a key to choose the color for this window. 
echo Those available options are as follows. 
echo		t, tradition (black and white); 
echo		d, dos (blue and white); 
echo		e, eye (yellow and black); 
echo		m, matrix (light green and black);
echo exit|cmd/kprompt $_e 100 CD 16 B4 4C CD 21$_g$_|debug>nul
set "mode=%errorlevel%"
if %mode% == 116 goto tradition 
if %mode% == 100 goto dos
if %mode% == 101 goto eye
if %mode% == 109 goto matrix
rem the default mode is tradition
:tradition
color 0f
goto last
:dos
color 1f
goto last
:eye
color 60
goto last
:matrix
color a0
goto last
:last
cls
