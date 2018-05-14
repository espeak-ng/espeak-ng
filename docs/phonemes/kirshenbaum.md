# Kirshenbaum (ASCII-IPA) Transcription Scheme

- [Phoneme Transcription Schemes](#phoneme-transcription-schemes)
- [Consonants](#consonants)
  - [Other Symbols](#other-symbols)
- [Vowels](#vowels)
  - [Other Symbols](#other-symbols-1)
- [Diacritics](#diacritics)
- [Suprasegmentals](#suprasegmentals)
  - [Stress](#stress)
  - [Length](#length)
- [References](#references)

----------

The Kirshenbaum (ASCII-IPA) transcription scheme was created by Evan
Kirshenbaum and others between 1991 and 1993 for use in Usenet and e-mail.
Since then, the support for Unicode has greatly improved. An ASCII encoding
of IPA is still useful as it is often easier to type.

The Kirshenbaum transcription scheme defines a set of phoneme features that are
described in eSpeak NG in the [phonemes](phonemes.md) document. The `cainteoir`
and `espeak-ng` phoneme features defined in that document are not useable in
transcriptions using the Kirshenbaum transcription scheme. They are used here
where relevant to define the Kirshenbaum phonemes in a consistent way across
the different transcription schemes.

Broad (phonemic) transcriptions are enclosed in `/` characters (`/p/`), while
narrow (phonetic) transcriptions are wrapped in `[` and `]` characters (`[p]`).

A phoneme can be transcribed as a named phoneme (`p`), or as a set of features
wrapped in `{` and `}` characters (`{vls,blb,stp}`). A feature (typically a
diacritic) of a phoneme can be added or modified by wrapping the feature in `<`
and `>` characters after the base phoneme (`p<dnt>`).

## Phoneme Transcription Schemes

| BCP47 Subtag               | Abbreviation | Transcription Scheme                                 | Encoding |
|----------------------------|--------------|------------------------------------------------------|----------|
| [`fonipa`](../phonemes.md) | IPA          | International Phonetic Alphabet                      | Unicode  |
| [`fonxsamp`](xsampa.md)    | X-SAMPA      | Extended Speech Assessment Methods Phonetic Alphabet | ASCII    |
| [`x-foncxs`](cxs.md)       | CXS          | Conlang X-SAMPA                                      | ASCII    |
| `x-fonkirsh`               |              | Kirshenbaum (ASCII-IPA)                              | ASCII    |

1. `foncxs` and `fonkirsh` are private use extensions defined in
   the [bcp47-extensions](https://raw.githubusercontent.com/espeak-ng/bcp47-data/master/bcp47-extensions)
   file, so have the `x-` private use specifier before their subtag names.

## Consonants

|                             | `blb`    |          | `lbd` |          | `dnt`     |           | `alv`    |          | `pla` |       | `rfx` |       | `alp` |       | `pal`    |          | `vel`    |          | `uvl`    |          | `phr` |          | `glt` |        |
|-----------------------------|----------|----------|-------|----------|-----------|-----------|----------|----------|-------|-------|-------|-------|-------|-------|----------|----------|----------|----------|----------|----------|-------|----------|-------|--------|
|                             | `vls`    | `vcd`    | `vls` | `vcd`    | `vls`     | `vcd`     | `vls`    | `vcd`    | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls`    | `vcd`    | `vls`    | `vcd`    | `vls`    | `vcd`    | `vls` | `vcd`    | `vls` | `vcd`  |
| `nas`                       |          | `m`      |       | `M`      |           | `n[`      |          | `n`      |       |       |       | `n.`  |       |       |          | `n^`     |          | `N`      |          | `n"`     |       |          |       |        |
| `stp`                       | `p`      | `b`      |       |          | `t[`      | `d[`      | `t`      | `d`      |       |       | `t.`  | `d.`  |       |       | `c`      | `J`      | `k`      | `g`      | `q`      | `G`      |       |          | `?`   |        |
| `sib`&#xA0;`afr`            |          |          |       |          |           |           |          |          |       |       |       |       |       |       |          |          |          |          |          |          |       |          |       |        |
| `afr`                       |          |          |       |          |           |           |          |          |       |       |       |       |       |       |          |          |          |          |          |          |       |          |       |        |
| `lat`&#xA0;`afr`            |          |          |       |          |           |           |          |          |       |       |       |       |       |       |          |          |          |          |          |          |       |          |       |        |
| `sib`&#xA0;`frc`            |          |          |       |          |           |           | `s`      | `z`      | `S`   | `Z`   | `s.`  | `z.`  |       |       |          |          |          |          |          |          |       |          |       |        |
| `frc`                       | `P`      | `B`      | `f`   | `v`      | `T`       | `D`       |          |          |       |       |       |       |       |       | `C`      | `C<vcd>` | `x`      | `Q`      | `X`      | `g"`     | `H`   | `H<vcd>` | `h`   | `h<?>` |
| `lat`&#xA0;`frc`            |          |          |       |          |           |           | `s<lat>` | `z<lat>` |       |       |       |       |       |       |          |          |          |          |          |          |       |          |       |        |
| `apr`                       |          |          |       | `r<lbd>` |           | `r[`      |          | `r`      |       |       |       | `r.`  |       |       |          | `j`      |          | `j<vel>` |          | `g"`     |       |          |       |        |
| `lat`&#xA0;`apr`            |          |          |       |          |           | `l[`      |          | `l`      |       |       |       | `l.`  |       |       |          | `l^`     |          | `L`      |          |          |       |          |       |        |
| `flp`                       |          |          |       |          |           |           |          | `*`      |       |       |       | `*.`  |       |       |          |          |          |          |          |          |       |          |       |        |
| `lat`&#xA0;`flp`            |          |          |       |          |           |           |          | `*<lat>` |       |       |       |       |       |       |          |          |          |          |          |          |       |          |       |        |
| `trl`                       |          | `b<trl>` |       |          |           |           |          | `r<trl>` |       |       |       |       |       |       |          |          |          |          |          | `r"`     |       |          |       |        |
| `clk`                       | `p!`     |          |       |          | `t!`      |           | `c!`     |          |       |       |       |       |       |       | `c!`     |          | `k!`     |          |          |          |       |          |       |        |
| `lat`&#xA0;`clk`            |          |          |       |          |           |           | `l!`     |          |       |       |       |       |       |       |          |          |          |          |          |          |       |          |       |        |
| `imp`                       |          | `` b` `` |       |          |           | `` d` ``  |          | `` d` `` |       |       |       |       |       |       |          | `` J` `` |          | `` g` `` |          | `` G` `` |       |          |       |        |
| `ejc`                       | `` p` `` |          |       |          | `` t[` `` |           | `` t` `` |          |       |       |       |       |       |       | `` c` `` |          | `` k` `` |          | `` q` `` |          |       |          |       |        |
| `ejc`&#xA0;`frc`            |          |          |       |          |           |           |          |          |       |       |       |       |       |       |          |          |          |          |          |          |       |          |       |        |
| `lat`&#xA0;`ejc`&#xA0;`frc` |          |          |       |          |           |           |          |          |       |       |       |       |       |       |          |          |          |          |          |          |       |          |       |        |

1. `h` is transcribed as `{glt,apr}` in ASCII-IPA. It is transcribed as
   `{glt,frc}` in this document to match the assignment used in the
   Wikipedia IPA table, and the placement in other documents in eSpeak NG.
   The IPA chart places them as being `{frc}` or `{apr}`, as does the main
   Wikipedia page for the `[h]` phoneme.

1. The `` q` `` phoneme is incorrectly placed in the segment table (Appendix D)
   of the ASCII-IPA document. According to the diacritics list (Appendix C),
   the `` ` `` character denotes an `{ejc}` phoneme for a `{vls}` base
   phoneme, not `{imp}` which is used for `{vcd}` base phonemes. The table
   above uses the correct assignment.

1. The `L` phoneme has an alternate meaning in ASCII-IPA. This document only
   describes the variant preferred by Evan Kirshenbaum.

### Other Symbols

|                             | `bld` |       | `alv` |       | `pla` |       | `pal` |       | `lbv`    |          | `vel`    |       |
|-----------------------------|-------|-------|-------|-------|-------|-------|-------|-------|----------|----------|----------|-------|
|                             | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls` | `vcd` | `vls`    | `vcd`    | `vls`    | `vcd` |
| `nas`                       |       |       |       |       |       |       |       |       |          | `n<lbv>` |          |       |
| `stp`                       |       |       |       |       |       |       |       |       | `t<lbv>` | `d<lbv>` |          |       |
| `afr`                       |       |       |       |       |       |       |       |       |          |          |          |       |
| `vzd`&#xA0;`frc`            |       |       |       |       |       |       |       |       |          |          |          |       |
| `ptr`&#xA0;`apr`            |       |       |       |       |       |       |       |       |          |          | `w<vls>` | `w`   |
| `fzd`&#xA0;`lat`&#xA0;`apr` |       |       |       |       |       |       |       |       |          |          |          |       |

1. The `w` phoneme is transcribed as being `{vcd,lbv,frc}` or `{lbv,apr}`, and
   `w<vls>` as `{vls,lbv,frc}` in ASCII-IPA. They are transcribed as
   `{ptr,vel,apr}` in this document to match their definition as labialized
   velar approximants in the IPA chart.

## Vowels

|        | `fnt` |       | `cnt`    |       | `bck` |       |
|--------|-------|-------|----------|-------|-------|-------|
|        | `unr` | `rnd` | `unr`    | `rnd` | `unr` | `rnd` |
| `hgh`  | `i`   | `y`   | `i"`     | `u"`  | `u-`  | `u`   |
| `smh`  | `I`   | `I.`  |          |       |       | `U`   |
| `umd`  | `e`   | `Y`   | `@<umd>` |       | `o-`  | `o`   |
| `mid`  |       |       | `@`      | `@.`  |       |       |
| `lmd`  | `E`   | `W`   | `V"`     | `O"`  | `V`   | `O`   |
| `sml`  | `&`   |       |          |       |       |       |
| `low`  | `a`   | `a.`  |          |       | `A`   | `A.`  |

1. The `&` phoneme is transcribed as being `{low,fnt,unr,vwl}`, and `a` as
   `{low,cnt,unr,vwl}` in ASCII-IPA. This does not match their placement in
   the IPA chart according to the Unicode characters those ASCII-IPA
   phonemes map to. As such, the table above reflects their IPA chart
   positions.

1. The `&.` phoneme is defined in the segment table (Appendix D) of the
   ASCII-IPA definition. According to the IPA charts, this is not a valid
   phoneme. As such, it is not listed in the table above.

1. The `W` and `Y` phonemes have an alternate meaning in ASCII-IPA. This
   document only describes the variant preferred by Evan Kirshenbaum.

### Other Symbols

| Symbol   | Features                      |
|----------|-------------------------------|
| `R`      | `unr` `mid` `cnt` `rzd` `vwl` |
| `R<umd>` | `unr` `lmd` `cnt` `rzd` `vwl` |

1. The `R<umd>` phoneme is transcribed as being `{umd,cnt,unr,rzd,vwl}` in
   ASCII-IPA. However, the IPA phoneme it is representing is based on the
   `{lmd}` vowel. The table above keeps the `<umd>` modifying feature to be
   compatible with ASCII-IPA, but assigns it to `{lmd}` in this case to be
   compatible with the IPA chart.

1. The `R` phoneme has an alternate meaning in ASCII-IPA. This document only
   describes the variant preferred by Evan Kirshenbaum.

## Diacritics

| Symbol | Feature |
|--------|---------|
| `!`    | `{clk}` |
| `[`    | `{dnt}` |
| `;`    | `{pzd}` |
| `^`    | `{pal}` |
| `<H>`  | `{fzd}` |
| `<h>`  | `{asp}` |
| `<o>`  | `{unx}` |
| `<r>`  | `{rzd}` |
| `<w>`  | `{ptr}` |
| `<?>`  | `{brv}` |

1. The `<o>` diacritic has an alternate meaning in ASCII-IPA. This document
   only describes the variant preferred by Evan Kirshenbaum.

1. The `{ptr}` (protruded) feature is specified as `{lzd}` (labialized) in
   ASCII-IPA.

1. The `{brv}` (breathy voice) feature is specified as `{mrm}` (murmured)
   in ASCII-IPA.

The ASCII-IPA transcription scheme defines several diacritics that vary in
meaning depending on whether the base phoneme is a vowel or consonant.

| Symbol | Vowel   | Consonant |
|--------|---------|-----------|
| `~`    | `{nzd}` | `{vzd}`   |
| `-`    | `{unr}` | `{syl}`   |
| `.`    | `{rnd}` | `{rfx}`   |
| `"`    | `{cnt}` | `{uvl}`   |

The ASCII-IPA transcription scheme defines several diacritics that vary in
meaning depending on the voicing of the base phoneme.

| Symbol  | Voiceless | Voiced  |
|---------|-----------|---------|
| `` ` `` | `{ejc}`   | `{imp}` |

## Suprasegmentals

### Stress

| Symbol | Name             |
|--------|------------------|
| `'`    | primary stress   |
| `,`    | secondary stress |

### Length

| Symbol | Feature |
|--------|---------|
| `:`    | `{lng}` |

## References

1. <a name="ref1"></a> Kirshenbaum, Evan,
   [Representing IPA phonetics in ASCII](https://web.archive.org/web/20160419125856/http://www.kirshenbaum.net/IPA/faq.html) (HTML).
   <time datetime="1993-01-04T00:00">1993</time>. Updated <time datetime="2003-04-30T00:00">2003</time>.

2. <a name="ref2"></a> Kirshenbaum, Evan,
   [Representing IPA phonetics in ASCII](https://web.archive.org/web/20160419125856/http://www.kirshenbaum.net/IPA/ascii-ipa.pdf) (PDF).
   <time datetime="2001-02-22T00:00">2001</time>. Updated <time datetime="2011-09-06T00:00">2011</time>.
