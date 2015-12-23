# Table of contents

  * [Espeakedit program](#espeakedit-program)
    * [Installation](#installation)
    * [Quick Guide](#quick-guide)
      * [Compiling Phoneme Data](#compiling-phoneme-data)
      * [Keyframe Sequences](#keyframe-sequences)
      * [Text and Prosody Windows](#text-and-prosody-windows)

# Espeakedit program

The **espeakedit** program is used to prepare phoneme data for the eSpeak speech synthesizer.

It has two main functions:

*   Prepare keyframe files for individual vowels and voiced consonants. These each contain a sequence of keyframes which define how formant peaks (peaks in the frequency spectrum) vary during the sound.
*   Process the master **phonemes** file which, by including the phoneme files for the various languages, defines all their phonemes and references the keyframe files and the sound sample files which they use. **espeakedit** processes these and compiles them into the **phondata**, **phonindex**, and **phontab** files in the **espeak-data** directory which are used by the eSpeak speech synthesizer.


## Installation

**espeakedit** needs the following packages:  
(The package names mentioned here are those from the Ubuntu "Dapper" Linux distribution).

*   **sox**   (a universal sound sample translator)
*   **libwxgtk2.6-0**   (wxWidgets Cross-platform C++ GUI toolkit)
*   **portaudio0**   (Portaudio V18, portable audio I/O)

In addition, a modified version of **praat** ([www.praat.org](http://www.praat.org/)) is used to view and analyse WAV sound files. This needs the package **libmotif3** to run and **libmotif-dev** to compile.

## Quick Guide

This will quickly illustrate the main features. Details of the interface and key commands are given in [editor_if](editor_if.md)

For more detailed information on analysing sound recordings and preparing phoneme definitions and keyframe data see [analyse](analyse.md). 

### Compiling Phoneme Data

1.  Run the `espeakedit` program.
2.  Select **Data->Compile phoneme data** from the menu bar. Dialog boxes will ask you to locate the directory (`phsource`) which contains the master phonemes file, and the directory (`dictsource,`) which contains the dictionary files (en_rules, en_list, etc). Once specified, espeakedit will remember their locations, although they can be changed later from **Options->Paths**.
3.  A message in the status line at the bottom of the espeakedit window will indicate whether there are any errors in the phoneme data, and how many language's dictionary files have been compiled. The compiled data is placed into the `espeak-data` directory, ready for use by the speak program. If errors are found in the phoneme data, they are listed in a file `error_log` in the `phsource` directory.

    NOTE: espeakedit can be used from the command line to compile the phoneme data, with the command:

    `espeakedit --compile`

5.  Select **Tools->Make vowels chart->From compiled phoneme data**. This will look for the vowels in the compiled phoneme data of each language and produce a vowel chart (.png file) in `phsource/vowelcharts`. These charts plot the vowels' F1 (formant 1) frequency against their F2 frequency, which corresponds approximately to their open/close and front/back positions. The colour in the circle for each vowel indicates its F3 frequency, red indicates a low F3, through yellow and green to blue and violet for a high F3\. In the case of a diphthong, a line is drawn from the circle to the position of the end of the vowel.

### Keyframe Sequences

1.  Select **File->Open** from the menu bar and select a vowel file, `phsource/vowel/a`. This will open a tab in the espeakedit window which contains a sequence of 4 keyframes. Each keyframe shows a black graph, which is the outline of an original analysed spectrum from a sound recording, and also a green line, which shows the formant peaks which have been added (using the black graph as a guide) and which produce the sound.
2.  Click in the "a" tab window and then press the **F2** key. This will produce and play the sound of the keyframe sequence. The first time you do this, you'll get a save dialog asking where you want the WAV file to be saved. Once you give a location all future sounds will be stored in that same location, although it can be changed from **Options->Paths**.
3.  Click on the second of the four frames, the one with the red square. Press **F1**. That plays the sound of just that frame.
4.  Press the **1** (number one) key. That selects formant F1 and a red triangle appears under the F1 formant peak to indicate that it's selected. Also an = sign appears next to formant 1 in the formants list in the left panel of the window.
5.  Press the left-arrow key a couple of times to move the F1 peak to the left. The red triangle and its associated green formant peak moves lower frequency. Its numeric value in the formants list in the left panel decreases.
6.  Press the **F1** key again. The frame will give a slightly different vowel sound. As you move the F1 peak slightly up and down and then press **F1** again, the sound changes. Similarly if you press the **2** key to select the F2 formant, then moving that will also change the sound. If you move the F1 peak down to about 700 Hz (and reduce its height a bit with the down-arrow key) and move F2 up to 1400 Hz, then you'll hear a "er" schwa [@] sound instead of the original [a].
7.  Select **File->Open** and choose `phsource/vowel/aI`. This opens a new tab labelled "aI" which contains more frames. This is the [aI] diphthong and if you click in the tab window and press **F2** you'll hear the English word "eye". If you click on each frame in turn and press **F1** then you can hear each of the keyframes in turn. They sound different, starting with an [A] sound (as in "palm"), going through something like [@] in "her" and ending with something like [I] in "kit" (or perhaps a French é). Together they make the diphthong [aI].

### Text and Prosody Windows

1.  Click on the **Text** tab in the left panel. Two text windows appear in the panel with buttons **Translate** and **Speak** below them.
2.  Type some text into the top window and click the **Translate** button. The phonetic translation will appear in the lower window.
3.  Click the **Speak** button. The text will be spoken and a **Prosody** tab will open in the main window.
4.  Click on a vowel phoneme which is displayed in the Prosody tab. A red line appears under it to indicate that it has been selected.
5.  Use the **up-arrow** or **down-arrow** key to move the vowel's blue pitch contour up or down. Then click the **Speak** button again to hear the effect of the altered pitch. If the adjacent phoneme also has a pitch contour then you may hear a discontinuity in the sound if it no longer matches with the one which you have moved.
6.  Hold down the **Ctrl** key while using the **up-arrow** or **down-arrow** keys. The gradient of the pitch contour will change.
7.  Click with the right mouse button over a phoneme. A menu allows you to select a different pitch envelope shape. Details of the currently selected phoneme appear in the Status line at the bottom of the window. The **Stress** number gives the stress level of the phoneme (see voices.html for a list).
8.  Click the **Translate** button. This re-translates the text and restores the original pitches.
9.  Click on a vowel phoneme in the Prosody window and use the **<** and **>** keys to shorten or lengthen it.

The Prosody window can be used to experiment with different phoneme lengths and different intonation.

