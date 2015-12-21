PHONEMES {.western}
--------

In general a different set of phonemes can be defined for each language.

In most cases different languages inherit the same basic set of
consonants. They can add to these or modify them as needed.

The phoneme mnemonics are based on the scheme by Kirshenbaum which
represents International Phonetic Alphabet symbols using ascii
characters. See:
[www.kirshenbaum.net/IPA/ascii-ipa.pdf](http://www.kirshenbaum.net/IPA/ascii-ipa.pdf).

Phoneme mnemonics can be used directly in the text input to
**espeak-ng**. They are enclosed within double square brackets. Spaces
are used to separate words, and all stressed syllables must be marked
explicitly. eg:\
 `[[D,Is Iz sVm f@n'EtIk t'Ekst 'InpUt]]`{.western}

### English Consonants {.western}

`[p]`{.western}

`[b]`{.western}

`[t]`{.western}

`[d]`{.western}

`[tS]`{.western}

**ch**urch

`[dZ]`{.western}

**j**udge

`[k]`{.western}

`[g]`{.western}

`[f]`{.western}

`[v]`{.western}

`[T]`{.western}

**th**in

`[D]`{.western}

**th**is

`[s]`{.western}

`[z]`{.western}

`[S]`{.western}

**sh**op

`[Z]`{.western}

plea**s**ure

`[h]`{.western}

`[m]`{.western}

`[n]`{.western}

`[N]`{.western}

si**ng**

`[l]`{.western}

`[r]`{.western}

**r**ed (Omitted if not immediately followed by a vowel).

`[j]`{.western}

**y**es

`[w]`{.western}

**Some Additional Consonants**

\

`[C]`{.western}

German i**ch**

`[x]`{.western}

German bu**ch**

`[l^]`{.western}

Italian **gl**i

`[n^]`{.western}

Spanish **ñ**

### English Vowels {.western}

These are the phonemes which are used by the English spelling-to-phoneme
translations (en\_rules and en\_list). In some varieties of English
different phonemes may have the same sound, but they are kept separate
because they may differ in another variety.

In rhotic accents, such as General American, the phonemes
`[3:], [A@], [e@], [i@], [O@], [U@] `{.western}include the "r" sound.

`[@]`{.western}

alph**a**

schwa

`[3]`{.western}

bett**er**

rhotic schwa. In British English this is the same as `[@]`{.western},
but it includes 'r' colouring in American and other rhotic accents. In
these cases a separate `[r]`{.western} should not be included unless it
is followed immediately by another vowel.

`[3:]`{.western}

n**ur**se

`[@L]`{.western}

simp**le**

`[@2]`{.western}

the

Used only for "the".

`[@5]`{.western}

to

Used only for "to".

`[a]`{.western}

tr**a**p

`[aa]`{.western}

b**a**th

This is `[a]`{.western} in some accents, `[A:]`{.western} in others.

`[a#]`{.western}

**a**bout

This may be `[@]`{.western} or may be a more open schwa.

`[A:]`{.western}

p**al**m

`[A@]`{.western}

st**ar**t

`[E]`{.western}

dr**e**ss

`[e@]`{.western}

squ**are**

`[I]`{.western}

k**i**t

`[I2]`{.western}

**i**ntend

As `[I]`{.western}, but also indicates an unstressed syllable.

`[i]`{.western}

happ**y**

An unstressed "i" sound at the end of a word.

`[i:]`{.western}

fl**ee**ce

`[i@]`{.western}

n**ear**

`[0]`{.western}

l**o**t

`[V]`{.western}

str**u**t

`[u:]`{.western}

g**oo**se

`[U]`{.western}

f**oo**t

`[U@]`{.western}

c**ure**

`[O:]`{.western}

th**ou**ght

`[O@]`{.western}

n**or**th

`[o@]`{.western}

f**or**ce

`[aI]`{.western}

pr**i**ce

`[eI]`{.western}

f**a**ce

`[OI]`{.western}

ch**oi**ce

`[aU]`{.western}

m**ou**th

`[oU]`{.western}

g**oa**t

`[aI@]`{.western}

sc**ie**nce

`[aU@]`{.western}

h**our**

### Some Additional Vowels {.western}

Other languages will have their own vowel definitions, eg:

+--------------------------------------+--------------------------------------+
| `[e]`{.western}                      | German **eh**, French **é**          |
+--------------------------------------+--------------------------------------+
| `[o]`{.western}                      | German **oo**, French **o**          |
+--------------------------------------+--------------------------------------+
| `[y]`{.western}                      | German **ü**, French **u**           |
+--------------------------------------+--------------------------------------+
| `[Y]`{.western}                      | German **ö**, French **oe**          |
+--------------------------------------+--------------------------------------+

`[:] `{.western}can be used to lengthen a vowel, eg `[e:]`{.western}
