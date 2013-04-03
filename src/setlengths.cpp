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
extern void DoSonicSpeed(int value);
extern int saved_parameters[];


// convert from words-per-minute to internal speed factor
// Use this to calibrate speed for wpm 80-350
static unsigned char speed_lookup[] = {
 255, 255, 255, 255, 255,   // 80
 253, 249, 245, 242, 238,   // 85
 235, 232, 228, 225, 222,   // 90
 218, 216, 213, 210, 207,   // 95
 204, 201, 198, 196, 193,   // 100
 191, 188, 186, 183, 181,   // 105
 179, 176, 174, 172, 169,   // 110
 168, 165, 163, 161, 159,   // 115
 158, 155, 153, 152, 150,   // 120
 148, 146, 145, 143, 141,   // 125
 139, 137, 136, 135, 133,   // 130
 131, 130, 129, 127, 126,   // 135
 124, 123, 122, 120, 119,   // 140
 118, 117, 115, 114, 113,   // 145
 112, 111, 110, 109, 107,   // 150
 106, 105, 104, 103, 102,   // 155
 101, 100,  99,  98,  97,   // 160
  96,  95,  94,  93,  92,   // 165
  91,  90,  89,  89,  88,   // 170
  87,  86,  85,  84,  83,   // 175
  82,  82,  81,  80,  80,   // 180
  79,  78,  77,  76,  76,   // 185
  75,  75,  74,  73,  72,   // 190
  71,  71,  70,  69,  69,   // 195
  68,  67,  67,  66,  66,   // 200
  65,  64,  64,  63,  62,   // 205
  62,  61,  61,  60,  59,   // 210
  59,  58,  58,  57,  57,   // 215
  56,  56,  55,  54,  54,   // 220
  53,  53,  52,  52,  52,   // 225
  51,  50,  50,  49,  49,   // 230
  48,  48,  47,  47,  46,   // 235
  46,  46,  45,  45,  44,   // 240
  44,  44,  43,  43,  42,   // 245
  41,  40,  40,  40,  39,   // 250
  39,  39,  38,  38,  38,   // 255
  37,  37,  37,  36,  36,   // 260
  35,  35,  35,  35,  34,   // 265
  34,  34,  33,  33,  33,   // 270
  32,  32,  31,  31,  31,   // 275
  30,  30,  30,  29,  29,   // 280
  29,  29,  28,  28,  27,   // 285
  27,  27,  27,  26,  26,   // 290
  26,  26,  25,  25,  25,   // 295
  24,  24,  24,  24,  23,   // 300
  23,  23,  23,  22,  22,   // 305
  22,  21,  21,  21,  21,   // 310
  20,  20,  20,  20,  19,   // 315
  19,  19,  18,  18,  17,   // 320
  17,  17,  16,  16,  16,   // 325
  16,  16,  16,  15,  15,   // 330
  15,  15,  14,  14,  14,   // 335
  13,  13,  13,  12,  12,   // 340
  12,  12,  11,  11,  11,   // 345
  11,  10,  10,  10,   9,   // 350
   9,   9,   8,   8,   8,   // 355
};


// speed_factor1 adjustments for speeds 350 to 374: pauses
static unsigned char pause_factor_350[] = {
22,22,22,22,22,22,22,21,21,21,  // 350
21,20,20,19,19,18,17,16,15,15,  // 360
15,15,15,15,15};                // 370

// wav_factor adjustments for speeds 350 to 450
// Use this to calibrate speed for wpm 350-450
static unsigned char wav_factor_350[] = {
 120, 121, 120, 119, 119,   // 350
 118, 118, 117, 116, 116,   // 355
 115, 114, 113, 112, 112,   // 360
 111, 111, 110, 109, 108,   // 365
 107, 106, 106, 104, 103,   // 370
 103, 102, 102, 102, 101,   // 375
 101,  99,  98,  98,  97,   // 380
  96,  96,  95,  94,  93,   // 385
  91,  90,  91,  90,  89,   // 390
  88,  86,  85,  86,  85,   // 395
  85,  84,  82,  81,  80,   // 400
  79,  77,  78,  78,  76,   // 405
  77,  75,  75,  74,  73,   // 410
  71,  72,  70,  69,  69,   // 415
  69,  67,  65,  64,  63,   // 420
  63,  63,  61,  61,  59,   // 425
  59,  59,  58,  56,  57,   // 430
  58,  56,  54,  53,  52,   // 435
  52,  53,  52,  52,  50,   // 440
  48,  47,  47,  45,  46,   // 445
  45};   // 450

static int speed1 = 130;
static int speed2 = 121;
static int speed3 = 118;



//#define TEST_SPEED

#ifdef INCLUDE_SONIC

void SetSpeed(int control)
{//=======================
	int x;
	int s1;
	int wpm;
	int wpm2;
	int wpm_value;
	double sonic;

	speed.loud_consonants = 0;
	speed.min_sample_len = 450;
	speed.lenmod_factor = 110;   // controls the effect of FRFLAG_LEN_MOD reduce length change
	speed.lenmod2_factor = 100;
	speed.min_pause = 5;

	wpm = embedded_value[EMBED_S];
	if(control == 2)
		wpm = embedded_value[EMBED_S2];

	wpm_value = wpm;

	if(voice->speed_percent > 0)
	{
		wpm = (wpm * voice->speed_percent)/100;
	}

	if(control & 2)
	{
		DoSonicSpeed(1 * 1024);
	}
	if((wpm_value > 450) || ((wpm_value > speed.fast_settings[0]) && (wpm > 350)))
	{
		wpm2 = wpm;
		wpm = 175;

		// set special eSpeak speed parameters for Sonic use
		// The eSpeak output will be speeded up by at least x2
		x = 73;
		if(control & 1)
		{
			speed1 = (x * voice->speedf1)/256;
			speed2 = (x * voice->speedf2)/256;
			speed3 = (x * voice->speedf3)/256;
		}
		if(control & 2)
		{
			sonic = ((double)wpm2)/wpm;
			DoSonicSpeed((int)(sonic * 1024));
			speed.pause_factor = 85;
			speed.clause_pause_factor = 80;
			speed.min_pause = 22;
			speed.min_sample_len = 450*2;
			speed.wav_factor = 211;
			speed.lenmod_factor = 210;
			speed.lenmod2_factor = 170;
		}
		return;
	}


#ifdef TEST_SPEED
	if(wpm > 1000)
	{
		// TESTING
//		test = wpm / 1000;
		wpm = wpm % 1000;
	}
#endif

	if(wpm > 450)
		wpm = 450;

	if(wpm > 360)
	{
		speed.loud_consonants = (wpm - 360) / 8;
	}

	wpm2 = wpm;
	if(wpm > 359) wpm2 = 359;
	if(wpm < 80) wpm2 = 80;
	x = speed_lookup[wpm2-80];

	if(wpm >= 380)
		x = 7;
	if(wpm >= 400)
		x = 6;

	if(control & 1)
	{
		// set speed factors for different syllable positions within a word
		// these are used in CalcLengths()
		speed1 = (x * voice->speedf1)/256;
		speed2 = (x * voice->speedf2)/256;
		speed3 = (x * voice->speedf3)/256;

		if(x <= 7)
		{
			speed1 = x;
			speed2 = speed3 = x - 1;
		}
	}

	if(control & 2)
	{
		// these are used in synthesis file

		if(wpm > 350)
		{
			speed.lenmod_factor = 85 - (wpm - 350) / 3;
			speed.lenmod2_factor = 60 - (wpm - 350) / 8;
		}
		else
		if(wpm > 250)
		{
			speed.lenmod_factor = 110 - (wpm - 250)/4;
			speed.lenmod2_factor = 110 - (wpm - 250)/2;
		}

		s1 = (x * voice->speedf1)/256;

		if(wpm >= 170)
			speed.wav_factor = 110 + (150*s1)/128;  // reduced speed adjustment, used for playing recorded sounds
		else
			speed.wav_factor = 128 + (128*s1)/130;  // = 215 at 170 wpm

		if(wpm >= 350)
		{
			speed.wav_factor = wav_factor_350[wpm-350];
		}

		if(wpm >= 390)
		{
			speed.min_sample_len = 450 - (wpm - 400)/2;
			if(wpm > 440)
				speed.min_sample_len = 420 - (wpm - 440);
		}

// adjust for different sample rates
speed.min_sample_len = (speed.min_sample_len * samplerate_native) / 22050;

		speed.pause_factor = (256 * s1)/115;      // full speed adjustment, used for pause length
		speed.clause_pause_factor = 0;

		if(wpm > 430)
		{
			speed.pause_factor = 12;
//			speed.clause_pause_factor = 15;
		}
		else
		if(wpm > 400)
		{
			speed.pause_factor = 13;
//			speed.clause_pause_factor = 15;
		}
		else
		if(wpm > 374)
		{
			speed.pause_factor = 14;
		}
		else
		if(wpm > 350)
		{
			speed.pause_factor = pause_factor_350[wpm - 350];
		}

		if(speed.clause_pause_factor == 0)
		{
			// restrict the reduction of pauses between clauses
			if((speed.clause_pause_factor = speed.pause_factor) < 16)
				speed.clause_pause_factor = 16;
		}
	}

#ifdef TEST_SPEED
//if(control==3)
printf("%3d: speedf %d %d %d   pause=%d %d   wav=%d  lenmod=%d %d\n",wpm,speed1,speed2,speed3, speed.pause_factor,speed.clause_pause_factor, speed.wav_factor,speed.lenmod_factor,speed.lenmod2_factor);
#endif
}  //  end of SetSpeed

#else  // not using sonic speed-up

void SetSpeed(int control)
{//=======================
// This is the earlier version of SetSpeed() before sonic speed-up was added
	int x;
	int s1;
	int wpm;
	int wpm2;

	speed.loud_consonants = 0;
	speed.min_sample_len = 450;
	speed.lenmod_factor = 110;   // controls the effect of FRFLAG_LEN_MOD reduce length change
	speed.lenmod2_factor = 100;

	wpm = embedded_value[EMBED_S];
	if(control == 2)
		wpm = embedded_value[EMBED_S2];

#ifdef TEST_SPEED
	if(wpm > 1000)
	{
		// TESTING
		test = wpm / 1000;
		wpm = wpm % 1000;
	}
#endif

	if(voice->speed_percent > 0)
	{
		wpm = (wpm * voice->speed_percent)/100;
	}
	if(wpm > 450)
		wpm = 450;

	if(wpm > 360)
	{
		speed.loud_consonants = (wpm - 360) / 8;
	}

	wpm2 = wpm;
	if(wpm > 359) wpm2 = 359;
	if(wpm < 80) wpm2 = 80;
	x = speed_lookup[wpm2-80];

	if(wpm >= 380)
		x = 7;
	if(wpm >= 400)
		x = 6;

	if(control & 1)
	{
		// set speed factors for different syllable positions within a word
		// these are used in CalcLengths()
		speed1 = (x * voice->speedf1)/256;
		speed2 = (x * voice->speedf2)/256;
		speed3 = (x * voice->speedf3)/256;

		if(x <= 7)
		{
			speed1 = x;
			speed2 = speed3 = x - 1;
		}
	}

	if(control & 2)
	{
		// these are used in synthesis file

		if(wpm > 350)
		{
			speed.lenmod_factor = 85 - (wpm - 350) / 3;
			speed.lenmod2_factor = 60 - (wpm - 350) / 8;
		}
		else
		if(wpm > 250)
		{
			speed.lenmod_factor = 110 - (wpm - 250)/4;
			speed.lenmod2_factor = 110 - (wpm - 250)/2;
		}

		s1 = (x * voice->speedf1)/256;

		if(wpm >= 170)
			speed.wav_factor = 110 + (150*s1)/128;  // reduced speed adjustment, used for playing recorded sounds
		else
			speed.wav_factor = 128 + (128*s1)/130;  // = 215 at 170 wpm

		if(wpm >= 350)
		{
			speed.wav_factor = wav_factor_350[wpm-350];
		}

		if(wpm >= 390)
		{
			speed.min_sample_len = 450 - (wpm - 400)/2;
			if(wpm > 440)
				speed.min_sample_len = 420 - (wpm - 440);
		}

		speed.pause_factor = (256 * s1)/115;      // full speed adjustment, used for pause length
		speed.clause_pause_factor = 0;

		if(wpm > 430)
		{
			speed.pause_factor = 12;
//			speed.clause_pause_factor = 15;
		}
		else
		if(wpm > 400)
		{
			speed.pause_factor = 13;
//			speed.clause_pause_factor = 15;
		}
		else
		if(wpm > 374)
		{
			speed.pause_factor = 14;
		}
		else
		if(wpm > 350)
		{
			speed.pause_factor = pause_factor_350[wpm - 350];
		}

		if(speed.clause_pause_factor == 0)
		{
			// restrict the reduction of pauses between clauses
			if((speed.clause_pause_factor = speed.pause_factor) < 16)
				speed.clause_pause_factor = 16;
		}
	}

#ifdef TEST_SPEED
//if(control==3)
printf("%3d: speedf %d %d %d   pause=%d %d   wav=%d  lenmod=%d %d\n",wpm,speed1,speed2,speed3, speed.pause_factor,speed.clause_pause_factor, speed.wav_factor,speed.lenmod_factor,speed.lenmod2_factor);
#endif
}  //  end of SetSpeed

#endif   // of INCLUDE_SONIC


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
	saved_parameters[parameter] = new_value;

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



static void DoEmbedded2(int *embix)
{//================================
	// There were embedded commands in the text at this point

	unsigned int word;

	do {
		word = embedded_list[(*embix)++];

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
	int  next2type;
	int  len;
	int  env2;
	int  end_of_clause;
	int  embedded_ix = 0;
	int  min_drop;
	int  pitch1;
	int emphasized;
	int  tone_mod;
	unsigned char *pitch_env=NULL;
	PHONEME_DATA phdata_tone;

	for(ix=1; ix<n_phoneme_list; ix++)
	{
		prev = &phoneme_list[ix-1];
		p = &phoneme_list[ix];
		stress = p->stresslevel & 0x7;
		emphasized = p->stresslevel & 0x8;

		next = &phoneme_list[ix+1];

		if(p->synthflags & SFLAG_EMBEDDED)
		{
			DoEmbedded2(&embedded_ix);
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
				p->prepause = 25;
			else
			if((more_syllables > 0) || (stress < 4))
				p->prepause = 48;
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
		case phFRICATIVE:
			if(p->newword)
			{
				if((prev->type == phVOWEL) && (p->ph->phflags & phNOPAUSE))
				{
				}
				else
				{
					p->prepause = 15;
				}
			}

			if(next->type==phPAUSE && prev->type==phNASAL && !(p->ph->phflags&phFORTIS))
				p->prepause = 25;

			if(prev->ph->phflags & phBRKAFTER)
				p->prepause = 30;

			if((tr->langopts.word_gap & 0x10) && (p->newword))
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

			if(type == phVFRICATIVE)
			{
				if(next->type==phVOWEL)
				{
					pre_voiced = 1;
				}
				if((prev->type==phVOWEL) || (prev->type == phLIQUID))
				{
					p->length = (255 + prev->length)/2;
				}
			}
			break;

		case phVSTOP:
			if(prev->type==phVFRICATIVE || prev->type==phFRICATIVE || (prev->ph->phflags & phSIBILANT) || (prev->type == phLIQUID))
				p->prepause = 30;

			if(next->type==phVOWEL || next->type==phLIQUID)
			{
				if((next->type==phVOWEL) || !next->newword)
					pre_voiced = 1;

				p->prepause = 40;

				if(prev->type == phVOWEL)
				{
					p->prepause = 0;   // use murmur instead to link from the preceding vowel
				}
				else
				if(prev->type == phPAUSE)
				{
					// reduce by the length of the preceding pause
					if(prev->length < p->prepause)
						p->prepause -= prev->length;
					else
						p->prepause = 0;
				}
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
			p->amp = tr->stress_amps[0];  // unless changed later
			p->length = 256;  //  TEMPORARY
			min_drop = 0;

			if(p->newword)
			{
				if(prev->type==phLIQUID)
					p->prepause = 25;
				if(prev->type==phVOWEL)
				{
					if(!(p->ph->phflags & phNOPAUSE))
						p->prepause = 12;
				}
			}

			if(next->type==phVOWEL)
			{
				pre_sonorant = 1;
			}
			else
			{
				p->pitch2 = last_pitch;

				if((prev->type==phVOWEL) || (prev->type == phLIQUID))
				{
					p->length = prev->length;

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
					for(ix2=ix; ix2<n_phoneme_list; ix2++)
					{
						if(phoneme_list[ix2].type == phVOWEL)
						{
							p->pitch2 = phoneme_list[ix2].pitch2;
							break;
						}
					}
				}

				p->pitch1 = p->pitch2-16;
				if(p->pitch2 < 16)
				{
					p->pitch1 = 0;
				}
				p->env = PITCHfall;
				pre_voiced = 0;
			}
			break;

		case phVOWEL:
			min_drop = 0;
			next2 = &phoneme_list[ix+2];
			next3 = &phoneme_list[ix+3];

			if(stress > 7) stress = 7;

if(stress <= 1)
{
  stress = stress ^ 1;  // swap diminished and unstressed (until we swap stress_amps,stress_lengths in tr_languages)
}
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

			next2type = next2->ph->length_mod;
			if(more_syllables==0)
			{
				if(next->newword || next2->newword)
				{
					// don't use 2nd phoneme over a word boundary, unless it's a pause
					if(next2type != 1)
						next2type = 0;
				}

				len = tr->langopts.length_mods0[next2type *10+ next->ph->length_mod];

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
				length_mod = tr->langopts.length_mods[next2type *10+ next->ph->length_mod];

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
				length_mod += tr->langopts.lengthen_tonic;
				if(emphasized)
					length_mod += (tr->langopts.lengthen_tonic/2);
			}
			else
			if(emphasized)
			{
				length_mod += tr->langopts.lengthen_tonic;
			}

			if((len = tr->stress_lengths[stress]) == 0)
				len = tr->stress_lengths[6];

			length_mod = length_mod * len;

			if(p->tone_ph != 0)
			{
				if((tone_mod = phoneme_tab[p->tone_ph]->std_length) > 0)
				{
					// a tone phoneme specifies a percentage change to the length
					length_mod = (length_mod * tone_mod) / 100;
				}
			}


			if((end_of_clause == 2) && !(tr->langopts.stress_flags & S_NO_EOC_LENGTHEN))
			{
				// this is the last syllable in the clause, lengthen it - more for short vowels
				len = (p->ph->std_length * 2);
				if(tr->langopts.stress_flags & S_EO_CLAUSE1)
					len=200;  // don't lengthen short vowels more than long vowels at end-of-clause
				length_mod = length_mod * (256 + (280 - len)/3)/256;
			}

			if(length_mod > tr->langopts.max_lengthmod*speed1)
			{
				//limit the vowel length adjustment for some languages
				length_mod = (tr->langopts.max_lengthmod*speed1);
			}

			length_mod = length_mod / 128;

if(p->type != phVOWEL)
{
	length_mod = 256;   // syllabic consonant
	min_drop = 16;
}
			p->length = length_mod;

			if(p->env >= (N_ENVELOPE_DATA-1))
			{
				fprintf(stderr,"espeak: Bad intonation data\n");
				p->env = 0;
			}

			// pre-vocalic part
			// set last-pitch
			env2 = p->env + 1;  // version for use with preceding semi-vowel

			if(p->tone_ph != 0)
			{
				InterpretPhoneme2(p->tone_ph, &phdata_tone);
				pitch_env = GetEnvelope(phdata_tone.pitch_env);
			}
			else
			{
				pitch_env = envelope_data[env2];
			}

			pitch_start = p->pitch1 + ((p->pitch2-p->pitch1)*pitch_env[0])/256;

			if(pre_sonorant || pre_voiced)
			{
				// set pitch for pre-vocalic part
				if(pitch_start == 255)
					last_pitch = pitch_start;    // pitch is not set

				if(pitch_start - last_pitch > 16)
					last_pitch = pitch_start - 16;

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
				pitch1 = p->pitch2 - min_drop;
				if(pitch1 < 0)
					pitch1 = 0;
				p->pitch1 = pitch1;
			}

			last_pitch = p->pitch1 + ((p->pitch2-p->pitch1)*envelope_data[p->env][127])/256;
			pre_sonorant = 0;
			pre_voiced = 0;
			break;
		}
	}
}  //  end of CalcLengths

