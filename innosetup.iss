;; InnoSetup config file for QarkDown installer

[Setup]
AppName=QarkDown
AppVersion={#appversion}
DefaultDirName={pf}\QarkDown
DefaultGroupName=QarkDown
LicenseFile=LICENSE.md

[Files]
Source: "qarkdown-windows-installerfiles\qarkdown.exe"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\mingwm10.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\libgcc_s_dw2-1.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\QtCore4.dll"; DestDir: "{app}"
Source: "qarkdown-windows-installerfiles\QtGui4.dll"; DestDir: "{app}"
Source: "README.md"; DestDir: "{app}"; DestName: "README.txt"; Flags: isreadme
Source: "LICENSE.md"; DestDir: "{app}"; DestName: "LICENSE.txt";

[Icons]
Name: "{group}\QarkDown"; Filename: "{app}\qarkdown.exe"; WorkingDir: "{app}"
Name: "{group}\Uninstall QarkDown"; Filename: "{uninstallexe}"
