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

#include <stdlib.h>
#include <stdio.h>
#include <wctype.h>

#include "speak_lib.h"
#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "translate.h"

extern int GetAmplitude(void);


// convert from words-per-minute to internal speed factor
static unsigned char speed_lookup[290] = {
	 250, 246, 243, 239, 236,	//  80
	 233, 229, 226, 223, 220,	//  85
	 217, 214, 211, 208, 205,	//  90
	 202, 197, 194, 192, 190,	//  95
	 187, 185, 183, 180, 178,	// 100
	 176, 174, 172, 170, 168,	// 105
	 166, 164, 161, 159, 158,	// 110
	 156, 154, 152, 150, 148,	// 115
	 146, 145, 143, 141, 137,	// 120
	 136, 135, 133, 132, 131,	// 125
	 129, 128, 127, 126, 125,	// 130
	 124, 122, 121, 120, 119,	// 135
	 117, 116, 115, 114, 113,	// 140
	 112, 111, 110, 108, 107,	// 145
	 106, 105, 104, 103, 102,	// 150
	 101, 100,  99,  98,  97,	// 155
	  96,  95,  93,  92,  92,	// 160
	  91,  90,  89,  89,  88,	// 165
	  87,  87,  86,  85,  85,	// 170
	  84,  83,  83,  82,  81,	// 175
	  80,  80,  79,  78,  78,	// 180
	  77,  76,  76,  75,  73,	// 185
	  72,  72,  71,  71,  70,	// 190
	  70,  69,  69,  68,  67,	// 195
	  67,  66,  66,  65,  65,	// 200
	  64,  64,  63,  63,  62,	// 205
	  62,  61,  60,  60,  59,	// 210
	  59,  58,  58,  57,  57,	// 215
	  56,  56,  55,  55,  55,	// 220
	  54,  54,  53,  53,  52,	// 225
	  52,  51,  51,  50,  50,	// 230
	  49,  49,  49,  48,  48,	// 235
	  47,  47,  46,  46,  46,	// 240
	  45,  45,  44,  44,  43,	// 245
	  43,  43,  42,  42,  41,	// 250
	  41,  41,  40,  40,  39,	// 255
	  39,  39,  38,  38,  38,	// 260
	  37,  37,  37,  36,  36,	// 265
	  35,  35,  35,  34,  34,	// 270
	  34,  33,  33,  33,  32,	// 275
	  32,  32,  32,  31,  31,	// 280
	  31,  30,  30,  30,  29,	// 285
	  29,  29,  29,  28,  28,	// 290
	  28,  28,  27,  27,  27,	// 295
	  26,  26,  26,  26,  25,	// 300
	  25,  25,  22,  22,  22,	// 305
	  22,  22,  22,  22,  22,	// 310
	  21,  21,  21,  21,  21,	// 315
	  21,  20,  20,  20,  20,	// 320
	  20,  15,  15,  15,  15,	// 325
	  15,  15,  15,  15,  16,	// 330
	  16,  16,  16,  15,  15,	// 335
	  15,  15,  15,  15,  15,	// 340
	  15,  17,  17,  16,  16,	// 345
	  15,  15,  14,  14,  13,	// 350
	  13,  12,  12,  11,  11,	// 355
	  10,  10,   9,   8,   8,	// 360
	   7,   6,   5,   5,   4,	// 365
};

// speed_factor2 adjustments for speeds 370 to 390
static unsigned char faster[] = {
114,112,110,109,107,105,104,102,100,98, // 370-379
96,94,92,90,88,85,83,80,78,75,72 }; //380-390

static int speed1 = 130;
static int speed2 = 121;
static int speed3 = 118;



void SetSpeed(int control)
{//=======================
	int x;
	int s1;
	int wpm;
	int wpm2;

	wpm = embedded_value[EMBED_S];
	if(control == 2)
		wpm = embedded_value[EMBED_S2];
	wpm2 = wpm;

	if(wpm > 369) wpm = 369;
	if(wpm < 80) wpm = 80;

	x = speed_lookup[wpm-80];

	if(control & 1)
	{
		// set speed factors for different syllable positions within a word
		// these are used in CalcLengths()
		speed1 = (x * voice->speedf1)/256;
		speed2 = (x * voice->speedf2)/256;
		speed3 = (x * voice->speedf3)/256;
	}

	if(control & 2)
	{
		// these are used in synthesis file
		s1 = (x * voice->speedf1)/256;
		speed.speed_factor1 = (256 * s1)/115;      // full speed adjustment, used for pause length
if(speed.speed_factor1 < 15)
	speed.speed_factor1 = 15;
		if(wpm >= 170)
//			speed_factor2 = 100 + (166*s1)/128;  // reduced speed adjustment, used for playing recorded sounds
			speed.speed_factor2 = 110 + (150*s1)/128;  // reduced speed adjustment, used for playing recorded sounds
		else
			speed.speed_factor2 = 128 + (128*s1)/130;  // = 215 at 170 wpm

		if(wpm2 > 369)
		{
			if(wpm2 > 390)
				wpm2 = 390;
			speed.speed_factor2 = faster[wpm2 - 370];
		}
	}

	speed.min_sample_len = 450;
	speed.speed_factor3 = 110;   // controls the effect of FRFLAG_LEN_MOD reduce length change

	if(wpm2 >= 370)
	{
		// TESTING
		// use experimental fast settings if they have been specified in the Voice
		if(speed.fast_settings[0] > 0)
			speed.speed_factor1 = speed.fast_settings[0];
		if(speed.fast_settings[1] > 0)
			speed.speed_factor2 = speed.fast_settings[1];
		if(speed.fast_settings[2] > 0)
			speed.speed_factor3 = speed.fast_settings[2];
	}
}  //  end of SetSpeed


#ifdef deleted
void SetAmplitude(int amp)
{//=======================
	static unsigned char amplitude_factor[] = {0,5,6,7,9,11,14,17,21,26, 32, 38,44,50,56,63,70,77,84,91,100 };

	if((amp >= 0) && (amp <= 20))
	{
		option_amplitude = (amplitude_factor[amp] * 480)/256; 
	}
}
#endif



void SetParameter(int parameter, int value, int relative)
{//======================================================
// parameter: reset-all, amp, pitch, speed, linelength, expression, capitals, number grouping
// relative 0=absolute  1=relative

	int new_value = value;
	int default_value;

	if(relative)
	{
		if(parameter < 5)
		{
			default_value = param_defaults[parameter];
			new_value = default_value + (default_value * value)/100;
		}
	}
	param_stack[0].parameter[parameter] = new_value;

	switch(parameter)
	{
	case espeakRATE:
		embedded_value[EMBED_S] = new_value;
		embedded_value[EMBED_S2] = new_value;
		SetSpeed(3);
		break;

	case espeakVOLUME:
		embedded_value[EMBED_A] = new_value;
		GetAmplitude();
		break;

	case espeakPITCH:
		if(new_value > 99) new_value = 99;
		if(new_value < 0) new_value = 0;
		embedded_value[EMBED_P] = new_value;
		break;

	case espeakRANGE:
		if(new_value > 99) new_value = 99;
		embedded_value[EMBED_R] = new_value;
		break;

	case espeakLINELENGTH:
		option_linelength = new_value;
		break;

	case espeakWORDGAP:
		option_wordgap = new_value;
		break;

	case espeakINTONATION:
		if((new_value & 0xff) != 0)
			translator->langopts.intonation_group = new_value & 0xff;
		option_tone_flags = new_value;
		break;

	default:
		break;
	}
}  // end of SetParameter



static void DoEmbedded2(int &embix)
{//================================
	// There were embedded commands in the text at this point

	unsigned int word;

	do {
		word = embedded_list[embix++];

		if((word & 0x1f) == EMBED_S)
		{
			// speed
			SetEmbedded(word & 0x7f, word >> 8);   // adjusts embedded_value[EMBED_S]
			SetSpeed(1);
		}
	} while((word & 0x80) == 0);
}


void CalcLengths(Translator *tr)
{//==============================
	int ix;
	int ix2;
	PHONEME_LIST *prev;
	PHONEME_LIST *next;
	PHONEME_LIST *next2;
	PHONEME_LIST *next3;
	PHONEME_LIST *p;
	PHONEME_LIST *p2;

	int  stress;
	int  type;
	static int  more_syllables=0;
	int  pre_sonorant=0;
	int  pre_voiced=0;
	int  last_pitch = 0;
	int  pitch_start;
	int  length_mod;
	int  len;
	int  env2;
	int  end_of_clause;
	int  embedded_ix = 0;
	int  min_drop;
	int emphasized;
	int  tone_mod;
	unsigned char *pitch_env=NULL;

	for(ix=1; ix<n_phoneme_list; ix++)
	{
		prev = &phoneme_list[ix-1];
		p = &phoneme_list[ix];
		stress = p->stresslevel & 0x7;
		emphasized = p->stresslevel & 0x8;

		next = &phoneme_list[ix+1];

		if(p->synthflags & SFLAG_EMBEDDED)
		{
			DoEmbedded2(embedded_ix);
		}

		type = p->type;
		if(p->synthflags & SFLAG_SYLLABLE)
			type = phVOWEL;

		switch(type)
		{
		case phPAUSE:
			last_pitch = 0;
			break;
			
		case phSTOP:
			last_pitch = 0;
			if(prev->type == phFRICATIVE)
				p->prepause = 20;
			else
			if((more_syllables > 0) || (stress < 4))
				p->prepause = 40;
			else
				p->prepause = 60;

			if(prev->type == phSTOP)
				p->prepause = 60;

			if((tr->langopts.word_gap & 0x10) && (p->newword))
				p->prepause = 60;

			if(p->ph->phflags & phLENGTHENSTOP)
				p->prepause += 30;

			if(p->synthflags & SFLAG_LENGTHEN)
				p->prepause += tr->langopts.long_stop;
			break;

		case phVFRICATIVE:
			if(next->type==phVOWEL)
			{
				pre_voiced = 1;
			}   // drop through
		case phFRICATIVE:
			if(p->newword)
				p->prepause = 15;

			if(next->type==phPAUSE && prev->type==phNASAL && !(p->ph->phflags&phFORTIS))
				p->prepause = 25;

			if(prev->ph->phflags & phBRKAFTER)
				p->prepause = 30;

			if((p->ph->phflags & phSIBILANT) && next->type==phSTOP && !next->newword)
			{
				if(prev->type == phVOWEL)
					p->length = 200;      // ?? should do this if it's from a prefix
				else
					p->length = 150;
			}
			else
				p->length = 256;

			if((tr->langopts.word_gap & 0x10) && (p->newword))
				p->prepause = 30;

			break;

		case phVSTOP:
			if(prev->type==phVFRICATIVE || prev->type==phFRICATIVE || (prev->ph->phflags & phSIBILANT) || (prev->type == phLIQUID))
				p->prepause = 30;

			if(next->type==phVOWEL || next->type==phLIQUID)
			{
				if((next->type==phVOWEL) || !next->newword)
					pre_voiced = 1;

				p->prepause = 40;

				if((prev->type == phPAUSE) || (prev->type == phVOWEL)) // || (prev->ph->mnemonic == ('/'*256+'r')))
					p->prepause = 0;
				else
				if(p->newword==0)
				{
					if(prev->type==phLIQUID)
						p->prepause = 20;
					if(prev->type==phNASAL)
						p->prepause = 12;

					if(prev->type==phSTOP && !(prev->ph->phflags & phFORTIS))
						p->prepause = 0;
				}
			}
			if((tr->langopts.word_gap & 0x10) && (p->newword) && (p->prepause < 20))
				p->prepause = 20;

			break;

		case phLIQUID:
		case phNASAL:
			p->amp = tr->stress_amps[1];  // unless changed later
			p->length = 256;  //  TEMPORARY
			min_drop = 0;
			
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
				if(p->pitch2 < 7)
					p->pitch2 = 7;
				p->pitch1 = p->pitch2 - 8;
				p->env = PITCHfall;
				pre_voiced = 0;
				
				if(p->type == phLIQUID)
				{
					p->length = speed1;
//p->pitch1 = p->pitch2 - 20;   // post vocalic [r/]
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
			min_drop = 0;
			next2 = &phoneme_list[ix+2];
			next3 = &phoneme_list[ix+3];

			if(stress > 7) stress = 7;

			if(pre_sonorant)
				p->amp = tr->stress_amps[stress]-1;
			else
				p->amp = tr->stress_amps[stress];

			if(emphasized)
				p->amp = 25;

			if(ix >= (n_phoneme_list-3))
			{
				// last phoneme of a clause, limit its amplitude
				if(p->amp > tr->langopts.param[LOPT_MAXAMP_EOC])
					p->amp = tr->langopts.param[LOPT_MAXAMP_EOC];
			}

			// is the last syllable of a word ?
			more_syllables=0;
			end_of_clause = 0;
			for(p2 = p+1; p2->newword== 0; p2++)
			{
				if((p2->type == phVOWEL) && !(p2->ph->phflags & phNONSYLLABIC))
					more_syllables++;

				if(p2->ph->code == phonPAUSE_CLAUSE)
					end_of_clause = 2;
			}
			if(p2->ph->code == phonPAUSE_CLAUSE)
				end_of_clause = 2;

			if((p2->newword & 2) && (more_syllables==0))
			{
				end_of_clause = 2;
			}

			// calc length modifier
			if((next->ph->code == phonPAUSE_VSHORT) && (next2->type == phPAUSE))
			{
				// if PAUSE_VSHORT is followed by a pause, then use that
				next = next2;
				next2 = next3;
				next3 = &phoneme_list[ix+4];
			}

			if(more_syllables==0)
			{
				len = tr->langopts.length_mods0[next2->ph->length_mod *10+ next->ph->length_mod];

				if((next->newword) && (tr->langopts.word_gap & 0x20))
				{
					// consider as a pause + first phoneme of the next word
					length_mod = (len + tr->langopts.length_mods0[next->ph->length_mod *10+ 1])/2;
				}
				else
					length_mod = len;
			}
			else
			{
				length_mod = tr->langopts.length_mods[next2->ph->length_mod *10+ next->ph->length_mod];

				if((next->type == phNASAL) && (next2->type == phSTOP || next2->type == phVSTOP) && (next3->ph->phflags & phFORTIS))
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

			if(length_mod < 8)
				length_mod = 8;     // restrict how much lengths can be reduced

			if(stress >= 7)
			{
				// tonic syllable, include a constant component so it doesn't decrease directly with speed
				length_mod += 20;
				if(emphasized)
					length_mod += 10;
			}
			else
			if(emphasized)
			{
				length_mod += 20;
			}
			
			if((len = tr->stress_lengths[stress]) == 0)
				len = tr->stress_lengths[6];

			length_mod = (length_mod * len)/128;

			if(p->tone_ph != 0)
			{
				if((tone_mod = phoneme_tab[p->tone_ph]->std_length) > 0)
				{
					// a tone phoneme specifies a percentage change to the length
					length_mod = (length_mod * tone_mod) / 100;
				}
			}

			if(end_of_clause == 2)
			{
				// this is the last syllable in the clause, lengthen it - more for short vowels
				len = p->ph->std_length;
				if(tr->langopts.stress_flags & 0x40000)
					len=200;  // don't lengthen short vowels more than long vowels at end-of-clause
				length_mod = length_mod * (256 + (280 - len)/3)/256;
			}

if(p->type != phVOWEL)
{
	length_mod = 256;   // syllabic consonant
	min_drop = 8;
}
			p->length = length_mod;

			// pre-vocalic part
			// set last-pitch
			env2 = p->env;
			if(env2 > 1) env2++;   // version for use with preceding semi-vowel

			if(p->tone_ph != 0)
			{
				pitch_env = LookupEnvelope(phoneme_tab[p->tone_ph]->spect);
			}
			else
			{
				pitch_env = envelope_data[env2];
			}

			pitch_start = p->pitch1 + ((p->pitch2-p->pitch1)*pitch_env[0])/256;

			if(pre_sonorant || pre_voiced)
			{
				// set pitch for pre-vocalic part
				if(pitch_start == 1024)
					last_pitch = pitch_start;    // pitch is not set

				if(pitch_start - last_pitch > 8)   // was 9
					last_pitch = pitch_start - 8;

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
			next->synthflags &= ~SFLAG_SEQCONTINUE;
			if(next->type == phNASAL && next2->type != phVOWEL)
				next->synthflags |= SFLAG_SEQCONTINUE;
				
			if(next->type == phLIQUID)
			{
				next->synthflags |= SFLAG_SEQCONTINUE;
					
				if(next2->type == phVOWEL)
				{
					next->synthflags &= ~SFLAG_SEQCONTINUE;
				}

				if(next2->type != phVOWEL)
				{
					if(next->ph->mnemonic == ('/'*256+'r'))
					{
						next->synthflags &= ~SFLAG_SEQCONTINUE;
//						min_drop = 15;
					}
				}
			}

			if((min_drop > 0) && ((p->pitch2 - p->pitch1) < min_drop))
			{
				p->pitch1 = p->pitch2 - min_drop;
				if(p->pitch1 < 0)
					p->pitch1 = 0;
			}

			last_pitch = p->pitch1 + ((p->pitch2-p->pitch1)*envelope_data[p->env][127])/256;
			pre_sonorant = 0;
			pre_voiced = 0;
			break;
		}
	}
}  //  end of CalcLengths

