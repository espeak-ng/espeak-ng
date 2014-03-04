/***************************************************************************
 *   Copyright (C) 2005 to 2014 by Jonathan Duddington                     *
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

#include "stdio.h"
#include "ctype.h"
#include "wctype.h"
#include "string.h"
#include "stdlib.h"
#include "speech.h"

#ifdef PLATFORM_WINDOWS
#include "windows.h"
#else
#ifdef PLATFORM_RISCOS
#include "kernel.h"
#else
#include "dirent.h"
#endif
#endif

#include "speak_lib.h"
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "translate.h"


MNEM_TAB genders [] = {
	{"unknown", 0},
	{"male", 1},
	{"female", 2},
	{NULL, 0 }
};

int tone_points[12] = {600,170, 1200,135, 2000,110, 3000,110, -1,0};
//int tone_points[12] = {250,200,  400,170, 600,170, 1200,135, 2000,110, -1,0};

// limit the rate of change for each formant number
//static int formant_rate_22050[9] = {50, 104, 165, 230, 220, 220, 220, 220, 220};  // values for 22kHz sample rate
//static int formant_rate_22050[9] = {240, 180, 180, 180, 180, 180, 180, 180, 180};  // values for 22kHz sample rate
static int formant_rate_22050[9] = {240, 170, 170, 170, 170, 170, 170, 170, 170};  // values for 22kHz sample rate
int formant_rate[9];         // values adjusted for actual sample rate



#define DEFAULT_LANGUAGE_PRIORITY  5
#define N_VOICES_LIST  250
static int n_voices_list = 0;
static espeak_VOICE *voices_list[N_VOICES_LIST];
static int len_path_voices;

espeak_VOICE current_voice_selected;


enum {
	V_NAME = 1,
	V_LANGUAGE,
	V_GENDER,
	V_TRANSLATOR,
	V_PHONEMES,
	V_DICTIONARY,

// these affect voice quality, are independent of language
	V_FORMANT,
	V_PITCH,
	V_ECHO,
	V_FLUTTER,
	V_ROUGHNESS,
	V_CLARITY,
	V_TONE,
	V_VOICING,
	V_BREATH,
	V_BREATHW,

// these override defaults set by the translator
	V_WORDGAP,
	V_INTONATION,
	V_TUNES,
	V_STRESSLENGTH,
	V_STRESSAMP,
	V_STRESSADD,
	V_DICTRULES,
	V_STRESSRULE,
	V_STRESSOPT,
	V_CHARSET,
	V_NUMBERS,
	V_OPTION,

	V_MBROLA,
	V_KLATT,
	V_FAST,
	V_SPEED,
	V_DICTMIN,
	V_ALPHABET2,
	V_DICTDIALECT,

// these need a phoneme table to have been specified
	V_REPLACE,
	V_CONSONANTS
};



static MNEM_TAB options_tab[] = {
	{"reduce_t",  LOPT_REDUCE_T},
	{"bracket", LOPT_BRACKET_PAUSE},
	{NULL,   -1}
};

static MNEM_TAB keyword_tab[] = {
	{"name",       V_NAME},
	{"language",   V_LANGUAGE},
	{"gender",     V_GENDER},

	{"formant",    V_FORMANT},
	{"pitch",      V_PITCH},
	{"phonemes",   V_PHONEMES},
	{"translator", V_TRANSLATOR},
	{"dictionary", V_DICTIONARY},
	{"stressLength", V_STRESSLENGTH},
	{"stressAmp",  V_STRESSAMP},
	{"stressAdd",  V_STRESSADD},
	{"intonation", V_INTONATION},
	{"tunes",      V_TUNES},
	{"dictrules",	V_DICTRULES},
	{"stressrule", V_STRESSRULE},
	{"stressopt",  V_STRESSOPT},
	{"charset",    V_CHARSET},
	{"replace",    V_REPLACE},
	{"words",      V_WORDGAP},
	{"echo",       V_ECHO},
	{"flutter",    V_FLUTTER},
	{"roughness",  V_ROUGHNESS},
	{"clarity",    V_CLARITY},
	{"tone",       V_TONE},
	{"voicing",    V_VOICING},
	{"breath",     V_BREATH},
	{"breathw",    V_BREATHW},
	{"numbers",    V_NUMBERS},
	{"option",     V_OPTION},
	{"mbrola",     V_MBROLA},
	{"consonants", V_CONSONANTS},
	{"klatt",      V_KLATT},
	{"fast_test2",  V_FAST},
	{"speed",      V_SPEED},
	{"dict_min",   V_DICTMIN},
	{"alphabet2",  V_ALPHABET2},
	{"dictdialect",   V_DICTDIALECT},

	// these just set a value in langopts.param[]
	{"l_dieresis", 0x100+LOPT_DIERESES},
//	{"l_lengthen", 0x100+LOPT_IT_LENGTHEN},
	{"l_prefix",   0x100+LOPT_PREFIXES},
	{"l_regressive_v", 0x100+LOPT_REGRESSIVE_VOICING},
	{"l_unpronouncable", 0x100+LOPT_UNPRONOUNCABLE},
	{"l_sonorant_min", 0x100+LOPT_SONORANT_MIN},
	{"l_length_mods", 0x100+LOPT_LENGTH_MODS},
	{"apostrophe", 0x100+LOPT_APOSTROPHE},
	{NULL,   0}
};

static MNEM_TAB dict_dialects[] = {
	{"en-us",   DICTDIALECT_EN_US},
	{"es-la",   DICTDIALECT_ES_LA},
	{NULL,   0}
};


#define N_VOICE_VARIANTS   12
const char variants_either[N_VOICE_VARIANTS] = {1,2,12,3,13,4,14,5,11,0};
const char variants_male[N_VOICE_VARIANTS] = {1,2,3,4,5,6,0};
const char variants_female[N_VOICE_VARIANTS] = {11,12,13,14,0};
const char *variant_lists[3] = {variants_either, variants_male, variants_female};

static voice_t voicedata;
voice_t *voice = &voicedata;


static char *fgets_strip(char *buf, int size, FILE *f_in)
{//======================================================
// strip trailing spaces, and truncate lines at // comment
	int len;
	char *p;

	if(fgets(buf,size,f_in) == NULL)
		return(NULL);

	if(buf[0] == '#')
	{
		buf[0] = 0;
		return(buf);
	}

	len = strlen(buf);
	while((--len > 0) && isspace(buf[len]))
		buf[len] = 0;

	if((p = strstr(buf,"//")) != NULL)
		*p = 0;

	return(buf);
}


static int LookupTune(const char *name)
{//====================================
	int ix;

	for(ix=0; ix<n_tunes; ix++)
	{
		if(strcmp(name, tunes[ix].name) == 0)
			return(ix);
	}
	return(-1);
}  // end of LookupTune



static void SetToneAdjust(voice_t *voice, int *tone_pts)
{//=====================================================
	int ix;
	int pt;
	int y;
	int freq1=0;
	int freq2;
	int height1 = tone_pts[1];
	int height2;
	double rate;

	for(pt=0; pt<12; pt+=2)
	{
		if(tone_pts[pt] == -1)
		{
			tone_pts[pt] = N_TONE_ADJUST*8;
			if(pt > 0)
				tone_pts[pt+1] = tone_pts[pt-1];
		}
		freq2 = tone_pts[pt] / 8;   // 8Hz steps
		height2 = tone_pts[pt+1];
		if((freq2 - freq1) > 0)
		{
			rate = (double)(height2-height1)/(freq2-freq1);

			for(ix=freq1; ix<freq2; ix++)
			{
				y = height1 + (int)(rate * (ix-freq1));
				if(y > 255)
					y = 255;
				voice->tone_adjust[ix] = y;
			}
		}
		freq1 = freq2;
		height1 = height2;
	}
}


void ReadTonePoints(char *string, int *tone_pts)
{//=============================================
// tone_pts[] is int[12]
	int ix;

	for(ix=0; ix<12; ix++)
		tone_pts[ix] = -1;

	sscanf(string,"%d %d %d %d %d %d %d %d %d %d",
		   &tone_pts[0],&tone_pts[1],&tone_pts[2],&tone_pts[3],
		   &tone_pts[4],&tone_pts[5],&tone_pts[6],&tone_pts[7],
		   &tone_pts[8],&tone_pts[9]);
}




static espeak_VOICE *ReadVoiceFile(FILE *f_in, const char *fname, const char*leafname)
{//===================================================================================
// Read a Voice file, allocate a VOICE_DATA and set data from the
// file's  language, gender, name  lines

	char linebuf[120];
	char vname[80];
	char vgender[80];
	char vlanguage[80];
	char languages[300];  // allow space for several alternate language names and priorities


	unsigned int len;
	int langix = 0;
	int n_languages = 0;
	char *p;
	espeak_VOICE *voice_data;
	int priority;
	int age;
	int n_variants = 4;    // default, number of variants of this voice before using another voice
	int gender;

#ifdef PLATFORM_WINDOWS
	char fname_buf[sizeof(path_home)+15];
	if(memcmp(leafname,"mb-",3) == 0)
	{
		// check whether the mbrola speech data is present for this voice
		memcpy(vname,&leafname[3],3);
		vname[3] = 0;
		sprintf(fname_buf,"%s/mbrola/%s",path_home,vname);

		if(GetFileLength(fname_buf) <= 0)
			return(0);
	}
#endif

	vname[0] = 0;
	vgender[0] = 0;
	age = 0;

	while(fgets_strip(linebuf,sizeof(linebuf),f_in) != NULL)
	{
		if(memcmp(linebuf,"name",4)==0)
		{
			p = &linebuf[4];
			while(isspace(*p)) p++;
			strncpy0(vname,p,sizeof(vname));
		}
		else if(memcmp(linebuf,"language",8)==0)
		{
			priority = DEFAULT_LANGUAGE_PRIORITY;
			vlanguage[0] = 0;

			sscanf(&linebuf[8],"%s %d",vlanguage,&priority);
			len = strlen(vlanguage) + 2;
			// check for space in languages[]
			if(len < (sizeof(languages)-langix-1))
			{
				languages[langix] = priority;

				strcpy(&languages[langix+1],vlanguage);
				langix += len;
				n_languages++;
			}
		}
		else if(memcmp(linebuf,"gender",6)==0)
		{
			sscanf(&linebuf[6],"%s %d",vgender,&age);
		}
		else if(memcmp(linebuf,"variants",8)==0)
		{
			sscanf(&linebuf[8],"%d",&n_variants);
		}
	}
	languages[langix++] = 0;

	gender = LookupMnem(genders,vgender);

	if(n_languages == 0)
	{
		return(NULL);    // no language lines in the voice file
	}

	p = (char *)calloc(sizeof(espeak_VOICE) + langix + strlen(fname) + strlen(vname) + 3, 1);
	voice_data = (espeak_VOICE *)p;
	p = &p[sizeof(espeak_VOICE)];

	memcpy(p,languages,langix);
	voice_data->languages = p;

	strcpy(&p[langix],fname);
	voice_data->identifier = &p[langix];
	voice_data->name = &p[langix];

	if(vname[0] != 0)
	{
		langix += strlen(fname)+1;
		strcpy(&p[langix],vname);
		voice_data->name = &p[langix];
	}

	voice_data->age = age;
	voice_data->gender = gender;
	voice_data->variant = 0;
	voice_data->xx1 = n_variants;
	return(voice_data);
}  // end of ReadVoiceFile




void VoiceReset(int tone_only)
{//===========================
// Set voice to the default values

	int  pk;
	static unsigned char default_heights[N_PEAKS] = {130,128,120,116,100,100,128,128,128};  // changed for v.1.47
	static unsigned char default_widths[N_PEAKS] = {140,128,128,160,171,171,128,128,128};
//	static unsigned char default_heights[N_PEAKS] = {128,128,120,120,110,110,128,128,128};  // previous version
//	static unsigned char default_widths[N_PEAKS] = {128,128,128,160,171,171,128,128,128};

	static int breath_widths[N_PEAKS] = {0,200,200,400,400,400,600,600,600};

	// default is:  pitch 80,118
	voice->pitch_base = 0x47000;
	voice->pitch_range = 4104;

//	default is:  pitch 80,117
//	voice->pitch_base = 0x47000;
//	voice->pitch_range = 3996;

	voice->formant_factor = 256;

	voice->speed_percent = 100;
	voice->echo_delay = 0;
	voice->echo_amp = 0;
	voice->flutter = 64;
	voice->n_harmonic_peaks = 5;
	voice->peak_shape = 0;
	voice->voicing = 64;
	voice->consonant_amp = 90;  // change from 100 to 90 for v.1.47
	voice->consonant_ampv = 100;
	voice->samplerate = samplerate_native;
	memset(voice->klattv,0,sizeof(voice->klattv));

	speed.fast_settings[0] = 450;
	speed.fast_settings[1] = 800;
	speed.fast_settings[2] = 175;

#ifdef PLATFORM_RISCOS
	voice->roughness = 1;
#else
	voice->roughness = 2;
#endif

	InitBreath();
	for(pk=0; pk<N_PEAKS; pk++)
	{
		voice->freq[pk] = 256;
		voice->height[pk] = default_heights[pk]*2;
		voice->width[pk] = default_widths[pk]*2;
		voice->breath[pk] = 0;
		voice->breathw[pk] = breath_widths[pk];  // default breath formant woidths
		voice->freqadd[pk] = 0;

		// adjust formant smoothing depending on sample rate
		formant_rate[pk] = (formant_rate_22050[pk] * 22050)/samplerate;
	}

	// This table provides the opportunity for tone control.
	// Adjustment of harmonic amplitudes, steps of 8Hz
	// value of 128 means no change
//	memset(voice->tone_adjust,128,sizeof(voice->tone_adjust));
	SetToneAdjust(voice,tone_points);

	// default values of speed factors
	voice->speedf1 = 256;
	voice->speedf2 = 238;
	voice->speedf3 = 232;

	if(tone_only == 0)
	{
		n_replace_phonemes = 0;
		option_quiet = 0;
		LoadMbrolaTable(NULL,NULL,0);
	}
}  // end of VoiceReset


static void VoiceFormant(char *p)
{//==============================
	// Set parameters for a formant
	int ix;
	int formant;
	int freq = 100;
	int height = 100;
	int width = 100;
	int freqadd = 0;

	ix = sscanf(p,"%d %d %d %d %d",&formant,&freq,&height,&width,&freqadd);
	if(ix < 2)
		return;

	if((formant < 0) || (formant > 8))
		return;

	if(freq >= 0)
		voice->freq[formant] = (int)(freq * 2.56001);
	if(height >= 0)
		voice->height[formant] = (int)(height * 2.56001);
	if(width >= 0)
		voice->width[formant] = (int)(width * 2.56001);
	voice->freqadd[formant] = freqadd;
}





static void PhonemeReplacement(int type, char *p)
{//==============================================
	int n;
	int  phon;
	int flags = 0;
	char phon_string1[12];
	char phon_string2[12];

	strcpy(phon_string2,"NULL");
	n = sscanf(p,"%d %s %s",&flags,phon_string1,phon_string2);
	if((n < 2) || (n_replace_phonemes >= N_REPLACE_PHONEMES))
		return;

	if((phon = LookupPhonemeString(phon_string1)) == 0)
		return;  // not recognised

	replace_phonemes[n_replace_phonemes].old_ph = phon;
	replace_phonemes[n_replace_phonemes].new_ph = LookupPhonemeString(phon_string2);
	replace_phonemes[n_replace_phonemes++].type = flags;
}  //  end of PhonemeReplacement



static int Read8Numbers(char *data_in,int *data)
{//=============================================
// Read 8 integer numbers
	memset(data, 0, 8+sizeof(int));
	return(sscanf(data_in,"%d %d %d %d %d %d %d %d",
				  &data[0],&data[1],&data[2],&data[3],&data[4],&data[5],&data[6],&data[7]));
}


static unsigned int StringToWord2(const char *string)
{//======================================================
// Convert a language name string to a word such as L('e','n')
	int ix;
	int c;
	unsigned int value = 0;

	for(ix=0; (ix<4) && ((c = string[ix]) != 0); ix++)
	{
		value = (value << 8) | (c & 0xff);
	}
	return(value);
}


voice_t *LoadVoice(const char *vname, int control)
{//===============================================
// control, bit 0  1= no_default
//          bit 1  1 = change tone only, not language
//          bit 2  1 = don't report error on LoadDictionary
//          bit 4  1 = vname = full path

	FILE *f_voice = NULL;
	char *p;
	int  key;
	int  ix;
	int  n;
	int  value;
	int  value2;
	int  langix = 0;
	int  tone_only = control & 2;
	int  language_set = 0;
	int  phonemes_set = 0;
	int  stress_amps_set = 0;
	int  stress_lengths_set = 0;
	int  stress_add_set = 0;
	int  conditional_rules = 0;
	LANGUAGE_OPTIONS *langopts = NULL;

	Translator *new_translator = NULL;

	char voicename[40];
	char language_name[40];
	char translator_name[40];
	char new_dictionary[40];
	char phonemes_name[40];
	char option_name[40];
	const char *language_type;
	char buf[sizeof(path_home)+30];
	char path_voices[sizeof(path_home)+12];

	int dict_min = 0;
	int stress_amps[8];
	int stress_lengths[8];
	int stress_add[8];
	char names[8][40];
	char name1[40];
	char name2[80];
	const char *voice_dir;

	int pitch1;
	int pitch2;

	static char voice_identifier[40];  // file name for  current_voice_selected
	static char voice_name[40];        // voice name for current_voice_selected
	static char voice_languages[100];  // list of languages and priorities for current_voice_selected

	// which directory to look for a named voice. List of voice names, must end in a space.
	static const char *voices_asia =
		"az bn fa fa-pin hi hy hy-west id ka kn ku ml ms ne pa ta te tr vi vi-hue vi-sgn zh zh-yue ";
	static const char *voices_europe =
		"an bg bs ca cs cy da de el en en-us es et fi fr fr-be ga hr hu is it lt lv mk nl no pl pt-pt ro ru sk sq sr sv ";


	strncpy0(voicename, vname, sizeof(voicename));
	if(control & 0x10)
	{
		strcpy(buf,vname);
		if(GetFileLength(buf) <= 0)
			return(NULL);
	}
	else
	{
		if(voicename[0]==0)
			strcpy(voicename,"default");

		sprintf(path_voices,"%s%cvoices%c",path_home,PATHSEP,PATHSEP);
		sprintf(buf,"%s%s",path_voices,voicename);  // first, look in the main voices directory

		if(GetFileLength(buf) <= 0)
		{
			// then look in the appropriate subdirectory
			if((voicename[0]=='m') && (voicename[1]=='b'))
			{
				voice_dir = "mb";   // mbrola voices
			}
			else
			{
				sprintf(name2, "%s ", voicename);
				if(strstr(voices_europe, voicename) != NULL)
					voice_dir = "europe";
				else if(strstr(voices_asia, voicename) != NULL)
					voice_dir = "asia";
				else
					voice_dir = "other";

				sprintf(buf,"%s%s%c%s", path_voices,voice_dir,PATHSEP,voicename);

				if(GetFileLength(buf) <= 0)
				{
					// if not found, look in "test" sub-directory
					sprintf(buf,"%stest%c%s",path_voices,PATHSEP,voicename);
				}
			}
		}
	}

	f_voice = fopen(buf,"r");

	language_type = "en";    // default
	if(f_voice == NULL)
	{
		if(control & 3)
			return(NULL);  // can't open file

		if(SelectPhonemeTableName(voicename) >= 0)
			language_type = voicename;
	}

	if(!tone_only && (translator != NULL))
	{
		DeleteTranslator(translator);
		translator = NULL;
	}

	strcpy(translator_name,language_type);
	strcpy(new_dictionary,language_type);
	strcpy(phonemes_name,language_type);


	if(!tone_only)
	{
		voice = &voicedata;
		strncpy0(voice_identifier,vname,sizeof(voice_identifier));
		voice_name[0] = 0;
		voice_languages[0] = 0;

		current_voice_selected.identifier = voice_identifier;
		current_voice_selected.name = voice_name;
		current_voice_selected.languages = voice_languages;
	}
	else
	{
		// append the variant file name to the voice identifier
		if((p = strchr(voice_identifier,'+')) != NULL)
			*p = 0;    // remove previous variant name
		sprintf(buf,"+%s",&vname[3]);    // omit  !v/  from the variant filename
		strcat(voice_identifier,buf);
		langopts = &translator->langopts;
	}
	VoiceReset(tone_only);

	if(!tone_only)
		SelectPhonemeTableName(phonemes_name);  // set up phoneme_tab


	while((f_voice != NULL) && (fgets_strip(buf,sizeof(buf),f_voice) != NULL))
	{
		// isolate the attribute name
		for(p=buf; (*p != 0) && !isspace(*p); p++);
		*p++ = 0;

		if(buf[0] == 0) continue;

		key = LookupMnem(keyword_tab, buf);

		switch(key)
		{
		case V_LANGUAGE:
		{
			unsigned int len;
			int priority;

			if(tone_only)
				break;

			priority = DEFAULT_LANGUAGE_PRIORITY;
			language_name[0] = 0;

			sscanf(p,"%s %d",language_name,&priority);
			if(strcmp(language_name,"variant") == 0)
				break;

			len = strlen(language_name) + 2;
			// check for space in languages[]
			if(len < (sizeof(voice_languages)-langix-1))
			{
				voice_languages[langix] = priority;

				strcpy(&voice_languages[langix+1],language_name);
				langix += len;
			}

			// only act on the first language line
			if(language_set == 0)
			{
				language_type = strtok(language_name,"-");
				language_set = 1;
				strcpy(translator_name,language_type);
				strcpy(new_dictionary,language_type);
				strcpy(phonemes_name,language_type);
				SelectPhonemeTableName(phonemes_name);

				if(new_translator != NULL)
					DeleteTranslator(new_translator);

				new_translator = SelectTranslator(translator_name);
				langopts = &new_translator->langopts;
				strncpy0(voice->language_name, language_name, sizeof(voice->language_name));
			}
		}
		break;

		case V_NAME:
			if(tone_only == 0)
			{
				while(isspace(*p)) p++;
				strncpy0(voice_name,p,sizeof(voice_name));
			}
			break;

		case V_GENDER:
		{
			int age = 0;
			char vgender[80];
			sscanf(p,"%s %d",vgender,&age);
			current_voice_selected.gender = LookupMnem(genders,vgender);
			current_voice_selected.age = age;
		}
		break;

		case V_TRANSLATOR:
			if(tone_only) break;

			sscanf(p,"%s",translator_name);

			if(new_translator != NULL)
				DeleteTranslator(new_translator);

			new_translator = SelectTranslator(translator_name);
			langopts = &new_translator->langopts;
			break;

		case V_DICTIONARY:        // dictionary
			sscanf(p,"%s",new_dictionary);
			break;

		case V_PHONEMES:        // phoneme table
			sscanf(p,"%s",phonemes_name);
			break;

		case V_FORMANT:
			VoiceFormant(p);
			break;

		case V_PITCH:
		{
			double factor;
			// default is  pitch 82 118
			n = sscanf(p,"%d %d",&pitch1,&pitch2);
			voice->pitch_base = (pitch1 - 9) << 12;
			voice->pitch_range = (pitch2 - pitch1) * 108;
			factor = (double)(pitch1 - 82)/82;
			voice->formant_factor = (int)((1+factor/4) * 256);  // nominal formant shift for a different voice pitch
		}
		break;

		case V_STRESSLENGTH:   // stressLength
			stress_lengths_set = Read8Numbers(p,stress_lengths);
			break;

		case V_STRESSAMP:   // stressAmp
			stress_amps_set = Read8Numbers(p,stress_amps);
			break;

		case V_STRESSADD:   // stressAdd
			stress_add_set = Read8Numbers(p,stress_add);
			break;

		case V_INTONATION:   // intonation
			sscanf(p,"%d %d",&option_tone_flags,&option_tone2);
			if((option_tone_flags & 0xff) != 0)
				langopts->intonation_group = option_tone_flags & 0xff;
			break;

		case V_TUNES:
			n = sscanf(p,"%s %s %s %s %s %s",names[0],names[1],names[2],names[3],names[4],names[5]);
			langopts->intonation_group = 0;
			for(ix=0; ix<n; ix++)
			{
				if(strcmp(names[ix],"NULL")==0)
					continue;

				if((value = LookupTune(names[ix])) < 0)
					fprintf(stderr,"Unknown tune '%s'\n",names[ix]);
				else
					langopts->tunes[ix] = value;
			}
			break;

		case V_DICTRULES:   // conditional dictionary rules and list entries
		case V_NUMBERS:
		case V_STRESSOPT:
			// expect a list of numbers
			while(*p != 0)
			{
				while(isspace(*p)) p++;
				n = -1;
				if((n = atoi(p)) > 0)
				{
					p++;
					if(n < 32)
					{
						if(key==V_DICTRULES)
							conditional_rules |= (1 << n);
						else if(key==V_NUMBERS)
							langopts->numbers |= (1 << n);
						else if(key==V_STRESSOPT)
							langopts->stress_flags |= (1 << n);
					}
					else
					{
						if((key==V_NUMBERS) && (n < 64))
							langopts->numbers |= (1 << (n-32));
						else
							fprintf(stderr,"Bad option number %d\n", n);
					}
				}
				while(isalnum(*p)) p++;
			}
			ProcessLanguageOptions(langopts);
			break;

		case V_REPLACE:
			if(phonemes_set == 0)
			{
				// must set up a phoneme table before we can lookup phoneme mnemonics
				SelectPhonemeTableName(phonemes_name);
				phonemes_set = 1;
			}
			PhonemeReplacement(key,p);
			break;

		case V_WORDGAP:   // words
			sscanf(p,"%d %d",&langopts->word_gap, &langopts->vowel_pause);
			break;

		case V_STRESSRULE:
			sscanf(p,"%d %d %d %d",&langopts->stress_rule,
				   &langopts->stress_flags,
				   &langopts->unstressed_wd1,
				   &langopts->unstressed_wd2);
			break;

		case V_CHARSET:
			if((sscanf(p,"%d",&value)==1) && (value < N_CHARSETS))
				new_translator->charset_a0 = charsets[value];
			break;

		case V_OPTION:
			value2 = 0;
			if(((sscanf(p,"%s %d %d",option_name,&value,&value2) >= 2) && ((ix = LookupMnem(options_tab, option_name)) >= 0)) ||
				((sscanf(p,"%d %d %d",&ix,&value,&value2) >= 2) && (ix < N_LOPTS)))
			{
				langopts->param[ix] = value;
				langopts->param2[ix] = value2;
			}
			else
			{
				fprintf(stderr,"Bad voice option: %s %s\n",buf,p);
			}
			break;

		case V_ECHO:
			// echo.  suggest: 135mS  11%
			value = 0;
			voice->echo_amp = 0;
			sscanf(p,"%d %d",&voice->echo_delay,&voice->echo_amp);
			break;

		case V_FLUTTER:   // flutter
			if(sscanf(p,"%d",&value)==1)
				voice->flutter = value * 32;
			break;

		case V_ROUGHNESS:   // roughness
			if(sscanf(p,"%d",&value)==1)
				voice->roughness = value;
			break;

		case V_CLARITY:  // formantshape
			if(sscanf(p,"%d",&value)==1)
			{
				if(value > 4)
				{
					voice->peak_shape = 1;  // squarer formant peaks
					value = 4;
				}
				voice->n_harmonic_peaks = 1+value;
			}
			break;

		case V_TONE:
		{
			int tone_data[12];
			ReadTonePoints(p,tone_data);
			SetToneAdjust(voice,tone_data);
		}
		break;

		case V_VOICING:
			if(sscanf(p,"%d",&value)==1)
				voice->voicing = (value * 64)/100;
			break;

		case V_BREATH:
			voice->breath[0] = Read8Numbers(p,&voice->breath[1]);
			for(ix=1; ix<8; ix++)
			{
				if(ix % 2)
					voice->breath[ix] = -voice->breath[ix];
			}
			break;

		case V_BREATHW:
			voice->breathw[0] = Read8Numbers(p,&voice->breathw[1]);
			break;

		case V_CONSONANTS:
			value = sscanf(p,"%d %d",&voice->consonant_amp, &voice->consonant_ampv);
			break;

		case V_SPEED:
			sscanf(p,"%d",&voice->speed_percent);
			break;

		case V_MBROLA:
		{
			int srate = 16000;

			name2[0] = 0;
			sscanf(p,"%s %s %d",name1,name2,&srate);
			if(LoadMbrolaTable(name1,name2,srate) != EE_OK)
			{
				fprintf(stderr,"mbrola voice not found\n");
			}
			voice->samplerate = srate;
		}
		break;

		case V_KLATT:
			voice->klattv[0] = 1;  // default source: IMPULSIVE
			Read8Numbers(p,voice->klattv);
			voice->klattv[KLATT_Kopen] -= 40;
			break;

		case V_FAST:
			Read8Numbers(p,speed.fast_settings);
			SetSpeed(3);
			break;

		case V_DICTMIN:
			sscanf(p,"%d",&dict_min);
			break;

		case V_ALPHABET2:
			{
				ALPHABET *alphabet;
				name1[0] = name2[0] = 0;
				sscanf(p, "%s %s", name1, name2);

				if(strcmp(name1, "latin") == 0)
				{
					strncpy0(langopts->ascii_language,name2,sizeof(langopts->ascii_language));
				}
				else if((alphabet = AlphabetFromName(name1)) != 0)
				{
					langopts->alt_alphabet = alphabet->offset;
					langopts->alt_alphabet_lang = StringToWord2(name2);
				}
				else
				{
					fprintf(stderr,"alphabet name '%s' not found\n", name1);
				}
			}
			break;

		case V_DICTDIALECT:
			// specify a dialect to use for foreign words, eg, en-us for _^_EN
			if(sscanf(p, "%s", name1) == 1)
			{
				if((ix = LookupMnem(dict_dialects, name1)) > 0)
				{
					langopts->dict_dialect |= (1 << ix);
				}
				else
				{
					fprintf(stderr, "dictdialect name '%s' not recognized\n", name1);
				}
			}
			break;

		default:
			if((key & 0xff00) == 0x100)
			{
				sscanf(p,"%d",&langopts->param[key &0xff]);
			}
			else
			{
				fprintf(stderr,"Bad voice attribute: %s\n",buf);
			}
			break;
		}
	}
	if(f_voice != NULL)
		fclose(f_voice);

	if((new_translator == NULL) && (!tone_only))
	{
		// not set by language attribute
		new_translator = SelectTranslator(translator_name);
	}

	SetSpeed(3);   // for speed_percent

	for(ix=0; ix<N_PEAKS; ix++)
	{
		voice->freq2[ix] = voice->freq[ix];
		voice->height2[ix] = voice->height[ix];
		voice->width2[ix] = voice->width[ix];
	}

	if(tone_only)
	{
		new_translator = translator;
	}
	else
	{
		if((ix = SelectPhonemeTableName(phonemes_name)) < 0)
		{
			fprintf(stderr,"Unknown phoneme table: '%s'\n",phonemes_name);
			ix = 0;
		}
		voice->phoneme_tab_ix = ix;
		new_translator->phoneme_tab_ix = ix;
		new_translator->dict_min_size = dict_min;
		LoadDictionary(new_translator, new_dictionary, control & 4);
		if(dictionary_name[0]==0)
			return(NULL);   // no dictionary loaded

		new_translator->dict_condition = conditional_rules;

		voice_languages[langix] = 0;
	}

	langopts = &new_translator->langopts;


	if((value = langopts->param[LOPT_LENGTH_MODS]) != 0)
	{
		SetLengthMods(new_translator,value);
	}

	voice->width[0] = (voice->width[0] * 105)/100;

	if(!tone_only)
	{
		translator = new_translator;
	}


	// relative lengths of different stress syllables
	for(ix=0; ix<stress_lengths_set; ix++)
	{
		translator->stress_lengths[ix] = stress_lengths[ix];
	}
	for(ix=0; ix<stress_add_set; ix++)
	{
		translator->stress_lengths[ix] += stress_add[ix];
	}
	for(ix=0; ix<stress_amps_set; ix++)
	{
		translator->stress_amps[ix] = stress_amps[ix];
		translator->stress_amps_r[ix] = stress_amps[ix] -1;
	}

	return(voice);
}  //  end of LoadVoice


static char *ExtractVoiceVariantName(char *vname, int variant_num, int add_dir)
{//===========================================================================
// Remove any voice variant suffix (name or number) from a voice name
// Returns the voice variant name

	char *p;
	static char variant_name[40];
	char variant_prefix[5];

	variant_name[0] = 0;
	sprintf(variant_prefix,"!v%c",PATHSEP);
	if(add_dir == 0)
		variant_prefix[0] = 0;

	if(vname != NULL)
	{
		if((p = strchr(vname,'+')) != NULL)
		{
			// The voice name has a +variant suffix
			variant_num = 0;
			*p++ = 0;   // delete the suffix from the voice name
			if(IsDigit09(*p))
			{
				variant_num = atoi(p);  // variant number
			}
			else
			{
				// voice variant name, not number
				sprintf(variant_name, "%s%s", variant_prefix, p);
			}
		}
	}

	if(variant_num > 0)
	{
		if(variant_num < 10)
			sprintf(variant_name,"%sm%d",variant_prefix, variant_num);  // male
		else
			sprintf(variant_name,"%sf%d",variant_prefix, variant_num-10);  // female
	}

	return(variant_name);
}  //  end of ExtractVoiceVariantName



voice_t *LoadVoiceVariant(const char *vname, int variant_num)
{//==========================================================
// Load a voice file.
// Also apply a voice variant if specified by "variant", or by "+number" or "+name" in the "vname"

	voice_t *v;
	char *variant_name;
	char buf[60];

	strncpy0(buf,vname,sizeof(buf));
	variant_name = ExtractVoiceVariantName(buf,variant_num, 1);

	if((v = LoadVoice(buf,0)) == NULL)
		return(NULL);

	if(variant_name[0] != 0)
	{
		v = LoadVoice(variant_name,2);
	}
	return(v);
}



static int __cdecl VoiceNameSorter(const void *p1, const void *p2)
{//=======================================================
	int ix;
	espeak_VOICE *v1 = *(espeak_VOICE **)p1;
	espeak_VOICE *v2 = *(espeak_VOICE **)p2;


	if((ix = strcmp(&v1->languages[1],&v2->languages[1])) != 0)  // primary language name
		return(ix);
	if((ix = v1->languages[0] - v2->languages[0]) != 0)  // priority number
		return(ix);
	return(strcmp(v1->name,v2->name));
}


static int __cdecl VoiceScoreSorter(const void *p1, const void *p2)
{//========================================================
	int ix;
	espeak_VOICE *v1 = *(espeak_VOICE **)p1;
	espeak_VOICE *v2 = *(espeak_VOICE **)p2;

	if((ix = v2->score - v1->score) != 0)
		return(ix);
	return(strcmp(v1->name,v2->name));
}


static int ScoreVoice(espeak_VOICE *voice_spec, const char *spec_language, int spec_n_parts, int spec_lang_len, espeak_VOICE *voice)
{//=========================================================================================================================
	int ix;
	const char *p;
	int c1, c2;
	int language_priority;
	int n_parts;
	int matching;
	int matching_parts;
	int score = 0;
	int x;
	int ratio;
	int required_age;
	int diff;

	p = voice->languages;  // list of languages+dialects for which this voice is suitable

	if(spec_n_parts < 0)
	{
		// match on the subdirectory
		if(memcmp(voice->identifier, spec_language, spec_lang_len) == 0)
			return(100);
		return(0);
	}

	if(spec_n_parts == 0)
	{
		score = 100;
	}
	else
	{
		if((*p == 0) && (strcmp(spec_language,"variants")==0))
		{
			// match on a voice with no languages if the required language is "variants"
			score = 100;
		}

		// compare the required language with each of the languages of this voice
		while(*p != 0)
		{
			language_priority = *p++;

			matching = 1;
			matching_parts = 0;
			n_parts = 1;

			for(ix=0; ; ix++)
			{
				if((ix >= spec_lang_len) || ((c1 = spec_language[ix]) == '-'))
					c1 = 0;
				if((c2 = p[ix]) == '-')
					c2 = 0;

				if(c1 != c2)
				{
					matching = 0;
				}

				if(p[ix] == '-')
				{
					n_parts++;
					if(matching)
						matching_parts++;
				}
				if(p[ix] == 0)
					break;
			}
			p += (ix+1);
			matching_parts += matching;  // number of parts which match

			if(matching_parts == 0)
				continue;   // no matching parts for this language

			x = 5;
			// reduce the score if not all parts of the required language match
			if((diff = (spec_n_parts - matching_parts)) > 0)
				x -= diff;

			// reduce score if the language is more specific than required
			if((diff = (n_parts - matching_parts)) > 0)
				x -= diff;

			x = x*100 - (language_priority * 2);

			if(x > score)
				score = x;
		}
	}
	if(score == 0)
		return(0);

	if(voice_spec->name != NULL)
	{
		if(strcmp(voice_spec->name,voice->name)==0)
		{
			// match on voice name
			score += 500;
		}
		else if(strcmp(voice_spec->name,voice->identifier)==0)
		{
			score += 400;
		}
	}

	if(((voice_spec->gender == 1) || (voice_spec->gender == 2)) &&
			((voice->gender == 1) || (voice->gender == 2)))
	{
		if(voice_spec->gender == voice->gender)
			score += 50;
		else
			score -= 50;
	}

	if((voice_spec->age <= 12) && (voice->gender == 2) && (voice->age > 12))
	{
		score += 5;  // give some preference for non-child female voice if a child is requested
	}

	if(voice->age != 0)
	{
		if(voice_spec->age == 0)
			required_age = 30;
		else
			required_age = voice_spec->age;

		ratio = (required_age*100)/voice->age;
		if(ratio < 100)
			ratio = 10000/ratio;
		ratio = (ratio - 100)/10;    // 0=exact match, 10=out by factor of 2
		x = 5 - ratio;
		if(x > 0) x = 0;

		score = score + x;

		if(voice_spec->age > 0)
			score += 10;    // required age specified, favour voices with a specified age (near it)
	}
	if(score < 1)
		score = 1;
	return(score);
}  // end of ScoreVoice


static int SetVoiceScores(espeak_VOICE *voice_select, espeak_VOICE **voices, int control)
{//======================================================================================
// control: bit0=1  include mbrola voices
	int ix;
	int score;
	int nv;           // number of candidates
	int n_parts=0;
	int lang_len=0;
	espeak_VOICE *vp;
	char language[80];
	char buf[sizeof(path_home)+80];

	// count number of parts in the specified language
	if((voice_select->languages != NULL) && (voice_select->languages[0] != 0))
	{
		n_parts = 1;
		lang_len = strlen(voice_select->languages);
		for(ix=0; (ix<=lang_len) && ((unsigned)ix < sizeof(language)); ix++)
		{
			if((language[ix] = tolower(voice_select->languages[ix])) == '-')
				n_parts++;
		}
	}

	if((n_parts == 1) && (control & 1))
	{
		if(strcmp(language, "mbrola") == 0)
		{
			language[2] = 0;  // truncate to "mb"
			lang_len = 2;
		}

		sprintf(buf, "%s/voices/%s", path_home, language);
		if(GetFileLength(buf) == -2)
		{
			// A subdirectory name has been specified.  List all the voices in that subdirectory
			language[lang_len++] = PATHSEP;
			language[lang_len] = 0;
			n_parts = -1;
		}

	}

	// select those voices which match the specified language
	nv = 0;
	for(ix=0; ix<n_voices_list; ix++)
	{
		vp = voices_list[ix];

		if(((control & 1) == 0) && (memcmp(vp->identifier,"mb/",3) == 0))
			continue;

		if((score = ScoreVoice(voice_select, language, n_parts, lang_len, voices_list[ix])) > 0)
		{
			voices[nv++] = vp;
			vp->score = score;
		}
	}
	voices[nv] = NULL;  // list terminator

	if(nv==0)
		return(0);

	// sort the selected voices by their score
	qsort(voices,nv,sizeof(espeak_VOICE *),(int (__cdecl *)(const void *,const void *))VoiceScoreSorter);

	return(nv);
}  // end of SetVoiceScores




espeak_VOICE *SelectVoiceByName(espeak_VOICE **voices, const char *name2)
{//======================================================================
	int ix;
	int match_fname = -1;
	int match_fname2 = -1;
	int match_name = -1;
	const char *id;   // this is the filename within espeak-data/voices
	char *variant_name;
	int last_part_len;
	char last_part[41];
	char name[40];

	if(voices == NULL)
	{
		if(n_voices_list == 0)
			espeak_ListVoices(NULL);   // create the voices list
		voices = voices_list;
	}

	strncpy0(name, name2, sizeof(name));
	if((variant_name = strchr(name, '+')) != NULL)
	{
		*variant_name = 0;
		variant_name++;
	}

	sprintf(last_part,"%c%s",PATHSEP,name);
	last_part_len = strlen(last_part);

	for(ix=0; voices[ix] != NULL; ix++)
	{
		if(strcmp(name,voices[ix]->name)==0)
		{
			match_name = ix;   // found matching voice name
			break;
		}
		else
		{
			id = voices[ix]->identifier;
			if(strcmp(name, id)==0)
			{
				match_fname = ix;  // matching identifier, use this if no matching name
			}
			else if(strcmp(last_part,&id[strlen(id)-last_part_len])==0)
			{
				match_fname2 = ix;
			}
		}
	}

	if(match_name < 0)
	{
		match_name = match_fname;  // no matching name, try matching filename
		if(match_name < 0)
			match_name = match_fname2;  // try matching just the last part of the filename
	}

	if(match_name < 0)
		return(NULL);

	return(voices[match_name]);
}  //  end of SelectVoiceByName




char const *SelectVoice(espeak_VOICE *voice_select, int *found)
{//============================================================
// Returns a path within espeak-voices, with a possible +variant suffix
// variant is an output-only parameter
	int nv;           // number of candidates
	int ix, ix2;
	int j;
	int n_variants;
	int variant_number;
	int gender;
	int skip;
	int aged=1;
	char *variant_name;
	const char *p, *p_start;
	espeak_VOICE *vp = NULL;
	espeak_VOICE *vp2;
	espeak_VOICE voice_select2;
	espeak_VOICE *voices[N_VOICES_LIST]; // list of candidates
	espeak_VOICE *voices2[N_VOICES_LIST+N_VOICE_VARIANTS];
	static espeak_VOICE voice_variants[N_VOICE_VARIANTS];
	static char voice_id[50];

	*found = 1;
	memcpy(&voice_select2,voice_select,sizeof(voice_select2));

	if(n_voices_list == 0)
		espeak_ListVoices(NULL);   // create the voices list

	if((voice_select2.languages == NULL) || (voice_select2.languages[0] == 0))
	{
		// no language is specified. Get language from the named voice
		static char buf[60];

		if(voice_select2.name == NULL)
		{
			if((voice_select2.name = voice_select2.identifier) == NULL)
				voice_select2.name = "default";
		}

		strncpy0(buf,voice_select2.name,sizeof(buf));
		variant_name = ExtractVoiceVariantName(buf,0,0);

		vp = SelectVoiceByName(voices_list,buf);
		if(vp != NULL)
		{
			voice_select2.languages = &(vp->languages[1]);

			if((voice_select2.gender==0) && (voice_select2.age==0) && (voice_select2.variant==0))
			{
				if(variant_name[0] != 0)
				{
					sprintf(voice_id,"%s+%s", vp->identifier, variant_name);
					return(voice_id);
				}

				return(vp->identifier);
			}
		}
	}

	// select and sort voices for the required language
	nv = SetVoiceScores(&voice_select2,voices,0);

	if(nv == 0)
	{
		// no matching voice, choose the default
		*found = 0;
		if((voices[0] = SelectVoiceByName(voices_list,"default")) != NULL)
			nv = 1;
	}

	gender = 0;
	if((voice_select2.gender == 2) || ((voice_select2.age > 0) && (voice_select2.age < 13)))
		gender = 2;
	else if(voice_select2.gender == 1)
		gender = 1;

#define AGE_OLD  60
	if(voice_select2.age < AGE_OLD)
		aged = 0;

	p = p_start = variant_lists[gender];
	if(aged == 0)
		p++;   // the first voice in the variants list is older

	// add variants for the top voices
	n_variants = 0;
	for(ix=0, ix2=0; ix<nv; ix++)
	{
		vp = voices[ix];
		// is the main voice the required gender?
		skip=0;

		if((gender != 0) && (vp->gender != gender))
		{
			skip=1;
		}
		if((ix2==0) && aged && (vp->age < AGE_OLD))
		{
			skip=1;
		}

		if(skip==0)
		{
			voices2[ix2++] = vp;
		}

		for(j=0; (j < vp->xx1) && (n_variants < N_VOICE_VARIANTS);)
		{
			if((variant_number = *p) == 0)
			{
				p = p_start;
				continue;
			}

			vp2 = &voice_variants[n_variants++];        // allocate space for voice variant
			memcpy(vp2,vp,sizeof(espeak_VOICE));        // copy from the original voice
			vp2->variant = variant_number;
			voices2[ix2++] = vp2;
			p++;
			j++;
		}
	}
	// add any more variants to the end of the list
	while((vp != NULL) && ((variant_number = *p++) != 0) && (n_variants < N_VOICE_VARIANTS))
	{
		vp2 = &voice_variants[n_variants++];        // allocate space for voice variant
		memcpy(vp2,vp,sizeof(espeak_VOICE));        // copy from the original voice
		vp2->variant = variant_number;
		voices2[ix2++] = vp2;
	}

	// index the sorted list by the required variant number
	if(ix2 == 0)
		return(NULL);
	vp = voices2[voice_select2.variant % ix2];

	if(vp->variant != 0)
	{
		variant_name = ExtractVoiceVariantName(NULL, vp->variant, 0);
		sprintf(voice_id,"%s+%s", vp->identifier, variant_name);
		return(voice_id);
	}

	return(vp->identifier);
}  //  end of SelectVoice



static void GetVoices(const char *path)
{//====================================
	FILE *f_voice;
	espeak_VOICE *voice_data;
	int ftype;
	char fname[sizeof(path_home)+100];

#ifdef PLATFORM_RISCOS
	int len;
	int *type;
	char *p;
	_kernel_swi_regs regs;
	_kernel_oserror *error;
	char buf[80];
	char directory2[sizeof(path_home)+100];

	regs.r[0] = 10;
	regs.r[1] = (int)path;
	regs.r[2] = (int)buf;
	regs.r[3] = 1;
	regs.r[4] = 0;
	regs.r[5] = sizeof(buf);
	regs.r[6] = 0;

	while(regs.r[3] > 0)
	{
		error = _kernel_swi(0x0c+0x20000,&regs,&regs);      /* OS_GBPB 10, read directory entries */
		if((error != NULL) || (regs.r[3] == 0))
		{
			break;
		}
		type = (int *)(&buf[16]);
		len = strlen(&buf[20]);
		sprintf(fname,"%s.%s",path,&buf[20]);

		if(*type == 2)
		{
			// a sub-directory
			GetVoices(fname);
		}
		else
		{
			// a regular line, add it to the voices list
			if((f_voice = fopen(fname,"r")) == NULL)
				continue;

			// pass voice file name within the voices directory
			voice_data = ReadVoiceFile(f_voice, fname+len_path_voices, &buf[20]);
			fclose(f_voice);

			if(voice_data != NULL)
			{
				voices_list[n_voices_list++] = voice_data;
			}
		}
	}
#else
#ifdef PLATFORM_WINDOWS
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;

#undef UNICODE         // we need FindFirstFileA() which takes an 8-bit c-string
	sprintf(fname,"%s\\*",path);
	hFind = FindFirstFileA(fname, &FindFileData);
	if(hFind == INVALID_HANDLE_VALUE)
		return;

	do {
		if(n_voices_list >= (N_VOICES_LIST-2))
			break;   // voices list is full

		if(FindFileData.cFileName[0] != '.')
		{
			sprintf(fname,"%s%c%s",path,PATHSEP,FindFileData.cFileName);
			ftype = GetFileLength(fname);

			if(ftype == -2)
			{
				// a sub-sirectory
				GetVoices(fname);
			}
			else if(ftype > 0)
			{
				// a regular line, add it to the voices list
				if((f_voice = fopen(fname,"r")) == NULL)
					continue;

				// pass voice file name within the voices directory
				voice_data = ReadVoiceFile(f_voice, fname+len_path_voices, FindFileData.cFileName);
				fclose(f_voice);

				if(voice_data != NULL)
				{
					voices_list[n_voices_list++] = voice_data;
				}
			}
		}
	} while(FindNextFileA(hFind, &FindFileData) != 0);
	FindClose(hFind);

#else
	DIR *dir;
	struct dirent *ent;

	if((dir = opendir((char *)path)) == NULL)    // note: (char *) is needed for WINCE
		return;

	while((ent = readdir(dir)) != NULL)
	{
		if(n_voices_list >= (N_VOICES_LIST-2))
			break;   // voices list is full

		if(ent->d_name[0] == '.')
			continue;

		sprintf(fname,"%s%c%s",path,PATHSEP,ent->d_name);

		ftype = GetFileLength(fname);

		if(ftype == -2)
		{
			// a sub-sirectory
			GetVoices(fname);
		}
		else if(ftype > 0)
		{
			// a regular line, add it to the voices list
			if((f_voice = fopen(fname,"r")) == NULL)
				continue;

			// pass voice file name within the voices directory
			voice_data = ReadVoiceFile(f_voice, fname+len_path_voices, ent->d_name);
			fclose(f_voice);

			if(voice_data != NULL)
			{
				voices_list[n_voices_list++] = voice_data;
			}
		}
	}
	closedir(dir);
#endif
#endif
}   // end of GetVoices



espeak_ERROR SetVoiceByName(const char *name)
{//=========================================
	espeak_VOICE *v;
	int ix;
	espeak_VOICE voice_selector;
	char *variant_name;
	static char buf[60];

	strncpy0(buf,name,sizeof(buf));

	variant_name = ExtractVoiceVariantName(buf, 0, 1);

	for(ix=0; ; ix++)
	{
		// convert voice name to lower case  (ascii)
		if((buf[ix] = tolower(buf[ix])) == 0)
			break;
	}

	memset(&voice_selector,0,sizeof(voice_selector));
	voice_selector.name = (char *)name;  // include variant name in voice stack ??

	// first check for a voice with this filename
	// This may avoid the need to call espeak_ListVoices().

	if(LoadVoice(buf,1) != NULL)
	{
		if(variant_name[0] != 0)
		{
			LoadVoice(variant_name,2);
		}

		DoVoiceChange(voice);
		voice_selector.languages = voice->language_name;
		SetVoiceStack(&voice_selector, variant_name);
		return(EE_OK);
	}

	if(n_voices_list == 0)
		espeak_ListVoices(NULL);   // create the voices list

	if((v = SelectVoiceByName(voices_list,buf)) != NULL)
	{
		if(LoadVoice(v->identifier,0) != NULL)
		{
			if(variant_name[0] != 0)
			{
				LoadVoice(variant_name,2);
			}
			DoVoiceChange(voice);
			voice_selector.languages = voice->language_name;
			SetVoiceStack(&voice_selector, variant_name);
			return(EE_OK);
		}
	}
	return(EE_INTERNAL_ERROR);   // voice name not found
}  // end of SetVoiceByName



espeak_ERROR SetVoiceByProperties(espeak_VOICE *voice_selector)
{//============================================================
	const char *voice_id;
	int voice_found;

	voice_id = SelectVoice(voice_selector, &voice_found);

	if(voice_found == 0)
		return(EE_NOT_FOUND);

	LoadVoiceVariant(voice_id,0);
	DoVoiceChange(voice);
	SetVoiceStack(voice_selector, "");

	return(EE_OK);
}  //  end of SetVoiceByProperties


void FreeVoiceList()
{//=================
	int ix;
	for(ix=0; ix<n_voices_list; ix++)
	{
		if(voices_list[ix] != NULL)
		{
			free(voices_list[ix]);
			voices_list[ix] = NULL;
		}
	}
	n_voices_list = 0;
}


//=======================================================================
//  Library Interface Functions
//=======================================================================
#pragma GCC visibility push(default)


ESPEAK_API const espeak_VOICE **espeak_ListVoices(espeak_VOICE *voice_spec)
{//========================================================================
	char path_voices[sizeof(path_home)+12];


#ifdef PLATFORM_RISCOS
	if(n_voices_list == 0)
	{
		sprintf(path_voices,"%s%cvoices",path_home,PATHSEP);
		len_path_voices = strlen(path_voices)+1;
		GetVoices(path_voices);
		voices_list[n_voices_list] = NULL;  // voices list terminator
	}
	return((const espeak_VOICE **)voices_list);

#else
	int ix;
	int j;
	espeak_VOICE *v;
	static espeak_VOICE **voices = NULL;

	// free previous voice list data
	FreeVoiceList();

	sprintf(path_voices,"%s%cvoices",path_home,PATHSEP);
	len_path_voices = strlen(path_voices)+1;

	GetVoices(path_voices);
	voices_list[n_voices_list] = NULL;  // voices list terminator
	voices = (espeak_VOICE **)realloc(voices, sizeof(espeak_VOICE *)*(n_voices_list+1));

	// sort the voices list
	qsort(voices_list,n_voices_list,sizeof(espeak_VOICE *),
		  (int (__cdecl *)(const void *,const void *))VoiceNameSorter);


	if(voice_spec)
	{
		// select the voices which match the voice_spec, and sort them by preference
		SetVoiceScores(voice_spec,voices,1);
	}
	else
	{
		// list all: omit variant voices and mbrola voices and test voices
		j = 0;
		for(ix=0; (v = voices_list[ix]) != NULL; ix++)
		{
			if((v->languages[0] != 0) && (strcmp(&v->languages[1],"variant") != 0)
				&& (memcmp(v->identifier,"mb/",3) != 0) && (memcmp(v->identifier,"test/",5) != 0))
			{
				voices[j++] = v;
			}
		}
		voices[j] = NULL;
	}
	return((const espeak_VOICE **)voices);
#endif
}  //  end of espeak_ListVoices



ESPEAK_API espeak_VOICE *espeak_GetCurrentVoice(void)
{//==================================================
	return(&current_voice_selected);
}

#pragma GCC visibility pop


