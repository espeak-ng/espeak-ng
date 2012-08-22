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

#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"
#include "tr_languages.h"


Translator_English::Translator_English() : Translator()
{//===================================
//	static int stress_lengths2[8] = {182,140, 220,220, 220,240, 248,270};
	static int stress_lengths2[8] = {182,140, 220,220, 0,0, 248,275};

	memcpy(stress_lengths,stress_lengths2,sizeof(stress_lengths));
	langopts.vowel_pause = 0;
	langopts.stress_rule = 0;
	langopts.word_gap = 0;
}



static unsigned char initials_bitmap[86] = {
 0x00, 0x00, 0x00, 0x00, 0x22, 0x08, 0x00, 0x88,  //  0
 0x20, 0x24, 0x20, 0x80, 0x10, 0x00, 0x00, 0x00,
 0x00, 0x28, 0x08, 0x00, 0x88, 0x22, 0x04, 0x00,  // 16
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x88, 0x22, 0x04, 0x00, 0x02, 0x00, 0x00,  // 32
 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x28, 0x8a, 0x03, 0x00, 0x00, 0x40, 0x00,  // 48
 0x02, 0x00, 0x41, 0xca, 0x9b, 0x06, 0x20, 0x80,
 0x91, 0x00, 0x00, 0x00, 0x00, 0x20, 0x08, 0x00,  // 64
 0x08, 0x20, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x22, 0x00, 0x01, 0x00, };



int Translator_English::Unpronouncable(char *word)
{//===============================================
/* Determines whether a word in 'unpronouncable', i.e. whether it should
	be spoken as individual letters.

	This function is language specific.
*/

	unsigned char  c;
	int  vowel_posn=9;
	int  index;
	int  ix;
	int  apostrophe=0;

	// words which we pass through to the dictionary, even though they look unpronouncable
	static const char *exceptions[] = {
		"'s ", "st ","nd ","rd ","th ",NULL };

	if((*word == ' ') || (*word == 0))
		return(0);

	for(ix=0; exceptions[ix] != NULL; ix++)
	{
		// Seemingly uncpronouncable words, but to be looked in the dictionary rules instead
		if(memcmp(word,exceptions[ix],3)==0)
			return(0);
	}

	index=0;
	while(((c = word[index++]) != 0) && !isspace(c))
	{
		if(IsVowel(c) || (c == 'y'))
		{
			vowel_posn = index;
			break;
		}

		if(c == '\'')
			apostrophe = 1;
		else
		if((c < 'a') || (c > 'z'))
			return(0);        // letter (not vowel) outside a-z range or apostrophe, abort test
	}
	if((vowel_posn > 5) || ((word[0]!='s') && (vowel_posn > 4)))
		return(1);  // no vowel, or no vowel in first four letters

	/* there is at least one vowel, is the initial letter combination valid ? */

	if(vowel_posn < 3)
		return(0);   /* vowel in first two letters, OK */

	if(apostrophe)
		return(0);   // first two letters not a-z, abort test

	index = (word[0]-'a') * 26 + (word[1]-'a');
	if(initials_bitmap[index >> 3] & (1L << (index & 7)))
		return(0);
	else
		return(1);   /****/
}   /* end of Unpronounceable */







//============================================================================================
// Experimental, for tone language
//============================================================================================





Translator_Tone::Translator_Tone() : Translator()
{//===================================
	static int stress_lengths2[8] = {248,248, 248,248, 248,248, 248,250};

	memcpy(stress_lengths,stress_lengths2,sizeof(stress_lengths));
	langopts.stress_rule = 0;
	langopts.vowel_pause = 1;
	langopts.word_gap = 3;
}



void Translator_Tone::CalcPitches(int clause_tone)
{//==========================================
//  clause_tone: 0=. 1=, 2=?, 3=! 4=none
	PHONEME_LIST *p;
	int  ix;
	int  count_stressed=0;
	int  count_stressed2=0;

	int  tone_ph;

	int  pitch_adjust = 13;     // pitch gradient through the clause - inital value
	int  pitch_decrement = 3;   //   decrease by this for each stressed syllable
	int  pitch_low =  0;         //   until it drops to this
	int  pitch_high = 10;       //   then reset to this

	p = &phoneme_list[0];

	// count number of stressed syllables
	p = &phoneme_list[0];
	for(ix=0; ix<n_phoneme_list; ix++, p++)
	{
		if((p->type == phVOWEL) && (p->tone >= 4))
		{
			count_stressed++;
		}
	}

	p = &phoneme_list[0];
	for(ix=0; ix<n_phoneme_list; ix++, p++)
	{
		if(p->type == phVOWEL)
		{
			tone_ph = p->tone_ph;

			if(p->tone >= 4)
			{
				// a stressed syllable
				count_stressed2++;
				if(count_stressed2 == count_stressed)
				{
					// the last stressed syllable
					pitch_adjust = pitch_low;
				}
				else
				{
					pitch_adjust -= pitch_decrement;
					if(pitch_adjust <= pitch_low)
						pitch_adjust = pitch_high;
				}

				if(tone_ph ==0)
				{
					tone_ph = phonDEFAULTTONE;  // no tone specified, use default tone 1
					p->tone_ph = tone_ph;
				}
				p->pitch1 = pitch_adjust + phoneme_tab[tone_ph]->start_type;
				p->pitch2 = pitch_adjust + phoneme_tab[tone_ph]->end_type;
			}
			else
			{
				// what to do for unstressed syllables ?
				p->pitch1 = 10;   // temporary
				p->pitch2 = 14;
			}
		}
	}
}  // end of Translator_Tone::CalcPitches


