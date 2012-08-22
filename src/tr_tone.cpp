/***************************************************************************
 *   Copyright (C) 2005 by Jonathan Duddington                             *
 *   jsd1@clara.co.uk                                                      *
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
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"



Translator_Tone::Translator_Tone() : Translator()
{//===================================
	static int stress_lengths2[8] = {248,248, 248,248, 248,248, 248,250};

	memcpy(stress_lengths,stress_lengths2,sizeof(stress_lengths));
	option_stress_rule = 0;
	option_vowel_pause = 1;
	option_words = 2;
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
				p->pitch1 = pitch_adjust + phoneme_tab[tone_ph].start_type;
				p->pitch2 = pitch_adjust + phoneme_tab[tone_ph].end_type;
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

 
