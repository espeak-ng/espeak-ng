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
 *   along with this program; if not, write see:                           *
 *               <http://www.gnu.org/licenses/>.                           *
 ***************************************************************************/

#include "StdAfx.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>
#include <wchar.h>
#include <math.h>

#include "speak_lib.h"
#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "translate.h"

#ifdef PLATFORM_POSIX
#include <unistd.h>
#endif

#include <locale.h>
#define N_XML_BUF   256


static const char *xmlbase = "";    // base URL from <speak>

static int namedata_ix=0;
static int n_namedata = 0;
char *namedata = NULL;


static FILE *f_input = NULL;
static int ungot_char2 = 0;
unsigned char *p_textinput;
wchar_t *p_wchar_input;
static int ungot_char;
static const char *ungot_word = NULL;
static int end_of_input;

static int ignore_text=0;   // set during <sub> ... </sub>  to ignore text which has been replaced by an alias
static int clear_skipping_text = 0;  // next clause should clear the skipping_text flag
int count_characters = 0;
static int sayas_mode;
static int ssml_ignore_l_angle = 0;

static const char *punct_stop = ".:!?";    // pitch fall if followed by space
static const char *punct_close = ")]}>;'\"";  // always pitch fall unless followed by alnum

// alter tone for announce punctuation or capitals
static const char *tone_punct_on = "\0016T";  // add reverberation, lower pitch
static const char *tone_punct_off = "\001T";

// punctuations symbols that can end a clause
static const unsigned short punct_chars[] = {',','.','?','!',':',';',
  0x2013,  // en-dash
  0x2014,  // em-dash
  0x2026,  // elipsis

  0x037e,  // Greek question mark (looks like semicolon)
  0x0387,  // Greek semicolon, ano teleia
  0x0964,  // Devanagari Danda (fullstop)

  0x0589,  // Armenian period
  0x055d,  // Armenian comma
  0x055c,  // Armenian exclamation
  0x055e,  // Armenian question
  0x055b,  // Armenian emphasis mark

  0x1362,  // Ethiopic period
  0x1363,
  0x1364,
  0x1365,
  0x1366,
  0x1367,
  0x1368,

  0x3001,  // ideograph comma
  0x3002,  // ideograph period

  0xff01,  // fullwidth exclamation
  0xff0c,  // fullwidth comma
  0xff0e,  // fullwidth period
  0xff1a,  // fullwidth colon
  0xff1b,  // fullwidth semicolon
  0xff1f,  // fullwidth question mark
  
  0};


// indexed by (entry num. in punct_chars) + 1
// bits 0-7 pause x 10mS, bits 12-14 intonation type, bit 15 don't need following space or bracket
static const unsigned int punct_attributes [] = { 0,
  CLAUSE_COMMA, CLAUSE_PERIOD, CLAUSE_QUESTION, CLAUSE_EXCLAMATION, CLAUSE_COLON, CLAUSE_SEMICOLON,
  CLAUSE_SEMICOLON,  // en-dash
  CLAUSE_SEMICOLON,  // em-dash
  CLAUSE_SEMICOLON,  // elipsis

  CLAUSE_QUESTION,   // Greek question mark
  CLAUSE_SEMICOLON,  // Greek semicolon
  CLAUSE_PERIOD+0x8000,     // Devanagari Danda (fullstop)

  CLAUSE_PERIOD+0x8000,  // Armenian period
  CLAUSE_COMMA,     // Armenian comma
  CLAUSE_EXCLAMATION + PUNCT_IN_WORD,  // Armenian exclamation
  CLAUSE_QUESTION + PUNCT_IN_WORD,  // Armenian question
  CLAUSE_PERIOD + PUNCT_IN_WORD,  // Armenian emphasis mark

  CLAUSE_PERIOD,     // Ethiopic period
  CLAUSE_COMMA,      // Ethiopic comma
  CLAUSE_SEMICOLON,  // Ethiopic semicolon
  CLAUSE_COLON,      // Ethiopic colon
  CLAUSE_COLON,      // Ethiopic preface colon
  CLAUSE_QUESTION,   // Ethiopic question mark
  CLAUSE_PERIOD,     // Ethiopic paragraph

  CLAUSE_COMMA+0x8000,      // ideograph comma
  CLAUSE_PERIOD+0x8000,     // ideograph period

  CLAUSE_EXCLAMATION+0x8000, // fullwidth
  CLAUSE_COMMA+0x8000,
  CLAUSE_PERIOD+0x8000,
  CLAUSE_COLON+0x8000,
  CLAUSE_SEMICOLON+0x8000,
  CLAUSE_QUESTION+0x8000,

  CLAUSE_SEMICOLON,  // spare
  0 };


// stack for language and voice properties
// frame 0 is for the defaults, before any ssml tags.
typedef struct {
	int tag_type;
	int voice_variant;
	int voice_gender;
	int voice_age;
	char voice_name[40];
	char language[20];
} SSML_STACK;

#define N_SSML_STACK  20
static int n_ssml_stack;
static SSML_STACK ssml_stack[N_SSML_STACK];

static char current_voice_id[40] = {0};


#define N_PARAM_STACK  20
static int n_param_stack;
PARAM_STACK param_stack[N_PARAM_STACK];

static int speech_parameters[N_SPEECH_PARAM];     // current values, from param_stack

const int param_defaults[N_SPEECH_PARAM] = {
   0,     // silence (internal use)
  170,    // rate wpm
  100,    // volume
   50,    // pitch
   50,    // range
   0,     // punctuation
   0,     // capital letters
   0,     // wordgap
   0,     // options
   0,     // intonation
   0,
   0,
   0,     // emphasis
   0,     // line length
   0,     // voice type
};


#ifdef NEED_WCHAR_FUNCTIONS

// additional Latin characters beyond the Latin1 character set
#define MAX_WALPHA  0x233
// indexed by character - 0x100
// 0=not alphabetic, 0xff=lower case, other=value to add to upper case to convert to lower case
static unsigned char walpha_tab[MAX_WALPHA-0xff] = {
      1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,  // 100
      1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,  // 110
      1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,  // 120
   0xff,0xff,   1,0xff,   1,0xff,   1,0xff,0xff,   1,0xff,   1,0xff,   1,0xff,   1,  // 130
   0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,0xff,   1,0xff,   1,0xff,   1,0xff,  // 140
      1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,  // 150
      1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,  // 160
      1,0xff,   1,0xff,   1,0xff,   1,0xff,0xff,   1,0xff,   1,0xff,   1,0xff,0xff,  // 170
   0xff, 210,   1,0xff,   1,0xff, 206,   1,0xff, 205, 205,   1,0xff,0xff,  79, 202,  // 180
    203,   1,0xff, 205, 207,0xff, 211, 209,   1,0xff,0xff,0xff, 211, 213,0xff, 214,  // 190
      1,0xff,   1,0xff,   1,0xff, 218,   1,0xff, 218,0xff,0xff,   1,0xff, 218,   1,  // 1a0
   0xff, 217, 217,   1,0xff,   1,0xff, 219,   1,0xff,0xff,0xff,   1,0xff,0xff,0xff,  // 1b0
   0xff,0xff,0xff,0xff,   2,   1,0xff,   2,   1,0xff,   2,   1,0xff,   1,0xff,   1,  // 1c0
   0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,0xff,   1,0xff,  // 1d0
      1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,  // 1e0
   0xff,   2,   1,0xff,   1,0xff,0xff,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,  // 1f0
      1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,  // 200
      1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,  // 210
   0xff,   0,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,   1,0xff,  // 220
      1,0xff,   1,0xff };    // 230

// use ctype.h functions for Latin1 (character < 0x100)
int iswalpha(int c)
{
	if(c < 0x100)
		return(isalpha(c));
	if((c > 0x3040) && (c <= 0xa700))
		return(1);  // japanese, chinese characters
	if(c > MAX_WALPHA)
		return(0);
	return(walpha_tab[c-0x100]);
}

int iswdigit(int c)
{
	if(c < 0x100)
		return(isdigit(c));
	return(0);
}

int iswalnum(int c)
{
	if(iswdigit(c))
		return(1);
	return(iswalpha(c));
}

int towlower(int c)
{
	int x;
	if(c < 0x100)
		return(tolower(c));
	if((c > MAX_WALPHA) || ((x = walpha_tab[c-0x100])==0xff))
		return(c);  // already lower case
	return(c + x);  // convert to lower case
}

int towupper(int c)
{
	// check whether the previous character code is the upper-case equivalent of this character
	if(tolower(c-1) == c)
		return(c-1);  // yes, use it
	return(c);  // no
}

int iswupper(int c)
{
	int x;
	if(c < 0x100)
		return(isupper(c));
	if(((c > MAX_WALPHA) || (x = walpha_tab[c-0x100])==0) || (x == 0xff))
		return(0);
	return(1);
}

int iswlower(int c)
{
	if(c < 0x100)
		return(islower(c));
	if((c > MAX_WALPHA) || (walpha_tab[c-0x100] != 0xff))
		return(0);
	return(1);
}

int iswspace(int c)
{
	if(c < 0x100)
		return(isspace(c));
	return(0);
}

int iswpunct(int c)
{
	if(c < 0x100)
		return(ispunct(c));
	return(0);
}

const wchar_t *wcschr(const wchar_t *str, int c)
{
   while(*str != 0)
   {
      if(*str == c)
         return(str);
      str++;
   }
   return(NULL);
}

#ifndef WINCE
// wcslen() is provided by WINCE, but not the other wchar functions
const int wcslen(const wchar_t *str)
{
	int ix=0;

	while(*str != 0)
	{
		ix++;
	}
	return(ix);
}
#endif

float wcstod(const wchar_t *str, wchar_t **tailptr)
{
   int ix;
   char buf[80];
   while(isspace(*str)) str++;
   for(ix=0; ix<80; ix++)
   {
      buf[ix] = str[ix];
      if(isspace(buf[ix]))
         break;
   }
   *tailptr = (wchar_t *)&str[ix];
   return(atof(buf));
}
#endif

int towlower2(unsigned int c)
{
	// check for non-standard upper to lower case conversions
	if(c == 'I')
	{
		if(translator->translator_name == L('t','r'))
		{
			c = 0x131;   // I -> ı
		}
	}
	return(towlower(c));
}

static void GetC_unget(int c)
{//==========================
// This is only called with UTF8 input, not wchar input
	if(f_input != NULL)
		ungetc(c,f_input);
	else
	{
		p_textinput--;
		*p_textinput = c;
		end_of_input = 0;
	}
}

int Eof(void)
{//==========
	if(ungot_char != 0)
		return(0);

	if(f_input != 0)
		return(feof(f_input));

	return(end_of_input);
}


static int GetC_get(void)
{//======================
	unsigned int c;
	unsigned int c2;

	if(f_input != NULL)
	{
		c = fgetc(f_input);
		if(feof(f_input)) c = ' ';

		if(option_multibyte == espeakCHARS_16BIT)
		{
			c2 = fgetc(f_input);
			if(feof(f_input)) c2 = 0;
			c = c + (c2 << 8);
		}
		return(c);
	}

	if(option_multibyte == espeakCHARS_WCHAR)
	{
		if(*p_wchar_input == 0)
		{
			end_of_input = 1;
			return(0);
		}

		if(!end_of_input)
			return(*p_wchar_input++);
	}
	else
	{
		if(*p_textinput == 0)
		{
			end_of_input = 1;
			return(0);
		}
	
		if(!end_of_input)
		{
			if(option_multibyte == espeakCHARS_16BIT)
			{
				c = p_textinput[0] + (p_textinput[1] << 8);
				p_textinput += 2;
				return(c);
			}
			return(*p_textinput++ & 0xff);
		}
	}
	return(0);
}


static int GetC(void)
{//==================
// Returns a unicode wide character
// Performs UTF8 checking and conversion

	int c;
	int c1;
	int c2;
	int cbuf[4];
	int ix;
	int n_bytes;
	unsigned char m;
	static int ungot2 = 0;
	static const unsigned char mask[4] = {0xff,0x1f,0x0f,0x07};
	static const unsigned char mask2[4] = {0,0x80,0x20,0x30};

	if((c1 = ungot_char) != 0)
	{
		ungot_char = 0;
		return(c1);
	}

	if(ungot2 != 0)
	{
		c1 = ungot2;
		ungot2 = 0;
	}
	else
	{
		c1 = GetC_get();
	}

	if((option_multibyte == espeakCHARS_WCHAR) || (option_multibyte == espeakCHARS_16BIT))
	{
		count_characters++;
		return(c1);   // wchar_t  text
	}

	if((option_multibyte < 2) && (c1 & 0x80))
	{
		// multi-byte utf8 encoding, convert to unicode
		n_bytes = 0;

		if(((c1 & 0xe0) == 0xc0) && ((c1 & 0x1e) != 0))
			n_bytes = 1;
		else
		if((c1 & 0xf0) == 0xe0)
			n_bytes = 2;
		else
		if(((c1 & 0xf8) == 0xf0) && ((c1 & 0x0f) <= 4))
			n_bytes = 3;

		if((ix = n_bytes) > 0)
		{
			c = c1 & mask[ix];
			m = mask2[ix];
			while(ix > 0)
			{
				if((c2 = cbuf[ix] = GetC_get()) == 0)
				{
					if(option_multibyte==espeakCHARS_AUTO)
						option_multibyte=espeakCHARS_8BIT;   // change "auto" option to "no"
					GetC_unget(' ');
					break;
				}

				if((c2 & 0xc0) != 0x80)
				{
					// This is not UTF8.  Change to 8-bit characterset.
					if((n_bytes == 2) && (ix == 1))
						ungot2 = cbuf[2];
					GetC_unget(c2);
					break;
				}
				m = 0x80;
				c = (c << 6) + (c2 & 0x3f);
				ix--;
			}
			if(ix == 0)
			{
				count_characters++;
				return(c);
			}
		}
		// top-bit-set character is not utf8, drop through to 8bit charset case
		if((option_multibyte==espeakCHARS_AUTO) && !Eof())
			option_multibyte=espeakCHARS_8BIT;   // change "auto" option to "no"
	}

	// 8 bit character set, convert to unicode if
	count_characters++;
	if(c1 >= 0xa0)
		return(translator->charset_a0[c1-0xa0]);
	return(c1);
}  // end of GetC


static void UngetC(int c)
{//======================
	ungot_char = c;
}


static const char *WordToString2(unsigned int word)
{//================================================
// Convert a language mnemonic word into a string
	int  ix;
	static char buf[5];
	char *p;

	p = buf;
	for(ix=3; ix>=0; ix--)
	{
		if((*p = word >> (ix*8)) != 0)
			p++;
	}
	*p = 0;
	return(buf);
}


static const char *LookupSpecial(Translator *tr, const char *string, char* text_out)
{//=================================================================================
	unsigned int flags[2];
	char phonemes[55];
	char phonemes2[55];
	char *string1 = (char *)string;

	if(LookupDictList(tr,&string1,phonemes,flags,0,NULL))
	{
		SetWordStress(tr, phonemes, flags[0], -1, 0);
		DecodePhonemes(phonemes,phonemes2);
		sprintf(text_out,"[[%s]]",phonemes2);
		option_phoneme_input |= 2;
		return(text_out);
	}
	return(NULL);
}


static const char *LookupCharName(Translator *tr, int c)
{//=====================================================
// Find the phoneme string (in ascii) to speak the name of character c
// Used for punctuation characters and symbols

	int ix;
	unsigned int flags[2];
	char single_letter[24];
	char phonemes[60];
	char phonemes2[60];
	const char *lang_name = NULL;
	char *string;
	static char buf[60];

	buf[0] = 0;
	flags[0] = 0;
	flags[1] = 0;
	single_letter[0] = 0;
	single_letter[1] = '_';
	ix = utf8_out(c,&single_letter[2]);
	single_letter[2+ix]=0;

	string = &single_letter[1];
	if(LookupDictList(tr, &string, phonemes, flags, 0, NULL) == 0)
	{
		// try _* then *
		string = &single_letter[2];
		if(LookupDictList(tr, &string, phonemes, flags, 0, NULL) == 0)
		{
			// now try the rules
			single_letter[1] = ' ';
			TranslateRules(tr, &single_letter[2], phonemes, sizeof(phonemes), NULL,0,NULL);
		}
	}

	if((phonemes[0] == 0) && (tr->translator_name != L('e','n')))
	{
		// not found, try English
		SetTranslator2("en");
		string = &single_letter[1];
		single_letter[1] = '_';
		if(LookupDictList(translator2, &string, phonemes, flags, 0, NULL) == 0)
		{
			string = &single_letter[2];
			LookupDictList(translator2, &string, phonemes, flags, 0, NULL);
		}
		if(phonemes[0])
		{
			lang_name = "en";
		}
		else
		{
			SelectPhonemeTable(voice->phoneme_tab_ix);  // revert to original phoneme table
		}
	}

	if(phonemes[0])
	{
		if(lang_name)
		{
			SetWordStress(translator2, phonemes, flags[0], -1, 0);
			DecodePhonemes(phonemes,phonemes2);
			sprintf(buf,"[[_^_%s %s _^_%s]]","en",phonemes2,WordToString2(tr->translator_name));
			SelectPhonemeTable(voice->phoneme_tab_ix);  // revert to original phoneme table
		}
		else
		{
			SetWordStress(tr, phonemes, flags[0], -1, 0);
			DecodePhonemes(phonemes,phonemes2);
			sprintf(buf,"[[%s]] ",phonemes2);
		}
		option_phoneme_input |= 2;
	}
	else
	{
		strcpy(buf,"[[(X1)(X1)(X1)]]");
		option_phoneme_input |= 2;
	}

	return(buf);
}

int Read4Bytes(FILE *f)
{//====================
// Read 4 bytes (least significant first) into a word
	int ix;
	unsigned char c;
	int acc=0;

	for(ix=0; ix<4; ix++)
	{
		c = fgetc(f) & 0xff;
		acc += (c << (ix*8));
	}
	return(acc);
}


static int LoadSoundFile(const char *fname, int index)
{//===================================================
	FILE *f;
	char *p;
	int *ip;
	int  length;
	char fname_temp[100];
	char fname2[sizeof(path_home)+13+40];

	if(fname == NULL)
	{
		// filename is already in the table
		fname = soundicon_tab[index].filename;
	}

	if(fname==NULL)
		return(1);

	if(fname[0] != '/')
	{
		// a relative path, look in espeak-data/soundicons
		sprintf(fname2,"%s%csoundicons%c%s",path_home,PATHSEP,PATHSEP,fname);
		fname = fname2;
	}

	f = NULL;
#ifdef PLATFORM_POSIX
	if((f = fopen(fname,"rb")) != NULL)
	{
		int ix;
		int fd_temp;
		const char *resample;
		int header[3];
		char command[sizeof(fname2)+sizeof(fname2)+40];

		fseek(f,20,SEEK_SET);
		for(ix=0; ix<3; ix++)
			header[ix] = Read4Bytes(f);

		// if the sound file is not mono, 16 bit signed, at the correct sample rate, then convert it
		if((header[0] != 0x10001) || (header[1] != samplerate) || (header[2] != samplerate*2))
		{
			fclose(f);
			f = NULL;

			if(header[2] == samplerate)
				resample = "";
			else
				resample = "polyphase";

			strcpy(fname_temp,"/tmp/espeakXXXXXX");
			if((fd_temp = mkstemp(fname_temp)) >= 0)
			{
				close(fd_temp);
//			sprintf(fname_temp,"%s.wav",tmpnam(NULL));
				sprintf(command,"sox \"%s\" -r %d -w -s -c1 %s %s\n", fname, samplerate, fname_temp, resample);
				if(system(command) == 0)
				{
					fname = fname_temp;
				}
			}
		}
	}
#endif

	if(f == NULL)
	{
		f = fopen(fname,"rb");
		if(f == NULL)
		{
			fprintf(stderr,"Can't read temp file: %s\n",fname);
			return(3);
		}
	}

	length = GetFileLength(fname);
	fseek(f,0,SEEK_SET);
	if((p = (char *)realloc(soundicon_tab[index].data, length)) == NULL)
	{
		fclose(f);
		return(4);
	}
	fread(p,length,1,f);
	fclose(f);
	remove(fname_temp);

	ip = (int *)(&p[40]);
	soundicon_tab[index].length = (*ip) / 2;  // length in samples
	soundicon_tab[index].data = p;
	return(0);
}  //  end of LoadSoundFile


static int LookupSoundicon(int c)
{//==============================
// Find the sound icon number for a punctuation chatacter
	int ix;

	for(ix=N_SOUNDICON_SLOTS; ix<n_soundicon_tab; ix++)
	{
		if(soundicon_tab[ix].name == c)
		{
			if(soundicon_tab[ix].length == 0)
			{
				if(LoadSoundFile(NULL,ix)!=0)
					return(-1);  // sound file is not available
			}
			return(ix);
		}
	}
	return(-1);
}


static int LoadSoundFile2(const char *fname)
{//=========================================
// Load a sound file into one of the reserved slots in the sound icon table
// (if it'snot already loaded)

	int ix;
	static int slot = -1;

	for(ix=0; ix<n_soundicon_tab; ix++)
	{
		if(((soundicon_tab[ix].filename != NULL) && strcmp(fname, soundicon_tab[ix].filename) == 0))
			return(ix);   // already loaded
	}

	// load the file into the next slot
	slot++;
	if(slot >= N_SOUNDICON_SLOTS)
		slot = 0;

	if(LoadSoundFile(fname, slot) != 0)
		return(-1);

	soundicon_tab[slot].filename = (char *)realloc(soundicon_tab[ix].filename, strlen(fname)+1);
	strcpy(soundicon_tab[slot].filename, fname);
	return(slot);
}



static int AnnouncePunctuation(Translator *tr, int c1, int c2, char *buf, int bufix)
{//=================================================================================
	// announce punctuation names
	// c1:  the punctuation character
	// c2:  the following character

	int punct_count;
	const char *punctname;
	int found = 0;
	int soundicon;
	char *p;

	if((soundicon = LookupSoundicon(c1)) >= 0)
	{
		// add an embedded command to play the soundicon
		sprintf(&buf[bufix],"\001%dI ",soundicon);
		UngetC(c2);
		found = 1;
	}
	else
	if((punctname = LookupCharName(tr, c1)) != NULL)
	{
		found = 1;
		if(bufix==0)
		{
			punct_count=1;
			while(c2 == c1)
			{
				punct_count++;
				c2 = GetC();
			}
			UngetC(c2);

			p = &buf[bufix];
			if(punct_count==1)
			{
				sprintf(p,"%s %s %s",tone_punct_on,punctname,tone_punct_off);
			}
			else
			if(punct_count < 4)
			{
				sprintf(p,"\001+10S%s",tone_punct_on);
				while(punct_count-- > 0)
					sprintf(buf,"%s %s",buf,punctname);
				sprintf(p,"%s %s\001-10S",buf,tone_punct_off);
			}
			else
			{
				sprintf(p,"%s %s %d %s %s",
						tone_punct_on,punctname,punct_count,punctname,tone_punct_off);
				return(CLAUSE_COMMA);
			}
		}
		else
		{
			// end the clause now and pick up the punctuation next time
			UngetC(c2);
			if(option_ssml)
			{
				if((c1 == '<') || (c1 == '&'))
					ssml_ignore_l_angle = c1;  // this was &lt; which was converted to <, don't pick it up again as <
			}
			ungot_char2 = c1;
			buf[bufix] = ' ';
			buf[bufix+1] = 0;
		}
	}

	if(found == 0)
		return(-1);

	if(c1 == '-')
		return(CLAUSE_NONE);   // no pause
	if(bufix > 0)
		return(CLAUSE_SHORTCOMMA);
	if((strchr_w(punct_close,c1) != NULL) && !iswalnum(c2))
		return(CLAUSE_SHORTFALL+4);
	if(iswspace(c2) && strchr_w(punct_stop,c1)!=NULL)
		return(punct_attributes[lookupwchar(punct_chars,c1)]);
	
	return(CLAUSE_SHORTCOMMA);
}  //  end of AnnouncePunctuation

#define SSML_SPEAK     1
#define SSML_VOICE     2
#define SSML_PROSODY   3
#define SSML_SAYAS     4
#define SSML_MARK      5
#define SSML_SENTENCE  6
#define SSML_PARAGRAPH 7
#define SSML_PHONEME   8
#define SSML_SUB       9
#define SSML_STYLE    10
#define SSML_AUDIO    11
#define SSML_EMPHASIS 12
#define SSML_BREAK    13
#define SSML_IGNORE_TEXT 14
#define HTML_BREAK    15
#define SSML_CLOSE    0x10   // for a closing tag, OR this with the tag type

// these tags have no effect if they are self-closing, eg. <voice />
static char ignore_if_self_closing[] = {0,1,1,1,1,0,0,0,0,1,1,0,1,0,1,0,0};


static MNEM_TAB ssmltags[] = {
	{"speak", SSML_SPEAK},
	{"voice", SSML_VOICE},
	{"prosody", SSML_PROSODY},
	{"say-as", SSML_SAYAS},
	{"mark", SSML_MARK},
	{"s", SSML_SENTENCE},
	{"p", SSML_PARAGRAPH},
	{"phoneme", SSML_PHONEME},
	{"sub", SSML_SUB},
	{"tts:style", SSML_STYLE},
	{"audio", SSML_AUDIO},
	{"emphasis", SSML_EMPHASIS},
	{"break", SSML_BREAK},
	{"metadata", SSML_IGNORE_TEXT},

	{"br", HTML_BREAK},
	{"li", HTML_BREAK},
	{"img", HTML_BREAK},
	{"td", HTML_BREAK},
	{"h1", SSML_PARAGRAPH},
	{"h2", SSML_PARAGRAPH},
	{"h3", SSML_PARAGRAPH},
	{"h4", SSML_PARAGRAPH},
	{"hr", SSML_PARAGRAPH},
	{"script", SSML_IGNORE_TEXT},
	{"style", SSML_IGNORE_TEXT},
	{NULL,0}};




static const char *VoiceFromStack()
{//================================
// Use the voice properties from the SSML stack to choose a voice, and switch
// to that voice if it's not the current voice
	int ix;
	SSML_STACK *sp;
	const char *v_id;
	int voice_name_specified;
	int voice_found;
	espeak_VOICE voice_select;
	char voice_name[40];
	char language[40];

	strcpy(voice_name,ssml_stack[0].voice_name);
	strcpy(language,ssml_stack[0].language);
	voice_select.age = ssml_stack[0].voice_age;
	voice_select.gender = ssml_stack[0].voice_gender;
	voice_select.variant = ssml_stack[0].voice_variant;
	voice_select.identifier = NULL;

	for(ix=0; ix<n_ssml_stack; ix++)
	{
		sp = &ssml_stack[ix];
		voice_name_specified = 0;

		if((sp->voice_name[0] != 0) && (SelectVoiceByName(NULL,sp->voice_name) != NULL))
		{
			voice_name_specified = 1;
			strcpy(voice_name, sp->voice_name);
			language[0] = 0;
			voice_select.gender = 0;
			voice_select.age = 0;
			voice_select.variant = 0;
		}
		if(sp->language[0] != 0)
		{
			strcpy(language, sp->language);
			if(voice_name_specified == 0)
				voice_name[0] = 0;  // forget a previous voice name if a language is specified
		}
		if(sp->voice_gender != 0)
			voice_select.gender = sp->voice_gender;
		if(sp->voice_age != 0)
			voice_select.age = sp->voice_age;
		if(sp->voice_variant != 0)
			voice_select.variant = sp->voice_variant;
	}

	voice_select.name = voice_name;
	voice_select.languages = language;
	v_id = SelectVoice(&voice_select, &voice_found);
	if(v_id == NULL)
		return("default");
	return(v_id);
}  // end of VoiceFromStack



static void ProcessParamStack(char *outbuf, int &outix)
{//====================================================
// Set the speech parameters from the parameter stack
	int param;
	int ix;
	int value;
	char buf[20];
	int new_parameters[N_SPEECH_PARAM];
	static char cmd_letter[N_SPEECH_PARAM] = {0, 'S','A','P','R', 0, 0, 0, 0, 0, 0, 0, 'F'};  // embedded command letters


	for(param=0; param<N_SPEECH_PARAM; param++)
		new_parameters[param] = -1;

	for(ix=0; ix<n_param_stack; ix++)
	{
		for(param=0; param<N_SPEECH_PARAM; param++)
		{
			if(param_stack[ix].parameter[param] >= 0)
				new_parameters[param] = param_stack[ix].parameter[param];
		}
	}

	for(param=0; param<N_SPEECH_PARAM; param++)
	{
		if((value = new_parameters[param]) != speech_parameters[param])
		{
			buf[0] = 0;

			switch(param)
			{
			case espeakPUNCTUATION:
				option_punctuation = value-1;
				break;

			case espeakCAPITALS:
				option_capitals = value;
				break;

			case espeakRATE:
			case espeakVOLUME:
			case espeakPITCH:
			case espeakRANGE:
			case espeakEMPHASIS:
				sprintf(buf,"%c%d%c",CTRL_EMBEDDED,value,cmd_letter[param]);
				break;
			}

			speech_parameters[param] = new_parameters[param];
			strcpy(&outbuf[outix],buf);
			outix += strlen(buf);
		}
	}
}  // end of ProcessParamStack


static PARAM_STACK *PushParamStack(int tag_type)
{//=============================================
	int  ix;
	PARAM_STACK *sp;

	sp = &param_stack[n_param_stack];
	if(n_param_stack < (N_PARAM_STACK-1))
		n_param_stack++;

	sp->type = tag_type;
	for(ix=0; ix<N_SPEECH_PARAM; ix++)
	{
		sp->parameter[ix] = -1;
	}
	return(sp);
}  //  end of PushParamStack


static void PopParamStack(int tag_type, char *outbuf, int &outix)
{//==============================================================
	// unwind the stack up to and including the previous tag of this type
	int ix;
	int top = 0;

	if(tag_type >= SSML_CLOSE)
		tag_type -= SSML_CLOSE;

	for(ix=0; ix<n_param_stack; ix++)
	{
		if(param_stack[ix].type == tag_type)
		{
			top = ix;
		}
	}
	if(top > 0)
	{
		n_param_stack = top;
	}
	ProcessParamStack(outbuf, outix);
}  // end of PopParamStack



static wchar_t *GetSsmlAttribute(wchar_t *pw, const char *name)
{//============================================================
// Gets the value string for an attribute.
// Returns NULL if the attribute is not present
	int ix;
	static wchar_t empty[1] = {0};

	while(*pw != 0)
	{
		if(iswspace(pw[-1]))
		{
			ix = 0;
			while(*pw == name[ix])
			{
				pw++;
				ix++;
			}
			if(name[ix]==0)
			{
				// found the attribute, now get the value
				while(iswspace(*pw)) pw++;
				if(*pw == '=') pw++;
				while(iswspace(*pw)) pw++;
				if(*pw == '"')
					return(pw+1);
				else
					return(empty);
			}
		}
		pw++;
	}
	return(NULL);
}  //  end of GetSsmlAttribute


static int attrcmp(const wchar_t *string1, const char *string2)
{//============================================================
	int  ix;
	
	if(string1 == NULL)
		return(1);

	for(ix=0; (string1[ix] == string2[ix]) && (string1[ix] != 0); ix++)
	{
	}
	if((string1[ix]=='"') && (string2[ix]==0))
		return(0);
	return(1);
}


static int attrlookup(const wchar_t *string1, const MNEM_TAB *mtab)
{//================================================================
	int ix;

	for(ix=0; mtab[ix].mnem != NULL; ix++)
	{
		if(attrcmp(string1,mtab[ix].mnem) == 0)
			return(mtab[ix].value);
	}
	return(mtab[ix].value);
}


static int attrnumber(const wchar_t *pw, int default_value, int type)
{//==================================================================
	int value = 0;

	if((pw == NULL) || !isdigit(*pw))
		return(default_value);

	while(isdigit(*pw))
	{
		value = value*10 + *pw++ - '0';
	}
	if((type==1) && (towlower(*pw)=='s'))
	{
		// time: seconds rather than ms
		value *= 1000;
	}
	return(value);
}  // end of attrnumber



static int attrcopy_utf8(char *buf, const wchar_t *pw, int len)
{//============================================================
// Convert attribute string into utf8, write to buf, and return its utf8 length
	unsigned int c;
	int ix = 0;
	int n;
	int prev_c = 0;

	if(pw != NULL)
	{
		while((ix < (len-4)) && ((c = *pw++) != 0))
		{
			if((c=='"') && (prev_c != '\\'))
				break;   // " indicates end of attribute, unless preceded by backstroke
			n = utf8_out(c,&buf[ix]);
			ix += n;
			prev_c = c;
		}
	}
	buf[ix] = 0;
	return(ix);
}  // end of attrcopy_utf8



static int attr_prosody_value(int param_type, const wchar_t *pw, int *value_out)
{//=============================================================================
	int sign = 0;
	wchar_t *tail;
	float value;

	while(iswspace(*pw)) pw++;
	if(*pw == '+')
	{
		pw++;
		sign = 1;
	}
	if(*pw == '-')
	{
		pw++;	
		sign = -1;
	}
	value = (float)wcstod(pw,&tail);
	if(tail == pw)
	{
		// failed to find a number, return 100%
		*value_out = 100;
		return(2);
	}

	if(*tail == '%')
	{
		if(sign != 0)
			value = 100 + (sign * value);
		*value_out = (int)value;
		return(2);   // percentage
	}

	if((tail[0]=='s') && (tail[1]=='t'))
	{
		double x;
		// convert from semitones to a  frequency percentage
		x = pow(double(2.0),double((value*sign)/12)) * 100;
		*value_out = (int)x;
		return(2);   // percentage
	}

	if(param_type == espeakRATE)
	{
		*value_out = (int)(value * 100);
		return(2);   // percentage
	}

	*value_out = (int)value;
	return(sign);   // -1, 0, or 1
}  // end of attr_prosody_value


int AddNameData(const char *name, int wide)
{//========================================
// Add the name to the namedata and return its position
// (Used by the Windows SAPI wrapper)
	int ix;
	int len;
	void *vp;

	if(wide)
	{
		len = (wcslen((const wchar_t *)name)+1)*sizeof(wchar_t);
		n_namedata = (n_namedata + sizeof(wchar_t) - 1) % sizeof(wchar_t);  // round to wchar_t boundary
	}
	else
	{
		len = strlen(name)+1;
	}

	if(namedata_ix+len >= n_namedata)
	{
		// allocate more space for marker names
		if((vp = realloc(namedata, namedata_ix+len + 300)) == NULL)
			return(-1);  // failed to allocate, original data is unchanged but ignore this new name

		namedata = (char *)vp;
		n_namedata = namedata_ix+len + 300;
	}
	memcpy(&namedata[ix = namedata_ix],name,len);
	namedata_ix += len;
	return(ix);
}  //  end of AddNameData


void SetVoiceStack(espeak_VOICE *v)
{//================================
	SSML_STACK *sp;
	sp = &ssml_stack[0];

	if(v == NULL)
	{
		memset(sp,0,sizeof(ssml_stack[0]));
		return;
	}
	if(v->languages != NULL)
		strcpy(sp->language,v->languages);
	if(v->name != NULL)
		strcpy(sp->voice_name,v->name);
	sp->voice_variant = v->variant;
	sp->voice_age = v->age;
	sp->voice_gender = v->gender;
}


static int GetVoiceAttributes(wchar_t *pw, int tag_type)
{//=====================================================
// Determines whether voice attribute are specified in this tag, and if so, whether this means
// a voice change.
// If it's a closing tag, delete the top frame of the stack and determine whether this implies
// a voice change.
// Returns  CLAUSE_BIT_VOICE if there is a voice change

	wchar_t *lang;
	wchar_t *gender;
	wchar_t *name;
	wchar_t *age;
	wchar_t *variant;
	const char *new_voice_id;
	SSML_STACK *ssml_sp;

	static const MNEM_TAB mnem_gender[] = {
		{"male", 1},
		{"female", 2},
		{"neutral", 3},
		{NULL, 0}};

	if(tag_type & SSML_CLOSE)
	{
		// delete a stack frame
		if(n_ssml_stack > 1)
		{
			n_ssml_stack--;
		}
	}
	else
	{
		// add a stack frame if any voice details are specified
		lang = GetSsmlAttribute(pw,"xml:lang");

		if(tag_type != SSML_VOICE)
		{
			// only expect an xml:lang attribute
			name = NULL;
			variant = NULL;
			age = NULL;
			gender = NULL;
		}
		else
		{
			name = GetSsmlAttribute(pw,"name");
			variant = GetSsmlAttribute(pw,"variant");
			age = GetSsmlAttribute(pw,"age");
			gender = GetSsmlAttribute(pw,"gender");
		}
	
		if((tag_type != SSML_VOICE) && (lang==NULL))
			return(0);  // <s> or <p> without language spec, nothing to do
	
		ssml_sp = &ssml_stack[n_ssml_stack++];

		attrcopy_utf8(ssml_sp->language,lang,sizeof(ssml_sp->language));
		attrcopy_utf8(ssml_sp->voice_name,name,sizeof(ssml_sp->voice_name));
		ssml_sp->voice_variant = attrnumber(variant,1,0)-1;
		ssml_sp->voice_age = attrnumber(age,0,0);
		ssml_sp->voice_gender = attrlookup(gender,mnem_gender);
		ssml_sp->tag_type = tag_type;
	}

	new_voice_id = VoiceFromStack();
	if(strcmp(new_voice_id,current_voice_id) != 0)
	{
		// add an embedded command to change the voice
		strcpy(current_voice_id,new_voice_id);
		return(CLAUSE_BIT_VOICE);    // change of voice
	}

	return(0);
}  //  end of GetVoiceAttributes


static void SetProsodyParameter(int param_type, wchar_t *attr1, PARAM_STACK *sp)
{//=============================================================================
	int value;
	int sign;

	static const MNEM_TAB mnem_volume[] = {
		{"default",100},
		{"silent",0},
		{"x-soft",30},
		{"soft",65},
		{"medium",100},
		{"loud",150},
		{"x-loud",230},
		{NULL, -1}};

	static const MNEM_TAB mnem_rate[] = {
		{"default",100},
		{"x-slow",60},
		{"slow",80},
		{"medium",100},
		{"fast",120},
		{"x-fast",150},
		{NULL, -1}};

	static const MNEM_TAB mnem_pitch[] = {
		{"default",100},
		{"x-low",70},
		{"low",85},
		{"medium",100},
		{"high",110},
		{"x-high",120},
		{NULL, -1}};

	static const MNEM_TAB mnem_range[] = {
		{"default",100},
		{"x-low",20},
		{"low",50},
		{"medium",100},
		{"high",140},
		{"x-high",180},
		{NULL, -1}};

	static const MNEM_TAB *mnem_tabs[5] = {
		NULL, mnem_rate, mnem_volume, mnem_pitch, mnem_range };


	if((value = attrlookup(attr1,mnem_tabs[param_type])) >= 0)
	{
		// mnemonic specifies a value as a percentage of the base pitch/range/rate/volume
		sp->parameter[param_type] = (param_stack[0].parameter[param_type] * value)/100;
	}
	else
	{
		sign = attr_prosody_value(param_type,attr1,&value);

		if(sign == 0)
			sp->parameter[param_type] = value;   // absolute value in Hz
		else
		if(sign == 2)
		{
			// change specified as percentage or in semitones
			sp->parameter[param_type] = (speech_parameters[param_type] * value)/100;
		}
		else
		{
			// change specified as plus or minus Hz
			sp->parameter[param_type] = speech_parameters[param_type] + (value*sign);
		}
	}
}  // end of SetProsodyParemeter



static int ProcessSsmlTag(wchar_t *xml_buf, char *outbuf, int &outix, int n_outbuf, int self_closing)
{//==================================================================================================
// xml_buf is the tag and attributes with a zero terminator in place of the original '>'
// returns a clause terminator value.

	unsigned int ix;
	int index;
	int c;
	int tag_type;
	int value;
	int value2;
	int value3;
	int voice_change_flag;
	wchar_t *px;
	wchar_t *attr1;
	wchar_t *attr2; 
	wchar_t *attr3;
	int terminator;
	char *uri;
	int param_type;
	char tag_name[40];
	char buf[80];
	PARAM_STACK *sp;
	SSML_STACK *ssml_sp;

	static const MNEM_TAB mnem_punct[] = {
		{"none", 1},
		{"all", 2},
		{"some", 3},
		{NULL, -1}};

	static const MNEM_TAB mnem_capitals[] = {
		{"no", 0},
		{"spelling", 2},
		{"icon", 1},
		{"pitch", 20},  // this is the amount by which to raise the pitch
		{NULL, -1}};

	static const MNEM_TAB mnem_interpret_as[] = {
		{"characters",SAYAS_CHARS},
		{"tts:char",SAYAS_SINGLE_CHARS},
		{"tts:key",SAYAS_KEY},
		{"tts:digits",SAYAS_DIGITS},
		{"telephone",SAYAS_DIGITS1},
		{NULL, -1}};

	static const MNEM_TAB mnem_sayas_format[] = {
		{"glyphs",1},
		{NULL, -1}};

	static const MNEM_TAB mnem_break[] = {
		{"none",0},
		{"x-weak",1},
		{"weak",2},
		{"medium",3},
		{"strong",4},
		{"x-strong",5},
		{NULL,-1}};

	static const MNEM_TAB mnem_emphasis[] = {
		{"none",1},
		{"reduced",2},
		{"moderate",3},
		{"strong",4},
		{NULL,-1}};

	static const char *prosody_attr[5] = {
	 NULL, "rate", "volume", "pitch", "range" };

	for(ix=0; ix<(sizeof(tag_name)-1); ix++)
	{
		if(((c = xml_buf[ix]) == 0) || iswspace(c))
			break;
		tag_name[ix] = tolower((char)c);
	}
	tag_name[ix] = 0;

	px = &xml_buf[ix];   // the tag's attributes
	
	if(tag_name[0] == '/')
	{
		tag_type = LookupMnem(ssmltags,&tag_name[1]) + SSML_CLOSE;  // closing tag
	}
	else
	{
		tag_type = LookupMnem(ssmltags,tag_name);

		if(self_closing && ignore_if_self_closing[tag_type])
			return(0);
	}

	voice_change_flag = 0;
	terminator = CLAUSE_NONE;
	ssml_sp = &ssml_stack[n_ssml_stack-1];

	switch(tag_type)
	{
	case SSML_STYLE:
		sp = PushParamStack(tag_type);
		attr1 = GetSsmlAttribute(px,"field");
		attr2 = GetSsmlAttribute(px,"mode");


		if(attrcmp(attr1,"punctuation")==0)
		{
			value = attrlookup(attr2,mnem_punct);
			sp->parameter[espeakPUNCTUATION] = value;
		}
		else
		if(attrcmp(attr1,"capital_letters")==0)
		{
			value = attrlookup(attr2,mnem_capitals);
			sp->parameter[espeakCAPITALS] = value;
		}
		ProcessParamStack(outbuf, outix);
		break;

	case SSML_PROSODY:
		sp = PushParamStack(tag_type);

		// look for attributes:  rate, volume, pitch, range
		for(param_type=espeakRATE; param_type <= espeakRANGE; param_type++)
		{
			if((attr1 = GetSsmlAttribute(px,prosody_attr[param_type])) != NULL)
			{
				SetProsodyParameter(param_type, attr1, sp);
			}
		}

		ProcessParamStack(outbuf, outix);
		break;

	case SSML_EMPHASIS:
		sp = PushParamStack(tag_type);
		value = 3;   // default is "moderate"
		if((attr1 = GetSsmlAttribute(px,"level")) != NULL)
		{
			value = attrlookup(attr1,mnem_emphasis);
		}

		if(translator->langopts.tone_language == 1)
		{
			static unsigned char emphasis_to_pitch_range[] = {50,50,40,70,90,90};
			static unsigned char emphasis_to_volume[] = {100,100,70,110,140,140};
			// tone language (eg.Chinese) do emphasis by increasing the pitch range.
			sp->parameter[espeakRANGE] = emphasis_to_pitch_range[value];
			sp->parameter[espeakVOLUME] = emphasis_to_volume[value];
		}
		else
		{
			sp->parameter[espeakEMPHASIS] = value;
		}
		ProcessParamStack(outbuf, outix);
		break;

	case SSML_STYLE + SSML_CLOSE:
	case SSML_PROSODY + SSML_CLOSE:
	case SSML_EMPHASIS + SSML_CLOSE:
		PopParamStack(tag_type, outbuf, outix);
		break;

	case SSML_SAYAS:
		attr1 = GetSsmlAttribute(px,"interpret-as");
		attr2 = GetSsmlAttribute(px,"format");
		attr3 = GetSsmlAttribute(px,"detail");
		value = attrlookup(attr1,mnem_interpret_as);
		value2 = attrlookup(attr2,mnem_sayas_format);
		if(value2 == 1)
			value = SAYAS_GLYPHS;

		value3 = attrnumber(attr3,0,0);

		if(value == SAYAS_DIGITS)
		{
			if(value3 <= 1)
				value = SAYAS_DIGITS1;
			else
				value = SAYAS_DIGITS + value3;
		}

		sprintf(buf,"%c%dY",CTRL_EMBEDDED,value);
		strcpy(&outbuf[outix],buf);
		outix += strlen(buf);

		sayas_mode = value;   // punctuation doesn't end clause during SAY-AS
		break;

	case SSML_SAYAS + SSML_CLOSE:
		outbuf[outix++] = CTRL_EMBEDDED;
		outbuf[outix++] = 'Y';
		sayas_mode = 0;
		break;

	case SSML_SUB:
		if((attr1 = GetSsmlAttribute(px,"alias")) != NULL)
		{
			// use the alias  rather than the text
			ignore_text = 1;
			outix += attrcopy_utf8(&outbuf[outix],attr1,n_outbuf-outix);
		}
		break;

	case SSML_IGNORE_TEXT:
		ignore_text = 1;
		break;

	case SSML_SUB + SSML_CLOSE:
	case SSML_IGNORE_TEXT + SSML_CLOSE:
		ignore_text = 0;
		break;

	case SSML_MARK:
		if((attr1 = GetSsmlAttribute(px,"name")) != NULL)
		{
			// add name to circular buffer of marker names
			attrcopy_utf8(buf,attr1,sizeof(buf));

			if(strcmp(skip_marker,buf)==0)
			{
				// This is the marker we are waiting for before starting to speak
				clear_skipping_text = 1;
				skip_marker[0] = 0;
				return(CLAUSE_NONE);
			}

			if((index = AddNameData(buf,0)) >= 0)
			{
				sprintf(buf,"%c%dM",CTRL_EMBEDDED,index);
				strcpy(&outbuf[outix],buf);
				outix += strlen(buf);
			}
		}
		break;

	case SSML_AUDIO:
		sp = PushParamStack(tag_type);

		if((attr1 = GetSsmlAttribute(px,"src")) != NULL)
		{
			char fname[256];
			attrcopy_utf8(buf,attr1,sizeof(buf));

			if(uri_callback == NULL)
			{
				if((xmlbase != NULL) && (buf[0] != '/'))
				{
					sprintf(fname,"%s/%s",xmlbase,buf);
					index = LoadSoundFile2(fname);
				}
				else
				{
					index = LoadSoundFile2(buf);
				}
				if(index >= 0)
				{
					sprintf(buf,"%c%dI",CTRL_EMBEDDED,index);
					strcpy(&outbuf[outix],buf);
					outix += strlen(buf);
					sp->parameter[espeakSILENCE] = 1;
				}
			}
			else
			{
				if((index = AddNameData(buf,0)) >= 0)
				{
					uri = &namedata[index];
					if(uri_callback(1,uri,xmlbase) == 0)
					{
						sprintf(buf,"%c%dU",CTRL_EMBEDDED,index);
						strcpy(&outbuf[outix],buf);
						outix += strlen(buf);
						sp->parameter[espeakSILENCE] = 1;
					}
				}
			}
		}
		ProcessParamStack(outbuf, outix);

		if(self_closing)
			PopParamStack(tag_type, outbuf, outix);
		return(CLAUSE_NONE);

	case SSML_AUDIO + SSML_CLOSE:
		PopParamStack(tag_type, outbuf, outix);
		return(CLAUSE_NONE);

	case SSML_BREAK:
		value = 21;
		terminator = CLAUSE_NONE;

		if((attr1 = GetSsmlAttribute(px,"strength")) != NULL)
		{
			static int break_value[6] = {0,7,14,21,40,80};  // *10mS
			value = attrlookup(attr1,mnem_break);
			if(value < 3)
			{
				// adjust prepause on the following word
				sprintf(&outbuf[outix],"%c%dB",CTRL_EMBEDDED,value);
				outix += 3;
				terminator = 0;
			}
			value = break_value[value];
		}
		if((attr2 = GetSsmlAttribute(px,"time")) != NULL)
		{
			value = (attrnumber(attr2,0,1) * 25) / speed.speed_factor1; // compensate for speaking speed to keep constant pause length

			if(terminator == 0)
				terminator = CLAUSE_NONE;
		}
		if(terminator)
		{
			if(value > 0xfff)
				value = 0xfff;
			return(terminator + value);
		}
		break;

	case SSML_SPEAK:
		if((attr1 = GetSsmlAttribute(px,"xml:base")) != NULL)
		{
			attrcopy_utf8(buf,attr1,sizeof(buf));
			if((index = AddNameData(buf,0)) >= 0)
			{
				xmlbase = &namedata[index];
			}
		}
		if(GetVoiceAttributes(px, tag_type) == 0)
			return(0);   // no voice change
		return(CLAUSE_VOICE);

	case SSML_VOICE:
		if(GetVoiceAttributes(px, tag_type) == 0)
			return(0);   // no voice change
		return(CLAUSE_VOICE);

	case SSML_SPEAK + SSML_CLOSE:
		// unwind stack until the previous <voice> or <speak> tag
		while((n_ssml_stack > 1) && (ssml_stack[n_ssml_stack-1].tag_type != SSML_SPEAK))
		{
			n_ssml_stack--;
		}
		return(CLAUSE_PERIOD + GetVoiceAttributes(px, tag_type));

	case SSML_VOICE + SSML_CLOSE:
		// unwind stack until the previous <voice> or <speak> tag
		while((n_ssml_stack > 1) && (ssml_stack[n_ssml_stack-1].tag_type != SSML_VOICE))
		{
			n_ssml_stack--;
		}

terminator=0;  // ??  Sentence intonation, but no pause ??
		return(terminator + GetVoiceAttributes(px, tag_type));

	case HTML_BREAK:
	case HTML_BREAK + SSML_CLOSE:
		return(CLAUSE_COLON);

	case SSML_SENTENCE:
		if(ssml_sp->tag_type == SSML_SENTENCE)
		{
			// new sentence implies end-of-sentence
			voice_change_flag = GetVoiceAttributes(px, SSML_SENTENCE+SSML_CLOSE);
		}
		voice_change_flag |= GetVoiceAttributes(px, tag_type);
		return(CLAUSE_PARAGRAPH + voice_change_flag);


	case SSML_PARAGRAPH:
		if(ssml_sp->tag_type == SSML_SENTENCE)
		{
			// new paragraph implies end-of-sentence or end-of-paragraph
			voice_change_flag = GetVoiceAttributes(px, SSML_SENTENCE+SSML_CLOSE);
		}
		if(ssml_sp->tag_type == SSML_PARAGRAPH)
		{
			// new paragraph implies end-of-sentence or end-of-paragraph
			voice_change_flag |= GetVoiceAttributes(px, SSML_PARAGRAPH+SSML_CLOSE);
		}
		voice_change_flag |= GetVoiceAttributes(px, tag_type);
		return(CLAUSE_PARAGRAPH + voice_change_flag);


	case SSML_SENTENCE + SSML_CLOSE:
		if(ssml_sp->tag_type == SSML_SENTENCE)
		{
			// end of a sentence which specified a language
			voice_change_flag = GetVoiceAttributes(px, tag_type);
		}
		return(CLAUSE_PERIOD + voice_change_flag);


	case SSML_PARAGRAPH + SSML_CLOSE:
		if((ssml_sp->tag_type == SSML_SENTENCE) || (ssml_sp->tag_type == SSML_PARAGRAPH))
		{
			// End of a paragraph which specified a language.
			// (End-of-paragraph also implies end-of-sentence)
			return(GetVoiceAttributes(px, tag_type) + CLAUSE_PARAGRAPH);
		}
		return(CLAUSE_PARAGRAPH);
	}
	return(0);
}  // end of ProcessSsmlTag


static MNEM_TAB xml_char_mnemonics[] = {
	{"gt",'>'},
	{"lt",'<'},
	{"amp", '&'},
	{"quot", '"'},
	{"nbsp", ' '},
	{"apos", '\''},
	{NULL,-1}};


int ReadClause(Translator *tr, FILE *f_in, char *buf, short *charix, int *charix_top, int n_buf, int *tone_type)
{//=============================================================================================================
/* Find the end of the current clause.
	Write the clause into  buf

	returns: clause type (bits 0-7: pause x10mS, bits 8-11 intonation type)

	Also checks for blank line (paragraph) as end-of-clause indicator.

	Does not end clause for:
		punctuation immediately followed by alphanumeric  eg.  1.23  !Speak  :path
		repeated punctuation, eg.   ...   !!!
*/
	int c1=' ';  // current character
	int c2;  // next character
	int cprev=' ';  // previous character
	int parag;
	int ix = 0;
	int j;
	int nl_count;
	int linelength = 0;
	int phoneme_mode = 0;
	int n_xml_buf;
	int terminator;
	int punct;
	int found;
	int any_alnum = 0;
	int self_closing;
	int punct_data;
	int stressed_word = 0;
	const char *p;
	wchar_t xml_buf[N_XML_BUF+1];

#define N_XML_BUF2   20
	char xml_buf2[N_XML_BUF2+2];           // for &<name> and &<number> sequences
	static char ungot_string[N_XML_BUF2+4];
	static int ungot_string_ix = -1;

	if(clear_skipping_text)
	{
		skipping_text = 0;
		clear_skipping_text = 0;
	}

	tr->clause_upper_count = 0;
	tr->clause_lower_count = 0;
	end_of_input = 0;
	*tone_type = 0;

f_input = f_in;  // for GetC etc

	if(ungot_word != NULL)
	{
		strcpy(buf,ungot_word);
		ix += strlen(ungot_word);
		ungot_word = NULL;
	}

	if(ungot_char2 != 0)
	{
		c2 = ungot_char2;
	}
	else
	{
		c2 = GetC();
	}

	while(!Eof() || (ungot_char != 0) || (ungot_char2 != 0) || (ungot_string_ix >= 0))
	{
		if(!iswalnum(c1))
		{
			if((end_character_position > 0) && (count_characters > end_character_position))
			{
				end_of_input = 1;
				return(CLAUSE_EOF);
			}

			if((skip_characters > 0) && (count_characters > skip_characters))
			{
				// reached the specified start position
				// don't break a word
				clear_skipping_text = 1;
				skip_characters = 0;
				UngetC(c2);
				return(CLAUSE_NONE);
			}
		}

		cprev = c1;
		c1 = c2;

		if(ungot_string_ix >= 0)
		{
			if(ungot_string[ungot_string_ix] == 0)
				ungot_string_ix = -1;
		}

		if((ungot_string_ix == 0) && (ungot_char2 == 0))
		{
			c1 = ungot_string[ungot_string_ix++];
		}
		if(ungot_string_ix >= 0)
		{
			c2 = ungot_string[ungot_string_ix++];
		}
		else
		{
			c2 = GetC();

			if(Eof())
			{
				c2 = ' ';
			}
		}
		ungot_char2 = 0;

		if((option_ssml) && (phoneme_mode==0))
		{
			if((ssml_ignore_l_angle != '&') && (c1 == '&') && ((c2=='#') || ((c2 >= 'a') && (c2 <= 'z'))))
			{
				n_xml_buf = 0;
				c1 = c2;
				while(!Eof() && (iswalnum(c1) || (c1=='#')) && (n_xml_buf < N_XML_BUF2))
				{
					xml_buf2[n_xml_buf++] = c1;
					c1 = GetC();
				}
				xml_buf2[n_xml_buf] = 0;
				c2 = GetC();
				sprintf(ungot_string,"%s%c%c",&xml_buf2[0],c1,c2);

				if(c1 == ';')
				{
					if(xml_buf2[0] == '#')
					{
						// character code number
						if(xml_buf2[1] == 'x')
							found = sscanf(&xml_buf2[2],"%x",(unsigned int *)(&c1));
						else
							found = sscanf(&xml_buf2[1],"%d",&c1);
					}
					else
					{
						if((found = LookupMnem(xml_char_mnemonics,xml_buf2)) != -1)
						{
							c1 = found;
							if(c2 == 0)
								c2 = ' ';
						}
					}
				}
				else
				{
					found = -1;
				}

				if(found <= 0)
				{
					ungot_string_ix = 0;
					c1 = '&';
					c2 = ' ';
				}

				if((c1 <= 0x20) && ((sayas_mode == SAYAS_SINGLE_CHARS) || (sayas_mode == SAYAS_KEY)))
				{
					c1 += 0xe000;  // move into unicode private usage area
				}
			}
			else
			if((c1 == '<') && (ssml_ignore_l_angle != '<'))
			{
				if(c2 == '!')
				{
					// a comment, ignore until closing '<'
					while(!Eof() && (c1 != '>'))
					{
						c1 = GetC();
					}
					c2 = ' ';
				}
				else
				if((c2 == '/') || iswalpha(c2))
				{
					// SSML Tag
					n_xml_buf = 0;
					c1 = c2;
					while(!Eof() && (c1 != '>') && (n_xml_buf < N_XML_BUF))
					{
						xml_buf[n_xml_buf++] = c1;
						c1 = GetC();
					}
					xml_buf[n_xml_buf] = 0;
					c2 = ' ';
		
					buf[ix++] = ' ';
		
					self_closing = 0;
					if(xml_buf[n_xml_buf-1] == '/')
					{
						// a self-closing tag
						xml_buf[n_xml_buf-1] = ' ';
						self_closing = 1;
					}
		
					terminator = ProcessSsmlTag(xml_buf,buf,ix,n_buf,self_closing);
		
					if(terminator != 0)
					{
						buf[ix] = ' ';
						buf[ix++] = 0;
		
						if(terminator & CLAUSE_BIT_VOICE)
						{
							// a change in voice, write the new voice name to the end of the buf
							p = current_voice_id;
							while((*p != 0) && (ix < (n_buf-1)))
							{
								buf[ix++] = *p++;
							}
							buf[ix++] = 0;
						}
						return(terminator);
					}
					continue;
				}
			}
		}
		ssml_ignore_l_angle=0;

		if(ignore_text)
			continue;

		if((c2=='\n') && (option_linelength == -1))
		{
			// single-line mode, return immediately on NL
			if((punct = lookupwchar(punct_chars,c1)) == 0)
			{
				charix[ix] = count_characters - clause_start_char;
				*charix_top = ix;
				ix += utf8_out(c1,&buf[ix]);
				terminator = CLAUSE_PERIOD;  // line doesn't end in punctuation, assume period
			}
			else
			{
				terminator = punct_attributes[punct];
			}
			buf[ix] = ' ';
			buf[ix+1] = 0;
			return(terminator);
		}

		if((c1 == CTRL_EMBEDDED) || (c1 == ctrl_embedded))
		{
			// an embedded command. If it's a voice change, end the clause
			if(c2 == 'V')
			{
				buf[ix++] = 0;      // end the clause at this point
				while(!iswspace(c1 = GetC()) && !Eof() && (ix < (n_buf-1)))
					buf[ix++] = c1;  // add voice name to end of buffer, after the text
				buf[ix++] = 0;
				return(CLAUSE_VOICE);
			}
			else
			if(c2 == 'B')
			{
				// set the punctuation option from an embedded command
				//  B0     B1     B<punct list><space>
				strcpy(&buf[ix],"   ");
				ix += 3;

				if((c2 = GetC()) == '0')
					option_punctuation = 0;
				else
				{
					option_punctuation = 1;
					option_punctlist[0] = 0;
					if(c2 != '1')
					{
						// a list of punctuation characters to be spoken, terminated by space
						j = 0;
						while(!iswspace(c2) && !Eof())
						{
							option_punctlist[j++] = c2;
							c2 = GetC();
							buf[ix++] = ' ';
						}
						option_punctlist[j] = 0;  // terminate punctuation list
						option_punctuation = 2;
					}
				}
				c2 = GetC();
				continue;
			}
		}

		linelength++;

		if(iswalnum(c1))
			any_alnum = 1;
		else
		{
			if(stressed_word)
			{
				stressed_word = 0;
				c1 = CHAR_EMPHASIS;   // indicate this word is stressed
				UngetC(c2);
				c2 = ' ';
			}

			if(iswspace(c1))
			{
				char *p_word;
	
				if(tr->translator_name == 0x6a626f)
				{
					// language jbo : lojban
					// treat "i" or ".i" as end-of-sentence
					p_word = &buf[ix-1];
					if(p_word[0] == 'i')
					{
						if(p_word[-1] == '.')
							p_word--;
						if(p_word[-1] == ' ')
						{
							ungot_word = "i ";
							UngetC(c2);
							p_word[0] = 0;
							return(CLAUSE_PERIOD);
						}
					}
				}
			}
		}

		if(iswupper(c1))
		{
			tr->clause_upper_count++;
			if((option_capitals == 2) && (sayas_mode == 0) && !iswupper(cprev))
			{
				char text_buf[40];
				char text_buf2[30];
				if(LookupSpecial(tr, "_cap", text_buf2) != NULL)
				{
					sprintf(text_buf,"%s%s%s",tone_punct_on,text_buf2,tone_punct_off);
					j = strlen(text_buf);
					if((ix + j) < n_buf)
					{
						strcpy(&buf[ix],text_buf);
						ix += j;
					}
				}
			}
		}
		else
		if(iswalpha(c1))
			tr->clause_lower_count++;

		if(option_phoneme_input)
		{
			if(phoneme_mode > 0)
				phoneme_mode--;
			else
			if((c1 == '[') && (c2 == '['))
				phoneme_mode = -1;     // input is phoneme mnemonics, so don't look for punctuation
			else
			if((c1 == ']') && (c2 == ']'))
				phoneme_mode = 2;      // set phoneme_mode to zero after the next two characters
		}

		if(c1 == '\n')
		{
			parag = 0;

			// count consecutive newlines, ignoring other spaces
			while(!Eof() && iswspace(c2))
			{
				if(c2 == '\n')
					parag++;
				c2 = GetC();
			}
			if(parag > 0)
			{
				// 2nd newline, assume paragraph
				UngetC(c2);

				buf[ix] = ' ';
				buf[ix+1] = 0;
				if(parag > 3)
					parag = 3;
if(option_ssml) parag=1;
				return((CLAUSE_PARAGRAPH-30) + 30*parag);  // several blank lines, longer pause
			}

			if(linelength <= option_linelength)
			{
				// treat lines shorter than a specified length as end-of-clause
				UngetC(c2);
				buf[ix] = ' ';
				buf[ix+1] = 0;
				return(CLAUSE_COLON);
			}

			linelength = 0;
		}

		if(option_punctuation && (phoneme_mode==0) && (sayas_mode==0) && iswpunct(c1))
		{
			// option is set to explicitly speak punctuation characters
			// if a list of allowed punctuation has been set up, check whether the character is in it
			if((option_punctuation == 1) || (wcschr(option_punctlist,c1) != NULL))
			{
				if((terminator = AnnouncePunctuation(tr, c1, c2, buf, ix)) >= 0)
					return(terminator);
			}
		}

		if((phoneme_mode==0) && (sayas_mode==0) && ((punct = lookupwchar(punct_chars,c1)) != 0))
		{
			punct_data = punct_attributes[punct];

			if(punct_data & PUNCT_IN_WORD)
			{
				// Armenian punctuation inside a word
				stressed_word = 1;
				*tone_type = punct_data >> 12 & 0xf;   // override the end-of-sentence type
				continue;
			}

			if((iswspace(c2) || (punct_data & 0x8000) || IsBracket(c2) || (c2=='?') || (c2=='-') || Eof()))
			{
				// note: (c2='?') is for when a smart-quote has been replaced by '?'
				buf[ix] = ' ';
				buf[ix+1] = 0;
	
				if((c1 == '.') && (cprev == '.'))
				{
					c1 = 0x2026;
					punct = 9;   // elipsis
				}
	
				nl_count = 0;
				while(!Eof() && iswspace(c2))
				{
					if(c2 == '\n')
						nl_count++;
					c2 = GetC();   // skip past space(s)
				}
				if(!Eof())
				{
					UngetC(c2);
				}
	
				if((nl_count==0) && (c1 == '.'))
				{
					if(iswdigit(cprev) && (tr->langopts.numbers & 0x10000) && islower(c2))
					{
						// dot after a number indicates an ordinal number
						c2 = '.';
						continue;
					}
					if(iswlower(c2))
					{
						c2 = ' ';
						continue;  // next word has no capital letter, this dot is probably from an abbreviation
					}
					if(any_alnum==0)
					{
						c2 = ' ';   // no letters or digits yet, so probably not a sentence terminator
						continue;
					}
				}
	
				punct_data = punct_attributes[punct];
				if(nl_count > 1)
				{
					if((punct_data == CLAUSE_QUESTION) || (punct_data == CLAUSE_EXCLAMATION))
						return(punct_data + 35);   // with a longer pause
					return(CLAUSE_PARAGRAPH);
				}
				return(punct_data);   // only recognise punctuation if followed by a blank or bracket/quote
			}
		}

		if(speech_parameters[espeakSILENCE]==1)
			continue;

		j = ix+1;
		ix += utf8_out(c1,&buf[ix]);    //	buf[ix++] = c1;
		if(!iswspace(c1) && !IsBracket(c1))
		{
			charix[ix] = count_characters - clause_start_char;
			while(j < ix)
				charix[j++] = -1;   // subsequent bytes of a multibyte character
		}
		*charix_top = ix;

		if(((ix > (n_buf-20)) && !IsAlpha(c1) && !iswdigit(c1))  ||  (ix >= (n_buf-2)))
		{
			// clause too long, getting near end of buffer, so break here
			// try to break at a word boundary (unless we actually reach the end of buffer).
			buf[ix] = ' ';
			buf[ix+1] = 0;
			UngetC(c2);
			return(CLAUSE_NONE);
		}
	}

	if(stressed_word)
	{
		ix += utf8_out(CHAR_EMPHASIS, &buf[ix]);
	}
	buf[ix] = ' ';
	buf[ix+1] = 0;
	return(CLAUSE_EOF);   //  end of file
}  //  end of ReadClause


void InitNamedata(void)
{//====================
	namedata_ix = 0;
	if(namedata != NULL)
	{
		free(namedata);
		namedata = NULL;
		n_namedata = 0;
	}
}


void InitText2(void)
{//=================
	int param;

	ungot_char = 0;

	n_ssml_stack =1;
	n_param_stack = 1;
	ssml_stack[0].tag_type = 0;

	for(param=0; param<N_SPEECH_PARAM; param++)
		speech_parameters[param] = param_stack[0].parameter[param];   // set all speech parameters to defaults

	option_punctuation = speech_parameters[espeakPUNCTUATION];
	option_capitals = speech_parameters[espeakCAPITALS];

	current_voice_id[0] = 0;

	ignore_text = 0;
	clear_skipping_text = 0;
	count_characters = -1;
	sayas_mode = 0;

	xmlbase = NULL;
}

