## Introduction
The present fork adds [Luxembourgish](https://en.wikipedia.org/wiki/Luxembourgish) as 127th language to the eSpeak-NG text-to-speech (TTS) synthesizer (version.1.50.1).

The main purpose of the project is the creation of a rule based International Phonetic Alphabet ([IPA](https://en.wikipedia.org/wiki/International_Phonetic_Alphabet)) phonemizer for transcriptions of luxembourgish audio utterances.

The phonemized text allows to create and use luxembourgish high-quality synthetic voices, trained with deep machine learning (ML) speech models, based on neural networks.

The speech generated with the associated eSpeak-NG sound engine is based on formant synthesis techniques and is of low quality. The related luxembourgish voice `Luxi` is intelligible, but I did no sound optimization because my focus is put on the phonemization front-end process.

## Luxembourgish customization
Four files are needed to include Luxembourgish as additional language in the eSpeak-NG project :   

* [phsource/ph_luxembourgish](https://github.com/mbarnig/espeak-ng-lb/blob/lëtzebuergesch/phsource/ph_letzebuergesch)
* [dictsource/lb_rules](https://github.com/mbarnig/espeak-ng-lb/blob/lëtzebuergesch/dictsource/lb_rules)
* [dictsource/lb_list](https://github.com/mbarnig/espeak-ng-lb/blob/lëtzebuergesch/dictsource/lb_list)
* [dictsource/lb_emoji](https://github.com/mbarnig/espeak-ng-lb/blob/lëtzebuergesch/dictsource/lb_emoji)

The customization process can be splitted into two steps :  

1. convert the characters (graphemes) of text into [phonetic descriptions](https://en.wikipedia.org/wiki/Luxembourgish_phonology) (phoneme-ID's) 
2. convert the phoneme-ID's into [IPA phonemes](https://en.wikipedia.org/wiki/Help:IPA/Luxembourgish) and define instructions how to generate sounds for each phoneme.

eSpeak-NG is an evolution of the famous speak-program for the [ACORN/RISC_OS](https://en.wikipedia.org/wiki/RISC_OS) computers developed in 1995 by Jonathan Duddington. In 2007 the program was renamed [eSpeak](https://en.wikipedia.org/wiki/ESpeak). After the disappearance of the original developer early 2015, a new project eSpeak-NG (new generation) was started in December 2015 under the direction of [Reece H. Dunn](https://github.com/rhdunn). 

When speak was created the phoneme-ID's were based on the text encoding ASCII, available at this time. This was never changed in the eSpeak-NG fork to avoid too much divergence from the original code. A few years ago, the support of IPA names in the output was added, but this work-around is still laborious, compared to a state-of-art solution where IPA names are used as phoneme-ID's. The current problems with the eSpeak-NG architecture are stated in this [roadmap](https://github.com/espeak-ng/espeak-ng/wiki/eSpeak-NG-roadmap) document.

Let's start to describe the `ph_luxembourgish` file which is the heart of the whole process.

### Phoneme inventory
We distinguish two types of phonemes : [vowels]( https://en.wikipedia.org/wiki/Vowel) ([monophtongs](https://en.wikipedia.org/wiki/Monophthong) and [diphtongs](https://en.wikipedia.org/wiki/Diphthong)) and [consonants](https://en.wikipedia.org/wiki/Consonant).

I use the word `Auto` with two vowels `au, o` and one consonant `t` to explain the definition of the related phonemes in the `ph_luxembourgish` file. The code is formed by simple text strings. The minimal code for the word `Auto` has the following format :

```   
phoneme aU
  vwl starttype #a endtype #u
  ipa ɑʊ
  length 190
  FMT(vdiph/au)
endphoneme

phoneme t
  vls alv stp
  ipa t
  WAV(ustop/t, 90)
endphoneme

phoneme o:
  vwl starttype #o endtype #o
  ipa oː
  length 170
  FMT(vowel/o)
endphoneme
```  

The minimal phoneme definition includes 5 parts :

* the term `phoneme` followed by the phoneme-ID
* the description of the articulation
* the IPA symbol
* the parameters for the sound generation (length, FMT, WAV, etc)
* the closing term `endphoneme`  

To specify the sound generation for a phoneme, eSpeak-NG provides a whole set of specific parameters and conditions for fine-tuning. A typical more complex example is shown hereafter :

```  
phoneme t
  vls alv stp
  voicingswitch d
  lengthmod 2
  Vowelin f1=0  f2=1700 -300 300  f3=-100 80
  Vowelout f1=0 f2=1700 -300 250  f3=-100 80  rms=20

  IF nextPh(isPause2) THEN
    WAV(ustop/t_)
  ELIF nextPh(r) THEN
    WAV(ustop/t_)
  ELIF nextPh(@-) THEN
    WAV(ustop/t_dnt, 50)
  ENDIF
  WAV(ustop/t, 90)
endphoneme
```    
I mentionned in the introduction that my goal is not the perfect sound design. Therefore I specified the strict minimum of parameters for each phoneme. [Interested users](https://github.com/espeak-ng/espeak-ng/issues/1013) can easely modify the code to enhance the speech quality. The official documentation provides the necessary guidance :

* [Phoneme Model](https://github.com/espeak-ng/espeak-ng/blob/master/docs/phoneme_model.md)
* [Phoneme features and IPA](https://github.com/espeak-ng/espeak-ng/blob/master/docs/phonemes.md)
* [Phoneme Tables](https://github.com/espeak-ng/espeak-ng/blob/master/docs/phontab.md)

It is not necessary to specify all the phonemes for a language, they can be inherited from a `master phoneme file` or from another language. Most of the 126 languages supported by eSpeak-NG use this mecanism. Some languages even rely only on inherited phoneme definitions.

To get the full control of the phonemization process and to rest independent from code changes in other languages, I prefer however to define all the used phonemes for the luxembourgish language.

Speech models based on deep ML models are usually trained with about hundred hours of audio records and related transcriptions. To train a deep ML neural network with small datasets, which is the case for Luxembourgish, it is necessary to use a low number of different phonemes to obtain valid results. Based on my earlier experience with ML-TTS models I selected the following sets of phonemes (total : 56) for this first release of my project :  

```   
Monophtongs: (20)
IPA phoneme symbols :    ɑ, aː, ɛː, e, æ, eː, ə,  ɐ,  i, iː, o, oː, u, uː, y, yː, ɑ̃ː, ɛ̃ː, õː, œː 
my eSpeak phoneme-ID's : a, a:, aE, e, E, e:, @E, eR, i, i:, o, o:, u, u:, y, y:, eA, iA, oA, OU

Diphtongs: (9)
IPA phoneme symbols :     æːɪ, ɑʊ, æːʊ, ɑɪ, ɜɪ, oɪ, iə, əʊ, uə
my eSpeak phoneme-ID's :   aI, aU,  AU, eI, OI, eU, iE, oU, uE 

Consonants: (27)
IPA phoneme symbols      b, c, d, f, g, h, j, k, l, m, n, ŋ, p, ʀ, s, t, v, w, z, ʒ, ʁ, ʑ,  ʦ,  ʃ, dʒ, X, ɕ
my eSpeak phoneme-ID's : b, c, d, f, g, h, j, k, l, m, n, N, p, r, s, t, v, w, z, J, rR, Z, TS, S, dZ, x, X
```   
To add more phonemes, if required in the future, it's easy to modify the `ph_luxembourgish` file and to adapt the related files `lb_rules`, `lb_list` and `lb_emoji`.

In the next chapter I will describe the process to link letters (characters, graphemes) to phoneme-ID's. 

### Text to Phoneme translation
#### lb_rules
The main file to define the translation (correspondence) between letters and phoneme-ID's is [lb_rules](https://github.com/mbarnig/espeak-ng-lb/blob/lëtzebuergesch/dictsource/lb_rules). It's a sort of large table with the following format, starting with a `.group` line :

```   
.group <one character>  

or

.group <two characters>  

then

   previous letter sequence )  current letter sequence ( next letter sequence   phoneme-ID's sequence  // eventually comments
```    

Let's take again the `Auto` example to explain the text code by considering that the the word starts with the dipthtong `au`, followed by the consonant `t`. The word ends with the monophtong `o`, preceded by the consonant `t`. The consonant `t` itself is preceded by the dipthtong `au` and followed by the monophtong `o`.

```   
.group aU
  _) au (t aU
  
.group o
  t) o (_  o:
  
.group t
  au) t (o  t 
```
Again, eSpeak-NG provides a large set of specific commands and parameters to define the translation rules in a very detailed and comprehensive manner. A simple example are the capital letter A which indicates any vowel and the capital letter C which indicates any consonant. If we use A and C in the above example, we can easely define the rules to phonemize the words `Aula` and `Auto` with combined code : 

```  
.group aU
  _) au (C aU
  
.group o
  C) o (_  o:

.group a
  C) a (_  a:
  
.group t
  A) t (A  t 
  
.group l
  A) l (A  l
```
Another way for coding to obtain the same result is :

```  
.group aU
  _) au (C aU
  
.group to
  A) to (_  to:
  
.group la
  A) la (_  la:
```   
To find the pronunciation of a word, the rules are searched and any which match the letters at the current position in the word are given a score, depending on how many letters are matched. The pronunciation from the best matching rule is chosen. The position pointer inside the word is then advanced past those consumed letters which have been matched and the process is repeated until all the letters of the word have been processed.
    
There are numerous possibilities to code the phonemization rules with `prev`, `current` and `post` letter sequences (or placeholders, symbols, options, letter groups with some common features, etc). 
  
There are also some restrictions. The first letter of a rule-group-ID with two letters must be a 7bit-ASCII character (less than 0x80). In the case of a one-letter-group-ID extended ASCII-characters are accepted (as `è ö î` etc). 

The guide [Text to Phoneme Translation](https://github.com/espeak-ng/espeak-ng/blob/master/docs/dictionary.md) provides all available options to set up a complete and complex translation table. I opted however for a few simple rules because I have a preference to include a luxembourgish pronunciation lexicon into the file `lb_list` described below. This way I take into account that Luxembourgish contains numerous loanwords from german, french and english, which makes the definition of global rules more difficult.
  
An interested user can easely extend and adapt the rules in the `lb_rules` file to his own needs.

#### lb_list
Usually a text to synthesize does not only contain common words from a specific language, but also punctuations, numbers, symbols, personal names, abbreviations, loanwords and eventually [Speech Synthesis Markup Language](https://en.wikipedia.org/wiki/Speech_Synthesis_Markup_Language) (SSML) code. These exceptions are handled in the [lb_list](https://github.com/mbarnig/espeak-ng-lb/blob/lëtzebuergesch/dictsource/lb_list). It's a simple text lexicon with the following entries :   
```    
letter-name    phoneme-ID               or            $accent
word           phoneme-ID sequence
word           other similar word                     $text
word           (stressed, unstressed, ..)             $u  // intonation 
abbreviation   phoneme-ID sequence      or            $$abbrev
number         phoneme-ID sequence
symbol         phoneme-ID sequence
```    

Here is a small excerpt from the luxembourgish `lb_list` file to show the principles :   
  
``` 
// letter names
_a a:
_e e:
ä    $accent  
û    $accent  
// intonation
de      $u  // unstress
den     $u
// numbers
_0 nul
_1 e:nt
_2 TSve:
_0C honert
_0M1 daUz@End
// punctuations
_. punkt
_, koma
// symbols
+ pluz
& an
€ eUro
CSV    $abbrev
OGBL   $abbrev
asbl   $abbrev
ASCII azki
// personal names
Marco  marko:
Barnig barniS
Simone zimon
```   
The flag $accent tells the processor to say the letter name, followed by the name of the [diacritics](https://en.wikipedia.org/wiki/Diacritic). The flag $abbrev indicates to say the letter names in sequence. Speaking the names of punctuations works only if the flag `--punc` is specified at inference. 

Again, eSpeak-NG offers a lot of options to finetune the list entries. My `lb_list` file for the first version of this project is work in progress. I will update it progressively to comply to my needs. An interested user can do the same.

Note: the supported SSML tags are documented in the document [SSML and HTML Support](https://github.com/mbarnig/espeak-ng-lb/blob/lëtzebuergesch/docs/markup.md).
  
#### lb_emoji
Today, graphics are gaining more and more importance in text. For this reason eSpeak-NG replaced the former file `language_extra` by a file called `language_emoji`. [Emoji's](https://en.wikipedia.org/wiki/Emoji) are small pictures which can be represented as encoded characters. Originating on Japanese mobile phones in 1997, emoji's became increasingly popular worldwide in the 2010s after being added to several mobile operating systems. To assure an  interoperability between browsers, mobiles and messaging systems, the emoji's are standardized since 2014 by the [Unicode Consortium](https://en.wikipedia.org/wiki/Unicode_Consortium). Every year additional emoji's are added to the standard upon public proposals.
  
The [full list of emoji's](https://unicode.org/emoji/charts/full-emoji-list.html) is available at the home-page of the Unicode Consortium.

Here are two examples how to include emoji's (graphics) in a luxembourgish sentence to submit to the eSpeak-NG phonemizer :

An der &#x1F570; hunn sech den &#x1F9ED;&#x1F4A8; an d’&#x1F31E; gestridden, wie vun hinnen zwee wuel méi &#x1F4AA; wier, wéi e &#x1F6B6;, deen an ee waarme &#x1F9E5; agepak war, iwwert de &#x1F6E4; koum.

Haut sinn &#x261D; mat mengen Enkelkanner &#x1F9D1;&#x200D;&#x1F91D;&#x200D;&#x1F9D1; , &#x1F466; , &#x1F467; , an &#x1F469; an den &#x1F3AA; gaangen. Do hunn mer e &#x1F98D;, eng &#x1F992;, en &#x1F418; an en &#x1F98F; gesinn.
  
To phonemize the above sentences I added the following entries into the [lb_emoji](https://github.com/mbarnig/espeak-ng-lb/blob/lëtzebuergesch/dictsource/lb_emoji) file :
  
```
&#x1F570;  TSaIt
&#x1F9ED;  nort
&#x1F32C;  vant
&#x1F31E;  zon
&#x1F4AA;  Sta:rk
&#x1F6B6;  vand@EreR
&#x1F9E5;  mant@El
&#x1F6E4;  ve:
&#x261D;   eS
&#x1F9D1;&#x200D;&#x1F91D;&#x200D;&#x1F9D1; Sa:rel, toma:
&#x1F466;  asto:r
&#x1F467;  kapuzin
&#x1F469;  tessa
&#x1F3AA;  TSirkus
&#x1F98D;  gorila:
&#x1F992;  giraf
&#x1F418;  elefant
&#x1F98F;  rino:zerus
```
A more comfortable option is to insert emoji's from the popup menu in the text-editor :   

![insert emoji](https://github.com/mbarnig/espeak-ng-lb/blob/l%C3%ABtzebuergesch/_layouts/insert-emoji.png)

Here are the results of the phonemization :

```  
ɑn dɐ ‚ʦæ:ɪt / hun zeɕ dən ’noʀtvɑnt ɑn ‚dzon gə’ʃtʀidən / viə fun hinən ‚ʦve: vuəl ‚meɪ ʃta:ʀk viɐ / veɪ eː ‚vɑndəʀɐ / de:n ɑn eː ‚va:ʀmə ‚mɑntəl ‚a:ɡəpa:k va:ʀ / ivɐt də ‚veː kəʊm //

haUt sin eS mat mengen enkelkaneR Sa:rel / toma: / asto:r / kapuzin an tessa an den TSirkus ga:ngen // do: hun meR e gorila: / eN giraf / en elefant an e rino:zerus gesin //
```   

For convenience I moved the symbols from the `lb_list` file to the `lb_emoji` file.

## Integration
In the last step the four luxembourgish files are embedded into the source code of the eSpeak-NG project. The following source files are modified :

* [Makefile.am](https://github.com/mbarnig/espeak-ng-lb/blob/master/Makefile.am)
* [phsource/phonemes](https://github.com/mbarnig/espeak-ng-lb/blob/master/phsource/phonemes)
* [docs/languages.md](https://github.com/mbarnig/espeak-ng-lb/blob/master/docs/languages.md)

In the `Makefile.am` file I added the following lines :

in position 392 :
```
phsource/ph_luxembourgish \
```
in position 570 :
```  
espeak-ng-data/lb_dict \
```   
in position 792 :
```
lb: espeak-ng-data/lb_dict
espeak-ng-data/lb_dict: dictsource/lb_list dictsource/lb_rules dictsource/lb_emoji
```   
In the `phsource/phonemes` file I added these lines at position 1763 :
```  
phonemetable lb base1
include ph_luxembourgish
```  
In the `docs/languages.md` file I added this line after the latin language :     

`gmw` | `lb` |  West Germanic | Lëtzebuergesch   

Two additional files must be created :
* [espeak-ng-data/lang/gmw/lb](https://github.com/mbarnig/espeak-ng-lb/blob/lëtzebuergesch/espeak-ng-data/lang/gmw/lb)
* [espeak-ng-data/voices/!v/Luxi](https://github.com/mbarnig/espeak-ng-lb/blob/lëtzebuergesch/espeak-ng-data/voices/!v/Luxi)

Both files are very simple if we include only the strict minimum. 

Here comes the content of the language file `lb` :
```
name Lëtzebuergesch
language lb
```
Here is the voice file `Luxi` :
```  
name Luxi
language lb
maintainer mbarnig
```   
As usual, eSpeak-NG provides numerous options to customize these files. Please read the guide [Voice and Language files](https://github.com/mbarnig/espeak-ng-lb/blob/lëtzebuergesch/docs/voices.md) to get a detailed documentation about all available features.
folder 
When building the project the first time, an additional file `lb_dict` is created inside the folder [espeak-ng-data](https://github.com/mbarnig/espeak-ng-lb/tree/l%C3%ABtzebuergesch/espeak-ng-data), which is a compressed binary combination of the three files `lb_rules`, `lb_list` and `lb_emoji`.

## Build and use the project
Now the forked eSpeak-NG source code is ready for compilation, hopefully without problems. Compilation is easy if you have a personal computer with a well configured development environment and all required tools for C-compilation. On my Ubuntu 20.04 system this is the case and I build and install the project with 4 commands :

```   
./autogen.sh
./configure
make
sudo make install
```

![build espeak-ng-lb](https://github.com/mbarnig/espeak-ng-lb/blob/l%C3%ABtzebuergesch/_layouts/espeak-ng-lb.png)

The [building guide](https://github.com/mbarnig/espeak-ng-lb/blob/master/docs/building.md) provides detailed info how to compile and build eSpeak NG from the source on different operating systems. If you need help, please visit the [issues section](https://github.com/espeak-ng/espeak-ng/issues) from the master eSpeak-NG project. Currently there are 333 open and 311 closed issues, so probably another user found already a solution for your problem.

To use the project, I need only one command :

`espeak-ng -v lb -q --ipa -f <text file>`   

The content of the text file is phonemized with luxembourgish rules (flag `-v lb`) with IPA symbols (flag `--ipa`), without producing speech (flag `-q`). All the optional configuration flags are explained in the [--help document](https://github.com/mbarnig/espeak-ng-lb/blob/lëtzebuergesch/src/espeak-ng.1.ronn).

A more comfortable option is to use the great tool [espeak-phonemizer](https://github.com/rhasspy/espeak-phonemizer), created by [Michael Hansen](https://www.linkedin.com/in/michael-hansen-9885b2105/) (alias [synesthesiam](https://synesthesiam.com)), which transforms the output from espeak-NG in a format ready for submission as input to train a deep machine learning luxembourgish TTS model, based on neural networks.

My favorite Github projects for my luxembourgish ML-TTS experiments are : 

* [Rhasppy/Larynx](https://github.com/rhasspy/larynx)
* [Coqui-TTS](https://github.com/coqui-ai/TTS)
* [Implementations by Keon Lee](https://github.com/keonlee9420)
* [Implementations by Jaehyeon Kim](https://github.com/jaywalnut310)


## References
* [The best of two breeds](https://www.web3.lu/the-best-of-two-breeds/), 2021, mbarnig  
* [Synthèse de la parole](https://www.amazon.fr/Synthèse-parole-électrique-électronique-informatique/dp/2322238600/), 2020, Marco Barnig   
* [Synthèse vocale](https://www.web3.lu/synthese-vocale/), 2019, mbarnig 
* [Emojis et Unicode](https://www.web3.lu/timeline/emojis-et-unicode/), 2018, mbarnig
* [Evolution of character encoding](https://www.web3.lu/evolution-of-character-encoding/), 2016, mbarnig  
* [Speech Corpora for TTS](https://www.web3.lu/speech-corpora-tts/), 2015, mbarnig
* [Festival Text-to-Speech Package](https://www.web3.lu/festival-text-speech-package/), 2015, mbarnig
* [eSpeak Formant Synthesizer](https://www.web3.lu/espeak-formant-synthesizer/), 2014, mbarnig 
* [Mary TTS (Text To Speech)](https://www.web3.lu/marytts-text-speech/), 2014, mbarnig
* [Language : fr, de, en, lb, eo](https://www.web3.lu/languages/), 2014, mbarnig
* [Spectrograms and speech processing](https://www.web3.lu/spectrogram-speech-processing/), 2014, mbarnig
* [Phonemes, phones, graphemes and visemes](https://www.web3.lu/phonemes-phones-graphemes-visemes/), 2014, mbarnig
* [FreeTTS : a Java speech synthesizer](https://www.web3.lu/freetts-a-java-speech-synthesizer/), 2005, mbarnig
