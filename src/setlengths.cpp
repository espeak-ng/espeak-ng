/***************************************************************************
 *   Copyright (C) 2005,2006 by Jonathan Duddington                        *
 *   jonsd@users.sourceforge.net                                           *
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
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <wctype.h>

#include "speak_lib.h"
#include "speech.h"
#include "voice.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"

extern int GetAmplitude(void);


// convert from words-per-minute to internal speed factor
static unsigned char speed_lookup[241] = {
	253, 250, 247, 243, 240, 237, 234, 231,   //  80
   229, 226, 223, 220, 217, 214, 212, 209,   //  88
   206, 204, 201, 198, 196, 193, 191, 189,   //  96
   186, 184, 181, 179, 177, 175, 172, 170,   // 104
   166, 164, 162, 160, 159, 157, 155, 153,   // 112
   151, 150, 148, 146, 144, 143, 141, 136,   // 120
   135, 134, 133, 131, 130, 129, 128, 126,   // 128
   125, 124, 123, 122, 121, 119, 118, 117,   // 136
   116, 114, 113, 112, 111, 110, 109, 108,   // 144
   107, 106, 105, 104, 103, 102, 101, 100,   // 152
    97,  96,  95,  95,  94,  93,  92,  92,   // 160
    91,  90,  89,  89,  88,  87,  86,  86,   // 168
    85,  84,  84,  83,  82,  81,  81,  80,   // 176
    78,  77,  77,  76,  75,  75,  74,  74,   // 184
    73,  73,  72,  71,  71,  70,  70,  69,   // 192
    69,  68,  68,  67,  66,  66,  65,  65,   // 200
    64,  64,  63,  63,  62,  62,  61,  61,   // 208
    60,  60,  58,  57,  57,  56,  56,  56,   // 216
    55,  55,  54,  54,  54,  53,  53,  53,   // 224
    52,  52,  51,  51,  51,  50,  50,  44,   // 232
    44,  44,  44,  44,  44,  44,  43,  43,   // 240
    43,  43,  43,  43,  42,  42,  42,  42,   // 248
    41,  41,  41,  41,  40,  40,  40,  40,   // 256
    34,  34,  34,  34,  34,  34,  34,  34,   // 264
    34,  34,  34,  33,  33,  33,  33,  32,   // 272
    32,  32,  32,  31,  31,  31,  30,  30,   // 280
    32,  32,  31,  30,  29,  29,  28,  27,   // 288
    26,  25,  25,  24,  23,  22,  21,  21,   // 296
    20,  19,  18,  17,  16,  16,  15,  15,   // 304
    14,  13,  12,  11,  10,   9,   8,   7,   6,  // 312
};

static int speed1 = 130;
static int speed2 = 121;
static int speed3 = 118;

extern int speed_factor1;
extern int speed_factor2;


void SetSpeed(int control)
{//=======================
	int x;
	int s1;
	int wpm;

	wpm = embedded_value[EMBED_S];
	if(wpm > 320) wpm = 320;
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
		speed_factor1 = (256 * s1)/115;      // full speed adjustment, used for pause length
if(speed_factor1 < 18)
	speed_factor1 = 18;
		speed_factor2 = 120 + (137*s1)/128;  // reduced speed adjustment, used for playing recorded sounds
//		speed_factor2 = 128 + (128*s1)/130;  // reduced speed adjustment
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
		embedded_value[EMBED_P] = new_value;
		break;

	case espeakRANGE:
		if(new_value > 99) new_value = 99;
		embedded_value[EMBED_R] = new_value;
		break;

	case espeakPUNCTUATION:
		break;

	case espeakCAPITALS:
		break;

	case espeakLINELENGTH:
		option_linelength = new_value;
		break;

	default:
		break;
	}
}  // end of espeak_SetParameter




#ifdef deleted
// used to set up the presets in the speed_lookup table
// interpolate between a set of measured wpm values
void SetSpeedTab(void)
{//===================
#define N_WPM   13

	// Interpolation table to translate from words-per-minute to internal speed
	// words-per-minute values (measured)
	static float wpm1[N_WPM] =
			{0, 99.0, 111.3, 127.0, 140.9, 160.0, 183.9, 217.3, 238.5, 263.2, 287.5, 304.4, 316 };
	// corresponding internal speed values
	static float wpm2[N_WPM] = 
			{0, 200,  170,   140,   120,   100,    80,    60,    50,    40,    30,    20,  10};


	int ix;
	float x;


	// convert from word-per-minute to internal speed code
	for(speed_wpm=80; speed_wpm<320; speed_wpm++)
	{
		for(ix=2; ix<N_WPM-2; ix++)
		{
			if(speed_wpm < wpm1[ix])
				break;
		}
		x = polint(&wpm1[ix-1], &wpm2[ix-1], 3, speed);

		speed_lookup[speed_wpm-80] = x;
	}
}	// end of SetSpeedTab
#endif


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
	unsigned char *pitch_env=NULL;

	for(ix=1; ix<n_phoneme_list; ix++)
	{
		prev = &phoneme_list[ix-1];
		p = &phoneme_list[ix];
		stress = p->tone & 0xf;

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
			if(prev->type == phSTOP || prev->type == phFRICATIVE)
				p->prepause = 20;
			else
			if((more_syllables > 0) || (stress < 4))
				p->prepause = 40;
			else
				p->prepause = 60;

			if((langopts.word_gap & 0x10) && (p->newword))
				p->prepause = 60;

			if(p->synthflags & SFLAG_LENGTHEN)
				p->prepause += langopts.long_stop;
			break;

		case phVFRICATIVE:
			if(next->type==phVOWEL)
			{
				pre_voiced = 1;
			}
		case phFRICATIVE:
			if(p->newword)
				p->prepause = 15;

			if(next->type==phPAUSE && prev->type==phNASAL && !(p->ph->phflags&phFORTIS))
				p->prepause = 25;

			if((p->ph->phflags & phSIBILANT) && next->type==phSTOP && !next->newword)
			{
				if(prev->type == phVOWEL)
					p->length = 200;      // ?? should do this if it's from a prefix
				else
					p->length = 150;
			}
			else
				p->length = 256;

			if((langopts.word_gap & 0x10) && (p->newword))
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
			if((langopts.word_gap & 0x10) && (p->newword) && (p->prepause < 20))
				p->prepause = 20;

			break;

		case phLIQUID:
		case phNASAL:
			p->amp = stress_amps[1];  // unless changed later
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
p->pitch1 = p->pitch2 - 20;   // post vocalic [r/]
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
				p->amp = stress_amps[stress]-1;
			else
				p->amp = stress_amps[stress];

			if(ix >= (n_phoneme_list-3))
			{
				// last phoneme of a clause, limit its amplitude
				if(p->amp > langopts.param[LOPT_MAXAMP_EOC])
					p->amp = langopts.param[LOPT_MAXAMP_EOC];
			}

			// is the last syllable of a word ?
			more_syllables=0;
			end_of_clause = 0;
			for(p2 = p+1; p2->newword== 0; p2++)
			{
				if(p2->type == phVOWEL)
					more_syllables++;
			}
			if((p2->newword & 2) && (more_syllables==0))
			{
				end_of_clause = 2;
			}

			// calc length modifier
			if(more_syllables==0)
			{
				len = langopts.length_mods0[next2->ph->length_mod *10+ next->ph->length_mod];

				if((next->newword) && (langopts.word_gap & 0x4))
				{
					// consider as a pause + first phoneme of the next word
					length_mod = (len + langopts.length_mods0[next->ph->length_mod *10+ 1])/2;
				}
				else
					length_mod = len;
			}
			else
			{
				length_mod = langopts.length_mods[next2->ph->length_mod *10+ next->ph->length_mod];

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
			if(length_mod < 24)
				length_mod = 24;     // restrict how much lengths can be reduced

			if(stress >= 7)
			{
				// tonic syllable, include a constant component so it doesn't decrease directly with speed
				length_mod += 20;
			}
			
			length_mod = (length_mod * stress_lengths[stress])/128;

			if(end_of_clause == 2)
			{
				// this is the last syllable in the clause, lengthen it - more for short vowels
				length_mod = length_mod * (256 + (280 - p->ph->std_length)/3)/256;
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

