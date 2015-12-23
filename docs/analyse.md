# Table of contents

  * [ANALYSIS](#analysis)
      * [Recording Sounds](#recording-sounds)
      * [Praat](#praat)
  * [Vowels and Diphthongs](#vowels-and-diphthongs)
      * [Analysing a Recording](#analysing-a-recording)
      * [Tone Quality](#tone-quality)
      * [Using an Existing Vowel File](#using-an-existing-vowel-file)
      * [Length and Amplitude](#length-and-amplitude)
      * [Using the New Vowel](#using-the-new-vowel)

# ANALYSIS

(Further notes are needed)

Recordings of spoken words and phrases can be analysed to try and make
eSpeak NG match a language more closely. Unlike most other (larger and
better quality) synthesizers, of eSpeak NG data is not produced directly
from recorded sounds. To use an analogy, it's like a drawing or sketch
compared with a photograph. Or vector graphics compared with a bitmap
image. It's smaller, less accurate, with less subtlety, but it can
sometimes show some aspects of the picture more clearly than a more
accurate image.

## Recording Sounds

Recordings should be made while speaking slowly, clearly, and firmly and
loudly (but not shouting). Speak about half a metre from the microphone.
Try to avoid background noise and hum interference from electrical power
cables.

## Praat

I use a modified version of the praat program
([www.praat.org](http://www.praat.org)) to view and analyse both sound
recordings and output from eSpeak NG. The modification adds a new function
(**Spectrum->To_eSpeak**) which analysis a voiced sound and
produces a file which can be loaded into espeakedit. Details of the
modification are in the `praat-mod` directory in the
espeakedit package. The analysis contains a sequence of frames, one per
cycle at the speech's fundamental frequency. Each frame is a short time
spectrum, together with praat's estimation of the f1 to f5 formant
frequencies at the time of that cycle. I also use Praat's
**New->Record_mono_sound** function to make sound recordings.

# Vowels and Diphthongs

## Analysing a Recording

Make a recording, with a male voice, and trim it in Praat to keep just
the required vowel sound. Then use the new
**Spectrum->To_eSpeak** modification (this was named
`To_Spectrogram2` in earlier versions) to analyse the sound.
It produces a file named `spectrum.dat`. Load the
`spectrum.dat` file into espeakedit. Espeakedit has two Open
functions, **File->Open**. They are
the same, except that they remember different paths. I generally use
**File->Open2** file.
The data is displayed in espeakedit as a sequence of spectrum frames
(see [editor](editor.md)).

## Tone Quality

It can be difficult to match the tonal quality of a new vowel to be
compatible with existing vowel files. This is determined by the relative
heights and widths of the formant peaks. These vary depending on how the
recording was made, the microphone, and the strength and tone of the
voice. Also the positions of the higher peaks (F3 upwards) can vary
depending on the characteristics of the speaker's voice. Formant peaks
correspond to resonances within the mouth and throat, and they depend on
its size and shape. With a female voice, all the formants (F1 upwards)
are generally shifted to higher frequencies. For these reasons, it's
best to use a male voice, and to use its analysed spectra only as
guidance. Rather than construct formant-peaks entirely to match the
analysed data, instead copy keyframes from a similar existing vowel.
Then make small adjustments to match the position of the F1, F2, F3
formant peaks and hopefully produce the required vowel sound.

## Using an Existing Vowel File

Choose a similar vowel file from `phsource/vowel` and open it
into espeakedit. It may be useful to use
`phsource/vowel/vowelchart` as a map to show how vowel files
compare with each other. You can select a keyframe from the vowel file
and use CTRL-C and CTRL-V to copy the green formant peaks onto a frame
of the new spectrum sequence. Then adjust the peaks to match the new
frame. Press F1 to hear the sound of the formant peaks in the selected
frame. The F0 peak is provided in order to adjust the correct balance of
low frequencies, below the F1 peak. If the sound is too muffled, or
conversely, too "thin", try adjusting the amplitude or position of the
F0 peak.

## Length and Amplitude

Use an existing vowel file as a guide for how to set the amplitude and
length of the keyframes. At the right of each keyframe, its length is
shown in mili seconds and under that is its relative (RMS) amplitude. The second
keyframe should be marked with a red marker (use CTRL-M to toggle this).
This divides the vowel into the front-part (with one frame), and the
rest. Use F2 to play the sound of the new vowel sequence. It will also
produce a WAV file (the default name is speech.wav) which you can read
into praat to see whether it has a sensible shape.

## Using the New Vowel

Make a new directory (eg. `vwl\_xx`) in phsource for your new vowels. Save
the spectrum sequence with a name which you have chosen for it. You can
then edit the phoneme file for your language (eg. `phsource/ph\_xxx`), and
change a phoneme to refer to your new vowel file. Then do
**Data->Compile_Phoneme_Data** from espeakedit's menubar to
re-compile the phoneme data.
