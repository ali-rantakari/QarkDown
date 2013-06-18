;; InnoSetup config file for QarkDown installer

[Setup]
AppName=QarkDown
AppVersion={#appversion}
DefaultDirName={pf}\QarkDown
DefaultGroupName=QarkDown
LicenseFile=LICENSE.md

[Files]
Source: "qarkdown-windows-installerfiles\qarkdown.exe"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\libstdc++-6.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\libgcc_s_sjlj-1.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\libEGL.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\libGLESv2.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\icudt49.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\icuin49.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\icuuc49.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\libwinpthread-1.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\Qt5Core.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\Qt5Gui.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\Qt5Widgets.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\Qt5Network.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\Qt5WebKit.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\Qt5Quick.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\Qt5Qml.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\Qt5OpenGL.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\Qt5PrintSupport.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\Qt5Sql.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\Qt5V8.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\D3DCompiler_43.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\qwindows.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\qminimal.dll"; DestDir: "{app}"
Source: "README.md"; DestDir: "{app}"; DestName: "README.txt"; Flags: isreadme
Source: "LICENSE.md"; DestDir: "{app}"; DestName: "LICENSE.txt";

[Icons]
Name: "{group}\QarkDown"; Filename: "{app}\qarkdown.exe"; WorkingDir: "{app}"
Name: "{group}\Uninstall QarkDown"; Filename: "{uninstallexe}"
