/***************************************************************************
 *   Copyright (C) 2005,2006 by Jonathan Duddington                        *
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


#include <stdio.h>
#include <ctype.h>
#include <wctype.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <wctype.h>

#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"
#include "tr_languages.h"



#define L(c1,c2)  (c1<<8)+c2

Translator *SelectTranslator(const char *name)
{//===========================================
	int name2 = 0;
	Translator *tr;

	while(*name != 0)
		name2 = (name2 << 8) + *name++;

	switch(name2)
	{
	case L('e','n'):
		tr = new Translator_English();
		break;

	case L('d','e'):
		tr = new Translator_German();
		break;

	case L('a','f'):
		tr = new Translator_Afrikaans();
		break;

	case L('c','y'):   // Welsh
		{
			static int stress_lengths_cy[8] = {200,200, 220,220, 220,240, 248,270};
			tr = new Translator();
			tr->charset_a0 = charsets[14];   // ISO-8859-14
			tr->langopts.stress_rule = 2;
			tr->SetLetterBits(0,"aeiouwy");  // A  vowels
			memcpy(tr->stress_lengths,stress_lengths_cy,sizeof(tr->stress_lengths));
		}
		break;

	case L('i','t'):   // Italian
		{
			static int stress_lengths_it[8] = {150, 140,  180, 180,  0, 0,  270, 320};
			tr = new Translator();
			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable
			tr->langopts.stress_rule = 2;
			tr->langopts.word_gap = 0;
			tr->langopts.vowel_pause = 1;
			tr->langopts.unstressed_wd1 = 2;
			tr->langopts.unstressed_wd2 = 2;
			tr->langopts.param[LOPT_IT_LENGTHEN] = 1;    // remove lengthen indicator from unstressed syllables
			tr->langopts.param[LOPT_IT_DOUBLING] = 1;
			tr->langopts.param[LOPT_SONORANT_MIN] = 130;  // limit the shortening of sonorants before short vowels
			memcpy(tr->stress_lengths,stress_lengths_it,sizeof(tr->stress_lengths));
		}
		break;

	case L('e','s'):   // Spanish
		{
			static int stress_lengths_es[8] = {175, 200,  190, 190,  0, 0,  260, 290};
			static int stress_amps_es[8] = {18,13, 20,20, 20,24, 24,22 };    // 'diminished' is used to mark a quieter, final unstressed syllable
			tr = new Translator();
			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable
			tr->langopts.stress_rule = 2;

			// stress last syllable if it doesn't end in vowel or "s" or "n"
			// 'diminished' is an unstressed final syllable
			tr->langopts.stress_flags = 0x8 | 0x6 | 0x10; 
			tr->langopts.word_gap = 0;
			tr->langopts.vowel_pause = 0;
			tr->langopts.unstressed_wd1 = 0;
			tr->langopts.unstressed_wd2 = 2;
			tr->langopts.param[LOPT_SONORANT_MIN] = 120;  // limit the shortening of sonorants before short vowels
			memcpy(tr->stress_lengths,stress_lengths_es,sizeof(tr->stress_lengths));
			memcpy(tr->stress_amps,stress_amps_es,sizeof(tr->stress_amps));
		}
		break;

	case L('e','l'):   // Greek
		{
			static int stress_lengths_el[8] = {155, 180,  230, 230,  0, 0,  270, 300};
			static int stress_amps_el[8] = {15,12, 20,20, 20,24, 24,22 };    // 'diminished' is used to mark a quieter, final unstressed syllable

			// character codes offset by 0x380
			#define OFFSET_GREEK  0x380
			static const char el_vowels[] = {0x10,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x35,0x37,0x39,0x3f,0x45,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0};
			static const char el_voiceless[]= {0x38,0x3a,0x3f,0x40,0x42,0x43,0x44,0x46,0x47,0};
			static const char el_consonants[]={0x32,0x33,0x34,0x36,0x38,0x3a,0x3b,0x3c,0x3d,0x3e,0x40,0x41,0x42,0x43,0x44,0x46,0x47,0x48,0};
			static const wchar_t el_char_apostrophe[] = {0x3c3,0};  // σ


			tr = new Translator();
			tr->charset_a0 = charsets[7];   // ISO-8859-7
			tr->char_plus_apostrophe = el_char_apostrophe;

			tr->letter_bits_offset = OFFSET_GREEK;
			memset(tr->letter_bits,0,sizeof(tr->letter_bits));
			tr->SetLetterBits(0,el_vowels);
			tr->SetLetterBits(1,el_voiceless);
			tr->SetLetterBits(2,el_consonants);

			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable
			tr->langopts.stress_rule = 2;
			tr->langopts.stress_flags = 0x6;  // mark unstressed final syllables as diminished
			tr->langopts.word_gap = 0;
			tr->langopts.vowel_pause = 0;
			tr->langopts.unstressed_wd1 = 0;
			tr->langopts.unstressed_wd2 = 2;
			tr->langopts.param[LOPT_SONORANT_MIN] = 130;  // limit the shortening of sonorants before short vowels
			memcpy(tr->stress_lengths,stress_lengths_el,sizeof(tr->stress_lengths));
			memcpy(tr->stress_amps,stress_amps_el,sizeof(tr->stress_amps));
		}
		break;

	case L('e','o'):
		{
			static int stress_lengths_eo[8] = {150, 150,  180, 180,    0,   0,  260, 310};
			static const wchar_t eo_char_apostrophe[2] = {'l',0};
		
			tr = new Translator();
			tr->charset_a0 = charsets[3];  // ISO-8859-3
			tr->char_plus_apostrophe = eo_char_apostrophe;
		
			tr->langopts.stress_rule = 2;
			tr->langopts.stress_flags = 0x1;  // don't give full stress to monosyllables
			tr->langopts.word_gap = 3;
			tr->langopts.vowel_pause = 1;
			tr->langopts.unstressed_wd1 = 2;
			tr->langopts.unstressed_wd2 = 2;
			memcpy(tr->stress_lengths,stress_lengths_eo,sizeof(tr->stress_lengths));
		}
		break;


	case L('p','l'):   // Polish
		{
			static int stress_lengths_pl[8] = {180, 180,  190, 190,  200, 200,  250, 250};
			tr = new Translator();
			tr->charset_a0 = charsets[2];   // ISO-8859-2
			tr->langopts.stress_rule = 2;
			tr->langopts.vowel_pause = 0;
			tr->langopts.word_gap = 0;
			tr->langopts.param[LOPT_REGRESSIVE_VOICING] = 1;
			tr->langopts.param[LOPT_UNPRONOUNCABLE] = 1;
			tr->SetLetterBits(0,"aeiouy");  // A  vowels
			memcpy(tr->stress_lengths,stress_lengths_pl,sizeof(tr->stress_lengths));
		}
		break;

	case L('x','x'):
		tr = new Translator_Tone();  // for testing
		break;

	default:
		tr = new Translator();
		break;
	}

	return(tr);
}  // end of SelectTranslator






//**********************************************************************************************************



Translator_German::Translator_German() : Translator()
{//===================================
// Initialise options for this language

	static const int stress_lengths2[8] = {150,140, 220,220, 240,240, 248,250};
	langopts.stress_rule = 0;
	langopts.word_gap = 1;
	langopts.vowel_pause = 2;
	langopts.param[LOPT_PREFIXES] = 1;
	memcpy(stress_lengths,stress_lengths2,sizeof(stress_lengths));
}



//**********************************************************************************************************



Translator_Afrikaans::Translator_Afrikaans() : Translator()
{//=========================================
// Initialise options for this language

	static const int stress_lengths2[8] = {170,140, 220,220, 220,240, 248,250};
	langopts.stress_rule = 0;
	langopts.word_gap = 0;
	langopts.vowel_pause = 1;
	langopts.param[LOPT_DIERESES] = 1;
	langopts.param[LOPT_PREFIXES] = 1;
	SetLetterBits(0,"aeiouy");  // A  vowels

	memcpy(stress_lengths,stress_lengths2,sizeof(stress_lengths));
}


int Translator_Afrikaans::TranslateChar(char *ptr, int prev_in, int c, int next_in)
{//===============================================================================
// look for 'n  and replace by a special character (unicode: schwa)
	static const int schwa = 0x0259;

	if(!iswalpha(prev_in))
	{
		if((c == '\'') && (next_in == 'n'))
		{
			// n preceded by either apostrophe or U2019 "right single quotation mark"
			ptr[0] = ' ';  // delete the  n
			return(schwa); // replace  '  by  schwa character
		}
	}
	return(c);
}


