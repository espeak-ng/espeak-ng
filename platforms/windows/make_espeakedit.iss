
[Setup]
AppName=eSpeakEdit
AppVerName=eSpeakEdit version 1.27
DefaultDirName={pf}\eSpeak
DefaultGroupName=eSpeak
OutputBaseFilename=setup_espeakedit
Compression=lzma
SolidCompression=yes
DirExistsWarning=no
ShowLanguageDialog=auto

[Files]
Source: "espeakedit.exe"; DestDir: "{app}"
Source: "dictsource\*"; DestDir: "{app}\dictsource"; Flags: recursesubdirs
Source: "espeakedit\*"; DestDir: "{app}\espeakedit"; Flags: recursesubdirs
Source: "phsource\*"; DestDir: "{app}\phsource"; Flags: recursesubdirs
Source: "docs\*"; DestDir: "{app}\docs"; Flags: recursesubdirs
;Source: "Readme.txt"; DestDir: "{app}"; Flags: isreadme


[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "af"; MessagesFile: "compiler:Languages\Afrikaans.isl"
Name: "cs"; MessagesFile: "compiler:Languages\Czech.isl"
Name: "de"; MessagesFile: "compiler:Languages\German.isl"
Name: "el"; MessagesFile: "compiler:Languages\Greek.isl"
Name: "es"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "fi"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "fr"; MessagesFile: "compiler:Languages\French.isl"
Name: "hr"; MessagesFile: "compiler:Languages\Croatian.isl"
Name: "hu"; MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "it"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "nl"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "pl"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "pt"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "ro"; MessagesFile: "compiler:Languages\Romanian.isl"
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "sk"; MessagesFile: "compiler:Languages\Slovak.isl"
Name: "sr"; MessagesFile: "compiler:Languages\Serbian.isl"



