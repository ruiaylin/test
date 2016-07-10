@echo off
call :script > "%temp%\%~n0.js" && cscript //nologo "%temp%\%~n0.js" %*
goto :EOF

:script
echo var specialFolders = WScript.CreateObject('WScript.Shell').SpecialFolders;
echo if (WScript.Arguments.length === 0) {
echo     for (var e = new Enumerator(specialFolders); !e.atEnd(); e.moveNext()) {
echo         WScript.Echo(e.item());
echo     }
echo } else {
echo     for (var e = new Enumerator(WScript.Arguments); !e.atEnd(); e.moveNext()) {
echo         WScript.Echo(specialFolders(e.item()));
echo     }
echo }
goto :EOF
