PHONEME TABLES {.western}
--------------

A phoneme table defines all the phonemes which are used by a language,
together with their properties and the data for their production as
sounds.

Generally each language has its own phoneme table, although additional
phoneme tables can be used for different voices within the language.
These alternatives are referenced from Voice files.

A phoneme table does not need to define all the phonemes used by a
language. It can inherit the phonemes from a previously defined phoneme
table. For example, a phoneme table may redefine (or add) some of the
vowels that it uses, but inherit most of its consonants from a standard
set.

The source files for the phoneme data are in the "phsource" directory in
the espeakedit download package. "Vowel files", which are referenced in
FMT(), VowelStart(), and VowelEnding() instructions are made using the
espeakedit program.

### Phoneme files {.western}

The phoneme tables are defined in a master phoneme file, named
**phonemes**. This starts with the **base** phoneme table followed by
phoneme tables for other languages and voices. These inherit phonemes
from the **base** table or previously defined tables.

In addition to phoneme definitions, the phoneme file can contain the
following:

**include** \<filename\> 
:   Includes the text of the specified file at this point. This allows
    different phoneme tables to be kept in different text files, for
    convenience. \<filename\> is a relative path. The included file can
    itself contain **include** statements.
 **phonemetable** \<name\> \<parent\> 
:   Starts a new phoneme table, and ends the previous table.\
     \<name\> Is the name of this phoneme table. This name is used in
    Voice files.\
     \<parent\> Is the name of a previously defined phoneme table whose
    phoneme definitions are inherited by this one. The name **base**
    indicates the first (base) phoneme table.

### Phoneme definitions {.western}

Note: These new Phoneme definitions apply to eSpeak version 1.42.20 and
later.

A phoneme table contains a list of phoneme definitions. Each starts with
the keyword **phoneme** and the phoneme name (this is the name used in
the pronunciation rules in a language's \*\_rules and \*\_list files),
and ends with the keyword **endphoneme**. For example:

~~~~ {.western}
  phoneme aI
    vowel
    starttype #a endtype #i
    length 230
    FMT(vowels/ai)
  endphoneme

  phoneme s
    vls alv frc sibilant
    voicingswitch z
    lengthmod 3
    Vowelin  f1=0  f2=1700 -300 300  f3=-100 80
    Vowelout f1=0  f2=1700 -300 250  f3=-100 80  rms=20

    IF nextPh(isPause) THEN
      WAV(ufric/s_)
    ELIF nextPh(p) OR nextPh(t) OR nextPh(k) THEN
      WAV(ufric/s!)
    ENDIF
    WAV(ufric/s)
  endphoneme
~~~~

A phoneme definition contains both static properties and executed
instructions. The instructions may contain conditional statements, so
that the effect of the phoneme may be different depending on adjacent
phonemes, whether the syllable is stressed, etc.

The instructions of a phoneme are interpreted in two different phases.
In the first phase, the instructions may change the phoneme and replace
it by a different phoneme. In the second phase, instructions are used to
produce the sound for the phoneme.

The **import\_phoneme** statement can be used to copy a previously
defined phoneme from a specified phoneme table. For example:

~~~~ {.western}
  phoneme t
    import_phoneme base/t[
  endphoneme 
~~~~

means: `phoneme t`{.western} in this phoneme table is a copy of
`phoneme t[`{.western} from phoneme table "base". A **length**
instruction can be used after **import\_phoneme** to vary the length
from the original.

### Phoneme Properties {.western}

Within the phoneme definition the following lines may occur: ( (V)
indicates only for vowels, (C) only for consonants)

### Phoneme Instructions {.western}

Phoneme Instructions may be included within conditional statements.

During the first phase of phoneme interpretation, an instruction which
causes a change to a different phoneme will terminate the instructions.
During the second phase, FMT() and WAV() instructions will terminate the
instructions.

### Conditional Statements {.western}

Phoneme definitions can contain conditional statements such as:

~~~~ {.western}
  IF <condition> THEN
    <statements>
  ENDIF
~~~~

or more generally:

~~~~ {.western}
  IF <condition> THEN
    <statements>
  ELIF <condition> THEN
    <statements>
  ...
  ELSE
    <statements>
  ENDIF
~~~~

where the `ELSE`{.western} and multiple `ELSE`{.western} parts are
optional.

Multiple conditions may be joined with `AND`{.western} or
`OR`{.western}, but not a mixture of `AND`{.western}s and
`OR`{.western}s.

A condition may be preceded by `NOT`{.western}. For example:

~~~~ {.western}
  IF <condition> AND NOT <condition> THEN
    <statements>
  ENDIF
~~~~

**Condition** Can be:

**Attributes**

### Sound Specifications {.western}

There are three ways to produce sounds:

-   -   -   

### Vowel Transitions {.western}

These specify how a consonant affects an adjacent vowel. A consonant may
cause a transition in the vowel's formants as the mouth changes shape
between the consonant and the vowel. The following attributes may be
specified. Note that the maximum rate of change of formant frequencies
is limited by the speak program.


