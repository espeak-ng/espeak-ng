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
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "translate.h"



#define M_NAME      0
#define M_SMALLCAP  1
#define M_TURNED    2
#define M_REVERSED  3
#define M_CURL      4

#define M_ACUTE     5
#define M_BREVE     6
#define M_CARON     7
#define M_CEDILLA   8
#define M_CIRCUMFLEX 9
#define M_DIAERESIS 10
#define M_DOUBLE_ACUTE 11
#define M_DOT_ABOVE 12
#define M_GRAVE     13
#define M_MACRON    14
#define M_OGONEK    15
#define M_RING      16
#define M_STROKE    17
#define M_TILDE     18

#define M_BAR       19
#define M_RETROFLEX 20
#define M_HOOK      21


#define M_MIDDLE_DOT  M_DOT_ABOVE  // duplicate of M_DOT_ABOVE
#define M_IMPLOSIVE   M_HOOK

typedef struct {
const char *name;
int  flags;
} ACCENTS;

// these are tokens to look up in the *_list file.
static ACCENTS accents_tab[] = {
{"_lig", 1},
{"_smc", 1},  // smallcap
{"_tur", 1},  // turned
{"_rev", 1},  // reversed
{"_crl", 0},  // curl

{"_acu", 0},  // acute
{"_brv", 0},  // breve
{"_hac", 0},  // caron/hacek
{"_ced", 0},  // cedilla
{"_cir", 0},  // circumflex
{"_dia", 0},  // diaeresis
{"_ac2", 0},  // double acute
{"_dot", 0},  // dot
{"_grv", 0},  // grave
{"_mcn", 0},  // macron
{"_ogo", 0},  // ogonek
{"_rng", 0},  // ring
{"_stk", 0},  // stroke
{"_tld", 0},  // tilde

{"_bar", 0},  // bar
{"_rfx", 0},  // retroflex
{"_hok", 0},  // hook
};


#define CAPITAL  0
#define LETTER(ch,mod1,mod2) (ch-59)+(mod1 << 6)+(mod2 << 11)
#define LIGATURE(ch1,ch2,mod1) (ch1-59)+((ch2-59) << 6)+(mod1 << 12)+0x8000


#define L_ALPHA  60   // U+3B1
#define L_SCHWA  61   // U+259
#define L_OPEN_E 62   // U+25B
#define L_GAMMA  63   // U+3B3
#define L_IOTA   64   // U+3B9
#define L_OE     65   // U+153
#define L_OMEGA  66   // U+3C9

#define L_PHI    67   // U+3C6
#define L_ESH    68   // U+283
#define L_UPSILON 69 // U+3C5
#define L_EZH     70 // U+292
#define L_GLOTTAL 71 // U+294
#define L_RTAP    72 // U+27E


static const short non_ascii_tab[] = {
    0, 0x3b1, 0x259, 0x25b, 0x3b3, 0x3b9, 0x153, 0x3c9,
0x3c6, 0x283, 0x3c5, 0x292, 0x294, 0x27e };


// characters U+00e0 to U+017f
static const unsigned short letter_accents_0e0[] = {
LETTER('a',M_GRAVE,0),   // U+00e0
LETTER('a',M_ACUTE,0),
LETTER('a',M_CIRCUMFLEX,0),
LETTER('a',M_TILDE,0),
LETTER('a',M_DIAERESIS,0),
LETTER('a',M_RING,0),
LIGATURE('a','e',0),
LETTER('c',M_CEDILLA,0),
LETTER('e',M_GRAVE,0),
LETTER('e',M_ACUTE,0),
LETTER('e',M_CIRCUMFLEX,0),
LETTER('e',M_DIAERESIS,0),
LETTER('i',M_GRAVE,0),
LETTER('i',M_ACUTE,0),
LETTER('i',M_CIRCUMFLEX,0),
LETTER('i',M_DIAERESIS,0),
LETTER('d',M_NAME,0),  // eth  // U+00f0
LETTER('n',M_TILDE,0),
LETTER('o',M_GRAVE,0),
LETTER('o',M_ACUTE,0),
LETTER('o',M_CIRCUMFLEX,0),
LETTER('o',M_TILDE,0),
LETTER('o',M_DIAERESIS,0),
0,     // division sign
LETTER('o',M_STROKE,0),
LETTER('u',M_GRAVE,0),
LETTER('u',M_ACUTE,0),
LETTER('u',M_CIRCUMFLEX,0),
LETTER('u',M_DIAERESIS,0),
LETTER('y',M_ACUTE,0),
LETTER('t',M_NAME,0),  // thorn
LETTER('y',M_DIAERESIS,0),
CAPITAL,                 // U+0100
LETTER('a',M_MACRON,0),
CAPITAL,
LETTER('a',M_BREVE,0),
CAPITAL,
LETTER('a',M_OGONEK,0),
CAPITAL,
LETTER('c',M_ACUTE,0),
CAPITAL,
LETTER('c',M_CIRCUMFLEX,0),
CAPITAL,
LETTER('c',M_DOT_ABOVE,0),
CAPITAL,
LETTER('c',M_CARON,0),
CAPITAL,
LETTER('d',M_CARON,0),
CAPITAL,                 // U+0110
LETTER('d',M_STROKE,0),
CAPITAL,
LETTER('e',M_MACRON,0),
CAPITAL,
LETTER('e',M_BREVE,0),
CAPITAL,
LETTER('e',M_DOT_ABOVE,0),
CAPITAL,
LETTER('e',M_OGONEK,0),
CAPITAL,
LETTER('e',M_CARON,0),
CAPITAL,
LETTER('g',M_CIRCUMFLEX,0),
CAPITAL,
LETTER('g',M_BREVE,0),
CAPITAL,                // U+0120
LETTER('g',M_DOT_ABOVE,0),
CAPITAL,
LETTER('g',M_CEDILLA,0),
CAPITAL,
LETTER('h',M_CIRCUMFLEX,0),
CAPITAL,
LETTER('h',M_STROKE,0),
CAPITAL,
LETTER('i',M_TILDE,0),
CAPITAL,
LETTER('i',M_MACRON,0),
CAPITAL,
LETTER('i',M_BREVE,0),
CAPITAL,
LETTER('i',M_OGONEK,0),
CAPITAL,               // U+0130
LETTER('i',M_NAME,0), // dotless i
CAPITAL,
LIGATURE('i','j',0),
CAPITAL,
LETTER('j',M_CIRCUMFLEX,0),
CAPITAL,
LETTER('k',M_CEDILLA,0),
LETTER('k',M_NAME,0),  // kra
CAPITAL,
LETTER('l',M_ACUTE,0),
CAPITAL,
LETTER('l',M_CEDILLA,0),
CAPITAL,
LETTER('l',M_CARON,0),
CAPITAL,
LETTER('l',M_MIDDLE_DOT,0),  // U+0140
CAPITAL,
LETTER('l',M_STROKE,0),
CAPITAL,
LETTER('n',M_ACUTE,0),
CAPITAL,
LETTER('n',M_CEDILLA,0),
CAPITAL,
LETTER('n',M_CARON,0),
LETTER('n',M_NAME,0),  // apostrophe n
CAPITAL,
LETTER('n',M_NAME,0),  // eng
CAPITAL,
LETTER('o',M_MACRON,0),
CAPITAL,
LETTER('o',M_BREVE,0),
CAPITAL,             // U+0150
LETTER('o',M_DOUBLE_ACUTE,0),
CAPITAL,
LIGATURE('o','e',0),
CAPITAL,
LETTER('r',M_ACUTE,0),
CAPITAL,
LETTER('r',M_CEDILLA,0),
CAPITAL,
LETTER('r',M_CARON,0),
CAPITAL,
LETTER('s',M_ACUTE,0),
CAPITAL,
LETTER('s',M_CIRCUMFLEX,0),
CAPITAL,
LETTER('s',M_CEDILLA,0),
CAPITAL,              // U+0160
LETTER('s',M_CARON,0),
CAPITAL,
LETTER('t',M_CEDILLA,0),
CAPITAL,
LETTER('t',M_CARON,0),
CAPITAL,
LETTER('t',M_STROKE,0),
CAPITAL,
LETTER('u',M_TILDE,0),
CAPITAL,
LETTER('u',M_MACRON,0),
CAPITAL,
LETTER('u',M_BREVE,0),
CAPITAL,
LETTER('u',M_RING,0),
CAPITAL,              // U+0170
LETTER('u',M_DOUBLE_ACUTE,0),
CAPITAL,
LETTER('u',M_OGONEK,0),
CAPITAL,
LETTER('w',M_CIRCUMFLEX,0),
CAPITAL,
LETTER('y',M_CIRCUMFLEX,0),
CAPITAL,   // Y-DIAERESIS
CAPITAL,
LETTER('z',M_ACUTE,0),
CAPITAL,
LETTER('z',M_DOT_ABOVE,0),
CAPITAL,
LETTER('z',M_CARON,0),
LETTER('s',M_NAME,0), // long-s  // U+17f
};


// characters U+0250 to U+029F
static const unsigned short letter_accents_250[] = {
LETTER('a',M_TURNED,0),		// U+250
LETTER(L_ALPHA,0,0),
LETTER(L_ALPHA,M_TURNED,0),
LETTER('b',M_IMPLOSIVE,0),
0,  // open-o
LETTER('c',M_CURL,0),
LETTER('d',M_RETROFLEX,0),
LETTER('d',M_IMPLOSIVE,0),
LETTER('e',M_REVERSED,0),	// U+258
0,   // schwa
LETTER(L_SCHWA,M_HOOK,0),
0,   // open-e
LETTER(L_OPEN_E,M_REVERSED,0),
LETTER(L_OPEN_E,M_HOOK,M_REVERSED),
0,//LETTER(L_OPEN_E,M_CLOSED,M_REVERSED),
LETTER('j',M_BAR,0),
LETTER('g',M_IMPLOSIVE,0),	// U+260
LETTER('g',0,0),
LETTER('g',M_SMALLCAP,0),
LETTER(L_GAMMA,0,0),
0,   // ramshorn
LETTER('h',M_TURNED,0),
LETTER('h',M_HOOK,0),
0,//LETTER(L_HENG,M_HOOK,0),
LETTER('i',M_BAR,0),		// U+268
LETTER(L_IOTA,0,0),
LETTER('i',M_SMALLCAP,0),
LETTER('l',M_TILDE,0),
LETTER('l',M_BAR,0),
LETTER('l',M_RETROFLEX,0),
LIGATURE('l','z',0),
LETTER('m',M_TURNED,0),
0,//LETTER('m',M_TURNED,M_LEG),	// U+270
LETTER('m',M_HOOK,0),
0,//LETTER('n',M_LEFTHOOK,0),
LETTER('n',M_RETROFLEX,0),
LETTER('n',M_SMALLCAP,0),
LETTER('o',M_BAR,0),
LIGATURE('o','e',M_SMALLCAP),
0,//LETTER(L_OMEGA,M_CLOSED,0),
LETTER(L_PHI,0,0),		// U+278
LETTER('r',M_TURNED,0),
0,//LETTER('r',M_TURNED,M_LEG),
LETTER('r',M_RETROFLEX,M_TURNED),
0,//LETTER('r',M_LEG,0),
LETTER('r',M_RETROFLEX,0),
0,  // r-tap
LETTER(L_RTAP,M_REVERSED,0),
LETTER('r',M_SMALLCAP,0),	// U+280
LETTER('r',M_TURNED,M_SMALLCAP),
LETTER('s',M_RETROFLEX,0),
0,  // esh
0,//LETTER('j',M_BAR,L_IMPLOSIVE),
LETTER(L_ESH,M_REVERSED,0),
LETTER(L_ESH,M_CURL,0),
LETTER('t',M_TURNED,0),
LETTER('t',M_RETROFLEX,0),	// U+288
LETTER('u',M_BAR,0),
LETTER(L_UPSILON,0,0),
LETTER('v',M_HOOK,0),
LETTER('v',M_TURNED,0),
LETTER('w',M_TURNED,0),
LETTER('y',M_TURNED,0),
LETTER('y',M_SMALLCAP,0),
LETTER('z',M_RETROFLEX,0),	// U+290
LETTER('z',M_CURL,0),
0,  // ezh
LETTER(L_EZH,M_CURL,0),
0,  // glottal stop
LETTER(L_GLOTTAL,M_REVERSED,0),
LETTER(L_GLOTTAL,M_TURNED,0),
0,//LETTER('c',M_LONG,0),
0,  // bilabial click		// U+298
LETTER('b',M_SMALLCAP,0),
0,//LETTER(L_OPEN_E,M_CLOSED,0),
LETTER('g',M_IMPLOSIVE,M_SMALLCAP),
LETTER('h',M_SMALLCAP,0),
LETTER('j',M_CURL,0),
LETTER('k',M_TURNED,0),
LETTER('l',M_SMALLCAP,0),
LETTER('q',M_HOOK,0),      // U+2a0
LETTER(L_GLOTTAL,M_STROKE,0),
LETTER(L_GLOTTAL,M_STROKE,M_REVERSED),
LIGATURE('d','z',0),
0,   // dezh
LIGATURE('d','z',M_CURL),
LIGATURE('t','s',0),
0,   // tesh
LIGATURE('t','s',M_CURL),
};

static int LookupLetter2(Translator *tr, unsigned int letter, char *ph_buf)
{//========================================================================
	int len;
	char single_letter[10];

	single_letter[0] = 0;
	single_letter[1] = '_';
	len = utf8_out(letter, &single_letter[2]);
	single_letter[len+2] = ' ';
	single_letter[len+3] = 0;

	if(Lookup(tr, &single_letter[1], ph_buf) == 0)
	{
		single_letter[1] = ' ';
		if(Lookup(tr, &single_letter[2], ph_buf) == 0)
		{
			TranslateRules(tr, &single_letter[2], ph_buf, 20, NULL,0,NULL);
		}
	}
	return(ph_buf[0]);
}


void LookupAccentedLetter(Translator *tr, unsigned int letter, char *ph_buf)
{//=========================================================================
	// lookup the character in the accents table
	int accent_data = 0;
	int accent1 = 0;
	int accent2 = 0;
	int basic_letter;
	int letter2=0;
	char ph_letter1[30];
	char ph_letter2[30];
	char ph_accent1[30];
	char ph_accent2[30];

	ph_accent2[0] = 0;

	if((letter >= 0xe0) && (letter < 0x17f))
	{
		accent_data = letter_accents_0e0[letter - 0xe0];
	}
	else
	if((letter >= 0x250) && (letter <= 0x2a8))
	{
		accent_data = letter_accents_250[letter - 0x250];
	} 

	if(accent_data != 0)
	{
		basic_letter = (accent_data & 0x3f) + 59;
		if(basic_letter < 'a')
			basic_letter = non_ascii_tab[basic_letter-59];

		if(accent_data & 0x8000)
		{
			letter2 = (accent_data >> 6) & 0x3f;
			letter2 += 59;
			accent2 = (accent_data >> 12) & 0x7;
		}
		else
		{
			accent1 = (accent_data >> 6) & 0x1f;
			accent2 = (accent_data >> 11) & 0xf;
		}


		if(Lookup(tr, accents_tab[accent1].name, ph_accent1) != 0)
		{

			if(LookupLetter2(tr, basic_letter, ph_letter1) != 0)
			{
				if(accent2 != 0)
				{
					if(Lookup(tr, accents_tab[accent2].name, ph_accent2) == 0)
					{
//						break;
					}

					if(accents_tab[accent2].flags & 1)
					{
						strcpy(ph_buf,ph_accent2);
						ph_buf += strlen(ph_buf);
						ph_accent2[0] = 0;
					}
				}
				if(letter2 != 0)
				{
					//ligature
					LookupLetter2(tr, letter2, ph_letter2);
					sprintf(ph_buf,"%s%c%s%c%s%s",ph_accent1, phonPAUSE_VSHORT, ph_letter1, phonSTRESS_P, ph_letter2, ph_accent2);
				}
				else
				{
					if(accent1 == 0)
						strcpy(ph_buf, ph_letter1);
					else
					if((tr->langopts.accents & 1) || (accents_tab[accent1].flags & 1))
						sprintf(ph_buf,"%s%c%c%s", ph_accent1, phonPAUSE_VSHORT, phonSTRESS_P, ph_letter1);
					else
						sprintf(ph_buf,"%c%s%c%s%c", phonSTRESS_2, ph_letter1, phonPAUSE_VSHORT, ph_accent1, phonPAUSE_VSHORT);
				}
			}
		}
	}
}  // end of LookupAccentedLetter



void LookupLetter(Translator *tr, unsigned int letter, int next_byte, char *ph_buf1)
{//=================================================================================
	int len;
	unsigned char *p;
	static char single_letter[10] = {0,0};
	char ph_stress[2];
	unsigned int dict_flags[2];
	char ph_buf3[40];
	char *ptr;

	ph_buf1[0] = 0;
	len = utf8_out(letter,&single_letter[2]);
	single_letter[len+2] = ' ';

	if(next_byte == -1)
	{
		// speaking normal text, not individual characters
		if(Lookup(tr, &single_letter[2], ph_buf1) != 0)
			return;

		single_letter[1] = '_';
		if(Lookup(tr, &single_letter[1], ph_buf3) != 0)
			return;   // the character is specified as _* so ignore it when speaking normal text

		// check whether this character is specified for English
		if(tr->translator_name == L('e','n'))
			return;   // we are already using English

		SetTranslator2("en");
		if(Lookup(translator2, &single_letter[2], ph_buf3) != 0)
		{
			// yes, switch to English and re-translate the word
			sprintf(ph_buf1,"%c",phonSWITCH);
		}
		SelectPhonemeTable(voice->phoneme_tab_ix);  // revert to original phoneme table
		return;
	}

	if((letter <= 32) || iswspace(letter))
	{
		// lookup space as _&32 etc.
		sprintf(&single_letter[1],"_#%d ",letter);
		Lookup(tr, &single_letter[1], ph_buf1);
		return;
	}

	if(next_byte != ' ')
		next_byte = RULE_SPELLING;
	single_letter[3+len] = next_byte;   // follow by space-space if the end of the word, or space-0x31

	single_letter[1] = '_';

	// if the $accent flag is set for this letter, use the accents table (below)
	dict_flags[1] = 0;
	ptr = &single_letter[1];
	
	if(Lookup(tr, &single_letter[1], ph_buf3) == 0)
	{
		single_letter[1] = ' ';
		if(Lookup(tr, &single_letter[2], ph_buf3) == 0)
		{
			TranslateRules(tr, &single_letter[2], ph_buf3, sizeof(ph_buf3), NULL,FLAG_NO_TRACE,NULL);
		}
	}

	if(ph_buf3[0] == 0)
	{
		LookupAccentedLetter(tr, letter, ph_buf3);
	}

	if(ph_buf3[0] == 0)
	{
		ph_buf1[0] = 0;
		return;
	}
	if(ph_buf3[0] == phonSWITCH)
	{
		strcpy(ph_buf1,ph_buf3);
		return;
	}
	// at a stress marker at the start of the letter name, unless one is already marked
	ph_stress[0] = phonSTRESS_P;
	ph_stress[1] = 0;

	for(p=(unsigned char *)ph_buf3; *p != 0; p++)
	{
		if(phoneme_tab[*p]->type == phSTRESS)
			ph_stress[0] = 0;  // stress is already marked
	}
	sprintf(ph_buf1,"%s%s",ph_stress,ph_buf3);
}



int TranslateLetter(Translator *tr, char *word, char *phonemes, int control, int word_length)
{//======================================================================================
// get pronunciation for an isolated letter
// return number of bytes used by the letter
// control 2=say-as glyphs, 3-say-as chars
	int n_bytes;
	int letter;
	int len;
	int save_option_phonemes;
	char *p2;
	char *pbuf;
	char capital[20];
	char ph_buf[60];
	char ph_buf2[60];
	char hexbuf[6];

	ph_buf[0] = 0;
	capital[0] = 0;

	n_bytes = utf8_in(&letter,word);

	if((letter & 0xfff00) == 0x0e000)
	{
		letter &= 0xff;   // uncode private usage area
	}

	if(control > 2)
	{
		// include CAPITAL information
		if(iswupper(letter))
		{
			Lookup(tr, "_cap", capital);
		}
	}
	letter = towlower2(letter);

	LookupLetter(tr, letter, word[n_bytes], ph_buf);

	if(ph_buf[0] == phonSWITCH)
	{
		strcpy(phonemes,ph_buf);
		return(0);
	}

	if((ph_buf[0] == 0) && (tr->translator_name != L('e','n')))
	{
		// speak as English, check whether there is a translation for this character
		SetTranslator2("en");
		save_option_phonemes = option_phonemes;
		option_phonemes = 0;
		LookupLetter(translator2, letter, word[n_bytes], ph_buf);
		SelectPhonemeTable(voice->phoneme_tab_ix);  // revert to original phoneme table
		option_phonemes = save_option_phonemes;

		if(ph_buf[0] != 0)
		{
			sprintf(phonemes,"%cen",phonSWITCH);
			return(0);
		}
	}

	if(ph_buf[0] == 0)
	{
		// character name not found
		if(iswalpha(letter))
			Lookup(tr, "_?A", ph_buf);

		if((ph_buf[0]==0) && !iswspace(letter))
			Lookup(tr, "_??", ph_buf);

		if(ph_buf[0] != 0)
		{
			// speak the hexadecimal number of the character code
			sprintf(hexbuf,"%x",letter);
			pbuf = ph_buf;
			for(p2 = hexbuf; *p2 != 0; p2++)
			{
				pbuf += strlen(pbuf);
				*pbuf++ = phonPAUSE_VSHORT;
				LookupLetter(tr, *p2, 0, pbuf);
			}
		}
	}

	len = strlen(phonemes);
	if(tr->langopts.accents & 2)
		sprintf(ph_buf2,"%c%s%s",0xff,ph_buf,capital);
	else
		sprintf(ph_buf2,"%c%s%s",0xff,capital,ph_buf);  // the 0xff marker will be removed or replaced in SetSpellingStress()
	if((len + strlen(ph_buf2)) < N_WORD_PHONEMES)
	{
		strcpy(&phonemes[len],ph_buf2);
	}
	return(n_bytes);
}  // end of TranslateLetter



void SetSpellingStress(Translator *tr, char *phonemes, int control, int n_chars)
{//=============================================================================
// Individual letter names, reduce the stress of some.
	int ix;
	unsigned int c;
	int n_stress=0;
	int count;
	unsigned char buf[N_WORD_PHONEMES];

	for(ix=0; (c = phonemes[ix]) != 0; ix++)
	{
		if(c == phonSTRESS_P)
		{
			n_stress++;
		}
		buf[ix] = c;
	}
	buf[ix] = 0;

	count = 0;
	for(ix=0; (c = buf[ix]) != 0; ix++)
	{
		if((c == phonSTRESS_P) && (n_chars > 1))
		{
			count++;

			if(tr->langopts.spelling_stress == 1)
			{
				// stress on initial letter when spelling
				if(count > 1)
					c = phonSTRESS_3;
			}
			else
			{
				if(count != n_stress)
				{
					if(((count % 3) != 0) || (count == n_stress-1))
						c = phonSTRESS_3;   // reduce to secondary stress
				}
			}
		}
		else
		if(c == 0xff)
		{
			if((control < 2) || (ix==0))
				continue;   // don't insert pauses

			if(control == 4)
				c = phonPAUSE;    // pause after each character
			if(((count % 3) == 0) || (control > 2))
				c = phonPAUSE_SHORT;  // pause following a primary stress
			else
				continue;       // remove marker
		}
		*phonemes++ = c;
	}
	if(control >= 2)
		*phonemes++ = phonPAUSE_NOLINK;
	*phonemes = 0;
}  // end of SetSpellingStress



// Numbers

static char ph_ordinal2[12];

int TranslateRoman(Translator *tr, char *word, char *ph_out)
{//=====================================================
	int c;
	char *p;
	const char *p2;
	int acc;
	int prev;
	int value;
	int subtract;
	int repeat = 0;
	unsigned int flags;
	char ph_roman[30];
	char number_chars[N_WORD_BYTES];

	static const char *roman_numbers = "ixcmvld";
	static int roman_values[] = {1,10,100,1000,5,50,500};
 
	acc = 0;
	prev = 0;
	subtract = 0x7fff;

	while((c = *word++) != ' ')
	{
		if((p2 = strchr(roman_numbers,c)) == NULL)
			return(0);

		value = roman_values[p2 - roman_numbers];
		if(value == prev)
		{
			repeat++;
			if(repeat >= 3)
				return(0);
		}
		else
			repeat = 0;

		if((prev > 1) && (prev != 10) && (prev != 100))
		{
			if(value >= prev)
				return(0);
		}
		if((prev != 0) && (prev < value))
		{
			if(((acc % 10) != 0) || ((prev*10) < value))
				return(0);
			subtract = prev;
			value -= subtract;
		}
		else
		if(value >= subtract)
			return(0);
		else
			acc += prev;
		prev = value;
	}
	acc += prev;
	if(acc < 2)
		return(0);

	if(acc > tr->langopts.max_roman)
		return(0);

	Lookup(tr, "_roman",ph_roman);   // precede by "roman" if _rom is defined in *_list
	p = &ph_out[0];

	if((tr->langopts.numbers & NUM_ROMAN_AFTER) == 0)
	{
		strcpy(ph_out,ph_roman);
		p = &ph_out[strlen(ph_out)];
	}

	sprintf(number_chars," %d ",acc);
	TranslateNumber(tr, &number_chars[1], p, &flags, 0);

	if(tr->langopts.numbers & NUM_ROMAN_AFTER)
		strcat(ph_out,ph_roman);
	return(1);
}  // end of TranslateRoman


static const char *M_Variant(int value)
{//====================================
	// returns M, or perhaps MA for some cases
	
	if((translator->langopts.numbers2 & 0x100) && (value >= 2) && (value <= 4))
		return("0MA");  // Czech, Slovak
	else
	if(((value % 100) < 10) || ((value % 100) > 20))   // but not teens, 10 to 19
	{
		if ((translator->langopts.numbers2 & 0x40) &&
			((value % 10)>=2) &&
			((value % 10)<=4))
		{
		// for Polish language - two forms of plural!
			return("0MA");
		}

		if((translator->langopts.numbers2 & 0x80) &&
			((value % 10)==1))
		{
			return("1MA");
		}

	}
	return("0M");
}


static int LookupThousands(Translator *tr, int value, int thousandplex, char *ph_out)
{//==================================================================================
	int found;
	char string[12];
	char ph_of[12];
	char ph_thousands[40];

	ph_of[0] = 0;

	// first look fora match with the exact value of thousands
	sprintf(string,"_%dM%d",value,thousandplex);

	if((found = Lookup(tr, string, ph_thousands)) == 0)
	{
		if((value % 100) >= 20) 
		{
			Lookup(tr, "_0of", ph_of);
		}

		sprintf(string,"_%s%d",M_Variant(value),thousandplex);

		if(Lookup(tr, string, ph_thousands) == 0)
		{
			// repeat "thousand" if higher order names are not available
			sprintf(string,"_%dM1",value);
			if((found = Lookup(tr, string, ph_thousands)) == 0)
				Lookup(tr, "_0M1", ph_thousands);
		}
	}
	sprintf(ph_out,"%s%s",ph_of,ph_thousands);
	return(found);
}


static int LookupNum2(Translator *tr, int value, int control, char *ph_out)
{//========================================================================
// Lookup a 2 digit number
// control bit 0: tens and units (use special form of '1')
// control bit 1: ordinal number
// control bit 2: use feminine form of '2'
// control bit 3: speak zero tens

	int found;
	int ix;
	int units;
	int used_and=0;
	int found_ordinal = 0;
	int next_phtype;
	char string[12];  // for looking up entries in *_list
	char ph_ordinal[20];
	char ph_tens[50];
	char ph_digits[50];
	char ph_and[12];

	// is there a special pronunciation for this 2-digit number
	found = 0;
	ph_ordinal[0] = 0;

	if(control & 4)
	{
		sprintf(string,"_%df",value);
		found = Lookup(tr, string, ph_digits);
	}
	if(control & 2)
	{
		strcpy(ph_ordinal, ph_ordinal2);

		sprintf(string,"_%do",value);
		if((found = Lookup(tr, string, ph_digits)) != 0)
		{
			found_ordinal = 1;
		}
	}

	if(found == 0)
	{
		if((value == 1) && (control & 1))
		{
			if(Lookup(tr, "_1a", ph_out) != 0)
				return(0);
		}
		sprintf(string,"_%d",value);
		found = Lookup(tr, string, ph_digits);
	}

	// no, speak as tens+units

	if((control & 8) && (value < 10))
	{
		// speak leading zero
		Lookup(tr, "_0", ph_tens);
	}
	else
	{
		if(found)
		{
			ph_tens[0] = 0;
		}
		else
		{
			units = (value % 10);

			if((control & 2) && ((units == 0) || (tr->langopts.numbers & 0x10)))
			{
				sprintf(string,"_%dXo",value / 10);
				if(Lookup(tr, string, ph_tens) != 0)
				{
					found_ordinal = 1;
				}
			}
			if(found_ordinal == 0)
			{
				sprintf(string,"_%dX",value / 10);
				Lookup(tr, string, ph_tens);
			}

			if((ph_tens[0] == 0) && (tr->langopts.numbers & NUM_VIGESIMAL))
			{
				// tens not found,  (for example) 73 is 60+13
				units = (value % 20);
				sprintf(string,"_%dX",(value / 10) & 0xfe);
				Lookup(tr, string, ph_tens);
			}

			ph_digits[0] = 0;
			if(units > 0)
			{	
				found = 0;
				if(control & 4)
				{
					// is there a variant form of this number?
					sprintf(string,"_%df",units);
					found = Lookup(tr, string, ph_digits);
				}
				if((control & 2) && ((tr->langopts.numbers & 0x10) == 0))
				{
					// ordinal
					sprintf(string,"_%do",units);
					if((found = Lookup(tr, string, ph_digits)) != 0)
					{
						found_ordinal = 1;
					}
				}
				if(found == 0)
				{
					sprintf(string,"_%d",units);
					Lookup(tr, string, ph_digits);
				}
			}
		}
	}

	if((control & 2) && (found_ordinal == 0) && (ph_ordinal[0] == 0))
	{
		if((value >= 20) && (((value % 10) == 0) || (tr->langopts.numbers & 0x10)))
			Lookup(tr, "_ord20", ph_ordinal);
		if(ph_ordinal[0] == 0)
			Lookup(tr, "_ord", ph_ordinal);
	}

	if((tr->langopts.numbers & 0x30) && (ph_tens[0] != 0) && (ph_digits[0] != 0))
	{
		Lookup(tr, "_0and", ph_and);
		if(tr->langopts.numbers & 0x10)
			sprintf(ph_out,"%s%s%s%s",ph_digits, ph_and, ph_tens, ph_ordinal);
		else
			sprintf(ph_out,"%s%s%s%s",ph_tens, ph_and, ph_digits, ph_ordinal);
		used_and = 1;
	}
	else
	{
		if(tr->langopts.numbers & 0x200)
		{
			// remove vowel from the end of tens if units starts with a vowel (LANG=Italian)
			if(((ix = strlen(ph_tens)-1) >= 0) && (ph_digits[0] != 0))
			{
				if((next_phtype = phoneme_tab[(unsigned int)(ph_digits[0])]->type) == phSTRESS)
					next_phtype = phoneme_tab[(unsigned int)(ph_digits[1])]->type;
	
				if((phoneme_tab[(unsigned int)(ph_tens[ix])]->type == phVOWEL) && (next_phtype == phVOWEL))
					ph_tens[ix] = 0;
			}
		}
		sprintf(ph_out,"%s%s%s",ph_tens, ph_digits, ph_ordinal);
	}

	if(tr->langopts.numbers & 0x100)
	{
		// only one primary stress
		found = 0;
		for(ix=strlen(ph_out)-1; ix>=0; ix--)
		{
			if(ph_out[ix] == phonSTRESS_P)
			{
				if(found)
					ph_out[ix] = phonSTRESS_3;
				else
					found = 1;
			}
		}
	}
	return(used_and);
}  // end of LookupNum2


static int LookupNum3(Translator *tr, int value, char *ph_out, int suppress_null, int thousandplex, int control)
{//=============================================================================================================
// Translate a 3 digit number
//  control  bit 0,  previous thousands
//           bit 1,  ordinal number
	int found;
	int hundreds;
	int x;
	char string[12];  // for looking up entries in **_list
	char buf1[100];
	char buf2[100];
	char ph_100[20];
	char ph_10T[20];
	char ph_digits[50];
	char ph_thousands[50];
	char ph_hundred_and[12];
	char ph_thousand_and[12];
	
	hundreds = value / 100;
	buf1[0] = 0;

	if(hundreds > 0)
	{
		ph_thousands[0] = 0;
		ph_thousand_and[0] = 0;

		found = 0;
		if((control & 2) && ((value % 100) == 0))
		{
			// ordinal number, with no tens or units
			found = Lookup(tr, "_0Co", ph_100);
		}
		if(found == 0)
		{
			Lookup(tr, "_0C", ph_100);
		}

		if(((tr->langopts.numbers & 0x0800) != 0) && (hundreds == 19))
		{
			// speak numbers such as 1984 as years: nineteen-eighty-four
//			ph_100[0] = 0;   // don't say "hundred", we also need to surpess "and"
		}
		else
		if(hundreds >= 10)
		{
			ph_digits[0] = 0;

			if(LookupThousands(tr, hundreds / 10, thousandplex+1, ph_10T) == 0)
			{
				x = 0;
				if(tr->langopts.numbers2 & (1 << (thousandplex+1)))
					x = 4;
				LookupNum2(tr, hundreds/10, x, ph_digits);
			}

			if(tr->langopts.numbers2 & 0x200)
				sprintf(ph_thousands,"%s%s%c",ph_10T,ph_digits,phonPAUSE_NOLINK);  // say "thousands" before its number, not after
			else
				sprintf(ph_thousands,"%s%s%c",ph_digits,ph_10T,phonPAUSE_NOLINK);

			hundreds %= 10;
			if(hundreds == 0)
				ph_100[0] = 0;
			suppress_null = 1;
		}

		ph_digits[0] = 0;
		if(hundreds > 0)
		{
			if((tr->langopts.numbers & 0x100000) && ((control & 1) || (ph_thousands[0] != 0)))
			{
				Lookup(tr, "_0and", ph_thousand_and);
			}

			suppress_null = 1;

			found = 0;
			if((value % 1000) == 100)
			{
				// is there a special pronunciation for exactly 100 ?
				found = Lookup(tr, "_1C0", ph_digits);
			}
			if(!found)
			{
				sprintf(string,"_%dC",hundreds);
				found = Lookup(tr, string, ph_digits);  // is there a specific pronunciation for n-hundred ?
			}

			if(found)
			{
				ph_100[0] = 0;
			}
			else
			{
				if((hundreds > 1) || ((tr->langopts.numbers & 0x400) == 0))
				{
					LookupNum2(tr, hundreds, 0, ph_digits);
				}
			}
		}

		sprintf(buf1,"%s%s%s%s",ph_thousands,ph_thousand_and,ph_digits,ph_100);
	}

	ph_hundred_and[0] = 0;
	if((tr->langopts.numbers & 0x40) && ((value % 100) != 0))
	{
		if((value > 100) || ((control & 1) && (thousandplex==0)))
		{
			Lookup(tr, "_0and", ph_hundred_and);
		}
	}


	buf2[0] = 0;
	value = value % 100;

	if((value != 0) || (suppress_null == 0))
	{
		x = 0;
		if(thousandplex==0)
		{
			x = 1;   // allow "eins" for 1 rather than "ein"
			if(control & 2)
				x = 3;   // ordinal number
		}
		else
		{
			if(tr->langopts.numbers2 & (1 << thousandplex))
				x = 4;   // use variant (feminine) for before thousands and millions
		}

		if(LookupNum2(tr, value, x, buf2) != 0)
		{
			if(tr->langopts.numbers & 0x80)
				ph_hundred_and[0] = 0;  // don't put 'and' after 'hundred' if there's 'and' between tens and units
		}
	}

	sprintf(ph_out,"%s%s%s",buf1,ph_hundred_and,buf2);

	return(0);
}  // end of LookupNum3


static int TranslateNumber_1(Translator *tr, char *word, char *ph_out, unsigned int *flags, int wflags)
{//====================================================================================================
//  Number translation with various options
// the "word" may be up to 4 digits
// "words" of 3 digits may be preceded by another number "word" for thousands or millions

	int n_digits;
	int value;
	unsigned int ix;
	unsigned char c;
	int suppress_null = 0;
	int decimal_point = 0;
	int thousandplex = 0;
	int thousands_inc = 0;
	int prev_thousands = 0;
	int ordinal = 0;
	int this_value;
	static int prev_value;
	int decimal_count;
	int max_decimal_count;
	int decimal_mode;
	int hyphen;
	char *p;
	char string[20];  // for looking up entries in **_list
	char buf1[100];
	char ph_append[50];
	char ph_buf[200];
	char ph_buf2[50];
	char suffix[20];

	static const char str_pause[2] = {phonPAUSE_NOLINK,0};

	*flags = 0;

	for(ix=0; isdigit(word[ix]); ix++) ;
	n_digits = ix;
	value = this_value = atoi(word);

	ph_ordinal2[0] = 0;
	if((tr->langopts.numbers & 0x10000) && (word[ix] == '.') && !isdigit(word[ix+2]))
	{
		// ordinal number is indicated by dot after the number
		ordinal = 2;
		word[ix] = ' ';
	}
	else
	{
		// look for an ordinal number suffix after the number
		ix++;
		hyphen = 0;
		p = suffix;
		if(word[ix] == '-')
		{
			*p++ = '-';
			hyphen = 1;
			ix += 2;
		}
		while((word[ix] != 0) && (word[ix] != ' ') && (ix < (sizeof(suffix)-1)))
		{
			*p++ = word[ix++];
		}
		*p = 0;

		if(suffix[0] != 0)
		{
			sprintf(string,"_0%s",suffix);
			if(Lookup(tr, string, ph_ordinal2))
			{
				// this is an ordinal suffix
				ordinal = 2;
				flags[0] |= FLAG_SKIPWORDS;
				dictionary_skipwords = 1 + hyphen;
			}
		}
	}

	ph_append[0] = 0;
	ph_buf2[0] = 0;

	// is there a previous thousands part (as a previous "word") ?
	if((n_digits == 3) && (word[-2] == tr->langopts.thousands_sep) && isdigit(word[-3]))
	{
		prev_thousands = 1;
	}
	else
	if((tr->langopts.thousands_sep == ' ') || (tr->langopts.numbers & 0x1000))
	{
		// thousands groups can be separated by spaces
		if((n_digits == 3) && isdigit(word[-2]))
		{
			prev_thousands = 1;
		}
	}

	if((word[0] == '0') && (prev_thousands == 0) && (word[1] != ' ') && (word[1] != tr->langopts.decimal_sep))
	{
		if((n_digits == 2) && (word[3] == ':') && isdigit(word[5]) && isspace(word[7]))
		{
			// looks like a time 02:30, omit the leading zero
		}
		else
		{
			return(0);     // number string with leading zero, speak as individual digits
		}
	}

	if((tr->langopts.numbers & 0x1000) && (word[n_digits] == ' '))
		thousands_inc = 1;
	else
	if(word[n_digits] == tr->langopts.thousands_sep)
		thousands_inc = 2;

	if(thousands_inc > 0)
	{
		// if the following "words" are three-digit groups, count them and add
		// a "thousand"/"million" suffix to this one

		ix = n_digits + thousands_inc;
		while(isdigit(word[ix]) && isdigit(word[ix+1]) && isdigit(word[ix+2]))
		{
			thousandplex++;
			if(word[ix+3] == tr->langopts.thousands_sep)
				ix += (3 + thousands_inc);
			else
				break;
		}
	}

	if((value == 0) && prev_thousands)
	{
		suppress_null = 1;
	}

	if((word[n_digits] == tr->langopts.decimal_sep) && isdigit(word[n_digits+1]))
	{
		// this "word" ends with a decimal point
		Lookup(tr, "_dpt", ph_append);
		decimal_point = 1;
	}
	else
	if(suppress_null == 0)
	{
		if(thousands_inc > 0)
		{
			if((thousandplex > 0) && (value < 1000))
			{
				if((suppress_null == 0) && (LookupThousands(tr,value,thousandplex,ph_append)))
				{
					// found an exact match for N thousand
					value = 0;
					suppress_null = 1;
				}
			}
		}
	}
	else
	if((thousandplex > 1) && prev_thousands && (prev_value > 0))
	{
		sprintf(string,"_%s%d",M_Variant(value),thousandplex+1);
		if(Lookup(tr, string, buf1)==0)
		{
			// speak this thousandplex if there was no word for the previous thousandplex
			sprintf(string,"_0M%d",thousandplex);
			Lookup(tr, string, ph_append);
		}
	}

	if((ph_append[0] == 0) && (word[n_digits] == '.') && (thousandplex == 0))
	{
		Lookup(tr, "_.", ph_append);
	}

	LookupNum3(tr, value, ph_buf, suppress_null, thousandplex, prev_thousands | ordinal);
	if((thousandplex > 0) && (tr->langopts.numbers2 & 0x200))
		sprintf(ph_out,"%s%s%s",ph_append,ph_buf2,ph_buf);  // say "thousands" before its number
	else
		sprintf(ph_out,"%s%s%s",ph_buf2,ph_buf,ph_append);


	while(decimal_point)
	{
		n_digits++;

		decimal_count = 0;
		while(isdigit(word[n_digits+decimal_count]))
			decimal_count++;

		if(decimal_count > 1)
		{
			max_decimal_count = 2;
			switch(decimal_mode = (tr->langopts.numbers & 0xe000))
			{
			case 0x8000:
				max_decimal_count = 5;
			case 0x4000:
				// French/Polish decimal fraction
				while(word[n_digits] == '0')
				{
					Lookup(tr, "_0", buf1);
					strcat(ph_out,buf1);
					decimal_count--;
					n_digits++;
				}
				if((decimal_count <= max_decimal_count) && isdigit(word[n_digits]))
				{
					LookupNum3(tr, atoi(&word[n_digits]), buf1, 0,0,0);
					strcat(ph_out,buf1);
					n_digits += decimal_count;
				}
				break;

			case 0x2000:
			case 0xa000:
				// Italian decimal fractions
				if(decimal_count <= 4)
				{
					LookupNum3(tr, atoi(&word[n_digits]), ph_buf, 0,0,0);
					if((word[n_digits]=='0') || (decimal_mode == 0xa000))
					{
						// decimal part has leading zeros, so add a "hundredths" or "thousandths" suffix
						sprintf(string,"_0Z%d",decimal_count);
						if(Lookup(tr, string, buf1) == 0)
							break;   // revert to speaking single digits

						strcat(ph_buf,buf1);
					}
					strcat(ph_out,ph_buf);
					n_digits += decimal_count;
				}
				break;

			case 0x6000:
				// Romanian decimal fractions
				if((decimal_count <= 4) && (word[n_digits] != '0'))
				{
						LookupNum3(tr, atoi(&word[n_digits]), buf1, 0,0,0);
						strcat(ph_out,buf1);
						n_digits += decimal_count;
				}
				break;
			}
		}

		while(isdigit(c = word[n_digits]) && (strlen(ph_out) < (N_WORD_PHONEMES - 10)))
		{
			value = word[n_digits++] - '0';
			LookupNum2(tr, value, 1, buf1);
			strcat(ph_out,buf1);
		}

		// something after the decimal part ?
		if(Lookup(tr, "_dpt2", buf1))
			strcat(ph_out,buf1);

		if((c == tr->langopts.decimal_sep) && isdigit(word[n_digits+1]))
		{
			Lookup(tr, "_dpt", buf1);
			strcat(ph_out,buf1);
		}
		else
		{
			decimal_point = 0;
		}
	}
	if((ph_out[0] != 0) && (ph_out[0] != phonSWITCH))
	{
		int next_char;
		char *p;
		p = &word[n_digits+1];

		p += utf8_in(&next_char,p);
		if((tr->langopts.numbers & NUM_NOPAUSE) && (next_char == ' '))
			utf8_in(&next_char,p);

		if(!iswalpha(next_char))
			strcat(ph_out,str_pause);  // don't add pause for 100s,  6th, etc.
	}

	*flags |= FLAG_FOUND;
	prev_value = this_value;
	return(1);
}  // end of TranslateNumber_1



int TranslateNumber(Translator *tr, char *word1, char *ph_out, unsigned int *flags, int wflags)
{//============================================================================================
	if(option_sayas == SAYAS_DIGITS1)
		return(0);  // speak digits individually

	if((tr->langopts.numbers & 0x3) == 1)
		return(TranslateNumber_1(tr, word1, ph_out, flags, wflags));

	return(0);
}  // end of TranslateNumber

