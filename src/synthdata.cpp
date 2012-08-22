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
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "speech.h"
#include "voice.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"

static int stress_lengths1[8] = {182,140, 220,220, 220,240, 248,250};
static int stress_amps1[] = {16,16, 20,20, 20,24, 24,22 };
char voice_name[40];

// limit the rate of change for each formant number
//static float formant_rate_22050[9] = {0.2, 0.4, 0.64, 0.9, 0.85, 0.85, 1, 1, 1};
static int formant_rate_22050[9] = {51, 102, 164, 230, 218, 218, 256, 256, 256};  // values for 22kHz sample rate
int formant_rate[9];         // values adjusted for actual sample rate


PHONEME_TAB *phoneme_tab=NULL;
unsigned int *phoneme_index=NULL;
static char *spects_data=NULL;
unsigned char *wavefile_data=NULL;
static unsigned char *phoneme_tab_data = NULL;

static int n_phoneme_tables;
static int phoneme_tab_number = 0;
static PHONEME_TAB_LIST phoneme_tab_list[N_PHONEME_TABS];

int wavefile_ix;              // a wavefile to play along with the synthesis
static int seq_len_adjust;

void BendVowel(frameref_t *seq, int &n_frames, PHONEME_TAB *other_ph, int which);


unsigned int StringToWord(const char *string)
{//==========================================
	int  ix;
	unsigned char c;
	unsigned int word;

	word = 0;
	for(ix=0; ix<4; ix++)
	{
		if(string[ix]==0) break;
		c = string[ix];
		word |= (c << (ix*8));
	}
	return(word);
}


int LookupPh(const char *string)
{//=============================
	int  ix;
	unsigned int  mnem;

	mnem = StringToWord(string);
	for(ix=0; ix<256; ix++)
	{
		if(phoneme_tab[ix].mnemonic == mnem)
			return(ix);
	}
	return(0);
}



const char *PhonemeTabName(void)
{//=============================
	return(phoneme_tab_list[phoneme_tab_number].name);
}


static int ReadPhFile(char **ptr, const char *fname)
{//=================================================
	FILE *f_in;
	char *p;
	unsigned int  length;
	char buf[200];

	sprintf(buf,"%s%c%s",path_home,PATHSEP,fname);
	length = GetFileLength(buf);
	
	if((f_in = fopen(buf,"rb")) == NULL)
	{
		fprintf(stderr,"Can't read data file: '%s'\n",buf);
		return(1);
	}

	if(*ptr != NULL)
		Free(*ptr);
		
	if((p = Alloc(length)) == NULL)
	{
		fclose(f_in);
		return(-1);
	}
	if(fread(p,1,length,f_in) != length)
	{
		fclose(f_in);
		return(-1);
	}
	*ptr = p;
	fclose(f_in);
	return(0);
}  //  end of ReadPhFile


int LoadPhData()
{//=============
	int ix;
	int n_phonemes;
	unsigned char *p;

	if(ReadPhFile((char **)(&phoneme_tab_data),"phontab") != 0)
		return(-1);
	if(ReadPhFile((char **)(&phoneme_index),"phonindex") != 0)
		return(-1);
	if(ReadPhFile((char **)(&spects_data),"phondata") != 0)
		return(-1);
   wavefile_data = (unsigned char *)spects_data;

	// set up phoneme tables
	p = phoneme_tab_data;
	n_phoneme_tables = p[0];
	p+=4;

	for(ix=0; ix<n_phoneme_tables; ix++)
	{
		n_phonemes = p[0];
		p += 4;
		memcpy(phoneme_tab_list[ix].name,p,N_PHONEME_TAB_NAME);
		p += N_PHONEME_TAB_NAME;
		phoneme_tab_list[ix].phoneme_tab_ptr = (PHONEME_TAB *)p;
		p += (n_phonemes * sizeof(PHONEME_TAB));
	}

	if(phoneme_tab_number >= n_phoneme_tables)
		phoneme_tab_number = 0;
	phoneme_tab = phoneme_tab_list[phoneme_tab_number].phoneme_tab_ptr;

	return(0);
}  //  end of LoadPhData

#ifdef PLATFORM_RISCOS
void FreePhData(void)
{//==================
	Free(phoneme_tab_data);
	Free(phoneme_index);
	Free(spects_data);
}
#endif

static unsigned int LookupSound2(int index, unsigned int phcode)
{//=============================================================
	unsigned int value, value2;
	
	while((value = phoneme_index[index++]) != 0)
	{
		if((value & 0xff) == phcode)
		{
			while(((value2 = phoneme_index[index]) != 0) && ((value2 & 0xff) < 8))
			{
				switch(value2 & 0xff)
				{
				case 0:
					// next entry is a wavefile to be played along with the synthesis
					wavefile_ix = value2 >> 8;
					break;
				case 1:
					seq_len_adjust = value2 >> 8;
					break;
				case 2:
					seq_len_adjust = -(value2 >> 8);
					break;
				}
				index++;
			}
			return(value >> 8);
		}
	}
	return(3);   // not found
}  //  end of LookupSound2


unsigned int LookupSound(PHONEME_TAB *this_ph, PHONEME_TAB *other_ph, int which, int *match_level)
{//===============================================================================================
	// follows,  1 other_ph preceeds this_ph,   2 other_ph follows this_ph
	int spect_list;
	int spect_list2;
	int s_list;
	unsigned char virtual_ph;
	int  result;
	int  level=0;
	unsigned int  other_code;
	unsigned int  other_virtual;
	
	wavefile_ix = 0;
	seq_len_adjust = 0;
	
	other_code = other_ph->code;
	if(phoneme_tab[other_code].type == phPAUSE)
		other_code = phonPAUSE_SHORT;       // use this version of Pause for matching

	if(which==1)
	{
		spect_list = this_ph->after;
		virtual_ph = this_ph->start_type;
		spect_list2 = phoneme_tab[virtual_ph].after;
		other_virtual = other_ph->end_type;
	}
	else
	{
		spect_list = this_ph->before;
		virtual_ph = this_ph->end_type;
		spect_list2 = phoneme_tab[virtual_ph].before;
		other_virtual = other_ph->start_type;
	}

	result = 3;
	// look for ph1-ph2 combination
	if((s_list = spect_list) != 0)
	{
		if((result = LookupSound2(s_list,other_code)) != 3)
		{
			level = 2;
		}
		else
		if(other_virtual != 0)
		{
			if((result = LookupSound2(spect_list,other_virtual)) != 3)
			{
				level = 1;
			}
		}
	}
	// not found, look in a virtual phoneme if one is given for this phoneme
	if((result==3) && (virtual_ph != 0) && ((s_list = spect_list2) != 0))
	{
		if((result = LookupSound2(s_list,other_code)) != 3)
		{
			level = 1;
		}
		else
		if(other_virtual != 0)
		{
			if((result = LookupSound2(spect_list2,other_virtual)) != 3)
			{
				level = 1;
			}
		}
	}

	*match_level = level;
	
	if(result==0)
		return(0);   // NULL was given in the phoneme source

	// note: values = 1 indicates use the default for this phoneme, even though we found a match
	// which set a secondary reference 
	if(result >= 4)
	{
		// values 1-3 can be used for special codes
		// 1 = DFT from the phoneme source file
		return(result);
	}
	
	// no match found for other_ph, return the default
	return(LookupSound2(this_ph->spect,phonPAUSE));

}  //  end of LookupSound



frameref_t *LookupSpect(PHONEME_TAB *this_ph, PHONEME_TAB *other_ph, int which, int *match_level, int *n_frames, int stress)
{//=========================================================================================================
	int  ix;
	int  nf;
	int  nf1;
	int  seq_break;
	frameref_t *frames;
	int  length1;
	int  length_std;
	int  length_factor;
	SPECT_SEQ *seq;
	SPECT_SEQ *seq2;
	static frameref_t frames_buf[N_SEQ_FRAMES];
	
	if((ix = LookupSound(this_ph,other_ph,which,match_level)) == 0)
		return(NULL);
	seq = (SPECT_SEQ *)(&spects_data[ix]);
	nf = seq->n_frames;

	seq_break = 0;
	length1 = 0;
	for(ix=0; ix<nf; ix++)
	{
		frames_buf[ix].frame = &seq->frame[ix];
		frames_buf[ix].flags = seq->frame[ix].flags;
		frames_buf[ix].length = seq->frame[ix].length;
		if(seq->frame[ix].flags & FRFLAG_VOWEL_CENTRE)
			seq_break = ix;
	}
	
	frames = &frames_buf[0];
	if(seq_break > 0)
	{
		if(which==1)
		{
			nf = seq_break + 1;
		}
		else
		{
			frames = &frames_buf[seq_break];  // body of vowel, skip past initial frames
			nf -= seq_break;
		}
	}
	
	// do we need to modify a frame for blending with a consonant?
	if((this_ph->type == phVOWEL) && (*match_level == 0))
	{
		BendVowel(frames,nf,other_ph,which);
	}

	nf1 = nf - 1;
	for(ix=0; ix<nf1; ix++)
		length1 += frames[ix].length;


	if((wavefile_ix != 0) && ((wavefile_ix & 0x800000)==0))
	{
		// a secondary reference has been returned, which is not a wavefile
		// add these spectra to the main sequence
		seq2 = (SPECT_SEQ *)(&spects_data[wavefile_ix]);
	
		// first frame of the addition just sets the length of the last frame of the main seq
		nf--;
		for(ix=0; ix<seq2->n_frames; ix++)
		{
			frames[nf].length = seq2->frame[ix].length;
			if(ix > 0)
				frames[nf].frame = &seq2->frame[ix];
			nf++;
		}
		wavefile_ix = 0;
	}
	
	if((this_ph->type == phVOWEL) && (length1 > 0))
	{
		if(which==2)
		{
			// adjust the length of the main part to match the standard length specified for the vowel
			//   less the front part of the vowel and any added suffix
	
			length_std = this_ph->std_length + seq_len_adjust - 45;

// can adjust vowel length for stressed syllables here


			length_factor = (length_std * 256)/ length1;
			
			for(ix=0; ix<nf1; ix++)
			{
				frames[ix].length = (frames[ix].length * length_factor)/256;
			}
		}
		else
		{
			// front of a vowel
			if(*match_level == 0)
			{
				// allow very short vowels to have shorter front parts
				if(this_ph->std_length < 130)
					frames[0].length = (frames[0].length * this_ph->std_length)/130;
			}

			if(seq_len_adjust != 0)
			{
				length_std = 0;
				for(ix=0; ix<nf1; ix++)
				{
					length_std += frames[ix].length;
				}
				length_factor = ((length_std + seq_len_adjust) * 256)/length_std;
				for(ix=0; ix<nf1; ix++)
				{
					frames[ix].length = (frames[ix].length * length_factor)/256;
				}
			}
		}
	}
	
	*n_frames = nf;
	return(frames);
}  //  end of LookupSpect


unsigned char *LookupEnvelope(int ix)
{//================================
	if(ix==0)
		return(NULL);
	return((unsigned char *)&spects_data[phoneme_index[ix]]);
}




typedef struct {
	const char *mnem;
	int data;
} keywtab_t;

static keywtab_t keyword_tab[] = {
	{"formant",   1},
	{"pitch",     2},
	{"phonemes",  3},
   {"language",  4},
	{"dictionary", 5},
	{"stressLength", 6},
	{"stressAmp",  7},
	{"intonation", 8},
	{"replace",    9},
	{"replaceWE",  10},
	{"words",      11},
	{"echo",       12},
	{"flutter",    13},
	{"roughness",  14},
	{NULL,   0} };

#define N_VOICES 100
int n_voices_tab = 0;
voice_t *voices_tab[N_VOICES];


static void VoiceReset(void)
{//=========================
	// Set voice to the default values
	int  pk;
	int  ix;

// try default of:  pitch 82,118
	voice->pitch_base =   0x49000;      // default 71 << 12;
	voice->pitch_range =  0x0f30;     // default = 0x1000

	voice->echo_delay = 0;
	voice->echo_amp = 0;
	voice->flutter = 64;
#ifdef PLATFORM_RISCOS
	voice->roughness = 1;
#else
	voice->roughness = 2;
#endif

	voice->intonation1 = 0;
	voice->intonation2 = 0;
	option_stress_rule = 2;
	

	for(pk=0; pk<N_PEAKS; pk++)
	{
		voice->freq[pk] = 256;
		voice->height[pk] = 256;
		voice->width[pk] = 256;

		// adjust formant smoothing depending on sample rate
		formant_rate[pk] = (formant_rate_22050[pk] * 22050)/samplerate;
	}
	phoneme_tab = phoneme_tab_list[0].phoneme_tab_ptr;

	// default values of speed factors
	voice->speedf1 = 256;
	voice->speedf2 = 238;
	voice->speedf3 = 232;

	// relative lengths of different stress syllables
	memcpy(stress_lengths,stress_lengths1,sizeof(stress_lengths));
	memcpy(stress_amps,stress_amps1,sizeof(stress_amps));
	memcpy(stress_amps_r,stress_amps1,sizeof(stress_amps));
	for(ix=0; ix<8; ix++)
		stress_amps_r[ix] = stress_amps[ix] -1;

	n_replace_phonemes = 0;
	option_words = 0;
	option_vowel_pause = 0;
	option_tone1 = 0;
}


static void VoiceFormant(char *p)
{//==============================
	// Set parameters for a formant
	int ix;
	int formant;
	int freq = -1;
	int height = -1;
	int width = -1;

	ix = sscanf(p,"%d %d %d %d",&formant,&freq,&height,&width);
	if(ix < 2)
		return;

	if((formant < 0) || (formant > 8))
		return;

	if(freq >= 0)
		voice->freq[formant] = int(freq * 2.56001);
	if(height >= 0)
		voice->height[formant] = int(height * 2.56001);
	if(width >= 0)
		voice->width[formant] = int(width * 2.56001);
}


static int VoicePhonemes(const char *name)
{//=======================================
// Look up a phoneme set by name, and select it if it exists
	int ix;

	for(ix=0; ix<n_phoneme_tables; ix++)
	{
		if(strcmp(name,phoneme_tab_list[ix].name)==0)
		{
			phoneme_tab = phoneme_tab_list[ix].phoneme_tab_ptr;
			phoneme_tab_number = ix;
			break;
		}
	}
	if(ix == n_phoneme_tables)
		return(1);
	return(0);
}


static int VoiceLanguage(const char *name)
{//=======================================
	if(translator != NULL)
		delete translator;

	if(strcmp(name,"english")==0)
	{
		translator = new Translator_English();
		return(1);
	}
	if(strcmp(name,"esperanto")==0)
	{
		translator = new Translator_Esperanto();
		return(1);
	}
	if(strcmp(name,"german")==0)
	{
		translator = new Translator_German();
		return(1);
	}
	if(strcmp(name,"tone")==0)
	{
		translator = new Translator_Tone();
		return(1);
	}
	translator = new Translator_Default();
	return(-1);
}


static voice_t *VoiceLookup(char *voicename)
{//=========================================
	int ix;
	voice_t *v;

	for(ix=0; ix < N_VOICES; ix++)
	{
		if(ix == n_voices_tab)
		{
			// found a free slot
			v = (voice_t *)Alloc(sizeof(voice_t));
			if(v == NULL)
				return(NULL);
			voices_tab[n_voices_tab++] = v;
			strncpy0(v->name,voicename,sizeof(v->name));
			return(v);
		}
		else
		if(strcmp(voices_tab[ix]->name,voicename)==0)
		{
			return(voices_tab[ix]);   // found the entry for the specified voice name
		}
	}
	return(NULL);  // table is full
}


voice_t *LoadVoice(char *vname, int reset)
{//=======================================
	FILE *f_voice = NULL;
	keywtab_t *k;
	char *p;
	int  key;
	int  ix;
	int  n;
	int  value;
	int  phon;
	int  replace_type=0;
	int  error = 0;
	int  language_set = 0;
	voice_t *v;
	char voicename[40];
	char new_dictionary[80];
	char language_name[80];
	char phonemes_name[80];
	char phon_string1[12];
	char phon_string2[12];
	char buf[120];

	int pitch1;
	int pitch2;

	strcpy(language_name,"english");
	strcpy(new_dictionary,language_name);      // default
	strcpy(phonemes_name,language_name);
	if(translator != NULL)
		delete translator;

	strcpy(voicename,vname);
	if(voicename[0]==0)
		strcpy(voicename,"default");

	sprintf(buf,"%s%cvoices%c%s",path_home,PATHSEP,PATHSEP,voicename);
	f_voice = fopen(buf,"r");

	if((v = VoiceLookup(voicename)) != NULL)
		voice = v;
	VoiceReset();

	translator = new Translator_English();
	VoicePhonemes(phonemes_name);


	while((f_voice != NULL) && (fgets(buf,sizeof(buf),f_voice) != NULL))
	{
		for(p=buf; (*p != 0) && !isspace(*p); p++);
		*p++ = 0;

		if(buf[0] == 0) continue;

		key = 0;
		for(k=keyword_tab; k->mnem != NULL; k++)
		{
			if(strcmp(buf,k->mnem)==0)
			{
				key = k->data;
				break;
			}
		}


		switch(key)
		{
		case 1:
			VoiceFormant(p);
			break;

		case 2:
			n = sscanf(p,"%d %d",&pitch1,&pitch2);
			voice->pitch_base = (pitch1 - 9) << 12;
			voice->pitch_range = (pitch2 - pitch1) * 108;
			break;

		case 3:        // phoneme table
			n = sscanf(p,"%s",phonemes_name);
			break;

		case 4:        // language_name
			n = sscanf(p,"%s",language_name);
			language_name[0] = tolower(language_name[0]);
			language_set = VoiceLanguage(language_name);
			if(language_set != 0)
			{
				strcpy(new_dictionary,language_name);
				strcpy(phonemes_name,language_name);
				VoicePhonemes(phonemes_name);
			}
			break;

		case 5:        // dictionary
			n = sscanf(p,"%s",new_dictionary);
			break;

		case 6:   // stressLength
			sscanf(p,"%d %d %d %d %d %d %d %d",
				&stress_lengths[0], &stress_lengths[1], &stress_lengths[2], &stress_lengths[3],
				&stress_lengths[4], &stress_lengths[5], &stress_lengths[6], &stress_lengths[7]);
			break;

		case 7:   // stressAmp
			sscanf(p,"%d %d %d %d %d %d %d %d",
				&stress_amps[0], &stress_amps[1], &stress_amps[2], &stress_amps[3],
				&stress_amps[4], &stress_amps[5], &stress_amps[6], &stress_amps[7]);
			for(ix=0; ix<8; ix++)
					stress_amps_r[ix] = stress_amps[ix] -1;
			break;

		case 8:   // intonation
			sscanf(p,"%d %d",&option_tone1,&option_tone2);
			break;

		case 10:
			replace_type = 1;
		case 9:
			strcpy(phon_string2,"NULL");
			n = sscanf(p,"%s %s",phon_string1,phon_string2);
			if((n < 1) || (n_replace_phonemes >= N_REPLACE_PHONEMES))
				break;
			
			if((phon = LookupPh(phon_string1)) == 0)
				break;  // not recognised
	
			replace_phonemes[n_replace_phonemes].old_ph = phon;
			replace_phonemes[n_replace_phonemes].new_ph = LookupPh(phon_string2);
			replace_phonemes[n_replace_phonemes++].type = replace_type;
			break;

		case 11:   // words
			sscanf(p,"%d",&option_words);
			break;

		case 12:
			// echo.  suggest: 135mS  11%
			value = 0;
			voice->echo_amp = 0;
			sscanf(p,"%d %d",&voice->echo_delay,&voice->echo_amp);
			break;

		case 13:   // flutter
			if(sscanf(p,"%d",&value)==1)
				voice->flutter = value * 32;
			break;

		case 14:   // roughness
			if(sscanf(p,"%d",&value)==1)
				voice->roughness = value;
			break;
		}
	}
	if(f_voice != NULL)
		fclose(f_voice);

	for(ix=0; ix<N_PEAKS; ix++)
	{
		voice->freq2[ix] = voice->freq[ix];
		voice->height2[ix] = voice->height[ix];
		voice->width2[ix] = voice->width[ix];
	}

	if(VoicePhonemes(phonemes_name) != 0)
	{
		fprintf(stderr,"Unknown phoneme table: '%s'\n",phonemes_name);
	}
	if(strcmp(new_dictionary,dictionary_name) != 0)
	{
		error = translator->LoadDictionary(new_dictionary);
		if(dictionary_name[0]==0)
			return(NULL);   // no dictionary loaded
	}

	strcpy(voice_name,voicename);
	return(voice);
}

