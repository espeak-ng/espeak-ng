/***************************************************************************
 *   Copyright (C) 2005 to 2007 by Jonathan Duddington                     *
 *   email: jonsd@users.sourceforge.net                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see:                                 *
 *               <http://www.gnu.org/licenses/>.                           *
 ***************************************************************************/

#include "StdAfx.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <wctype.h>
#include <wchar.h>

#include "speak_lib.h"
#include "speech.h"
#include "voice.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"

#define WORD_STRESS_CHAR   '*'


Translator *translator = NULL;    // the main translator
static Translator *translator2 = NULL;   // secondary translator for certain words
static char translator2_language[20] = {0};

FILE *f_trans = NULL;     // phoneme output text
int option_tone1 = 0;
int option_tone2 = 0;
int option_phonemes = 0;
int option_phoneme_events = 0;
int option_quiet = 0;
int option_endpause = 0;  // suppress pause after end of text
int option_capitals = 0;
int option_punctuation = 0;
int option_sayas = 0;
int option_sayas2 = 0;  // used in translate_clause()
int option_emphasis = 0;
int option_ssml = 0;
int option_phoneme_input = 1;  // allow [[phonemes]] in input
int option_phoneme_variants = 0;  // 0= don't display phoneme variant mnemonics

int count_sayas_digits;
int skip_sentences;
int skip_words;
int skip_characters;
char skip_marker[N_MARKER_LENGTH];
int skipping_text;   // waiting until word count, sentence count, or named marker is reached
int end_character_position;
int count_sentences;
int count_words;
int clause_start_char;
int clause_start_word;
int new_sentence;

int prev_clause_pause=0;
int max_clause_pause = 0;


wchar_t option_punctlist[N_PUNCTLIST]={0};
char ctrl_embedded = '\001';    // to allow an alternative CTRL for embedded commands
int option_multibyte=espeakCHARS_AUTO;   // 0=auto, 1=utf8, 2=8bit, 3=wchar

// these are overridden by defaults set in the "speak" file
int option_linelength = 0;

#define N_EMBEDDED_LIST  250
static int embedded_ix;
static int embedded_read;
unsigned int embedded_list[N_EMBEDDED_LIST];

// the source text of a single clause
#define N_TR_SOURCE    350
static char source[N_TR_SOURCE+40];     // extra space for embedded command & voice change info at end

int n_replace_phonemes;
REPLACE_PHONEMES replace_phonemes[N_REPLACE_PHONEMES];


// brackets, also 0x2014 to 0x021f which don't need to be in this list
static const unsigned short brackets[] = {
'(',')','[',']','{','}','<','>','"','\'','`',
0xab,0xbb,  // double angle brackets
0x300a,0x300b,  // double angle brackets (ideograph)
0};

// other characters which break a word, but don't produce a pause
static const unsigned short breaks[] = {'_', 0};



// Translate character codes 0xA0 to 0xFF into their unicode values
// ISO_8859_1 is set as default
static const unsigned short ISO_8859_1[0x60] = {
   0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7, // a0
   0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af, // a8
   0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7, // b0
   0x00b8, 0x00b9, 0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf, // b8
   0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7, // c0
   0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf, // c8
   0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7, // d0
   0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df, // d8
   0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7, // e0
   0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, // e8
   0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7, // f0
   0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff, // f8
};

static const unsigned short ISO_8859_2[0x60] = {
   0x00a0, 0x0104, 0x02d8, 0x0141, 0x00a4, 0x013d, 0x015a, 0x00a7, // a0
   0x00a8, 0x0160, 0x015e, 0x0164, 0x0179, 0x00ad, 0x017d, 0x017b, // a8
   0x00b0, 0x0105, 0x02db, 0x0142, 0x00b4, 0x013e, 0x015b, 0x02c7, // b0
   0x00b8, 0x0161, 0x015f, 0x0165, 0x017a, 0x02dd, 0x017e, 0x017c, // b8
   0x0154, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x0139, 0x0106, 0x00c7, // c0
   0x010c, 0x00c9, 0x0118, 0x00cb, 0x011a, 0x00cd, 0x00ce, 0x010e, // c8
   0x0110, 0x0143, 0x0147, 0x00d3, 0x00d4, 0x0150, 0x00d6, 0x00d7, // d0
   0x0158, 0x016e, 0x00da, 0x0170, 0x00dc, 0x00dd, 0x0162, 0x00df, // d8
   0x0155, 0x00e1, 0x00e2, 0x0103, 0x00e4, 0x013a, 0x0107, 0x00e7, // e0
   0x010d, 0x00e9, 0x0119, 0x00eb, 0x011b, 0x00ed, 0x00ee, 0x010f, // e8
   0x0111, 0x0144, 0x0148, 0x00f3, 0x00f4, 0x0151, 0x00f6, 0x00f7, // f0
   0x0159, 0x016f, 0x00fa, 0x0171, 0x00fc, 0x00fd, 0x0163, 0x02d9, // f8
};

static const unsigned short ISO_8859_3[0x60] = {
   0x00a0, 0x0126, 0x02d8, 0x00a3, 0x00a4, 0x0000, 0x0124, 0x00a7, // a0
   0x00a8, 0x0130, 0x015e, 0x011e, 0x0134, 0x00ad, 0x0000, 0x017b, // a8
   0x00b0, 0x0127, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x0125, 0x00b7, // b0
   0x00b8, 0x0131, 0x015f, 0x011f, 0x0135, 0x00bd, 0x0000, 0x017c, // b8
   0x00c0, 0x00c1, 0x00c2, 0x0000, 0x00c4, 0x010a, 0x0108, 0x00c7, // c0
   0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf, // c8
   0x0000, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x0120, 0x00d6, 0x00d7, // d0
   0x011c, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x016c, 0x015c, 0x00df, // d8
   0x00e0, 0x00e1, 0x00e2, 0x0000, 0x00e4, 0x010b, 0x0109, 0x00e7, // e0
   0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, // e8
   0x0000, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x0121, 0x00f6, 0x00f7, // f0
   0x011d, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x016d, 0x015d, 0x02d9, // f8
};

static const unsigned short ISO_8859_4[0x60] = {
   0x00a0, 0x0104, 0x0138, 0x0156, 0x00a4, 0x0128, 0x013b, 0x00a7, // a0
   0x00a8, 0x0160, 0x0112, 0x0122, 0x0166, 0x00ad, 0x017d, 0x00af, // a8
   0x00b0, 0x0105, 0x02db, 0x0157, 0x00b4, 0x0129, 0x013c, 0x02c7, // b0
   0x00b8, 0x0161, 0x0113, 0x0123, 0x0167, 0x014a, 0x017e, 0x014b, // b8
   0x0100, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x012e, // c0
   0x010c, 0x00c9, 0x0118, 0x00cb, 0x0116, 0x00cd, 0x00ce, 0x012a, // c8
   0x0110, 0x0145, 0x014c, 0x0136, 0x00d4, 0x00d5, 0x00d6, 0x00d7, // d0
   0x00d8, 0x0172, 0x00da, 0x00db, 0x00dc, 0x0168, 0x016a, 0x00df, // d8
   0x0101, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x012f, // e0
   0x010d, 0x00e9, 0x0119, 0x00eb, 0x0117, 0x00ed, 0x00ee, 0x012b, // e8
   0x0111, 0x0146, 0x014d, 0x0137, 0x00f4, 0x00f5, 0x00f6, 0x00f7, // f0
   0x00f8, 0x0173, 0x00fa, 0x00fb, 0x00fc, 0x0169, 0x016b, 0x02d9, // f8
};

static const unsigned short ISO_8859_5[0x60] = {
   0x00a0, 0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407, // a0  Cyrillic
   0x0408, 0x0409, 0x040a, 0x040b, 0x040c, 0x00ad, 0x040e, 0x040f, // a8
   0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, // b0
   0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e, 0x041f, // b8
   0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427, // c0
   0x0428, 0x0429, 0x042a, 0x042b, 0x042c, 0x042d, 0x042e, 0x042f, // c8
   0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437, // d0
   0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e, 0x043f, // d8
   0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, // e0
   0x0448, 0x0449, 0x044a, 0x044b, 0x044c, 0x044d, 0x044e, 0x044f, // e8
   0x2116, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457, // f0
   0x0458, 0x0459, 0x045a, 0x045b, 0x045c, 0x00a7, 0x045e, 0x045f, // f8
};

static const unsigned short ISO_8859_7[0x60] = {
   0x00a0, 0x2018, 0x2019, 0x00a3, 0x20ac, 0x20af, 0x00a6, 0x00a7, // a0  Greek
   0x00a8, 0x00a9, 0x037a, 0x00ab, 0x00ac, 0x00ad, 0x0000, 0x2015, // a8
   0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x0384, 0x0385, 0x0386, 0x00b7, // b0
   0x0388, 0x0389, 0x038a, 0x00bb, 0x038c, 0x00bd, 0x038e, 0x038f, // b8
   0x0390, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397, // c0
   0x0398, 0x0399, 0x039a, 0x039b, 0x039c, 0x039d, 0x039e, 0x039f, // c8
   0x03a0, 0x03a1, 0x0000, 0x03a3, 0x03a4, 0x03a5, 0x03a6, 0x03a7, // d0
   0x03a8, 0x03a9, 0x03aa, 0x03ab, 0x03ac, 0x03ad, 0x03ae, 0x03af, // d8
   0x03b0, 0x03b1, 0x03b2, 0x03b3, 0x03b4, 0x03b5, 0x03b6, 0x03b7, // e0
   0x03b8, 0x03b9, 0x03ba, 0x03bb, 0x03bc, 0x03bd, 0x03be, 0x03bf, // e8
   0x03c0, 0x03c1, 0x03c2, 0x03c3, 0x03c4, 0x03c5, 0x03c6, 0x03c7, // f0
   0x03c8, 0x03c9, 0x03ca, 0x03cb, 0x03cc, 0x03cd, 0x03ce, 0x0000, // f8
};

static const unsigned short ISO_8859_9[0x60] = {
   0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7, // a0
   0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af, // a8
   0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7, // b0
   0x00b8, 0x00b9, 0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf, // b8
   0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7, // c0
   0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf, // c8
   0x011e, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7, // d0
   0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x0130, 0x015e, 0x00df, // d8
   0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7, // e0
   0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, // e8
   0x011f, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7, // f0
   0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x0131, 0x015f, 0x00ff, // f8
};

static const unsigned short ISO_8859_14[0x60] = {
   0x00a0, 0x1e02, 0x1e03, 0x00a3, 0x010a, 0x010b, 0x1e0a, 0x00a7, // a0  Welsh
   0x1e80, 0x00a9, 0x1e82, 0x1e0b, 0x1ef2, 0x00ad, 0x00ae, 0x0178, // a8
   0x1e1e, 0x1e1f, 0x0120, 0x0121, 0x1e40, 0x1e41, 0x00b6, 0x1e56, // b0
   0x1e81, 0x1e57, 0x1e83, 0x1e60, 0x1ef3, 0x1e84, 0x1e85, 0x1e61, // b8
   0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7, // c0
   0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf, // c8
   0x0174, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x1e6a, // d0
   0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x0176, 0x00df, // d8
   0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7, // e0
   0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, // e8
   0x0175, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x1e6b, // f0
   0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x0177, 0x00ff, // f8
};

static const unsigned short KOI8_R[0x60] = {
   0x2550, 0x2551, 0x2552, 0x0451, 0x2553, 0x2554, 0x2555, 0x2556, // a0  Russian
   0x2557, 0x2558, 0x2559, 0x255a, 0x255b, 0x255c, 0x255d, 0x255e, // a8
   0x255f, 0x2560, 0x2561, 0x0401, 0x2562, 0x2563, 0x2564, 0x2565, // b0
   0x2566, 0x2567, 0x2568, 0x2569, 0x256a, 0x256b, 0x256c, 0x00a9, // b8
   0x044e, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433, // c0
   0x0445, 0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e, // c8
   0x043f, 0x044f, 0x0440, 0x0441, 0x0442, 0x0443, 0x0436, 0x0432, // d0
   0x044c, 0x044b, 0x0437, 0x0448, 0x044d, 0x0449, 0x0447, 0x044a, // d8
   0x042e, 0x0410, 0x0411, 0x0426, 0x0414, 0x0415, 0x0424, 0x0413, // e0
   0x0425, 0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e, // e8
   0x041f, 0x042f, 0x0420, 0x0421, 0x0422, 0x0423, 0x0416, 0x0412, // f0
   0x042c, 0x042b, 0x0417, 0x0428, 0x042d, 0x0429, 0x0427, 0x042a, // f8
};

static const unsigned short ISCII[0x60] = {
   0x0020, 0x0901, 0x0902, 0x0903, 0x0905, 0x0906, 0x0907, 0x0908, // a0
   0x0909, 0x090a, 0x090b, 0x090e, 0x090f, 0x0910, 0x090d, 0x0912, // a8
   0x0913, 0x0914, 0x0911, 0x0915, 0x0916, 0x0917, 0x0918, 0x0919, // b0
   0x091a, 0x091b, 0x091c, 0x091d, 0x091e, 0x091f, 0x0920, 0x0921, // b8
   0x0922, 0x0923, 0x0924, 0x0925, 0x0926, 0x0927, 0x0928, 0x0929, // c0
   0x092a, 0x092b, 0x092c, 0x092d, 0x092e, 0x092f, 0x095f, 0x0930, // c8
   0x0931, 0x0932, 0x0933, 0x0934, 0x0935, 0x0936, 0x0937, 0x0938, // d0
   0x0939, 0x0020, 0x093e, 0x093f, 0x0940, 0x0941, 0x0942, 0x0943, // d8
   0x0946, 0x0947, 0x0948, 0x0945, 0x094a, 0x094b, 0x094c, 0x0949, // e0
   0x094d, 0x093c, 0x0964, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, // e8
   0x0020, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, // f0
   0x0037, 0x0038, 0x0039, 0x20,   0x20,   0x20,   0x20,   0x20,   // f8
};

const unsigned short *charsets[N_CHARSETS] = {
	ISO_8859_1,
	ISO_8859_1,
	ISO_8859_2,
	ISO_8859_3,
	ISO_8859_4,
	ISO_8859_5,
	ISO_8859_1,
	ISO_8859_7,
	ISO_8859_1,
	ISO_8859_9,
	ISO_8859_1,
	ISO_8859_1,
	ISO_8859_1,
	ISO_8859_1,
	ISO_8859_14,
	ISO_8859_1,
	ISO_8859_1,
	ISO_8859_1,
	KOI8_R,          // 18
	ISCII };

// Tables of the relative lengths of vowels, depending on the
// type of the two phonemes that follow
// indexes are the "length_mod" value for the following phonemes

// use this table if vowel is not the last in the word
static unsigned char length_mods_en[100] = {
/*  a   ,   t   s   n   d   z   r   N   <- next */
	100,120,100,105,100,110,110,100, 95, 100,  /* a  <- next2 */
	105,120,105,110,125,130,135,115,125, 100,  /* , */
	105,120, 75,100, 75,105,120, 85, 75, 100,  /* t */
	105,120, 85,105, 95,115,120,100, 95, 100,  /* s */
	110,120, 95,105,100,115,120,100,100, 100,  /* n */
	105,120,100,105, 95,115,120,110, 95, 100,  /* d */
	105,120,100,105,105,122,125,110,105, 100,  /* z */
	105,120,100,105,105,122,125,110,105, 100,  /* r */
	105,120, 95,105,100,115,120,110,100, 100,  /* N */
	100,120,100,100,100,100,100,100,100, 100 }; // SPARE

// as above, but for the last syllable in a word
static unsigned char length_mods_en0[100] = {
/*  a   ,   t   s   n   d   z   r    N  <- next */
	100,150,100,105,110,115,110,110,110, 100,  /* a  <- next2 */
	105,150,105,110,125,135,140,115,135, 100,  /* , */
	105,150, 90,105, 90,122,135,100, 90, 100,  /* t */
	105,150,100,105,100,122,135,100,100, 100,  /* s */
	105,150,100,105,105,115,135,110,105, 100,  /* n */
	105,150,100,105,105,122,130,120,125, 100,  /* d */
	105,150,100,105,110,122,125,115,110, 100,  /* z */
	105,150,100,105,105,122,135,120,105, 100,  /* r */
	105,150,100,105,105,115,135,110,105, 100,  /* N */
	100,100,100,100,100,100,100,100,100, 100 }; // SPARE


static unsigned char length_mods_equal[100] = {
/*  a   ,   t   s   n   d   z   r   N   <- next */
	110,110,110,110,110,110,110,110,110, 110,  /* a  <- next2 */
	110,110,110,110,110,110,110,110,110, 110,  /* , */
	110,110,110,110,110,110,110,110,110, 110,  /* t */
	110,110,110,110,110,110,110,110,110, 110,  /* s */
	110,110,110,110,110,110,110,110,110, 110,  /* n */
	110,110,110,110,110,110,110,110,110, 110,  /* d */
	110,110,110,110,110,110,110,110,110, 110,  /* z */
	110,110,110,110,110,110,110,110,110, 110,  /* r */
	110,110,110,110,110,110,110,110,110, 110,  /* N */
	110,110,110,110,110,110,110,110,110, 110 }; // SPARE


unsigned char *length_mod_tabs[6] = {
  length_mods_en,
  length_mods_en,     // 1
  length_mods_en0,    // 2
  length_mods_equal,  // 3
  length_mods_equal,  // 4
  length_mods_equal   // 5
 };


/* index by 0=. 1=, 2=?, 3=! 4=none */
static unsigned char punctuation_to_tone[4][5] = {
	{0,1,1,2,0},
	{3,4,4,5,3},
	{6,7,7,8,6},
	{9,10,10,11,9} };



void SetLengthMods(Translator *tr, int value)
{//==========================================
	int value2;

	tr->langopts.length_mods0 = tr->langopts.length_mods = length_mod_tabs[value % 100];
	if((value2 = value / 100) != 0)
	{
		tr->langopts.length_mods0 = length_mod_tabs[value2];
	}
}


int IsAlpha(unsigned int c)
{//========================
// Replacement for iswalph() which also checks for some in-word symbols

	if(iswalpha(c))
		return(1);

	if((c >= 0x901) && (c <= 0x957))
		return(1);    // Devanagari  vowel signs and other signs

	return(0);
}

int IsDigit(unsigned int c)
{//========================
	if(iswdigit(c))
		return(1);

	if((c >= 0x966) && (c <= 0x96f))
		return(1);

	return(0);
}


Translator::Translator()
{//=====================
	int ix;
	static int stress_amps2[] = {16,16, 20,20, 20,24, 24,21 };
	static int stress_lengths2[8] = {182,140, 220,220, 220,240, 260,280};
	static const wchar_t empty_wstring[1] = {0};

	charset_a0 = charsets[1];   // ISO-8859-1, this is for when the input is not utf8
	dictionary_name[0] = 0;
	dict_condition=0;
	data_dictrules = NULL;     // language_1   translation rules file
	data_dictlist = NULL;      // language_2   dictionary lookup file

	transpose_offset = 0;

	// only need lower case
	letter_bits_offset = 0;
	memset(letter_bits,0,sizeof(letter_bits));
	memset(letter_groups,0,sizeof(letter_groups));

	// 0-5 sets of characters matched by A B C E F G in pronunciation rules
	// these may be set differently for different languages
	SetLetterBits(this,0,"aeiou");  // A  vowels, except y
	SetLetterBits(this,1,"bcdfgjklmnpqstvxz");      // B  hard consonants, excluding h,r,w
	SetLetterBits(this,2,"bcdfghjklmnpqrstvwxz");  // C  all consonants
	SetLetterBits(this,3,"hlmnr");                 // H  'soft' consonants
	SetLetterBits(this,4,"cfhkpqstx");             // F  voiceless consonants
	SetLetterBits(this,5,"bdgjlmnrvwyz");   // G voiced
	SetLetterBits(this,6,"eiy");   // Letter group Y, front vowels
	SetLetterBits(this,7,"aeiouy");  // vowels, including y


	char_plus_apostrophe = empty_wstring;

	for(ix=0; ix<8; ix++)
	{
		stress_amps[ix] = stress_amps2[ix];
		stress_amps_r[ix] = stress_amps2[ix] - 1;
		stress_lengths[ix] = stress_lengths2[ix];
	}
	memset(&langopts,0,sizeof(langopts));

	langopts.stress_rule = 2;
	langopts.unstressed_wd1 = 1;
	langopts.unstressed_wd2 = 3;
	langopts.param[LOPT_SONORANT_MIN] = 95;
	langopts.param[LOPT_MAXAMP_EOC] = 19;
	langopts.param[LOPT_UNPRONOUNCABLE] = 's';    // don't count this character at start of word
	langopts.max_initial_consonants = 3;
	langopts.replace_chars = NULL;

	langopts.length_mods = length_mods_en;
	langopts.length_mods0 = length_mods_en0;
	langopts.long_stop = 100;

	langopts.max_roman = 49;
	langopts.thousands_sep = ',';
	langopts.decimal_sep = '.';

	memcpy(punct_to_tone,punctuation_to_tone,sizeof(punct_to_tone));
	punct_to_tone[0][3] = 0;   // exclamation, use period until we can improve the exclamation intonation 
}


Translator::~Translator(void)
{//==========================
	if(data_dictlist != NULL)
		Free(data_dictlist);
}


int lookupwchar(const unsigned short *list,int c)
{//==============================================
// Is the character c in the list ?
	int ix;

	for(ix=0; list[ix] != 0; ix++)
	{
		if(list[ix] == c)
			return(ix+1);
	}
	return(0);
}

int IsBracket(int c)
{//=================
	if((c >= 0x2014) && (c <= 0x201f))
		return(1);
	return(lookupwchar(brackets,c));
}


int utf8_out(unsigned int c, char *buf)
{//====================================
// write a unicode character into a buffer as utf8
// returns the number of bytes written
	int n_bytes;
	int j;
	int shift;
	static char unsigned code[4] = {0,0xc0,0xe0,0xf0};

	if(c < 0x80)
	{
		buf[0] = c;
		return(1);
	}
	if(c >= 0x110000)
	{
		buf[0] = ' ';      // out of range character code
		return(1);
	}
	if(c < 0x0800)
		n_bytes = 1;
	else
	if(c < 0x10000)
		n_bytes = 2;
	else
		n_bytes = 3;

	shift = 6*n_bytes;
	buf[0] = code[n_bytes] | (c >> shift);
	for(j=0; j<n_bytes; j++)
	{
		shift -= 6;
		buf[j+1] = 0x80 + ((c >> shift) & 0x3f);
	}
	return(n_bytes+1);
}  // end of utf8_out


int utf8_in(int *c, char *buf, int backwards)
{//==========================================
	int c1;
	int n_bytes;
	int ix;
	static const unsigned char mask[4] = {0xff,0x1f,0x0f,0x07};

	// find the start of the next/previous character
	while((*buf & 0xc0) == 0x80)
	{
		// skip over non-initial bytes of a multi-byte utf8 character
		if(backwards)
			buf--;
		else
			buf++;
	}

	n_bytes = 0;

	if((c1 = *buf++) & 0x80)
	{
		if((c1 & 0xe0) == 0xc0)
			n_bytes = 1;
		else
		if((c1 & 0xf0) == 0xe0)
			n_bytes = 2;
		else
		if((c1 & 0xf8) == 0xf0)
			n_bytes = 3;

		c1 &= mask[n_bytes];
		for(ix=0; ix<n_bytes; ix++)
		{
			c1 = (c1 << 6) + (*buf++ & 0x3f);
		}
	}
	*c = c1;
	return(n_bytes+1);
}




char *strchr_w(const char *s, int c)
{//=================================
// return NULL for any non-ascii character
	if(c >= 0x80)
		return(NULL);
	return(strchr((char *)s,c));    // (char *) is needed for Borland compiler
}


int PhonemeCode(unsigned int mnem)
{//===============================
	int ix;

	for(ix=0; ix<n_phoneme_tab; ix++)
	{
		if(phoneme_tab[ix]->mnemonic == mnem)
			return(phoneme_tab[ix]->code);
	}
	return(phonSCHWA);
}




int Translator::TranslateWord(char *word1, int next_pause, WORD_TAB *wtab)
{//=======================================================================
// word1 is terminated by space (0x20) character

	int length;
	int word_length;
	int ix;
	int posn;
	unsigned int dictionary_flags=0;
	unsigned int dictionary_flags2=0;
	int end_type=0;
	int prefix_type=0;
	char *word;
	char phonemes[N_WORD_PHONEMES];
	char *ph_limit;
	char *phonemes_ptr;
	char prefix_phonemes[N_WORD_PHONEMES];
	char end_phonemes[N_WORD_PHONEMES];
	char word_copy[N_WORD_BYTES];
	int found=0;
   int end_flags;
	char c_temp;   // save a character byte while we temporarily replace it with space
	int first_char;
	int last_char = 0;
	int unpron_length;
	int add_plural_suffix = 0;
	int prefix_flags = 0;
	int confirm_prefix;
	int spell_word;
	int wflags = wtab->flags;
	int wmark = wtab->wmark;

	// translate these to get pronunciations of plural 's' suffix (different forms depending on
	// the preceding letter
	static char word_zz[4] = {0,'z','z',0};
	static char word_iz[4] = {0,'i','z',0};
	static char word_ss[4] = {0,'s','s',0};

	word = word1;
	prefix_phonemes[0] = 0;
	end_phonemes[0] = 0;
	ph_limit = &phonemes[N_WORD_PHONEMES];

	// count the length of the word
	utf8_in(&first_char,word,0);
	word_length = 0;
	while((*word != 0) && (*word != ' '))
	{
		word += utf8_in(&last_char,word,0);
		word_length++;
	}
	word = word1;

	// try an initial lookup in the dictionary list, we may find a pronunciation specified, or
	// we may just find some flags
	if((option_sayas & 0xf0) == 0x10)
	{
		// SAYAS_CHAR, SAYAS_GYLPH, or SAYAS_SINGLE_CHAR
		spell_word = option_sayas & 0xf;    // 2,3,4
	}
	else
	{
		spell_word = 0;
		found = LookupDictList(word,phonemes,&dictionary_flags,wflags << 16);
		if(phonemes[0] == phonSWITCH)
		{
			// change to another language in order to translate this word
			strcpy(word_phonemes,phonemes);
			return(0);
		}

if((wmark > 0) && (wmark < 8))
{
	// the stressed syllable has been specified in the text  (TESTING)
	dictionary_flags = (dictionary_flags & ~0xf) | wmark;
}

		if(!found && (dictionary_flags & FLAG_ABBREV))
		{
			// the word has $abbrev flag, but no pronunciation specified.  Speak as individual letters
			spell_word = 1;
		}
 
		if(!found && iswdigit(first_char))
		{
			found = TranslateNumber(word,phonemes,&dictionary_flags,wflags);
		}

		if(!found & ((word_flags & FLAG_UPPERS) != FLAG_FIRST_UPPER))
		{
			// either all upper or all lower case
			if((found = TranslateRoman(word,phonemes)) != 0)
				dictionary_flags |= FLAG_ABBREV;      // don't spell capital Roman numbers as individual letters
		}

		if((wflags & FLAG_ALL_UPPER) && (clause_upper_count <= clause_lower_count) &&
			!(dictionary_flags & (FLAG_ABBREV | FLAG_SKIPWORDS)) && (word_length>1) && (word_length<4) && iswalpha(first_char))
		{
			// An upper case word in a lower case clause. This could be an abbreviation.
			spell_word = 1;
		}
	}

	if(spell_word > 0)
	{
		// Speak as individual letters
		word = word1;
		posn = 0;
		phonemes[0] = 0;
		end_type = 0;

		while(*word != ' ')
		{
			word += TranslateLetter(word, phonemes,spell_word);
			if(phonemes[0] == phonSWITCH)
			{
				// change to another language in order to translate this word
				strcpy(word_phonemes,phonemes);
				return(0);
			}
		}
		SetSpellingStress(phonemes,spell_word);
	}
	else
	if(found == 0)
	{
		// word's pronunciation is not given in the dictionary list, although
		// dictionary_flags may have ben set there

		posn = 0;
		length = 999;
		while(((length < 3) && (length > 0))|| (word_length > 1 && Unpronouncable(word)))
		{
			char *p;
			// This word looks "unpronouncable", so speak letters individually until we
			// find a remainder that we can pronounce.
			word += TranslateLetter(word,phonemes,0);
			if(phonemes[0] == phonSWITCH)
			{
				// change to another language in order to translate this word
				strcpy(word_phonemes,phonemes);
				return(0);
			}

			p = &word[word_length-3];
			if(memcmp(p,"'s ",3) == 0)
			{
				// remove a 's suffix and pronounce this separately (not as an individual letter)
				add_plural_suffix = 1;
				p[0] = ' ';
				p[1] = ' ';
				last_char = p[-1];
			}

			length=0;
			while(word[length] != ' ') length++;
			word[-1] = ' ';            // prevent this affecting the pronunciation of the pronuncable part
		}
		SetSpellingStress(phonemes,0);

		// anything left ?
		if(*word != ' ')
		{
			// Translate the stem
			unpron_length = strlen(phonemes);
			end_type = TranslateRules(word, phonemes, N_WORD_PHONEMES, end_phonemes, wflags, dictionary_flags);

			if(phonemes[0] == phonSWITCH)
			{
				// change to another language in order to translate this word
				strcpy(word_phonemes,phonemes);
				return(0);
			}

			c_temp = word[-1];

			found = 0;
			confirm_prefix = 1;
			while(end_type & SUFX_P)
			{
				// Found a standard prefix, remove it and retranslate

				if(confirm_prefix)
				{
					int end2;
					char phonemes2[N_WORD_PHONEMES];
					char end_phonemes2[N_WORD_PHONEMES];

					// remove any standard suffix and confirm that the prefix is still recognised
					phonemes2[0] = 0;
					end2 = TranslateRules(word, phonemes2, N_WORD_PHONEMES, end_phonemes2, wflags|FLAG_NO_PREFIX|FLAG_NO_TRACE, dictionary_flags);
					if(end2)
					{
						RemoveEnding(word,end2,word_copy);
						end_type = TranslateRules(word, phonemes, N_WORD_PHONEMES, end_phonemes, wflags|FLAG_NO_TRACE, dictionary_flags);
						memcpy(word,word_copy,strlen(word_copy));
						if((end_type & SUFX_P) == 0)
						{
							// after removing the suffix, the prefix is no longer recognised.
							// Keep the suffix, but don't use the prefix
							end_type = end2;
							strcpy(phonemes,phonemes2);
							strcpy(end_phonemes,end_phonemes2);
							if(option_phonemes == 2)
							{
								DecodePhonemes(end_phonemes,end_phonemes2);
								fprintf(f_trans,"  suffix [%s]\n\n",end_phonemes2);
							}
						}
						confirm_prefix = 0;
						continue;
					}
				}

				strcat(prefix_phonemes,end_phonemes);
				end_phonemes[0] = 0;
				prefix_type = end_type;

				if(prefix_type & SUFX_V)
				{
					expect_verb = 1;      // use the verb form of the word
				}

				word[-1] = c_temp;
				for(ix=(prefix_type & 0xf); ix>0; ix--)    // num. of characters to remove
				{
					word++;
					while((*word & 0xc0) == 0x80) word++;  // for multibyte characters
				}
				c_temp = word[-1];
				word[-1] = ' ';
				confirm_prefix = 1;

				end_type = 0;
				found = LookupDictList(word,phonemes,&dictionary_flags2,SUFX_P | (wflags << 16));
				if(dictionary_flags==0)
					dictionary_flags = dictionary_flags2;
				else
					prefix_flags = 1;
				if(found == 0)
				{
					end_type = TranslateRules(word, phonemes, N_WORD_PHONEMES, end_phonemes, 0, dictionary_flags);

					if(phonemes[0] == phonSWITCH)
					{
						// change to another language in order to translate this word
						word[-1] = c_temp;
						strcpy(word_phonemes,phonemes);
						return(0);
					}
				}
			}

			if((end_type != 0) && !(end_type & SUFX_P))
			{
char phonemes2[N_WORD_PHONEMES];
strcpy(phonemes2,phonemes);

				// The word has a standard ending, re-translate without this ending
				end_flags = RemoveEnding(word,end_type,word_copy);

				phonemes_ptr = &phonemes[unpron_length];
				phonemes_ptr[0] = 0;

				if(prefix_phonemes[0] != 0)
				{
					// lookup the stem without the prefix removed
					word[-1] = c_temp;
					found = LookupDictList(word1,phonemes_ptr,&dictionary_flags2,end_flags | (wflags << 16));
					word[-1] = ' ';
					if(dictionary_flags==0)
						dictionary_flags = dictionary_flags2;
					if(found)
						prefix_phonemes[0] = 0;  // matched whole word, don't need prefix now

//					if(found || (dictionary_flags2 != 0))
//						prefix_flags = 1;        // ?? this looks wrong
					if((found==0) && (dictionary_flags2 != 0))
						prefix_flags = 1;
				}
				if(found == 0)
				{
					found = LookupDictList(word,phonemes_ptr,&dictionary_flags2,end_flags | (wflags << 16));
					if(phonemes_ptr[0] == phonSWITCH)
					{
						// change to another language in order to translate this word
						memcpy(word,word_copy,strlen(word_copy));
						strcpy(word_phonemes,phonemes_ptr);
						return(0);
					}
					if(dictionary_flags==0)
						dictionary_flags = dictionary_flags2;
				}
				if(found == 0)
				{
					if(end_type & SUFX_Q)
					{
						// don't retranslate, use the original lookup result
						strcpy(phonemes,phonemes2);

						// language specific changes
						ApplySpecialAttribute(phonemes,dictionary_flags);
					}
					else
					{
						if(end_flags & FLAG_SUFX)
							TranslateRules(word, phonemes, N_WORD_PHONEMES, NULL,wflags | FLAG_SUFFIX_REMOVED, dictionary_flags);
						else
							TranslateRules(word, phonemes, N_WORD_PHONEMES, NULL,wflags,dictionary_flags);

						if(phonemes[0] == phonSWITCH)
						{
							// change to another language in order to translate this word
							strcpy(word_phonemes,phonemes);
							memcpy(word,word_copy,strlen(word_copy));
							word[-1] = c_temp;
							return(0);
						}
					}
				}

				if((end_type & SUFX_T) == 0)
				{
					// the default is to add the suffix and then determine the word's stress pattern
					AppendPhonemes(phonemes, N_WORD_PHONEMES, end_phonemes);
					end_phonemes[0] = 0;
				}
			}
			word[-1] = c_temp;
		}
	}

	if((add_plural_suffix) || (wflags & FLAG_HAS_PLURAL))
	{
		// s or 's suffix, append [s], [z] or [Iz] depending on previous letter
		if(last_char == 'f')
			TranslateRules(&word_ss[1], phonemes, N_WORD_PHONEMES, NULL, 0, 0);
		else
		if(strchr_w("hsx",last_char)==NULL)
			TranslateRules(&word_zz[1], phonemes, N_WORD_PHONEMES, NULL, 0, 0);
		else
			TranslateRules(&word_iz[1], phonemes, N_WORD_PHONEMES, NULL, 0, 0);
	}


	/* determine stress pattern for this word */
	/******************************************/
	/* NOTE: this also adds a single PAUSE if the previous word ended
				in a primary stress, and this one starts with one */
	if(prefix_flags || (strchr(prefix_phonemes,phonSTRESS_P)!=NULL))
	{
		if((langopts.param[LOPT_PREFIXES]) || (prefix_type & SUFX_T))
		{
			char *p;
			// German, keep a secondary stress on the stem
			SetWordStress(phonemes,dictionary_flags,3,0);

			// reduce all but the first primary stress
			ix=0;
			for(p=prefix_phonemes; *p != 0; p++)
			{
				if(*p == phonSTRESS_P)
				{
					if(ix==0)
						ix=1;
					else
						*p = phonSTRESS_3;
				}
			}
			strcpy(word_phonemes,prefix_phonemes);
			strcat(word_phonemes,phonemes);
			SetWordStress(word_phonemes,dictionary_flags,-1,0);
		}
		else
		{
			// stress position affects the whole word, including prefix
			strcpy(word_phonemes,prefix_phonemes);
			strcat(word_phonemes,phonemes);
			SetWordStress(word_phonemes,dictionary_flags,-1,prev_last_stress);
		}
	}
	else
	{
		if(prefix_phonemes[0] == 0)
			SetWordStress(phonemes,dictionary_flags,-1,prev_last_stress);
		else
			SetWordStress(phonemes,dictionary_flags,-1,0);
		strcpy(word_phonemes,prefix_phonemes);
		strcat(word_phonemes,phonemes);
	}

	if(end_phonemes[0] != 0)
	{
		// a suffix had the SUFX_T option set, add the suffix after the stress pattern has been determined
		strcat(word_phonemes,end_phonemes);
	}

//	if(next_pause > 2)
	ix = ((dictionary_flags >> 5) & 7);  // dictionary indicates skip next word(s)
	if(wtab[ix].flags & FLAG_LAST_WORD)
	{
		// the word has attribute to stress or unstress when at end of clause
		if(dictionary_flags & (FLAG_STRESS_END | FLAG_STRESS_END2))
			ChangeWordStress(this,word_phonemes,4);
//			SetWordStress(word_phonemes,0,4,prev_last_stress);
		else
		if(dictionary_flags & FLAG_UNSTRESS_END)
			ChangeWordStress(this,word_phonemes,3);
//			SetWordStress(word_phonemes,0,3,prev_last_stress);
	}
	if(wflags & FLAG_STRESSED_WORD)
	{
		// A word is indicated in the source text as stressed

		// we need to improve the intonation module to deal better with a clauses tonic
		// stress being early in the clause, before enabling this
		//SetWordStress(word_phonemes,0,5,prev_last_stress);
	}

	// dictionary flags for this word give a clue about which alternative pronunciations of
	// following words to use.
	if(end_type & SUFX_F)
	{
		// expect a verb form, with or without -s suffix
		expect_verb = 2;
		expect_verb_s = 2;
	}

	if(dictionary_flags & FLAG_PASTF)
	{
		/* expect perfect tense in next two words */
		expect_past = 3;
		expect_verb = 0;
	}
	else
	if(dictionary_flags & FLAG_VERBF)
	{
		/* expect a verb in the next word */
		expect_verb = 2;
		expect_verb_s = 0;   /* verb won't have -s suffix */
	}
	else
	if(dictionary_flags & FLAG_VERBSF)
	{
		// expect a verb, must have a -s suffix
		expect_verb = 0;
		expect_verb_s = 2;
		expect_past = 0;
	}
	else
	if(dictionary_flags & FLAG_NOUNF)
	{
		/* not expecting a verb next */
		expect_verb = 0;
		expect_verb_s = 0;
		expect_past = 0;
	}

	if((word[0] != 0) && (!(dictionary_flags & FLAG_VERB_EXT)))
	{
		if(expect_verb > 0)
			expect_verb -= 1;

		if(expect_verb_s > 0)
			expect_verb_s -= 1;

		if(expect_past > 0)
			expect_past -= 1;
	}

	if((word_length == 1) && iswalpha(first_char) && (first_char != 'i'))
	{
// English Specific !!!!
		// any single letter before a dot is an abbreviation, except 'I'
		dictionary_flags |= FLAG_DOT;
	}

	return(dictionary_flags);
}  //  end of TranslateWord



static void SetPlist2(PHONEME_LIST2 *p, unsigned char phcode)
{//==========================================================
	p->phcode = phcode;
	p->stress = 0;
	p->tone_number = 0;
	p->synthflags = 0;
	p->sourceix = 0;
}

static int CountSyllables(unsigned char *phonemes)
{//===============================================
	int count = 0;
	int phon;
	while((phon = *phonemes++) != 0)
	{
		if(phoneme_tab[phon]->type == phVOWEL)
			count++;
	}
	return(count);
}

int Translator::TranslateWord2(char *word, WORD_TAB *wtab, int pre_pause, int next_pause)
{//======================================================================================
	int flags=0;
	int stress;
	int next_stress;
	int next_tone=0;
	unsigned char *p;
	int srcix;
	int embedded_flag=0;
	int embedded_cmd;
	int value;
	int found_dict_flag;
	unsigned char ph_code;
	PHONEME_LIST2 *plist2;
	PHONEME_TAB *ph;
	int max_stress;
	int max_stress_ix=0;
	int prev_vowel = -1;
	int pitch_raised = 0;
	int switch_phonemes = -1;
	int first_phoneme = 1;
	int source_ix;
	int len;
	int sylimit;        // max. number of syllables in a word to be combined with a preceding preposition
	char *new_language;
	unsigned char bad_phoneme[4];

	len = wtab->length;
	if(len > 31) len = 31;
	source_ix = (wtab->sourceix & 0x7ff) | (len << 11); // bits 0-10 sourceix, bits 11-15 word length

	word_flags = wtab[0].flags;
	if(word_flags & FLAG_EMBEDDED)
	{
		embedded_flag = SFLAG_EMBEDDED;
		value = embedded_list[embedded_read] >> 8;

		switch(embedded_cmd = embedded_list[embedded_read] & 0x1f)
		{
		case EMBED_Y:
			option_sayas = value;
			break;

		case EMBED_F:
			option_emphasis = value;
			break;

		case EMBED_B:
			// break command
			if(value == 0)
				pre_pause = 0;  // break=none
			else
				pre_pause += value;
			break;
		}
		while((embedded_list[embedded_read++] & 0x80) == 0);  // read over the embedded commands for this word
	}

	if(word[0] == 0)
	{
		// nothing to translate
		word_phonemes[0] = 0;
		return(0);
	}

	// after a $pause word attribute, ignore a $pause attribute on the next two words
	if(prepause_timeout > 0)
		prepause_timeout--;

	if((option_sayas & 0xf0) == 0x10)
	{
		// SAYAS_CHARS, SAYAS_GLYPHS, or SAYAS_SINGLECHARS.  Pause between each word.
		pre_pause += 4;
	}

	if(word_flags & FLAG_FIRST_UPPER)
	{
		if((option_capitals > 2) && (embedded_ix < N_EMBEDDED_LIST-6))
		{
			// indicate capital letter by raising pitch
			if(embedded_flag)
				embedded_list[embedded_ix-1] &= ~0x80;   // already embedded command before this word, remove terminator
			if((pitch_raised = option_capitals) == 3)
				pitch_raised = 20;  // default pitch raise for capitals
			embedded_list[embedded_ix++] = EMBED_P+0x40+0x80 + (pitch_raised << 8);  // raise pitch
			embedded_flag = SFLAG_EMBEDDED;
		}
	}

	p = (unsigned char *)translator->word_phonemes;
	if(word_flags & FLAG_PHONEMES)
	{
		// The input is in phoneme mnemonics, not language text
		EncodePhonemes(word,word_phonemes,bad_phoneme);
		flags = FLAG_FOUND;
	}
	else
	{
		flags = translator->TranslateWord(word, next_pause, wtab);

		if((flags & FLAG_ALT2_TRANS) && ((sylimit = langopts.param[LOPT_COMBINE_WORDS]) > 0))
		{
			char *p2;
			int ok = 1;
			int flags2;
			int c_word2;
			char ph_buf[N_WORD_PHONEMES];

			// LANG=cs,sk
			// combine a preposition with the following word
			p2 = word;
			while(*p2 != ' ') p2++;

			utf8_in(&c_word2, p2+1, 0);   // first character of the next word;
			if(!iswalpha(c_word2))
			{
				ok =0;
			}

			if(sylimit & 0x100)
			{
				// only if the second word has $alt attribute
				strcpy(ph_buf,word_phonemes);
				flags2 = translator->TranslateWord(p2+1, 0, wtab+1);
				if((flags2 & FLAG_ALT_TRANS) == 0)
				{
					ok = 0;
					strcpy(word_phonemes,ph_buf);
				}
			}

			if((sylimit & 0x200) && ((wtab+1)->flags & FLAG_LAST_WORD))
			{
				// not if the next word is end-of-sentence
				ok = 0;
			}

			if(ok)
			{
				*p2 = '-'; // replace next space by hyphen
				flags = translator->TranslateWord(word, next_pause, wtab);  // translate the combined word
				if(CountSyllables(p) > (sylimit & 0xf))
				{
					// revert to separate words
					*p2 = ' ';
					flags = translator->TranslateWord(word, next_pause, wtab);
				}
				else
				{
					flags |= FLAG_SKIPWORDS_1;
				}
			}
		}

		if(p[0] == phonSWITCH)
		{
			// this word uses a different language
			new_language = (char *)(&p[1]);
			if(new_language[0]==0)
				new_language = "en";
			if((switch_phonemes = SelectPhonemeTableName(new_language)) >= 0)
			{
				if((translator2 != NULL) && (strcmp(new_language,translator2_language) != 0))
				{
					// we already have an alternative translator, but not for the required language, delete it
					delete translator2;
					translator2 = NULL;
				}

				if(translator2 == NULL)
				{
					translator2 = SelectTranslator(new_language);
					strcpy(translator2_language,new_language);

					if(translator2->LoadDictionary(new_language,0) != 0)
					{
						SelectPhonemeTable(voice->phoneme_tab_ix);  // revert to original phoneme table
						switch_phonemes = -1;
						translator2_language[0] = 0;
					}
				}
			}
			if(switch_phonemes >= 0)
			{
				// re-translate the word using the new translator
				flags = translator2->TranslateWord(word, next_pause, wtab);
				strcpy((char *)p,translator2->word_phonemes);
				if(p[0] == phonSWITCH)
				{
					// the second translator doesn't want to process this word
					switch_phonemes = -1;
				}
			}
			if(switch_phonemes < 0)
			{
				// language code is not recognised or 2nd translator won't translate it
				p[0] = phonSCHWA;  // just say something
				p[1] = phonSCHWA;
				p[2] = 0;
			}
		}

		if(!(word_flags & FLAG_LAST_WORD)  && !(word_flags & FLAG_HYPHEN))
		{
			if(flags & FLAG_PAUSE1)
			{
				if(pre_pause < 1)
					pre_pause = 1;
			}
			if((flags & FLAG_PREPAUSE) && (prepause_timeout == 0))
			{
				// the word is marked in the dictionary list with $pause
				if(pre_pause < 4) pre_pause = 4;
				prepause_timeout = 3;
			}
		}

		if((option_emphasis >= 3) && (pre_pause < 2))
			pre_pause = 2;
	}

	plist2 = &ph_list2[n_ph_list2];
	stress = 0;
	next_stress = 0;
	srcix = 0;
	max_stress = -1;

	found_dict_flag = 0;
	if(flags & FLAG_FOUND)
		found_dict_flag = SFLAG_DICTIONARY;

	while((pre_pause > 0) && (n_ph_list2 < N_PHONEME_LIST-4))
	{
		// add pause phonemes here. Either because of punctuation (brackets or quotes) in the
		// text, or because the word is marked in the dictionary lookup as a conjunction
		if(pre_pause > 1)
		{
			SetPlist2(&ph_list2[n_ph_list2++],phonPAUSE);
			pre_pause -= 2;
		}
		else
		{
			SetPlist2(&ph_list2[n_ph_list2++],phonPAUSE_NOLINK);
			pre_pause--;
		}
		end_stressed_vowel = 0;   // forget about the previous word
		prev_dict_flags = 0;
	}

	if((option_capitals==1) && (word_flags & FLAG_FIRST_UPPER))
	{
		SetPlist2(&ph_list2[n_ph_list2++],phonPAUSE_SHORT);
		SetPlist2(&ph_list2[n_ph_list2++],phonCAPITAL);
		if((word_flags & FLAG_ALL_UPPER) && IsAlpha(word[1]))
		{
			// word > 1 letter and all capitals
			SetPlist2(&ph_list2[n_ph_list2++],phonPAUSE_SHORT);
			SetPlist2(&ph_list2[n_ph_list2++],phonCAPITAL);
		}
	}

	if(switch_phonemes >= 0)
	{
		// this word uses a different phoneme table
		SetPlist2(&ph_list2[n_ph_list2],phonSWITCH);
		ph_list2[n_ph_list2++].tone_number = switch_phonemes;  // temporary phoneme table number
	}

	// remove initial pause from a word if it follows a hyphen
	if((word_flags & FLAG_HYPHEN) && (phoneme_tab[*p]->type == phPAUSE))
		p++;

	while(((ph_code = *p++) != 0) && (n_ph_list2 < N_PHONEME_LIST-4))
	{
		if(ph_code == 255)
			continue;      // unknown phoneme

		// Add the phonemes to the first stage phoneme list (ph_list2)
		ph = phoneme_tab[ph_code];
		if(ph->type == phSTRESS)
		{
			// don't add stress phonemes codes to the list, but give their stress
			// value to the next vowel phoneme
			// std_length is used to hold stress number or (if >10) a tone number for a tone language
			if(ph->spect == 0)
				next_stress = ph->std_length;
			else
			{
				// for tone languages, the tone number for a syllable follows the vowel
				if(prev_vowel >= 0)
				{
					ph_list2[prev_vowel].tone_number = ph_code;
				}
				else
				{
					next_tone = ph_code;       // no previous vowel, apply to the next vowel
				}
			}
		}
		else
		if(ph_code == phonSYLLABIC)
		{
			// mark the previous phoneme as a syllabic consonant
			ph_list2[n_ph_list2-1].synthflags |= SFLAG_SYLLABLE;
			ph_list2[n_ph_list2-1].stress = next_stress;
		}
		else
		if(ph_code == phonLENGTHEN)
		{
			ph_list2[n_ph_list2-1].synthflags |= SFLAG_LENGTHEN;
		}
		else
		if(ph_code == phonEND_WORD)
		{
			// a || symbol in a phoneme string was used to indicate a word boundary
			// Don't add this phoneme to the list, but make sure the next phoneme has
			// a newword indication
			srcix = source_ix+1;
		}
		else
		if(ph_code == phonX1)
		{
			// a language specific action 
			if(langopts.param[LOPT_IT_DOUBLING])
			{
				flags |= FLAG_DOUBLING;
			}
		}
		else
		{
			ph_list2[n_ph_list2].phcode = ph_code;
			ph_list2[n_ph_list2].tone_number = 0;
			ph_list2[n_ph_list2].synthflags = embedded_flag | found_dict_flag;
			embedded_flag = 0;
			ph_list2[n_ph_list2].sourceix = srcix;
			srcix = 0;

			if(ph->type == phVOWEL)
			{
				stress = next_stress;
				next_stress = 0;

				if((prev_vowel >= 0) && (n_ph_list2-1) != prev_vowel)
					ph_list2[n_ph_list2-1].stress = stress;  // set stress for previous consonant

				ph_list2[n_ph_list2].synthflags |= SFLAG_SYLLABLE;
				prev_vowel = n_ph_list2;

				if(stress > max_stress)
				{
					max_stress = stress;
					max_stress_ix = n_ph_list2;
				}
				if(next_tone != 0)
				{
					ph_list2[n_ph_list2].tone_number = next_tone;
					next_tone=0;
				}
			}
			else
			{
				if(first_phoneme && langopts.param[LOPT_IT_DOUBLING])
				{
					if(((prev_dict_flags & FLAG_DOUBLING) && (langopts.param[LOPT_IT_DOUBLING] & 1)) || 
						(end_stressed_vowel && (langopts.param[LOPT_IT_DOUBLING] & 2)))
					{
						// italian, double the initial consonant if the previous word ends with a
						// stressed vowel, or is marked with a flag
						ph_list2[n_ph_list2].synthflags |= SFLAG_LENGTHEN;
					}
				}
			}

			ph_list2[n_ph_list2].stress = stress;
			n_ph_list2++;
			first_phoneme = 0;
		}
	}
	plist2->sourceix = source_ix;  // ????

	end_stressed_vowel = 0;
	if((stress >= 4) && (phoneme_tab[ph_list2[n_ph_list2-1].phcode]->type == phVOWEL))
	{
		end_stressed_vowel = 1;   // word ends with a stressed vowel
	}

	if(switch_phonemes >= 0)
	{
		// this word uses a different phoneme table, now switch back
		SelectPhonemeTable(voice->phoneme_tab_ix);
		SetPlist2(&ph_list2[n_ph_list2],phonSWITCH);
		ph_list2[n_ph_list2++].tone_number = voice->phoneme_tab_ix;  // original phoneme table number
	}


	if(pitch_raised > 0)
	{
		embedded_list[embedded_ix++] = EMBED_P+0x60+0x80 + (pitch_raised << 8);  // lower pitch
		SetPlist2(&ph_list2[n_ph_list2],phonPAUSE_SHORT);
		ph_list2[n_ph_list2++].synthflags = SFLAG_EMBEDDED;
	}

	if(flags & FLAG_STRESS_END2)
	{
		// this's word's stress could be increased later
		ph_list2[max_stress_ix].stress |= 0x80;
	}

	prev_dict_flags = flags;
	return(flags);
}  //  end of TranslateWord2



static int EmbeddedCommand(unsigned int &source_index)
{//===================================================
	// An embedded command to change the pitch, volume, etc.
	// returns number of commands added to embedded_list

	// pitch,speed,amplitude,expression,reverb,tone,voice,sayas
	const char *commands = "PSARHTIVYMUBF";
	int value = -1;
	int sign = 0;
	unsigned char c;
	char *p;
	int cmd;

	c = source[source_index];
	if(c == '+')
	{
		sign = 0x40;
		source_index++;
	}
	else
	if(c == '-')
	{
		sign = 0x60;
		source_index++;
	}

	if(isdigit(source[source_index]))
	{
		value = atoi(&source[source_index]);
		while(isdigit(source[source_index]))
			source_index++;
	}

	c = source[source_index++];
	if(embedded_ix >= (N_EMBEDDED_LIST - 2))
		return(0);  // list is full

	if((p = strchr_w(commands,c)) == NULL)
		return(0);
	cmd = (p - commands)+1;
	if(value == -1)
	{
		value = embedded_default[cmd];
		sign = 0;
	}

	if(cmd == EMBED_Y)
	{
		option_sayas2 = value;
		count_sayas_digits = 0;
	}

	embedded_list[embedded_ix++] = cmd + sign + (value << 8);
	return(1);
}  //  end of EmbeddedCommand



int Translator::TranslateChar(char *ptr, int prev_in, unsigned int c, unsigned int next_in, int *insert)
{//=====================================================================================================
	// To allow language specific examination and replacement of characters

	int ix;
	unsigned int word;
	unsigned int new_c, c2;
	int upper_case = 0;
	static int ignore_next = 0;

	if(ignore_next)
	{
		ignore_next = 0;
		return(8);
	}
	if(c == 0) return(0);

	if(langopts.replace_chars == NULL)
		return(c);

	// there is a list of character codes to be substituted with alternative codes

	if(iswupper(c))
	{
		c = towlower(c);
		upper_case = 1;
	}

	new_c = 0;
	for(ix=0; (word = langopts.replace_chars[ix]) != 0; ix++)
	{
		if(c == (word & 0xffff))
		{
			if((word >> 16) == 0)
			{
				new_c = langopts.replacement_chars[ix];
				break;
			}
			if((word >> 16) == tolower(next_in))
			{
				new_c = langopts.replacement_chars[ix];
				ignore_next = 1;
				break;
			}
		}
	}

	if(new_c == 0)
		return(c);    // no substitution

	if(new_c & 0xffe00000)
	{
		// there is a second character to be inserted
		// don't convert the case of the second character unless the next letter is also upper case
		c2 = new_c >> 16;
		if(upper_case && iswupper(next_in))
			c2 = towupper(c2);
		*insert = c2;
		new_c &= 0xffff;
	}
#ifndef PLATFORM_RISCOS
	if(upper_case)
		new_c = towupper(new_c);
#endif
	return(new_c);
}


void *Translator::TranslateClause(FILE *f_text, const void *vp_input, int *tone_out, char **voice_change)
{//======================================================================================================
	int ix;
	int c;
	int cc;
	unsigned int source_index=0;
	unsigned int prev_source_index=0;
	int prev_in;
	int prev_out=' ';
	int prev_out2;
	int prev_in2=0;
	int next_in;
	int char_inserted=0;
	int clause_pause;
	int pre_pause=0;
	int pre_pause_add=0;
	int word_mark = 0;
	int all_upper_case=FLAG_ALL_UPPER;
	int finished;
	int single_quoted;
	int phoneme_mode = 0;
	int dict_flags;        // returned from dictionary lookup
	int word_flags;        // set here
	int next_word_flags;
	int embedded_count = 0;
	int letter_count = 0;
	char *word;
	char *p;
	int j, k;
	int n_digits;

	unsigned short charix[N_TR_SOURCE+1];

	WORD_TAB words[N_CLAUSE_WORDS];
	int word_count=0;      // index into words

	char sbuf[512];

	int terminator;
	int tone;

	p_textinput = (char *)vp_input;
	p_wchar_input = (wchar_t *)vp_input;

	embedded_ix = 0;
	embedded_read = 0;

	if((clause_start_char = count_characters) < 0)
		clause_start_char = 0;
	clause_start_word = count_words + 1;

	for(ix=0; ix<N_TR_SOURCE; ix++)
		charix[ix] = 0;
	terminator = translator->ReadClause(f_text,source,charix,N_TR_SOURCE);

	charix[N_TR_SOURCE] = count_characters;

	clause_pause = (terminator & 0xff) * 10;  // mS
	tone = (terminator & 0xf00) >> 8;

	for(p=source; *p != 0; p++)
	{
		if(!isspace2(*p))
		{
			break;
		}
	}
	if(*p == 0)
	{
		// No characters except spaces. This is not a sentence.
		// Don't add this pause, just make up the previous pause to this value;
		clause_pause -= max_clause_pause;
		if(clause_pause < 0)
			clause_pause = 0;

		terminator &= ~CLAUSE_BIT_SENTENCE;  // clear sentence bit
		max_clause_pause += clause_pause;
	}
	else
	{
		max_clause_pause = clause_pause;
	}

	if(new_sentence)
	{
		count_sentences++;
		if(skip_sentences > 0)
		{
			skip_sentences--;
			if(skip_sentences == 0)
				skipping_text = 0;
		}
	}

	ph_list2[0].phcode = phonPAUSE_SHORT;
   ph_list2[0].stress = 0;
	ph_list2[0].tone_number = 0;
	ph_list2[0].sourceix = 0;
	n_ph_list2 = 1;
	prev_last_stress = 0;
	prepause_timeout = 0;
	word_count = 0;
	single_quoted = 0;
	word_flags = 0;
	next_word_flags = 0;
	expect_verb=0;
	expect_past=0;
	expect_verb_s=0;
	end_stressed_vowel=0;
	prev_dict_flags = 0;

	sbuf[0] = 0;
	sbuf[1] = ' ';
	sbuf[2] = ' ';
	ix = 3;
	prev_in = ' ';

	words[0].start = ix;
	words[0].flags = 0;
	finished = 0;

	for(j=0; charix[j]==0; j++);
	words[0].sourceix = charix[j];
	for(k=j; charix[k]!=0; k++);
	words[0].length = k-j;

	while(!finished && (ix < (int)sizeof(sbuf))&& (n_ph_list2 < N_PHONEME_LIST-4))
	{
		prev_out2 = prev_out;
		utf8_in(&prev_out,&sbuf[ix-1],1);   // prev_out = sbuf[ix-1];

		if(prev_in2 != 0)
		{
			prev_in = prev_in2;
			prev_in2 = 0;
		}
		else
		if(source_index > 0)
		{
			utf8_in(&prev_in,&source[source_index-1],1);  //  prev_in = source[source_index-1];
		}

		if(prev_source_index == source_index)
			char_inserted = 0;   // have done an unget, so discard any character to be inserted

		prev_source_index = source_index;

		if(char_inserted)
		{
			c = char_inserted;
			char_inserted = 0;
		}
		else
		{
			source_index += utf8_in(&cc,&source[source_index],0);   // cc = source[source_index++];
			c = cc;
		}
		utf8_in(&next_in,&source[source_index],0);

		if((c == CTRL_EMBEDDED) || (c == ctrl_embedded))
		{
			// start of embedded command in the text
			int srcix = source_index-1;

			if(prev_in != ' ')
			{
				c = ' ';
				prev_in2 = c;
				source_index--;
			}
			else
			{
				embedded_count += EmbeddedCommand(source_index);
				prev_in2 = prev_in;
				// replace the embedded command by spaces
				memset(&source[srcix],' ',source_index-srcix);
				source_index = srcix;
				continue;
			}
		}

		if(phoneme_mode)
		{
			all_upper_case = FLAG_PHONEMES;

			if((c == ']') && (next_in == ']'))
			{
				phoneme_mode = 0;
				source_index++;
				c = ' ';
			}
		}
		else
		if((option_sayas2 & 0xf0) == SAYAS_DIGITS)
		{
			if(iswdigit(c))
			{
				count_sayas_digits++;
				if(count_sayas_digits > (option_sayas2 & 0xf))
				{
					// break after the specified number of digits
					c = ' ';
					source_index = prev_source_index;  // unget
					count_sayas_digits = 0;
				}
			}
			else
			{
				count_sayas_digits = 0;
				if(iswdigit(prev_out))
				{
					c = ' ';
					source_index = prev_source_index;  // unget
				}
			}
		}
		else
		if((option_sayas2 & 0xf0) != 0x10)
		{
if((c == '/') && (langopts.testing & 2) && isdigit(next_in) && IsAlpha(prev_out))
{
	// TESTING, explicit indication of stressed syllable by /2 after the word
	word_mark = next_in-'0';
	source_index++;
	c = ' ';
}
			if((c == 0x92) || (c == 0xb4) || (c == 0x2019))
				c = '\'';    // 'microsoft' quote or sexed closing single quote - possibly used as apostrophe 

			if((c == '?') && IsAlpha(prev_out) && IsAlpha(next_in))
			{
				// ? between two letters may be a smart-quote replaced by ?
				c = '\'';
			}

			c = TranslateChar(&source[source_index], prev_in,c, next_in, &char_inserted);  // optional language specific function
			if(c == 8)
				continue;  // ignore this character

			if(char_inserted)
				next_in = char_inserted;

			if(!IsAlpha(c) && !iswspace(c) && (c != '\''))
			{
				if(IsAlpha(prev_out))
				{
					if(langopts.tone_numbers && isdigit(c) && !isdigit(next_in))
					{
						// allow a tone number as part of the word
					}
					else
					{
						c = ' ';   // ensure we have an end-of-word terminator
						source_index = prev_source_index;
					}
				}
			}
			if(iswdigit(prev_out))
			{
				if(!iswdigit(c) && (c != '.') && (c != ','))
				{
					c = ' ';   // terminate digit string with a space
					source_index = prev_source_index;
				}
			}

			if((c == '[') && (next_in == '[') && option_phoneme_input)
			{
				phoneme_mode = FLAG_PHONEMES;
				source_index++;
				continue;
			}

			if(c == 0)
			{
				finished = 1;
				c = ' ';
			}
			else
			if(IsAlpha(c))
			{
				if(!IsAlpha(prev_out) || (langopts.ideographs && (c >= 0x3000)))
				{
					if(prev_out != '\'')
						letter_count = 0;    // don't reset count for an apostrophy within a word

					if((prev_out != ' ') && (prev_out != '\''))
					{
						// start of word, insert space if not one there already
						c = ' ';
						source_index = prev_source_index;  // unget
					}
					else
					{
						if(iswupper(c))
							word_flags |= FLAG_FIRST_UPPER;

						if((prev_out == ' ') && iswdigit(sbuf[ix-2]) && !iswdigit(prev_in))
						{
							// word, following a number, but with a space between
							// Add an extra space, to distinguish "2 a" from "2a"
							sbuf[ix++] = ' ';
							words[word_count].start++;
						}
					}
				}

				letter_count++;

				if(iswupper(c))
				{
					c = towlower(c);

					if(iswlower(prev_in))
					{
						c = ' ';      // lower case followed by upper case, treat as new word
						prev_in2 = c;
						source_index = prev_source_index;  // unget
					}
//#ifdef deleted
// changed to break after the last uppercase letter, not before.  See below
					else
					if((c != ' ') && iswupper(prev_in) && iswlower(next_in) &&
							(memcmp(&source[source_index],"s ",2) != 0))          // ENGLISH specific plural
					{
						c = ' ';      // change from upper to lower case, start new word at the last uppercase
						prev_in2 = c;
						source_index = prev_source_index;  // unget
						next_word_flags |= FLAG_NOSPACE;
					}
//#endif
				}
				else
				{
#ifdef deleted
					if(iswupper(prev_in) && iswalpha(prev_out2) &&
						(memcmp(&source[source_index-1],"s ",2) != 0))          // ENGLISH specific plural
					{
// change to break after the last uppercase letter, not before.
						c = ' ';      // more than one upper case followed by lower case, treat as new word
						prev_in2 = c;
						source_index = prev_source_index;  // unget
					}
#endif
					if((all_upper_case) && (letter_count > 2))
					{
						if((c == 's') && (next_in==' '))
						{
							c = ' ';
							all_upper_case |= FLAG_HAS_PLURAL;

							if(sbuf[ix-1] == '\'')
								sbuf[ix-1] = ' ';
						}
						else
							all_upper_case = 0;  // current word contains lower case letters, not "'s"
					}
					else
						all_upper_case = 0;
				}
			}
			else
			if(c=='-')
			{
				if(IsAlpha(prev_in) && IsAlpha(next_in))
				{
					// '-' between two letters is a hyphen, treat as a space
					word_flags |= FLAG_HYPHEN;
					words[word_count-1].flags |= FLAG_HYPHEN_AFTER;
					c = ' ';
				}
				else
				if((prev_in==' ') && (next_in==' '))
				{
					// ' - ' dash between two spaces, treat as pause
					c = ' ';
					pre_pause_add = 4;
				}
				else
				if(next_in=='-')
				{
					// double hyphen, treat as pause
					source_index++;
					c = ' ';
					pre_pause_add = 4;
				}
				else
				if((prev_out == ' ') && IsAlpha(sbuf[ix-2]) && !IsAlpha(prev_in))
				{
					// insert extra space between a word + space + hyphen, to distinguish 'a -2' from 'a-2'
					sbuf[ix++] = ' ';
					words[word_count].start++;
				}
			}
			else
			if(c == '\'')
			{
				if(iswalnum(prev_in) && IsAlpha(next_in))
				{
					// between two letters, consider apostrophe as part of the word
					single_quoted = 0;
				}
				else
				if((wcschr(char_plus_apostrophe,prev_in) != 0) && (prev_out2 == ' '))
				{
					// consider single character plus apostrophe as a word
					single_quoted = 0;
					if(next_in == ' ')
					{
						source_index++;  // skip following space
					}
				}
				else
				{
					if((prev_out == 's') && (single_quoted==0))
					{
						// looks like apostrophe after an 's'
						c = ' ';
					}
					else
					{
						if(iswspace(prev_out))
							single_quoted = 1;
						else
							single_quoted = 0;

						pre_pause_add = 4;   // single quote
						c = ' ';
					}
				}
			}
			else
			if(IsBracket(c))
			{
				pre_pause_add = 4;
				c = ' ';
			}
			else
			if(lookupwchar(breaks,c) != 0)
			{
				c = ' ';  // various characters to treat as space
			}
			else
			if(iswdigit(c))
			{
				if(langopts.tone_numbers && IsAlpha(prev_out) && !isdigit(next_in))
				{
				}
				else
				if((prev_out != ' ') && !iswdigit(prev_out) && (prev_out != langopts.decimal_sep))   // TEST2
				{
					c = ' ';
					source_index = prev_source_index;
				}
				else
				if((prev_out == ' ') && IsAlpha(sbuf[ix-2]) && !IsAlpha(prev_in))
				{
					// insert extra space between a word and a number, to distinguish 'a 2' from 'a2'
					sbuf[ix++] = ' ';
					words[word_count].start++;
				}
			}
		}

		if(iswspace(c))
		{
			if(prev_out == ' ')
			{
				continue;   // multiple spaces
			}

			// end of 'word'
			sbuf[ix++] = ' ';

			if((ix > words[word_count].start) && (word_count < N_CLAUSE_WORDS-1))
			{
				if(embedded_count > 0)
				{
					// there are embedded commands before this word
					embedded_list[embedded_ix-1] |= 0x80;   // terminate list of commands for this word
					words[word_count].flags |= FLAG_EMBEDDED;
					embedded_count = 0;
				}
				words[word_count].pre_pause = pre_pause;
				words[word_count].flags |= (all_upper_case | word_flags);
				words[word_count].wmark = word_mark;

				if(pre_pause > 0)
				{
					// insert an extra space before the word, to prevent influence from previous word across the pause
					for(j=ix; j>words[word_count].start; j--)
					{
						sbuf[j] = sbuf[j-1];
					}
					sbuf[j] = ' ';
					words[word_count].start++;
					ix++;
				}

				word_count++;
				words[word_count].start = ix;
				words[word_count].flags = 0;

				for(j=source_index; charix[j] == 0; j++);
				words[word_count].sourceix = charix[j];
				for(k=j; charix[k]!=0; k++);
				words[word_count].length = k-j;

				word_flags = next_word_flags;
				next_word_flags = 0;
				pre_pause = 0;
				word_mark = 0;
				all_upper_case = FLAG_ALL_UPPER;
			}
		}
		else
		{
			ix += utf8_out(c,&sbuf[ix]);   // sbuf[ix++] = c;
		}
		if(pre_pause_add > pre_pause)
			pre_pause = pre_pause_add;
		pre_pause_add = 0;
	}

	if((word_count==0) && (embedded_count > 0))
	{
		// add a null 'word' to carry the embedded command flag
		embedded_list[embedded_ix-1] |= 0x80; 
		words[word_count].flags |= FLAG_EMBEDDED;
		word_count = 1;
	}

	clause_end = &sbuf[ix-1];
	sbuf[ix] = 0;
	words[0].pre_pause = 0;  // don't add extra pause at beginning of clause
	words[word_count].pre_pause = 8;
	if(word_count > 0)
		words[word_count-1].flags |= FLAG_LAST_WORD;

	for(ix=0; ix<word_count; ix++)
	{
		int j;
		char *pn;
		char *pw;
		char number_buf[80];

		// start speaking at a specified word position in the text?
		count_words++;
		if(skip_words > 0)
		{
			skip_words--;
			if(skip_words == 0)
				skipping_text = 0;
		}
		if(skipping_text)
			continue;


		word = pw = &sbuf[words[ix].start];
		for(n_digits=0; iswdigit(word[n_digits]); n_digits++);  // count consecutive digits
		if((n_digits > 4) && (word[0] != '0'))
		{
			// word is entirely digits, insert commas and break into 3 digit "words"
			number_buf[0] = ' ';
			pn = &number_buf[1];
			j = n_digits;
			while(pn < &number_buf[sizeof(number_buf)-3])
			{
				if(!isdigit(c = *pw++) && (c != langopts.decimal_sep))
					break;

				*pn++ = c;
				if((--j > 0) && (j % 3)==0)
				{
					if(langopts.thousands_sep != ' ')
						*pn++ = langopts.thousands_sep;
					*pn++ = ' ';
				}
			}
			pn[0] = ' ';
			pn[1] = 0;
			word = pw;

			for(pw = &number_buf[1]; pw < pn;)
			{
				TranslateWord2(pw, &words[ix], words[ix].pre_pause,0 );
				while(*pw++ != ' ');
				words[ix].pre_pause = 0;
				words[ix].flags = 0;
			}
		}
		else
		{
			dict_flags = TranslateWord2(word, &words[ix], words[ix].pre_pause, words[ix+1].pre_pause);
			ix += ((dict_flags >> 5) & 7);  // dictionary indicates skip next word(s)

			if((dict_flags & FLAG_DOT) && (ix == word_count-1) && (terminator == CLAUSE_PERIOD))
			{
				// probably an abbreviation such as Mr. or B. rather than end of sentence
				clause_pause = 10;
				tone = 4;
			}
		}
	}

	for(ix=0; ix<2; ix++)
	{
		// terminate the clause with 2 PAUSE phonemes
		ph_list2[n_ph_list2+ix].phcode = phonPAUSE;
   	ph_list2[n_ph_list2+ix].stress = 0;
		ph_list2[n_ph_list2+ix].sourceix = 0;
		ph_list2[n_ph_list2+ix].synthflags = 0;
	}
	n_ph_list2 += 2;

	if(Eof() && ((word_count == 0) || (option_endpause==0)))
	{
		clause_pause = 10;
	}

	MakePhonemeList(clause_pause, new_sentence);

	if(embedded_count)   // ???? is this needed
	{
		phoneme_list[n_phoneme_list-2].synthflags = SFLAG_EMBEDDED;
		embedded_list[embedded_ix-1] |= 0x80;
	}


	prev_clause_pause = clause_pause;

	GetTranslatedPhonemeString(phon_out,sizeof(phon_out));
	*tone_out = tone;

	new_sentence = 0;
	if(terminator & CLAUSE_BIT_SENTENCE)
	{
		new_sentence = 1;  // next clause is a new sentence
	}


	if(voice_change != NULL)
	{
		// return new voice name if an embedded voice change command terminated the clause
		if(terminator & CLAUSE_BIT_VOICE)
			*voice_change = &source[source_index];
		else
			*voice_change = NULL;
	}

	if(Eof() || (vp_input==NULL))
		return(NULL);

	if(option_multibyte == espeakCHARS_WCHAR)
		return((void *)p_wchar_input);
	else
		return((void *)p_textinput);
}  //  end of TranslateClause





void InitText(int control)
{//=======================
	count_sentences = 0;
	count_words = 0;
	end_character_position = 0;
	skip_sentences = 0;
	skip_marker[0] = 0;
	skip_words = 0;
	skip_characters = 0;
	skipping_text = 0;
	new_sentence = 1;

	ungot_char = 0;
	prev_clause_pause = 0;

	option_sayas = 0;
	option_sayas2 = 0;
	option_emphasis = 0;

	InitText2();

	if((control & espeakKEEP_NAMEDATA) == 0)
	{
		InitNamedata();
	}
}

