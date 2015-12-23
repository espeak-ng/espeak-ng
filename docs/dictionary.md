# Table of contents

* [Text to phoneme translation](#text-to-phoneme-translation)
  * [Translation Files](#translation-files)
  * [Phoneme names](#phoneme-names)
  * [Pronunciation Rules](#pronunciation-rules)
    * [Rule Groups](#rule-groups)
    * [Rules](#rules)
    * [Special characters in \<phoneme string\>](#special-characters-in-phoneme-string)
    * [Special Characters in both \<pre\> and \<post\> ](#special-characters-in-both-pre-and-post)
    * [Special characters only in \<pre\> ](#special-characters-only-in-pre)
    * [Special characters only in \<post\> ](#special-characters-only-in-post)
  * [Pronunciation Dictionary List](#pronunciation-dictionary-list)
    * [Multiple Words](#multiple-words)
    * [Special characters in \<phoneme string\>](#special-characters-in-phoneme-string)
    * [Flags](#flags)
    * [Translating a Word to another Word](#translating-a-word-to-another-word)
  * [Conditional Rules](#conditional-rules)
  * [Numbers and Character Names](#numbers-and-character-names)
    * [Letter names](#letter-names)
    * [Numbers](#numbers)
  * [Character Substitution](#character-substitution)

# Text to phoneme translation


## Translation Files

There is a separate set of pronunciation files for each language, their
names starting with the language name.

There are two separate methods for translating words into phonemes:

* Pronunciation Rules. These are an attempt to define the pronunciation rules for the language. The source file is:
**\<language\>\_rules**  (eg. `en_rules`)

* Lookup Dictionary. A list of individual words and their pronunciations and/or various other properties. The source files are:
**\<language\>\_list**  (eg. `en_list`) and optionally **\<language\>\_extra**.

These two files are compiled into the file **\<language\>\_dict**  in
the espeak-data directory (eg. `espeak-data/en_dict`)

## Phoneme names

Each of the language's phonemes is represented by a mnemonic of 1, 2, 3,
or 4 characters. Together with a number of utility codes (eg. stress
marks and pauses), these are defined in the phoneme data file (_TODO_).

The utility 'phonemes' are:


Symbol    | Description
--------- | -------------
**'**     | primary stress
**,**     | secondary stress
**%**     | unstressed syllable
**=**     | put the primary stress on the preceding syllable
**\_:**   | short pause
**\_**    | a shorter pause
**\|\|**  | indicates a word boundary within a phoneme string
**\|**    | can be used to separate two adjacent characters, to prevent them from being considered as a multi-character phoneme mnemonic 
 
It is not necessary to specify the stress of every syllable. Stress
markers are only needed in order to change the effect of the language's
default stress rule.

The phonemes which are used to represent a language's sounds are based
loosely on the Kirshenbaum ascii character representation of the
International Phonetic Alphabet
[www.kirshenbaum.net/IPA/ascii-ipa.pdf](http://www.kirshenbaum.net/IPA/ascii-ipa.pdf)

Full list of commonly used phonemes can be found in [phsource/phonemes](../phsource/phonemes) file.

## Pronunciation Rules

The rules in the **\<language\>\_rules**  file specify the phonemes
which are used to pronounce each letter, or sequence of letters. Some
rules only apply when the letter or letters are preceded by, or followed
by, other specified letters.

To find the pronunciation of a word, the rules are searched and any
which match the letters at the in the word are given a score depending
on how many letters are matched. The pronunciation from the best
matching rule is chosen. The pointer into the source word is then
advanced past those letters which have been matched and the process is
repeated until all the letters of the word have been processed.

### Rule Groups

The rules are organized in groups, each starting with a ".group" line:

* **.group \<character\>**  
  A group for each letter or character.

* **.group \<2 characters\>**  
  Optional groups for some common 2 letter combinations. This is only needed, for efficiency, in cases where there are many rules for a particular letter. They would not be needed for a language which has regular spelling rules. The first character can only be an ascii character (less than 0x80).

* **.group**  
  A group for other characters which don't have their own group.

* **.L\<nn\>**  
  Defines a group of letter sequences, any of which can match with Lnn in a pre or post rule (see below). nn is a 2 digit decimal number in the range 01 to 25. eg:  
  `.L01 b bl br pl pr`

* **.replace**  
  See section [Character Substitution](#character-substitution).


When matching a word, firstly the 2-letter group for the two letters at
the current position in the word (if such a group exists) is searched,
and then the single-letter group. The highest scoring rule in either of
those two groups is used.

### Rules

Each rule is on separate line, and has the syntax:

`[<pre>)] <match> [(<post>] <phoneme string>`

eg.

```
.group o
       o        0    // "o" is pronounced as [0]
       oo       u:   // but "oo" is pronounced as [u:]
    b) oo (k    U
```

"oo" is pronounced as [u:], but when also preceded by "b" and followed
by "k", it is pronounced [U].

In the case of a single-letter group, the first character of \<match\>
much be the group letter. In the case of a 2-letter group, the first two
characters of \<match\> must be the group letters. The second and third
rules above may be in either .group o or .group oo

Alphabetic characters in the \<pre\>, \<match\>, and \<post\> parts must
be lower case, and matching is case-insensitive. Some upper case letters
are used in \<pre\> and \<post\> with special meanings.

### Special characters in \<phoneme string\>:


* **_^_\<language code\>**  
  Translate using a different language.
If this rule is selected when translating a word, then the translation is aborted and the word is re-translated using the specified different language. \<language code\> may be upper or lower case. This can be used to recognise certain letter combinations as being foreign words and to use the foreign pronunciation for them. eg:
 `th (_    _^_EN`

indicates that a word which ends in "th" is translated using the English translation rules and spoken with English phonemes. 

### Special Characters in both \<pre\> and \<post\>


Symbol           | Description
-----------------| -----------
**\_**           | Beginning or end of a word (or a hyphen)
**-**            | Hyphen.                              
**A**            | Any vowel (the set of vowel characters may be defined for a particular language).                
**C**            | Any consonant.                       
**B H F G Y**    | These may indicate other sets of characters (defined for a particular language).                           
**L\<nn\>**      | Any of the sequence of characters defined as a letter group (see above).                              
**D**            | Any digit.                           
**K**            | Not a vowel (i.e. a consonant or word boundary or non-alphabetic character).                          
**X**            | There is no vowel until the word boundary.                            
**Z**            | A non-alphabetic character.          
**%**            | Doubled (placed before a character in \<pre\> and after it in \<post\>. 
**/**            | The following character is treated literally.                           

The sets of letters indicated by A, B, C, E, F G may be defined
differently for each language.

Examples of rules:

```
     _)  a         // "a" at the start of a word
         a (CC     // "a" followed by two consonants
         a (C%     // "a" followed by a double consonant (the same letter twice)
         a (/%     // "a" followed by a percent sign
     %C) a         // "a" preceded by a double consonants
```

### Special characters only in \<pre\>:

Symbol          | Description
**@**           | Any syllable.   
**&**           | A syllable which may be stressed (i.e. is not defined as unstressed).
**V**           | Matches only if a previous word has  indicated that a verb form is expected.                            

eg.
```
     @@)  bi      // "bi" preceded by at least two syllables
     @@a) bi      // "bi" preceded by at least 2 syllables and following 'a'
```

Note, that matching characters in the \<pre\> part do not affect the
syllable counting.

### Special characters only in \<post\>


Symbol             | Description
-------------------| -----------
**@**              | A vowel follows somewhere in the word.                               
**+**              | Force an increase in the score in this rule (may be repeated for more effect).                            
**S\<number\>**    | This number of matching characters are a standard suffix, remove them and retranslate the word.           
**P\<number\>**    | This number of matching characters are a standard prefix, remove them and retranslate the word.           
**Lnn**            | **nn** is a 2-digit decimal number in the range 01 to 20 Matches with any of the letter sequences which have been defined for letter group **nn** 
**N**              | Only use this rule if the word is not a retranslation after removing a suffix.                             
**\#**             | (English specific) change the next "e" into a special character "E"    
**$noprefix**     | Only use this rule if the word is not a retranslation after removing a prefix.                             
**$w\_alt**       | Only use this rule if the word is $w\_alt2 found in the \*\_list file with the **$w\_alt3** **$alt**, **$alt2** or **$alt3** attribute respectively.             
**$p\_alt**       | Only use this rule if the part-word, $p\_alt2\ up to and including the pre and $p\_alt3 match parts of this rule, is found in the \*\_list file with the **$alt**, **$alt2** or **$alt3** attribute respectively.             

eg.
```
   @) ly (_S2   lI      // "ly", at end of a word with at least one other
                        //   syllable, is a suffix pronounced [lI].  Remove
                        //   it and retranslate the word.

   _) un (@P2   %Vn     // "un" at the start of a word is an unstressed
                        //   prefix pronounced [Vn]
   _) un (i     ju:     // ... except in words starting "uni"
   _) un (inP2  ,Vn     // ... but it is for words starting "unin"
```

S and P must be at the end of the \<post\> string.

S\<number\> may be followed by additional letters (eg. S2ei ). Some of
these are probably specific to English, but similar functions could be
made for other languages.

Symbol| Description
----- | -----------
**q** | query the \_list file to find stress position or other attributes for the stem, but don't re-translate the word with the suffix removed.       
**t** | determine the stress pattern of the word **before** adding the suffix   
**d** | the previous letter may have been doubled when the suffix was added.  
**e** | "e" may have been removed.          
**i** | "y" may have been changed to "i."   
**v** | the suffix means the verb form of pronunciation should be used.       
**f** | the suffix means the next word is likely to be a verb.                
**m** | after this suffix has been removed, additional suffixes may be removed. 

P\<number\> may be followed by additonal letters (eg. P3v ).

Symbol| Description
----- | -----------
**t** | determine the stress pattern of the word **before** adding the prefix  
**v** | the suffix means the verb form of pronunciation should be used.      

## Pronunciation Dictionary List

The **\<language\>\_list**  file contains a list of words whose
pronunciations are given explicitly, rather than determined by the
Pronunciation Rules. The **\<language\>\_extra**  file, if present, is
also used and it's contents are taken as coming after those in
**\<language\>\_list**.

Also the list can be used to specify the stress pattern, or other
properties, of a word.

If the Pronunciation rules are applied to a word and indicate a standard
prefix or suffix, then the word is again looked up in Pronunciation
Dictionary List after the prefix or suffix has been removed.

Lines in the dictionary list have the form:

```
<word>     [<phoneme string>]     [<flags>]
```

eg.
```
     book      bUk
```

Rather than a full pronunciation, just the stress may be given, to
change where it would be otherwise placed by the Pronunciation Rules:

```
     berlin       $2      // stress on second syllable
     absolutely   $3      // stress on third syllable
     for          $u      // an unstressed word
```

### Multiple Words

A pronunciation may also be specified for a group of words, when these
appear together. Up to four words may be given, enclosed in brackets.
This may be used for change the pronunciation or stress pattern when
these words occur together,

```
    (de jure)    deI||dZ'U@rI2   // note || used as a word break in the phoneme string
```

or to run them together, pronounced as a single word
```
    (of a)       @v@
```

or to give them a flag when they occur together

```
    (such as)    sVtS||a2z   $pause        // precede with a pause
```

Hyphenated words in the **\<language\>\_list**  file must also be
enclosed within brackets, because the two parts are considered as
separate words.

### Special characters in \<phoneme string\>:

Symbol| Description
----- | -----------
**\_\^\_\<language code\>**  | Translate using a different language. See explanation above.                         

### 3 Flags

A word (or group of words) may be given one or more flags, either
instead of, or as well as, the phonetic translation.

Symbol| Description
----- | -----------
$u                        | The word is unstressed. In the case of a multi-syllable word, a slight stress is applied according to the default stress rules.               
$u1                       | The word is unstressed, with a slight stress on its 1st syllable.  
$u2                       | The word is unstressed, with a slight stress on its 2nd syllable.  
$u3                       | The word is unstressed, with a slight stress on its 3rd syllable.  
$u+ $u1+ $u2+ $u3+        | As above, but the word has full stress if it's at the end of a clause.                             
$1                        | Primary stress on the 1st syllable. 
$2                        | Primary stress on the 2nd syllable. 
$3                        | Primary stress on the 3rd syllable. 
$4                        | Primary stress on the 4th syllable. 
$5                        | Primary stress on the 5th syllable. 
$6                        | Primary stress on the 6th syllable. 
$7                        | Primary stress on the 7th syllable. 
$pause                    | Ensure a short pause before this word (eg. for conjunctions such as "and", some prepositions, etc).     
$brk                      | Ensure a very short pause before this word, shorter than $pause (eg. for some prepositions, etc).        
$only                     | The rule does not apply if a prefix or suffix has already been removed. 
$onlys                    | As $only, except that a standard  plural ending is allowed.           
$stem                     | The rule only applies if a suffix has already been removed.           
$strend                   | Word is fully stressed if it's at the end of a clause.                
$strend2                  | As $strend, but the word is also stressed if followed only by unstressed word(s).                 
$unstressend              | Word is unstressed if it's at the end of a clause.                    
$atend                    | Use this pronunciation if it's at the end of a clause.                
$double                   | Cause a doubling of the initial consonant of the following word (used for Italian).                 
$capital                  | Use this pronunciation if the word has initial capital letter (eg. polish v Polish).                   
$allcaps                  | Use this pronunciation if the word is all capitals.                    
$dot                      | Ignore a . after this word even when followed by a capital letter (eg. Mr. Dr. ).                          
$hasdot                   | Use this pronunciation if the word is followed by a dot. (This attribute also implies $dot).      
$sentence                 | The rule only applies if the clause includes end-of-sentence (i.e. it is not terminated by a comma). For example, "$atend $sentence" means that the rule only applies at the end of a sentence.                  
$abbrev                   | This has two meanings. If there is no phoneme string: Speak the word as individual letters, even if it contains a vowel (eg. "abc" should be spoken as "a" "b" "c"). If there is a phoneme string: This word is capitalized because it is an abbreviation and capitalization does not indicate emphasis (if the "emphasize all-caps" is on).                   
$accent                   | Used for the pronunciation of a single alphabetic character. The character name is spoken as the base-letter name plus the accent (diacritic) name. eg. It can be used to specify that "â" is spoken as "a" "circumflex".                       
$combine                  | This word is treated as though it is combined with the following word with a hyphen. This may be subject to fuither conditions for certain languages.                          
$alt   $alt2   $alt3      | These are language specific. Their use should be described in the language's \*\*\_list file 
$verb                     | Use this pronunciation if it's a verb.                               
$noun                     | Use this pronunciation if it's a noun.                               
$past                     | Use this pronunciation if it's past tense.                              
$verbf                    | The following word is probably is a verb.                               
$verbsf                   | The following word is probably is a if it has an "s" suffix.            
$nounf                    | The following word is probably not a verb.                               
$pastf                    | The following word is probably past tense.                              
$verbextend               | Extend the influence of $verbf and $verbsf.                           

The last group are probably English specific, but something similar may
be useful in other languages. They are a crude attempt to improve the
accuracy of pairs like ob'ject (verb) v 'object (noun) and read
(present) v read (past).

The dictionary list is searched from bottom to top. The first match that
satisfies any conditions is used (i.e. the one lowest down the list). So
if we have:

```
    to    t@               // unstressed version
    to    tu:   $atend     // stressed version
```

then if "to" is at the end of the clause, we get [tu:], if not then we
get [t@].

### Translating a Word to another Word

Rather than specifying the pronunciation of a word by a phoneme string,
you can specify another "sounds like" word.

Use the attribute **$text** eg.
```
    cough    coff   $text
```

Alternatively, use the command **$textmode** on a line by itself to
turn this on for all subsequent entries in the file, until it's turned
off by **$phonememode**. eg.
```
    $textmode
    cough     coff
    through   threw
    $phonememode
```

This feature cannot be used for the special entries in the **\_list**
files which start with an underscore, such as numbers.

Currently "textmode" entries are only recognized for complete words, and
not for for stems from which a prefix or suffix has been removed (eg.
the word "coughs" would not match the example above).

## Conditional Rules

Rules in a **\_rules** file and entries in a **\_list** file can be made
conditional. They apply only to some voices. This can be useful to
specify different pronunciations for different variants of a language
(dialects or accents).

Conditional rules have   **?**   and a condition number at the start if
the line in the **\_rules** or **\_list** file. This means that the rule
only applies of that condition number is specified in a **dictrules**
line in the [voice file](voices.html).

If the rule starts with   **?!**   then the rule only applies if the
condition number is **not** specified in the voice file. eg.

```
   ?3     can't     kant    // only use this if the voice has:  dictrules 3
   ?!3    rather    rA:D3   // only use if the voice doesn't have:  dictrules 3
```

## Numbers and Character Names

### Letter names

The names of individual letters can be given either in the **\_rules**
or **\_list** file. Sometimes an individual letter is also used as a
word in the language and its pronunciation as a word differs from its
letter name. If so, it should be listed in the **\_list** file, preceded
by an underscore, to give the letter name (as distinct from its
pronunciation as a word). eg. in English:

```
   _a   eI
```

### Numbers

The operation the TranslateNumber() function is controlled by the
language's `langopts.numbers` option. This constructs spoken
numbers from fragments according to various options which can be set for
each language. The number fragments are given in the **\_list** file.

Symbol| Description
----- | -----------
\_0 to \_9      | The numbers 0 to 9                  
\_13            | etc. Any pronunciations which are needed for specific numbers in the range \_10 to \_99                  
\_2X  \_3X      | Twenty, thirty, etc., used to make  numbers 10 to 99                    
\_0C            | The word for "hundred"              
\_1C  \_2C      | Special pronunciation for one hundred, two hundred, etc., if needed.                             
\_1C0           | Special pronunciation (if needed) for 100 exactly                     
\_0M1           | The word for "thousand"             
\_0M2           | The word for "million"              
\_0M3           | The word for 1000000000             
\_1M1  \_2M1    | Special pronunciation for one thousand, two thousand, etc, if needed                              
\_0and          | Word for "and" when speaking numbers (eg. "two hundred and twenty").     
\_dpt           | Word spoken for the decimnal point/comma                         
\_dpt2          | Word spoken (if any) at the end of all the digits after a decimal point.                              

## Character Substitution

Character substitutions can be specified by using a **.replace**section
at the start of the **\_rules**file. Each line specified either one or
two alphabetic characters to be replaced by another one or two
alphabetic characters. This substitution is done to a word before it is
translated using the spelling-to-phoneme rules. Only the lower-case
version of the characters needs to be specified. eg.

```
  .replace
     ô   ő   // (Hungarian) allow the use of o-circumflex instead of o-double-accute
     û   ű
    cx   ĉ   // (Esperanto) allow "cx" as an alternative to c-circumflex
    ﬁ   fi   // replace a single character ligature by two characters
```

