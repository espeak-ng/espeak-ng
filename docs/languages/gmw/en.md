# English

- [Vowels](#vowels)
  - [Short Vowels](#short-vowels)
  - [Long Vowels](#long-vowels)
  - [Rhotic Vowels](#rhotic-vowels)
  - [Reduced Vowels](#reduced-vowels)
  - [Diphthongs](#diphthongs)
- [References](#references)

----------

The following English accents are supported by eSpeak NG and are referenced in
this document:

| BCP47          | Abbreviation | Accent Name            |
|----------------|--------------|------------------------|
| en             | BrE          | British English        |
| en-029         | JaE          | Caribbean              |
| en-GB-scotland | ScE          | Scottish English       |
| en-GB-x-gbclan |              | Lancastrian            |
| en-GB-x-gbcwmd |              | West Midlands          |
| en-GB-x-rp     | RP           | Received Pronunciation |
| en-US          | GenAm        | General American       |

The BCP47 name is the standard language identifier for the accent, used as the
espeak language name. The Abbreviation is used in the tables below for the IPA
transcriptions of that accent, and the BCP47 names are used for the eSpeak NG
phoneme names.

## Vowels

The English language support uses a vowel system based on John Wells' Lexical
Sets<sup>\[<a href="#ref1">1</a>\]</sup>. These were created by Wells in 1982
by comparing the Received Pronunciation British (RP) and General American
(GenAm) accents in use at that time.

### Short Vowels

| Lexical Set | en    | RP    | GenAm |
|-------------|-------|-------|-------|
| KIT         | `I`   | ɪ     | ɪ     |
| DRESS       | `E`   | e     | ɛ     |
| TRAP        | `a`   | æ     | æ     |
| LOT         | `0`   | ɒ     | ɑ     |
| STRUT       | `V`   | ʌ     | ʌ     |
| FOOT        | `U`   | ʊ     | ʊ     |

Additionally, Wells defines the following lexical sets to describe vowels that
are different in both RP and GenAm:

| Lexical Set | en    | RP    | GenAm |
|-------------|-------|-------|-------|
| BATH        | `aa`  | ɑː    | æ     |
| CLOTH       | `O2`  | ɒ     | ɔ     |

### Long Vowels

| Lexical Set | en    | RP    | GenAm |
|-------------|-------|-------|-------|
| FLEECE      | `i:`  | iː    | i     |
| PALM        | `A:`  | ɑː    | ɑ     |
| THOUGHT     | `O:`  | ɔː    | ɔ     |
| GOOSE       | `u:`  | uː    | u     |

### Rhotic Vowels

These are vowels that are followed by an `r` that is not part of the next syllable
when considering the root form of the word containing that vowel.

| Lexical Set | en    | en-GB-scotland | RP    | GenAm | ScE   |
|-------------|-------|----------------|-------|-------|-------|
| NURSE       | `3:`  | `VR`           | ɜː    | ɝ     | ʌɾ    |
| START       | `A@`  | `A@`           | ɑː    | ɑɹ    | ɐ̟ɾ    |
| NORTH       | `O@`  | `O@`           | ɔː    | ɔɹ    | ɔɾ    |
| FORCE       | `o@`  | `o@`           | ɔː    | oɹ    | oɾ    |
| CURE        | `U@`  | `U@`           | ʊə̯    | ʊɹ    | ʉɾ    |
| NEAR        | `i@3` | `i@3`          | ɪə̯    | ɪɹ    | iɾ    |
| SQUARE      | `e@`  | `e@`           | eə̯    | ɛɹ    | eɾ    |

__NOTE:__ `/i@3/` is used for the NEAR lexical set to differentiate it from
`/i@/` used in words like `million`.

Additionally, espeak-ng has the following phonemes for different accents:

| Lexical Set | en    | en-GB-scotland | RP    | GenAm | ScE   |
|-------------|-------|----------------|-------|-------|-------|
| TERM        | `3:`  | `3:`           | ɜː    | ɝ     | ɛɾ    |
| BIRD        | `3:`  | `IR`           | ɜː    | ɝ     | ɪɾ    |

### Reduced Vowels

These are unstressed vowels that differ from the vowels in the main lexical sets.

| Lexical Set | en    | RP    | GenAm |
|-------------|-------|-------|-------|
| HAPPY       | `i`   | ɪ     | i     |
| COMMA       | `@`   | ə     | ə     |
| LETTER      | `3`   | ə     | ɚ     |

Additionally, espeak-ng has the following phonemes for unstressed vowels.

| Lexical Set | en    | BrE   | RP    | GenAm | JaE   |
|-------------|-------|-------|-------|-------|-------|
| EXPLORE     | `e#`  | ɛ     | ɪ     | ɛ     | ɛ     |
| ROSES       | `I#`  | ɪ     | ɪ     | ᵻ     | ɪ     |
| BLESSED     | `I2#` | ɪ     | ɪ     | ᵻ     | ɛ     |
| RABBIT      | `I2`  | ɪ     | ɪ     | ɪ     | ɪ     |

The EXPLORE lexical set is used to support unstressed KIT vowels that have split
from the KIT vowel and merged with the DRESS vowel in some accents. This includes
`ex-` words.

The ROSES lexical set is used for words that are KIT in some accents and COMMA
in others. The degree to which this occurs varies between accents and speakers.

The BLESSED lexical set is used for -ed based adjectives. These tend to preserve
the KIT vowel in accents.

The RABBIT lexical set is used for unstressed KIT vowels. Some American accents
have merged this with the COMMA lexical set, such that `rabbit` and `abbot`
rhyme.

### Diphthongs

| Lexical Set | en    | RP    | GenAm |
|-------------|-------|-------|-------|
| FACE        | `eI`  | eɪ̯    | eɪ̯    |
| PRICE       | `aI`  | aɪ̯    | aɪ̯    |
| CHOICE      | `OI`  | ɔɪ̯    | ɔɪ̯    |
| GOAT        | `oU`  | əʊ̯    | oʊ̯    |
| MOUTH       | `aU`  | aʊ̯    | aʊ̯    |

## References

1. <a name="ref1"></a> Wikipedia.
   [Lexical set](https://en.wikipedia.org/wiki/Lexical_set). 2017.
   Creative Commons Attribution-Sharealike 3.0 Unported License (CC-BY-SA).

2. <a name="ref2"></a> Wikipedia.
   [IPA chart for English dialects](https://en.wikipedia.org/wiki/International_Phonetic_Alphabet_chart_for_English_dialects). 2018.
   Creative Commons Attribution-Sharealike 3.0 Unported License (CC-BY-SA).
