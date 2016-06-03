@ECHO ON
REM Determine script location for Windows Batch File

REM Get current folder with no trailing slash
SET ScriptDir=%~dp0
SET ScriptDir=%ScriptDir:~0,-1%

SET GOPATH=%ScriptDir%;%GOPATH%
ECHO %GOPATH%
DEL /q /f .\\gopath.bat