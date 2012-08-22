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


#include "stdio.h"
#include "ctype.h"
#include "wctype.h"
#include "string.h"
#include "stdlib.h"
#include "speech.h"

#ifndef PLATFORM_RISCOS
#include "dirent.h"
#endif

#include "voice.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"

#include "speak_lib.h"

MNEM_TAB genders [] = {
	{"unknown", 0},
	{"male", 1},
	{"female", 2},
	{NULL, 0 }};

// limit the rate of change for each formant number
//static float formant_rate_22050[9] = {0.2, 0.4, 0.65, 0.9, 0.85, 0.85, 1, 1, 1};
static int formant_rate_22050[9] = {52, 104, 165, 232, 220, 220, 256, 256, 256};  // values for 22kHz sample rate
int formant_rate[9];         // values adjusted for actual sample rate



#define DEFAULT_LANGUAGE_PRIORITY  5
#define N_VOICES_LIST  100
static int n_voices_list = 0;
static espeak_VOICE *voices_list[N_VOICES_LIST];
static int len_path_voices;

espeak_VOICE *voice_selected = NULL;
char voice_name[40];



#define V_NAME       1
#define V_LANGUAGE   2
#define V_GENDER     3
#define V_TRANSLATOR  4
#define V_PHONEMES    5
#define V_DICTIONARY  6

// these affect voice quality, are independent of language
#define V_FORMANT     7
#define V_PITCH       8
#define V_ECHO        9
#define V_FLUTTER    10
#define V_ROUGHNESS  11
#define V_CLARITY    12

// these override defaults set by the translator
#define V_WORDGAP    13
#define V_INTONATION 14
#define V_STRESSLENGTH  15
#define V_STRESSAMP  16
#define V_DICTRULES   17
#define V_STRESSRULE  18
#define V_CHARSET     19

// these two need a phoneme table to have been specified
#define V_REPLACE    20


typedef struct {
	const char *mnem;
	int data;
} keywtab_t;

static keywtab_t keyword_tab[] = {
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
	{"intonation", V_INTONATION},
	{"dictrules",	V_DICTRULES},
	{"stressrule", V_STRESSRULE},
	{"charset",    V_CHARSET},
	{"replace",    V_REPLACE},
	{"words",      V_WORDGAP},
	{"echo",       V_ECHO},
	{"flutter",    V_FLUTTER},
	{"roughness",  V_ROUGHNESS},
	{"clarity",    V_CLARITY},

	// these just set a value in langopts.param[]
	{"l_dieresis", 0x100+LOPT_DIERESES},
//	{"l_lengthen", 0x100+LOPT_IT_LENGTHEN},
	{"l_prefix",   0x100+LOPT_PREFIXES},
	{"l_regressive_voicing", 0x100+LOPT_REGRESSIVE_VOICING},
	{"l_unpronouncable", 0x100+LOPT_UNPRONOUNCABLE},
	{"l_final_syllable", 0x100+LOPT_FINAL_SYLLABLE},
	{"l_sonorant_min", 0x100+LOPT_SONORANT_MIN},
	{NULL,   0} };

#define N_VOICES 100
static int n_voices_tab = 0;
static voice_t *voices_tab[N_VOICES];


void VoiceReset(void)
{//==================
	// Set voice to the default values
	int  pk;

	// default is:  pitch 82,118
	voice->pitch_base =   0x49000;    // default, 73 << 12;
	voice->pitch_range =  0x0f30;     // default = 0x1000

	voice->echo_delay = 0;
	voice->echo_amp = 0;
	voice->flutter = 64;
	voice->n_harmonic_peaks = 5;
	voice->peak_shape = 1;
#ifdef PLATFORM_RISCOS
	voice->roughness = 1;
#else
	voice->roughness = 2;
#endif

	for(pk=0; pk<N_PEAKS; pk++)
	{
		voice->freq[pk] = 256;
		voice->height[pk] = 256;
		voice->width[pk] = 256;

		// adjust formant smoothing depending on sample rate
		formant_rate[pk] = (formant_rate_22050[pk] * 22050)/samplerate;
	}

	// default values of speed factors
	voice->speedf1 = 256;
	voice->speedf2 = 238;
	voice->speedf3 = 232;

	n_replace_phonemes = 0;
	option_tone1 = 0;
}  // end of VoiceReset


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
}  // end of VoiceLookup




static int LookupPh(const char *string)
{//====================================
	int  ix;
	unsigned char c;
	unsigned int  mnem;

	// Pack up to 4 characters into a word
	mnem = 0;
	for(ix=0; ix<4; ix++)
	{
		if(string[ix]==0) break;
		c = string[ix];
		mnem |= (c << (ix*8));
	}

	for(ix=0; ix<n_phoneme_tab; ix++)
	{
		if(phoneme_tab[ix] == NULL)
			continue;
		if(phoneme_tab[ix]->mnemonic == mnem)
			return(ix);
	}
	return(0);
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

	if((phon = LookupPh(phon_string1)) == 0)
		return;  // not recognised

	replace_phonemes[n_replace_phonemes].old_ph = phon;
	replace_phonemes[n_replace_phonemes].new_ph = LookupPh(phon_string2);
	replace_phonemes[n_replace_phonemes++].type = flags;
}  //  end of PhonemeReplacement


voice_t *LoadVoice(char *vname, int no_default)
{//============================================
	FILE *f_voice = NULL;
	keywtab_t *k;
	char *p;
	int  key;
	int  ix;
	int  n;
	int  value;
	int  error = 0;
	int  language_set = 0;
	int  phonemes_set = 0;
	int  stress_amps_set = 0;
	int  stress_lengths_set = 0;
	int  conditional_rules = 0;
	LANGUAGE_OPTIONS *langopts = NULL;

	voice_t *v;
	Translator *new_translator = NULL;

	char voicename[40];
	char language_name[40];
	char translator_name[40];
	char new_dictionary[40];
	char phonemes_name[40];
	char language_type[8];
	char buf[120];
	char langname[4];

	int stress_amps[8];
	int stress_lengths[8];

	int pitch1;
	int pitch2;

	strcpy(voicename,vname);
	if(voicename[0]==0)
		strcpy(voicename,"default");

	sprintf(buf,"%s%cvoices%c%s",path_home,PATHSEP,PATHSEP,voicename);

	if(GetFileLength(buf) <= 0)
	{
		// look for the voice in a sub-directory of the language name
		langname[0] = voicename[0];
		langname[1] = voicename[1];
		langname[2] = 0;
		sprintf(buf,"%s%cvoices%c%s%c%s",path_home,PATHSEP,PATHSEP,langname,PATHSEP,voicename);
	}

	f_voice = fopen(buf,"r");

	if((f_voice == NULL) && no_default)
		return(NULL);  // can't open file

	if(translator != NULL)
	{
		delete translator;
		translator = NULL;
	}

	strcpy(language_type,"en");    // default
	strcpy(translator_name,language_type);
	strcpy(new_dictionary,language_type);
	strcpy(phonemes_name,language_type);


	if((v = VoiceLookup(voicename)) != NULL)
		voice = v;
	VoiceReset();

	SelectPhonemeTableName(phonemes_name);  // set up phoneme_tab


	while((f_voice != NULL) && (fgets(buf,sizeof(buf),f_voice) != NULL))
	{
		for(p=buf; (*p != 0) && !isspace(*p); p++);
		*p++ = 0;

		if(buf[0] == 0) continue;

		if((buf[0]=='/') && (buf[1]=='/'))
			continue;   // skip comment line

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
		case V_LANGUAGE:
			// only act on the first language line
			if(language_set)
				break;

			sscanf(p,"%s",language_name);
			memcpy(language_type,language_name,2);
			language_type[2] = 0;
			language_set = 1;
			strcpy(translator_name,language_type);
			strcpy(new_dictionary,language_type);
			strcpy(phonemes_name,language_type);
			SelectPhonemeTableName(phonemes_name);

			if(new_translator != NULL)
					delete new_translator;

			new_translator = SelectTranslator(translator_name);
			langopts = &new_translator->langopts;
			break;

		case V_NAME:
		case V_GENDER:
			break;

		case V_TRANSLATOR:        // language_name
			sscanf(p,"%s",translator_name);

			if(new_translator != NULL)
					delete new_translator;

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
			// default is  pitch 82 118
			n = sscanf(p,"%d %d",&pitch1,&pitch2);
			voice->pitch_base = (pitch1 - 9) << 12;
			voice->pitch_range = (pitch2 - pitch1) * 108;
			break;

		case V_STRESSLENGTH:   // stressLength
			stress_lengths_set = sscanf(p,"%d %d %d %d %d %d %d %d",
				&stress_lengths[0], &stress_lengths[1], &stress_lengths[2], &stress_lengths[3],
				&stress_lengths[4], &stress_lengths[5], &stress_lengths[6], &stress_lengths[7]);

			break;

		case V_STRESSAMP:   // stressAmp
			stress_amps_set = sscanf(p,"%d %d %d %d %d %d %d %d",
				&stress_amps[0], &stress_amps[1], &stress_amps[2], &stress_amps[3],
				&stress_amps[4], &stress_amps[5], &stress_amps[6], &stress_amps[7]);
			break;

		case V_INTONATION:   // intonation
			sscanf(p,"%d %d",&option_tone1,&option_tone2);
			break;

		case V_DICTRULES:   // conditional dictionary rules and list entries
			while(*p != 0)
			{
				while(isspace(*p)) p++;
				n = -1;
				if((n = atoi(p++)) >= 0)
					conditional_rules |= (1 << n);
				while(isalnum(*p)) p++;
			}
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

	if(new_translator == NULL)
	{
		// not set by language attribute
		new_translator = SelectTranslator(translator_name);
		langopts = &new_translator->langopts;
	}

	for(ix=0; ix<N_PEAKS; ix++)
	{
		voice->freq2[ix] = voice->freq[ix];
		voice->height2[ix] = voice->height[ix];
		voice->width2[ix] = voice->width[ix];
	}

	if((ix = SelectPhonemeTableName(phonemes_name)) < 0)
	{
		fprintf(stderr,"Unknown phoneme table: '%s'\n",phonemes_name);
	}
	voice->phoneme_tab_ix = ix;
	error = new_translator->LoadDictionary(new_dictionary);
	if(dictionary_name[0]==0)
		return(NULL);   // no dictionary loaded

	// relative lengths of different stress syllables
	for(ix=0; ix<stress_lengths_set; ix++)
	{
		new_translator->stress_lengths[ix] = stress_lengths[ix];
	}
	for(ix=0; ix<stress_amps_set; ix++)
	{
		new_translator->stress_amps[ix] = stress_amps[ix];
		new_translator->stress_amps_r[ix] = stress_amps[ix] -1;
	}
	new_translator->dict_condition = conditional_rules;

	if(langopts->param[LOPT_FINAL_SYLLABLE])
		langopts->length_mods0 = langopts->length_mods;

	voice->width[0] = (voice->width[0] * 105)/100;
	translator = new_translator;
	strcpy(voice_name,voicename);
	return(voice);
}  //  end of LoadVoice




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
	return(strcmp(v2->name,v1->name));
}


static int ScoreVoice(espeak_VOICE *voice_spec, int spec_n_parts, int spec_lang_len, espeak_VOICE *voice)
{//======================================================================================================
	int ix;
	char *p;
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

	if(voice_spec->languages == NULL)
	{
		score = 100;
	}
	else
	{
		// compare the required language with each of the languages of this voice
		while(*p != 0)
		{
			language_priority = *p++;

			matching = 1;
			matching_parts = 0;
			n_parts = 1;

			for(ix=0; ; ix++)
			{
				if((ix >= spec_lang_len) || ((c1 = voice_spec->languages[ix]) == '-'))
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
				break;   // no matching parts for this language

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

	if((voice_spec->name != NULL) && (strcmp(voice_spec->name,voice->name)==0))
	{
		// match on voice name
		score += 400;
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


static int SetVoiceScores(espeak_VOICE *voice_select, espeak_VOICE **voices)
{//=========================================================================
	int ix;
	int score;
	int nv;           // number of candidates
	int n_parts=1;
	int lang_len=0;
	char *p;

	// count number if parts in the specified language
	if(voice_select->languages != NULL)
	{
		lang_len = strlen(voice_select->languages);
		for(p = voice_select->languages; *p != 0; p++)
		{
			if(*p == '-')
				n_parts++;
		}
	}

	// select those voices which match the specified language
	nv = 0;
	for(ix=0; ix<n_voices_list; ix++)
	{
		if((score = ScoreVoice(voice_select, n_parts, lang_len, voices_list[ix])) > 0)
		{
			voices[nv] = voices_list[ix];
			voices[nv]->score = score;
			nv++;
		}
	}
	voices[nv] = NULL;  // list terminator

	if(nv==0)
		return(0);

	// sort the selected voices by their score
	qsort(voices,nv,sizeof(espeak_VOICE *),(int (__cdecl *)(const void *,const void *))VoiceScoreSorter);
	return(nv);
}  // end of SetVoiceScores


void DisplayVoices(FILE *f_out, espeak_VOICE **voices_list, char *language)
{//========================================================================
	int ix;
	char *p;
	int len;
	int count;
	int scores = 0;
	espeak_VOICE *v;
	char *lang_name;
	char age_buf[12];
	espeak_VOICE voice_select;
	espeak_VOICE *voices[N_VOICES_LIST];

	static char genders[4] = {' ','M','F',' '};

	if((language != NULL) && (language[0] != 0))
	{
		// display only voices for the specified language, in order of priority
		voice_select.languages = language;
		voice_select.age = 0;
		voice_select.gender = 0;
		voice_select.name = NULL;
		SetVoiceScores(&voice_select,voices);
		scores = 1;
	}
	else
	{
		for(ix=0; (voices[ix] = voices_list[ix]) != NULL; ix++);
	}

	fprintf(f_out,"Pty Language Age/Gender VoiceName     File       Other Langs\n");

	for(ix=0; (v = voices[ix]) != NULL; ix++)
	{
		count = 0;
		p = v->languages;
		while(*p != 0)
		{
			len = strlen(p+1);
			lang_name = p+1;

			if(v->age == 0)
				strcpy(age_buf,"   ");
			else
				sprintf(age_buf,"%3d",v->age);

			if(count==0)
			{
				fprintf(f_out,"%2d  %-12s%s%c  %-15s %-10s ",
               p[0],lang_name,age_buf,genders[v->gender],v->name,v->identifier);
			}
			else
			{
				fprintf(f_out,"(%s %d)",lang_name,p[0]);
			}
			count++;
			p += len+2;
		}
//		if(scores)
//			fprintf(f_out,"%3d  ",v->score);
		fputc('\n',f_out);
	}
}   //  end of DisplayVoices



static espeak_VOICE *ReadVoiceFile(FILE *f_in, const char *fname)
{//==============================================================
// Read a Voice file, allocate a VOICE_DATA and set data from the
// file's  language, gender, name  lines

	char linebuf[80];
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

	vname[0] = 0;
	vgender[0] = 0;
	age = 0;

	while(fgets(linebuf,sizeof(linebuf),f_in) != NULL)
	{
		linebuf[strlen(linebuf)-1] = 0;

		if(memcmp(linebuf,"name",4)==0)
		{
			p = &linebuf[4];
			while(isspace(*p)) p++;
			strncpy0(vname,p,sizeof(vname));
		}
		else
		if(memcmp(linebuf,"language",8)==0)
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
		else
		if(memcmp(linebuf,"gender",6)==0)
		{
			sscanf(&linebuf[6],"%s %d",vgender,&age);
		}
	}
	languages[langix++] = 0;

	if(n_languages == 0)
		return(NULL);    // no language lines in the voice file

	p = (char *)calloc(sizeof(espeak_VOICE) + langix + + strlen(fname) + strlen(vname) + 3, 1);
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
	voice_data->gender = LookupMnem(genders,vgender);
	return(voice_data);
}


static espeak_VOICE *SelectVoiceByName(espeak_VOICE **voices, const char *name)
{//============================================================================
	int ix;
	int match_fname = -1;
	int match_fname2 = -1;
	int match_name = -1;
	char *id;
	int last_part_len;
	char last_part[20];

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
		if(strcmp(name,id = voices[ix]->identifier)==0)
		{
			match_fname = ix;  // matching identifier, use this if no matching name
		}
		else
		if(strcmp(last_part,&id[strlen(id)-last_part_len])==0)
		{
			match_fname2 = ix;
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



espeak_VOICE *SelectVoice(espeak_VOICE *voice_select)
{//==================================================
	int nv;           // number of candidates
	espeak_VOICE *voices[N_VOICES_LIST]; // list of candidates

	if(n_voices_list == 0)
		espeak_ListVoices();   // create the voices list

	// select and sort voices for the required language
	nv = SetVoiceScores(voice_select,voices);

	if(nv==0)
	{
		// no matching voices, choose the default
		return (SelectVoiceByName(voices_list,"default"));
	}
	// index the sorted list by the required variant number
	return(voices[voice_select->variant % nv]);
}  //  end of SelectVoice



void GetVoices(const char *path)
{//=============================
	FILE *f_voice;
	espeak_VOICE *voice_data;
	int ftype;
	char fname[80];

#ifdef PLATFORM_RISCOS
#else
#ifdef PLATFORM_WINDOWS
   WIN32_FIND_DATA FindFileData;
   HANDLE hFind = INVALID_HANDLE_VALUE;
   DWORD dwError;

	sprintf(fname,"%s\\*",path);
	hFind = FindFirstFile(fname, &FindFileData);
	if(hFind == INVALID_HANDLE_VALUE)
		return;

	do {
		sprintf(fname,"%s%c%s",path,PATHSEP,FindFileData.cFileName);

		ftype = GetFileLength(fname);

		if((ftype == -2) && (FindFileData.cFileName[0] != '.'))
		{
			// a sub-sirectory
			GetVoices(fname);
		}
		else
		if(ftype > 0)
		{
			// a regular line, add it to the voices list	
			if((f_voice = fopen(fname,"r")) == NULL)
				continue;
		
			// pass voice file name within the voices directory
			voice_data = ReadVoiceFile(f_voice,fname+len_path_voices);
			fclose(f_voice);

			if(voice_data != NULL)
			{
				voices_list[n_voices_list++] = voice_data;
			}
		}
	} while(FindNextFile(hFind, &FindFileData) != 0);
	FindClose(hFind);

#else
	DIR *dir;
	struct dirent *ent;

	if((dir = opendir(path)) == NULL)
		return;

	while((ent = readdir(dir)) != NULL)
	{
		if(n_voices_list >= (N_VOICES_LIST-2))
			break;   // voices list is full

		sprintf(fname,"%s%c%s",path,PATHSEP,ent->d_name);

		ftype = GetFileLength(fname);

		if((ftype == -2) && (ent->d_name[0] != '.'))
		{
			// a sub-sirectory
			GetVoices(fname);
		}
		else
		if(ftype > 0)
		{
			// a regular line, add it to the voices list	
			if((f_voice = fopen(fname,"r")) == NULL)
				continue;
		
			// pass voice file name within the voices directory
			voice_data = ReadVoiceFile(f_voice,fname+len_path_voices);
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


//=======================================================================
//  Library Interface Functions
//=======================================================================
#pragma GCC visibility push(default)


espeak_VOICE **espeak_ListVoices(void)
{//===================================
#ifndef PLATFORM_RISCOS
	int ix;

	char selected_voice_id[80];
	char path_voices[80];

	// free previous voice list data
	if(voice_selected != NULL)
		strcpy(selected_voice_id,voice_selected->identifier);
	else
		selected_voice_id[0] = 0;
	voice_selected = NULL;

	for(ix=0; ix<n_voices_list; ix++)
	{
		if(voices_list[ix] != NULL)
			free(voices_list[ix]);
	}
	n_voices_list = 0;

	sprintf(path_voices,"%s%cvoices",path_home,PATHSEP);
	len_path_voices = strlen(path_voices)+1;

	GetVoices(path_voices);
	voices_list[n_voices_list] = NULL;  // voices list terminator

	// sort the voices list
	qsort(voices_list,n_voices_list,sizeof(espeak_VOICE *),
		(int (__cdecl *)(const void *,const void *))VoiceNameSorter);

	// restore pointer to current voice
	if(selected_voice_id[0] != 0)
	{
		for(ix=0; ix<n_voices_list; ix++)
		{
			if(strcmp(selected_voice_id, voices_list[ix]->identifier)==0)
			{
				voice_selected = voices_list[ix];
				break;
			}
		}
	}
#endif
	return(voices_list);
}  //  end of espeak_ListVoices



int espeak_SetVoiceByName(const char *name)
{//========================================
	espeak_VOICE *v;

	if(n_voices_list == 0)
		espeak_ListVoices();   // create the voices list

	if((v = SelectVoiceByName(voices_list,name)) != NULL)
	{
		if(LoadVoice(v->identifier,1) != NULL)
		{
			voice_selected = v;
			WavegenSetVoice(voice);
			return(0);
		}
	}
	return(-1);   // voice name not found
}  // end of espeak_SetVoiceByName



int espeak_SetVoiceByProperties(espeak_VOICE *voice_selector)
{//==========================================================
	voice_selected = SelectVoice(voice_selector);

	LoadVoice(voice_selected->identifier,0);
	WavegenSetVoice(voice);
	return(0);
}  //  end of espeak_SetVoiceByProperties



espeak_VOICE *espeak_GetCurrentVoice(void)
{//=======================================
	return(voice_selected);
}

#pragma GCC visibility pop


