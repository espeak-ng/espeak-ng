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

#include <stdlib.h>
#include <stdio.h>

#include "speech.h"
#include "voice.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"


/* amplitudes for syllable stress levels */
//static int stress_amps[] = {13,13, 16,16, 16,20, 20,20 };
//static int stress_amps_r[] = {13,13, 15,15, 15,18, 18,20 };

int stress_amps[8];
int stress_amps_r[8];
int stress_lengths[8];

// Tables of the relative lengths of vowels, depending on the
// type of the two phonemes that follow

// indexes are the "length_mod" value for the following phonemes

	// use this table if vowel is not the last in the word
   static unsigned char length_mod_tab[9][9] = {
   /*  a   ,   t   s   n   d   z   r   N   <- next */
      100,150,100,105, 95,110,110,100, 95,  /* a  <- next2 */
      105,150,105,110,125,130,135,115,125,  /* , */
      105,150, 75,100, 75,115,120, 85, 75,  /* t */
      105,150, 85,105, 95,115,120,100, 95,  /* s */
      110,150, 95,105,100,115,120,100,100,  /* n */
      105,150,100,105, 95,115,120,110, 95,  /* d */
      105,150,100,105,105,122,125,110,105,  /* z */
      105,150,100,105,105,122,125,110,105,  /* r */
      105,150, 95,105,100,115,120,110,100 }; /* N */

   // as above, but for the last syllable in a word
   static unsigned char length_mod_tab0[9][9] = {
   /*  a   ,   t   s   n   d   z   r    N  <- next */
      100,150,100,105,110,115,110,110,110,  /* a  <- next2 */
      105,150,105,110,125,135,140,115,135,  /* , */
      105,150, 90,105, 90,122,135,100, 90,  /* t */
      105,150,100,105,100,122,135,100,100,  /* s */
      105,150,100,105,105,115,135,110,105,  /* n */
      105,150,100,105,105,122,130,120,125,  /* d */
      105,150,100,105,110,122,125,115,110,  /* z */
      105,150,100,105,105,122,135,120,105,  /* r */
      105,150,100,105,105,115,135,110,105 };  /* N */


int speed1 = 130;
int speed2 = 121;
int speed3 = 118;

extern int speed_factor1;
extern int speed_factor2;


void SetSpeed(int speed, int amp)
{//==============================
#define N_WPM   13

	// Interpolation table to translate from words-per-minute to internal speed
	// words-per-minute values (measured)
	static float wpm1[N_WPM] =
			{0, 99.0, 111.3, 127.0, 140.9, 160.0, 183.9, 217.3, 238.5, 263.2, 287.5, 304.4, 316 };
	// corresponding internal speed values
	static float wpm2[N_WPM] = 
			{0, 200,  170,   140,   120,   100,    80,    60,    50,    40,    30,    20,  10};

	static int amplitude_factor[] = {0,5,6,7,9,11,14,17,21,26, 32, 38,44,50,56,63,70,77,84,91,100 };

	int ix;
	float x;


	// convert from word-per-minute to internal speed code
	if(speed > 320)
		speed = 320;

	for(ix=2; ix<N_WPM-2; ix++)
	{
		if(speed < wpm1[ix])
			break;
	}
	x = polint(&wpm1[ix-1], &wpm2[ix-1], 3, speed);
	speed1 = int(x * voice->speedf1 + 0.5);
	speed2 = int(x * voice->speedf2 + 0.5);
	speed3 = int(x * voice->speedf3 + 0.5);


	speed_factor1 = (256 * speed1)/110;      // full speed adjustment
	speed_factor2 = 128 + (128*speed1)/130;  // reduced speed adjustment

	if((amp >= 0) && (amp <= 20))
	{
		x = amplitude_factor[amp] * 1.875; 
		option_amplitude = int(x);
	}
}	// end of SetSpeed



void Translator::CalcLengths()
{//===========================
	int ix;
	int ix2;
	PHONEME_LIST *prev;
	PHONEME_LIST *next;
	PHONEME_LIST *next2;
	PHONEME_LIST *next3;
	PHONEME_LIST *p;
	PHONEME_LIST *p2;

	int  stress;
	static int  more_syllables=0;
	int  pre_sonorant=0;
	int  pre_voiced=0;
	int  last_pitch = 0;
	int  pitch_start;
	int  length_mod;
	int  env2;
	int  end_of_clause;

	for(ix=1; ix<n_phoneme_list; ix++)
	{
		prev = &phoneme_list[ix-1];
		p = &phoneme_list[ix];
		next = &phoneme_list[ix+1];
			
		switch(p->type)
		{
		case phPAUSE:
			last_pitch = 0;
			break;
			
		case phSTOP:
			last_pitch = 0;
			if(prev->type == phSTOP || prev->type == phFRICATIVE)
				p->prepause = 20;
			else
			if((more_syllables > 0) || (p->tone < 4))
				p->prepause = 40;
			else
				p->prepause = 60;

			if((option_words==2) && (p->newword))
				p->prepause = 60;
			break;

		case phVFRICATIVE:
			if(next->type==phVOWEL)
			{
				pre_voiced = 1;
			}
		case phFRICATIVE:
			if(p->newword)
				p->prepause = 15;

			if(next->type==phPAUSE && prev->type==phNASAL && !p->flags&phFORTIS)
				p->prepause = 25;

			if((p->flags & phSIBILANT) && next->type==phSTOP && !next->newword && prev->type != phVOWEL)
				p->length = 150;
			else
				p->length = 256;

			if((option_words==2) && (p->newword))
				p->prepause = 30;
			break;

		case phVSTOP:
			if(prev->type==phVFRICATIVE || prev->type==phFRICATIVE || (prev->flags & phSIBILANT) || (prev->type == phLIQUID))
				p->prepause = 30;

			if(next->type==phVOWEL || next->type==phLIQUID)
			{
				if((next->type==phVOWEL) || !next->newword)
					pre_voiced = 1;

				p->prepause = 40;

				if((prev->type == phPAUSE) || (prev->type == phVOWEL))
					p->prepause = 0;
				else
				if(p->newword==0)
				{
					if(prev->type==phLIQUID)
						p->prepause = 20;
					if(prev->type==phNASAL)
						p->prepause = 12;

					if(prev->type==phSTOP && !(prev->flags & phFORTIS))
						p->prepause = 0;
				}
			}
			if((option_words==2) && (p->newword) && (p->prepause < 20))
				p->prepause = 20;
			break;

		case phLIQUID:
		case phNASAL:
			p->amp = stress_amps[1];  // unless changed later
			p->length = 256;  //  TEMPORARY
			
			if(p->newword)
			{
				if(prev->type==phLIQUID)
					p->prepause = 25;
				if(prev->type==phVOWEL)
					p->prepause = 12;
			}

			if(next->type==phVOWEL)
			{
				pre_sonorant = 1;
			}
			else
			if((prev->type==phVOWEL) || (prev->type == phLIQUID))
			{
				p->length = prev->length;
				p->pitch2 = last_pitch;
				p->pitch1 = p->pitch2-8;
				p->env = PITCHfall;
				pre_voiced = 0;
				
				if(p->type == phLIQUID)
				{
					p->length = speed1;
				}

				if(next->type == phVSTOP)
				{
					p->length = (p->length * 160)/100;
				}
				if(next->type == phVFRICATIVE)
				{
					p->length = (p->length * 120)/100;
				}
			}
			else
			{
				p->pitch2 = last_pitch;
				for(ix2=ix; ix2<n_phoneme_list; ix2++)
				{
					if(phoneme_list[ix2].type == phVOWEL)
					{
						p->pitch2 = phoneme_list[ix2].pitch2;
						break;
					}
				}
				p->pitch1 = p->pitch2-8;
				p->env = PITCHfall;
				pre_voiced = 0;
			}
			break;

		case phVOWEL:
			next2 = &phoneme_list[ix+2];
			next3 = &phoneme_list[ix+3];

			if((stress = p->tone) > 7) stress = 7;

			if(pre_sonorant)
				p->amp = stress_amps_r[stress];
			else
				p->amp = stress_amps[stress];

			// is the last syllable of a word ?
			more_syllables=0;
			end_of_clause = 0;
			for(p2 = p+1; p2->newword== 0; p2++)
			{
				if(p2->type == phVOWEL)
					more_syllables++;
			}
			if((p2->newword == 2) && (more_syllables==0))
			{
				end_of_clause = 2;
			}

			// calc length modifier
			if(more_syllables==0)
				length_mod = length_mod_tab0[next2->ph->length_mod][next->ph->length_mod];
			else
			{
				length_mod = length_mod_tab[next2->ph->length_mod][next->ph->length_mod];
				if((next->type == phNASAL) && (next2->type == phSTOP || next2->type == phVSTOP) && (next3->flags & phFORTIS))
					length_mod -= 15;
			}

			if(more_syllables==0)
				length_mod *= speed1;
			else
			if(more_syllables==1)
				length_mod *= speed2;
			else
				length_mod *= speed3;

			length_mod = length_mod / 128;
			if(length_mod < 28)
				length_mod = 28;

			if(stress >= 7)
			{
				// include a constant component so it doesn't decrease directly with speed
				length_mod += 20;
			}
			
			length_mod = (length_mod * stress_lengths[stress])/128;

			if(end_of_clause == 2)
			{
				// this is the last syllable in the clause, lengthen it - more for short vowels
				length_mod = length_mod * (256 + (280 - p->ph->std_length)/3)/256;
			}

			p->length = length_mod;
#ifdef LOG_WGEN
if(f_log != NULL)
{
	fprintf(f_log,"%2s LMOD %3d  stress %d  syllab %d\n",
		WordToString(p->ph->mnemonic),length_mod,stress,more_syllables);
}
#endif

			// pre-vocalic part
			// set last-pitch
			env2 = p->env;
			if(env2 > 1) env2++;   // version for use with preceding semi-vowel
			pitch_start = p->pitch1 + ((p->pitch2-p->pitch1)*envelope_data[env2][0])/256;

			if(pre_sonorant || pre_voiced)
			{
				// set pitch for pre-vocalic part
				if(pitch_start - last_pitch > 9)
					last_pitch = pitch_start - 9;
				prev->pitch1 = last_pitch;
				prev->pitch2 = pitch_start;
				if(last_pitch < pitch_start)
				{
					prev->env = PITCHrise;
					p->env = env2;
				}
				else
				{
					prev->env = PITCHfall;
				}

				prev->length = length_mod;
				prev->amp = p->amp;
				if((prev->type != phLIQUID) && (prev->amp > 18))
					prev->amp = 18;
			}

			// vowel & post-vocalic part
			next->sflags &= ~SFLAG_SEQCONTINUE;
			if(next->type == phNASAL && next2->type != phVOWEL)
				next->sflags |= SFLAG_SEQCONTINUE;
				
			if(next->type == phLIQUID)
			{
//				if(next2->prepause == 0)    // what was this for?  caused discon. for [er]
					next->sflags |= SFLAG_SEQCONTINUE;
					
				if(next2->type == phVOWEL)
				{
					next->sflags &= ~SFLAG_SEQCONTINUE;
				}

				if(next2->type != phVOWEL)
				{
					if(next->ph->mnemonic == 'R')
					{
						ix2 = p->pitch2 - p->pitch1;
						if(ix2 < 15)
						{
							p->pitch1 = p->pitch2 - 15;
						}
					}
				}
			}

			last_pitch = p->pitch1 + ((p->pitch2-p->pitch1)*envelope_data[p->env][127])/256;
			pre_sonorant = 0;
			pre_voiced = 0;
			break;
		}
	}
}  //  end of CalcLengths

