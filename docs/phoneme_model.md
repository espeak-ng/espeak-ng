# Phoneme Model

- [References](#references)

----------

Evan Kirshenbaum's feature set used in his ASCII transcription of the
International Phonetic Alphabet (IPA)<sup>\[<a href="#ref1">1</a>\],
\[<a href="#ref2">2</a>\]</sup> describes the phonemes in a way consistent with
how the phonemes are organised in the IPA code chart. That is the approach used
in the [Phonemes](phonemes.md) document to describe the phonemes in a phoneme
definition file.

Those phoneme features often represent the action of more than one articulatory
mechanism used to produce speech, or affect the same area. Internally, espeak-ng
makes use of the articulatory model, not the IPA descriptions. This document
describes how the feature-based IPA model is mapped to the articulatory model.

People working on adding new voices or languages do not need to read this
document, but should instead read the [Phonemes](phonemes.md) document. This is
intended for people working on the espeak-ng codebase, or people interested in
how espeak-ng works internally.

__NOTE:__ This model is in the process of being implemented. As such, the
current implementation does not reflect this document.

## References

1. <a name="ref1"></a> Kirshenbaum, Evan,
   [Representing IPA phonetics in ASCII](http://www.kirshenbaum.net/IPA/faq.html) (HTML). 1993.

2. <a name="ref2"></a> Kirshenbaum, Evan,
   [Representing IPA phonetics in ASCII](http://www.kirshenbaum.net/IPA/ascii-ipa.pdf) (PDF). 2001.

3. <a name="ref3"></a> International Phonetic Association,
   [The International Phonetic Alphabet and the IPA Chart](https://www.internationalphoneticassociation.org/content/ipa-chart). 2015.
   Creative Commons Attribution-Sharealike 3.0 Unported License (CC-BY-SA).

4. <a name="ref4"></a> Wikipedia.
   [International Phonetic Alphabet](https://en.wikipedia.org/wiki/International_Phonetic_Alphabet). 2017.
   Creative Commons Attribution-Sharealike 3.0 Unported License (CC-BY-SA).

5. <a name="ref7"></a> Wikipedia.
   [Extensions to the International Phonetic Alphabet](https://en.wikipedia.org/wiki/Extensions_to_the_International_Phonetic_Alphabet). 2017,
   Creative Commons Attribution-Sharealike 3.0 Unported License (CC-BY-SA).
