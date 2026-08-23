Portuguese
=======================================

Introduction
---------------------------------------

This document contains important information that the maintainer wishes they had learned when they started using eSpeak NG. They hope that reading this document will make the work of future contributors easier.

Basic Usage
---------------------------------------

### Check installation

Check if eSpeak NG is installed:

```
espeak-ng --version
```
```
eSpeak NG text-to-speech: 1.52.0  Data at: /usr/share/espeak-ng-data
```

The command's output shows the executable installed version and the directory where the data folder is installed.

The data folder contains the compiled voices and configuration files. It must be the same version as the executable and the library.

On Ubuntu and other Debian-like operating systems, the eSpeak executable, library, and data folder are installed in separate packages, although the software is small and self-contained.

### How to use default synthetic voices

How to use the European Portuguese language:

```bash
espeak-ng -v pt "Olá, mundo! Eu falo português europeu."
```

How to use the Brazilian Portuguese language:

```bash
espeak-ng -v pt-br "Olá, mundo! Eu também falo português brasileiro."
```

> **NOTE**: `pt` is synonym of `pt-pt`. All the other examples will be using `pt-br` (Brazilian Portuguese).

### How to use variant synthetic voices

How to use the Brazilian Portuguese language plus a variant voice, for example, "antonio":

```bash
espeak-ng -v pt-br+antonio "Olá, mundo!"
```

You can list all variants with this command:

```bash
ls '/usr/share/espeak-ng-data/voices/!v'
```

If you want to listen all variants, use this `bash` script:

```bash
for VARIANT in `ls '/usr/share/espeak-ng-data/voices/!v'`; do
	sleep 1; echo "$VARIANT"; \
	espeak-ng -v pt-br+$VARIANT "Eu sou a variante $VARIANT."; \
done;
```

### How to read from the standard input

You use eSpeak NG to read the text from the standard input.

How to speak a text from standard input:

```bash
echo "Olá, mundo!" | espeak-ng -v pt-br
```

> **NOTE**: each line read from the standard input is limited to 1000 characters by default. Prefer reading from a text file instead to avoid this limit.

### How to read content from text files

You use eSpeak NG to read the text in a file using the `-f` flag.

How to speak the contents of a file:

```bash
espeak-ng -v pt-br -f text-file.txt
```

If necessary, you can convert the text file to UTF-8 using this command:

```bash
# sudo apt install iconv
iconv -t UTF-8 text-file.txt > text-file.utf8.txt
```

> **NOTE**: To recognize non-ASCII characters, eSpeak NG expects text files encoded in UTF-8.

### How to Generate audio files as output

You can use eSpeak NG to generate an audio file using the `-w` flag.

The generated files have these properties:

* Container: WAVE
* Channels: 1 (mono)
* Bit rate: 256 kbps
* Sample rate: 16 kHz
* Format: PCM signed 16-bit little-endian

How to generate an WAVE audio file from a text file:

```bash
espeak-ng -v pt-br -f text-file.txt -w audio-file.wav
```

However, the audio files produced by eSpeak NG can be big, e.g. an e-book may take precious gigabytes of your storage. You have to use another program to convert the WAVE file into another format, say, MP3 file.

How to generate an MP3 audio file using `ffmpeg` program:

```bash
# sudo apt install ffmpeg
espeak-ng -v pt-br -f text-file.txt -w audio-file.wav
ffmpeg -i audio-file.wav audio-file.mp3 && rm audio-file.wav
```

How to generate an MP3 audio file using `--stdout` flag and `lame` program:

```bash
# sudo apt install lame
espeak-ng -v pt-br -f text-file.txt --stdout | lame -b 32 - audio-file.mp3
```

> **NOTE**: there's a [misterious bug](https://github.com/espeak-ng/espeak-ng/issues/2334) that truncates some audio files after 13 hours of playback, specifically at 13:31:36.

MBrola voices
---------------------------------------

If you don't like the robotic default synthetic voice, you can use voices from the MBrola project. But first your have to install MBrola and the desired voices.

This is the list of MBrola voices available for Portuguese:

* [`mb-pt1`](https://github.com/numediart/MBROLA-voices/tree/master/data/pt1): the only female voice for European Portuguese, developed by Babel Technologies SA.
* [`mb-br1`](https://github.com/numediart/MBROLA-voices/tree/master/data/br1): the male voice of Delta Talk, a synthesizer used in [Virtual Vision](https://www.virtualvision.com.br/blog/virtual-vision-completa-20-anos-em-2018/), both developed by MicroPower LTDA.
* [`mb-br2`](https://github.com/numediart/MBROLA-voices/tree/master/data/br2): the female voice of Delta Talk.
* [`mb-br3`](https://github.com/numediart/MBROLA-voices/tree/master/data/br3): the other male voice of Delta Talk?
* [`mb-br4`](https://github.com/numediart/MBROLA-voices/tree/master/data/br4): the voice of [Liane TTS](https://intervox.nce.ufrj.br/lianetts/), available for [DosVox](https://intervox.nce.ufrj.br/dosvox/) and [Orca](https://help.gnome.org/orca/), developed by SERPRO and UFRJ.

How to install all MBrola voices for Portuguese on Ubuntu:

```bash
sudo apt install mbrola mbrola-pt1 mbrola-br1 mbrola-br2 mbrola-br3 mbrola-br4
```

How to use a MBrola voice:

```bash
espeak-ng -v mb-br1 "Olá, mundo!"
```

If you want to listen all MBrola voices, use this `bash` script:

```bash
for VOICE in mb-pt1 mb-br1 mb-br2 mb-br3 mb-br4; do
	sleep 1; echo "$VOICE";
	espeak-ng -v $VOICE "Eu sou a voz $VOICE do MBrola.";
done;
```

> **NOTE**: from version 1.53, all voices have about the same speed. In older versions, MBrola voices were 25% or 30% faster than the synthetic voices.

Where to find information about the MBrola voices on Github: https://github.com/numediart/MBROLA-voices

Where to download Liane TTS: https://www.serpro.gov.br/menu/suporte/downloads/downloads-e-softwares/lianetts

Phonemization
---------------------------------------

eSpeak NG supports simple text-to-phonemes conversion, a process also known as phonemization.

Two types of phonemization are available:

* Kirshenbaum, also known as ASCII-IPA;
* International Phonetic Alphabet (IPA).

[Kirshenbaum](https://en.wikipedia.org/wiki/Usenet_ASCII-IPA_transcription) is the native way to specify pronounces in eSpeak NG. As input string, it describes pronunciation. As output string, it helps debugging. In both directions, Kirshenbaum is meant for eSpeak NG developers.

You can translate the input text to Kirshenbaum using the `-x` flag:

```bash
espeak-ng -v pt-br "Oi, Mundo!" -x
```
```
'oI
m'u~NdU
```

[IPA](https://en.wikipedia.org/wiki/International_Phonetic_Alphabet) is the international standard for speech description and transcription. For the language learner or the AI engigeer, IPA is more useful than Kirshenbaum.

You can translate the input text to IPA using the `--ipa` flag:

```bash
espeak-ng -v pt-br "Oi, Mundo!" --ipa
```
```
ˈoɪ
mˈũndʊ
```

### Sample Phonemization

This section contains IPA transcriptions generated by eSpeak NG.

Sample text to be transcribed:

> O vento norte e o sol discutiam qual dos dois era o mais forte, quando sucedeu passar um  
> viajante envolto numa capa. Ao vê-lo, põem-se de acordo em como aquele que primeiro  
> conseguisse obrigar o viajante a tirar a capa seria considerado o mais forte. O vento norte  
> começou a soprar com muita fúria, mas quanto mais soprava, mais o viajante se aconchegava  
> à sua capa, até que o vento norte desistiu. O sol brilhou então com todo o esplendor, e  
> imediatamente o viajante tirou a capa. O vento norte teve assim de reconhecer a superioridade  
> do sol.

Brazilian Portuguese transcription of the sample:

```bash
espeak-ng --ipa -v pt-br -f text-sample.txt
```

> ʊ vˈẽntʊ nˈɔɾtʃi iʲ ʊ sˈɔw dʒˌiskutʃˈiʲɐ̃ʊ̃ kwˈaʊ dʊz dˈoɪz ˌɛɾɐ ʊ mˈaɪs fˈɔɾtʃɪ  
> kwˈɐ̃ndʊ sˌusedˈeʊ pasˈaɾ ũn vˌiʲaʒˈɐ̃ntʃj ˌẽnvˈoltʊ nˌumɐ kˈapɐ  
> aʊ vˈelʊ  
> pˈõɪ̃msɪ dʒj ˌakˈoɾdw eɪŋ kˌomw ˌakˈelɪ kɪ pɾˌimˈeɪɾʊ kˌõnseɡˈisj ˌobɾiɡˈaɾ ʊ vˌiʲaʒˈɐ̃ntʃj a tʃiɾˈaɾ a kˈapɐ seɾˌiʲɐ kˌõnsideɾˈadw ʊ mˈaɪs fˈɔɾtʃɪ  
> ʊ vˈẽntʊ nˈɔɾtʃɪ kˌomesˈow a sopɾˈaɾ kõn mˈũɪ̃tɐ fˈuɾjɐ  
> mas kwˈɐ̃ntʊ mˈaɪs sˌopɾˈavɐ  
> mˈaɪz ʊ vˌiʲaʒˈɐ̃ntʃɪ sj ˌakõnʃeɡˈav ˌaː sˌuɐ kˈapɐ  
> atˌɛ kɪʲ ʊ vˈẽntʊ nˈɔɾtʃɪ dˌezistʃˈiʊ  
> ʊ sˈɔw bɾiʎˈow ẽntˈɐ̃ʊ̃ kõn tˈodw u ˌesplẽndˈoɾ  
> iʲ ˌimedʒˌiʲatɐmˈẽntʃɪʲ ʊ vˌiʲaʒˈɐ̃ntʃɪ tʃiɾˈow a kˈapɐ  
> ʊ vˈẽntʊ nˈɔɾtʃɪ tˌevj asˈĩn dʒɪ xˌekõɲesˈeɾ a sˌupeɾˌiʲoɾidˈadʒɪ dʊ sˈɔw  

European Portuguese transcription of the sample:

```bash
espeak-ng --ipa -v pt-pt -f text-sample.txt
```

> ʊ vˈẽntʊ nˈɔɾtɨ iʲ ʊ sˈɔl dˌiʃkutˈiʲɐ̃ʊ̃ kwˈɑl dʊʒ dˈoɪz ˌɛɾɐ ʊ mˈaɪʃ fˈɔɾtɨ  
> kwˈɐ̃ndʊ sˌusɨdˈeʊ pɐsˈaɾ ũn vˌiʲɐʒˈɐ̃ntɨ ˌẽnvˈoltʊ nˌumɐ kˈapɐ  
> aʊ vˈelʊ  
> pˈõɪ̃msɨ dɨ ˌɐkˈoɾdw eɪŋ kˈomw ˌɐkˈelɨ kɨ pɾˌimˈeɪɾʊ kˌõnsɨɡˈisɨ ˌɔbɾiɡˈaɾ ʊ vˌiʲɐʒˈɐ̃ntɨ ɐ tiɾˈaɾ ɐ kˈapɐ sɨɾˌiʲɐ kˌõnsidɨɾˈadw ʊ mˈaɪʃ fˈɔɾtɨ  
> ʊ vˈẽntʊ nˈɔɾtɨ kˌumɨsˈow ɐ supɾˈaɾ kõn mˈũɪ̃tɐ fˈuɾjɐ  
> mɐʃ kwˈɐ̃ntʊ mˈaɪʃ sˌupɾˈavɐ  
> mˈaɪz ʊ vˌiʲɐʒˈɐ̃ntɨ sɨ ˌɐkõnʃɨɡˈav ˌaː sˌuɐ kˈapɐ  
> ɐtˈɛ kɨ ʊ vˈẽntʊ nˈɔɾtɨ dˌɨziʃtˈiʊ  
> ʊ sˈɔl bɾiʎˈow ẽntˈɐ̃ʊ̃ kõn tˈodw u ʃplẽndˈoɾ  
> iʲ ˌimɨdˌiʲɐtɐmˈẽntɨ ʊ vˌiʲɐʒˈɐ̃ntɨ tiɾˈow ɐ kˈapɐ  
> ʊ vˈẽntʊ nˈɔɾtɨ tˌevɨ ɐsˈĩn dɨ ʁˌekõɲɨsˈeɾ ɐ supˌɛɾiʲˌuɾidˈadɨ dʊ sˈɔl  

The sample text used in this document was extracted from:

> Barbosa PA, Albano EC. Brazilian Portuguese. Journal of the International Phonetic Association. 2004;34(2):227-232. doi:10.1017/S0025100304001756


> **NOTE**: the transcription of MBrola voices have some differences due to necessary ajustments.

### Phoneme Tables

The following tables map phonetic symbols used in the Portuguese dialects providade by eSpeak NG.

**Consonants table:**

|IPA	|KIRSH	|X-SAMPA|
|---	|---	|---	|
|p	|p	|p	|
|b	|b	|b	|
|	|	|	|
|t	|t	|t	|
|d	|d	|d	|
|tʃ	|tS	|tS	|
|dʒ	|tZ	|tZ	|
|	|	|	|
|k	|k	|k	|
|ɡ  (1)	|g	|g	|
|kw	|kw	|kw	|
|ɡw	|kw	|gw	|
|	|	|	|
|f	|f	|f	|
|v	|v	|v	|
|	|	|	|
|s	|s	|s	|
|z	|z	|z	|
|ʃ	|S	|S	|
|ʒ	|Z	|Z	|
|	|	|	|
|ɾ	|*	|4	|
|r	|R	|r	|
|ɹ	|r	|r\	|
|	|	|	|
|x  (2)	|x  (3)	|x	|
|ɣ	|Q"	|G	|
|χ	|X	|X	|
|ʁ	|x  (3)	|R	|
|h	|h	|h	|
|ɦ	|h\<?>	|h\	|
|	|	|	|
|m	|m	|m	|
|n	|n	|n	|
|ŋ	|N	|N	|
|ɲ	|n^	|J	|
|	|	|	|
|l	|l	|l	|
|ɫ	|l/	|5	|
|w	|w	|w	|
|ʎ	|l^	|L	|

Notes about these consonants:

- (1): `ɡ` has a specific Unicode number: U+0261; i.e it is different from U+0067.
- (2): `x` is traditionally used by phoneticians in Brazil as an umbrella for all rhotics, such as `χ` and `h`.
- (3): `x` in eSpeak NG's Kirshenbaum is `x` in eSpeak NG's BP IPA and `ʁ` in EP IPA.

**Vowels table:**

|IPA	|KIRSH	|X-SAMPA|
|---	|---	|---	|
|a	|a	|a	|
|ɐ	|&  (1)	|6	|
|e	|e	|e	|
|ɛ	|E	|E	|
|i	|i	|i	|
|ɪ  (2)	|y	|I	|
|ɨ  (2)	|y	|1	|
|o	|o	|o	|
|ɔ	|O	|O	|
|u	|u	|u	|
|ʊ	|U	|U	|

Notes about these vowels:

- (1): `&` in original Kirshenbaum it is mapped to IPA's `æ`, but in eSpeak NG's Portuguese it is mapped to `ɐ`.
- (2): `y` in original Kirshenbaum it is mapped to IPA's `y` (rounded "i"), but in eSpeak NG's BP IPA it is `ɪ` and in EP IPA it is `ɨ`.

**Oral decrescent diphthongs:**

|IPA	|KIRSH	|X-SAMPA|
|---	|---	|---	|
|aɪ aj	|aI aj	|aj	|
|eɪ ej 	|eI ej	|ej	|
|ɛɪ ɛj	|EI Ej	|Ej	|
|oɪ oj	|oI oj	|oj	|
|ɔɪ ɔj	|OI Oj	|Oj	|
|uɪ uj	|uI uj	|uj	|
|	|	|	|
|aʊ aw	|aU aw	|aw	|
|eʊ ew	|eU ew	|ew	|
|ɛʊ ɛw	|EU Ew	|Ew	|
|oʊ ow	|oU ow	|ow	|
|iʊ iw	|iU iw	|iw	|

**Nasal Vowels:**

|IPA		|KIRSH			|X-SAMPA|
|---		|---			|---	|
|ɐ̃ ɐ̃n ɐ̃m ɐ̃ŋ	|&\~ &\~n &\~m &\~N	|6\~	|
|ẽ ẽn ẽm ẽŋ	|e\~ e\~n e\~m e\~N	|e\~	|
|ı͂ ı͂n ı͂m ı͂ŋ	|i\~ i\~n i\~m i\~N		|i\~	|
|õ õn õm õŋ	|o\~ o\~n o\~m o\~N		|o\~	|
|ũ u͂n ũm ũŋ	|u\~ u\~n u\~m u\~N		|u\~	|

**Nasal decrescent diphthongs:**

|IPA		|KIRSH		|X-SAMPA|
|---		|---		|---	|
|ãɪ ãj		|&\~j a\~j	|a\~j	|
|e͂ɪ e͂j		|eIN e\~j	|e\~j	|
|o͂ɪ o͂j		|oI\~ o\~j	|o\~j	|
|u͂ɪ u͂j		|uI\~ u\~j	|u\~j	|
|		|		|	|
|ãʊ ãw		|&U\~ a\~w	|a\~w	|


Normalization
---------------------------------------

eSpeak NG supports simple text normalization.

Supported normalizations:

* Cardinal number;
* Ordinal number;
* Roman number.

The normalizations above are very limited and do not take context into account, i.e. only the masculine forms are available and the words around the number are ignored.

Although not a strict normalization, some common abbreviations are pronounced as complete words; for example, "Dr." is pronouned as "doutor" and "Dra." is pronunced as "doutora".

Another loose normalization is the substitution of currency simbols with the corresponding names; for example, "US$" is pronounced as "dólar".

### Cardinal numbers

Cardinal numbers are supported with a comma as decimal separator, for example: 1 or 1,00.

Large cardinal numbers may have a dot as thousands separator, for example: 1.000 or 1.000,00.

> **NOTE**: currently, there's an unreported bug that produces a very high noise in MBrola voices if the space is allowed as thousands separator, for example: 1 000. What causes the bug is unknown. For that reason, the space as thousand separator should not be enabled for Portuguese.

### Ordinal numbers

Masculine and feminine are supported in these formats only: 1º, 1ª.

> **NOTE**: ordinal numbers with a dot such as 1.º are not supported.

### Roman numbers

Roman number normalization is supported, except for I, V, VI, LI and MI.

Roman numbers are read as cardinal numbers, i.e, the Roman number III is pronounced "três" (three), not "terceiro" (third).

Lowercase Roman numbers are pronounced as words, e.g. the Roman number "LII" (52) in lowercase is pronounced like "Lee" (the kung-foo actor last name).

> **NOTE**: there are some historical exceptions included in `dictlist/pt_rules` file, such as "Pedro I" (Pedro Primeiro) and "Paulo II" (Paulo Segundo).

### Calendar dates

Currently, there's no support for calendar date normalization.

Calendar dates are read as a series of numbers; for example, 01/01/2000 is read like: one one two thousand.

### Clock times

Currently, there's no support for clock time normalization.

Clock time is also read as a series of numbers; for example, 12:30 is read like: twelve thirty.

### Abbreviations

Common abbreviations for people titles are included in Portuguese configuration (`dictlist/pt_list`), such as "Sr." and "Dr.".

Abbreviations used in Law, such as "art." (law article) and "§" (law paragraph) are also included.

Also, there are many acronyms listed in Portuguese configuration that can be recognized, such as, CPI and IPVA.

### Currency

Some currency symbols are recognized and replaced with the respective currency name, e.g. "R$" is pronunced as "real" and "US$" is pronunced as "dólar".

It does not take the context into acount, thus they are always pronunced as singular words even when the numeric value is more than 1; e.g. "R$ 10" is pronunced like "real 10".


The List of Words File
---------------------------------------

This section shows some illustrative itens of `dictlist/pt_list` file, the list of words for Portuguese language.

Each item of the list specifies how a word should be pronounced by eSpeak NG. 

Generally speaking, each line has two columns: the word and the respective pronunciation. For example, the following line describes with phonetic symbols how the word "casa" (house) is pronouced:

```
casa	k'az&
```

The example above is the simpler pattern of line you can find in `dictlist/pt_list` file.

Some lines also contain flags to change the behaviour or eSpeak NG while pronouncing a word. For example, in the following line the flag `$text` is been used to specify the English word "like" using plain Portuguese, instead of phonetic symbols:

```
like	laique $text
```

The phonetic system used to specify pronunciations in eSpeak NG is the [Kirshenbaum alphabet](https://en.wikipedia.org/wiki/Usenet_ASCII-IPA_transcription), also known as ASCII-IPA.

We will liste more complex examples in the next subtopic.

### More examples

Acronym with a phoneme sequence to be pronounced as a word:

```
pix	p'iks
```

Acronym with an abbreviation indication to be pronounced letter by letter:

```
fbi	$abbrev
```

Abbreviation of a personal title with an alternative text and which may have a dot:

```
dr	doutor $text $dot
```

Foreign word with a phoneme sequence:

```
mouse	m'aUzy
```

Native name with an alternative text:

```
mario	mário $text
```

Foreign name with an indication of stress in the first syllable:

```
jennifer	$1
```

### Conditional numbers

Some lines of the list of words start with a conditional number to indicate it's scope:

* `?1`: the current line is specific for European Portuguese;
* `?2`: the current line is specific for Brasilian Portuguese;
* `?3`: the current line is for adding empethetic vowel between consonants in Brazilian Portuguese;
* `?4`: the current line is for phonetic ajustments to make MBrola voices sound better in Brazilian Portuguese.

For example, there are different pronunciations for the word "harwdare" in Portugal and Brazil, respectively indicated by the numbers `?1` and `?2`:

```
?1 hardware	'ard,w'Er
?2 hardware	h'ardw,Er
```

The List of Rules File
---------------------------------------

This section explain some aspects of `dictlist/pt_rules` file, the list of rules for Portuguese language.

### Group of rules

Pronunciation rules for letters are grouped togheter. The following example specifies basic groups of rules for the letters "c" and "h":

```
.group c
     _) c (_       se        // in acronym
        c          k         // except before the letter "e" and "i".

.group h
     _) h (_       ag'a      // in acronym
        h                    // silent letter
```

#### Rule strucutre

Each pronunciation rule may have up to 4 fiels:

* PRE-pattern: a sequence of letters before the pattern to be matched.
* Pattern: a sequence of letters to be matched, leaded by the letter that names the group.
* POST-pattern: a sequence of letters after the pattern to be matched.
* Pronunciation: a sequence of phonetic symbols to specify the pronunciation of the matched letters.

The structure of fields can be sketched this way:

```
   PRE) PATTERN (POST   PRONUNCIATION
```

The following example shows a pronunciation rule for the pair of letters "lh":

```
        lh         l^
```

### Wildcards

Some wildcard symbols can be used in PRE-pattern and POST-pattern fields.

These are the wildcards in Portuguese:

* `A`: a vowel, including y.
* `B`: a consonant, except h, l, r, w, and y.
* `C`: a consonant.
* `G`: a voiced consonant: b, d, g...
* `Y`: a front vowel: e, i and y.

### Special characters

Some special caracters are also used to change the matching behaviour.

These are some special characters:

* `+`: increase the matching score by 20 points.
* `D`: the character is a digit: 0, 1, 2... 9.
* `K`: the character is not a vowel, i.e. anything alse, including nothing. (actually another wildcard)
* `N`: use the rule if the word doesn't have a suffix, e.g. when pronuncing a non-derived word.
* `W`: use the rule when spelling a word letter by letter, e.g when pronouncing an acronym like FBI.
* `P<n>`: the n characters in PRE is a prefix, e.g the `4` letters in the pre-pattern field is the prefix "mini".
* `S<n>`: the n characters in POST is a suffix, e.g the `5` letters in the post-pattern field is the suffix "mente".

### Letter replacement

Letters can be replaced before the matching fase by listing pairs of letters right after the keyword `.replace`.

This is an hypothetical replacement group:

```
.replace
å       a
ñ       n
```

In the example, the letter "å" is replaced by "a", and the letter ñ is replaced by "n".

Actually, the `.replace` group, in Portuguese language, is used as a hack for Unicode normalization.

### Language Compilation

It is necessary to compile the language for the modifications made to the `dictlist/pt_list` and `dictlist/pt_rules` files to take effect. This command can be executed in the `dictsource` folder within the local eSpeak NG repository to compile the changes:

```bash
cd dictsource;
sudo espeak-ng --compile-debug=pt-br;
```

You should check if any errors occurred during compilation.

### Language Debugging

Debugging can be done using the `-X` flag:

```bash
espeak-ng -v pt-br "Oi, Mundo!" -X
```
```
Translate 'oi'
  1	 1001:	  [o]
 22	 1025:	  [oI]

'oI
Translate 'mundo'
  1	  963:	  [m]

  1	 1478:	  [u]
 42	 1512:	  [u~N]

  1	  284:	  [d#]

 41	 1002:	) o (_ [,U]
  1	 1001:	  [o]

m'u~NdU
```

In the example above, we observe the following:

- the words 'oi' and 'mundo' were translated separately.
- the first column contains numbers representing the score of a match.
- the second column contains numbers indicating the line of the rule that resulted in a match.
- the third column shows, incompletely, which rule was analyzed.
- in the last line of each translation block, there is a transcription of the word using characters from the Kirshenbaum alphabet.


References
---------------------------------------

* [IPA](https://en.wikipedia.org/wiki/IPA)
* [Kirshenbaum](https://en.wikipedia.org/wiki/Kirshenbaum)
* [X-SAMPA](https://en.wikipedia.org/wiki/X-SAMPA)
* [fonologia.org](https://fonologia.org)
* [Panglossa's IPA](https://panglossa.fandom.com/wiki/International_Phonetic_Alphabet)
* [Guia de pronúncia](https://pt.wiktionary.org/wiki/Ajuda:Guia_de_pron%C3%BAncia/Portugu%C3%AAs)
* [ASCII encodings](https://en.wikipedia.org/wiki/Comparison_of_ASCII_encodings_of_the_International_Phonetic_Alphabet)
* [Portuguese phonology](https://en.wikipedia.org/wiki/Portuguese_phonology)
* [European Portuguese](https://en.wikipedia.org/wiki/European_Portuguese)
* [Brazilian Portuguese](https://en.wikipedia.org/wiki/Brazilian_Portuguese)
* [IPA Help: Portuguese](https://en.wikipedia.org/wiki/Help:IPA/Portuguese)


