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
#include "StdAfx.h"

#include "stdio.h"
#include "ctype.h"
#include "string.h"
#include "stdlib.h"
#include "wchar.h"
#include "locale.h"

#include "speech.h"

#include "sys/stat.h"
#ifndef __WIN32__
#include <unistd.h>
#endif

#include "voice.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"

#include "speak_lib.h"

unsigned char *outbuf=NULL;
int outbuf_size=0;
espeak_EVENT *event_list=NULL;
int event_list_ix=0;
int n_event_list;
long count_samples;


int (* synth_callback)(short*, int, espeak_EVENT*) = NULL;
int (* uri_callback)(int, const char *, const char *) = NULL;



char path_home[120];

voice_t voicedata;
voice_t *voice = &voicedata;


int GetFileLength(const char *filename)
{//====================================
	struct stat statbuf;

	if(stat(filename,&statbuf) != 0)
		return(0);

	if((statbuf.st_mode & S_IFMT) == S_IFDIR)
//	if(S_ISDIR(statbuf.st_mode))
		return(-2);  // a directory

	return(statbuf.st_size);
}  // end of GetFileLength


char *Alloc(int size)
{//==================
	char *p;
	if((p = (char *)malloc(size)) == NULL)
		fprintf(stderr,"Can't allocate memory\n");
	return(p);
}

void Free(void *ptr)
{//=================
	if(ptr != NULL)
		free(ptr);
}



static void init_path(void)
{//========================
#ifdef __WIN32__
	strcpy(path_home,"espeak-data");
#else
	sprintf(path_home,"%s/espeak-data",getenv("HOME"));
	if(access(path_home,R_OK) != 0)
	{
		strcpy(path_home,PATH_ESPEAK_DATA);
	}
#endif

#ifdef PLATFORM_WINDOWS
	strcpy(path_home,"c:\\Program Files\\espeak\\espeak-data");
#endif
}

static int initialise(void)
{//========================
	int param;

	WavegenInit(22050,0);   // 22050
	LoadPhData();
#ifndef __WIN32__
	LoadConfig();  // causes problem on Windows, don't know why
#endif
	SynthesizeInit();

	for(param=0; param<N_SPEECH_PARAM; param++)
		param_stack[0].parameter[param] = param_defaults[param];

	return(0);
}



static int Synthesize(const void *text, int flags)
{//===============================================
// Fill the buffer with output sound
	int length;
	int finished = 0;
	int count_buffers = 0;

	if((outbuf==NULL) || (event_list==NULL))
		return(-1);  // espeak_Initialize()  has not been called

	option_multibyte = flags & 7;
	option_ssml = flags & espeakSSML;
	option_phoneme_input = flags & espeakPHONEMES;
	option_endpause = flags & espeakENDPAUSE;

	count_samples = 0;

	if(translator == NULL)
	{
		LoadVoice("default",0);
	}

	SpeakNextClause(NULL,text,0);

	for(;;)
	{
		out_ptr = outbuf;
		out_end = &outbuf[outbuf_size];
		event_list_ix = 0;
		WavegenFill(0);

		length = (out_ptr - outbuf)/2;
		count_samples += length;
		event_list[event_list_ix].type = 0;   // type=0 indicates end of event list

		count_buffers++;
		finished = synth_callback((short *)outbuf, length, event_list);
		if(finished)
			break;

		if(Generate(phoneme_list,n_phoneme_list,1)==0)
		{
			if(WcmdqUsed() == 0)
			{
				// don't process the next clause until the previous clause has finished generating speech.
				// This ensures that <audio> tag (which causes end-of-clause) is at a sound buffer boundary
				if(SpeakNextClause(NULL,NULL,1)==0)
				{
					synth_callback(NULL, 0, event_list);  // NULL buffer ptr indicates end of data
					break;
				}
			}
		}
	}
	return(0);
}  //  end of Synthisize


void MarkerEvent(int type, unsigned int char_position, int value, unsigned char *out_ptr)
{//======================================================================================
// type: 1=word, 2=sentence, 3=named mark, 4=play audio, 5=end
	espeak_EVENT *ep;
	double time;

	if((event_list == NULL) || (event_list_ix >= (n_event_list-2)))
		return;

	ep = &event_list[event_list_ix++];
	ep->type = type;
	ep->text_position = char_position & 0xffff;
	ep->length = char_position >> 24;

	time = (double(count_samples + (out_ptr - outbuf)/2)*1000.0)/samplerate;
	ep->audio_position = int(time);

	if((type == espeakEVENT_MARK) || (type == espeakEVENT_PLAY))
		ep->id.name = &namedata[value];
	else
		ep->id.number = value;
}  //  end of MarkerEvent




#pragma GCC visibility push(default)


void espeak_SetSynthCallback(int (* SynthCallback)(short*, int, espeak_EVENT*))
{//============================================================================
	synth_callback = SynthCallback;
}

void espeak_SetUriCallback(int (* UriCallback)(int, const char*, const char *))
{//============================================================================
	uri_callback = UriCallback;
}

int espeak_Initialize(int buf_length)
{//=================================
	int param;

	// It seems that the wctype functions don't work until the locale has been set
	// to something other than the default "C".  Then, not only Latin1 but also the
	// other characters give the correct results with iswalpha() etc.
#ifdef PLATFORM_RISCOS
   setlocale(LC_CTYPE,"ISO8859-1");
#else
	if(setlocale(LC_CTYPE,"en_US.UTF-8") == NULL)
		setlocale(LC_CTYPE,"german");
#endif


	init_path();
	initialise();

// buflength is in mS, allocate 2 bytes per sample
	outbuf_size = (buf_length * samplerate)/500;
	outbuf = (unsigned char*)realloc(outbuf,outbuf_size);

	// allocate space for event list.  Allow 500 events per minute
	n_event_list = (buf_length*500)/1000;
	event_list = (espeak_EVENT *)realloc(event_list,sizeof(espeak_EVENT) * n_event_list);

	option_waveout = 1;
	option_phonemes = 0;

	LoadVoice("default",0);

	for(param=0; param<N_SPEECH_PARAM; param++)
		param_stack[0].parameter[param] = param_defaults[param];

	espeak_SetParameter(espeakRATE,165,0);
	espeak_SetParameter(espeakVOLUME,100,0);
	espeak_SetParameter(espeakCAPITALS,option_capitals,0);
	espeak_SetParameter(espeakPUNCTUATION,option_punctuation,0);
	WavegenSetVoice(voice);

	return(samplerate);
}


int espeak_Synth(const void *text, unsigned int position, espeak_POSITION_TYPE position_type,
	unsigned int end_position, unsigned int flags)
{//=====================================================================================
	InitText();

	switch(position_type)
	{
	case POS_CHARACTER:
		skip_characters = position;
		break;

	case POS_WORD:
		skip_words = position;
		break;

	case POS_SENTENCE:
		skip_sentences = position;
		break;

	}
	if(skip_characters || skip_words || skip_sentences)
		skipping_text = 1;

	end_character_position = end_position;

	return(Synthesize(text,flags));
}  //  end of espeak_Synth


int espeak_Synth_Mark(const void *text, const char *index_mark,
		unsigned int end_position, unsigned int flags)
{//=========================================================================
	InitText();

	if(index_mark != NULL)
	{
		strncpy0(skip_marker, index_mark, sizeof(skip_marker));
		skipping_text = 1;
	}

	end_character_position = end_position;

	return(Synthesize(text, flags | espeakSSML));
}  //  end of espeak_Synth_Mark



void espeak_Key(const char *key)
{//=============================
// symbolic name, symbolicname_character  - is there a system resource of symbolicnames per language
	Synthesize(key,0);  // for now, just say the key name as passed
}


void espeak_Char(wchar_t character)
{//================================
// is there a system resource of character names per language?
	char buf[80];
	
	sprintf(buf,"<say-as interpret-as=\"tts:char\">&#%d;</say-as>",character);
	Synthesize(buf,espeakSSML);
}


int espeak_GetParameter(int parameter, int current)
{//================================================
// current: 0=default value, 1=current value
	if(current)
	{
		return(param_stack[0].parameter[parameter]);
	}
	else
	{
		return(param_defaults[parameter]);
	}
}  //  end of espeak_GetParameter


void espeak_SetPunctuationList(wchar_t *punctlist)
{//===============================================
// Set the list of punctuation which are spoken for "some".
	wcsncpy(option_punctlist, punctlist, N_PUNCTLIST);
	option_punctlist[N_PUNCTLIST-1] = 0;
}  //  end of espeak_SetPunctuationList


void espeak_SetPhonemes(int value, FILE *stream)
{//=============================================
/* Controls the output of phoneme symbols for the text
   value=0  No phoneme output (default)
   value=1  Output the translated phoneme symbols for the text
   value=2  as (1), but also output a trace of how the translation was done (matching rules and list entries)
*/
	option_phonemes = value;
	f_trans = stream;
	if(stream == NULL)
		f_trans = stdout;

}   //  end of espeak_SetPhonemes


void espeak_CompileDictionary(const char *path, FILE *log)
{//======================================================
	CompileDictionary(path,dictionary_name,log,NULL);
}   //  end of espeak_CompileDirectory


#pragma GCC visibility pop

