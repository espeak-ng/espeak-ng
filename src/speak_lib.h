/***************************************************************************
 *   Copyright (C) 2006 by Jonathan Duddington                             *
 *   jsd@clara.co.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


/*************************************************************/
/* This is the header file for the library version of espeak */
/*                                                           */
/*  Draft: 10.Jul.2006                                       */
/*************************************************************/


         /********************/
         /*  Initialization  */
         /********************/


typedef enum {
	espeakEVENT_WORD = 1,
	espeakEVENT_SENTENCE,
	espeakEVENT_MARK,
	espeakEVENT_PLAY
} espeak_EVENT_TYPE;

typedef struct {
	int type;
	int text_position;    // the number of characters from the start of the text
	int length;           // word length, in characters (for espeakEVENT_WORD)
	int audio_position;   // the time in mS within the generated speech output data
	union {
		int number;        // used for WORD and SENTENCE events
		const char *name;  // used for MARK and PLAY events.  UTF8 string
	} id;
} espeak_EVENT;
/* The event list is terminated list by an entry with type=0
   A MARK event indicates a <mark> element in the text.
   A PLAY event indicates an <audio> element in the text, for which the calling program should play
   the named sound file.
*/



typedef enum {
	POS_CHARACTER = 1,
	POS_WORD,
	POS_SENTENCE
} espeak_POSITION_TYPE;


int espeak_Initialize(int buflength);
/* Must be called before any synthesis functions are called.
   buflength:  The length in mS of sound buffers passed to the SynthCallback function.
   Returns: sample rate in Hz
*/

void espeak_SetSynthCallback(int (* SynthCallback)(short*, int, espeak_EVENT*));
/* Must be called before any synthesis functions are called.
   This specifies a function in the calling program which is called when a buffer of
   speech sound data has been produced.

   The callback function is of the form:

int SynthCallback(short *wav, int numsamples, espeak_EVENT *events);

   wav:  is the speech sound data which has been produced.
      NULL indicates that the synthesis has been completed.

   numsamples: is the number of entries in wav.  This number may vary, may be less than
      the value implied by the buflength parameter given in espeak_Initialize, and may
      sometimes be zero (which does NOT indicate end of synthesis).

   events: an array of espeak_EVENT items which indicate word and sentence events, and
      also the occurance if <mark> and <audio> elements within the text.

   Returns: 0=continue synthesis,  1=abort synthesis.
*/


void espeak_SetUriCallback(int (*UriCallback)(int, const char*, const char*));
/* This function must be called before synthesis functions are used, in order to deal with
   <audio> tags.  It specifies a callback function which is called when an <audio> element is
   encountered and allows the calling program to indicate whether the sound file which
   is specified in the <audio> element is available and is to be played.

   The callback function is of the form:

int UriCallback(int type, const char *uri, const char *base);

   type:  type of callback event.  Currently only 1= <audio> element

   uri:   the "src" attribute from the <audio> element

   base:  the "xml:base" attribute (if any) from the <speak> element

   Return: 1=don't play the sound, but speak the text alternative.
           0=place a PLAY event in the event list at the point where the <audio> element
             occurs.  The calling program can then play the sound at that point.
*/


         /********************/
         /*    Synthesis     */
         /********************/


#define espeakCHARS_AUTO   0
#define espeakCHARS_UTF8   1
#define espeakCHARS_8BIT   2
#define espeakCHARS_WCHAR  3

#define espeakSSML        0x10
#define espeakPHONEMES    0x100
#define espeakENDPAUSE    0x1000

int espeak_Synth(const void *text, unsigned int position, espeak_POSITION_TYPE position_type, unsigned int end_position, unsigned int flags);
/* Synthesize speech for the specified text.  The speech sound data is passed to the calling
   program in buffers by means of the callback function specified by espeak_SetSynthCallback()

   text: The text to be spoken, terminated by a zero character. It may be either 8-bit characters,
      wide characters (wchar_t), or UTF8 encoding.  Which of these is determined by the "flags"
      parameter.

   position:  The position in the text where speaking starts. Zero indicates speak from the
      start of the text.

   position_type:  Determines whether "position" is a number of characters, words, or sentences.
      Values: 

   end_position:  If set, this gives a character position at which speaking will stop.  A value
      of zero indicates no end position.

   flags:  These may be OR'd together:
      Type of character codes, one of:
         espeakCHARS_UTF8     UTF8 encoding
         espeakCHARS_8BIT     The 8 bit ISO-8859 character set for the particular language.
         espeakCHARS_AUTO     8 bit or UTF8  (this is the default)
         espeakCHARS_WCHAR    Wide characters (wchar_t)

      espeakSSML   Elements within < > are treated as SSML elements, or if not recognised are ignored.

      espeakPHONEMES  Text within [[ ]] is treated as phonemes codes (in espeak's Hirschenbaum encoding).

      espeakENDPAUSE  If set then a sentence pause is added at the end of the text.  If not set then
         this pause is suppressed.
*/

int espeak_Synth_Mark(const void *text, const char *index_mark, unsigned int end_position, unsigned int flags);
/* Synthesize speech for the specified text.  Similar to espeak_Synth() but the start position is
   specified by the name of a <mark> element in the text.

   index_mark:  The "name" attribute of a <mark> element within the text which specified the
      point at which synthesis starts.  UTF8 string.
*/


void espeak_Key(const char *key_name);
/* Speak the name of a keyboard key.
   Currently this just speaks the "key_name" as given */

void espeak_Char(wchar_t character);
/* Speak the name of the given character */

/* Note, there is no function to play a sound icon. This would be done by the calling program */



         /***********************/
         /*  Speech Parameters  */
         /***********************/


#define espeakRATE    1
#define espeakVOLUME  2
#define espeakPITCH   3
#define espeakRANGE   4
#define espeakPUNCTUATION  5
#define espeakCAPITALS  6

void espeak_SetParameter(int parameter, int value, int relative);
/* Sets the value of the specified parameter.
   relative=0   Sets the absolute value of the parameter.
   relative=1   Sets a relative value of the parameter.

   parameter:
      espeakRATE:    speaking speed in word per minute.

      espeakVOLUME:  volume in range 0-100    0=silence

      espeakPITCH:   base pitch in Hz

      espeakRANGE:   pitch range in Hz

      epeakPUNCTUATION:  which punctuation characters to announce:
         0=none,
         1=all,
         2=some, see espeak_GetParameter() to specify which characters are announced.

      espeakCAPITALS: announce capital letters by:
         0=none,
         1=sound icon,
         2=spelling,
         3 or higher, by raising pitch.  This values gives the amount in Hz by which the pitch
            of a word raised to indicate it has a capital letter.
*/

int espeak_GetParameter(int parameter, int current);
/* current=0  Returns the default value of the specified parameter.
   current=1  Returns the current value of the specified parameter, as set by SetParameter()
*/


void espeak_SetPunctuationList(wchar_t *punctlist);
/* Specified a list of punctuation characters whose names are to be spoken when the
   value of the Punctuation parameter is set to "some".

   punctlist:  A list of character codes, terminated by a zero character.
*/


void espeak_SetPhonemes(int value, FILE *stream);
/* Controls the output of phoneme symbols for the text
   value=0  No phoneme output (default)
   value=1  Output the translated phoneme symbols for the text
   value=2  as (1), but also output a trace of how the translation was done (matching rules and list entries)

   stream   output stream for the phoneme symbols (and trace).  If stream=NULL then it uses stdout.
*/


void espeak_CompileDictionary(const char *path, FILE *log);
/* Compile pronunciation dictionary for a language which corresponds to the currently
   selected voice.  The required voice should be selected before calling this function.

   path:  The directory which contains the language's '_rules' and '_list' files.
          'path' should end with a path separator character ('/').
   log:   Stream for error reports and statistics information. If log=NULL then stderr will be used.
*/
         /***********************/
         /*   Voice Selection   */
         /***********************/


// voice table
typedef struct {
	char *name;            // a given name for this voice. UTF8 string.
	char *languages;       // list of pairs of (byte) priority + (string) language (and dialect qualifier)
	char *identifier;      // the filename for this voice within espeak-data/voices
	unsigned char gender;  // 0=none 1=male, 2=female,
	unsigned char age;     // 0=not specified, or age in years
	unsigned char variant;  // only used when passed as a parameter to espeak_SetVoiceByProperties
	int score;   // for internal use
} espeak_VOICE;

/* Note: The espeak_VOICE structure is used for two purposes:
  1.  To return the details of the available voices.
  2.  As a parameter to  espeak_SetVoiceByName() in order to specify selection criteria.

   In (1), the "languages" field consists of a list of (UTF8) language names for which this voice
   may be used, each language name in the list is terminated by a zero byte and is also preceded by
   a single byte which gives a "priority" number.  The list of languages is terminated by an
   additional zero byte.

   A language name consists of a language code, optionally followed by one or more qualifier (dialect)
   names separated by hyphens (eg. "en-uk").  A voice might, for example, have languages "en-uk" and
   "en".  Even without "en" listed, voice would still be selected for the "en" language (because
   "en-uk" is related) but at a lower priority.

   The priority byte indicates how the voice is preferred for the language. A low number indicates a
   more preferred voice, a higher number indicates a less preferred voice.

   In (2), the "languages" field consists simply of a single (UTF8) language name, with no preceding
   priority byte.
*/

extern espeak_VOICE **espeak_ListVoices();
/* Reads the voice files from espeak-data/voices and creates an array or espeak_VOICE pointers.
   The list is terminated by a NULL pointer
*/

extern int espeak_SetVoiceByName(const char *name);
/* Searches for a voice with a matching "name" field.  Language is not considered.
   "name" is a UTF8 string.
*/

extern int espeak_SetVoiceByProperties(espeak_VOICE *voice_spec);
/* An espeak_VOICE structure is used to pass criteria to select a voice.  Any of the following
   fields may be set:

   name     NULL, or a voice name

   languages  NULL, or a single language string (with optional dialect), eg. "en-uk", or "en"

   gender   0=not specified, 1=male, 2=female

   age      0=not specified, or an age in years

   variant  After a list of candidates is produced, scored and sorted, "variant" is used to index
            that list and choose a voice.
            variant=0 takes the top voice (i.e. best match). variant=1 takes the next voice, etc
*/

extern espeak_VOICE *espeak_GetCurrentVoice(void);
/* Returns the espeak_VOICE data for the currently selected voice.
   This is not effected by temporary voice changes caused by SSML elements such as <voice> and <s>
*/

