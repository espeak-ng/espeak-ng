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

#include "stdio.h"
#include "ctype.h"
#include "string.h"
#include "stdlib.h"
#include "wchar.h"
#include "locale.h"
#include <assert.h>
#include <time.h>

#include "speech.h"

#include <sys/stat.h>
#ifdef PLATFORM_WINDOWS
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#include <winreg.h>
#else  /* PLATFORM_POSIX */
#include <unistd.h>
#endif

#include "speak_lib.h"
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "translate.h"
#include "debug.h"

#include "fifo.h"
#include "event.h"
#include "wave.h"

unsigned char *outbuf=NULL;

espeak_EVENT *event_list=NULL;
int event_list_ix=0;
int n_event_list;
long count_samples;
void* my_audio=NULL;

static unsigned int my_unique_identifier=0;
static void* my_user_data=NULL;
static espeak_AUDIO_OUTPUT my_mode=AUDIO_OUTPUT_SYNCHRONOUS;
static int synchronous_mode = 1;
static int out_samplerate = 0;
static int voice_samplerate = 22050;
static espeak_ERROR err = EE_OK;

t_espeak_callback* synth_callback = NULL;
int (* uri_callback)(int, const char *, const char *) = NULL;
int (* phoneme_callback)(const char *) = NULL;

char path_home[N_PATH_HOME];   // this is the espeak-data directory
extern int saved_parameters[N_SPEECH_PARAM]; //Parameters saved on synthesis start


void WVoiceChanged(voice_t *wvoice)
{//=================================
// Voice change in wavegen
	voice_samplerate = wvoice->samplerate;
}


#ifdef USE_ASYNC

static int dispatch_audio(short* outbuf, int length, espeak_EVENT* event)
{//======================================================================
	ENTER("dispatch_audio");

	int a_wave_can_be_played = fifo_is_command_enabled();

#ifdef DEBUG_ENABLED
	SHOW("*** dispatch_audio > uid=%d, [write=%p (%d bytes)], sample=%d, a_wave_can_be_played = %d\n",
			(event) ? event->unique_identifier : 0, wave_test_get_write_buffer(), 2*length,
			(event) ? event->sample : 0,
			a_wave_can_be_played);
#endif

	switch(my_mode)
	{
	case AUDIO_OUTPUT_PLAYBACK:
	{
		int event_type=0;
		if(event)
		{
			event_type = event->type;
		}

		if(event_type == espeakEVENT_SAMPLERATE)
		{
			voice_samplerate = event->id.number;

			if(out_samplerate != voice_samplerate)
			{
				if(out_samplerate != 0)
				{
					// sound was previously open with a different sample rate
					wave_close(my_audio);
					sleep(1);
				}
				out_samplerate = voice_samplerate;
				if(!wave_init(voice_samplerate))
				{
					err = EE_INTERNAL_ERROR;
					return(-1);
				}
				wave_set_callback_is_output_enabled( fifo_is_command_enabled);
				my_audio = wave_open("alsa");
				event_init();
			}
		}

		if (outbuf && length && a_wave_can_be_played)
		{
			wave_write (my_audio, (char*)outbuf, 2*length);
		}

		while(a_wave_can_be_played) {
			// TBD: some event are filtered here but some insight might be given
			// TBD: in synthesise.cpp for avoiding to create WORDs with size=0.
			// TBD: For example sentence "or ALT)." returns three words
			// "or", "ALT" and "".
			// TBD: the last one has its size=0.
			if (event && (event->type == espeakEVENT_WORD) && (event->length==0))
			{
				break;
			}
			espeak_ERROR a_error = event_declare(event);
			if (a_error != EE_BUFFER_FULL)
			{
				break;
			}
			SHOW_TIME("dispatch_audio > EE_BUFFER_FULL\n");
			usleep(10000);
			a_wave_can_be_played = fifo_is_command_enabled();
		}
	}
	break;

	case AUDIO_OUTPUT_RETRIEVAL:
		if (synth_callback)
		{
			synth_callback(outbuf, length, event);
		}
		break;

	case AUDIO_OUTPUT_SYNCHRONOUS:
	case AUDIO_OUTPUT_SYNCH_PLAYBACK:
		break;
	}

	if (!a_wave_can_be_played)
	{
		SHOW_TIME("dispatch_audio > synth must be stopped!\n");
	}

	SHOW_TIME("LEAVE dispatch_audio\n");

	return (a_wave_can_be_played==0); // 1 = stop synthesis, -1 = error
}



static int create_events(short* outbuf, int length, espeak_EVENT* event, uint32_t the_write_pos)
{//=====================================================================
	int finished;
	int i=0;

	// The audio data are written to the output device.
	// The list of events in event_list (index: event_list_ix) is read:
	// Each event is declared to the "event" object which stores them internally.
	// The event object is responsible of calling the external callback
	// as soon as the relevant audio sample is played.

	do
	{ // for each event
		espeak_EVENT* event;
		if (event_list_ix == 0)
		{
			event = NULL;
		}
      else
		{
			event = event_list + i;
#ifdef DEBUG_ENABLED
			SHOW("Synthesize: event->sample(%d) + %d = %d\n", event->sample, the_write_pos, event->sample + the_write_pos);
#endif
			event->sample += the_write_pos;
		}
#ifdef DEBUG_ENABLED
		SHOW("*** Synthesize: i=%d (event_list_ix=%d), length=%d\n",i,event_list_ix,length);
#endif
		finished = dispatch_audio((short *)outbuf, length, event);
		length = 0; // the wave data are played once.
		i++;
	} while((i < event_list_ix) && !finished);
	return finished;
}


int sync_espeak_terminated_msg( uint32_t unique_identifier, void* user_data)
{//=====================================================================
	ENTER("sync_espeak_terminated_msg");

	int finished=0;

	memset(event_list, 0, 2*sizeof(espeak_EVENT));

	event_list[0].type = espeakEVENT_MSG_TERMINATED;
	event_list[0].unique_identifier = unique_identifier;
	event_list[0].user_data = user_data;
	event_list[1].type = espeakEVENT_LIST_TERMINATED;
	event_list[1].unique_identifier = unique_identifier;
	event_list[1].user_data = user_data;

	if (my_mode==AUDIO_OUTPUT_PLAYBACK)
	{
		while(1)
		{
			espeak_ERROR a_error = event_declare(event_list);
			if (a_error != EE_BUFFER_FULL)
			{
		 		break;
			}
			SHOW_TIME("sync_espeak_terminated_msg > EE_BUFFER_FULL\n");
			usleep(10000);
		}
	}
	else
	{
		if (synth_callback)
		{
			finished=synth_callback(NULL,0,event_list);
		}
	}
	return finished;
}

#endif


static void select_output(espeak_AUDIO_OUTPUT output_type)
{//=======================================================
	my_mode = output_type;
	my_audio = NULL;
	synchronous_mode = 1;
 	option_waveout = 1;   // inhibit portaudio callback from wavegen.cpp
	out_samplerate = 0;

	switch(my_mode)
	{
	case AUDIO_OUTPUT_PLAYBACK:
		// wave_init() is now called just before the first wave_write()
		synchronous_mode = 0;
		break;

	case AUDIO_OUTPUT_RETRIEVAL:
		synchronous_mode = 0;
		break;

	case AUDIO_OUTPUT_SYNCHRONOUS:
		break;

	case AUDIO_OUTPUT_SYNCH_PLAYBACK:
		option_waveout = 0;
		WavegenInitSound();
		break;
	}
}   // end of select_output




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
		fprintf(stderr,"Can't allocate memory\n");  // I was told that size+1 fixes a crash on 64-bit systems
	return(p);
}

void Free(void *ptr)
{//=================
	if(ptr != NULL)
		free(ptr);
}



static void init_path(const char *path)
{//====================================
#ifdef PLATFORM_WINDOWS
	HKEY RegKey;
	unsigned long size;
	unsigned long var_type;
	char *env;
	unsigned char buf[sizeof(path_home)-13];

	if(path != NULL)
	{
		sprintf(path_home,"%s/espeak-data",path);
		return;
	}

	if((env = getenv("ESPEAK_DATA_PATH")) != NULL)
	{
		sprintf(path_home,"%s/espeak-data",env);
		if(GetFileLength(path_home) == -2)
			return;   // an espeak-data directory exists
	}

	buf[0] = 0;
	RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Speech\\Voices\\Tokens\\eSpeak", 0, KEY_READ, &RegKey);
	size = sizeof(buf);
	var_type = REG_SZ;
	RegQueryValueExA(RegKey, "path", 0, &var_type, buf, &size);

	sprintf(path_home,"%s\\espeak-data",buf);

#else
	char *env;

	if(path != NULL)
	{
		snprintf(path_home,sizeof(path_home),"%s/espeak-data",path);
		return;
	}

	// check for environment variable
	if((env = getenv("ESPEAK_DATA_PATH")) != NULL)
	{
		snprintf(path_home,sizeof(path_home),"%s/espeak-data",env);
		if(GetFileLength(path_home) == -2)
			return;   // an espeak-data directory exists
	}

	snprintf(path_home,sizeof(path_home),"%s/espeak-data",getenv("HOME"));
	if(access(path_home,R_OK) != 0)
	{
		strcpy(path_home,PATH_ESPEAK_DATA);
	}
#endif
}

static int initialise(int control)
{//===============================
	int param;
	int result;
	int srate = 22050;  // default sample rate 22050 Hz

	err = EE_OK;
	LoadConfig();

	if((result = LoadPhData(&srate)) != 1)  // reads sample rate from espeak-data/phontab
	{
		if(result == -1)
		{
			fprintf(stderr,"Failed to load espeak-data\n");
			if((control & espeakINITIALIZE_DONT_EXIT) == 0)
			{
				exit(1);
			}
		}
		else
			fprintf(stderr,"Wrong version of espeak-data 0x%x (expects 0x%x) at %s\n",result,version_phdata,path_home);
	}
	WavegenInit(srate,0);

	memset(&current_voice_selected,0,sizeof(current_voice_selected));
	SetVoiceStack(NULL, "");
	SynthesizeInit();
	InitNamedata();

	for(param=0; param<N_SPEECH_PARAM; param++)
		param_stack[0].parameter[param] = param_defaults[param];

	return(0);
}


static espeak_ERROR Synthesize(unsigned int unique_identifier, const void *text, int flags)
{//========================================================================================
	// Fill the buffer with output sound
	int length;
	int finished = 0;
	int count_buffers = 0;
#ifdef USE_ASYNC
	uint32_t a_write_pos=0;
#endif

#ifdef DEBUG_ENABLED
	ENTER("Synthesize");
	if (text)
	{
	SHOW("Synthesize > uid=%d, flags=%d, >>>text=%s<<<\n", unique_identifier, flags, text);
	}
#endif

	if((outbuf==NULL) || (event_list==NULL))
		return(EE_INTERNAL_ERROR);  // espeak_Initialize()  has not been called

	option_multibyte = flags & 7;
	option_ssml = flags & espeakSSML;
	option_phoneme_input = flags & espeakPHONEMES;
	option_endpause = flags & espeakENDPAUSE;

	count_samples = 0;

#ifdef USE_ASYNC
	if(my_mode == AUDIO_OUTPUT_PLAYBACK)
	{
		a_write_pos = wave_get_write_position(my_audio);
	}
#endif

	if(translator == NULL)
	{
		SetVoiceByName("default");
	}

	SpeakNextClause(NULL,text,0);

	if(my_mode == AUDIO_OUTPUT_SYNCH_PLAYBACK)
	{
		for(;;)
		{
#ifdef PLATFORM_WINDOWS
			Sleep(300);   // 0.3s
#else
#ifdef USE_NANOSLEEP
			struct timespec period;
			struct timespec remaining;
			period.tv_sec = 0;
			period.tv_nsec = 300000000;  // 0.3 sec
			nanosleep(&period,&remaining);
#else
			sleep(1);
#endif
#endif
			if(SynthOnTimer() != 0)
				break;
		}
		return(EE_OK);
	}

	for(;;)
	{
#ifdef DEBUG_ENABLED
		SHOW("Synthesize > %s\n","for (next)");
#endif
		out_ptr = outbuf;
		out_end = &outbuf[outbuf_size];
		event_list_ix = 0;
		WavegenFill(0);

		length = (out_ptr - outbuf)/2;
		count_samples += length;
		event_list[event_list_ix].type = espeakEVENT_LIST_TERMINATED; // indicates end of event list
		event_list[event_list_ix].unique_identifier = my_unique_identifier;
		event_list[event_list_ix].user_data = my_user_data;

		count_buffers++;
		if (my_mode==AUDIO_OUTPUT_PLAYBACK)
		{
#ifdef USE_ASYNC
			finished = create_events((short *)outbuf, length, event_list, a_write_pos);
			if(finished < 0)
				return EE_INTERNAL_ERROR;
			length = 0; // the wave data are played once.
#endif
		}
		else
		{
			finished = synth_callback((short *)outbuf, length, event_list);
		}
		if(finished)
		{
			SpeakNextClause(NULL,0,2);  // stop
			break;
		}

		if(Generate(phoneme_list,&n_phoneme_list,1)==0)
		{
			if(WcmdqUsed() == 0)
			{
				// don't process the next clause until the previous clause has finished generating speech.
				// This ensures that <audio> tag (which causes end-of-clause) is at a sound buffer boundary

				event_list[0].type = espeakEVENT_LIST_TERMINATED;
				event_list[0].unique_identifier = my_unique_identifier;
				event_list[0].user_data = my_user_data;

				if(SpeakNextClause(NULL,NULL,1)==0)
				{
#ifdef USE_ASYNC
					if (my_mode==AUDIO_OUTPUT_PLAYBACK)
					{
						if(dispatch_audio(NULL, 0, NULL) < 0) // TBD: test case
							return err = EE_INTERNAL_ERROR;
					}
					else
					{
						synth_callback(NULL, 0, event_list);  // NULL buffer ptr indicates end of data
					}
#else
					synth_callback(NULL, 0, event_list);  // NULL buffer ptr indicates end of data
#endif
					break;
				}
			}
		}
	}
	return(EE_OK);
}  //  end of Synthesize

#ifdef DEBUG_ENABLED
static const char* label[] = {
  "END_OF_EVENT_LIST",
  "WORD",
  "SENTENCE",
  "MARK",
  "PLAY",
  "END",
  "MSG_TERMINATED",
  "PHONEME",
  "SAMPLERATE",
  "??" };
#endif


void MarkerEvent(int type, unsigned int char_position, int value, int value2, unsigned char *out_ptr)
{//==================================================================================================
	// type: 1=word, 2=sentence, 3=named mark, 4=play audio, 5=end, 7=phoneme
	ENTER("MarkerEvent");
	espeak_EVENT *ep;
	double time;

	if((event_list == NULL) || (event_list_ix >= (n_event_list-2)))
		return;

	ep = &event_list[event_list_ix++];
	ep->type = (espeak_EVENT_TYPE)type;
	ep->unique_identifier = my_unique_identifier;
	ep->user_data = my_user_data;
	ep->text_position = char_position & 0xffffff;
	ep->length = char_position >> 24;

	time = (double(count_samples + mbrola_delay + (out_ptr - out_start)/2)*1000.0)/samplerate;
	ep->audio_position = int(time);
	ep->sample = (count_samples + mbrola_delay + (out_ptr - out_start)/2);

#ifdef DEBUG_ENABLED
	SHOW("MarkerEvent > count_samples=%d, out_ptr=%x, out_start=0x%x\n",count_samples, out_ptr, out_start);
	SHOW("*** MarkerEvent > type=%s, uid=%d, text_pos=%d, length=%d, audio_position=%d, sample=%d\n",
			label[ep->type], ep->unique_identifier, ep->text_position, ep->length,
			ep->audio_position, ep->sample);
#endif

	if((type == espeakEVENT_MARK) || (type == espeakEVENT_PLAY))
		ep->id.name = &namedata[value];
	else
//#ifdef deleted
// temporarily removed, don't introduce until after eSpeak version 1.46.02
	if(type == espeakEVENT_PHONEME)
	{
		int *p;
		p = (int *)(ep->id.string);
		p[0] = value;
		p[1] = value2;
	}
	else
//#endif
	{
		ep->id.number = value;
	}
}  //  end of MarkerEvent




espeak_ERROR sync_espeak_Synth(unsigned int unique_identifier, const void *text, size_t size,
		      unsigned int position, espeak_POSITION_TYPE position_type,
		      unsigned int end_position, unsigned int flags, void* user_data)
{//===========================================================================

#ifdef DEBUG_ENABLED
	ENTER("sync_espeak_Synth");
	SHOW("sync_espeak_Synth > position=%d, position_type=%d, end_position=%d, flags=%d, user_data=0x%x, text=%s\n", position, position_type, end_position, flags, user_data, text);
#endif

	espeak_ERROR aStatus;

	InitText(flags);
	my_unique_identifier = unique_identifier;
	my_user_data = user_data;

	for (int i=0; i < N_SPEECH_PARAM; i++)
		saved_parameters[i] = param_stack[0].parameter[i];

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

	aStatus = Synthesize(unique_identifier, text, flags);
	#ifdef USE_ASYNC
	wave_flush(my_audio);
	#endif

	SHOW_TIME("LEAVE sync_espeak_Synth");
	return aStatus;
}  //  end of sync_espeak_Synth




espeak_ERROR sync_espeak_Synth_Mark(unsigned int unique_identifier, const void *text, size_t size,
			   const char *index_mark, unsigned int end_position,
			   unsigned int flags, void* user_data)
{//=========================================================================
	espeak_ERROR aStatus;

	InitText(flags);

	my_unique_identifier = unique_identifier;
	my_user_data = user_data;

	if(index_mark != NULL)
		{
		strncpy0(skip_marker, index_mark, sizeof(skip_marker));
		skipping_text = 1;
		}

	end_character_position = end_position;


	aStatus = Synthesize(unique_identifier, text, flags | espeakSSML);
	SHOW_TIME("LEAVE sync_espeak_Synth_Mark");

	return (aStatus);
}  //  end of sync_espeak_Synth_Mark



void sync_espeak_Key(const char *key)
{//==================================
	// symbolic name, symbolicname_character  - is there a system resource of symbolic names per language?
	int letter;
	int ix;

	ix = utf8_in(&letter,key);
	if(key[ix] == 0)
	{
		// a single character
		sync_espeak_Char(letter);
		return;
	}

	my_unique_identifier = 0;
	my_user_data = NULL;
	Synthesize(0, key,0);   // speak key as a text string
}


void sync_espeak_Char(wchar_t character)
{//=====================================
	// is there a system resource of character names per language?
	char buf[80];
	my_unique_identifier = 0;
	my_user_data = NULL;

	sprintf(buf,"<say-as interpret-as=\"tts:char\">&#%d;</say-as>",character);
	Synthesize(0, buf,espeakSSML);
}



void sync_espeak_SetPunctuationList(const wchar_t *punctlist)
{//==========================================================
	// Set the list of punctuation which are spoken for "some".
	my_unique_identifier = 0;
	my_user_data = NULL;

	option_punctlist[0] = 0;
	if(punctlist != NULL)
	{
		wcsncpy(option_punctlist, punctlist, N_PUNCTLIST);
		option_punctlist[N_PUNCTLIST-1] = 0;
	}
}  //  end of sync_espeak_SetPunctuationList




#pragma GCC visibility push(default)


ESPEAK_API void espeak_SetSynthCallback(t_espeak_callback* SynthCallback)
{//======================================================================
	ENTER("espeak_SetSynthCallback");
	synth_callback = SynthCallback;
#ifdef USE_ASYNC
	event_set_callback(synth_callback);
#endif
}

ESPEAK_API void espeak_SetUriCallback(int (* UriCallback)(int, const char*, const char *))
{//=======================================================================================
	ENTER("espeak_SetUriCallback");
	uri_callback = UriCallback;
}


ESPEAK_API void espeak_SetPhonemeCallback(int (* PhonemeCallback)(const char*))
{//===========================================================================
	phoneme_callback = PhonemeCallback;
}

ESPEAK_API int espeak_Initialize(espeak_AUDIO_OUTPUT output_type, int buf_length, const char *path, int options)
{//=============================================================================================================
ENTER("espeak_Initialize");
	int param;

	// It seems that the wctype functions don't work until the locale has been set
	// to something other than the default "C".  Then, not only Latin1 but also the
	// other characters give the correct results with iswalpha() etc.
#ifdef PLATFORM_RISCOS
	setlocale(LC_CTYPE,"ISO8859-1");
#else
	if(setlocale(LC_CTYPE,"C.UTF-8") == NULL)
	{
		if(setlocale(LC_CTYPE,"UTF-8") == NULL)
			if(setlocale(LC_CTYPE,"en_US.UTF-8") == NULL)
				setlocale(LC_CTYPE,"");
	}
#endif

	init_path(path);
	initialise(options);
	select_output(output_type);

	if(f_logespeak)
	{
		fprintf(f_logespeak,"INIT mode %d options 0x%x\n",output_type,options);
	}

	// buflength is in mS, allocate 2 bytes per sample
	if((buf_length == 0) || (output_type == AUDIO_OUTPUT_PLAYBACK) || (output_type == AUDIO_OUTPUT_SYNCH_PLAYBACK))
		buf_length = 200;

	outbuf_size = (buf_length * samplerate)/500;
	outbuf = (unsigned char*)realloc(outbuf,outbuf_size);
	if((out_start = outbuf) == NULL)
		return(EE_INTERNAL_ERROR);

	// allocate space for event list.  Allow 200 events per second.
	// Add a constant to allow for very small buf_length
	n_event_list = (buf_length*200)/1000 + 20;
	if((event_list = (espeak_EVENT *)realloc(event_list,sizeof(espeak_EVENT) * n_event_list)) == NULL)
		return(EE_INTERNAL_ERROR);

	option_phonemes = 0;
	option_mbrola_phonemes = 0;
	option_phoneme_events = (options & (espeakINITIALIZE_PHONEME_EVENTS | espeakINITIALIZE_PHONEME_IPA));

	VoiceReset(0);
//	SetVoiceByName("default");

	for(param=0; param<N_SPEECH_PARAM; param++)
		param_stack[0].parameter[param] = saved_parameters[param] = param_defaults[param];

	SetParameter(espeakRATE,175,0);
	SetParameter(espeakVOLUME,100,0);
	SetParameter(espeakCAPITALS,option_capitals,0);
	SetParameter(espeakPUNCTUATION,option_punctuation,0);
	SetParameter(espeakWORDGAP,0,0);
//	DoVoiceChange(voice);

#ifdef USE_ASYNC
	fifo_init();
#endif

  return(samplerate);
}



ESPEAK_API espeak_ERROR espeak_Synth(const void *text, size_t size,
				     unsigned int position,
				     espeak_POSITION_TYPE position_type,
				     unsigned int end_position, unsigned int flags,
				     unsigned int* unique_identifier, void* user_data)
{//=====================================================================================
#ifdef DEBUG_ENABLED
	ENTER("espeak_Synth");
	SHOW("espeak_Synth > position=%d, position_type=%d, end_position=%d, flags=%d, user_data=0x%x, text=%s\n", position, position_type, end_position, flags, user_data, text);
#endif

	if(f_logespeak)
	{
		fprintf(f_logespeak,"\nSYNTH posn %d %d %d flags 0x%x\n%s\n",position,end_position,position_type,flags, (const char *)text);
		fflush(f_logespeak);
	}

	espeak_ERROR a_error=EE_INTERNAL_ERROR;
	static unsigned int temp_identifier;

	if (unique_identifier == NULL)
	{
		unique_identifier = &temp_identifier;
	}
	*unique_identifier = 0;

	if(synchronous_mode)
	{
		return(sync_espeak_Synth(0,text,size,position,position_type,end_position,flags,user_data));
	}

#ifdef USE_ASYNC
	// Create the text command
	t_espeak_command* c1 = create_espeak_text(text, size, position, position_type, end_position, flags, user_data);

	// Retrieve the unique identifier
	*unique_identifier = c1->u.my_text.unique_identifier;

	// Create the "terminated msg" command (same uid)
	t_espeak_command* c2 = create_espeak_terminated_msg(*unique_identifier, user_data);

	// Try to add these 2 commands (single transaction)
	if (c1 && c2)
	{
		a_error = fifo_add_commands(c1, c2);
		if (a_error != EE_OK)
		{
			delete_espeak_command(c1);
			delete_espeak_command(c2);
			c1=c2=NULL;
		}
	}
	else
	{
		delete_espeak_command(c1);
		delete_espeak_command(c2);
	}

#endif
	return a_error;
}  //  end of espeak_Synth



ESPEAK_API espeak_ERROR espeak_Synth_Mark(const void *text, size_t size,
					  const char *index_mark,
					  unsigned int end_position,
					  unsigned int flags,
					  unsigned int* unique_identifier,
					  void* user_data)
{//=========================================================================
#ifdef DEBUG_ENABLED
  ENTER("espeak_Synth_Mark");
  SHOW("espeak_Synth_Mark > index_mark=%s, end_position=%d, flags=%d, text=%s\n", index_mark, end_position, flags, text);
#endif

	espeak_ERROR a_error=EE_OK;
	static unsigned int temp_identifier;

	if(f_logespeak)
	{
		fprintf(f_logespeak,"\nSYNTH MARK %s posn %d flags 0x%x\n%s\n",index_mark,end_position,flags, (const char *)text);
	}


	if (unique_identifier == NULL)
	{
		unique_identifier = &temp_identifier;
	}
	*unique_identifier = 0;

	if(synchronous_mode)
	{
		return(sync_espeak_Synth_Mark(0,text,size,index_mark,end_position,flags,user_data));
	}

#ifdef USE_ASYNC
	// Create the mark command
	t_espeak_command* c1 = create_espeak_mark(text, size, index_mark, end_position,
						flags, user_data);

	// Retrieve the unique identifier
	*unique_identifier = c1->u.my_mark.unique_identifier;

	// Create the "terminated msg" command (same uid)
	t_espeak_command* c2 = create_espeak_terminated_msg(*unique_identifier, user_data);

	// Try to add these 2 commands (single transaction)
	if (c1 && c2)
	{
		a_error = fifo_add_commands(c1, c2);
		if (a_error != EE_OK)
		{
			delete_espeak_command(c1);
			delete_espeak_command(c2);
			c1=c2=NULL;
		}
	}
	else
	{
		delete_espeak_command(c1);
		delete_espeak_command(c2);
	}

#endif
	return a_error;
}  //  end of espeak_Synth_Mark



ESPEAK_API espeak_ERROR espeak_Key(const char *key)
{//================================================
	ENTER("espeak_Key");
	// symbolic name, symbolicname_character  - is there a system resource of symbolicnames per language

	if(f_logespeak)
	{
		fprintf(f_logespeak,"\nKEY %s\n",key);
	}

	espeak_ERROR a_error = EE_OK;

	if(synchronous_mode)
	{
		sync_espeak_Key(key);
		return(EE_OK);
	}

#ifdef USE_ASYNC
	t_espeak_command* c = create_espeak_key( key, NULL);
	a_error = fifo_add_command(c);
	if (a_error != EE_OK)
	{
		delete_espeak_command(c);
	}

#endif
  return a_error;
}


ESPEAK_API espeak_ERROR espeak_Char(wchar_t character)
{//===========================================
  ENTER("espeak_Char");
  // is there a system resource of character names per language?

	if(f_logespeak)
	{
		fprintf(f_logespeak,"\nCHAR U+%x\n",character);
	}

#ifdef USE_ASYNC
	espeak_ERROR a_error;

	if(synchronous_mode)
	{
		sync_espeak_Char(character);
		return(EE_OK);
	}

	t_espeak_command* c = create_espeak_char( character, NULL);
	a_error = fifo_add_command(c);
	if (a_error != EE_OK)
	{
		delete_espeak_command(c);
	}
	return a_error;
#else
	sync_espeak_Char(character);
	return(EE_OK);
#endif
}


ESPEAK_API espeak_ERROR espeak_SetVoiceByName(const char *name)
{//============================================================
  ENTER("espeak_SetVoiceByName");

//#ifdef USE_ASYNC
// I don't think there's a need to queue change voice requests
#ifdef deleted
	espeak_ERROR a_error;

	if(synchronous_mode)
	{
		return(SetVoiceByName(name));
	}

	t_espeak_command* c = create_espeak_voice_name(name);
	a_error = fifo_add_command(c);
	if (a_error != EE_OK)
	{
		delete_espeak_command(c);
	}
	return a_error;
#else
	return(SetVoiceByName(name));
#endif
}  // end of espeak_SetVoiceByName



ESPEAK_API espeak_ERROR espeak_SetVoiceByProperties(espeak_VOICE *voice_selector)
{//==============================================================================
  ENTER("espeak_SetVoiceByProperties");

//#ifdef USE_ASYNC
#ifdef deleted
	espeak_ERROR a_error;

	if(synchronous_mode)
	{
		return(SetVoiceByProperties(voice_selector));
	}

	t_espeak_command* c = create_espeak_voice_spec( voice_selector);
	a_error = fifo_add_command(c);
	if (a_error != EE_OK)
	{
		delete_espeak_command(c);
	}
	return a_error;
#else
	return(SetVoiceByProperties(voice_selector));
#endif
}  // end of espeak_SetVoiceByProperties


ESPEAK_API int espeak_GetParameter(espeak_PARAMETER parameter, int current)
{//========================================================================
	ENTER("espeak_GetParameter");
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


ESPEAK_API espeak_ERROR espeak_SetParameter(espeak_PARAMETER parameter, int value, int relative)
{//=============================================================================================
  ENTER("espeak_SetParameter");

	if(f_logespeak)
	{
		fprintf(f_logespeak,"SETPARAM %d %d %d\n",parameter,value,relative);
	}
#ifdef USE_ASYNC
	espeak_ERROR a_error;

	if(synchronous_mode)
	{
		SetParameter(parameter,value,relative);
		return(EE_OK);
	}

	t_espeak_command* c = create_espeak_parameter(parameter, value, relative);

	a_error = fifo_add_command(c);
	if (a_error != EE_OK)
	{
		delete_espeak_command(c);
	}
	return a_error;
#else
	SetParameter(parameter,value,relative);
	return(EE_OK);
#endif
}


ESPEAK_API espeak_ERROR espeak_SetPunctuationList(const wchar_t *punctlist)
{//================================================================
  ENTER("espeak_SetPunctuationList");
  // Set the list of punctuation which are spoken for "some".

#ifdef USE_ASYNC
	espeak_ERROR a_error;

	if(synchronous_mode)
	{
		sync_espeak_SetPunctuationList(punctlist);
		return(EE_OK);
	}

	t_espeak_command* c = create_espeak_punctuation_list( punctlist);
	a_error = fifo_add_command(c);
	if (a_error != EE_OK)
	{
		delete_espeak_command(c);
	}
	return a_error;
#else
	sync_espeak_SetPunctuationList(punctlist);
	return(EE_OK);
#endif
}  //  end of espeak_SetPunctuationList


ESPEAK_API void espeak_SetPhonemeTrace(int value, FILE *stream)
{//============================================================
	ENTER("espeak_SetPhonemes");
	/* Controls the output of phoneme symbols for the text
		bits 0-3:
		 value=0  No phoneme output (default)
		 value=1  Output the translated phoneme symbols for the text
		 value=2  as (1), but also output a trace of how the translation was done (matching rules and list entries)
		 value=3  as (1), but produces IPA phoneme names rather than ascii
		bit 4:   produce mbrola pho data
	*/
	option_phonemes = value & 7;
	option_mbrola_phonemes = value & 16;
	f_trans = stream;
	if(stream == NULL)
		f_trans = stderr;

}   //  end of espeak_SetPhonemes


ESPEAK_API const char *espeak_TextToPhonemes(const void **textptr, int textmode, int phonememode)
{//=================================================================================================
	/* phoneme_mode  bits 0-3: 0=only phoneme names, 1=ties, 2=ZWJ, 3=underscore separator
	                 bits 4-7:   0=eSpeak phoneme names, 1=IPA
	*/

	option_multibyte = textmode & 7;
	*textptr = TranslateClause(translator, NULL, *textptr, NULL, NULL);
	return(GetTranslatedPhonemeString(phonememode));
}


ESPEAK_API void espeak_CompileDictionary(const char *path, FILE *log, int flags)
{//=============================================================================
	ENTER("espeak_CompileDictionary");
	CompileDictionary(path, dictionary_name, log, NULL, flags);
}   //  end of espeak_CompileDirectory


ESPEAK_API espeak_ERROR espeak_Cancel(void)
{//===============================
#ifdef USE_ASYNC
	ENTER("espeak_Cancel");
	fifo_stop();
	event_clear_all();

	if(my_mode == AUDIO_OUTPUT_PLAYBACK)
	{
		wave_close(my_audio);
	}
	SHOW_TIME("espeak_Cancel > LEAVE");
#endif
	embedded_value[EMBED_T] = 0;    // reset echo for pronunciation announcements

	for (int i=0; i < N_SPEECH_PARAM; i++)
		SetParameter(i, saved_parameters[i], 0);

	return EE_OK;
}   //  end of espeak_Cancel


ESPEAK_API int espeak_IsPlaying(void)
{//==================================
//	ENTER("espeak_IsPlaying");
#ifdef USE_ASYNC
	if((my_mode == AUDIO_OUTPUT_PLAYBACK) && wave_is_busy(my_audio))
		return(1);

	return(fifo_is_busy());
#else
	return(0);
#endif
}   //  end of espeak_IsPlaying


ESPEAK_API espeak_ERROR espeak_Synchronize(void)
{//=============================================
	espeak_ERROR berr = err;
#ifdef USE_ASYNC
	SHOW_TIME("espeak_Synchronize > ENTER");
	while (espeak_IsPlaying())
	{
		usleep(20000);
	}
#endif
	err = EE_OK;
	SHOW_TIME("espeak_Synchronize > LEAVE");
	return berr;
}   //  end of espeak_Synchronize


extern void FreePhData(void);
extern void FreeVoiceList(void);

ESPEAK_API espeak_ERROR espeak_Terminate(void)
{//===========================================
	ENTER("espeak_Terminate");
#ifdef USE_ASYNC
	fifo_stop();
	fifo_terminate();
	event_terminate();

	if(my_mode == AUDIO_OUTPUT_PLAYBACK)
	{
		wave_close(my_audio);
		wave_terminate();
		out_samplerate = 0;
	}

#endif
	Free(event_list);
	event_list = NULL;
	Free(outbuf);
	outbuf = NULL;
	FreePhData();
	FreeVoiceList();

	if(f_logespeak)
	{
		fclose(f_logespeak);
		f_logespeak = NULL;
	}

	return EE_OK;
}   //  end of espeak_Terminate

ESPEAK_API const char *espeak_Info(const char **ptr)
{//=================================================
	if(ptr != NULL)
	{
		*ptr = path_home;
	}
	return(version_string);
}

#pragma GCC visibility pop


