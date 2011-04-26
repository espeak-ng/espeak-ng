/***************************************************************************
 *   Copyright (C) 2005 to 2011 by Jonathan Duddington                     *
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
#include <wctype.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <wctype.h>

#include "speak_lib.h"
#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"



#define L_qa   0x716100
#define L_grc  0x677263   // grc  Ancient Greek
#define L_jbo  0x6a626f   // jbo  Lojban
#define L_pap  0x706170   // pap  Papiamento]
#define L_qvi  0x717669   // qvi  Kichwa
#define L_shs  0x736873   // shs  Shuswap / Secwepemctsin
#define L_zhy  0x7a6879   // zhy

// start of unicode pages for character sets
#define OFFSET_GREEK    0x380
#define OFFSET_CYRILLIC 0x420
#define OFFSET_ARMENIAN 0x530
#define OFFSET_ARABIC   0x600
#define OFFSET_THAANA   0x780      // Divehi/Maldives
#define OFFSET_DEVANAGARI  0x900
#define OFFSET_BENGALI  0x980
#define OFFSET_GURMUKHI 0xa00
#define OFFSET_GUJARATI 0xa80
#define OFFSET_ORIYA    0xb00
#define OFFSET_TAMIL    0xb80
#define OFFSET_TELUGU   0xc00
#define OFFSET_KANNADA  0xc80
#define OFFSET_MALAYALAM 0xd00
#define OFFSET_SINHALA  0x0d80
#define OFFSET_GEORGIAN 0x1080

#define OFFSET_KOREAN   0x1100

static void Translator_Russian(Translator *tr);



static void SetLetterVowel(Translator *tr, int c)
{//==============================================
	tr->letter_bits[c] = (tr->letter_bits[c] & 0x40) | 0x81;  // keep value for group 6 (front vowels e,i,y)
}

static void ResetLetterBits(Translator *tr, int groups)
{//====================================================
// Clear all the specified groups
	unsigned int ix;
	unsigned int mask;

	mask = ~groups;

	for(ix=0; ix<sizeof(tr->letter_bits); ix++)
	{
		tr->letter_bits[ix] &= mask;
	}
}

static void SetLetterBits(Translator *tr, int group, const char *string)
{//=====================================================================
	int bits;
	unsigned char c;
	
	bits = (1L << group);
	while((c = *string++) != 0)
		tr->letter_bits[c] |= bits;
}

static void SetLetterBitsRange(Translator *tr, int group, int first, int last)
{//===========================================================================
	int bits;
	int ix;

	bits = (1L << group);
	for(ix=first; ix<=last; ix++)
	{
		tr->letter_bits[ix] |= bits;
	}
}


static Translator* NewTranslator(void)
{//===================================
	Translator *tr;
	int ix;
	static const unsigned char stress_amps2[] = {18,18, 20,20, 20,22, 22,20 };
	static const short stress_lengths2[8] = {182,140, 220,220, 220,240, 260,280};
	static const wchar_t empty_wstring[1] = {0};
	static const wchar_t punct_in_word[2] = {'\'', 0};  // allow hyphen within words
	static const unsigned char default_tunes[6] = {0, 1, 2, 3, 0, 0};

	tr = (Translator *)Alloc(sizeof(Translator));
	if(tr == NULL)
		return(NULL);

	tr->charset_a0 = charsets[1];   // ISO-8859-1, this is for when the input is not utf8
	dictionary_name[0] = 0;
	tr->dict_condition=0;
	tr->data_dictrules = NULL;     // language_1   translation rules file
	tr->data_dictlist = NULL;      // language_2   dictionary lookup file

	tr->transpose_min = 'a';
	tr->transpose_max = 'z';
	tr->frequent_pairs = NULL;

	// only need lower case
	tr->letter_bits_offset = 0;
	memset(tr->letter_bits,0,sizeof(tr->letter_bits));
	memset(tr->letter_groups,0,sizeof(tr->letter_groups));

	// 0-5 sets of characters matched by A B C H F G Y  in pronunciation rules
	// these may be set differently for different languages
	SetLetterBits(tr,0,"aeiou");  // A  vowels, except y
	SetLetterBits(tr,1,"bcdfgjklmnpqstvxz");      // B  hard consonants, excluding h,r,w
	SetLetterBits(tr,2,"bcdfghjklmnpqrstvwxz");  // C  all consonants
	SetLetterBits(tr,3,"hlmnr");                 // H  'soft' consonants
	SetLetterBits(tr,4,"cfhkpqstx");             // F  voiceless consonants
	SetLetterBits(tr,5,"bdgjlmnrvwyz");   // G voiced
	SetLetterBits(tr,6,"eiy");   // Letter group Y, front vowels
	SetLetterBits(tr,7,"aeiouy");  // vowels, including y


	tr->char_plus_apostrophe = empty_wstring;
	tr->punct_within_word = punct_in_word;

	for(ix=0; ix<8; ix++)
	{
		tr->stress_amps[ix] = stress_amps2[ix];
		tr->stress_amps_r[ix] = stress_amps2[ix] - 1;
		tr->stress_lengths[ix] = stress_lengths2[ix];
	}
	memset(&(tr->langopts),0,sizeof(tr->langopts));
	tr->langopts.max_lengthmod = 500;

	tr->langopts.stress_rule = STRESSPOSN_2R;
	tr->langopts.unstressed_wd1 = 1;
	tr->langopts.unstressed_wd2 = 3;
	tr->langopts.param[LOPT_SONORANT_MIN] = 95;
	tr->langopts.param[LOPT_LONG_VOWEL_THRESHOLD] = 190/2;
	tr->langopts.param[LOPT_MAXAMP_EOC] = 19;
	tr->langopts.param[LOPT_UNPRONOUNCABLE] = 's';    // don't count this character at start of word
	tr->langopts.param[LOPT_BRACKET_PAUSE] = 4;      // pause at bracket
	tr->langopts.param2[LOPT_BRACKET_PAUSE] = 2;    // pauses when announcing bracket names
	tr->langopts.max_initial_consonants = 3;
	tr->langopts.replace_chars = NULL;
	tr->langopts.ascii_language = "";    // Non-Latin alphabet languages, use this language to speak Latin words, default is English


	SetLengthMods(tr,201);
//	tr->langopts.length_mods = length_mods_en;
//	tr->langopts.length_mods0 = length_mods_en0;

	tr->langopts.long_stop = 100;

	tr->langopts.max_roman = 49;
	tr->langopts.min_roman = 2;
	tr->langopts.thousands_sep = ',';
	tr->langopts.decimal_sep = '.';
	tr->langopts.break_numbers = BREAK_THOUSANDS;   // 1000, 1000,000  1,000,000 etc
	tr->langopts.max_digits = 14;

	memcpy(tr->punct_to_tone, punctuation_to_tone, sizeof(tr->punct_to_tone));

	memcpy(tr->langopts.tunes, default_tunes, sizeof(tr->langopts.tunes));

	return(tr);
}

// common letter pairs, encode these as a single byte
//  2 bytes, using the transposed character codes
static const short pairs_ru[] = { 
0x010c, //  ла   21052  0x23
0x010e, //  на   18400
0x0113, //  та   14254
0x0301, //  ав   31083
0x030f, //  ов   13420
0x060e, //  не   21798
0x0611, //  ре   19458
0x0903, //  ви   16226
0x0b01, //  ак   14456
0x0b0f, //  ок   17836
0x0c01, //  ал   13324
0x0c09, //  ил   16877
0x0e01, //  ан   15359
0x0e06, //  ен   13543  0x30
0x0e09, //  ин   17168
0x0e0e, //  нн   15973
0x0e0f, //  он   22373
0x0e1c, //  ын   15052
0x0f03, //  во   24947
0x0f11, //  ро   13552
0x0f12, //  со   16368
0x100f, //  оп   19054
0x1011, //  рп   17067
0x1101, //  ар   23967
0x1106, //  ер   18795
0x1109, //  ир   13797
0x110f, //  ор   21737
0x1213, //  тс   25076
0x1220, //  яс   14310
0x7fff};
//0x040f  ог   12976
//0x1306  ет   12826
//0x0f0d  мо   12688



static const unsigned int replace_cyrillic_latin[] = 
	{0x430,'a',
	0x431,'b',
	0x446,'c',
	0x45b,0x107,
	0x447,0x10d,
	0x45f,'d'+(0x17e<<16),
	0x455,'d'+('z'<<16),
	0x434,'d',
	0x452,0x111,
	0x435,'e',
	0x444,'f',
	0x433,'g',
	0x445,'h',
	0x438,'i',
	0x458,'j',
	0x43a,'k',
	0x459,'l'+('j'<<16),
	0x43b,'l',
	0x43c,'m',
	0x45a,'n'+('j'<<16),
	0x43d,'n',
	0x43e,'o',
	0x43f,'p',
	0x440,'r',
	0x441,'s',
	0x448,0x161,
	0x442,'t',
	0x443,'u',
	0x432,'v',
	0x437,'z',
	0x436,0x17e,
	0x453,0x111,
	0x45c,0x107,
0};  // ѓ  ѕ  ќ

static void SetCyrillicLetters(Translator *tr)
{//===========================================
	// character codes offset by 0x420
	static const char ru_vowels[] = {0x10,0x15,0x31,0x18,0x1e,0x23,0x2b,0x2d,0x2e,0x2f,0};
	static const char ru_consonants[] = {0x11,0x12,0x13,0x14,0x16,0x17,0x19,0x1a,0x1b,0x1c,0x1d,0x1f,0x20,0x21,0x22,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2c,0};
	static const char ru_soft[] = {0x2c,0x19,0x27,0x29,0};   // letter group B  [k ts; s;]
	static const char ru_hard[] = {0x2a,0x16,0x26,0x28,0};   // letter group H  [S Z ts]
	static const char ru_nothard[] = {0x11,0x12,0x13,0x14,0x17,0x19,0x1a,0x1b,0x1c,0x1d,0x1f,0x20,0x21,0x22,0x24,0x25,0x27,0x29,0x2c,0};
	static const char ru_voiced[] = {0x11,0x12,0x13,0x14,0x16,0x17,0};    // letter group G  (voiced obstruents)
	static const char ru_ivowels[] = {0x2c,0x2e,0x2f,0x31,0};   // letter group Y  (iotated vowels & soft-sign)
	tr->charset_a0 = charsets[18];   // KOI8-R
	tr->transpose_min = 0x430;  // convert cyrillic from unicode into range 0x01 to 0x22
	tr->transpose_max = 0x451;
	tr->frequent_pairs = pairs_ru;

	tr->letter_bits_offset = OFFSET_CYRILLIC;
	memset(tr->letter_bits,0,sizeof(tr->letter_bits));
	SetLetterBits(tr,LETTERGP_A,ru_vowels);
	SetLetterBits(tr,1,ru_soft);
	SetLetterBits(tr,2,ru_consonants);
	SetLetterBits(tr,3,ru_hard);
	SetLetterBits(tr,4,ru_nothard);
	SetLetterBits(tr,5,ru_voiced);
	SetLetterBits(tr,6,ru_ivowels);
	SetLetterBits(tr,LETTERGP_VOWEL2,ru_vowels);
}  // end of SetCyrillicLetters


void SetIndicLetters(Translator *tr)
{//=================================
	// Set letter types for Indic scripts, Devanagari, Tamill, etc
	static const char dev_consonants2[] = {0x02,0x03,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f};

	memset(tr->letter_bits,0,sizeof(tr->letter_bits));
	SetLetterBitsRange(tr,LETTERGP_A,0x04,0x14);   // vowel letters
	SetLetterBitsRange(tr,LETTERGP_A,0x3e,0x4d);   // + vowel signs, and virama
	SetLetterBitsRange(tr,LETTERGP_A,0x55,0x57);   // + vowel signs

	SetLetterBitsRange(tr,LETTERGP_B,0x3e,0x4d);   // vowel signs, and virama
	SetLetterBitsRange(tr,LETTERGP_B,0x55,0x57);   // + vowel signs

	SetLetterBitsRange(tr,LETTERGP_C,0x15,0x39);   // the main consonant range
	SetLetterBits(tr,LETTERGP_C,dev_consonants2);  // + additional consonants

	SetLetterBitsRange(tr,LETTERGP_Y,0x04,0x14);   // vowel letters
	SetLetterBitsRange(tr,LETTERGP_Y,0x3e,0x4c);   // + vowel signs
	SetLetterBitsRange(tr,LETTERGP_Y,0x55,0x57);   // + vowel signs

	tr->langopts.param[LOPT_UNPRONOUNCABLE] = 1;   // disable check for unpronouncable words
	tr->langopts.suffix_add_e = tr->letter_bits_offset + 0x4d;   //virama
}


void SetupTranslator(Translator *tr, const short *lengths, const unsigned char *amps)
{//==================================================================================
	if(lengths != NULL)
		memcpy(tr->stress_lengths,lengths,sizeof(tr->stress_lengths));
	if(amps != NULL)
		memcpy(tr->stress_amps,amps,sizeof(tr->stress_amps));
}


Translator *SelectTranslator(const char *name)
{//===========================================
	int name2 = 0;
	Translator *tr;

	static const short stress_lengths_fr[8] = {190, 170,  190, 200,  0, 0,  190, 240};
	static const unsigned char stress_amps_fr[8] = {18,16, 18,18, 18,18, 18,18 };

	static const unsigned char stress_amps_sk[8] = {17,17, 20,20, 20,22, 22,21 };
	static const short stress_lengths_sk[8] = {190,190, 210,210, 0,0, 210,210};

	static const short stress_lengths_ta[8] = {200, 200,  210, 210,  0, 0,  230, 230};
	static const unsigned char stress_amps_ta[8] = {18,18, 18,18, 20,20, 22,22 };

	// convert name string into a word of up to 4 characters, for the switch()
	while(*name != 0)
		name2 = (name2 << 8) + *name++;

	tr = NewTranslator();

	switch(name2)
	{
	case L('a','f'):
		{
			static const short stress_lengths_af[8] = {170,140, 220,220,  0, 0, 250,270};
			SetupTranslator(tr,stress_lengths_af,NULL);

			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.vowel_pause = 0x30;
			tr->langopts.param[LOPT_DIERESES] = 1;
			tr->langopts.param[LOPT_PREFIXES] = 1;
			SetLetterVowel(tr,'y');  // add 'y' to vowels
		
			tr->langopts.numbers = NUM_SWAP_TENS | NUM_HUNDRED_AND | NUM_SINGLE_AND | NUM_ROMAN | NUM_1900;
			tr->langopts.accents = 1;
		}
		break;

	case L('a','m'):  // Amharic, Ethiopia
		{
			SetupTranslator(tr,stress_lengths_fr,stress_amps_fr);
			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.stress_flags = 0x0024;  // don't use secondary stress
			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable
			tr->langopts.param[LOPT_UNPRONOUNCABLE] = 1;   // disable check for unpronouncable words
		}
		break;


	case L('a','r'):   // Arabic
			tr->letter_bits_offset = OFFSET_ARABIC;
			tr->langopts.numbers = NUM_SWAP_TENS | NUM_AND_UNITS | NUM_HUNDRED_AND | NUM_OMIT_1_HUNDRED | NUM_AND_HUNDRED | NUM_THOUSAND_AND | NUM_OMIT_1_THOUSAND;
			tr->langopts.param[LOPT_UNPRONOUNCABLE] = 1;   // disable check for unpronouncable words
		break;

	case L('b','g'):  //Bulgarian
		{
			SetCyrillicLetters(tr);
			SetLetterVowel(tr,0x2a);
			tr->langopts.param[LOPT_UNPRONOUNCABLE] = 0x432;    // [v]  don't count this character at start of word
			tr->langopts.param[LOPT_REGRESSIVE_VOICING] = 0x17;  // devoice at end of word, and change voicing to match a following consonant (except v)
			tr->langopts.param[LOPT_REDUCE] = 2;
			tr->langopts.stress_rule = STRESSPOSN_2R;
			tr->langopts.numbers = NUM_DECIMAL_COMMA | NUM_ALLOW_SPACE | NUM_OMIT_1_HUNDRED | NUM_HUNDRED_AND | NUM_AND_UNITS | NUM_SINGLE_AND | NUM_ROMAN | NUM_ROMAN_ORDINAL | NUM_ROMAN_CAPITALS ;
			tr->langopts.thousands_sep = ' ';   // don't allow dot as thousands separator
		}
		break;

	case L('b','n'):  // Bengali
		{
			static const short stress_lengths_bn[8] = {180, 180,  210, 210,  0, 0,  230, 240};
			static const unsigned char stress_amps_bn[8] = {18,18, 18,18, 20,20, 22,22 };

			SetupTranslator(tr,stress_lengths_bn,stress_amps_bn);
			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable

			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.stress_flags =  0x10004;   // use 'diminished' for unstressed final syllable
			tr->letter_bits_offset = OFFSET_BENGALI;
			SetIndicLetters(tr);   // call this after setting OFFSET_BENGALI
			SetLetterBitsRange(tr,LETTERGP_F,0x3e,0x4c);   // vowel signs, but not virama

			tr->langopts.numbers = 0x1;
			tr->langopts.break_numbers = 0x24924aa8;  // for languages which have numbers for 100,000 and 100,00,000, eg Hindi
		}
		break;

	case L('c','y'):   // Welsh
		{
			static const short stress_lengths_cy[8] = {170,220, 180,180, 0, 0, 250,270};
			static const unsigned char stress_amps_cy[8] = {17,15, 18,18, 0,0, 22,20 };    // 'diminished' is used to mark a quieter, final unstressed syllable

			SetupTranslator(tr,stress_lengths_cy,stress_amps_cy);

			tr->charset_a0 = charsets[14];   // ISO-8859-14
//			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable
			tr->langopts.stress_rule = STRESSPOSN_2R;
//			tr->langopts.intonation_group = 4;

			// 'diminished' is an unstressed final syllable
			tr->langopts.stress_flags =  0x6 | 0x10; 
			tr->langopts.unstressed_wd1 = 0;
			tr->langopts.unstressed_wd2 = 2;
			tr->langopts.param[LOPT_SONORANT_MIN] = 120;  // limit the shortening of sonorants before short vowels

			tr->langopts.numbers = NUM_OMIT_1_HUNDRED;

			SetLetterVowel(tr,'w');  // add letter to vowels and remove from consonants
			SetLetterVowel(tr,'y');
		}
		break;

	case L('d','a'):  // Danish
		{
			static const short stress_lengths_da[8] = {160,140, 200,200, 0,0, 220,230};
			SetupTranslator(tr,stress_lengths_da,NULL);

			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.param[LOPT_PREFIXES] = 1;
			SetLetterVowel(tr,'y');
			tr->langopts.numbers = NUM_DECIMAL_COMMA | NUM_SWAP_TENS | NUM_HUNDRED_AND | NUM_OMIT_1_HUNDRED | NUM_ORDINAL_DOT | NUM_1900 | NUM_ROMAN | NUM_ROMAN_CAPITALS | NUM_ROMAN_ORDINAL;
		}
		break;


	case L('d','e'):
		{
			static const short stress_lengths_de[8] = {150,130, 200,200,  0, 0, 270,270};
			static const unsigned char stress_amps_de[] = {20,20, 20,20, 20,22, 22,20 };
			SetupTranslator(tr, stress_lengths_de, stress_amps_de);
			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.word_gap = 0x8;   // don't use linking phonemes
			tr->langopts.vowel_pause = 0x30;
			tr->langopts.param[LOPT_PREFIXES] = 1;
			tr->langopts.param[LOPT_REGRESSIVE_VOICING] = 0x10;  // devoice at end of word
			tr->langopts.param[LOPT_LONG_VOWEL_THRESHOLD] = 175/2;
		
			tr->langopts.numbers = NUM_DECIMAL_COMMA | NUM_SWAP_TENS | NUM_ALLOW_SPACE | NUM_ORDINAL_DOT | NUM_ROMAN;
//			tr->langopts.numbers = NUM_DECIMAL_COMMA | NUM_SWAP_TENS | NUM_OMIT_1_HUNDRED | NUM_OMIT_1_THOUSAND | NUM_ALLOW_SPACE | NUM_ORDINAL_DOT | NUM_ROMAN;
			SetLetterVowel(tr,'y');
			tr->langopts.param[LOPT_UNPRONOUNCABLE] = 2;   // use de_rules for unpronouncable rules
		}
		break;

	case L('d','v'):   // Divehi (Maldives)
		{
			SetupTranslator(tr,stress_lengths_ta,stress_amps_ta);
			tr->langopts.param[LOPT_UNPRONOUNCABLE] = 1;   // disable check for unpronouncable words
			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable
			tr->letter_bits_offset = OFFSET_THAANA;
			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.stress_flags =  0x10004;   // use 'diminished' for unstressed final syllable
			SetLetterBitsRange(tr,LETTERGP_B,0x26,0x30);   // vowel signs, and virama
			tr->langopts.break_numbers = 0x14a8;  // 1000, 100,000  10,000,000 
			tr->langopts.numbers = 1;
		}
		break;

	case L('e','n'):
		{
			static const short stress_lengths_en[8] = {182,140, 220,220, 0,0, 248,275};
			SetupTranslator(tr,stress_lengths_en,NULL);

			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.stress_flags = 0x08;
			tr->langopts.numbers = NUM_HUNDRED_AND | NUM_ROMAN | NUM_1900;
			tr->langopts.param[LOPT_COMBINE_WORDS] = 2;       // allow "mc" to cmbine with the following word
			tr->langopts.suffix_add_e = 'e';
			tr->langopts.param[LOPT_UNPRONOUNCABLE] = 2;   // use en_rules for unpronouncable rules
			SetLetterBits(tr,6,"aeiouy");  // Group Y: vowels, including y
		}
		break;

	case L('e','l'):   // Greek
	case L_grc:        // Ancient Greek
		{
			static const short stress_lengths_el[8] = {155, 180,  210, 210,  0, 0,  270, 300};
			static const unsigned char stress_amps_el[8] = {15,12, 20,20, 20,22, 22,21 };    // 'diminished' is used to mark a quieter, final unstressed syllable

			// character codes offset by 0x380
			static const char el_vowels[] = {0x10,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x35,0x37,0x39,0x3f,0x45,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0};
			static const char el_fvowels[] = {0x2d,0x2e,0x2f,0x35,0x37,0x39,0x45,0x4d,0}; // ε η ι υ  έ ή ί ύ
			static const char el_voiceless[]= {0x38,0x3a,0x3e,0x40,0x42,0x43,0x44,0x46,0x47,0};  // θ κ ξ π ς σ τ φ χ 
			static const char el_consonants[]={0x32,0x33,0x34,0x36,0x38,0x3a,0x3b,0x3c,0x3d,0x3e,0x40,0x41,0x42,0x43,0x44,0x46,0x47,0x48,0};
			static const wchar_t el_char_apostrophe[] = {0x3c3,0};  // σ

			SetupTranslator(tr,stress_lengths_el,stress_amps_el);

			tr->charset_a0 = charsets[7];   // ISO-8859-7
			tr->char_plus_apostrophe = el_char_apostrophe;

			tr->letter_bits_offset = OFFSET_GREEK;
			memset(tr->letter_bits,0,sizeof(tr->letter_bits));
			SetLetterBits(tr,LETTERGP_A,el_vowels);
			SetLetterBits(tr,LETTERGP_VOWEL2,el_vowels);
			SetLetterBits(tr,LETTERGP_B,el_voiceless);
			SetLetterBits(tr,LETTERGP_C,el_consonants);
			SetLetterBits(tr,LETTERGP_Y,el_fvowels);    // front vowels: ε η ι υ

			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable
			tr->langopts.stress_rule = STRESSPOSN_2R;
			tr->langopts.stress_flags = 0x6;  // mark unstressed final syllables as diminished
			tr->langopts.unstressed_wd1 = 0;
			tr->langopts.unstressed_wd2 = 2;
			tr->langopts.param[LOPT_SONORANT_MIN] = 130;  // limit the shortening of sonorants before short vowels

			tr->langopts.numbers = NUM_SINGLE_STRESS | NUM_DECIMAL_COMMA; 
			tr->langopts.numbers2 = 0x2 | NUM2_MULTIPLE_ORDINAL;   // variant form of numbers before thousands

			if(name2 == L_grc)
			{
				// ancient greek
				tr->langopts.param[LOPT_UNPRONOUNCABLE] = 1;
			}
		}
		break;

	case L('e','o'):
		{
			static const short stress_lengths_eo[8] = {150, 150,  230, 180,    0,   0,  300, 320};
			static const unsigned char stress_amps_eo[] = {16,14, 20,20, 20,22, 22,21 };
			static const wchar_t eo_char_apostrophe[2] = {'l',0};
		
			SetupTranslator(tr,stress_lengths_eo,stress_amps_eo);

			tr->charset_a0 = charsets[3];  // ISO-8859-3
			tr->char_plus_apostrophe = eo_char_apostrophe;

//			tr->langopts.word_gap = 1;
			tr->langopts.vowel_pause = 2;
			tr->langopts.stress_rule = STRESSPOSN_2R;
			tr->langopts.stress_flags =  0x6 | 0x10; 
//			tr->langopts.unstressed_wd1 = 3;
			tr->langopts.unstressed_wd2 = 2;

			tr->langopts.numbers = NUM_DECIMAL_COMMA | NUM_OMIT_1_HUNDRED | NUM_ALLOW_SPACE | NUM_ROMAN;
		}
		break;

	case L('e','s'):   // Spanish
	case L('c','a'):   // Catalan
	case L_pap:        // Papiamento
		{
			static const short stress_lengths_es[8] = {180, 210,  190, 190,  0, 0,  230, 260};
//			static const short stress_lengths_es[8] = {170, 200,  180, 180,  0, 0,  220, 250};
			static const unsigned char stress_amps_es[8] = {16,12, 18,18, 20,20, 20,20 };    // 'diminished' is used to mark a quieter, final unstressed syllable
			static const wchar_t ca_punct_within_word[] = {'\'',0xb7,0};   // ca: allow middle-dot within word

			SetupTranslator(tr,stress_lengths_es,stress_amps_es);

			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable
			tr->langopts.stress_rule = STRESSPOSN_2R;

			// stress last syllable if it doesn't end in vowel or "s" or "n"
			// 'diminished' is an unstressed final syllable
			tr->langopts.stress_flags = 0x200 | 0x6 | 0x10; 
			tr->langopts.unstressed_wd1 = 0;
			tr->langopts.unstressed_wd2 = 2;
			tr->langopts.param[LOPT_SONORANT_MIN] = 120;  // limit the shortening of sonorants before short vowels

			tr->langopts.numbers = NUM_SINGLE_STRESS | NUM_DECIMAL_COMMA | NUM_AND_UNITS | NUM_OMIT_1_HUNDRED | NUM_OMIT_1_THOUSAND | NUM_ROMAN | NUM_ROMAN_AFTER;
			tr->langopts.numbers2 = NUM2_MULTIPLE_ORDINAL; 

			if(name2 == L('c','a'))
			{
				tr->punct_within_word = ca_punct_within_word;
				tr->langopts.stress_flags = 0x200 | 0x6 | 0x30;  // stress last syllable unless word ends with a vowel
			}
			else
			if(name2 == L_pap)
			{
				tr->langopts.stress_flags = 0x100 | 0x6 | 0x30;  // stress last syllable unless word ends with a vowel
			}
			else
			{
				tr->langopts.param[LOPT_UNPRONOUNCABLE] = 2;   // use es_rules for unpronouncable rules
			}
		}
		break;

	case L('e','u'):  // basque
		{
			static const short stress_lengths_eu[8] = {200, 200,  200, 200,  0, 0,  210, 230};  // very weak stress
			static const unsigned char stress_amps_eu[8] = {16,16, 18,18, 18,18, 18,18 };
			SetupTranslator(tr,stress_lengths_eu,stress_amps_eu);
			tr->langopts.stress_rule = STRESSPOSN_2L;  // ?? second syllable ??
			tr->langopts.numbers = NUM_SINGLE_STRESS | NUM_DECIMAL_COMMA | NUM_AND_UNITS | NUM_HUNDRED_AND | NUM_OMIT_1_HUNDRED | NUM_VIGESIMAL;
		}
		break;


	case L('f','a'):   // Farsi
		{
			tr->letter_bits_offset = OFFSET_ARABIC;
			tr->langopts.numbers = NUM_AND_UNITS | NUM_HUNDRED_AND;
			tr->langopts.param[LOPT_UNPRONOUNCABLE] = 1;   // disable check for unpronouncable words
		}
		break;


	case L('e','t'):   // Estonian
			tr->charset_a0 = charsets[4];   // ISO-8859-4
		// drop through to Finnish
	case L('f','i'):   // Finnish
		{
			static const unsigned char stress_amps_fi[8] = {18,16, 22,22, 20,22, 22,22 };
			static const short stress_lengths_fi[8] = {150,180, 200,200, 0,0, 210,250};

			SetupTranslator(tr,stress_lengths_fi,stress_amps_fi);

			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.stress_flags = 0x56;  // move secondary stress from light to a following heavy syllable
			tr->langopts.param[LOPT_IT_DOUBLING] = 1;
			tr->langopts.long_stop = 130;

			tr->langopts.numbers = NUM_DECIMAL_COMMA + NUM_ALLOW_SPACE;
			SetLetterVowel(tr,'y');
//			tr->langopts.max_initial_consonants = 2;  // BUT foreign words may have 3
			tr->langopts.spelling_stress = 1;
			tr->langopts.intonation_group = 3;  // less intonation, don't raise pitch at comma
		}
		break;

	case L('f','r'):  // french
		{
			SetupTranslator(tr,stress_lengths_fr,stress_amps_fr);
			tr->langopts.stress_rule = STRESSPOSN_1R;      // stress on final syllable
			tr->langopts.stress_flags = 0x0024;  // don't use secondary stress
			tr->langopts.param[LOPT_IT_LENGTHEN] = 1;    // remove lengthen indicator from unstressed syllables
			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable
			tr->langopts.accents = 2;   // Say "Capital" after the letter.

			tr->langopts.numbers = NUM_SINGLE_STRESS | NUM_DECIMAL_COMMA | NUM_ALLOW_SPACE | NUM_OMIT_1_HUNDRED | NUM_NOPAUSE | NUM_ROMAN | NUM_ROMAN_CAPITALS | NUM_ROMAN_AFTER | NUM_VIGESIMAL | NUM_DFRACTION_4;
			SetLetterVowel(tr,'y');
		}
		break;

	case L('h','i'):    // Hindi
	case L('n','e'):    // Nepali
	case L('p','a'):    // Punjabi
		{
			static const short stress_lengths_hi[8] = {190, 190,  210, 210,  0, 0,  230, 250};
			static const unsigned char stress_amps_hi[8] = {17,14, 20,19, 20,22, 22,21 };

			SetupTranslator(tr,stress_lengths_hi,stress_amps_hi);
			tr->charset_a0 = charsets[19];   // ISCII
			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable

			tr->langopts.stress_rule = 6;      // stress on last heaviest syllable, excluding final syllable
			tr->langopts.stress_flags =  0x10004;   // use 'diminished' for unstressed final syllable
			tr->langopts.numbers = NUM_SWAP_TENS;
			tr->langopts.break_numbers = 0x14aa8;  // for languages which have numbers for 100,000 and 100,00,000, eg Hindi
			tr->letter_bits_offset = OFFSET_DEVANAGARI;

			if(name2 == L('p','a'))
			{
				tr->letter_bits_offset = OFFSET_GURMUKHI;
			}
			SetIndicLetters(tr);
		}
		break;


	case L('h','r'):   // Croatian
	case L('b','s'):   // Bosnian
	case L('s','r'):   // Serbian
		{
			static const unsigned char stress_amps_hr[8] = {17,17, 20,20, 20,22, 22,21 };
			static const short stress_lengths_hr[8] = {180,160, 200,200, 0,0, 220,230};
			static const short stress_lengths_sr[8] = {160,150, 200,200, 0,0, 250,260};

			if(name2 == L('s','r'))
				SetupTranslator(tr,stress_lengths_sr,stress_amps_hr);
			else
				SetupTranslator(tr,stress_lengths_hr,stress_amps_hr);
			tr->charset_a0 = charsets[2];   // ISO-8859-2

			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.stress_flags = 0x10;  
			tr->langopts.param[LOPT_REGRESSIVE_VOICING] = 0x3;
 			tr->langopts.max_initial_consonants = 5;
			tr->langopts.spelling_stress = 1;
			tr->langopts.accents = 1;

			tr->langopts.numbers = NUM_SINGLE_STRESS | NUM_HUNDRED_AND | NUM_OMIT_1_HUNDRED | NUM_DECIMAL_COMMA | NUM_THOUS_SPACE | NUM_DFRACTION_2 | NUM_ROMAN_CAPITALS;
			tr->langopts.numbers2 = 0x4a;  // variant numbers before thousands,milliards
			tr->langopts.replace_chars = replace_cyrillic_latin;

			SetLetterVowel(tr,'y');
			SetLetterVowel(tr,'r');
		}
		break;


	case L('h','t'):  // Haitian Creole
//			memcpy(tr->stress_lengths,stress_lengths_fr,sizeof(tr->stress_lengths));
			tr->langopts.stress_rule = STRESSPOSN_1R;      // stress on final syllable
			tr->langopts.stress_flags = 0x0024;  // don't use secondary stress
			tr->langopts.numbers = NUM_SINGLE_STRESS | NUM_OMIT_1_HUNDRED | NUM_NOPAUSE | NUM_ROMAN | NUM_VIGESIMAL | NUM_DFRACTION_4;
		break;


	case L('h','u'):   // Hungarian
		{
			static const unsigned char stress_amps_hu[8] = {17,17, 19,19, 20,22, 22,21 };
			static const short stress_lengths_hu[8] = {185,195, 195,190, 0,0, 210,220};

			SetupTranslator(tr,stress_lengths_hu,stress_amps_hu);
			tr->charset_a0 = charsets[2];   // ISO-8859-2

			tr->langopts.vowel_pause = 0x20;
			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.stress_flags = 0x8036 | S_HYPEN_UNSTRESS;
			tr->langopts.unstressed_wd1 = 2;
			tr->langopts.param[LOPT_IT_DOUBLING] = 1;
			tr->langopts.param[LOPT_ANNOUNCE_PUNCT] = 2;  // don't break clause before announcing . ? !

			tr->langopts.numbers = NUM_DFRACTION_5 | NUM_ALLOW_SPACE | NUM_ROMAN | NUM_ROMAN_ORDINAL | NUM_ROMAN_CAPITALS | NUM_ORDINAL_DOT | NUM_OMIT_1_HUNDRED | NUM_OMIT_1_THOUSAND;
			tr->langopts.thousands_sep = ' ';   // don't allow dot as thousands separator
			tr->langopts.decimal_sep = ',';
			tr->langopts.max_roman = 899;
			tr->langopts.min_roman = 1;
			SetLetterVowel(tr,'y');
			tr->langopts.spelling_stress = 1;
SetLengthMods(tr,3);  // all equal
		}
		break;

	case L('h','y'):   // Armenian
		{
			static const short stress_lengths_hy[8] = {250, 200,  250, 250,  0, 0,  250, 250};
			static const char hy_vowels[] = {0x31, 0x35, 0x37, 0x38, 0x3b, 0x48, 0x55, 0};
			static const char hy_consonants[] = {0x32,0x33,0x34,0x36,0x39,0x3a,0x3c,0x3d,0x3e,0x3f,
				0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x56,0};

			SetupTranslator(tr,stress_lengths_hy,NULL);
			tr->langopts.stress_rule = STRESSPOSN_1R;  // default stress on final syllable

			tr->letter_bits_offset = OFFSET_ARMENIAN;
			memset(tr->letter_bits,0,sizeof(tr->letter_bits));
			SetLetterBits(tr,LETTERGP_A,hy_vowels);
			SetLetterBits(tr,LETTERGP_VOWEL2,hy_vowels);
			SetLetterBits(tr,LETTERGP_C,hy_consonants);
			tr->langopts.max_initial_consonants = 6;
			tr->langopts.numbers = NUM_DECIMAL_COMMA | NUM_OMIT_1_HUNDRED;
//	tr->langopts.param[LOPT_UNPRONOUNCABLE] = 1;   // disable check for unpronouncable words
		}
		break;

	case L('i','d'):   // Indonesian
		{
			static const short stress_lengths_id[8] = {160, 200,  180, 180,  0, 0,  220, 240};
			static const unsigned char stress_amps_id[8] = {16,18, 18,18, 20,22, 22,21 };

			SetupTranslator(tr,stress_lengths_id,stress_amps_id);
			tr->langopts.stress_rule = STRESSPOSN_2R;
			tr->langopts.numbers = NUM_DECIMAL_COMMA | NUM_ALLOW_SPACE | NUM_ROMAN;
			tr->langopts.stress_flags =  0x6 | 0x10; 
			tr->langopts.accents = 2;  // "capital" after letter name
		}
		break;

	case L('i','s'):   // Icelandic
		{
			static const short stress_lengths_is[8] = {180,160, 200,200, 0,0, 240,250};
			static const wchar_t is_lettergroup_B[] = {'c','f','h','k','p','t','x',0xfe,0};  // voiceless conants, including 'þ'  ?? 's'

			SetupTranslator(tr,stress_lengths_is,NULL);
			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.stress_flags = 0x10;
			tr->langopts.param[LOPT_IT_LENGTHEN] = 0x11;    // remove lengthen indicator from unstressed vowels
			tr->langopts.param[LOPT_REDUCE] = 2;

			ResetLetterBits(tr,0x18);
			SetLetterBits(tr,4,"kpst");   // Letter group F
			SetLetterBits(tr,3,"jvr");    // Letter group H
			tr->letter_groups[1] = is_lettergroup_B;
			SetLetterVowel(tr,'y');
			tr->langopts.numbers = NUM_DECIMAL_COMMA | NUM_SINGLE_AND | NUM_HUNDRED_AND | NUM_AND_UNITS | NUM_1900;
			tr->langopts.numbers2 = 0x2;
		}
		break;

	case L('i','t'):   // Italian
		{
			static const short stress_lengths_it[8] = {150, 140,  170, 170,  0, 0,  300, 330};
			static const unsigned char stress_amps_it[8] = {15,14, 19,19, 20,22, 22,20 };

			SetupTranslator(tr,stress_lengths_it,stress_amps_it);

			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable
			tr->langopts.stress_rule = STRESSPOSN_2R;
			tr->langopts.stress_flags = 0x10 | 0x20000; 
			tr->langopts.vowel_pause = 1;
			tr->langopts.unstressed_wd1 = 2;
			tr->langopts.unstressed_wd2 = 2;
			tr->langopts.param[LOPT_IT_LENGTHEN] = 2;    // remove lengthen indicator from unstressed or non-penultimate syllables
			tr->langopts.param[LOPT_IT_DOUBLING] = 2;    // double the first consonant if the previous word ends in a stressed vowel
			tr->langopts.param[LOPT_SONORANT_MIN] = 130;  // limit the shortening of sonorants before short vowels
			tr->langopts.param[LOPT_REDUCE] = 1;        // reduce vowels even if phonemes are specified in it_list
			tr->langopts.param[LOPT_ALT] = 2;      // call ApplySpecialAttributes2() if a word has $alt or $alt2
			tr->langopts.numbers = NUM_SINGLE_VOWEL | NUM_OMIT_1_HUNDRED |NUM_DECIMAL_COMMA | NUM_ROMAN | NUM_DFRACTION_1;
			tr->langopts.accents = 2;   // Say "Capital" after the letter.
			SetLetterVowel(tr,'y');
		}
		break;

	case L_jbo:   // Lojban
		{
			static const short stress_lengths_jbo[8] = {145,145, 170,160, 0,0, 330,350};
			static const wchar_t jbo_punct_within_word[] = {'.',',','\'',0x2c8,0};  // allow period and comma within a word, also stress marker (from LOPT_CAPS_IN_WORD)

			SetupTranslator(tr,stress_lengths_jbo,NULL);
			tr->langopts.stress_rule = STRESSPOSN_2R;
			tr->langopts.vowel_pause = 0x20c;  // pause before a word which starts with a vowel, or after a word which ends in a consonant
//			tr->langopts.word_gap = 1;
			tr->punct_within_word = jbo_punct_within_word;
			tr->langopts.param[LOPT_CAPS_IN_WORD] = 2;  // capitals indicate stressed syllables
			SetLetterVowel(tr,'y');
			tr->langopts.max_lengthmod = 368;
		}
		break;

	case L('k','a'):   // Georgian
		{
			// character codes offset by 0x1080
			static const char ka_vowels[] = {0x50,0x54,0x58,0x5d,0x63,0x75,0x77,0};
			static const char ka_consonants[] = {0x51,0x52,0x53,0x55,0x56,0x57,0x59,0x5a,0x5b,0x5c,0x5e,0x5f,0x60,0x61,0x62,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x76,0};
			SetupTranslator(tr,stress_lengths_ta,stress_amps_ta);
			memset(tr->letter_bits,0,sizeof(tr->letter_bits));
			SetLetterBits(tr,LETTERGP_A,ka_vowels);
			SetLetterBits(tr,LETTERGP_C,ka_consonants);
			SetLetterBits(tr,LETTERGP_VOWEL2,ka_vowels);

			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.stress_flags = S_FINAL_NO_2;
			tr->letter_bits_offset = OFFSET_GEORGIAN;
//			tr->langopts.param[LOPT_UNPRONOUNCABLE] = 1;   // disable check for unpronouncable words
			tr->langopts.max_initial_consonants = 7;
			tr->langopts.numbers = NUM_VIGESIMAL | NUM_AND_UNITS | NUM_OMIT_1_HUNDRED |NUM_OMIT_1_THOUSAND | NUM_DFRACTION_5;
		}
		break;

	case L('k','o'):   // Korean, TEST
		{
			static const char ko_ivowels[] = {0x63,0x64,0x67,0x68,0x6d,0x72,0x74,0x75,0};  // y and i vowels
			static const unsigned char ko_voiced[] = {0x02,0x05,0x06,0xab,0xaf,0xb7,0xbc,0};  // voiced consonants, l,m,n,N

			tr->letter_bits_offset = OFFSET_KOREAN;
			memset(tr->letter_bits,0,sizeof(tr->letter_bits));
			SetLetterBitsRange(tr,LETTERGP_A,0x61,0x75);
			SetLetterBits(tr,LETTERGP_Y,ko_ivowels);
			SetLetterBits(tr,LETTERGP_G,(const char *)ko_voiced);

			tr->langopts.stress_rule = 8;   // ?? 1st syllable if it is heavy, else 2nd syllable
			tr->langopts.param[LOPT_UNPRONOUNCABLE] = 1;   // disable check for unpronouncable words
			tr->langopts.numbers = NUM_OMIT_1_HUNDRED;
		}
		break;

	case L('k','u'):   // Kurdish
		{
			static const unsigned char stress_amps_ku[8] = {18,18, 20,20, 20,22, 22,21 };
			static const short stress_lengths_ku[8] = {180,180, 190,180, 0,0, 230,240};

			SetupTranslator(tr,stress_lengths_ku,stress_amps_ku);
			tr->charset_a0 = charsets[9];   // ISO-8859-9 - Latin5

			tr->langopts.stress_rule = 7;   // stress on the last syllable, before any explicitly unstressed syllable

			tr->langopts.numbers = NUM_HUNDRED_AND | NUM_AND_UNITS | NUM_OMIT_1_HUNDRED | NUM_AND_HUNDRED;
			tr->langopts.max_initial_consonants = 2;
		}
		break;

	case L('l','a'):  //Latin
		{
			tr->charset_a0 = charsets[4];   // ISO-8859-4, includes a,e,i,o,u-macron
			tr->langopts.stress_rule = STRESSPOSN_2R;
			tr->langopts.stress_flags = 0x20;
			tr->langopts.unstressed_wd1 = 0;
			tr->langopts.unstressed_wd2 = 2;
			tr->langopts.param[LOPT_DIERESES] = 1;
			tr->langopts.numbers = NUM_ROMAN;
			tr->langopts.max_roman = 5000;
		}
		break;

	case L('l','v'):  // latvian
		{
			static const unsigned char stress_amps_lv[8] = {17,13, 20,20, 20,22, 22,21 };
			static const short stress_lengths_lv[8] = {180,130, 210,210, 0,0, 210,210};

			SetupTranslator(tr,stress_lengths_lv,stress_amps_lv);

			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.spelling_stress = 1;
			tr->charset_a0 = charsets[4];   // ISO-8859-4
			tr->langopts.numbers = NUM_DECIMAL_COMMA | NUM_OMIT_1_HUNDRED | NUM_DFRACTION_4 | NUM_ORDINAL_DOT;
			tr->langopts.stress_flags = 0x16 + 0x40000;
		}
		break;

	case L('m','k'):   // Macedonian
		{
			static wchar_t vowels_cyrillic[] = {0x440,  // also include 'р' [R]
				 0x430,0x435,0x438,0x439,0x43e,0x443,0x44b,0x44d,0x44e,0x44f,0x450,0x451,0x456,0x457,0x45d,0x45e,0};
			static const unsigned char stress_amps_mk[8] = {17,17, 20,20, 20,22, 22,21 };
			static const short stress_lengths_mk[8] = {180,160, 200,200, 0,0, 220,230};

			SetupTranslator(tr,stress_lengths_mk,stress_amps_mk);
			tr->charset_a0 = charsets[5];   // ISO-8859-5
			tr->letter_groups[0] = tr->letter_groups[7] = vowels_cyrillic;

			tr->langopts.stress_rule = STRESSPOSN_3R;   // antipenultimate
			tr->langopts.numbers = NUM_DECIMAL_COMMA | NUM_AND_UNITS | NUM_OMIT_1_HUNDRED | NUM_OMIT_1_THOUSAND | NUM_DFRACTION_2;
			tr->langopts.numbers2 = 0x8a;  // variant numbers before thousands,milliards
		}
		break;


	case L('n','l'):  // Dutch
		{
			static const short stress_lengths_nl[8] = {160,135, 210,210,  0, 0, 260,280};

			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.vowel_pause = 0x30;  // ??
			tr->langopts.param[LOPT_DIERESES] = 1;
			tr->langopts.param[LOPT_PREFIXES] = 1;
			tr->langopts.param[LOPT_REGRESSIVE_VOICING] = 0x10;  // devoice at end of word
			SetLetterVowel(tr,'y');

			tr->langopts.numbers = NUM_DECIMAL_COMMA | NUM_SWAP_TENS | NUM_OMIT_1_HUNDRED | NUM_OMIT_1_THOUSAND | NUM_ALLOW_SPACE | NUM_1900 | NUM_ORDINAL_DOT;
			tr->langopts.ordinal_indicator = "e";
			tr->langopts.stress_flags = S_FIRST_PRIMARY;
			memcpy(tr->stress_lengths,stress_lengths_nl,sizeof(tr->stress_lengths));
		}
		break;

	case L('n','o'):  // Norwegian
		{
			static const short stress_lengths_no[8] = {160,140, 200,200, 0,0, 220,230};

			SetupTranslator(tr,stress_lengths_no,NULL);
			tr->langopts.stress_rule = STRESSPOSN_1L;
			SetLetterVowel(tr,'y');
			tr->langopts.numbers = NUM_DECIMAL_COMMA | NUM_HUNDRED_AND | NUM_ALLOW_SPACE | NUM_1900 | NUM_ORDINAL_DOT;
		}
		break;

	case L('o','m'):
		{
			static const unsigned char stress_amps_om[] = {18,15, 20,20, 20,22, 22,22 };
			static const short stress_lengths_om[8] = {200,200, 200,200, 0,0, 200,200};

			SetupTranslator(tr,stress_lengths_om,stress_amps_om);
			tr->langopts.stress_rule = STRESSPOSN_2R;
			tr->langopts.stress_flags = 0x16 + 0x80000;
		}
		break;

	case L('p','l'):   // Polish
		{
			static const short stress_lengths_pl[8] = {160, 190,  175, 175,  0, 0,  200, 210};
			static const unsigned char stress_amps_pl[8] = {17,13, 19,19, 20,22, 22,21 };    // 'diminished' is used to mark a quieter, final unstressed syllable

			SetupTranslator(tr,stress_lengths_pl,stress_amps_pl);

			tr->charset_a0 = charsets[2];   // ISO-8859-2
			tr->langopts.stress_rule = STRESSPOSN_2R;
			tr->langopts.stress_flags = 0x6;  // mark unstressed final syllables as diminished
			tr->langopts.param[LOPT_REGRESSIVE_VOICING] = 0x8;
 			tr->langopts.max_initial_consonants = 7; // for example: wchrzczony :)
 			tr->langopts.numbers = NUM_DECIMAL_COMMA | NUM_ALLOW_SPACE | NUM_DFRACTION_2;
			tr->langopts.numbers2 = 0x40;
			tr->langopts.param[LOPT_COMBINE_WORDS] = 4 + 0x100;  // combine 'nie' (marked with $alt2) with some 1-syllable (and 2-syllable) words (marked with $alt)
			SetLetterVowel(tr,'y');
		}
		break;

	case L('p','t'):  // Portuguese
		{
			static const short stress_lengths_pt[8] = {170, 115,  210, 240,  0, 0,  260, 280};
			static const unsigned char stress_amps_pt[8] = {16,11, 19,21, 20,22, 22,21 };    // 'diminished' is used to mark a quieter, final unstressed syllable

			SetupTranslator(tr,stress_lengths_pt,stress_amps_pt);
			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable

			tr->langopts.stress_rule = STRESSPOSN_1R;        // stress on final syllable
			tr->langopts.stress_flags =  0x6 | 0x10 | 0x2000 | 0x20000; 
			tr->langopts.numbers = NUM_DECIMAL_COMMA | NUM_DFRACTION_2 | NUM_HUNDRED_AND | NUM_AND_UNITS | NUM_ROMAN;
			SetLetterVowel(tr,'y');
			ResetLetterBits(tr,0x2);
			SetLetterBits(tr,1,"bcdfgjkmnpqstvxz");      // B  hard consonants, excluding h,l,r,w,y
			tr->langopts.param[LOPT_ALT] = 2;      // call ApplySpecialAttributes2() if a word has $alt or $alt2
		}
		break;

	case L('r','o'):  // Romanian
		{
			static const short stress_lengths_ro[8] = {170, 170,  180, 180,  0, 0,  240, 260};
			static const unsigned char stress_amps_ro[8] = {15,13, 18,18, 20,22, 22,21 };

			SetupTranslator(tr,stress_lengths_ro,stress_amps_ro);

			tr->langopts.stress_rule = STRESSPOSN_2R;
			tr->langopts.stress_flags = 0x100 + 0x6;

			tr->charset_a0 = charsets[2];   // ISO-8859-2
			tr->langopts.numbers = NUM_DECIMAL_COMMA | NUM_ALLOW_SPACE | NUM_DFRACTION_3 | NUM_AND_UNITS | NUM_ROMAN;
			tr->langopts.numbers2 = 0x1e;  // variant numbers before all thousandplex
		}
		break;

	case L('r','u'):  // Russian
			Translator_Russian(tr);
		break;

	case L('r','w'):   // Kiryarwanda
		{
			tr->langopts.stress_rule = STRESSPOSN_2R;
			tr->langopts.stress_flags = 0x16;
			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable

			tr->langopts.numbers = NUM_HUNDRED_AND | NUM_AND_UNITS | NUM_DFRACTION_2 | NUM_AND_HUNDRED;
			tr->langopts.numbers2 = 0x200;  // say "thousands" before its number
		}
		break;

	case L('s','k'):   // Slovak
	case L('c','s'):   // Czech
		{
			static const char *sk_voiced = "bdgjlmnrvwzaeiouy";

			SetupTranslator(tr,stress_lengths_sk,stress_amps_sk);
			tr->charset_a0 = charsets[2];   // ISO-8859-2

			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.stress_flags = 0x16;  
			tr->langopts.param[LOPT_REGRESSIVE_VOICING] = 0x3;
 			tr->langopts.max_initial_consonants = 5;
			tr->langopts.spelling_stress = 1;
			tr->langopts.param[LOPT_COMBINE_WORDS] = 4;  // combine some prepositions with the following word

			tr->langopts.numbers = NUM_OMIT_1_HUNDRED | NUM_DFRACTION_2 | NUM_ROMAN;
			tr->langopts.numbers2 = 0x100;
			tr->langopts.thousands_sep = STRESSPOSN_1L;   //no thousands separator
			tr->langopts.decimal_sep = ',';

			if(name2 == L('c','s'))
			{
				tr->langopts.numbers2 = 0x108;  // variant numbers before milliards
			}

			SetLetterVowel(tr,'y');
			SetLetterVowel(tr,'r');
			ResetLetterBits(tr,0x20);
			SetLetterBits(tr,5,sk_voiced);
		}
		break;

	case L('s','i'):  // Sinhala
		{
			SetupTranslator(tr,stress_lengths_ta,stress_amps_ta);
			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable

			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.stress_flags =  S_FINAL_DIM | S_NO_AUTO_DIM | S_FINAL_NO_2;
			tr->langopts.spelling_stress = 1;

			tr->letter_bits_offset = OFFSET_SINHALA;
			memset(tr->letter_bits,0,sizeof(tr->letter_bits));
			SetLetterBitsRange(tr,LETTERGP_A,0x05,0x16);   // vowel letters
			SetLetterBitsRange(tr,LETTERGP_A,0x4a,0x73);   // + vowel signs, and virama
		
			SetLetterBitsRange(tr,LETTERGP_B,0x4a,0x73);   // vowel signs, and virama
		
			SetLetterBitsRange(tr,LETTERGP_C,0x1a,0x46);   // the main consonant range
		
			tr->langopts.param[LOPT_UNPRONOUNCABLE] = 1;   // disable check for unpronouncable words
			tr->langopts.suffix_add_e = tr->letter_bits_offset + 0x4a;   //virama
			tr->langopts.numbers =  NUM_OMIT_1_THOUSAND ;
		}
		break;

	case L('s','l'):  // Slovenian
			tr->charset_a0 = charsets[2];   // ISO-8859-2
			tr->langopts.stress_rule = STRESSPOSN_2R;   // Temporary
			tr->langopts.stress_flags = 0x20;
			tr->langopts.param[LOPT_REGRESSIVE_VOICING] = 0x13;
			tr->langopts.numbers =  NUM_DECIMAL_COMMA | NUM_ALLOW_SPACE | NUM_SWAP_TENS | NUM_OMIT_1_HUNDRED | NUM_DFRACTION_2 | NUM_ORDINAL_DOT | NUM_ROMAN;
			tr->langopts.numbers2 = 0x100;   // plural forms of millions etc
			tr->langopts.thousands_sep = ' ';   // don't allow dot as thousands separator
			SetLetterVowel(tr,'r');
		break;

	case L('s','q'):  // Albanian
		{
			static const short stress_lengths_sq[8] = {150, 150,  180, 180,  0, 0,  300, 300};
			static const unsigned char stress_amps_sq[8] = {16,12, 16,16, 20,20, 21,19 };

			SetupTranslator(tr,stress_lengths_sq,stress_amps_sq);

			tr->langopts.stress_rule = STRESSPOSN_2R;
			tr->langopts.stress_flags =  0x16 + 0x100; 
			SetLetterVowel(tr,'y');
			tr->langopts.numbers = NUM_DECIMAL_COMMA | NUM_HUNDRED_AND | NUM_AND_UNITS | NUM_DFRACTION_4;
			tr->langopts.accents = 2;  // "capital" after letter name
		}
		break;


	case L('s','v'):  // Swedish
		{
			static const unsigned char stress_amps_sv[] = {16,16, 20,20, 20,22, 22,21 };
			static const short stress_lengths_sv[8] = {160,135, 220,220, 0,0, 250,280};
			SetupTranslator(tr,stress_lengths_sv,stress_amps_sv);

			tr->langopts.stress_rule = STRESSPOSN_1L;
			SetLetterVowel(tr,'y');
			tr->langopts.numbers = NUM_SINGLE_STRESS | NUM_DECIMAL_COMMA | NUM_ALLOW_SPACE | NUM_1900;
			tr->langopts.accents = 1;
		}
		break;

	case L('s','w'):  // Swahili
	case L('t','n'):  // Setswana
		{
			static const short stress_lengths_sw[8] = {160, 170,  200, 200,    0,   0,  320, 340};
			static const unsigned char stress_amps_sw[] = {16,12, 19,19, 20,22, 22,21 };

			SetupTranslator(tr,stress_lengths_sw,stress_amps_sw);
			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable

			tr->langopts.vowel_pause = 1;
			tr->langopts.stress_rule = STRESSPOSN_2R;
			tr->langopts.stress_flags =  0x6 | 0x10; 

			tr->langopts.numbers = NUM_AND_UNITS | NUM_HUNDRED_AND | NUM_SINGLE_AND | NUM_OMIT_1_HUNDRED;
			tr->langopts.break_numbers = 0x49249268;  // for languages which have numbers for 100,000 and 1,000,000
		}
		break;

	case L('t','a'):  // Tamil
	case L('m','l'):  // Malayalam
	case L('k','n'):  // Kannada
	case L('m','r'):  // Marathi
	case L('t','e'):  // Telugu
		{
			SetupTranslator(tr,stress_lengths_ta,stress_amps_ta);
			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable

			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.stress_flags =  0x10004;   // use 'diminished' for unstressed final syllable
			tr->langopts.spelling_stress = 1;
			tr->langopts.break_numbers = 0x14a8;  // 1000, 100,000  10,000,000 

			if(name2 == L('t','a'))
			{
				tr->letter_bits_offset = OFFSET_TAMIL;
				tr->langopts.numbers =  NUM_OMIT_1_THOUSAND ;
			}
			if(name2 == L('m','r'))
			{
				tr->letter_bits_offset = OFFSET_DEVANAGARI;
			}
			else
			if(name2 == L('m','l'))
			{
				tr->letter_bits_offset = OFFSET_MALAYALAM;
				tr->langopts.numbers = NUM_OMIT_1_THOUSAND;
			}
			else
			if(name2 == L('k','n'))
			{
				tr->letter_bits_offset = OFFSET_KANNADA;
				tr->langopts.numbers = 0x1;
			}
			else
			if(name2 == L('t','e'))
			{
				tr->letter_bits_offset = OFFSET_TELUGU;
				tr->langopts.numbers = 0x1;
			}
			tr->langopts.param[LOPT_WORD_MERGE] = 1;   // don't break vowels betwen words
			SetIndicLetters(tr);   // call this after setting OFFSET_
				SetLetterBitsRange(tr,LETTERGP_B,0x4e,0x4e);   // chillu-virama (unofficial)
		}
		break;

#ifdef deleted
	case L('t','h'):  // Thai
		{
			static const short stress_lengths_th[8] = {230,150, 230,230, 230,0, 230,250};
			static const unsigned char stress_amps_th[] = {22,16, 22,22, 22,22, 22,22 };

			SetupTranslator(tr,stress_lengths_th,stress_amps_th);

			tr->langopts.stress_rule = 0;   // stress on final syllable of a "word"
			tr->langopts.stress_flags = 2;          // don't automatically set diminished stress (may be set in the intonation module)
			tr->langopts.tone_language = 1;   // Tone language, use  CalcPitches_Tone() rather than CalcPitches()
			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable
//			tr->langopts.tone_numbers = 1;   // a number after letters indicates a tone number (eg. pinyin or jyutping)
			tr->langopts.word_gap = 0x21;   // length of a final vowel is less dependent on the next consonant, don't merge consonant with next word
		}
		break;
#endif

	case L('t','r'):   // Turkish
		{
			static const unsigned char stress_amps_tr[8] = {18,16, 20,21, 20,21, 21,20 };
			static const short stress_lengths_tr[8] = {190,180, 200,230, 0,0, 240,250};

			SetupTranslator(tr,stress_lengths_tr,stress_amps_tr);
			tr->charset_a0 = charsets[9];   // ISO-8859-9 - Latin5

			tr->langopts.stress_rule = 7;   // stress on the last syllable, before any explicitly unstressed syllable
			tr->langopts.stress_flags = 0x20;  //no automatic secondary stress

			tr->langopts.numbers = NUM_SINGLE_STRESS | NUM_DECIMAL_COMMA | NUM_OMIT_1_HUNDRED | NUM_OMIT_1_THOUSAND | NUM_DFRACTION_2;
			tr->langopts.max_initial_consonants = 2;
		}
		break;

	case L('u','r'):   // Urdu
			tr->letter_bits_offset = OFFSET_ARABIC;
			tr->langopts.param[LOPT_UNPRONOUNCABLE] = 1;   // disable check for unpronouncable words
			tr->langopts.numbers = NUM_SWAP_TENS;
			tr->langopts.break_numbers = 0x52a8;  // for languages which have numbers for 100,000 and 100,00,000, eg Hindi
		break;

	case L('v','i'):  // Vietnamese
		{
			static const short stress_lengths_vi[8] = {150, 150,  180, 180,  210, 230,  230, 240};
			static const unsigned char stress_amps_vi[] = {16,16, 16,16, 22,22, 22,22 };
			static wchar_t vowels_vi[] = {
				0x61, 0xe0, 0xe1, 0x1ea3, 0xe3, 0x1ea1,			// a
				0x103, 0x1eb1, 0x1eaf, 0x1eb3, 0x1eb5, 0x1eb7,	// ă
				0xe2, 0x1ea7, 0x1ea5, 0x1ea9, 0x1eab, 0x1ead,	// â
				0x65, 0xe8, 0xe9, 0x1ebb, 0x1ebd, 0x1eb9,			// e
				0xea, 0x1ec1, 0x1ebf, 0x1ec3, 0x1ec5, 0x1ec7,	// i
				0x69, 0xec, 0xed, 0x1ec9, 0x129, 0x1ecb,			// i
				0x6f, 0xf2, 0xf3, 0x1ecf, 0xf5, 0x1ecd,			// o
				0xf4, 0x1ed3, 0x1ed1, 0x1ed5, 0x1ed7, 0x1ed9, 	// ô
				0x1a1, 0x1edd, 0x1edb, 0x1edf, 0x1ee1, 0x1ee3,	// ơ
				0x75, 0xf9, 0xfa, 0x1ee7, 0x169, 0x1ee5,			// u
				0x1b0, 0x1eeb, 0x1ee9, 0x1eed, 0x1eef, 0x1ef1,	// ư
				0x79, 0x1ef3, 0xfd, 0x1ef7, 0x1ef9, 0x1e, 0 };	// y

			SetupTranslator(tr,stress_lengths_vi,stress_amps_vi);
			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable

			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.word_gap = 0x21;   // length of a final vowel is less dependent on the next consonant, don't merge consonant with next word
//			tr->langopts.vowel_pause = 4;
			tr->letter_groups[0] = tr->letter_groups[7] = vowels_vi;
			tr->langopts.tone_language = 1;   // Tone language, use  CalcPitches_Tone() rather than CalcPitches()
			tr->langopts.unstressed_wd1 = 2;
			tr->langopts.numbers = NUM_DECIMAL_COMMA | NUM_HUNDRED_AND_DIGIT | NUM_DFRACTION_4 | NUM_ZERO_HUNDRED;

		}
		break;

	case L('w','o'):
			tr->langopts.stress_rule = STRESSPOSN_1L;
			tr->langopts.numbers = NUM_AND_UNITS | NUM_HUNDRED_AND | NUM_OMIT_1_HUNDRED | NUM_OMIT_1_THOUSAND | NUM_SINGLE_STRESS;
		break;

	case L('z','h'):
	case L_zhy:
		{
			static const short stress_lengths_zh[8] = {230,150, 230,230, 230,0, 240,250};  // 1=tone5. end-of-sentence, 6=tone 1&4, 7=tone 2&3
			static const unsigned char stress_amps_zh[] = {22,16, 22,22, 22,22, 22,22 };

			SetupTranslator(tr,stress_lengths_zh,stress_amps_zh);

			tr->langopts.stress_rule = STRESSPOSN_1R;   // stress on final syllable of a "word"
			tr->langopts.stress_flags = 2;          // don't automatically set diminished stress (may be set in the intonation module)
			tr->langopts.vowel_pause = 0;
			tr->langopts.tone_language = 1;   // Tone language, use  CalcPitches_Tone() rather than CalcPitches()
			tr->langopts.length_mods0 = tr->langopts.length_mods;  // don't lengthen vowels in the last syllable
			tr->langopts.tone_numbers = 1;   // a number after letters indicates a tone number (eg. pinyin or jyutping)
			tr->langopts.ideographs = 1;
			tr->langopts.word_gap = 0x21;   // length of a final vowel is less dependent on the next consonant, don't merge consonant with next word
			if(name2 == L('z','h'))
			{
				tr->langopts.textmode = 1;
				tr->langopts.listx = 1;    // compile zh_listx after zh_list
			}
		}
		break;

	default:
		tr->langopts.param[LOPT_UNPRONOUNCABLE] = 1;   // disable check for unpronouncable words
		break;
	}

	tr->translator_name = name2;

	if(tr->langopts.numbers & NUM_DECIMAL_COMMA)
	{
		// use . and ; for thousands and decimal separators
		tr->langopts.thousands_sep = '.';
		tr->langopts.decimal_sep = ',';
	}
	if(tr->langopts.numbers & NUM_THOUS_SPACE)
	{
		tr->langopts.thousands_sep = 0;   // don't allow thousands separator, except space
	}

	return(tr);
}  // end of SelectTranslator



//**********************************************************************************************************




static void Translator_Russian(Translator *tr)
{//===========================================
	static const unsigned char stress_amps_ru[] = {16,16, 18,18, 20,24, 24,22 };
	static const short stress_lengths_ru[8] = {150,140, 220,220, 0,0, 260,280};
	static const char ru_ivowels2[] = {0x2c,0x15,0x18,0x2e,0x2f,0};   // add more vowels to letter group Y  (iotated vowels & soft-sign)

	SetupTranslator(tr,stress_lengths_ru,stress_amps_ru);
	SetCyrillicLetters(tr);
	SetLetterBits(tr,6,ru_ivowels2);

	tr->langopts.param[LOPT_UNPRONOUNCABLE] = 0x432;    // [v]  don't count this character at start of word
	tr->langopts.param[LOPT_REGRESSIVE_VOICING] = 1;
	tr->langopts.param[LOPT_REDUCE] = 2;
	tr->langopts.stress_rule = 5;
	tr->langopts.stress_flags = 0x0020;  // waas 0x1010

	tr->langopts.numbers = NUM_DECIMAL_COMMA | NUM_OMIT_1_HUNDRED;
	tr->langopts.numbers2 = 0xc2;  // variant numbers before thousands
	tr->langopts.phoneme_change = 1;
	tr->langopts.testing = 2;

}  // end of Translator_Russian



/*
typedef struct {
	int flags;
	unsigned char stress;          // stress level of this vowel
	unsigned char stress_highest;  // the highest stress level of a vowel in this word
	unsigned char n_vowels;        // number of vowels in the word
	unsigned char vowel_this;      // syllable number of this vowel (counting from 1)
	unsigned char vowel_stressed;  // syllable number of the highest stressed vowel
} CHANGEPH;
*/



#ifdef RUSSIAN2
// This is now done in the phoneme data, ph_russian

int ChangePhonemes_ru(Translator *tr, PHONEME_LIST2 *phlist, int n_ph, int index, PHONEME_TAB *ph, CHANGEPH *ch)
{//=============================================================================================================
// Called for each phoneme in the phoneme list, to allow a language to make changes
// ph     The current phoneme

	int variant;
	int vowelix;
	PHONEME_TAB *prev, *next;

	if(ch->flags & 8)
		return(0);    // full phoneme translation has already been given
	// Russian vowel softening and reduction rules

	if(ph->type == phVOWEL)
	{
		int prestressed = ch->vowel_stressed==ch->vowel_this+1;  // the next vowel after this has the main stress

		#define N_VOWELS_RU   11
                static unsigned int vowels_ru[N_VOWELS_RU] = {'a','V','O','I',PH('I','#'),PH('E','#'),PH('E','2'),
PH('V','#'),PH('I','3'),PH('I','2'),PH('E','3')};


                static unsigned int vowel_replace[N_VOWELS_RU][6] = {
                        // stressed, soft, soft-stressed, j+stressed, j+soft, j+soft-stressed
                /*0*/        {'A', 'I', PH('j','a'),         'a', 'a', 'a'},                // a   Uses 3,4,5 columns.
                /*1*/        {'A', 'V', PH('j','a'),         'a', 'V', 'a'},                // V   Uses 3,4,5 columns.
                /*2*/        {'o', '8', '8',                 'o', '8', '8'},                // O
                /*3*/        {'i', 'I', 'i',                 'a', 'I', 'a'},                // I  Uses 3,4,5 columns.
                /*4*/        {'i', PH('I','#'), 'i',         'i', PH('I','#'), 'i'},        // I#
                /*5*/        {'E', PH('E','#'), 'E',         'e', PH('E','#'), 'e'},        // E# 
                /*6*/        {'E', PH('E','2'), 'E',         'e', PH('E','2'), 'e'},        // E2  Uses 3,4,5 columns.
                /*7*/        {PH('j','a'), 'V', PH('j','a'), 'A', 'V', 'A'},                // V#
                /*8*/        {PH('j','a'), 'I', PH('j','a'), 'e', 'I', 'e'},                // I3 Uses 3,4,5 columns.
                /*9*/        {'e', 'I', 'e',                 'e', 'I', 'e'},                // I2
                /*10*/       {'e', PH('E', '2'), 'e',        'e', PH('E','2'), 'e'}         // E3
                };

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

		if(prestressed)
		{
			if((vowelix==6)&&(prev->mnemonic=='j'))
				vowelix=8;
			if(vowelix==1)
				vowelix=0;
			if(vowelix==4)
				vowelix=3;
			if(vowelix==6)
				vowelix=5;
			if(vowelix==7)
				vowelix=8;
			if(vowelix==10)
				vowelix=9;
		}
		// do we need a variant of this vowel, depending on the stress and adjacent phonemes ?
		variant = -1;
		int stressed = ch->flags & 2;
		int soft=prev->phflags & phPALATAL;

		if (soft && stressed)
			variant = 2; else
				if (stressed)
					variant = 0; else
						if (soft)
							variant = 1;
		if(variant >= 0)
		{
			if(prev->mnemonic == 'j')
				variant += 3;

			phlist[index].phcode = PhonemeCode(vowel_replace[vowelix][variant]);
		}
		else
		{
			phlist[index].phcode = PhonemeCode(vowels_ru[vowelix]);
		}
	}

	return(0);
}

#endif

