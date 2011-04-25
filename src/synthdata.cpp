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
#include <stdlib.h>
#include <ctype.h>
#include <wctype.h>
#include <string.h>


#include "speak_lib.h"
#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "translate.h"
#include "wave.h"

const char *version_string = "1.45.04  25.Apr.11";
const int version_phdata  = 0x014500;

int option_device_number = -1;
FILE *f_logespeak = NULL;
int logging_type;

// copy the current phoneme table into here
int n_phoneme_tab;
int current_phoneme_table;
PHONEME_TAB *phoneme_tab[N_PHONEME_TAB];
unsigned char phoneme_tab_flags[N_PHONEME_TAB];   // bit 0: not inherited

USHORT *phoneme_index=NULL;
char *phondata_ptr=NULL;
unsigned char *wavefile_data=NULL;
static unsigned char *phoneme_tab_data = NULL;

int n_phoneme_tables;
PHONEME_TAB_LIST phoneme_tab_list[N_PHONEME_TABS];
int phoneme_tab_number = 0;

int wavefile_ix;              // a wavefile to play along with the synthesis
int wavefile_amp;
int wavefile_ix2;
int wavefile_amp2;

int seq_len_adjust;
int vowel_transition[4];
int vowel_transition0;
int vowel_transition1;

int FormantTransition2(frameref_t *seq, int &n_frames, unsigned int data1, unsigned int data2, PHONEME_TAB *other_ph, int which);



static char *ReadPhFile(void *ptr, const char *fname, int *size)
{//=============================================================
	FILE *f_in;
	char *p;
	unsigned int  length;
	char buf[sizeof(path_home)+40];

	sprintf(buf,"%s%c%s",path_home,PATHSEP,fname);
	length = GetFileLength(buf);
	
	if((f_in = fopen(buf,"rb")) == NULL)
	{
		fprintf(stderr,"Can't read data file: '%s'\n",buf);
		return(NULL);
	}

	if(ptr != NULL)
		Free(ptr);
		
	if((p = Alloc(length)) == NULL)
	{
		fclose(f_in);
		return(NULL);
	}
	if(fread(p,1,length,f_in) != length)
	{
		fclose(f_in);
		return(NULL);
	}

	fclose(f_in);
	if(size != NULL)
		*size = length;
	return(p);
}  //  end of ReadPhFile


int LoadPhData()
{//=============
	int ix;
	int n_phonemes;
	int version;
	int result = 1;
	int length;
	unsigned char *p;
	int *pw;

	if((phoneme_tab_data = (unsigned char *)ReadPhFile((void *)(phoneme_tab_data),"phontab",NULL)) == NULL)
		return(-1);
	if((phoneme_index = (USHORT *)ReadPhFile((void *)(phoneme_index),"phonindex",NULL)) == NULL)
		return(-1);
	if((phondata_ptr = ReadPhFile((void *)(phondata_ptr),"phondata",NULL)) == NULL)
		return(-1);
	if((tunes = (TUNE *)ReadPhFile((void *)(tunes),"intonations",&length)) == NULL)
		return(-1);
   wavefile_data = (unsigned char *)phondata_ptr;
	n_tunes = length / sizeof(TUNE);

	// read the version number from the first 4 bytes of phondata
	version = 0;
	for(ix=0; ix<4; ix++)
	{
		version += (wavefile_data[ix] << (ix*8));
	}

	if(version != version_phdata)
	{
		result = version;
	}

	// set up phoneme tables
	p = phoneme_tab_data;
	n_phoneme_tables = p[0];
	p+=4;

	for(ix=0; ix<n_phoneme_tables; ix++)
	{
		n_phonemes = p[0];
		phoneme_tab_list[ix].n_phonemes = p[0];
		phoneme_tab_list[ix].includes = p[1];
		pw = (int *)p;
		phoneme_tab_list[ix].equivalence_tables = Reverse4Bytes(pw[1]);
		p += 8;
		memcpy(phoneme_tab_list[ix].name,p,N_PHONEME_TAB_NAME);
		p += N_PHONEME_TAB_NAME;
		phoneme_tab_list[ix].phoneme_tab_ptr = (PHONEME_TAB *)p;
		p += (n_phonemes * sizeof(PHONEME_TAB));
	}

	if(phoneme_tab_number >= n_phoneme_tables)
		phoneme_tab_number = 0;

	return(result);
}  //  end of LoadPhData


void FreePhData(void)
{//==================
	Free(phoneme_tab_data);
	Free(phoneme_index);
	Free(phondata_ptr);
	phoneme_tab_data=NULL;
	phoneme_index=NULL;
	phondata_ptr=NULL;
}


int PhonemeCode(unsigned int mnem)
{//===============================
	int ix;

	for(ix=0; ix<n_phoneme_tab; ix++)
	{
		if(phoneme_tab[ix] == NULL)
			continue;
		if(phoneme_tab[ix]->mnemonic == mnem)
			return(phoneme_tab[ix]->code);
	}
	return(0);
}


int LookupPhonemeString(const char *string)
{//========================================
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

	return(PhonemeCode(mnem));
}




frameref_t *LookupSpect(PHONEME_TAB *this_ph, int which, FMT_PARAMS *fmt_params,  int *n_frames, PHONEME_LIST *plist)
{//===================================================================================================================
	int  ix;
	int  nf;
	int  nf1;
	int  seq_break;
	frameref_t *frames;
	int  length1;
	int  length_std;
	int  length_factor;
	SPECT_SEQ *seq, *seq2;
	SPECT_SEQK *seqk, *seqk2;
	frame_t *frame;
	static frameref_t frames_buf[N_SEQ_FRAMES];
	
	seq = (SPECT_SEQ *)(&phondata_ptr[fmt_params->fmt_addr]);
	seqk = (SPECT_SEQK *)seq;
	nf = seq->n_frames;


	if(nf >= N_SEQ_FRAMES)
		nf = N_SEQ_FRAMES - 1;

	seq_len_adjust = fmt_params->fmt2_lenadj + fmt_params->fmt_length;
	seq_break = 0;

	for(ix=0; ix<nf; ix++)
	{
		if(seq->frame[0].frflags & FRFLAG_KLATT)
			frame = &seqk->frame[ix];
		else
			frame = (frame_t *)&seq->frame[ix];
		frames_buf[ix].frame = frame;
		frames_buf[ix].frflags = frame->frflags;
		frames_buf[ix].length = frame->length;
		if(frame->frflags & FRFLAG_VOWEL_CENTRE)
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
	if((this_ph->type == phVOWEL) && (fmt_params->fmt2_addr == 0) && (fmt_params->use_vowelin))
	{
		seq_len_adjust += FormantTransition2(frames,nf,fmt_params->transition0,fmt_params->transition1,NULL,which);
	}

	length1 = 0;
	nf1 = nf - 1;
	for(ix=0; ix<nf1; ix++)
		length1 += frames[ix].length;

	if(fmt_params->fmt2_addr != 0)
	{
		// a secondary reference has been returned, which is not a wavefile
		// add these spectra to the main sequence
		seq2 = (SPECT_SEQ *)(&phondata_ptr[fmt_params->fmt2_addr]);
		seqk2 = (SPECT_SEQK *)seq2;
	
		// first frame of the addition just sets the length of the last frame of the main seq
		nf--;
		for(ix=0; ix<seq2->n_frames; ix++)
		{
			if(seq2->frame[0].frflags & FRFLAG_KLATT)
				frame = &seqk2->frame[ix];
			else
				frame = (frame_t *)&seq2->frame[ix];

			frames[nf].length = frame->length;
			if(ix > 0)
			{
				frames[nf].frame = frame;
				frames[nf].frflags = frame->frflags;
			}
			nf++;
		}
		wavefile_ix = 0;
	}
	
	if(length1 > 0)
	{
		if(which==2)
		{
			// adjust the length of the main part to match the standard length specified for the vowel
			//   less the front part of the vowel and any added suffix

			length_std = fmt_params->std_length + seq_len_adjust - 45;
			if(length_std < 10)
				length_std = 10;
			if(plist->synthflags & SFLAG_LENGTHEN)
				length_std += (phoneme_tab[phonLENGTHEN]->std_length * 2);  // phoneme was followed by an extra : symbol

// can adjust vowel length for stressed syllables here


			length_factor = (length_std * 256)/ length1;
			
			for(ix=0; ix<nf1; ix++)
			{
				frames[ix].length = (frames[ix].length * length_factor)/256;
			}
		}
		else
		{
			if(which == 1)
			{
				// front of a vowel
				if(fmt_params->fmt_control == 1)
				{
					// This is the default start of a vowel.
					// Allow very short vowels to have shorter front parts
					if(fmt_params->std_length < 130)
						frames[0].length = (frames[0].length * fmt_params->std_length)/130;
				}
			}
			else
			{
				//not a vowel
				if(fmt_params->std_length > 0)
				{
					seq_len_adjust += (fmt_params->std_length - length1);
				}
			}

			if(seq_len_adjust != 0)
			{
				length_factor = ((length1 + seq_len_adjust) * 256)/length1;
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



unsigned char *GetEnvelope(int index)
{//==================================
	if(index==0)
	{
		fprintf(stderr,"espeak: No envelope\n");
		return(envelope_data[0]);   // not found, use a default envelope
	}
	return((unsigned char *)&phondata_ptr[index]);
}


static void SetUpPhonemeTable(int number, int recursing)
{//=====================================================
	int ix;
	int includes;
	int ph_code;
	PHONEME_TAB *phtab;

	if(recursing==0)
	{
		memset(phoneme_tab_flags,0,sizeof(phoneme_tab_flags));
	}

	if((includes = phoneme_tab_list[number].includes) > 0)
	{
		// recursively include base phoneme tables
		SetUpPhonemeTable(includes-1,1);
	}

	// now add the phonemes from this table
	phtab = phoneme_tab_list[number].phoneme_tab_ptr;
	for(ix=0; ix<phoneme_tab_list[number].n_phonemes; ix++)
	{
		ph_code = phtab[ix].code;
		phoneme_tab[ph_code] = &phtab[ix];
		if(ph_code > n_phoneme_tab)
			n_phoneme_tab = ph_code;

		if(recursing == 0)
			phoneme_tab_flags[ph_code] |= 1;   // not inherited
	}
}  // end of SetUpPhonemeTable


void SelectPhonemeTable(int number)
{//================================
	n_phoneme_tab = 0;
	SetUpPhonemeTable(number,0);  // recursively for included phoneme tables
	n_phoneme_tab++;
	current_phoneme_table = number;
}  //  end of SelectPhonemeTable


int LookupPhonemeTable(const char *name)
{//=====================================
	int ix;

	for(ix=0; ix<n_phoneme_tables; ix++)
	{
		if(strcmp(name,phoneme_tab_list[ix].name)==0)
		{
			phoneme_tab_number = ix;
			break;
		}
	}
	if(ix == n_phoneme_tables)
		return(-1);

	return(ix);
}


int SelectPhonemeTableName(const char *name)
{//=========================================
// Look up a phoneme set by name, and select it if it exists
// Returns the phoneme table number
	int ix;

	if((ix = LookupPhonemeTable(name)) == -1)
		return(-1);

	SelectPhonemeTable(ix);
	return(ix);
}  //  end of DelectPhonemeTableName




void LoadConfig(void)
{//==================
// Load configuration file, if one exists
	char buf[sizeof(path_home)+10];
	FILE *f;
	int ix;
	char c1;
	char *p;
	char string[200];

	logging_type = 0;

	for(ix=0; ix<N_SOUNDICON_SLOTS; ix++)
	{
		soundicon_tab[ix].filename = NULL;
		soundicon_tab[ix].data = NULL;
	}

	sprintf(buf,"%s%c%s",path_home,PATHSEP,"config");
	if((f = fopen(buf,"r"))==NULL)
	{
		return;
	}

	while(fgets(buf,sizeof(buf),f)!=NULL)
	{
		if(buf[0] == '/')  continue;

		if(memcmp(buf,"log",3)==0)
		{
			if(sscanf(&buf[4],"%d %s",&logging_type,string)==2)
				f_logespeak = fopen(string,"w");
		}
		else
		if(memcmp(buf,"tone",4)==0)
		{
			ReadTonePoints(&buf[5],tone_points);
		}
		else
		if(memcmp(buf,"pa_device",9)==0)
		{
			sscanf(&buf[10],"%d",&option_device_number);
		}
		else
		if(memcmp(buf,"soundicon",9)==0)
		{
			ix = sscanf(&buf[10],"_%c %s",&c1,string);
			if(ix==2)
			{
				soundicon_tab[n_soundicon_tab].name = c1;
				p = Alloc(strlen(string)+1);
				strcpy(p,string);
				soundicon_tab[n_soundicon_tab].filename = p;
				soundicon_tab[n_soundicon_tab++].length = 0;
			}
		}
	}
}  //  end of LoadConfig




PHONEME_DATA this_ph_data;


static void InvalidInstn(PHONEME_TAB *ph, int instn)
{//====================================================
	fprintf(stderr,"Invalid instruction %.4x for phoneme '%s'\n", instn, WordToString(ph->mnemonic));
}


static bool StressCondition(Translator *tr, PHONEME_LIST *plist, int condition, int control)
{//========================================================================================
// condition:
//	0	if diminished, 1 if unstressed, 2 if not stressed, 3 if stressed, 4 if max stress

	int stress_level;
	PHONEME_LIST *pl;
	static int condition_level[4] = {1,2,4,15};

	if(phoneme_tab[plist[0].phcode]->type == phVOWEL)
	{
		pl = plist;
	}
	else
	{
		// consonant, get stress from the following vowel
		if(phoneme_tab[plist[1].phcode]->type == phVOWEL)
		{
			pl = &plist[1];
		}
		else
			return(false);  // no stress elevel for this consonant
	}

	stress_level = pl->stresslevel & 0xf;

	if(tr != NULL)
	{
		if((control & 1) && (plist->synthflags & SFLAG_DICTIONARY) && ((tr->langopts.param[LOPT_REDUCE] & 1)==0))
		{
			// change phoneme.  Don't change phonemes which are given for the word in the dictionary.
			return(false);
		}
	
		if((tr->langopts.param[LOPT_REDUCE] & 0x2) && (stress_level >= pl->wordstress))
		{
			// treat the most stressed syllable in an unstressed word as stressed
			stress_level = 4;
		}
	}

	if(condition == 4)
	{
		return(stress_level >= pl->wordstress);
	}

	if(condition == 3)
	{
		// if stressed
		if(stress_level > 3)
			return(true);
	}
	else
	{
		if(stress_level < condition_level[condition])
			return(true);
	}
	return(false);

}  // end of StressCondition


static bool InterpretCondition(Translator *tr, int control, PHONEME_LIST *plist, int instn)
{//========================================================================================
	int which;
	unsigned int data;
	int instn2;
	int count;
	PHONEME_TAB *ph;
	PHONEME_LIST *plist_this;
	static int ph_position[8] = {0, 1, 2, 3, 2, 0, 1, 3};  // prevPh, thisPh, nextPh, next2Ph, nextPhW, prevPhW, nextVowel, next2PhW

	data = instn & 0xff;
	instn2 = instn >> 8;

	if(instn2 < 14)
	{
		which = (instn2) % 7;

		if(which==4)
		{
			// nextPh not word boundary
			if(plist[1].sourceix)
				return(false);
		}
		if(which==5)
		{
			// prevPh, not word boundary
			if(plist[0].sourceix)
				return(false);
		}
		if(which==7)
		{
			// nextPh2 not word boundary
			if((plist[1].sourceix) || (plist[2].sourceix))
				return(false);
		}

		if(which==6)
		{
			// nextVowel, not word boundary
			for(which=2;;which++)
			{
				if(plist[which-1].sourceix)
					return(false);
				if(phoneme_tab[plist[which-1].phcode]->type == phVOWEL)
					break;
			}
		}
		else
		{
			which = ph_position[which];
		}

		plist_this = plist;
		plist = &plist[which-1];

		if(which == 0)
		{
			if(plist->phcode == 1)
			{
				// This is a NULL phoneme, a phoneme has been deleted so look at the previous phoneme
				plist = &plist[-1];
			}
		}

		if(control & 0x100)
		{
			// change phonemes pass
			plist->ph = phoneme_tab[plist->phcode];
		}
		ph = plist->ph;

		if(instn2 < 7)
		{
			// 'data' is a phoneme number
			if((phoneme_tab[data]->mnemonic == ph->mnemonic) == true)
				return(true);
			if((which == 0) && (ph->type == phVOWEL))
				return(data == ph->end_type);   // prevPh() match on end_type
			return(data == ph->start_type);    // thisPh() or nextPh(), match on start_type
		}

		data = instn & 0x1f;

		switch(instn & 0xe0)
		{
		case 0x00:
			// phoneme type, vowel, nasal, fricative, etc
			return(ph->type == data);
			break;

		case 0x20:
			// place of articulation
			return(((ph->phflags >> 16) & 0xf) == data);
			break;

		case 0x40:
			// is a bit set in phoneme flags
			return((ph->phflags & (1 << data)) != 0);
			break;

		case 0x80:
			switch(data)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
				return(StressCondition(tr, plist, data, 0));

			case 5:  // isBreak, Either pause phoneme, or (stop/vstop/vfric not followed by vowel or (liquid in same word))
				return((ph->type == phPAUSE) || (plist_this->synthflags & SFLAG_NEXT_PAUSE));

			case 6:  // isWordStart
				return(plist->sourceix != 0);

			case 7:  // notWordStart
				return(plist->sourceix == 0);

			case 8:  // isWordEnd
				return(plist[1].sourceix || (plist[1].ph->type == phPAUSE));
				break;

			case 9:  // isAfterStress
				if(plist->sourceix != 0)
					return(false);
				do {
					plist--;
					if((plist->stresslevel & 0xf) >= 4)
						return(true);
					
				} while (plist->sourceix == 0);
				break;

			case 10:  // isNotVowel
				return(ph->type != phVOWEL);

			case 11:  // isFinalVowel
				for(;;)
				{
					plist++;
					plist->ph = phoneme_tab[plist->phcode];
					if(plist->sourceix != 0)
						return(true);   // start of next word, without finding another vowel
					if(plist->ph->type == phVOWEL)
						return(false);
				}
				break;

			case 12:  // isVoiced
				return((ph->type == phVOWEL) || (ph->type == phLIQUID) || (ph->phflags & phVOICED));
			}

			case 13:  // isFirstVowel
				count = 0;
				for(;;)
				{
					if(plist->ph->type == phVOWEL)
						count++;
					if(plist->sourceix != 0)
						break;
					plist--;
				}
				return(count==1);
			break;

		}
		return(false);
	}
	else
	if(instn2 == 0xf)
	{
		// Other conditions
		switch(data)
		{
		case 1:   // PreVoicing
			return(control & 1);
		case 2:   // KlattSynth
			return(voice->klattv[0] != 0);
		case 3:   // MbrolaSynth
			return(mbrola_name[0] != 0);
		}
	}
	return(false);
}  // end of InterpretCondition


static void SwitchVowelType(PHONEME_LIST *plist, PHONEME_DATA *phdata, USHORT **p_prog, int instn_type)
{//=====================================================================================================
	USHORT *prog;
	int voweltype;
	signed char x;

	if(instn_type == 2)
	{
		phdata->pd_control |= pd_FORNEXTPH;
		voweltype = plist[1].ph->start_type;  // SwitchNextVowelType
	}
	else
	{
		voweltype = plist[-1].ph->end_type;  // SwitchPrevVowelType
	}

	voweltype -= phonVOWELTYPES;
	if((voweltype >= 0) && (voweltype < 6))
	{
		prog = *p_prog + voweltype*2;
		phdata->sound_addr[instn_type] = (((prog[1] & 0xf) << 16) + prog[2]) * 4;
		x = (prog[1] >> 4) & 0xff;
		phdata->sound_param[instn_type] = x;  // sign extend
	}

	*p_prog += 12;
}  // end of SwitchVowelType


static int NumInstnWords(USHORT *prog)
{//===================================
	int instn;
	int instn2;
	int instn_type;
	static char n_words[11] = {1,1,1,1,1,1,1,1,1,2,4};

	instn = *prog;
	instn_type = instn >> 12;
	if(instn_type < 11)
		return(n_words[instn_type]);

	// 11 to 15, 2 words
	instn2 = prog[2];
	if((instn2 >> 12) == 0xf)
	{
		// addWav(), 2 more words
		return(4);
	}
	if(instn2 == i_CONTINUE)
	{
		return(3);
	}
	return(2);
}  //  end of NumInstnWords



void InterpretPhoneme(Translator *tr, int control, PHONEME_LIST *plist, PHONEME_DATA *phdata)
{//==========================================================================================
// control:
//bit 0:  PreVoicing
//bit 8:  change phonemes
	PHONEME_TAB *ph;
	USHORT *prog;
	USHORT instn;
	int instn2;
	int or_flag;
	bool truth;
	int data;
	int end_flag;
	int ix;
	signed char param_sc;

	#define N_RETURN 10
	int n_return=0;
	USHORT *return_addr[N_RETURN];  // return address stack

	ph = plist->ph;

	memset(phdata, 0, sizeof(PHONEME_DATA));
	phdata->pd_param[i_SET_LENGTH] = ph->std_length;
	phdata->pd_param[i_LENGTH_MOD] = ph->length_mod;

	if(ph->program == 0)
		return;

	end_flag = 0;
	
	for(prog = &phoneme_index[ph->program]; end_flag != 1; prog++)
	{
		instn = *prog;
		instn2 = (instn >> 8) & 0xf;
		or_flag = 0;
		
		switch(instn >> 12)
		{
		case 0:
			data = instn & 0xff;

			if(instn2 == 0)
			{
				// instructions with no operand
				switch(data)
				{
				case i_RETURN:
					end_flag = 1;
					break;

				case i_CONTINUE:
					break;

				default:
					InvalidInstn(ph,instn);
					break;
				}
			}
			else
			if(instn2 == i_APPEND_IFNEXTVOWEL)
			{
				if(phoneme_tab[plist[1].phcode]->type == phVOWEL)
					phdata->pd_param[i_APPEND_PHONEME] = data;
			}
			else
			if(instn2 == i_IPA_NAME)
			{
				// followed by utf-8 characters, 2 per instn word
				for(ix=0; (ix < data) && (ix < 16); ix += 2)
				{
					prog++;
					phdata->ipa_string[ix] = prog[0] >> 8;
					phdata->ipa_string[ix+1] = prog[0] & 0xff;
				}
				phdata->ipa_string[ix] = 0;
			}
			else
			if(instn2 < N_PHONEME_DATA_PARAM)
			{
				if(instn2 == i_CHANGE_PHONEME2)
				{
					phdata->pd_param[i_CHANGE_PHONEME] = data;  // also set ChangePhoneme
				}
				phdata->pd_param[instn2] = data;
				if((instn2 == i_CHANGE_PHONEME) && (control & 0x100))
				{
					// found ChangePhoneme() in PhonemeList mode, exit
					end_flag = 1;
				}
			}
			else
			{
				InvalidInstn(ph,instn);
			}
			break;

		case 1:
			if(tr == NULL)
				break;   // ignore if in synthesis stage

			if(instn2 < 8)
			{
				// ChangeIf
				if(StressCondition(tr, plist, instn2 & 7, 1))
				{
					phdata->pd_param[i_CHANGE_PHONEME] = instn & 0xff;
					end_flag = 1;    // change phoneme, exit
				}
			}
			break;

		case 2:
		case 3:
			// conditions
			or_flag = 0;
			truth = true;
			while((instn & 0xe000) == 0x2000)
			{
				// process a sequence of conditions, using  boolean accumulator
				if(or_flag)
					truth = (truth || InterpretCondition(tr, control, plist, instn & 0xfff));
				else
					truth = (truth && InterpretCondition(tr, control, plist, instn & 0xfff));
				or_flag = instn & 0x1000;
				instn = *(++prog);
			}

			if(truth == false)
			{
				if((instn & 0xf800) == i_JUMP_FALSE)
				{
					prog += instn & 0xff;
				}
				else
				{
					// instruction after a condition is not JUMP_FALSE, so skip the instruction.
					prog += NumInstnWords(prog);
					if((prog[0] & 0xfe00) == 0x6000)
						prog++;    // and skip ELSE jump 
				}
			}
			prog--;
		break;

		case 6:
			// JUMP
			switch(instn2 >> 1)
			{
			case 0:
				prog += (instn & 0xff) - 1;
				break;

			case 4:
				// conditional jumps should have been processed in the Condition section
				break;

			case 5:   // NexttVowelStarts
				SwitchVowelType(plist, phdata, &prog, 2);
				break;

			case 6:   // PrevVowelTypeEndings
				SwitchVowelType(plist, phdata, &prog, 3);
				break;
			}
		break;

		case 9:
			data = ((instn & 0xf) << 16) + prog[1];
			prog++;
			switch(instn2)
			{
			case 1:
				// call a procedure or another phoneme
				if(n_return < N_RETURN)
				{
					return_addr[n_return++] = prog;
					prog = &phoneme_index[data] - 1;
				}
				break;

			case 2:
				// pitch envelope
				phdata->pitch_env = data;
				break;

			case 3:
				// amplitude envelope
				phdata->amp_env = data;
				break;
			}
			break;

		case 10:   //  Vowelin, Vowelout
			if(instn2 == 1)
				ix = 0;
			else
				ix = 2;

			phdata->vowel_transition[ix] = ((prog[0] & 0xff) << 16) + prog[1];
			phdata->vowel_transition[ix+1] = (prog[2] << 16) + prog[3];
			prog += 3;
			break;

		case 11:   // FMT
		case 12:   // WAV
		case 13:   // VowelStart
		case 14:   // VowelEnd
		case 15:   // addWav
			instn2 = (instn >> 12) - 11;
			phdata->sound_addr[instn2] = ((instn & 0xf) << 18) + (prog[1] << 2);
			param_sc = phdata->sound_param[instn2] = (instn >> 4) & 0xff;
			prog++;

			if(prog[1] != i_CONTINUE)
			{
				if(instn2 < 2)
				{
					// FMT() and WAV() imply Return
					end_flag = 1;
					if((prog[1] >> 12) == 0xf)
					{
						// Return after the following addWav()
						end_flag = 2;
					}
				}
				else
				if(instn2 ==pd_ADDWAV)
				{
					// addWav(), return if previous instruction was FMT() or WAV()
					end_flag--;
				}

				if((instn2 == pd_VWLSTART) || (instn2 == pd_VWLEND))
				{
					// VowelStart or VowelEnding.
					phdata->sound_param[instn2] = param_sc;   // sign extend
				}
			}
		break;

		default:
			InvalidInstn(ph,instn);
			break;
		}

		if(ph->phflags & phSINGLE_INSTN)
		{
			end_flag = 1;  // this phoneme has a one-instruction program, with an implicit Return
		}

		if((end_flag == 1) && (n_return > 0))
		{
			// return from called procedure or phoneme
			end_flag = 0;
			prog = return_addr[--n_return];
		}
	}

}  // end of InterpretPhoneme


void InterpretPhoneme2(int phcode, PHONEME_DATA *phdata)
{//=====================================================
// Examine the program of a single isolated phoneme
	int ix;
	PHONEME_LIST plist[4];
	memset(plist, 0, sizeof(plist));

	for(ix=0; ix<4; ix++)
	{
		plist[ix].phcode = phonPAUSE;
		plist[ix].ph = phoneme_tab[phonPAUSE];
	}

	plist[1].phcode = phcode;
	plist[1].ph = phoneme_tab[phcode];
	plist[2].sourceix = 1;

	InterpretPhoneme(NULL, 0, &plist[1], phdata);
}  // end of InterpretPhoneme2
