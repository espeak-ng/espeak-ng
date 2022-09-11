# Text to Phoneme Translation

- [Translation Files](#translation-files)
- [Phoneme names](#phoneme-names)
- [Pronunciation Rules](#pronunciation-rules)
  - [Rule Groups](#rule-groups)
  - [Letter Groups](#letter-groups)
  - [Rules](#rules)
  - [Special Characters in \<phoneme string\>](#special-characters-in-phoneme-string)
  - [Special Characters in Both \<pre\> and \<post\> ](#special-characters-in-both-pre-and-post)
  - [Special Characters Only in \<pre\> ](#special-characters-only-in-pre)
  - [Special Characters Only in \<post\> ](#special-characters-only-in-post)
- [Pronunciation Dictionary List](#pronunciation-dictionary-list)
  - [Multiple Words](#multiple-words)
  - [Special Characters in \<phoneme string\>](#special-characters-in-phoneme-string-1)
  - [Flags](#flags)
  - [Translating a Word to Another Word](#translating-a-word-to-another-word)
- [Conditional Rules](#conditional-rules)
- [Numbers and Character Names](#numbers-and-character-names)
  - [Letter Names](#letter-names)
  - [Numbers](#numbers)
- [Character Substitution](#character-substitution)
- [(Re)definition of character groups](#redefinition-of-character-groups)

----------

## Translation Files

There is a separate set of pronunciation files for each language, their names
starting with the language name.

There are two separate methods for translating words into phonemes:

* Pronunciation Rules. These are an attempt to define the pronunciation rules
  for the language. The source file is: `<language>_rules` (e.g. `en_rules`).

* Lookup Dictionary. A list of individual words and their pronunciations and/or
  various other properties. The source files are: `<language>_list` (e.g.
  `en_list`) and optionally `<language>_extra` (e.g. `en_extra`).

These files are compiled into the file `<language>_dict`  in the espeak-ng-data
directory (e.g. `espeak-ng-data/en_dict`).

## Phoneme names

Each of the language's phonemes is represented by a mnemonic of 1, 2, 3,
or 4 characters. Together with a number of utility codes (eg. stress
marks and pauses), these are defined in the phoneme data file.

The utility 'phonemes' are:

| Symbol | Description |
|--------|-------------|
| `'`    | primary stress |
| `,`    | secondary stress |
| `%`    | unstressed syllable |
| `=`    | put the primary stress on the preceding syllable |
| `_:`   | short pause |
| `_`    | a shorter pause |
| <code>&#124;</code> | use to separate two adjacent characters, to prevent them from being considered as a single multi-character phoneme |
| <code>&#124;&#124;</code> | indicates a word boundary within a phonetic string |

It is not necessary to specify the stress of every syllable. Stress
markers are only needed in order to change the effect of the language's
default stress rule.

The phonemes which are used to represent a language's sounds are based
loosely on the [Kirshenbaum (ASCII-IPA)](phonemes/kirshenbaum.md)
representation of the International Phonetic Alphabet.

Full list of commonly used phonemes can be found in the `phsource/phonemes`
file.

## Pronunciation Rules

The rules in the `<language>_rules` file specify the phonemes which are
used to pronounce each letter, or sequence of letters. Some rules only
apply when the letter or letters are preceded by, or followed by, other
specified letters.

To find the pronunciation of a word, the rules are searched and any
which match the letters at the in the word are given a score depending
on how many letters are matched. The pronunciation from the best
matching rule is chosen. The pointer into the source word is then
advanced past those letters which have been matched and the process is
repeated until all the letters of the word have been processed.

### Rule Groups

The rules are organized in groups, each starting with a `.group` line:

* `.group <character>`
  A group for each letter or character.

* `.group <2 characters>`
  Optional groups for some common 2 letter combinations. This is only needed,
  for efficiency, in cases where there are many rules for a particular letter.
  They would not be needed for a language which has regular spelling rules. The
  first character can only be an ascii character (less than 0x80).

**notes about rule groups**

When matching a word, firstly the 2-letter group for the two letters at
the current position in the word (if such a group exists) is searched,
and then the single-letter group. The highest scoring rule in either of
those two groups is used.

* `.group`
  A group for other characters which don't have their own group.

* `.replace`
  See section [Character Substitution](#character-substitution).

### Letter groups

Specific group of rules is declaration of letter sequences with some common
feature of letters for particular language. It may be used as a placeholder
of prefixes/infixes of words (in prerules) or infixed/postfixes in (postrules).

* `.L<nn>`
  Defines a group of letter sequences, any of which can match with `Lnn` in a
  pre or post rule (see below). nn is a 2 digit decimal number in the range 01
  to 94. e.g.:
  `.L01 b bl br pl pr`

**notes about letter groups**

There can be up to 200 items in one letter group.

When matching a word, firstly the group containing most letters is checked at
the current position in the word (if such a group exists), then shorter ones
till to the single-letter groups. The highest scoring rule of matching group is used.

`~` Letter in letter group means, that there can be no letter in this group
    in the pre- or post- rule.

_Example with prerule group:_

```
.L01 ~ b c
.group a
  L01) a      i  // A
```
following rules will match for words:

|Word |Phonetic spelling|
|-----|-----------------|
|base |bice             |
|case |cice             |
|ace  |ice              |

_Example with postrule group:_

```
.L01 ~ b c
.group a
       a (L01 u
```
|Word |Phonetic spelling|
|-----|-----------------|
|tab  |tub              |
|mac  |muc              |
|tea  |teu              |

### Rules

Each rule is on separate line, and has the syntax:

	[<pre>)] <match> [(<post>] <phoneme string>

* characters in `<pre>)` group are already spelled and "consumed"
* characters in `<match>` group are ones which will be spelled and "consumed"
by best matching rule
* characters in `(<post>` group will not be spelled and produced, but can be
used as reference to choose matching rule

Note that `<match>` group can be longer than name of character group, but cannot
be shorter.

Example:

	.group o
	       o      0   // "o" is pronounced as [0], one letter consumed
	       oo     u:  // but "oo" is pronounced as [u:], two letters consumed
	    b) oo (k  U   // pronounced as [U], two letters consumed


`oo` is pronounced as `[u:]`, but when also preceded by `b` and followed
by `k`, it is pronounced `[U]`. If 

In the case of a single-letter group, the first character of `<match>`
much be the group letter. In the case of a 2-letter group, the first two
characters of `<match>` must be the group letters. The second and third
rules above may be in either .group o or .group oo

Alphabetic characters in the `<pre>`, `<match>`, and `<post>` parts must
be lower case, and matching is case-insensitive. Some upper case letters
are used in `<pre>` and `<post>` with special meanings.

### Special characters in \<phoneme string\>:

* `_^_<language code>`
  Translate using a different language.

If this rule is selected when translating a word, then the translation is
aborted and the word is re-translated using the specified different language.
`<language code>` may be upper or lower case. This can be used to recognise
certain letter combinations as being foreign words and to use the foreign
pronunciation for them. e.g.:

	th (_    _^_EN

indicates that a word which ends in `th` is translated using the English
translation rules and spoken with English phonemes. 

### Special Characters in both \<pre\> and \<post\>

| Symbol      | Description |
|-------------|-------------|
| `_`         | Beginning or end of a word (or a hyphen). |
| `-`         | Hyphen. |
| `A`         | Any vowel[<sup>1</sup>](#redefinition-of-character-groups). |
| `C`         | Any consonant [<sup>1</sup>](#redefinition-of-character-groups). |
| `B H F G Y` | These may indicate other sets of characters[<sup>1</sup>](#redefinition-of-character-groups). |
| `L<nn>`     | Any of the sequence of characters defined as a letter grup. |
| `D`         | Any digit. |
| `K`         | Not a vowel (i.e. a consonant or word boundary or non-alphabetic character). |
| `X`         | There is no vowel until the word boundary. |
| `Z`         | A non-alphabetic character. |
| `%`         | Doubled (placed before a character in \<pre\> and after it in \<post\>. |
| `/`         | The following character is treated literally. |
| `\xxx`      | Character is written as by 3 digit octal value of `xxx`|
| `@`         | One syllable (i.e. at least one vowel or diphthong) |



Examples of rules:
```
        _)  a        A       // "a" at the start of a word
            a (CC    A       // "a" followed by two consonants
            a (C%    A       // "a" followed by a double consonant (the same letter twice)
            a (/%    A       // "a" followed by a percent sign
        %C) a        A       // "a" preceded by a double consonants
        @@) bi       bI      // "bi" preceded by at least two syllables
       @@a) bi       bI      // "bi" preceded by at least 2 syllables and following 'a'

         @) ly (_S2  lI       // "ly", at end of a word with at least one other
                              // syllable, is a suffix pronounced [lI].  Remove
                              // it and retranslate the word.
        
         _) un (@P2  %Vn      // "un" at the start of a word is an unstressed
                              // prefix pronounced [Vn]
```

Note, that:

1. Matching characters in the \<pre\> part do not affect the syllable counting.
1. Word end mark can't be used with syllable mark. E.g. `_@)` or `@_` will not work,
but you can use `@) ... (+` or `... (@+` to make rule with only one syllable prevail
rule with more syllables.

### Special characters only in \<pre\>:

| Symbol | Description |
|--------|-------------|
| `&`    | A syllable which may be stressed (i.e. is not defined as unstressed). |
| `V`    | Matches only if a previous word has indicated that a verb form is expected. |
| `xxJ`  | Skip letters until `xx`. Simple `xx` means start of current word. `xx_yy` means `xx` as end of previous and `yy` as start of current word. If necessary, more than one `J` can be used, and `Lxx` group as letter mark. |

e.g.

          get_J)  a   i      // for `get ada` will say `get adi`
           setJ)  a   o      // for `set ada` will say `set oda`
          ge_tJ)  a   i:     // for `ge tada` will say `get adi:`


### Special characters only in \<post\>:

| Symbol      | Description |
|-------------|-------------|
| `+`         | Force an increase in the score in this rule by 20 points (may be repeated for more effect). |
| `<`         | Force a decrease in the score in this rule by 20 points (may be repeated for more effect). |
| `Jxx`       | Skip letters until `xx`. Simple `xx` means end of current word. `xx_yy` means `xx` as end of current and `yy` as start of next word.  If necessary, more than one `J` can be used, and `Lxx` group as letter mark. |
| `S<number>` | This number of matching characters are a standard suffix, remove them and retranslate the word. |
| `P<number>` | This number of matching characters are a standard prefix, remove them and retranslate the word. |
| `Lnn`       | `nn` is a 2-digit decimal number in the range 01 to 20 Matches with any of the letter sequences which have been defined for letter group `nn` |
| `N`         | Only use this rule if the word is not a retranslation after removing a suffix. |
| `#`         | (English specific) change the next "e" into a special character "E" |
| `$noprefix` | Only use this rule if the word is not a retranslation after removing a prefix. |
| `$w_alt`    | Only use this rule if the word is `$w_alt2` found in the `*_list` file with the `$w_alt3`, `$alt`, `$alt2` or `$alt3` attribute respectively. |
| `$p_alt`    | Only use this rule if the part-word, `$p_alt2` up to and including the pre and `$p_alt3` match parts of this rule, is found in the `*_list` file with the `$alt`, `$alt2` or `$alt3` attribute respectively. |

e.g.

	_) un (i      ju:    // ... except in words starting "uni"
	_) un (inP2   ,Vn    // ... but it is for words starting "unin"
            a (J_get  u      // for `ada get` will say `uda get`
            a (Jset   e      // for `ada set` will say `ade set`
            a (Jg_et  u:     // for `adag et` will say `u:dag et`

`S` and `P` must be at the end of the \<post\> string.

`S<number>` may be followed by additional letters (e.g. `S2ei`). Some of
these are probably specific to English, but similar functions could be
made for other languages.

`P<number>` may be followed by additional letters (e.g. `P3v`).

| Symbol | Description |
|--------|-------------|
| `q`    | Query the `*_list` file to find stress position or other attributes for the stem, but don't re-translate the word with the suffix removed. |
| `t`    | Determine the stress pattern of the word *before* adding the suffix. |
| `d`    | The previous letter may have been doubled when the suffix was added. |
| `e`    | `e` may have been removed. |
| `i`    | `y` may have been changed to `i`. |
| `v`    | The suffix means the verb form of pronunciation should be used. |
| `f`    | The suffix means the next word is likely to be a verb. |
| `m`    | After this suffix has been removed, additional suffixes may be removed. |
| `t`    | Determine the stress pattern of the word **before** adding the prefix. |

## Pronunciation Dictionary List

The `<language>_list` file contains a list of words whose pronunciations are
given explicitly, rather than determined by the Pronunciation Rules. The
`<language>_extra` file, if present, is also used and it's contents are taken
as coming after those in `<language>_list`.

Also the list can be used to specify the stress pattern, or other
properties, of a word.

If the Pronunciation rules are applied to a word and indicate a standard
prefix or suffix, then the word is again looked up in Pronunciation
Dictionary List after the prefix or suffix has been removed.

Lines in the dictionary list have the form:

	<word>     [<phoneme string>]     [<flags>]

e.g.

	book      bUk

Rather than a full pronunciation, just the stress may be given, to
change where it would be otherwise placed by the Pronunciation Rules:

	berlin       $2      // stress on second syllable
	absolutely   $3      // stress on third syllable
	for          $u      // an unstressed word

### Multiple Words

A pronunciation may also be specified for a group of words, when these
appear together. Up to four words may be given, enclosed in brackets.
This may be used for change the pronunciation or stress pattern when
these words occur together,

	(de jure)    deI||dZ'U@rI2   // note || used as a word break in the phoneme string

or to run them together, pronounced as a single word

	(of a)       @v@

or to give them a flag when they occur together

	(such as)    sVtS||a2z   $pause        // precede with a pause

Hyphenated words in the `<language>_list` file must also be enclosed within
brackets, because the two parts are considered as separate words.

### Special characters in \<phoneme string\>:

| Symbol               | Description |
|----------------------|-------------|
| `_^_<language code>` | Translate using a different language. See explanation above. |

### Flags

A word (or group of words) may be given one or more flags, either
instead of, or as well as, the phonetic translation.

| Symbol               | Description |
|----------------------|-------------|
| `$1`                 | Primary stress on the 1st syllable. |
| `$2`                 | Primary stress on the 2nd syllable. |
| `$3`                 | Primary stress on the 3rd syllable. |
| `$4`                 | Primary stress on the 4th syllable. |
| `$5`                 | Primary stress on the 5th syllable. |
| `$6`                 | Primary stress on the 6th syllable. |
| `$7`                 | Primary stress on the 7th syllable. |
| `$u`                 | The word is unstressed. In the case of a multi-syllable word, a slight stress is applied according to the default stress rules. |
| `$u1`                | The word is unstressed, with a slight stress on its 1st syllable. |
| `$u2`                | The word is unstressed, with a slight stress on its 2nd syllable. |
| `$u3`                | The word is unstressed, with a slight stress on its 3rd syllable. |
| `$u+ $u1+ $u2+ $u3+` | As above, but the word has full stress if it's at the end of a clause. |
| `$pause`             | Ensure a short pause before this word (eg. for conjunctions such as "and", some prepositions, etc). Does not apply for 1st, 2nd or last word of a sentence. |
| `$brk`               | Ensure a very short pause before this word, shorter than $pause (eg. for some prepositions, etc). Does not apply for the last word of a sentence. |
| `$only`              | The rule does not apply if a prefix or suffix has already been removed. |
| `$onlys`             | As `$only`, except that a standard  plural ending is allowed. |
| `$stem`              | The rule only applies if a suffix has already been removed (i.e. word had to have suffix before). |
| `$strend`            | Word is fully stressed if it's at the end of a clause. |
| `$strend2`           | As $strend, but the word is also stressed if followed only by unstressed word(s). |
| `$unstressend`       | Word is unstressed if it's at the end of a clause. |
| `$abbrev`            | This has two meanings. If there is no phoneme string: Speak the word as individual letters, even if it contains a vowel (eg. "abc" should be spoken as "a" "b" "c"). If there is a phoneme string: This word is capitalized because it is an abbreviation and capitalization does not indicate emphasis (if the "emphasize all-caps" is on). |
| `$double`            | Cause a doubling of the initial consonant of the following word (used e.g. for Italian and Finnish). |
| `$alt $alt2 .. $alt7`| These 7 groups are language specific. Their use should be described in the language's `*_list` file. `$alt` and `$alt1` are synonyms.|
| `$combine`           | This word is treated as though it is combined with the following word with a hyphen. This may be subject to further conditions for certain languages. |
| `$dot`               | Ignore a `.` after this word even when followed by a capital letter (e.g. `Mr.` and `Dr.`). |
| `$hasdot`            | Use this pronunciation if the word is followed by a dot. (This attribute also implies `$dot`). |
| `$max3`              | Limit to 3 repetitions in pronunciation.|
| `$text`              | Word translates to replacement text, not phonemes.|
| `$verbf`             | The following word is probably a verb. |
| `$verbsf`            | The following word is probably a verb if it has an "s" suffix. |
| `$nounf`             | The following word is probably a noun. |
| `$pastf`             | The following word is probably past tense. |
| `$verb`              | Use this pronunciation if it's a verb, i.e. previously processed word had `$verbf` or `$verbsf` set.|
| `$noun`              | Use this pronunciation if it's a noun, i.e. previously processed word had `$nounf` set.|
| `$past`              | Use this pronunciation if it's past tense, i.e. previously processed word had `$pastf` set.|
| `$verbextend`        | Extend the influence of `$verbf` and `$verbsf` until  a word with $verb or $nounf is encountered. |
| `$capital`           | Use this pronunciation if the word has initial capital letter (eg. polish v Polish). |
| `$allcaps`           | Use this pronunciation if the word is all capitals. |
| `$accent`            | Used for the pronunciation of a single alphabetic character. The character name is spoken as the base-letter name plus the accent (diacritic) name. e.g. It can be used to specify that "â" is spoken as "a" "circumflex". |
| `$accent_before`     | used with accent names, say this accent name before the letter name. |
| `$atstart`           | Use this pronunciation if it's at the start of a clause. |
| `$atend`             | Use this pronunciation if it's at the end of a clause. |
| `$sentence`          | The rule only applies if the clause includes end-of-sentence (i.e. terminator is {. ? !} not {, ; :}). For example, `$atend $sentence` means that the rule only applies at the end of a sentence. |
| `$native`            | Not used, if switched to other translation. |

The last group are probably English specific, but something similar may
be useful in other languages. They are a crude attempt to improve the
accuracy of pairs like ob'ject (verb) v 'object (noun) and read
(present) v read (past).

The dictionary list is searched from bottom to top. The first match that
satisfies any conditions is used (i.e. the one lowest down the list). So
if we have:

	to    t@               // unstressed version
	to    tu:   $atend     // stressed version

then if `to` is at the end of the clause, we get `[tu:]`, if not then we
get `[t@]`.

### Translating a Word to Another Word

Rather than specifying the pronunciation of a word by a phoneme string,
you can specify another "sounds like" word.

Use the attribute `$text` e.g.

	cough    coff   $text

Alternatively, use the command `$textmode` on a line by itself to turn this on
for all subsequent entries in the file, until it's turned off by
`$phonememode`. e.g.

	$textmode
	cough     coff
	through   threw
	$phonememode

This feature cannot be used for the special entries in the `*_list` files which
start with an underscore, such as numbers.

Currently `textmode` entries are only recognized for complete words, and
not for stems from which a prefix or suffix has been removed (e.g.
the word "coughs" would not match the example above).

## Conditional Rules

Rules in a `*_rules` file and entries in a `*_list` file can be made
conditional. They apply only to some voices. This can be useful to
specify different pronunciations for different variants of a language
(dialects or accents).

Conditional rules have `?` and a condition number at the start if
the line in the `*_rules` or `*_list` file. This means that the rule
only applies of that condition number is specified in a `dictrules`
line in the [voice file](voices.md#dictrules).

If the rule starts with `?!` then the rule only applies if the
condition number is `not` specified in the voice file. e.g.

	?3     can't     kant    // only use this if the voice has:  dictrules 3
	?!3    rather    rA:D3   // only use if the voice doesn't have:  dictrules 3

## Numbers and Character Names

### Letter names

The names of individual letters can be given either in the `*_rules` or
`*_list` file. Sometimes an individual letter is also used as a word in
the language and its pronunciation as a word differs from its letter name.
If so, it should be listed in the `*_list` file, preceded by an underscore,
to give the letter name (as distinct from its pronunciation as a word).
e.g. in English:

	_a   eI

### Numbers

The operation the `TranslateNumber()` function is controlled by the
language's `langopts.numbers` option. This constructs spoken
numbers from fragments according to various options which can be set for
each language. The number fragments are given in the `*_list` file.

| Symbol        | Description |
|---------------|-------------|
| `_0` to `_9`  | The numbers 0 to 9. |
| `_13` etc.    | Any pronunciations which are needed for specific numbers in the range `_10` to `_99`. |
| `_2X` `_3X`   | Twenty, thirty, etc., used to make numbers 10 to 99. |
| `_0C`         | The word for `hundred`. |
| `_1C` `_2C`   | Special pronunciation for one hundred, two hundred, etc., if needed. |
| `_1C0`        | Special pronunciation (if needed) for 100 exactly. |
| `_0M1`        | The word for `thousand`. |
| `_1M1` `_2M1` | Special pronunciation for `1` thousand, `2` thousand, etc, if needed. |
| `_0M2`        | The word for `million`. |
| `_0M3`        | The word for 1,000,000,000. |
| `_0MX`        | The word for `10^3X` of number<sup>[1](#footnote1)</sup>. |
| `_0and`       | Word for `and` when speaking numbers (e.g. `two hundred and twenty`). |
| `_dpt`        | Word spoken for the decimal point/comma. |
| `_dpt2`       | Word spoken (if any) at the end of all the digits after a decimal point. |


**notes about ordinal numbers**
To enable ordinal numbers:
1. set `langopts.numbers` | NUM_ORDINAL_DOT in tr_languages.c
2. for each number symbol explained above, add a line with suffix o. 
For example: `_1o` for first, `_51o` for fifty first and so on.


## Character Substitution

Character substitutions can be specified by using a `.replace` section
at the start of the `*_rules` file. In each line multiple _source_ characters
can be replaced by one or two characters. This substitution is done to a word
_before_ word is searched in `*_list` or `*_listx` file and translated using
the spelling-to-phoneme rules. Only the lower-case version of the characters
needs to be specified. e.g.:

	.replace
	   ô   ő   // (Hungarian) allow the use of o-circumflex instead of o-double-accute
	   û   ű
	   cx  ĉ   // (Esperanto) allow "cx" as an alternative to c-circumflex
	   ﬁ   fi  // replace a single character ligature by two characters

## (Re)definition of character groups

The set of these vowel characters in `A` group and consonants in `C` group may be
redefined for a particular language. Other sets of letters indicated by `B`, `E`, `F` and `G`
usually have specific meaning for each particular language.

(Re)definition of letter groups is done in [tr_languages.c](../src/libespeak-ng/tr_languages.c)
file by calling `SetLetterBits()` function from (usually) `NewTranslator()` function.
Note, that letters should be stored as array of chars, thus multibyte
unicode letters should be transposed using `transpose_min` and `transpose_max` parameters
of particular `Translator` structure, or using `SetLetterBitsUTF8()` function.

----

<a name="footnote1"></a>1. Default length, to switch between named orders and just spelled digits in number, is 14 digits and is set in `tr->langopts.max_digits` field of `NewTranslator` function of [tr_languages.c](../src/libespeak-ng/tr_languages.c) file. To change it, update `SelectTranslator` function for particular language.
