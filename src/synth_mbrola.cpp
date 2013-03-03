/***************************************************************************
 *   Copyright (C) 2005 to 2013 by Jonathan Duddington                     *
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

#include <stdio.h>
#include <ctype.h>
#include <wctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "speak_lib.h"
#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"
#include "voice.h"

int option_mbrola_phonemes;

#ifdef INCLUDE_MBROLA

extern int Read4Bytes(FILE *f);
extern void SetPitch2(voice_t *voice, int pitch1, int pitch2, int *pitch_base, int *pitch_range);
extern unsigned char *outbuf;

#ifndef PLATFORM_WINDOWS

#include "mbrowrap.h"

#else
#include <windows.h>
typedef void (WINAPI *PROCVV)(void);
typedef void (WINAPI *PROCVI)(int);
typedef void (WINAPI *PROCVF)(float);
typedef int (WINAPI *PROCIV)();
typedef int (WINAPI *PROCIC) (char *);
typedef int (WINAPI *PROCISI)(short *,int);
typedef char* (WINAPI *PROCVCI)(char *,int);

PROCIC		init_MBR;
PROCIC		write_MBR;
PROCIV		flush_MBR;
PROCISI		read_MBR;
PROCVV		close_MBR;
PROCVV		reset_MBR;
PROCIV		lastError_MBR;
PROCVCI		lastErrorStr_MBR;
PROCVI		setNoError_MBR;
PROCIV		getFreq_MBR;
PROCVF		setVolumeRatio_MBR;



HINSTANCE	hinstDllMBR = NULL;


BOOL load_MBR()
{
	if(hinstDllMBR != NULL)
		return TRUE;   // already loaded

	if ((hinstDllMBR=LoadLibraryA("mbrola.dll")) == 0)
		return FALSE;
	init_MBR			=(PROCIC) GetProcAddress(hinstDllMBR,"init_MBR");
	write_MBR			=(PROCIC) GetProcAddress(hinstDllMBR,"write_MBR");
	flush_MBR			=(PROCIV) GetProcAddress(hinstDllMBR,"flush_MBR");
	read_MBR			=(PROCISI) GetProcAddress(hinstDllMBR,"read_MBR");
	close_MBR			=(PROCVV) GetProcAddress(hinstDllMBR,"close_MBR");
	reset_MBR			=(PROCVV) GetProcAddress(hinstDllMBR,"reset_MBR");
	lastError_MBR		=(PROCIV) GetProcAddress(hinstDllMBR,"lastError_MBR");
	lastErrorStr_MBR	=(PROCVCI) GetProcAddress(hinstDllMBR,"lastErrorStr_MBR");
	setNoError_MBR		=(PROCVI) GetProcAddress(hinstDllMBR,"setNoError_MBR");
	setVolumeRatio_MBR	=(PROCVF) GetProcAddress(hinstDllMBR,"setVolumeRatio_MBR");
	return TRUE;
}


void unload_MBR()
{
	if (hinstDllMBR)
	{
		FreeLibrary (hinstDllMBR);
		hinstDllMBR=NULL;
	}
}

#endif   // windows


static MBROLA_TAB *mbrola_tab = NULL;
static int mbrola_control = 0;
static int mbr_name_prefix = 0;

espeak_ERROR LoadMbrolaTable(const char *mbrola_voice, const char *phtrans, int srate)
{//===================================================================================
// Load a phoneme name translation table from espeak-data/mbrola

	int size;
	int ix;
	int *pw;
	FILE *f_in;
	char path[sizeof(path_home)+15];

	mbrola_name[0] = 0;
	mbrola_delay = 0;
	mbr_name_prefix = 0;

	if(mbrola_voice == NULL)
	{
		samplerate = samplerate_native;
		SetParameter(espeakVOICETYPE,0,0);
		return(EE_OK);
	}

	sprintf(path,"%s/mbrola/%s",path_home,mbrola_voice);
#ifdef PLATFORM_POSIX
	// if not found, then also look in
	//   usr/share/mbrola/xx, /usr/share/mbrola/xx/xx, /usr/share/mbrola/voices/xx
	if(GetFileLength(path) <= 0)
	{
     sprintf(path,"/usr/share/mbrola/%s",mbrola_voice);

		if(GetFileLength(path) <= 0)
		{
			sprintf(path,"/usr/share/mbrola/%s/%s",mbrola_voice,mbrola_voice);

			if(GetFileLength(path) <= 0)
			{
				sprintf(path,"/usr/share/mbrola/voices/%s",mbrola_voice);
			}
		}
	}
	close_MBR();
#endif
#ifdef PLATFORM_WINDOWS
	if(load_MBR() == FALSE)     // load mbrola.dll
	{
		fprintf(stderr, "Can't load mbrola.dll\n");
		return(EE_INTERNAL_ERROR);
	}
#endif

	if(init_MBR(path) != 0)      // initialise the required mbrola voice
		return(EE_NOT_FOUND);

	setNoError_MBR(1);     // don't stop on phoneme errors

	// read eSpeak's mbrola phoneme translation data, eg. en1_phtrans
	sprintf(path,"%s/mbrola_ph/%s",path_home,phtrans);
	size = GetFileLength(path);
	if((f_in = fopen(path,"rb")) == NULL) {
		close_MBR();
		return(EE_NOT_FOUND);
	}

	if((mbrola_tab = (MBROLA_TAB *)realloc(mbrola_tab,size)) == NULL)
	{
		fclose(f_in);
		close_MBR();
		return(EE_INTERNAL_ERROR);
	}

	mbrola_control = Read4Bytes(f_in);
	pw = (int *)mbrola_tab;
	for(ix=4; ix<size; ix+=4)
	{
		*pw++ = Read4Bytes(f_in);
	}
	size = fread(mbrola_tab,1,size,f_in);
	fclose(f_in);

	setVolumeRatio_MBR((float)(mbrola_control & 0xff) /16.0f);
//	srate = getFreq_MBR();
	samplerate = srate;
	if(srate == 22050)
		SetParameter(espeakVOICETYPE,0,0);
	else
		SetParameter(espeakVOICETYPE,1,0);
	strcpy(mbrola_name,mbrola_voice);
//	mbrola_delay = 3800;  // improve synchronization of events
	mbrola_delay = 1000;  // improve synchronization of events
	return(EE_OK);
}  // end of LoadMbrolaTable


static int GetMbrName(PHONEME_LIST *plist, PHONEME_TAB *ph, PHONEME_TAB *ph_prev, PHONEME_TAB *ph_next, int *name2, int *split, int *control)
{//==========================================================================================================================================
// Look up a phoneme in the mbrola phoneme name translation table
// It may give none, 1, or 2 mbrola phonemes
	MBROLA_TAB *pr;
	PHONEME_TAB *other_ph;
	int found = 0;
	static int mnem;

	// control
	// bit 0  skip the next phoneme
	// bit 1  match this and Previous phoneme
	// bit 2  only at the start of a word
	// bit 3  don't match two phonemes across a word boundary
	// bit 4  add this phoneme name as a prefix to the next phoneme name (used for de4 phoneme prefix '?')
	// bit 5  only in stressed syllable
	// bit 6  only at the end of a word

	*name2=0;
	*split=0;
	*control=0;
	mnem = ph->mnemonic;

	pr = mbrola_tab;
	while(pr->name != 0)
	{
		if(mnem == pr->name)
		{
			if(pr->next_phoneme == 0)
				found = 1;
			else
			if((pr->next_phoneme == ':') && (plist->synthflags & SFLAG_LENGTHEN))
			{
				found = 1;
			}
			else
			{
				if(pr->control & 2)
					other_ph = ph_prev;
				else
				if((pr->control & 8) && ((plist+1)->newword))
					other_ph = phoneme_tab[phPAUSE];  // don't match the next phoneme over a word boundary
				else
					other_ph = ph_next;

				if((pr->next_phoneme == other_ph->mnemonic) ||
					((pr->next_phoneme == 2) && (other_ph->type == phVOWEL)) ||
					((pr->next_phoneme == '_') && (other_ph->type == phPAUSE)))
				{
					found = 1;
				}
			}

			if((pr->control & 4) && (plist->newword == 0))  // only at start of word
				found = 0;

			if((pr->control & 0x40) && (plist[1].newword == 0))  // only at the end of a word
				found = 0;

			if((pr->control & 0x20) && (plist->stresslevel < plist->wordstress))
				found = 0;   // only in stressed syllables

			if(found)
			{
				*name2 = pr->mbr_name2;
				*split = pr->percent;
				*control = pr->control;

				if(pr->control & 0x10)
				{
					mbr_name_prefix = pr->mbr_name;
					return(0);
				}
				mnem = pr->mbr_name;
				break;
			}
		}

		pr++;
	}

	if(mbr_name_prefix != 0)
	{
		mnem = (mnem << 8) | (mbr_name_prefix & 0xff);
	}
	mbr_name_prefix = 0;
	return(mnem);
}


static char *WritePitch(int env, int pitch1, int pitch2, int split, int final)
{//===========================================================================
// final=1:  only give the final pitch value.
	int x;
	int ix;
	int pitch_base;
	int pitch_range;
	int p1,p2,p_end;
	unsigned char *pitch_env;
	int max = -1;
	int min = 999;
	int y_max=0;
	int y_min=0;
	int env100 = 80;  // apply the pitch change only over this proportion of the mbrola phoneme(s)
	int y2;
	int y[4];
	int env_split;
	char buf[50];
	static char output[50];

	output[0] = 0;
	pitch_env = envelope_data[env];


	SetPitch2(voice, pitch1, pitch2, &pitch_base, &pitch_range);


	env_split = (split * 128)/100;
	if(env_split < 0)
		env_split = 0-env_split;

	// find max and min in the pitch envelope
	for(x=0; x<128; x++)
	{
		if(pitch_env[x] > max)
		{
			max = pitch_env[x];
			y_max = x;
		}
		if(pitch_env[x] < min)
		{
			min = pitch_env[x];
			y_min = x;
		}
	}
	// set an additional pitch point half way through the phoneme.
	// but look for a maximum or a minimum and use that instead
	y[2] = 64;
	if((y_max > 0) && (y_max < 127))
	{
		y[2] = y_max;
	}
	if((y_min > 0) && (y_min < 127))
	{
		y[2] = y_min;
	}
	y[1] = y[2] / 2;
	y[3] = y[2] + (127 - y[2])/2;

	// set initial pitch
	p1 = ((pitch_env[0]*pitch_range)>>8) + pitch_base;   // Hz << 12
	p_end = ((pitch_env[127]*pitch_range)>>8) + pitch_base;


	if(split >= 0)
	{
		sprintf(buf," 0 %d",p1/4096);
		strcat(output,buf);
	}

	// don't use intermediate pitch points for linear rise and fall
	if(env > 1)
	{
		for(ix=1; ix<4; ix++)
		{
			p2 = ((pitch_env[y[ix]]*pitch_range)>>8) + pitch_base;

			if(split > 0)
			{
				y2 = (y[ix] * env100)/env_split;
			}
			else
			if(split < 0)
			{
				y2 = ((y[ix]-env_split) * env100)/env_split;
			}
			else
			{
				y2 = (y[ix] * env100)/128;
			}
			if((y2 > 0) && (y2 <= env100))
			{
				sprintf(buf," %d %d",y2,p2/4096);
				strcat(output,buf);
			}
		}
	}

	p_end = p_end/4096;
	if(split <= 0)
	{
		sprintf(buf," %d %d",env100,p_end);
		strcat(output,buf);
	}
	if(env100 < 100)
	{
		sprintf(buf," %d %d",100,p_end);
		strcat(output,buf);
	}
	strcat(output,"\n");

	if(final)
		sprintf(output,"\t100 %d\n",p_end);
	return(output);
}  // end of WritePitch


int MbrolaTranslate(PHONEME_LIST *plist, int n_phonemes, int resume, FILE *f_mbrola)
{//=================================================================================
// Generate a mbrola pho file
	unsigned int name;
	int len;
	int len1;
	PHONEME_TAB *ph;
	PHONEME_TAB *ph_next;
	PHONEME_TAB *ph_prev;
	PHONEME_LIST *p;
	PHONEME_LIST *next;
	PHONEME_DATA phdata;
	FMT_PARAMS fmtp;
	int pause = 0;
	int released;
	int name2;
	int control;
	int done;
	int len_percent;
	const char *final_pitch;
	char *ptr;
	char mbr_buf[120];

	static int phix;
	static int embedded_ix;
	static int word_count;

	if (!resume) {
		phix = 1;
		embedded_ix = 0;
		word_count = 0;
	}

	while (phix < n_phonemes)
	{
		if (WcmdqFree() < MIN_WCMDQ)
			return 1;

		ptr = mbr_buf;

		p = &plist[phix];
		next = &plist[phix+1];
		ph = p->ph;
		ph_prev = plist[phix-1].ph;
		ph_next = plist[phix+1].ph;

		if(p->synthflags & SFLAG_EMBEDDED)
		{
			DoEmbedded(&embedded_ix, p->sourceix);
		}

		if(p->newword & 4)
			DoMarker(espeakEVENT_SENTENCE, (p->sourceix & 0x7ff) + clause_start_char, 0, count_sentences);
		if(p->newword & 1)
			DoMarker(espeakEVENT_WORD, (p->sourceix & 0x7ff) + clause_start_char, p->sourceix >> 11, clause_start_word + word_count++);

		name = GetMbrName(p,ph,ph_prev,ph_next,&name2,&len_percent,&control);
		if(control & 1)
			phix++;

		if(name == 0) {
			phix++;
			continue;   // ignore this phoneme
		}

		if((ph->type == phPAUSE) && (name == ph->mnemonic))
		{
			// a pause phoneme, which has not been changed by the translation
			name = '_';
			len = (p->length * speed.pause_factor)/256;
//			if(len == 0) continue;
			if(len == 0)
				len = 1;
		}
		else
			len = (80 * speed.wav_factor)/256;

		if(ph->code != phonEND_WORD)
		{
			char phoneme_name[16];
			WritePhMnemonic(phoneme_name, p->ph, p, option_phoneme_events & espeakINITIALIZE_PHONEME_IPA, NULL);
			DoPhonemeMarker(espeakEVENT_PHONEME, (p->sourceix & 0x7ff) + clause_start_char, 0, phoneme_name);
		}

		ptr += sprintf(ptr,"%s\t",WordToString(name));

		if(name2 == '_')
		{
			// add a pause after this phoneme
			pause = len_percent;
			name2 = 0;
		}

		done = 0;
		final_pitch = "";

		switch(ph->type)
		{
		case phVOWEL:
			len = ph->std_length;
			if(p->synthflags & SFLAG_LENGTHEN)
				len += phoneme_tab[phonLENGTHEN]->std_length;  // phoneme was followed by an extra : symbol

			if(ph_next->type == phPAUSE)
				len += 50;        // lengthen vowels before a pause
			len = (len * p->length)/256;

			if(name2 == 0)
			{
				char *pitch = WritePitch(p->env,p->pitch1,p->pitch2,0,0);
				ptr += sprintf(ptr,"%d\t%s", len, pitch);
			}
			else
			{
				char *pitch;

				pitch = WritePitch(p->env,p->pitch1,p->pitch2,len_percent,0);
				len1 = (len * len_percent)/100;
				ptr += sprintf(ptr,"%d\t%s", len1, pitch);

				pitch = WritePitch(p->env,p->pitch1,p->pitch2,-len_percent,0);
				ptr += sprintf(ptr,"%s\t%d\t%s", WordToString(name2), len-len1, pitch);
			}
			done = 1;
			break;

		case phSTOP:
			released = 0;
			if(next->type==phVOWEL) released = 1;
			if(next->type==phLIQUID && !next->newword) released = 1;

			if(released == 0)
				p->synthflags |= SFLAG_NEXT_PAUSE;
			InterpretPhoneme(NULL, 0, p, &phdata, NULL);
			len = DoSample3(&phdata, 0, -1);

			len = (len * 1000)/samplerate;  // convert to mS
			len += PauseLength(p->prepause,1);
			break;

		case phVSTOP:
			len = (80 * speed.wav_factor)/256;
			break;

		case phFRICATIVE:
			len = 0;
			InterpretPhoneme(NULL, 0, p, &phdata, NULL);
			if(p->synthflags & SFLAG_LENGTHEN)
				len = DoSample3(&phdata, p->length, -1);  // play it twice for [s:] etc.
			len += DoSample3(&phdata, p->length, -1);

			len = (len * 1000)/samplerate;  // convert to mS
			break;

		case phNASAL:
			if(next->type != phVOWEL)
			{
				memset(&fmtp, 0, sizeof(fmtp));
				InterpretPhoneme(NULL, 0, p, &phdata, NULL);
				fmtp.fmt_addr = phdata.sound_addr[pd_FMT];
				len = DoSpect2(p->ph, 0, &fmtp,  p, -1);
//				len = DoSpect(p->ph,prev->ph,phoneme_tab[phonPAUSE],2,p,-1);
				len = (len * 1000)/samplerate;
				if(next->type == phPAUSE)
					len += 50;
				final_pitch = WritePitch(p->env,p->pitch1,p->pitch2,0,1);
			}
			break;

		case phLIQUID:
			if(next->type == phPAUSE)
			{
				len += 50;
				final_pitch = WritePitch(p->env,p->pitch1,p->pitch2,0,1);
			}
			break;
		}

		if(!done)
		{
			if(name2 != 0)
			{
				len1 = (len * len_percent)/100;
				ptr += sprintf(ptr,"%d\n%s\t",len1,WordToString(name2));
				len -= len1;
			}
			ptr += sprintf(ptr,"%d%s\n",len,final_pitch);
		}

		if(pause)
		{
			len += PauseLength(pause,0);
			ptr += sprintf(ptr,"_ \t%d\n",PauseLength(pause,0));
			pause = 0;
		}

		if(f_mbrola)
		{
			fwrite(mbr_buf,1,(ptr-mbr_buf),f_mbrola);  // write .pho to a file
		}
		else
		{
			int res = write_MBR(mbr_buf);
			if (res < 0)
				return 0;  /* don't get stuck on error */
			if (res == 0)
				return 1;
			wcmdq[wcmdq_tail][0] = WCMD_MBROLA_DATA;
			wcmdq[wcmdq_tail][1] = len;
			WcmdqInc();
		}

		phix++;
	}

	if(!f_mbrola)
	{
		flush_MBR();

		// flush the mbrola output buffer
		wcmdq[wcmdq_tail][0] = WCMD_MBROLA_DATA;
		wcmdq[wcmdq_tail][1] = 500;
		WcmdqInc();
	}

	return 0;
}  // end of MbrolaTranslate


int MbrolaGenerate(PHONEME_LIST *phoneme_list, int *n_ph, int resume)
{//==================================================================
	FILE *f_mbrola = NULL;

	if(*n_ph == 0)
		return(0);

	if(option_mbrola_phonemes)
	{
		// send mbrola data to a file, not to the mbrola library
		f_mbrola = f_trans;
	}

	int again = MbrolaTranslate(phoneme_list, *n_ph, resume, f_mbrola);
	if (!again)
		*n_ph = 0;
	return again;
}


int MbrolaFill(int length, int resume, int amplitude)
{//==================================================
// Read audio data from Mbrola (length is in millisecs)

	static int n_samples;
	int req_samples, result;
	int ix;
	short value16;
	int value;

	if (!resume)
		n_samples = samplerate * length / 1000;

	req_samples = (out_end - out_ptr)/2;
	if (req_samples > n_samples)
		req_samples = n_samples;
	result = read_MBR((short *)out_ptr, req_samples);
	if (result <= 0)
		return 0;

	for(ix=0; ix < result; ix++)
	{
		value16 = out_ptr[0] + (out_ptr[1] << 8);
		value = value16 * amplitude;
		value = value / 40;   // adjust this constant to give a suitable amplitude for mbrola voices
		if(value > 0x7fff)
			value = 0x7fff;
		if(value < -0x8000)
			value = 0x8000;
		out_ptr[0] = value;
		out_ptr[1] = value >> 8;
		out_ptr += 2;
	}
	n_samples -= result;
	return n_samples ? 1 : 0;
}


void MbrolaReset(void)
{//===================
// Reset the Mbrola engine and flush the pending audio

	reset_MBR();
}

#else   // INCLUDE_MBROLA

// mbrola interface is not compiled, provide dummy functions.

espeak_ERROR LoadMbrolaTable(const char *mbrola_voice, const char *phtrans, int srate)
{
	return(EE_INTERNAL_ERROR);
}

int MbrolaGenerate(PHONEME_LIST *phoneme_list, int *n_ph, int resume)
{
	return(0);
}

int MbrolaFill(int length, int resume, int amplitude)
{
	return(0);
}

void MbrolaReset(void)
{
}


#endif  // INCLUDE_MBROLA
