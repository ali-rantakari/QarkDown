echo off
echo Remember to build a release binary from Qt Creator first.
pause

python get_version.py > version-temp.txt
SET /P appversion= < version-temp.txt
del version-temp.txt

SET packagedir=qarkdown-windows-installerfiles
SET default_qtversion=5.1.1
SET innosetup="C:\Program Files\Inno Setup 5\ISCC.exe"
SET installerfile=QarkDown-Installer-%appversion%
SET zipfile=QarkDown-Windows-%appversion%.zip

echo Type in the Qt version to use, or just press enter to use the default (%default_qtversion%)
SET /P qtversion=[Qt version:]
IF [%qtversion%]==[] SET qtversion=%default_qtversion%

SET qtdir=C:\Qt\Qt%qtversion%\%qtversion%\mingw47_32

echo ----- Creating package directory: %packagedir%
mkdir %packagedir%

call :COPYTOPACKAGE "%qtdir%\bin\libgcc_s_sjlj-1.dll"
call :COPYTOPACKAGE "%qtdir%\bin\libstdc++-6.dll"
call :COPYTOPACKAGE "%qtdir%\bin\libEGL.dll"
call :COPYTOPACKAGE "%qtdir%\bin\libGLESv2.dll"
call :COPYTOPACKAGE "%qtdir%\bin\icudt49.dll"
call :COPYTOPACKAGE "%qtdir%\bin\icuin49.dll"
call :COPYTOPACKAGE "%qtdir%\bin\icuuc49.dll"
call :COPYTOPACKAGE "%qtdir%\bin\libwinpthread-1.dll"
call :COPYTOPACKAGE %qtdir%\bin\Qt5Core.dll
call :COPYTOPACKAGE %qtdir%\bin\Qt5Gui.dll
call :COPYTOPACKAGE %qtdir%\bin\Qt5Widgets.dll
call :COPYTOPACKAGE %qtdir%\bin\Qt5Network.dll
call :COPYTOPACKAGE %qtdir%\bin\Qt5WebKit.dll
call :COPYTOPACKAGE %qtdir%\bin\Qt5Quick.dll
call :COPYTOPACKAGE %qtdir%\bin\Qt5Qml.dll
call :COPYTOPACKAGE %qtdir%\bin\Qt5OpenGL.dll
call :COPYTOPACKAGE %qtdir%\bin\Qt5PrintSupport.dll
call :COPYTOPACKAGE %qtdir%\bin\Qt5Sql.dll
call :COPYTOPACKAGE %qtdir%\bin\Qt5V8.dll
call :COPYTOPACKAGE %qtdir%\bin\Qt5Sensors.dll
call :COPYTOPACKAGE %qtdir%\bin\D3DCompiler_43.dll
call :COPYTOPACKAGE %qtdir%\plugins\platforms\qwindows.dll
call :COPYTOPACKAGE %qtdir%\plugins\platforms\qminimal.dll
call :COPYTOPACKAGE qarkdown-build-Desktop_Qt_5_0_1_MinGW_32bit-Release\release\qarkdown.exe
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