
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

#define USE_MODULE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include "kernel.h"

#include "speech.h"
#include "speak_lib.h"
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "translate.h"

#define os_X  0x20000

// interface to the assembler section
extern "C" {
extern void DMA_Handler(void);


// used from the cmhgfile
extern _kernel_oserror *user_init(char *cmd_fail, int podule_base, void *pw);
extern _kernel_oserror *swi_handler(int swi_no, int  *r, void *pw);
extern _kernel_oserror *command_handler(char *arg_string, int argc, int cmd_no, void *pw);
extern int callback_handler(_kernel_swi_regs *r, void *pw);
extern int callback_entry(_kernel_swi_regs *r, void *pw);
extern int sound_handler(_kernel_swi_regs *r, void *pw);
extern int sound_entry(_kernel_swi_regs *r, void *pw);

}

extern int Generate(PHONEME_LIST *phoneme_list, int *n_ph, int resume);
extern void RiscosOpenSound();
extern int WcmdqUsed();
extern void FreePhData();
extern void FreeDictionary();
extern void Write4Bytes(FILE *f, int value);

extern int wcmdq_head;
extern int wcmdq_tail;
extern int current_source_index;

FILE *f_text;
FILE *f_wave = NULL;
int (* uri_callback)(int, const char *, const char *) = NULL;
int (* phoneme_callback)(const char *) = NULL;

int amp = 8;     // default

char path_home[N_PATH_HOME] = "";
char wavefile[120];
char textbuffile[L_tmpnam];
int  sample_rate_index;  // current value
int  current_voice_num=0;
int n_voice_files = 0;
int n_voice_variant_files = 0;

// output sound buffer,  2 bytes per sample
static unsigned short SoundBuf[4096];
static void *module_data;
static int callback_inhibit = 0;
static int more_text=0;

#define N_VOICE_NAMES   60
#define N_VOICE_VARIANT_NAMES  30
static char *voice_names[N_VOICE_NAMES];
static char *voice_variant_names[N_VOICE_VARIANT_NAMES];

#define N_STATIC_BUF 8000
static char static_buf[N_STATIC_BUF];
static _kernel_oserror errblk;



USHORT voice_pcnt[N_PEAKS+1][3];

static const char *help_text =
"\nspeak [options] [\"<words>\"]\n\n"
"-f <text file>   Text file to speak\n"
//"--stdin    Read text input from stdin instead of a file\n\n"
"If neither -f nor --stdin, <words> are spoken, or if none then text is\n"
"spoken from stdin, each line separately.\n\n"
"-a <integer>\n"
"\t   Amplitude, 0 to 200, default is 100\n"
"-g <integer>\n"
"\t   Word gap. Pause between words, units of 10mS at the default speed\n"
"-l <integer>\n"
"\t   Line length. If not zero (which is the default), consider\n"
"\t   lines less than this length as end-of-clause\n"
"-p <integer>\n"
"\t   Pitch adjustment, 0 to 99, default is 50\n"
"-s <integer>\n"
"\t   Speed in words per minute 80 to 390, default is 170\n"
"-v <voice name>\n"
"\t   Use voice file of this name from espeak-data/voices\n"
"-w <wave file name>\n"
"\t   Write output to this WAV file, rather than speaking it directly\n"
"-b\t   Input text is 8-bit encoding\n"
"-m\t   Interpret SSML markup, and ignore other < > tags\n"
"-q\t   Quiet, don't produce any speech (may be useful with -x)\n"
"-x\t   Write phoneme mnemonics to stdout\n"
"-X\t   Write phonemes mnemonics and translation trace to stdout\n"
//"--stdout   Write speech output to stdout\n"
"--compile=<voice name>\n"
"\t   Compile the pronunciation rules and dictionary in the current\n"
"\t   directory. =<voice name> is optional and specifies which language\n"
"--punct=\"<characters>\"\n"
"\t   Speak the names of punctuation characters during speaking.  If\n"
"\t   =<characters> is omitted, all punctuation is spoken.\n"
"--voices=<language>\n"
"\t   List the available voices for the specified language.\n"
"\t   If <language> is omitted, then list all voices.\n"
"-k <integer>\n"
"\t   Indicate capital letters with: 1=sound, 2=the word \"capitals\",\n"
"\t   higher values = a pitch increase (try -k20).\n";


int GetFileLength(const char *filename)
{//====================================
	int length=0;
	int type;
	_kernel_swi_regs regs;
	_kernel_oserror *error;

	regs.r[0] = 5;
	regs.r[1] = (int)filename;
	regs.r[2] = 0;
	regs.r[3] = 0;
	regs.r[4] = 0;
	regs.r[5] = 0;

	error = _kernel_swi(0x20008,&regs,&regs);
	if(error)
		return(0);

	type = regs.r[0];
	length = regs.r[4];

	if(type==2)
		return(-2);   // a directory
	if((type!=1) && (type!=3))
		return(0);   /* not a file */

	return(length);
}   /* end of GetFileLength */



void ReadVoiceNames2(char *directory)
{//==================================
	int len;
	int path_len;
	int *type;
	char *p;
	_kernel_swi_regs regs;
	_kernel_oserror *error;
	char buf[80];
	char directory2[sizeof(path_home)+100];

	regs.r[0] = 10;
	regs.r[1] = (int)directory;
	regs.r[2] = (int)buf;
	regs.r[3] = 1;
	regs.r[4] = 0;
	regs.r[5] = sizeof(buf);
	regs.r[6] = 0;

	path_len = strlen(directory);

	while(regs.r[3] > 0)
	{
		error = _kernel_swi(0x0c+os_X,&regs,&regs);      /* OS_GBPB 10, read directory entries */
		if((error != NULL) || (regs.r[3] == 0))
		{
			break;
		}
		type = (int *)(&buf[16]);
		len = strlen(&buf[20]);

		if(*type == 2)
		{
			// a sub-directory
			sprintf(directory2,"%s.%s",directory,&buf[20]);
			ReadVoiceNames2(directory2);
		}
		else
		{
			p = (char *)malloc(len+1);
			strcpy(p,&buf[20]);

			if(strcmp(&directory[path_len-3],".!v")==0)
			{
				if(n_voice_variant_files >= (N_VOICE_VARIANT_NAMES-1))
					continue;
				voice_variant_names[n_voice_variant_files++] = p;
			}
			else
			{
				if(n_voice_files >= (N_VOICE_NAMES-1))
					continue;
				voice_names[n_voice_files++] = p;
				if(strcmp(p,"en")==0)
				{
					voice_names[n_voice_files++] = "en+f2";
				}
			}
		}
	}
}


void ReadVoiceNames()
{//===================
	char directory[sizeof(path_home)+10];

	n_voice_files = 0;
	n_voice_variant_files = 0;

	sprintf(directory,"%s.voices",path_home);

	ReadVoiceNames2(directory);

	voice_names[n_voice_files] = NULL;
	voice_variant_names[n_voice_variant_files] = NULL;
}


#ifdef USE_MODULE
char *Alloc(int size)
/*******************/
{	// version of malloc() for use in RISC_OS module
_kernel_swi_regs regs;

	regs.r[0] = 6;
	regs.r[3] = size;
	_kernel_swi(0x1e, &regs, &regs);   /* OS_Module 6   claim memory */

	return(char *)regs.r[2];
}   /* end of module_malloc */



void Free(void *ptr)
/*******************/
{  // version of free() for use in RISC_OS module
_kernel_swi_regs regs;

	if(ptr == NULL)
		return;

	regs.r[0] = 7;
	regs.r[2] = (int)(ptr);
	_kernel_swi(0x1e, &regs, &regs);    /* OS_Module 7   free memory */
}   /* end of Free */

#else

char *Alloc(int size)
{//==================
	char *p;
	if((p = (char *)malloc(size)) == NULL)
		fprintf(stderr,"Can't allocate memory\n");
	return(p);
}

void Free(void **ptr)
{//=================
	if(ptr != NULL)
	{
		free(ptr);
	}
}
#endif




static int OpenWaveFile(const char *path, int rate)
//=================================================
{
	// Set the length of 0x7fffffff for --stdout
	// This will be changed to the correct length for -w (write to file)
	static unsigned char wave_hdr[44] = {
		'R','I','F','F',0,0,0,0,'W','A','V','E','f','m','t',' ',
		0x10,0,0,0,1,0,1,0,  9,0x3d,0,0,0x12,0x7a,0,0,
		2,0,0x10,0,'d','a','t','a',  0xff,0xff,0xff,0x7f};

	if(path == NULL)
		return(2);

	if(strcmp(path,"stdout")==0)
		f_wave = stdout;
	else
		f_wave = fopen(path,"wb");

	if(f_wave != NULL)
	{
		fwrite(wave_hdr,1,24,f_wave);
		Write4Bytes(f_wave,rate);
		Write4Bytes(f_wave,rate * 2);
		fwrite(&wave_hdr[32],1,12,f_wave);
		return(0);
	}
	return(1);
}   //  end of OpenWaveFile




static void CloseWaveFile(int rate)
//=================================
{
   unsigned int pos;

   if((f_wave == NULL) || (f_wave == stdout))
      return;

   fflush(f_wave);
   pos = ftell(f_wave);

	fseek(f_wave,4,SEEK_SET);
	Write4Bytes(f_wave,pos - 8);

	fseek(f_wave,40,SEEK_SET);
	Write4Bytes(f_wave,pos - 44);


   fclose(f_wave);
   f_wave = NULL;

} // end of CloseWaveFile




void MarkerEvent(int type, unsigned int char_position, int value, unsigned char *out_ptr)
{//======================================================================================
// Do nothing in the command-line version.
}  // end of MarkerEvent


static int WavegenFile(void)
{//=========================
	int finished;
	unsigned char wav_outbuf[1024];

	out_ptr = out_start = wav_outbuf;
	out_end = wav_outbuf + sizeof(wav_outbuf);

	finished = WavegenFill(0);

	if(f_wave != NULL)
	{
		fwrite(wav_outbuf, 1, out_ptr - wav_outbuf, f_wave);
	}
	return(finished);
}  // end of WavegenFile




void FillSoundBuf(int size)
{//========================
// Fill the buffer with output sound

// size is number of samples*4

	size = size;
	if(size > sizeof(SoundBuf))
		size = sizeof(SoundBuf);

	out_ptr = (unsigned char *)(&SoundBuf[0]);
	out_end = (unsigned char *)(&SoundBuf[size]);
	WavegenFill(1);
}



int initialise(void)
{//=================
	sprintf(path_home,"%s.espeak-data","<eSpeak$Dir>");
	if(GetFileLength(path_home) != -2)
	{
		// not found, try the 10 character version of the directory name
		sprintf(path_home,"%s.espeak-dat","<eSpeak$Dir>");
	}
	if(GetFileLength(path_home) != -2)
	{
	   // still can't find data directory
	   sprintf(errblk.errmess,"Speak: Can't find data directory: '%s'\n",path_home);
	   return(-1);
	}

	WavegenInit(22050,0);
	LoadPhData();
	SetVoiceStack(NULL);
	SynthesizeInit();
	return(0);
}



void speak_text_string(char *data, int terminator, int len, int wait, int voice_num)
/**********************************************************************************/
/* 'wait' indictes wait until speaking is finished before returning */
{
	int  c;
	int ix;
	static static_length=0;
	static int user_token=0;   /* increment for each call of translate() */
	_kernel_swi_regs regs;

	regs.r[0] = (int)callback_entry;
	regs.r[1] = (int)module_data;
	_kernel_swi(0x5f, &regs, &regs);

	option_endpause = 1;

	if((voice_num >= 0) && (voice_num != current_voice_num) && (voice_num < N_VOICE_NAMES))
	{
		SetVoiceByName(voice_names[voice_num]);
	   WavegenSetVoice(voice);
	}
	current_voice_num = voice_num;

	/* don't count CR as terminator if length is specified */
	if(len > 0) terminator = 0;

	ix = 0;
	if(more_text == 0)
		static_length = 0;
	else
	{
		strcat(&static_buf[static_length]," : ");
		static_length+=3;
	}

	if(terminator == 0)
	{
		while(((c = data[ix++]) != 0) && (static_length < N_STATIC_BUF-4))
		{
			static_buf[static_length++] = c;
			if(ix == len)
				break;
		}
	}
	else
	{
		while(((c = data[ix++]) != '\r') && (c != '\n') && (c != 0) && (static_length < N_STATIC_BUF-3))
		{
			static_buf[static_length++] = c;
			if(ix == len)
				break;
		}
	}
	static_buf[static_length] = 0;

	if(option_waveout==0)
	{
		if(more_text == 0)
		{
			InitText(0);
			RiscosOpenSound();
			more_text = SpeakNextClause(NULL,(void *)static_buf,0);
		}

		while(wait)
		{
			if((more_text==0) && (wcmdq_head == wcmdq_tail))
				break;

			//we need to block to allow the callback handler to run
			regs.r[0] = 129;    // wait for key press
			regs.r[1] = 10;
			regs.r[2] = 0;
			_kernel_swi(0x06, &regs, &regs);  // OS_Byte
		}
	}
	else
	{
		more_text = 0;
		SpeakNextClause(NULL,(void *)static_buf,0);

		for(;;)
		{
			if(WavegenFile() != 0)
				break;   // finished, wavegen command queue is empty

			if(Generate(phoneme_list,&n_phoneme_list,1)==0)
				SpeakNextClause(NULL,NULL,1);
		}

		CloseWaveFile(samplerate);
	}

}   /* end of speak_text_string */




void speak_file(char *fname)
{//=========================
	FILE *f_in;
	char buf[120];

	f_in = fopen(fname,"r");
	if(f_in == NULL)
	{
		fprintf(stderr,"Can't read file: '%s'",fname);
		return;
	}
	more_text = 1;

	if(option_waveout == 0)
	{
		RiscosOpenSound();
		SpeakNextClause(f_in,NULL,0);
	}
	else
	{
		more_text = 0;
		SpeakNextClause(f_in,NULL,0);

		for(;;)
		{
			if(WavegenFile() != 0)
				break;   // finished, wavegen command queue is empty

			if(Generate(phoneme_list,&n_phoneme_list,1)==0)
				SpeakNextClause(NULL,NULL,1);
		}

		CloseWaveFile(samplerate);
	}
}



void set_say_options(int reg2, int reg3)
/**************************************/
/* Sets options from information in 'say' SWI */
/* R3 bits 0-7  stress indicator character
		bit 8     inhibit unpronouncable check */
{
	option_linelength = 0;
	option_phonemes = 0;
	option_waveout = 0;
	option_multibyte = 0;    // auto
	option_capitals = 0;
	option_punctuation = 0;
	option_punctlist[0] = 0;
}   /* end of set_say_options */



void jsd_swi_functions(int *r)
/****************************/
{
	espeak_VOICE voice_select;

	switch(r[0])
	{
	case 0:    /* major version */
		r[0] = 4;
		r[1] = 331;
		break;

	case 1:    /* register user */
		break;

	case 2:    /* deregister user */
		break;

	case 3:
//      r[0] = (int)speech_to_phonemes((char *)r[1]);
		break;

	case 4:
//      r[0] = reload_word_dict(NULL);
		break;

	case 5:   /* get table of voice names */
		r[0] = (int)voice_names;
		r[1] = (int)voice_variant_names;
		break;

	case 6:   /* update voice data, r1 = voice_number */
		if(r[1] < N_VOICE_NAMES)
		{
			SetVoiceByName(voice_names[r[1]]);
			current_voice_num = r[1];
			WavegenSetVoice(voice);
		}
		break;

	case 7:   /* load voice data */
//      init_voice((char *)r[1]);
		break;

	case 8:
		// list voices, r[1] contains optional language name (or "variant")
		voice_select.languages = (char *)r[1];
		voice_select.age = 0;
		voice_select.gender = 0;
		voice_select.name = NULL;
		r[0] = (int)espeak_ListVoices(&voice_select);
		break;

	default:
		r[0] = 0;
		r[1] = 0;
		break;
	}
}   /* end of jsd_swi_functions */




_kernel_oserror *swi_handler(int swi_no, int  *r, void *pw)
/*********************************************************/
{
	int  value;
	int  q_length;
	int  speed;
	int  amp;
	value = r[0];

	switch(swi_no)
	{

	case 0:   // ready ?
		// returns the index into the source text of the currently speaking word
       if(current_source_index > 0)
          r[1] = current_source_index-1;
       else
          r[1] = current_source_index;      /* source index */
		r[2] = 0;      /* source tag */
		r[3] = 0;    /* for future expansion */
		r[4] = 0;
		r[5] = 0;

		if(wcmdq_head == wcmdq_tail)
		{
			r[0] = -1;   /* ready, or nearly */
		}
		else
			r[0] = 0;
		break;

	case 1:  /* restore old sound channel. DO NOTHING */
		break;

	case 2:  /* miscellaneous functions */
		jsd_swi_functions(r);
		break;

	case 3:   /* speak text */
//      _kernel_irqs_on();
		set_say_options(r[2],r[3]);
		speak_text_string((char *)r[0],'\r',r[1],0,r[2]);
		break;

	case 4:   /* speak text and wait */
//      _kernel_irqs_on();         /* enable interrupts */
		set_say_options(r[2],r[3]);
		speak_text_string((char *)r[0],'\r',r[1],1,r[2]);
		break;

	case 5:   /* stop speaking */
		SpeakNextClause(NULL,NULL,2);
		more_text = 0;
		break;

	case 7:   /* pitch */
		// not implemented
		break;

	case 8:   /* speed */
		speed = (value * 160)/140;
		SetParameter(espeakRATE,speed,0);
		break;

	case 9:   /* word_gap */
		SetParameter(espeakWORDGAP,value,0);
		break;

	case 10:  /* pitch_range */
		// not implemented
		break;

	case 12:  /* reset */
		// not implemented
		break;

	case 13:  /* volume */
		amp = (value*45)/100;
		SetParameter(espeakVOLUME,amp,0);
		WavegenSetVoice(voice);
		break;
	}
	return(NULL);
}   /* end of swi_handler */



void PitchAdjust(int pitch_adjustment)
{//===================================
	int ix, factor;
	
	extern unsigned char pitch_adjust_tab[MAX_PITCH_VALUE+1];

	voice->pitch_base = (voice->pitch_base * pitch_adjust_tab[pitch_adjustment])/128;

	// adjust formants to give better results for a different voice pitch
	factor = 256 + (25 * (pitch_adjustment - 50))/50;
	for(ix=0; ix<=5; ix++)
	{
		voice->freq[ix] = (voice->freq2[ix] * factor)/256;
	}
}  //  end of PitchAdjustment



void DisplayVoices(FILE *f_out, char *language)
{//============================================
	int ix;
	const char *p;
	int len;
	int count;
	int scores = 0;
	const espeak_VOICE *v;
	const char *lang_name;
	char age_buf[12];
	const espeak_VOICE **voices;
	espeak_VOICE voice_select;

	static char genders[4] = {' ','M','F',' '};

	if(language[0] == '=')
	{
		// display only voices for the specified language, in order of priority
		voice_select.languages = &language[1];
		voice_select.age = 0;
		voice_select.gender = 0;
		voice_select.name = NULL;
		voices = espeak_ListVoices(&voice_select);
		scores = 1;
	}
	else
	{
		voices = espeak_ListVoices(NULL);
	}

	fprintf(f_out,"Pty Language Age/Gender VoiceName       File        Other Langs\n");

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
				fprintf(f_out,"%2d  %-12s%s%c  %-17s %-11s ",
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



char *param_string(char **argp)
{//============================
	char *p;
	int ix=0;
	static char buf[80];

	p = *argp;
	while(*p == ' ') p++;
	while(!isspace(*p))
		buf[ix++] = *p++;
	buf[ix]=0;

	*argp = p;
	return(buf);
}

int param_number(char **argp)
{//==========================
	int value;
	char *p;

	p = *argp;
	while(*p == ' ') p++;

	value = atoi(p);
	while(!isspace(*p)) p++;
	*argp = p;
	return(value);
}

void command_line(char *arg_string, int wait)
{//==========================================

	int option_index = 0;
	int c;
	int value;
	int speed;
	int amp;
	int wordgap;
	int speaking = 0;
	int flag_stdin = 0;
	int flag_compile = 0;
	int error;
	int pitch_adjustment = 50;
	char filename[80];
	char voicename[40];
	char command[80];
	char *p;
	int ix;

	voicename[0] = 0;
	wavefile[0] = 0;
	filename[0] = 0;
	option_linelength = 0;
	option_phonemes = 0;
	option_waveout = 0;
	option_quiet = 0;
	option_multibyte = 0;   // auto
	option_capitals = 0;
	option_punctuation = 0;
	option_punctlist[0] = 0;

	f_trans = stdout;

	p = arg_string;

	for(;;)
	{
		while(*p==' ') p++;   // skip spaces
		if(*p == '\r') break;  // end of line

		if(*p == '-')
		{
			// a command line argument
			p++;
			switch(*p++)
			{
			case 'b':
				option_multibyte = espeakCHARS_8BIT;
				break;

			case 'h':
				printf("\nspeak text-to-speech: %s\n%s",version_string,help_text);
				return;

			case 'k':
				option_capitals = param_number(&p);
				SetParameter(espeakCAPITALS,option_capitals,0);
				break;

			case 'x':
				option_phonemes = 1;
				break;

			case 'X':
				option_phonemes = 2;
				break;

			case 'm':
				option_ssml = 1;
				break;
	
			case 'p':
				pitch_adjustment = param_number(&p);
				break;

			case 'q':
				option_quiet = 1;
				break;

			case 'f':
				strncpy0(filename,param_string(&p),sizeof(filename));
				break;

			case 'l':
				option_linelength = param_number(&p);
				break;

			case 'a':
				amp = param_number(&p);
				SetParameter(espeakVOLUME,amp,0);
				break;

			case 's':
				speed = param_number(&p);
				SetParameter(espeakRATE,speed,0);
				break;

			case 'g':
				wordgap = param_number(&p);
				SetParameter(espeakWORDGAP,wordgap,0);
				break;
	
			case 'v':
				strncpy0(voicename,param_string(&p),sizeof(voicename));
				break;

			case 'w':
				option_waveout=1;
				strncpy0(wavefile,param_string(&p),sizeof(wavefile));
				break;

			case '-':
				strncpy0(command,param_string(&p),sizeof(command));
				if(memcmp(command,"compile=",8)==0)
				{
					CompileDictionary(NULL,&command[8],NULL,NULL,0);
					return;
				}
				else
				if(memcmp(command,"voices",6)==0)
				{
					DisplayVoices(stdout,&command[6]);
					return;
				}
				else
				if(strcmp(command,"help")==0)
				{
					printf("\nspeak text-to-speech: %s\n%s",version_string,help_text);
					return;
				}
				else
				if(memcmp(command,"punct",5)==0)
				{
					option_punctuation = 1;
					if((command[5]=='=') && (command[6]=='"'))
					{
						ix = 0;
						while((ix < N_PUNCTLIST) && ((option_punctlist[ix] = command[ix+7]) != 0)) ix++;
						option_punctlist[N_PUNCTLIST-1] = 0;
						option_punctuation = 2;
					}
					SetParameter(espeakPUNCTUATION,option_punctuation,0);
				}
				else
				{
					printf("Command not recognised\n");
				}
				break;

			default:
				printf("Command not recognised\n");
				break;
			}

		}
		else
		{
			break;
		}
	}

	SetVoiceByName(voicename);

	if((filename[0]==0) && (p[0]=='\r'))
	{
		// nothing to speak
		if(option_quiet)
		{
			SpeakNextClause(NULL,NULL,2);   // stop speaking
			more_text = 0;
		}
	}

	if(option_waveout || option_quiet)
	{
		// write speech to a WAV file
		if(option_quiet)
		{
			OpenWaveFile(NULL,samplerate);
			option_waveout = 2;
		}
		else
		{
			if(OpenWaveFile(wavefile,samplerate) != 0)
			{
				fprintf(stderr,"Can't write to output file '%s'\n'",wavefile);
				return;
			}
		}
	}

	if(pitch_adjustment != 50)
	{
		PitchAdjust(pitch_adjustment);
	}
	WavegenSetVoice(voice);

	if(filename[0]==0)
		speak_text_string(p,'\r',0,wait,-1);
	else
		speak_file(filename);
}



_kernel_oserror *command_handler(char *arg_string, int argc, int cmd_no, void *pw)
/********************************************************************************/
{
	switch(cmd_no)
	{
	case 0:    /* Say <string> */
		command_line(arg_string,0);  // for compatibility with speak V2
		break;

	case 1:    /* Sayw <string */
		command_line(arg_string,1);
		break;

	case 2:    /* speak [options] [<string>] */
		command_line(arg_string,0);
		break;

	}
	return(NULL);
}   /* end of cmd_handler */


// sound handler data
int current_sound_handler=0;
int prev_sound_handler=0;
int prev_sound_data=0;
int prev_sound_rate=13;
int sound_handler_changed=0;


void RiscosCloseSound()
{//====================
_kernel_swi_regs regs;
	if((sound_handler_changed) && (prev_sound_handler != (int)DMA_Handler))
	{
		// check whether current handler is ours
		regs.r[0]=0;
		_kernel_swi(0x40145,&regs,&regs);
		if(regs.r[1] == (int)DMA_Handler)
		{
			regs.r[0]=1;
			regs.r[1]=prev_sound_handler;
			regs.r[2]=prev_sound_data;
			_kernel_swi(0x40145,&regs,&regs);  // Sound LinearHandler 1

			// reset to the previous sample rate
			regs.r[0]=3;
			regs.r[1]=prev_sound_rate;
			_kernel_swi(0x40146,&regs,&regs);  // Sound_SampleRate 3

			current_sound_handler = prev_sound_handler;
			sound_handler_changed = 0;
		}
	}
}  // end of RiscosCloseSound



void RiscosOpenSound()
{//===================
	_kernel_swi_regs regs;

	if(current_sound_handler != (int)DMA_Handler)
	{
		// register the sound handler
		regs.r[0]=1;
		regs.r[1]=(int)DMA_Handler;
		regs.r[2]=(int)module_data;
		_kernel_swi(0x40145,&regs,&regs);  // Sound_LinearHandler 1
		prev_sound_handler = regs.r[1];
		prev_sound_data = regs.r[2];

		// set the sample rate
		regs.r[0]=3;
		regs.r[1]=sample_rate_index;
		regs.r[2]=0;
		_kernel_swi(0x40146,&regs,&regs);  // Sound_SampleRate
		prev_sound_rate = regs.r[1];

		current_sound_handler = (int)DMA_Handler;
		sound_handler_changed = 1;
	}
}  //  end of RiscosOpenSound





int callback_handler(_kernel_swi_regs *r, void *pw)
/*************************************************/
{
	if(Generate(phoneme_list,&n_phoneme_list,1)==0)
	{
		more_text = SpeakNextClause(NULL,NULL,1);
	}

	if((WcmdqUsed() == 0) && (more_text == 0))
	{
		RiscosCloseSound();
	}
	callback_inhibit = 0;

	return(1);
}   /* end of callback_handler */



int sound_handler(_kernel_swi_regs *r, void *pw)
/**********************************************/
{
	int n_queue;
	int size;
	int *dma_buf;
	int x;
	int ix;

	module_data = (int *)pw;
	dma_buf = (int *)r->r[1];
	size = (r->r[2] - r->r[1])/4;
	FillSoundBuf(size);

	for(ix=0; ix<size; ix++)
	{
		x = SoundBuf[ix];
		dma_buf[ix] = x + (x << 16);
	}

	n_queue = WcmdqUsed();

	r->r[0] = 0;
	if(callback_inhibit == 0)
	{
		// set a callback either:
		// - queue is low and there is more text to be processed
		// - queue is empty and no more text, so callback handler will remove the sound handler
		if(((n_queue < 20) && (more_text != 0)) ||
			((n_queue==0) && (more_text == 0)))
		{
			callback_inhibit = 1;
			r->r[0] = 1;
			r->r[1] = (int)pw;
		}
	}
	return(1);
}




int InitSound16(int sample_rate)
/******************************/
/* Find sample rate index */
{
	int  current_rate_index;  // current value

	int  sound_mode;
	int  sound_config;
	int  srate;
	int  n_srix;
	int  ix;
	_kernel_swi_regs regs;
	_kernel_oserror *error;

	sound_mode = 0;
	regs.r[0] = 0;
	error = _kernel_swi(0x40144+os_X,&regs,&regs);
	sound_mode = regs.r[0];
	sound_config = regs.r[1];

	if((error == NULL) && (sound_mode == 1))
	{
		/* 16 bit sound, find sample rate index */
		regs.r[0] = 0;
		regs.r[1] = 0;
		_kernel_swi(0x40146,&regs,&regs);
		n_srix = regs.r[1];

		regs.r[0] = 1;
		regs.r[1] = 0;
		_kernel_swi(0x40146,&regs,&regs);
		current_rate_index = regs.r[1];      // current sample rate index
		srate = regs.r[2];
		for(ix=1; ix<=n_srix; ix++)
		{
			regs.r[0] = 2;
			regs.r[1] = ix;
			_kernel_swi(0x40146,&regs,&regs);
			srate = regs.r[2];
			if(srate >= (sample_rate*1024))
			{
				return(ix);
			}
		}
	}
	return(14);       // this was the index for 22050
}   //  end of InitSound16



void RemoveCallback()
/*******************/
{
	_kernel_swi_regs regs;

	regs.r[0] = (int)callback_entry;
	regs.r[1] = (int)module_data;
	_kernel_swi(0x5f, &regs, &regs);
}


void terminate_module(void)
/*************************/
{
	RiscosCloseSound();
	RemoveCallback();
	DeleteTranslator(translator);
	FreePhData();
}   /* end of terminate_module */


void kill_module(void)
/********************/
{
	_kernel_swi_regs regs;
	regs.r[0]=4;
	regs.r[1]=(int)"Speak";
	_kernel_swi(0x1e,&regs,&regs);   /* RMKill */
}


_kernel_oserror *user_init(char *cmd_fail, int podule_base, void *pw)
/*******************************************************************/
{
	_kernel_swi_regs regs;
	_kernel_oserror *error;
	int param;

	// It seems that the wctype functions don't work until the locale has been set
	// to something other than the default "C".  Then, not only Latin1 but also the
	// other characters give the correct results with iswalpha() etc.

	static char *locale = "ISO8859-1";
   setlocale(LC_CTYPE,locale);

	module_data = pw;

	sample_rate_index = InitSound16(22050);

	if(initialise() < 0)
	{
	   // exit module, errblk.errmess is set by initialise()
		errblk.errnum = 0x101;
	   return(&errblk);
	}

	ReadVoiceNames();
	SetVoiceByName("default");

	for(param=0; param<N_SPEECH_PARAM; param++)
		param_stack[0].parameter[param] = param_defaults[param];

	SetParameter(espeakRATE,170,0);
	SetParameter(espeakVOLUME,65,0);

	WavegenSetVoice(voice);
	atexit(terminate_module);
	return(NULL);
}   /* end of user_init */


