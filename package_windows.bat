echo off
echo Remember to build a release binary from Qt Creator first.
pause

SET packagedir=qarkdown-windows
SET zipfile=qarkdown-windows.zip
SET default_qtversion=4.8.0

echo Type in the Qt version to use, or just press enter to use the default (%default_qtversion%)
SET /P qtversion=[Qt version:]
IF [%qtversion%]==[] SET qtversion=%default_qtversion%

echo Creating package directory: %packagedir%
mkdir %packagedir%

call :COPYTOPACKAGE C:\QtSDK\mingw\bin\mingwm10.dll
call :COPYTOPACKAGE C:\QtSDK\mingw\bin\libgcc_s_dw2-1.dll
call :COPYTOPACKAGE C:\QtSDK\Desktop\Qt\%qtversion%\mingw\bin\QtCore4.dll
call :COPYTOPACKAGE C:\QtSDK\Desktop\Qt\%qtversion%\mingw\bin\QtGui4.dll
call :COPYTOPACKAGE qarkdown-build-desktop-Qt_4_8_0_RC_for_Desktop_-_MinGW__Qt_SDK__Release\release\qarkdown.exe
call :COPYTOPACKAGE README.md
call :COPYTOPACKAGE LICENSE.md

IF EXIST %zipfile% (
echo Deleting existing zip file: %zipfile%
del %zipfile%
)

echo Zipping up the package to: %zipfile%
tools\7z.exe a %zipfile% %packagedir%

echo Removing package directory: %packagedir%
rmdir /S /Q %packagedir%

echo DONE.
pause
goto :EOF


Rem Function COPYTOPACKAGE
:COPYTOPACKAGE
echo Copying: %*
copy %* %packagedir%\.
goto :EOF