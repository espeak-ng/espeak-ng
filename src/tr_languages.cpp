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


void SetupTranslator(Translator *tr, int *lengths, int *amps)
{//==========================================================
	if(lengths != NULL)
		memcpy(tr->stress_lengths,lengths,sizeof(tr->stress_lengths));
	if(amps != NULL)
		memcpy(tr->stress_amps,amps,sizeof(tr->stress_amps));
}


Translator *SelectTranslator(const char *name)
{//===========================================
	int name2 = 0;
	Translator *tr;

	// convert name string into a word of up to 4 characters, for the switch()
	while(*name != 0)
		name2 = (name2 << 8) + *name++;

	switch(name2)
	{
	case L('e','n'):
		tr = new Translator_English();
		break;

	case L('a','f'):
		tr = new Translator_Afrikaans();
		break;

	case L('r','u'):  // Russian
			tr = new Translator_Russian();
		break;

	case L('d','e'):
		{
			static const int stress_lengths_de[8] = {145,135, 190,190,  0, 0, 270,285};
			tr = new Translator();
			tr->langopts.stress_rule = 0;
			tr->langopts.word_gap = 1;
			tr->langopts.vowel_pause = 2;
			tr->langopts.param[LOPT_PREFIXES] = 1;
			memcpy(tr->stress_lengths,stress_lengths_de,sizeof(tr->stress_lengths));
		
			tr->langopts.numbers = 0x1c11;
			tr->langopts.thousands_sep = '.';   // and also allow space
			tr->langopts.decimal_sep = ',';
			tr->SetLetterBits(0,"aeiouy");  // A  vowels
		}
		break;

	case L('c','y'):   // Welsh
		{
			static int stress_lengths_cy[8] = {170,230, 190,190, 0, 0, 250,270};
			static int stress_amps_cy[8] = {16,15, 18,18, 0,0, 24,22 };    // 'diminished' is used to mark a quieter, final unstressed syllable

			tr = new Translator();
			SetupTranslator(tr,stress_lengths_cy,stress_amps_cy);

			tr->charset_a0 = charsets[14];   // ISO-8859-14
//			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable
			tr->langopts.stress_rule = 2;

			// 'diminished' is an unstressed final syllable
			tr->langopts.stress_flags =  0x6 | 0x10; 
			tr->langopts.word_gap = 0;
			tr->langopts.vowel_pause = 0;
			tr->langopts.unstressed_wd1 = 0;
			tr->langopts.unstressed_wd2 = 2;
			tr->langopts.param[LOPT_SONORANT_MIN] = 120;  // limit the shortening of sonorants before short vowels

			tr->langopts.numbers = 0x401;

			tr->SetLetterBits(0,"aeiouwy");  // A  vowels
		}
		break;

	case L('i','t'):   // Italian
		{
			static int stress_lengths_it[8] = {150, 140,  180, 180,  0, 0,  270, 320};

			tr = new Translator();
			SetupTranslator(tr,stress_lengths_it,NULL);

			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable
			tr->langopts.stress_rule = 2;
			tr->langopts.word_gap = 0;
			tr->langopts.vowel_pause = 1;
			tr->langopts.unstressed_wd1 = 2;
			tr->langopts.unstressed_wd2 = 2;
			tr->langopts.param[LOPT_IT_LENGTHEN] = 1;    // remove lengthen indicator from unstressed syllables
			tr->langopts.param[LOPT_IT_DOUBLING] = 1;
			tr->langopts.param[LOPT_SONORANT_MIN] = 130;  // limit the shortening of sonorants before short vowels
			tr->langopts.numbers = 0x2701;
			tr->langopts.thousands_sep = '.';
			tr->langopts.decimal_sep = ',';
		}
		break;

	case L('e','s'):   // Spanish
		{
			static int stress_lengths_es[8] = {175, 200,  190, 190,  0, 0,  230, 260};
			static int stress_amps_es[8] = {16,13, 19,19, 20,24, 24,22 };    // 'diminished' is used to mark a quieter, final unstressed syllable

			tr = new Translator();
			SetupTranslator(tr,stress_lengths_es,stress_amps_es);

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

			tr->langopts.numbers = 0x521;
			tr->langopts.thousands_sep = '.';
			tr->langopts.decimal_sep = ',';
		}
		break;

	case L('e','l'):   // Greek
		{
			static int stress_lengths_el[8] = {155, 180,  210, 210,  0, 0,  270, 300};
			static int stress_amps_el[8] = {15,12, 20,20, 20,24, 24,22 };    // 'diminished' is used to mark a quieter, final unstressed syllable

			// character codes offset by 0x380
			#define OFFSET_GREEK  0x380
			static const char el_vowels[] = {0x10,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x35,0x37,0x39,0x3f,0x45,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0};
			static const char el_voiceless[]= {0x38,0x3a,0x3f,0x40,0x42,0x43,0x44,0x46,0x47,0};
			static const char el_consonants[]={0x32,0x33,0x34,0x36,0x38,0x3a,0x3b,0x3c,0x3d,0x3e,0x40,0x41,0x42,0x43,0x44,0x46,0x47,0x48,0};
			static const wchar_t el_char_apostrophe[] = {0x3c3,0};  // σ

			tr = new Translator();
			SetupTranslator(tr,stress_lengths_el,stress_amps_el);

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

			tr->langopts.numbers = 0x501;
			tr->langopts.thousands_sep = '.';
			tr->langopts.decimal_sep = ',';
		}
		break;

	case L('e','o'):
		{
			static int stress_lengths_eo[8] = {150, 150,  180, 180,    0,   0,  260, 310};
			static const wchar_t eo_char_apostrophe[2] = {'l',0};
		
			tr = new Translator();
			SetupTranslator(tr,stress_lengths_eo,NULL);

			tr->charset_a0 = charsets[3];  // ISO-8859-3
			tr->char_plus_apostrophe = eo_char_apostrophe;
		
			tr->langopts.stress_rule = 2;
			tr->langopts.stress_flags = 0x1;  // don't give full stress to monosyllables
			tr->langopts.word_gap = 0;
			tr->langopts.vowel_pause = 0;
			tr->langopts.unstressed_wd1 = 2;
			tr->langopts.unstressed_wd2 = 2;
		}
		break;


	case L('p','l'):   // Polish
		{
			static int stress_lengths_pl[8] = {155, 170,  175, 175,  0, 0,  240, 280};
			static int stress_amps_pl[8] = {16,14, 20,20, 20,24, 24,22 };    // 'diminished' is used to mark a quieter, final unstressed syllable

			tr = new Translator();
			SetupTranslator(tr,stress_lengths_pl,stress_amps_pl);

			tr->charset_a0 = charsets[2];   // ISO-8859-2
			tr->langopts.stress_rule = 2;
			tr->langopts.stress_flags = 0x6;  // mark unstressed final syllables as diminished
			tr->langopts.word_gap = 0;
			tr->langopts.param[LOPT_REGRESSIVE_VOICING] = 1;
			tr->langopts.param[LOPT_UNPRONOUNCABLE] = 1;
			tr->SetLetterBits(0,"aeiouy");  // A  vowels
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


Translator_Russian::Translator_Russian() : Translator()
{//===================================
	static int stress_amps_ru[] = {16,16, 19,19, 20,24, 24,22 };
	static int stress_lengths_ru[8] = {165,140, 190,190, 0,0, 250,260};

	// character codes offset by 0x420
	#define OFFSET_CYRILLIC 0x420
	static const char ru_vowels[] = {0x10,0x15,0x31,0x18,0x1e,0x23,0x2b,0x2d,0x2e,0x2f,0};
	static const char ru_consonants[] = {0x11,0x12,0x13,0x14,0x16,0x17,0x19,0x1a,0x1b,0x1c,0x1d,0x1f,0x20,0x21,0x22,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2c,0};
	static const char ru_soft[] = {0x2c,0x19,0x27,0x29,0};   // letter group B  [k ts; s;]
	static const char ru_hard[] = {0x2a,0x16,0x26,0x28,0};   // letter group H  [S Z ts]
	static const char ru_nothard[] = {0x11,0x12,0x13,0x14,0x17,0x19,0x1a,0x1b,0x1c,0x1d,0x1f,0x20,0x21,0x22,0x24,0x25,0x27,0x29,0x2c,0};
	static const char ru_voiced[] = {0x11,0x12,0x13,0x14,0x16,0x17,0};    // letter group G  (voiced obstruents)
	static const char ru_ivowels[] = {0x2c,0x15,0x31,0x18,0x2e,0x2f,0};   // letter group J  (iotated vowels & soft-sign)

	SetupTranslator(this,stress_lengths_ru,stress_amps_ru);

	charset_a0 = charsets[18];   // KOI8-R
	transpose_offset = 0x42f;  // convert cyrillic from unicode into range 0x01 to 0x22
	transpose_min = 0x430;
	transpose_max = 0x451;

	letter_bits_offset = OFFSET_CYRILLIC;
	memset(letter_bits,0,sizeof(letter_bits));
	SetLetterBits(0,ru_vowels);
	SetLetterBits(1,ru_soft);
	SetLetterBits(2,ru_consonants);
	SetLetterBits(3,ru_hard);
	SetLetterBits(4,ru_nothard);
	SetLetterBits(5,ru_voiced);
	SetLetterBits(6,ru_ivowels);
	SetLetterBits(7,ru_vowels);

	langopts.param[LOPT_UNPRONOUNCABLE] = 0x432;    // [v]  don't count this character at start of word
	langopts.param[LOPT_REGRESSIVE_VOICING] = 2;
	langopts.param[LOPT_KEEP_UNSTR_VOWEL] = 1;
	langopts.stress_rule = 4;

	langopts.numbers = 0x401;
	langopts.thousands_sep = '.';
	langopts.decimal_sep = ',';
	langopts.phoneme_change = 1;
	langopts.testing = 2;

}  // end of Translator_Russian



int Translator_Russian::ChangePhonemes(PHONEME_LIST2 *phlist, int n_ph, int index, PHONEME_TAB *ph, int flags)
{//===========================================================================================================
// Called for each phoneme in the phoneme list, to allow a language to make changes
// flags: bit 0=1 last phoneme in a word
//        bit 1=1 this is the highest stressed vowel in the current word
//        bit 2=1 after the highest stressed vowel in the current word
//        bit 3=1 the phonemes were specified explicitly, or found from an entry in the xx_list dictionary

	int variant;
	int vowelix;
	PHONEME_TAB *prev, *next;

	if(flags & 8)
		return(0);    // full phoneme translation has already been given

	// Russian vowel softening and reduction rules
	if(ph->type == phVOWEL)
	{
		#define N_VOWELS_RU   7
		static unsigned char vowels_ru[N_VOWELS_RU] = {'a','A','o','E','i','u','y'};

		// each line gives: soft, reduced, soft-reduced, post-tonic
		static unsigned short vowel_replace[N_VOWELS_RU][4] = {
			{'&', 'V', 'I', '@'},  // a
			{'&', 'V', 'I', '@'},  // A
			{'8', 'V', 'I', '@'},  // o
			{'e', 'I', 'I', 'I'},  // E
			{'i', 'I', 'I', 'I'},  // i
			{'u'+('"'<<8), 'U', 'U', 'U'},  // u
			{'y', 'Y', 'Y', 'Y'}};  // y

		prev = phoneme_tab[phlist[index-1].phcode];
		next = phoneme_tab[phlist[index+1].phcode];

		// lookup the vowel name to get an index into the vowel_replace[] table
		for(vowelix=0; vowelix<N_VOWELS_RU; vowelix++)
		{
			if(vowels_ru[vowelix] == ph->mnemonic)
				break;
		}
		if(vowelix == N_VOWELS_RU)
			return(0);

		// do we need a variant of this vowel, depending on the stress and adjacent phonemes ?
		variant = -1;
		if(flags & 2)
		{
			// a stressed vowel
			if((prev->phflags & phPALATAL) && ((next->phflags & phPALATAL) || phoneme_tab[phlist[index+2].phcode]->mnemonic == ';'))
			{
				// between two palatal consonants, use the soft variant
				variant = 0;
			}
		}
		else
		{
			// an unstressed vowel
			if(prev->phflags & phPALATAL)
			{
				variant = 2;  // unstressed soft
			}
			else
			if((ph->mnemonic == 'o') && ((prev->phflags & phPLACE) == phPLACE_pla))
			{
				variant = 2;  // unstressed soft  ([o] vowel following:  ш ж
			}
			else
			if(flags & 4)
			{
				variant = 3;  // post tonic
			}
			else
			{
				variant = 1;  // unstressed
			}
		}
		if(variant >= 0)
		{
			phlist[index].phcode = PhonemeCode(vowel_replace[vowelix][variant]);
		}
	}

	return(0);
}

//**********************************************************************************************************



Translator_Afrikaans::Translator_Afrikaans() : Translator()
{//=========================================
// Initialise options for this language

	static const int stress_lengths2[8] = {170,140, 220,220,  0, 0, 250,270};
	langopts.stress_rule = 0;
	langopts.word_gap = 0;
	langopts.vowel_pause = 1;
	langopts.param[LOPT_DIERESES] = 1;
	langopts.param[LOPT_PREFIXES] = 1;
	SetLetterBits(0,"aeiouy");  // A  vowels

	langopts.numbers = 0x451;
	memcpy(stress_lengths,stress_lengths2,sizeof(stress_lengths));
}


int Translator_Afrikaans::TranslateChar(char *ptr, int prev_in, int c, int next_in)
{//===============================================================================
// look for 'n  and replace by a special character (unicode: schwa)

	if(!iswalpha(prev_in))
	{
		if((c == '\'') && (next_in == 'n'))
		{
			// n preceded by either apostrophe or U2019 "right single quotation mark"
			ptr[0] = ' ';  // delete the  n
			return(0x0259); // replace  '  by  unicode schwa character
		}
	}
	return(c);
}


