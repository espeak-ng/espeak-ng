# Table of contents

  * [User interface - formant editor](#user-interface---formant-editor)
    * [Frame Sequence Display](#frame-sequence-display)
    * [Text Tab](#text-tab)
    * [Spect Tab](#spect-tab)
    * [Key Commands](#key-commands)
      * [Selection](#selection)
      * [Formant movement](#formant-movement)
      * [Frame Cut and Paste](#frame-cut-and-paste)
      * [Frame editing](#frame-editing)
      * [Display and Sound](#display-and-sound)
  * [User interface - prosody editor](#user-interface---prosody-editor)

# User interface - formant editor

## Frame Sequence Display

The eSpeak editor can display a number of frame-sequencies in tabbed windows. Each frame can contain a short-time frequency spectrum, covering the period of one cycle at the sound's pitch. Frames can also show:

*   Blue vertical lines showing the estimated position of the f1 to f5 formants (if the sequence was produced by praat analysis). These should correspond with the peaks in the spectrum, but may not do so exactly
*   Numbers at the right side of the frame showing the position from the start of the sequence in miliseconds, and the pitch of the sound.
*   Up to 9 formant peaks (numbered 0 to 9) added by the user, usually to match the peaks in the spectrum, in order to produce the required sound. These are shown in green, can be moved by keyboard presses as described below, and may merge if they are close together. If a frame has formant peaks then it is a Keyframe and is shown with a pale yellow background.
*   If formant peaks are present, a relative amplitude (r.m.s.) value is shown at the right side of the frame.

## Text Tab

Enter text in the top left text window. Click the **Translate** button to see the phonetic transcription in the text window below. Then click the **Speak** button to speak the text and show the results in the **Prosody** tab, if that is open.

If changes are made in the **Prosody** tab, then clicking **Speak** will speak the modified prosody while **Translate** will revert to the default prosody settings for the text.

To enter phonetic symbols in [Kirschenbaum](https://en.wikipedia.org/wiki/Kirshenbaum)-like encoding in the top left text window, enclose them within **[[ ]]**.

## Spect Tab

* **Spect**  
  tab in the left panel of the eSpeak editor shows information about the currently selected frame and sequence.

* **Formants**  
  section displays the Frequency, Height, and Width of each formant peak (peaks 0 to 8). Peaks 6, 7, 8 don't have a variable width.

* **% amp - Frame**  
  can be used to adjust the amplitiude of the frame. If you change this value then the rms amplitude value at the right side of the frame will change.  
  The formant peaks don't change, just the overall amplitude of the frame.

* **mS**  
  shows the time in miliseconds until the next keyframe (or end of sequence if there is none).  
  The spin control initially shows the same value, but this can be changed in order to increase or decrease the effctive length of a keyframe.

* **% amp - Sequence**  
  adjusts the amplitude of the whole sequence. Changing this values changes the rms amplitudes of all the keyframes in the sequence.

* **% mS - Sequence**  
    shows the total length of the sequence.

* **Graph**  
    Yellow vertical lines show the position of keyframes within the sequence.  
    Black bars on these show the frequencies of formant peaks which have been set at these keyframes.  
    Thick red lines, if present, show the formants, as detected in the original analysis.  
    Thin black line, if present, shows the pitch profile measured in the original analysis.

## Key Commands

### Selection

The selected frame(s) are shown with a red border. The selected formant peak is also indicated by an equals (**=**) sign next to its number in the "Spect" panel to the right of the window.  
The selected formant peak is shown with a red triangle under the peak.  
Keyframes are shown with a pale yellow background. A keyframe is any frame with any formant peaks which are not zero height. If all formant peaks become zero height, the frame is no longer a keyframe. If you increase a peak's height the frame becomes a keyframe.

* **Numbers 0 to 8**  
  Select formant peak number 0 to 8.

* **Page Up/Down**  
  Move to next/previous frame

### Formant movement

With the following keys, holding down **Shift** causes slower movement.

* **Left**  
  Moves the selected formant peak to higher frequency.

* **Right**  
 Moves the selected formant peak to lower frequency.

* **Up**  
 Increases height of the selected formant peak.

* **Down**  
 Decreases height of the selected formant peak.

* **<**  
 Narrows the selected formant peak.

* **>**  
 Widens the selected formant peak.

* **CTRL <**  
 Narrows the selected formant peak.

* **CTRL >**  
 Widens the selected formant peak.

* **/**  
 Makes the selected formant peak symmetrical.

### Frame Cut and Paste

* **CTRL A**  
  Select all frames in the sequence.

*  **CTRL C**  
  Copy selected frames to (internal) clipboard.

* **CTRL V**  
  Paste frames from the clipboard to overwrite the contents of the selected frame and the frames which follow it. Only the formant peaks information is pasted.

* **CTRL SHIFT V**  
  Paste frames from the clippoard to insert them above the selected frame.

* **CTRL X**  
  Delete the selected frames.

### Frame editing

* **CTRL D**  
  Copy the formant peaks down to the selected frame from the next keyframe above.

* **CTRL SHIFT D**  
  Copy the formant peaks up to the selected frame from the next key-frame below.

* **CTRL Z**  
  Set all formant peaks in the selected frame to zero height. It is no longer a key-frame.

* **CTRL I**  
  Set the formant peaks in the selected frame as an interpolation between the next keyframes above and below it. A dialog box allows you to enter a percentage. 50% gives values half-way between the two adjacent key-frames, 0% gives values equal to the one above, and 100% equal to the one below.

### Display and Sound

* **CTRL Q**  
 Shows interpolated formant peaks on non-keyframes. These frames don't become keyframes until any of the peaks are edited to increase their height.

* **CTRL SHIFT Q**  
 Removes the interpolated formant peaks display.

* **CTRL G**  
 Toggle grid on and off.

* **F1**  
 Play sound made from the one selected keyframe.

* **F2**  
 Play sound made from all the keyframes in the sequence.

# User interface - prosody editor

* **Left**  
    Move to previous phoneme.

* **Right**  
    Move to next phoneme.

* **Up**  
    Increase pitch.

* **Down**  
    Decrease pitch.

* **Ctrl Up**  
    Increase pitch range.

* **Ctrl Down**  
    Decrease pitch range.

* **>**  
    Increase length.

* **<**  
    Decrease length.

