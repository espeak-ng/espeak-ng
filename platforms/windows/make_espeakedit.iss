
[Setup]
AppName=eSpeakEdit
AppVerName=eSpeakEdit version 1.45.01
DefaultDirName={pf}\eSpeak
DefaultGroupName=eSpeak
OutputBaseFilename=setup_espeakedit
Compression=lzma
SolidCompression=yes
DirExistsWarning=no
ShowLanguageDialog=auto

[Icons]
Name: "{group}\espeakedit"; Filename: "{app}\espeakedit.exe"; WorkingDir: "{app}"; Flags: runmaximized
Name: "{group}\Uninstall espeakedit"; Filename: "{uninstallexe}"

[InstallDelete]
Type: filesandordirs; Name: "{app}\phsource\vowelcharts"


[Files]
Source: "espeakedit.exe"; DestDir: "{app}"
;Source: "dictsource\*"; DestDir: "{app}\dictsource"; Flags: recursesubdirs
Source: "espeakedit\*"; DestDir: "{app}\espeakedit"; Flags: recursesubdirs
Source: "phsource\*"; DestDir: "{app}\phsource"; Flags: recursesubdirs
Source: "docs\*"; DestDir: "{app}\docs"; Flags: recursesubdirs
Source: "License.txt"; DestDir: "{app}";
;Source: "Readme.txt"; DestDir: "{app}"; Flags: isreadme


[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "af"; MessagesFile: "compiler:Languages\Afrikaans.isl"
Name: "bs"; MessagesFile: "compiler:Languages\Bosnian.isl"
Name: "cs"; MessagesFile: "compiler:Languages\Czech.isl"
Name: "de"; MessagesFile: "compiler:Languages\German.isl"
Name: "el"; MessagesFile: "compiler:Languages\Greek.isl"
Name: "es"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "fi"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "fr"; MessagesFile: "compiler:Languages\French.isl"
Name: "hr"; MessagesFile: "compiler:Languages\Croatian.isl"
Name: "hu"; MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "id"; MessagesFile: "compiler:Languages\Indonesian-5.1.11.isl"
Name: "it"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "lv"; MessagesFile: "compiler:Languages\Latvian-1-5.1.11.isl"
Name: "nl"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "no"; MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "pl"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "pt"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "ro"; MessagesFile: "compiler:Languages\Romanian.isl"
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "sk"; MessagesFile: "compiler:Languages\Slovak.isl"
;Name: "sq"; MessagesFile: "compiler:Languages\Albanian-2-5.1.11.isl"
Name: "sr"; MessagesFile: "compiler:Languages\Serbian.isl"
Name: "tr"; MessagesFile: "compiler:Languages\Turkish.isl"
Name: "zh"; MessagesFile: "compiler:Languages\ChineseSimp-12-5.1.11.isl"

