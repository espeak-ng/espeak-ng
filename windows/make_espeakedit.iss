
[Setup]
AppName=eSpeakEdit
AppVerName=eSpeakEdit version 1.26
DefaultDirName={pf}\eSpeak
DefaultGroupName=eSpeak
OutputBaseFilename=setup_espeakedit
Compression=lzma
SolidCompression=yes
DirExistsWarning=no

[Files]
Source: "espeakedit.exe"; DestDir: "{app}"
Source: "dictsource\*"; DestDir: "{app}\dictsource"; Flags: recursesubdirs
Source: "espeakedit\*"; DestDir: "{app}\espeakedit"; Flags: recursesubdirs
Source: "phsource\*"; DestDir: "{app}\phsource"; Flags: recursesubdirs
Source: "docs\*"; DestDir: "{app}\docs"; Flags: recursesubdirs
;Source: "Readme.txt"; DestDir: "{app}"; Flags: isreadme



