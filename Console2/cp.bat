@echo off
dir /ad %1 >nul 2>nul && (FOR /D %%I IN (%1) DO SET _LAST_SEGMENT_=%%~nxI && md %2\\%_LAST_SEGMENT_% && xcopy /S /E /H /Q %1 %2\\%_LAST_SEGMENT_%) || (copy %1 %2)
