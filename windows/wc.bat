@echo off&setlocal enabledelayedexpansion
set /a line_num=0
for /f "delims=" %%a in (%1) do (
set /a line_num+=1
)
echo line number: rem !line_num!
