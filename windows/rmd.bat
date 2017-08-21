@echo off
for /f "delims=" %%i in ('dir /ad /s /b .\*%1*') do (
	@if exist "%%i" (echo "del %%i" && rd /s/q "%%i")
)
@echo.

