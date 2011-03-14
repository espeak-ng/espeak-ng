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

static int n_digit_lookup;
static char *digit_lookup;
static int speak_missing_thousands;
static int number_control;


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



void LookupLetter(Translator *tr, unsigned int letter, int next_byte, char *ph_buf1, int control)
{//==============================================================================================
// control, bit 0:  not the first letter of a word

	int len;
	static char single_letter[10] = {0,0};
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
	single_letter[3+len] = next_byte;   // follow by space-space if the end of the word, or space-31

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

	strcpy(ph_buf1, ph_buf3);
	if((ph_buf1[0] == 0) || (ph_buf1[0] == phonSWITCH))
	{
		return;
	}

	dict_flags[0] = 0;
	dict_flags[1] = 0;
	SetWordStress(tr, ph_buf1, dict_flags, -1, control & 1);

}  // end of LookupLetter



int TranslateLetter(Translator *tr, char *word, char *phonemes, int control)
{//=========================================================================
// get pronunciation for an isolated letter
// return number of bytes used by the letter
// control bit 0:  a non-initial letter in a word
//         bit 1:  say 'capital'

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

	if(control & 2)
	{
		// include CAPITAL information
		if(iswupper(letter))
		{
			Lookup(tr, "_cap", capital);
		}
	}
	letter = towlower2(letter);

	LookupLetter(tr, letter, word[n_bytes], ph_buf, control & 1);

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
		LookupLetter(translator2, letter, word[n_bytes], ph_buf, control & 1);
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
				LookupLetter(tr, *p2, 0, pbuf, 1);
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
				c = phonPAUSE_NOLINK;  // pause following a primary stress
			else
				c = phonPAUSE_VSHORT;
//			else
//				continue;       // remove marker
		}
		*phonemes++ = c;
	}
	if(control >= 2)
		*phonemes++ = phonPAUSE_NOLINK;
	*phonemes = 0;
}  // end of SetSpellingStress



// Numbers

static char ph_ordinal2[12];


static int CheckDotOrdinal(Translator *tr, char *word, char *word_end, WORD_TAB *wtab, int roman)
{//==============================================================================================

	int ordinal = 0;
	int c2;
	int nextflags;

	if((tr->langopts.numbers & NUM_ORDINAL_DOT) && ((word_end[0] == '.') || (wtab[0].flags & FLAG_HAS_DOT)) && !(wtab[1].flags & FLAG_NOSPACE))
	{
		if(roman || !(wtab[1].flags & FLAG_FIRST_UPPER))
		{
			if(word_end[0] == '.')
				utf8_in(&c2, &word_end[2]);
			else
				utf8_in(&c2, &word_end[0]);

			if((word_end[0] != 0) && (word_end[1] != 0) && ((c2 == 0) || (wtab[0].flags & FLAG_COMMA_AFTER) || IsAlpha(c2)))
			{
				// ordinal number is indicated by dot after the number
				// but not if the next word starts with an upper-case letter
            // (c2 == 0) is for cases such as, "2.,"
				ordinal = 2;
				if(word_end[0] == '.')
					word_end[0] = ' ';

				if((roman==0) && (tr->translator_name == L('h','u')))
				{
					// lang=hu don't treat dot as ordinal indicator if the next word is a month name ($alt). It may have a suffix.
					nextflags = 0;
					if(IsAlpha(c2))
					{
						nextflags = TranslateWord(tr, &word_end[2], 0, NULL);
					}

if((tr->prev_dict_flags & FLAG_ALT_TRANS) && ((c2 == 0) || (wtab[0].flags & FLAG_COMMA_AFTER) || iswdigit(c2)))
	ordinal = 0;   // TEST  09.02.10

					if(nextflags & FLAG_ALT_TRANS)
						ordinal = 0;

					if(nextflags & FLAG_ALT3_TRANS)
					{
						if(word[-2] == '-')
							ordinal = 0;   // eg. december 2-5. között

						if(tr->prev_dict_flags & (FLAG_ALT_TRANS | FLAG_ALT3_TRANS))
							ordinal = 0x22;
					}
				}
			}
		}
	}
	return(ordinal);
}  // end of CheckDotOrdinal


static int hu_number_e(const char *word, int thousandplex, int value)
{//==================================================================
// lang-hu: variant form of numbers when followed by hyphen and a suffix starting with 'a' or 'e' (but not a, e, az, ez, azt, ezt, att. ett

	if((word[0] == 'a') || (word[0] == 'e'))
	{
		if((word[1] == ' ') || (word[1] == 'z') || ((word[1] == 't') && (word[2] == 't')))
			return(0);

		if(((thousandplex==1) || ((value % 1000) == 0)) && (word[1] == 'l'))
			return(0);   // 1000-el

		return(1);
	}
	return(0);
}  // end of hu_numnber_e



int TranslateRoman(Translator *tr, char *word, char *ph_out, WORD_TAB *wtab)
{//=========================================================================
	int c;
	char *p;
	const char *p2;
	int acc;
	int prev;
	int value;
	int subtract;
	int repeat = 0;
	int n_digits = 0;
	char *word_start;
	int num_control = 0;
	unsigned int flags[2];
	char ph_roman[30];
	char number_chars[N_WORD_BYTES];

	static const char *roman_numbers = "ixcmvld";
	static int roman_values[] = {1,10,100,1000,5,50,500};
 
	acc = 0;
	prev = 0;
	subtract = 0x7fff;
	ph_out[0] = 0;
	flags[0] = 0;
	flags[1] = 0;

	if(((tr->langopts.numbers & NUM_ROMAN_CAPITALS) && !(wtab[0].flags & FLAG_ALL_UPPER)) || isdigit(word[-2]))
		return(0);    // not '2xx'

	word_start = word;
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
		n_digits++;
	}

	if(isdigit(word[0]))
		return(0);      // eg. 'xx2'
 
	acc += prev;
	if(acc < tr->langopts.min_roman)
		return(0);

	if(acc > tr->langopts.max_roman)
		return(0);


	Lookup(tr, "_roman",ph_roman);   // precede by "roman" if _rom is defined in *_list
	p = &ph_out[0];

	if((tr->langopts.numbers & NUM_ROMAN_AFTER) == 0)
	{
		strcpy(ph_out,ph_roman);
		p = &ph_out[strlen(ph_roman)];
	}

	sprintf(number_chars,"  %d    ",acc);

	if(word[0] == '.')
	{
		// dot has not been removed.  This implies that there was no space after it
		return(0);
	}

	if(CheckDotOrdinal(tr, word_start, word, wtab, 1))
		wtab[0].flags |= FLAG_ORDINAL;

	if(tr->langopts.numbers & NUM_ROMAN_ORDINAL)
	{
		if(tr->translator_name == L('h','u'))
		{
			if(!(wtab[0].flags & FLAG_ORDINAL))
			{
				if((wtab[0].flags & FLAG_HYPHEN_AFTER) && hu_number_e(word, 0, acc))
				{
					// should use the 'e' form of the number
					num_control |= 1;
				}
				else
					return(0);
			}
		}
		else
		{
			wtab[0].flags |= FLAG_ORDINAL;
		}
	}

	tr->prev_dict_flags = 0;
	TranslateNumber(tr, &number_chars[2], p, flags, wtab, num_control);

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


static int LookupThousands(Translator *tr, int value, int thousandplex, int thousands_exact, char *ph_out)
{//=======================================================================================================
// thousands_exact:  bit 0  no hundreds,tens,or units,  bit 1  ordinal numberr
	int found;
	int found_value=0;
	char string[12];
	char ph_of[12];
	char ph_thousands[40];
	char ph_buf[40];

	ph_of[0] = 0;

	// first look for a match with the exact value of thousands
	if(value > 0)
	{
		if(thousands_exact & 1)
		{
			if(thousands_exact & 2)
			{
				// ordinal number
				sprintf(string,"_%dM%do",value,thousandplex);
				found_value = Lookup(tr, string, ph_thousands);
			}
			if(!found_value & (number_control & 1))
			{
				// look for the 'e' variant
				sprintf(string,"_%dM%de",value,thousandplex);
				found_value = Lookup(tr, string, ph_thousands);
			}
			if(!found_value)
			{
				// is there a different pronunciation if there are no hundreds,tens,or units ? (LANG=ta)
				sprintf(string,"_%dM%dx",value,thousandplex);
				found_value = Lookup(tr, string, ph_thousands);
			}
		}
		if(found_value == 0)
		{
			sprintf(string,"_%dM%d",value,thousandplex);
			found_value = Lookup(tr, string, ph_thousands);
		}
	}

	if(found_value == 0)
	{
		if((value % 100) >= 20) 
		{
			Lookup(tr, "_0of", ph_of);
		}

		found = 0;
		if(thousands_exact & 1)
		{
			if(thousands_exact & 2)
			{
				// ordinal number
				sprintf(string,"_%s%do",M_Variant(value), thousandplex);
				found = Lookup(tr, string, ph_thousands);
			}
			if(!found && (number_control & 1))
			{
				// look for the 'e' variant
				sprintf(string,"_%s%de",M_Variant(value), thousandplex);
				found = Lookup(tr, string, ph_thousands);
			}
			if(!found)
			{
				// is there a different pronunciation if there are no hundreds,tens,or units ?
				sprintf(string,"_%s%dx",M_Variant(value), thousandplex);
				found = Lookup(tr, string, ph_thousands);
			}
		}
		if(found == 0)
		{
			sprintf(string,"_%s%d",M_Variant(value), thousandplex);

			if(Lookup(tr, string, ph_thousands) == 0)
			{
				if(thousandplex > 3)
				{
					sprintf(string,"_0M%d", thousandplex-1);
					if(Lookup(tr, string, ph_buf) == 0)
					{
						// say "millions" if this name is not available and neither is the next lower
						Lookup(tr, "_0M2", ph_thousands);
						speak_missing_thousands = 3;
					}
				}
				if(ph_thousands[0] == 0)
				{
					// repeat "thousand" if higher order names are not available
					sprintf(string,"_%dM1",value);
					if((found_value = Lookup(tr, string, ph_thousands)) == 0)
						Lookup(tr, "_0M1", ph_thousands);
					speak_missing_thousands = 2;
				}
			}
		}
	}
	sprintf(ph_out,"%s%s",ph_of,ph_thousands);

	if((value == 1) && (thousandplex == 1) && (tr->langopts.numbers & NUM_OMIT_1_THOUSAND))
		return(1);

	return(found_value);
}  // end f LookupThousands


static int LookupNum2(Translator *tr, int value, int control, char *ph_out)
{//========================================================================
// Lookup a 2 digit number
// control bit 0: ordinal number
// control bit 1: final tens and units (not number of thousands) (use special form of '1', LANG=de "eins")
// control bit 2: tens and units only, no higher digits
// control bit 3: use feminine form of '2' (for thousands
// control bit 4: speak zero tens
// control bit 5: variant of ordinal number (lang=hu)

	int found;
	int ix;
	int units;
	int tens;
	int used_and=0;
	int found_ordinal = 0;
	int next_phtype;
	int ord_type = 'o';
	char string[12];  // for looking up entries in *_list
	char ph_ordinal[20];
	char ph_tens[50];
	char ph_digits[50];
	char ph_and[12];

	units = value % 10;
	tens = value / 10;

	found = 0;
	ph_ordinal[0] = 0;
	ph_tens[0] = 0;
	ph_digits[0] = 0;
	ph_and[0] = 0;

	if(control & 0x20)
	{
		ord_type = 'q';
	}

	if((control & 2) && (n_digit_lookup == 2))
	{
		// pronunciation of the final 2 digits has already been found
		strcpy(ph_out, digit_lookup);
	}
	else
	{
		if(digit_lookup[0] == 0)
		{
			// is there a special pronunciation for this 2-digit number
			if(control & 8)
			{
				sprintf(string,"_%df",value);
				found = Lookup(tr, string, ph_digits);
			}
			else
			if(control & 1)
			{
				strcpy(ph_ordinal, ph_ordinal2);
		
				if(control & 4)
				{
					sprintf(string,"_%d%cx",value,ord_type);  // LANG=hu, special word for 1. 2. when there are no higher digits
					found = Lookup(tr, string, ph_digits);
				}
				if(found == 0)
				{
					sprintf(string,"_%d%c",value,ord_type);
					found = Lookup(tr, string, ph_digits);
				}
				found_ordinal = found;
			}
	
			if(found == 0)
			{
				if(control & 2)
				{
					// the final tens and units of a number
					if(number_control & 1)
					{
						// look for 'e' variant
						sprintf(string,"_%de",value);
						found = Lookup(tr, string, ph_digits);
					}
				}
				else
				{
					// followed by hundreds or thousands etc
					sprintf(string,"_%da",value);
					found = Lookup(tr, string, ph_digits);
				}

				if(!found)
				{
					sprintf(string,"_%d",value);
					found = Lookup(tr, string, ph_digits);
				}
			}
		}
	
		// no, speak as tens+units
	
		if((control & 0x10) && (value < 10))
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
	
				if((control & 1) && 
					((units == 0) || (tr->langopts.numbers & NUM_SWAP_TENS) || (tr->langopts.numbers2 & NUM2_MULTIPLE_ORDINAL)))
				{
					sprintf(string,"_%dX%c", tens, ord_type);
					if(Lookup(tr, string, ph_tens) != 0)
					{
						found_ordinal = 1;

						if((units != 0) && (tr->langopts.numbers2 & NUM2_MULTIPLE_ORDINAL))
						{
							// Use the ordinal form of tens as well as units. Add the ordinal ending
							strcat(ph_tens, ph_ordinal2);
						}
					}
				}
				if(found_ordinal == 0)
				{
					sprintf(string,"_%dX", tens);
					Lookup(tr, string, ph_tens);
				}
	
				if((ph_tens[0] == 0) && (tr->langopts.numbers & NUM_VIGESIMAL))
				{
					// tens not found,  (for example) 73 is 60+13
					units = (value % 20);
					sprintf(string,"_%dX", tens & 0xfe);
					Lookup(tr, string, ph_tens);
				}
	
				ph_digits[0] = 0;
				if(units > 0)
				{	
					found = 0;
	
					if((control & 2) && (digit_lookup[0] != 0))
					{
						// we have an entry for this digit (possibly together with the next word)
						strcpy(ph_digits, digit_lookup);
						found_ordinal = 1;
						ph_ordinal[0] = 0;
					}
					else
					{
						if(control & 8)
						{
							// is there a variant form of this number?
							sprintf(string,"_%df",units);
							found = Lookup(tr, string, ph_digits);
						}
						if((control & 1) && ((tr->langopts.numbers & NUM_SWAP_TENS) == 0))
						{
							// ordinal
							sprintf(string,"_%d%c",units,ord_type);
							if((found = Lookup(tr, string, ph_digits)) != 0)
							{
								found_ordinal = 1;
							}
						}
						if(found == 0)
						{
							if((number_control & 1) && (control & 2))
							{
								// look for 'e' variant
								sprintf(string,"_%de",units);
								found = Lookup(tr, string, ph_digits);
							}
							else
							if(((control & 2) == 0) || ((tr->langopts.numbers & NUM_SWAP_TENS) != 0))
							{
								// followed by hundreds or thousands (or tens)
								sprintf(string,"_%da",units);
								found = Lookup(tr, string, ph_digits);
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
		}
	
		if((control & 1) && (found_ordinal == 0) && (ph_ordinal[0] == 0))
		{
			if((value >= 20) && (((value % 10) == 0) || (tr->langopts.numbers & NUM_SWAP_TENS)))
				Lookup(tr, "_ord20", ph_ordinal);
			if(ph_ordinal[0] == 0)
				Lookup(tr, "_ord", ph_ordinal);
		}
	
		if((tr->langopts.numbers & (NUM_SWAP_TENS | NUM_AND_UNITS)) && (ph_tens[0] != 0) && (ph_digits[0] != 0))
		{
			Lookup(tr, "_0and", ph_and);

			if(tr->langopts.numbers2 & NUM2_MULTIPLE_ORDINAL)
				ph_and[0] = 0;

			if(tr->langopts.numbers & NUM_SWAP_TENS)
				sprintf(ph_out,"%s%s%s%s",ph_digits, ph_and, ph_tens, ph_ordinal);
			else
				sprintf(ph_out,"%s%s%s%s",ph_tens, ph_and, ph_digits, ph_ordinal);
			used_and = 1;
		}
		else
		{
			if(tr->langopts.numbers & NUM_SINGLE_VOWEL)
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
	}

	if(tr->langopts.numbers & NUM_SINGLE_STRESS)
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
//           bit 5   variant form of ordinal number
	int found;
	int hundreds;
	int tensunits;
	int x;
	int ix;
	int exact;
	int ordinal;
	int say_zero_hundred=0;
	char string[12];  // for looking up entries in **_list
	char buf1[100];
	char buf2[100];
	char ph_100[20];
	char ph_10T[20];
	char ph_digits[50];
	char ph_thousands[50];
	char ph_hundred_and[12];
	char ph_thousand_and[12];

	ordinal = control & 0x22;
	hundreds = value / 100;
	tensunits = value % 100;
	buf1[0] = 0;

	ph_thousands[0] = 0;
	ph_thousand_and[0] = 0;

	if((tr->langopts.numbers & NUM_ZERO_HUNDRED) && ((control & 1) || (hundreds >= 10)))
	{
		say_zero_hundred = 1;  // lang=vi
	}

	if((hundreds > 0) || say_zero_hundred)
	{
		found = 0;
		if(ordinal && (tensunits == 0))
		{
			// ordinal number, with no tens or units
			found = Lookup(tr, "_0Co", ph_100);
		}
		if(found == 0)
		{
			if(tensunits==0)
			{
				// special form for exact hundreds?
				found = Lookup(tr, "_0C0", ph_100);
			}
			if(!found)
			{
				Lookup(tr, "_0C", ph_100);
			}
		}

		if(((tr->langopts.numbers & NUM_1900) != 0) && (hundreds == 19))
		{
			// speak numbers such as 1984 as years: nineteen-eighty-four
//			ph_100[0] = 0;   // don't say "hundred", we also need to surpess "and"
		}
		else
		if(hundreds >= 10)
		{
			ph_digits[0] = 0;

			exact = 0;
			if ((value % 1000) == 0)
				exact = 1;

			if(LookupThousands(tr, hundreds / 10, thousandplex+1, exact | ordinal, ph_10T) == 0)
			{
				x = 0;
				if(tr->langopts.numbers2 & (1 << (thousandplex+1)))
					x = 8;   // use variant (feminine) for before thousands and millions
				LookupNum2(tr, hundreds/10, x, ph_digits);
			}

			if(tr->langopts.numbers2 & 0x200)
				sprintf(ph_thousands,"%s%s",ph_10T,ph_digits);  // say "thousands" before its number, not after
			else
				sprintf(ph_thousands,"%s%s",ph_digits,ph_10T);

			hundreds %= 10;
			if((hundreds == 0) && (say_zero_hundred == 0))
				ph_100[0] = 0;
			suppress_null = 1;
		}

		ph_digits[0] = 0;

		if((hundreds > 0) || say_zero_hundred)
		{
			if((tr->langopts.numbers & NUM_AND_HUNDRED) && ((control & 1) || (ph_thousands[0] != 0)))
			{
				Lookup(tr, "_0and", ph_thousand_and);
			}

			suppress_null = 1;

			found = 0;
			if((ordinal)
				&& ((tensunits == 0) || (tr->langopts.numbers2 & NUM2_MULTIPLE_ORDINAL)))
			{
				// ordinal number
				sprintf(string, "_%dCo", hundreds);
				found = Lookup(tr, string, ph_digits);

				if((tr->langopts.numbers2 & NUM2_MULTIPLE_ORDINAL) && (tensunits > 0))
				{
					// Use ordinal form of hundreds, as well as for tens and units
					// Add ordinal suffix to the hundreds
					strcat(ph_digits, ph_ordinal2);
				}
			}

			if((hundreds == 0) && say_zero_hundred)
			{
				Lookup(tr, "_0", ph_digits);
			}
			else
			{
				if((!found) && (tensunits == 0))
				{
				// is there a special pronunciation for exactly n00 ?
					sprintf(string,"_%dC0",hundreds);
					found = Lookup(tr, string, ph_digits);
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
					if((hundreds > 1) || ((tr->langopts.numbers & NUM_OMIT_1_HUNDRED) == 0))
					{
						LookupNum2(tr, hundreds, 0, ph_digits);
					}
				}
			}
		}

		sprintf(buf1,"%s%s%s%s",ph_thousands,ph_thousand_and,ph_digits,ph_100);
	}

	ph_hundred_and[0] = 0;
	if(tensunits > 0)
	{
		if((value > 100) || ((control & 1) && (thousandplex==0)))
		{
			if((tr->langopts.numbers & NUM_HUNDRED_AND) || ((tr->langopts.numbers & NUM_HUNDRED_AND_DIGIT) && (tensunits < 10)))
			{
				Lookup(tr, "_0and", ph_hundred_and);
			}
		}
		if((tr->langopts.numbers & NUM_THOUSAND_AND) && (hundreds == 0) && ((control & 1) || (ph_thousands[0] != 0)))
		{
			Lookup(tr, "_0and", ph_hundred_and);
		}
	}


	buf2[0] = 0;

	if((tensunits != 0) || (suppress_null == 0))
	{
		x = 0;
		if(thousandplex==0)
		{
			x = 2;   // allow "eins" for 1 rather than "ein"
			if(ordinal)
				x = 3;   // ordinal number
			if((value < 100) && !(control & 1))
				x |= 4;   // tens and units only, no higher digits
			if(ordinal & 0x20)
				x |= 0x20;  // variant form of ordinal number
		}
		else
		{
			if(tr->langopts.numbers2 & (1 << thousandplex))
				x = 8;   // use variant (feminine) for before thousands and millions
		}

		if(LookupNum2(tr, tensunits, x, buf2) != 0)
		{
			if(tr->langopts.numbers & NUM_SINGLE_AND)
				ph_hundred_and[0] = 0;  // don't put 'and' after 'hundred' if there's 'and' between tens and units
		}
	}
	else
	{
		if(ph_ordinal2[0] != 0)
		{
			ix = strlen(buf1);
			if((ix > 0) && (buf1[ix-1] == phonPAUSE_SHORT))
				buf1[ix-1] = 0;   // remove pause before addding ordinal suffix
			strcpy(buf2, ph_ordinal2);
		}
	}

	sprintf(ph_out,"%s%s%s",buf1,ph_hundred_and,buf2);

	return(0);
}  // end of LookupNum3


static int TranslateNumber_1(Translator *tr, char *word, char *ph_out, unsigned int *flags, WORD_TAB *wtab, int control)
{//=====================================================================================================================
//  Number translation with various options
// the "word" may be up to 4 digits
// "words" of 3 digits may be preceded by another number "word" for thousands or millions

	int n_digits;
	int value;
	int ix;
	int digix;
	unsigned char c;
	int suppress_null = 0;
	int decimal_point = 0;
	int thousandplex = 0;
	int thousands_exact = 1;
	int thousands_inc = 0;
	int prev_thousands = 0;
	int ordinal = 0;
	int dot_ordinal;
	int this_value;
	int decimal_count;
	int max_decimal_count;
	int decimal_mode;
	int hyphen;
	int suffix_ix;
	int skipwords = 0;
	char *p;
	char string[20];  // for looking up entries in **_list
	char buf1[100];
	char ph_append[50];
	char ph_buf[200];
	char ph_buf2[50];
	char ph_zeros[50];
	char suffix[20];
	char buf_digit_lookup[50];

	static const char str_pause[2] = {phonPAUSE_NOLINK,0};

	*flags = 0;
	n_digit_lookup = 0;
	buf_digit_lookup[0] = 0;
	digit_lookup = buf_digit_lookup;
	number_control = control;

	for(ix=0; isdigit(word[ix]); ix++) ;
	n_digits = ix;
	value = this_value = atoi(word);

	// is there a previous thousands part (as a previous "word") ?
	if((n_digits == 3) && (word[-2] == tr->langopts.thousands_sep) && isdigit(word[-3]))
	{
		prev_thousands = 1;
	}
	else
	if((tr->langopts.thousands_sep == ' ') || (tr->langopts.numbers & NUM_ALLOW_SPACE))
	{
		// thousands groups can be separated by spaces
		if((n_digits == 3) && !(wtab->flags & FLAG_MULTIPLE_SPACES) && isdigit(word[-2]))
		{
			prev_thousands = 1;
		}
	}
	if(prev_thousands == 0)
	{
		speak_missing_thousands = 0;
	}

	ph_ordinal2[0] = 0;
	ph_zeros[0] = 0;

	if(prev_thousands || (word[0] != '0'))
	{
		// don't check for ordinal if the number has a leading zero
		if((ordinal = CheckDotOrdinal(tr, word, &word[ix], wtab, 0)) != 0)
			dot_ordinal = 1;
	}

	if((word[ix] == '.') && !isdigit(word[ix+1]) && !isdigit(word[ix+2]) && !(wtab[1].flags & FLAG_NOSPACE))
	{
		// remove dot unless followed by another number
		word[ix] = 0;
	}

	if((ordinal == 0) || (tr->translator_name == L('h','u')))
	{
// NOTE lang=hu, allow both dot and ordinal suffix, eg. "december 21.-én"
		// look for an ordinal number suffix after the number
		ix++;
		hyphen = 0;
		p = suffix;
		if(wtab[0].flags & FLAG_HYPHEN_AFTER)
		{
			*p++ = '-';
			hyphen = 1;
			ix++;
		}
		while((word[ix] != 0) && (word[ix] != ' ') && (ix < (int)(sizeof(suffix)-1)))
		{
			*p++ = word[ix++];
		}
		*p = 0;

		if(suffix[0] != 0)
		{
			if((tr->langopts.ordinal_indicator != NULL) && (strcmp(suffix, tr->langopts.ordinal_indicator) == 0))
			{
				ordinal = 2;
			}
			else
			if(!isdigit(suffix[0]))   // not _#9 (tab)
			{
				sprintf(string,"_#%s",suffix);
				if(Lookup(tr, string, ph_ordinal2))
				{
					// this is an ordinal suffix
					ordinal = 2;
					flags[0] |= FLAG_SKIPWORDS;
					skipwords = 1; 
				}
			}
		}
	}

	if(wtab[0].flags & FLAG_ORDINAL)
		ordinal = 2;

	ph_append[0] = 0;
	ph_buf2[0] = 0;


	if((word[0] == '0') && (prev_thousands == 0) && (word[1] != ' ') && (word[1] != tr->langopts.decimal_sep))
	{
		if((n_digits == 2) && (word[3] == ':') && isdigit(word[5]) && isspace(word[7]))
		{
			// looks like a time 02:30, omit the leading zero
		}
		else
		{
			if(n_digits > 3)
			{
				flags[0] &= ~FLAG_SKIPWORDS;
				return(0);     // long number string with leading zero, speak as individual digits
			}

			// speak leading zeros
			for(ix=0; (word[ix] == '0') && (ix < (n_digits-1)); ix++)
			{
				Lookup(tr, "_0", &ph_zeros[strlen(ph_zeros)]);
			}
		}
	}

	if((tr->langopts.numbers & NUM_ALLOW_SPACE) && (word[n_digits] == ' '))
		thousands_inc = 1;
	else
	if(word[n_digits] == tr->langopts.thousands_sep)
		thousands_inc = 2;

	suffix_ix = n_digits+2;
	if(thousands_inc > 0)
	{
		// if the following "words" are three-digit groups, count them and add
		// a "thousand"/"million" suffix to this one
		digix = n_digits + thousands_inc;

		while(((wtab[thousandplex+1].flags & FLAG_MULTIPLE_SPACES) == 0) &&
			isdigit(word[digix]) && isdigit(word[digix+1]) && isdigit(word[digix+2]) && !isdigit(word[digix+3]) && !isdigit(word[digix-1]))
		{
			if((word[digix] != '0') || (word[digix+1] != '0') || (word[digix+2] != '0'))
				thousands_exact = 0;

			thousandplex++;
			digix += 3;
			if((word[digix] == tr->langopts.thousands_sep) || ((tr->langopts.numbers & NUM_ALLOW_SPACE) && (word[digix] == ' ')))
			{
				suffix_ix = digix+2;
				digix += thousands_inc;
			}
			else
				break;
		}
	}

	if((value == 0) && prev_thousands)
	{
		suppress_null = 1;
	}

	if(tr->translator_name == L('h','u'))
	{
		// variant form of numbers when followed by hyphen and a suffix starting with 'a' or 'e' (but not a, e, az, ez, azt, ezt
		if((wtab[thousandplex].flags & FLAG_HYPHEN_AFTER) && (thousands_exact==1) && hu_number_e(&word[suffix_ix], thousandplex, value))
		{
			number_control |= 1;  // use _1e variant of number
		}
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
				if((suppress_null == 0) && (LookupThousands(tr,value,thousandplex, thousands_exact, ph_append)))
				{
					// found an exact match for N thousand
					value = 0;
					suppress_null = 1;
				}
			}
		}
	}
	else

	if(speak_missing_thousands == 1)
	{
		// speak this thousandplex if there was no word for the previous thousandplex
		sprintf(string,"_0M%d",thousandplex+1);
		if(Lookup(tr, string, buf1)==0)
		{
			sprintf(string,"_0M%d",thousandplex);
			Lookup(tr, string, ph_append);
		}
	}

	if((ph_append[0] == 0) && (word[n_digits] == '.') && (thousandplex == 0))
	{
		Lookup(tr, "_.", ph_append);
	}

	if(thousandplex == 0)
	{
		char *p2;
		// look for combinations of the number with the next word
		p = word;
		while(isdigit(p[1])) p++;  // just use the last digit
		if(isdigit(p[-1]))
		{
			p2 = p - 1;
			if(LookupDictList(tr, &p2, buf_digit_lookup, flags, FLAG_SUFX, wtab))  // lookup 2 digits
			{
				n_digit_lookup = 2;
			}
		}

//		if((buf_digit_lookup[0] == 0) && (*p != '0') && (dot_ordinal==0))
		if((buf_digit_lookup[0] == 0) && (*p != '0'))
		{
			// not found, lookup only the last digit (?? but not if dot-ordinal has been found)
			if(LookupDictList(tr, &p, buf_digit_lookup, flags, FLAG_SUFX, wtab))  // don't match '0', or entries with $only
			{
				n_digit_lookup = 1;
			}
		}
	}

	LookupNum3(tr, value, ph_buf, suppress_null, thousandplex, prev_thousands | ordinal);
	if((thousandplex > 0) && (tr->langopts.numbers2 & 0x200))
		sprintf(ph_out,"%s%s%s%s",ph_zeros,ph_append,ph_buf2,ph_buf);  // say "thousands" before its number
	else
		sprintf(ph_out,"%s%s%s%s",ph_zeros,ph_buf2,ph_buf,ph_append);


	while(decimal_point)
	{
		n_digits++;

		decimal_count = 0;
		while(isdigit(word[n_digits+decimal_count]))
			decimal_count++;

//		if(decimal_count > 1)
		{
			max_decimal_count = 2;
			switch(decimal_mode = (tr->langopts.numbers & 0xe000))
			{
			case NUM_DFRACTION_4:
				max_decimal_count = 5;
			case NUM_DFRACTION_2:
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

			case NUM_DFRACTION_1:   // italian, say "hundredths" if leading zero
			case NUM_DFRACTION_5:   // hungarian, always say "tenths" etc.
				LookupNum3(tr, atoi(&word[n_digits]), ph_buf, 0,0,0);
				if((word[n_digits]=='0') || (decimal_mode == NUM_DFRACTION_5))
				{
					// decimal part has leading zeros, so add a "hundredths" or "thousandths" suffix
					sprintf(string,"_0Z%d",decimal_count);
					if(Lookup(tr, string, buf1) == 0)
						break;   // revert to speaking single digits

					strcat(ph_buf,buf1);
				}
				strcat(ph_out,ph_buf);
				n_digits += decimal_count;
				break;

			case NUM_DFRACTION_3:
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
			// speak any remaining decimal fraction digits individually
			value = word[n_digits++] - '0';
			LookupNum2(tr, value, 2, buf1);
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

		if(!iswalpha(next_char) && !((wtab[thousandplex].flags & FLAG_HYPHEN_AFTER) && (thousands_exact != 0)))
			strcat(ph_out,str_pause);  // don't add pause for 100s,  6th, etc.
	}

	*flags |= FLAG_FOUND;
	speak_missing_thousands--;

	if(skipwords)
		dictionary_skipwords = skipwords;
	return(1);
}  // end of TranslateNumber_1



int TranslateNumber(Translator *tr, char *word1, char *ph_out, unsigned int *flags, WORD_TAB *wtab, int control)
{//=============================================================================================================
	if((option_sayas == SAYAS_DIGITS1) || (wtab[0].flags & FLAG_INDIVIDUAL_DIGITS))
		return(0);  // speak digits individually

	if(tr->langopts.numbers != 0)
		return(TranslateNumber_1(tr, word1, ph_out, flags, wtab, control));

	return(0);
}  // end of TranslateNumber

