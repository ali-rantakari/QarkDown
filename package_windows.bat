echo off
echo Remember to build a release binary from Qt Creator first.
pause

python get_version.py > version-temp.txt
SET /P appversion= < version-temp.txt
del version-temp.txt

SET packagedir=qarkdown-windows-installerfiles
SET default_qtversion=4.8.0
SET innosetup="C:\Program Files\Inno Setup 5\ISCC.exe"
SET installerfile=QarkDown-Installer-%appversion%
SET zipfile=QarkDown-Windows-%appversion%.zip

echo Type in the Qt version to use, or just press enter to use the default (%default_qtversion%)
SET /P qtversion=[Qt version:]
IF [%qtversion%]==[] SET qtversion=%default_qtversion%

echo ----- Creating package directory: %packagedir%
mkdir %packagedir%

call :COPYTOPACKAGE C:\QtSDK\mingw\bin\mingwm10.dll
call :COPYTOPACKAGE C:\QtSDK\mingw\bin\libgcc_s_dw2-1.dll
call :COPYTOPACKAGE C:\QtSDK\Desktop\Qt\%qtversion%\mingw\bin\QtCore4.dll
call :COPYTOPACKAGE C:\QtSDK\Desktop\Qt\%qtversion%\mingw\bin\QtGui4.dll
call :COPYTOPACKAGE qarkdown-build-desktop-Qt_4_8_0_RC_for_Desktop_-_MinGW__Qt_SDK__Release\release\qarkdown.exe
call :COPYTOPACKAGE README.md
call :COPYTOPACKAGE LICENSE.md

echo ----- Creating installer via Inno Setup
%innosetup% innosetup.iss /o. /f%installerfile% /dappversion=%appversion%

IF EXIST %zipfile% (
echo ----- Deleting existing zip file: %zipfile%
del %zipfile%
)

echo ----- Zipping up the package to: %zipfile%
tools\7z.exe a %zipfile% %installerfile%.exe README.md LICENSE.md

echo ----- Removing package directory: %packagedir%
rmdir /S /Q %packagedir%

echo ----- Removing installer exe
del %installerfile%.exe

echo DONE.
pause
goto :EOF


Rem Function COPYTOPACKAGE
:COPYTOPACKAGE
echo Copying: %*
copy %* %packagedir%\.
goto :EOF