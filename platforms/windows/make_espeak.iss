; This is the script for "Inno Setup 5" to create the setup_espeak.exe installer for Windows

[Setup]
AppName=eSpeak
AppVerName=eSpeak version 1.48.03
AppCopyright=Licensed under GNU General Public License version 3.   (See file License.txt for details).
WindowVisible=yes

DefaultDirName={pf}\eSpeak
DefaultGroupName=eSpeak
OutputBaseFilename=setup_espeak
Compression=lzma
SolidCompression=yes
ShowLanguageDialog=auto

[Icons]
Name: "{group}\TTSApp"; Filename: "{app}\TTSApp.exe"; WorkingDir: "{app}"

[InstallDelete]
Type: files; Name: "{app}\espeak.dll"
Type: files; Name: "{app}\espeak-data\voices\*"
Type: filesandordirs; Name: "{app}\espeak-data\voices\en"
Type: filesandordirs; Name: "{app}\espeak-data\voices\asia"
Type: filesandordirs; Name: "{app}\espeak-data\voices\europe"
Type: filesandordirs; Name: "{app}\espeak-data\voices\other"
Type: filesandordirs; Name: "{app}\espeak-data\voices\test"
Type: filesandordirs; Name: "{app}\docs"

[Dirs]
Name: "{app}\espeak-data\soundicons"
Name: "{app}\espeak-data\mbrola"

[Files]
Source: "espeak_sapi.dll"; DestDir: "{app}"; Flags: regserver promptifolder replacesameversion
Source: "TTSApp.exe"; DestDir:"{app}"
Source: "espeak-data\*"; DestDir: "{app}\espeak-data"; Flags: recursesubdirs
Source: "dictsource\*"; DestDir: "{app}\dictsource"; Flags: recursesubdirs
Source: "docs\*"; DestDir: "{app}\docs"; Flags: recursesubdirs
Source: "command_line\*"; DestDir: "{app}\command_line"
Source: "Readme.txt"; DestDir: "{app}"; Flags: isreadme
Source: "License.txt"; DestDir: "{app}";
Source: "ChangeLog.txt"; DestDir: "{app}";

[Registry]
Root: HKLM; Subkey: "Software\Microsoft\Speech\PhoneConverters\Tokens\eSpeak"; Flags: deletekey uninsdeletekey


[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "af"; MessagesFile: "compiler:Languages\Afrikaans.isl"
Name: "bg"; MessagesFile: "compiler:Languages\Bulgarian.isl"
Name: "bs"; MessagesFile: "compiler:Languages\Bosnian.isl"
Name: "cs"; MessagesFile: "compiler:Languages\Czech.isl"
Name: "da"; MessagesFile: "compiler:Languages\Danish.isl"
Name: "de"; MessagesFile: "compiler:Languages\German.isl"
Name: "el"; MessagesFile: "compiler:Languages\Greek.isl"
Name: "es"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "fa"; MessagesFile: "compiler:Languages\Farsi.isl"
Name: "fi"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "fr"; MessagesFile: "compiler:Languages\French.isl"
Name: "hi"; MessagesFile: "compiler:Languages\Hindi.islu"
Name: "hr"; MessagesFile: "compiler:Languages\Croatian.isl"
Name: "hu"; MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "id"; MessagesFile: "compiler:Languages\Indonesian-5.1.11.isl"
Name: "it"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "ka"; MessagesFile: "compiler:Languages\Georgian.islu"
Name: "lv"; MessagesFile: "compiler:Languages\Latvian.isl"
Name: "ne"; MessagesFile: "compiler:Languages\Nepali.islu"
Name: "nl"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "no"; MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "pl"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "pt"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "ro"; MessagesFile: "compiler:Languages\Romanian.isl"
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "sk"; MessagesFile: "compiler:Languages\Slovak.isl"
Name: "sr"; MessagesFile: "compiler:Languages\Serbian.isl"
Name: "tr"; MessagesFile: "compiler:Languages\Turkish.isl"
Name: "vi"; MessagesFile: "compiler:Languages\Vietnamese.isl"
Name: "zh"; MessagesFile: "compiler:Languages\ChineseSimp-12-5.1.11.isl"

[CustomMessages]
v1=Select which voices to install
v2=or press Enter to accept defaults
v3=Enter voice names, eg: (for Portuguese)   pt,  or with a variant, eg: pt+f3

bs.v1=Odaberite jezik kojeg želite instalirati
bs.v2=ili pritisnite Enter za prihvat zadanih.
bs.v3=Upišite, npr. bs (za Bosanski) ili s varjantom: bs+f3

fr.v1=Sélectionnez les voix à installer
fr.v2=ou appuyez sur Entrée pour accepter les valeurs par défaut.
fr.v3=Entrez le nom des voix, ex : (pour le Français) fr, ou avec une variante ex : fr+f3

hr.v1=Odaberite jezik kojeg želite instalirati
hr.v2=ili pritisnite Enter za prihvat zadanih.
hr.v3=Upišite, npr. hr (za Hrvatski) ili s varjantom: hr+f3

pt.v1=Seleccione as vozes que pretende instalar
pt.v2=ou precione enter para aceitar as predefinidas.
pt.v3=Introduza os nomes das vozes, ex: (Brazil) pt (ou Portugal) pt-pt, ou com outras caracter?sticas, ex: pt+f3

sr.v1=Molimo vas da odaberete glasove, koje želite da instalirate
sr.v2=ili pritisnite Enter za instalaciju podrazumevanih glasova.
sr.v3=Da odaberete glas, unesite ime glasa kojeg želite instalirati, na primer sr, ili sr+f3

ne.v1=भित्र्याउन चाहेको आवाजको चयन गर्नु होस् ।
ne.v2=अथवा यसमा निर्धारण गरिएको आवाजलाई नै चयन गर्ने हो भने ईन्टर कुञ्जीलाई दबाउनु होस् । 
ne.v3=आवाजको नाम प्रविष्टी गर्नु होस् , जस्तै: नेपालीका लागि ne, अथवा स्वरको पनि चयन गर्ने हो भने ne+f3

hi.v1=स्थापना करने के लिए पसन्दिदा आवाजों को चयन करें ।
hi.v2=अगर आप कि चयन इसमें निर्दारित आवाज हि  हो तो केवल  ईन्टर कुञ्जी दबाए । 
hi.v3=आवाज कि   नाम प्रविष्टी करें, जैसा कि हिन्दी के लिए hi, और किसि स्वर भि चयन करना चाहते हो तो hi+f3

[Code]
var
  UILanguage: Integer;
  UIVoice: String;
  Page: TInputQueryWizardPage;
  voices_installed: array [0..200] of String;
  n_voices_installed: Integer;

const
  sEspeak = 'eSpeak-';
  RegVoice1 = 'Software\Microsoft\Speech\Voices\Tokens\eSpeak';
  RegPhConv = 'Software\Microsoft\Speech\PhoneConverters\Tokens';


function VoiceFromLanguage(language: Integer): String;
var
  lang_main: Integer;

begin
  lang_main := language and $3ff;
  Result := 'en';

  // Translation from microsoft codes to language codes
  // Used to set default voices for installation.
  case lang_main of
  $02: Result := 'bg';
  $03: Result := 'ca';
  $04: Result := 'zh';
  $05: Result := 'cs';
  $06: Result := 'da';
  $07: Result := 'de';
  $08: Result := 'el';
  $09: Result := 'en';
  $0a: Result := 'es-la  es-la+m3  es-la+f3';
  $0b: Result := 'fi';
  $0c: Result := 'fr';
  $0e: Result := 'hu';
  $0f: Result := 'is';
  $10: Result := 'it';
  $12: Result := 'ko';
  $13: Result := 'nl';
  $14: Result := 'no';
  $15: Result := 'pl';
  $16: Result := 'pt';
  $18: Result := 'ro';
  $19: Result := 'ru';
  $1a: Result := 'hr';
  $1b: Result := 'sk';
  $1c: Result := 'sq';
  $1d: Result := 'sv';
  $1f: Result := 'tr';
  $20: Result := 'ur';
  $21: Result := 'id';
  $25: Result := 'et';
  $26: Result := 'lv';
  $27: Result := 'lt';
  $29: Result := 'fa';
  $2a: Result := 'vi';
  $2b: Result := 'hy';
  //$2c: Result := 'az';
  $2d: Result := 'eu';
  $2f: Result := 'mk';
  $36: Result := 'af';
  $37: Result := 'kn';
  $39: Result := 'hi';
  //$3a: Result := 'mt';
  $3c: Result := 'ga';
  $3e: Result := 'ms';
  //$3f: Result := 'kk';
  $41: Result := 'sw';
  //$44: Result := 'tt';
  $46: Result := 'pa';
  //$48: Result := 'or';
  $49: Result := 'ta';
  $46: Result := 'pa';
  $4a: Result := 'te';
  $4b: Result := 'kn';
  $4c: Result := 'ml';
  //$4d: Result := 'as';
  //$50: Result := 'mn';
  $52: Result := 'cy';
  //$5e: Result := 'am';
  $61: Result := 'ne';
  //$87: Result := 'rw';
  //$88: Result := 'wo';
  end;

  // is there a match on the full language code?
  case language of
  $80a: Result := 'es-la  es-la+f3  es-la+m3';
  $c0a: Result := 'es';
  $816: Result := 'pt-pt  es-la';
  $41a: Result := 'hr';
  $81a: Result := 'sr';
  $c1a: Result := 'sr';
  $141a: Result := 'bs';
  end;
end;


function LanguageFromVoice(voice: String): String;
var
  value: Integer;
  lang1: String;
  len: Integer;
begin
  value := $409;   // default en-us

  lang1 := Copy(voice,0,2);
  if lang1 = 'mb' then
  begin
    lang1 := Copy(voice,3,3);
    len := Length(voice);
    if len > 8 then
      lang1 := Copy(voice,8,6);  // eg. mb-de4-en, return 'en'
  end;
  
  // Used to set the correct Microsoft language code in the registry
  // when a SAPI5 voice is installed.
  case lang1 of
  'af': value := $436;
  'am': value := $45e;
  'an': value := $40a;   // Aragon, use code for Spanish
  'as': value := $44d;
  'az': value := $42c;
  'bg': value := $402;
  'bn': value := $445;
  'bs': value := $41a;   // should be $141a but Jaws crashes on startup
  'ca': value := $403;
  'cs': value := $405;
  'cy': value := $452;
  'da': value := $406;
  'de': value := $407;
  'dv': value := $465;
  'el': value := $408;
  'en': value := $409;
  'es': value := $40a;
  'et': value := $425;
  'eu': value := $42d;
  'fa': value := $429;
  'fi': value := $40b;
  'fil': value := $464;
  'fr': value := $40c;
  'ga': value := $83c;
  'gd': value := $43c;
  'gu': value := $447;
  'hi': value := $439;
  'hr': value := $41a;
  'hu': value := $40e;
  'hy': value := $42b;
  'id': value := $421;
  'is': value := $40f;
  'it': value := $410;
  'ka': value := $437;
  'kk': value := $43f;
  'kn': value := $44b;
  'ko': value := $412;
  'lt': value := $427;
  'lv': value := $426;
  'mk': value := $42f;
  'ml': value := $44c;
  'mn': value := $450;
  'ms': value := $43e;
  'mt': value := $43a;
  'my': value := $455;
  'ne': value := $461;
  'nl': value := $413;
  'no': value := $414;
  'or': value := $448;
  'pa': value := $446;
  'pl': value := $415;
  'pt': value := $416;
  'ro': value := $418;
  'ru': value := $419;
  'rw': value := $487;
  'si': value := $45b;
  'sk': value := $41b;
  'sl': value := $424;
  'sq': value := $41c;
  'sr': value := $81a;
  'sv': value := $41d;
  'sw': value := $441;
  'ta': value := $449;
  'te': value := $44a;
  'tg': value := $45e;  // 'am'
  'tl': value := $464;
  'tn': value := $432;
  'tr': value := $41f;
  'tt': value := $444;
  'ur': value := $420;
  'vi': value := $42a;
  'wo': value := $488;
  'zh': value := $804;
  // mbrola voices
  '-af': value := $436;
  '-br': value := $416;
  '-ca': value := $c0c;
  '-cr': value := $41a;
  '-cz': value := $405;
  '-de': value := $407;
  '-en': value := $809;
  '-es': value := $40a;
  '-fr': value := $40c;
  '-gr': value := $408;
  '-hu': value := $40e;
  '-ic': value := $40f;
  '-in': value := $439;
  '-ir': value := $429;
  '-it': value := $410;
  '-mx': value := $80a;
  '-nl': value := $413;
  '-pl': value := $415;
  '-pt': value := $816;
  '-ro': value := $418;
  '-sw': value := $41d;
  '-us': value := $409;
  '-vz': value := $200a;
  end;

  // check for specific voices
  case voice of
  'pt-pt': value := $816;
  end;
  
  Result := Format('%X',[value]);
end;


// is the language number already in a PhoneConvertor ?
function CheckPhoneConvertors(Lcode: String): Integer;
var
  Convertors: TArrayOfString;
  ix: Integer;
  len: Integer;
  s1: String;
  s2: String;
  sLangs: String;
begin
  Result := 0;
  len := Length(Lcode);
  if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, RegPhConv, Convertors) then
  begin
    for ix := 0 to GetArrayLength(Convertors) - 1 do
    begin
      s1 := RegPhConv + '\' + Convertors[ix] + '\Attributes';
      if RegQueryStringValue(HKEY_LOCAL_MACHINE, s1, 'Language', sLangs) then
      begin
        s2 := Copy(sLangs, 1, len);
        if s2 = Lcode then
          Result := 1;
        if sLangs = Lcode then
          Result := 1;
        if Pos(';' + Lcode, sLangs) > 0 then
          Result := 1;
      end;
    end;
  end;
end;

// ensure the language number is included in a PhoneConvertor
procedure SetPhoneConvertor(Lcode: String);
var
  done: Boolean;
  s1: String;
  sLangs: String;
begin
  if CheckPhoneConvertors(Lcode) = 0 then
  begin
    done := False;
    s1 := RegPhConv + '\Universal\Attributes';
    if RegQueryStringValue(HKEY_LOCAL_MACHINE, s1, 'Language', sLangs) then
    begin
      done := RegWriteStringValue(HKEY_LOCAL_MACHINE, s1, 'Language', Lcode + ';' + sLangs);
    end;

    if done = False then
    begin
      s1 := RegPhConv + '\eSpeak\Attributes';
      if RegQueryStringValue(HKEY_LOCAL_MACHINE, s1, 'Language', sLangs) = False then
      begin
        // add 'eSpeak' dummy PhoneConvertor
        RegWriteStringValue(HKEY_LOCAL_MACHINE, RegPhConv + '\eSpeak', 'CLSID','{9185F743-1143-4C28-86B5-BFF14F20E5C8}');
        RegWriteStringValue(HKEY_LOCAL_MACHINE, RegPhConv + '\eSpeak', 'PhoneMap','- 0001');
        RegWriteStringValue(HKEY_LOCAL_MACHINE, s1, 'Language','');
      end;
      
      if RegQueryStringValue(HKEY_LOCAL_MACHINE, s1, 'Language', sLangs) then
      begin
        done := RegWriteStringValue(HKEY_LOCAL_MACHINE, s1, 'Language', Lcode + ';' + sLangs);
      end;
    end;
  end;
end;


procedure SetupVoice(Voice: String; Index: Integer);
var
  ix: Integer;
  RegVoice2: String;
  RegVoice2a: String;
  VoiceUC: String;
  Lcode: String;
  
begin
  Lcode := LanguageFromVoice(Voice);
  
  if Index = 0 then
    RegVoice2 := RegVoice1
  else
    RegVoice2 := RegVoice1 + Format('_%d',[Index]);
    
  RegVoice2a := RegVoice2 + '\Attributes';

  if Voice = 'default' then
    VoiceUC := 'default'
  else
    VoiceUC := Uppercase(Voice);

  // check for duplicate voice names
  for ix := 0 to n_voices_installed - 1 do begin
    if voices_installed[ix] = VoiceUC then
      Exit;
  end;
  if n_voices_installed < 200 then begin
    voices_installed[n_voices_installed] := VoiceUC;
    n_voices_installed := n_voices_installed + 1;
  end;
    
  RegWriteStringValue(HKEY_LOCAL_MACHINE,RegVoice2,'',sEspeak+VoiceUC);
  RegWriteStringValue(HKEY_LOCAL_MACHINE,RegVoice2,'CLSID','{BE985C8D-BE32-4A22-AA93-55C16A6D1D91}');
  RegWriteStringValue(HKEY_LOCAL_MACHINE,RegVoice2,'Path',ExpandConstant('{app}'));
  RegWriteStringValue(HKEY_LOCAL_MACHINE,RegVoice2,'VoiceName',Voice);

  RegWriteStringValue(HKEY_LOCAL_MACHINE,RegVoice2a,'Name',sEspeak+Voice);
  RegWriteStringValue(HKEY_LOCAL_MACHINE,RegVoice2a,'Gender','Male');
  RegWriteStringValue(HKEY_LOCAL_MACHINE,RegVoice2a,'Age','Adult');
  RegWriteStringValue(HKEY_LOCAL_MACHINE,RegVoice2a,'Language',Lcode);
  RegWriteStringValue(HKEY_LOCAL_MACHINE,RegVoice2a,'Vendor','http://espeak.sf.net');
  
  SetPhoneConvertor(Lcode);
end;



procedure CurStepChanged(CurStep: TSetupStep);
var
  field: Integer;
  Index: Integer;
  ix: Integer;
  ix2: Integer;
  Line: String;
  Voice: String;
  Voice2: String;
  separator: String;
begin
  if CurStep = ssPostInstall then
  begin
    separator := ' ';
    Index := 0;
    for field := 0 to 4 do begin
      Line := Page.Values[field];
      ix := 1;
      ix2 := 1;
      while ix2 > 0 do begin
        ix2 := Pos(separator,Copy(Line,ix,999));

        if ix2 = 0 then
          Voice := Copy(Line,ix,999)
        else
          Voice := Copy(Line,ix,ix2-1);
          
        Voice2 := Trim(Voice);
        if Voice2 <> '' then begin
          SetUpVoice(Trim(Voice),Index);
          Index := Index + 1;
        end;
        ix := ix + ix2;
      end;
    end;
  end;
end;


procedure InitializeWizard;
var
  lang: String;
  voice2: String;
  uilang: String;
begin
  // Create the language selection page

  UILanguage := GetUILanguage;
  UIVoice := VoiceFromLanguage(UILanguage);
  uilang := Format('  (Language code %x)',[UILanguage]);

  lang := ActiveLanguage;
  Page := CreateInputQueryPage(wpSelectDir,CustomMessage('v1'),CustomMessage('v2')+uilang,CustomMessage('v3'));

  // Add items (False means it's not a password edit)
  Page.Add('', False);
  Page.Add('', False);
  Page.Add('', False);
  Page.Add('', False);
  Page.Add('', False);


  // Set initial values (optional)
  if UIVoice = 'en' then
    voice2 := 'en-us'
  else
    voice2 := 'en';
    
  if Pos('+',UIVoice) = 0 then
    Page.Values[0] := Format('%s  %s',[UIVoice, UIVoice+'+f2'])
  else
    Page.Values[0] := Format('%s',[UIVoice]);
  Page.Values[1] := Format('%s',[voice2]);

  n_voices_installed := 0;
end;

procedure ClearRegistry;
var
  Index: Integer;
  RegVoice2: String;
  exists: Boolean;
begin
// remove all espeak voices from the registry
  exists := True;
  Index := 0;
  while exists do begin
    if Index = 0 then
      RegVoice2 := RegVoice1
    else
      RegVoice2 := RegVoice1 + Format('_%d',[Index]);

    exists := RegKeyExists(HKEY_LOCAL_MACHINE,RegVoice2);
    RegDeleteKeyIncludingSubkeys(HKEY_LOCAL_MACHINE,RegVoice2);
    Index := Index + 1;
  end;
end;

function InitializeSetup: Boolean;
begin
// remove all espeak voices from the registry
  ClearRegistry;
  result := True;
end;

function InitializeUninstall: Boolean;
begin
// remove all espeak voices from the registry
  ClearRegistry;
  result := True;
end;





