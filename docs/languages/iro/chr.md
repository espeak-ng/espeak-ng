
=== Western Cherokee

Cherokee is a tonal language which does not use stress.

This is an initial attempt at supporting Cherokee "DF" phonetics. (See Cherokee English Dictionary - Feeling - 1972). It does not support Syllabary or transliterated/romanized texts, neither of which indicate tone or cadence.

The language file is configured to use stress on all syllables to mimic having no stress by having the stress equal across all syllables.

Generally, long vowels are twice as long as their short counterparts.

Trying to get espeak-ng's output to have the correct range on the various pitch contours has been difficult as documentation on how to do this is scarce. 

Note: The final "High Fall" tone generally found at the end of words doesn't always start high enough and never falls low enough. My attempts at increasing the range of the pitch contour for final word vowels shows that specifying too large a range in a phonemes tone statement causes some sort of disconnect between what one is trying to specify and what is actually generated. (At least by analysis using the Praat software).

I have also attempted to enforce a zero intonation sentence structure to try and prevent any built-in intonation rules from slaughtering the pitch contours of syllables because of their locations.

In general, the pitch contours seem to come out sounding "flat" and "constricted" when compared with the pitch contours from actual human speech. I do not know how to fix this.

This language should be considered as alpha level *testing* and is **not** production ready.

TODO:

* Devoice short vowels when followed by an 'h'.
* Fix where the 'm' and 'n' consonants are hard to perceive.
* Add rules to handle the C.E.D. like phonetics as used by the Raven Rock Dictionary.
* Add a basic Eastern Cherokee dialect to account for differences of pronuncations of 's vs sh' and 'j/ch vs ts'.
* Figure out why ',' is treated as part of a word and not a short pause and fix it.
