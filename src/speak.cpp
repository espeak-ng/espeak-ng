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
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <signal.h>
#include <locale.h>
#include "sys/stat.h"

#include "speech.h"
#include "voice.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"
#include "speak_lib.h"



char path_home[120];
char wavefile[120];
int (* uri_callback)(int, const char *, const char *) = NULL;


static const char *version = "Speak text-to-speech: 1.16  02.Oct.06";

static const char *help_text =
"\nspeak [options] [\"<words>\"]\n\n"
"-f <text file>   Text file to speak\n"
"--stdin    Read text input from stdin instead of a file\n\n"
"If neither -f nor --stdin, <words> are spoken, or if none then text is\n"
"spoken from stdin, each line separately.\n\n"
"-a <integer>\n"
"\t   Amplitude, 0 to 200, default is 100\n"
"-l <integer>\n"
"\t   Line length. If not zero (which is the default), consider\n"
"\t   lines less than this length as and-of-clause\n"
"-p <integer>\n"
"\t   Pitch adjustment, 0 to 99, default is 50\n"
"-s <integer>\n"
"\t   Speed in words per minute, default is 160\n"
"-v <voice name>\n"
"\t   Use voice file of this name from espeak-data/voices\n"
"-w <wave file name>\n"
"\t   Write output to this WAV file, rather than speaking it directly\n"
"-m\t   Interpret SSML markup, and ignore other < > tags\n"
"-q\t   Quiet, don't produce any speech (may be useful with -x)\n"
"-x\t   Write phoneme mnemonics to stdout\n"
"-X\t   Write phonemes mnemonics and translation trace to stdout\n"
"--stdout   Write speech output to stdout\n"
"--compile=<voice name>\n"
"\t   Compile the pronunciation rules and dictionary in the current\n"
"\t   directory. =<voice name> is optional and specifies which language\n"
"--punct=\"<characters>\"\n"
"\t   Speak the names of punctuation characters during speaking.  If\n"
"\t   =<characters> is omitted, all punctuation is spoken.\n"
"--voices=<langauge>\n"
"\t   List the available voices for the specified language.\n"
"\t   If <language> is omitted, then list all voices.\n";


#ifdef deleted
"-k <integer>\n"
"\t   Indicate capital letters with: 1=sound, 2=the word \"capitals\",\n"
"\t   higher values = a pitch increase (try -k20).\n";
#endif


void DisplayVoices(FILE *f_out, espeak_VOICE **voices_list, char *language);

voice_t voice_data;
USHORT voice_pcnt[N_PEAKS+1][3];
voice_t *voice;



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





static void PitchAdjust(int pitch_adjustment)
{//==========================================
	int ix, factor;
	extern unsigned char pitch_adjust_tab[100];

	voice->pitch_base = (voice->pitch_base * pitch_adjust_tab[pitch_adjustment])/128;

	// adjust formants to give better results for a different voice pitch
	factor = 256 + (25 * (pitch_adjustment - 50))/50;
	for(ix=0; ix<=5; ix++)
	{
		voice->freq[ix] = (voice->freq2[ix] * factor)/256;
	}
}  //  end of PitchAdjustment


void MarkerEvent(int type, unsigned int char_position, int value, unsigned char *out_ptr)
{//======================================================================================
// Do nothing in the command-line version.
}  // end of MarkerEvent



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
}


static int initialise(void)
{//========================
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


static void StopSpeak(int unused)
{//==============================
	signal(SIGINT,SIG_IGN);
	// DEBUG
//	printf("\n*** Interrupting speech output (use Ctrl-D to actually quit).\n");
	fflush(stdout);
	SpeakNextClause(NULL,NULL,5);
	signal(SIGINT,StopSpeak);
}  //  end of StopSpeak()



int main (int argc, char **argv)
//==============================
{
	static struct option long_options[] =
		{
		/* These options set a flag. */
//		{"verbose", no_argument,       &verbose_flag, 1},
//		{"brief",   no_argument,       &verbose_flag, 0},

		/* These options don't set a flag.
			We distinguish them by their indices. */
		{"help",    no_argument,       0, 'h'},
		{"stdin",   no_argument,       0, 0x100},
		{"stdout",  no_argument,       0, 0x101},
		{"compile", optional_argument, 0, 0x102},
		{"punct",   optional_argument, 0, 0x103},
		{"voices",  optional_argument, 0, 0x104},
		{0, 0, 0, 0}
		};

	FILE *f_text=NULL;
	const char *p_text=NULL;

	int option_index = 0;
	int c;
	int value;
	int speed=170;
	int ix;
	int amp = 100;     // default
	int speaking = 0;
	int quiet = 0;
	int flag_stdin = 0;
	int flag_compile = 0;
	int pitch_adjustment = 50;
	char filename[120];
	char voicename[40];
	char dictname[40];

	voicename[0] = 0;
	dictname[0] = 0;
	wavefile[0] = 0;
	filename[0] = 0;
	option_linelength = 0;
	option_phonemes = 0;
	option_waveout = 0;
	option_multibyte = 0;  // auto
	f_trans = stdout;

	init_path();

	while(true)
	{
		c = getopt_long (argc, argv, "a:f:hk:l:p:qs:v:w:xXm",
					long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c)
		{
		case 'h':
			printf("\n");
			printf("%s\n%s",version,help_text);
			exit(0);
			break;

		case 'k':
			option_capitals = atoi(optarg);
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
			pitch_adjustment = atoi(optarg);
			if(pitch_adjustment > 99) pitch_adjustment = 99;
			break;

		case 'q':
			quiet = 1;
			break;

		case 'f':
			strncpy0(filename,optarg,sizeof(filename));
			break;

		case 'l':
			value = 0;
			value = atoi(optarg);
			option_linelength = value;
			break;

		case 'a':
			amp = atoi(optarg);
			break;

		case 's':
			speed = atoi(optarg);
			break;

		case 'v':
			strncpy0(voicename,optarg,sizeof(voicename));
			break;

		case 'w':
			option_waveout = 1;
			strncpy0(wavefile,optarg,sizeof(filename));
			break;

		case 0x100:		// --stdin
			flag_stdin = 1;
			break;

		case 0x101:		// --stdout
			option_waveout = 1;
			strcpy(wavefile,"stdout");
			break;

		case 0x102:		// --compile
			if(optarg != NULL)
				strncpy0(voicename,optarg,sizeof(voicename));
			flag_compile = 1;
			break;

		case 0x103:		// --punct
			option_punctuation = 1;
			if(optarg != NULL)
			{
				ix = 0;
				while((ix < N_PUNCTLIST) && ((option_punctlist[ix] = optarg[ix]) != 0)) ix++;
				option_punctlist[N_PUNCTLIST-1] = 0;
				option_punctuation = 2;
			}
			break;

		case 0x104:   // --voices
			DisplayVoices(stdout,espeak_ListVoices(),optarg);
			exit(0);

		default:
			abort();
		}
	}

	initialise();

	if((LoadVoice(voicename,0) == NULL) && (flag_compile == 0))
	{
		fprintf(stderr,"Failed to load voice '%s'\n",voicename);
		exit(2);
	}

	if(flag_compile)
	{
		CompileDictionary(NULL,dictionary_name,NULL,NULL);
		exit(0);
	}

	espeak_SetParameter(espeakRATE,speed,0);
	espeak_SetParameter(espeakVOLUME,amp,0);
	espeak_SetParameter(espeakCAPITALS,option_capitals,0);
	espeak_SetParameter(espeakPUNCTUATION,option_punctuation,0);

	if(pitch_adjustment != 50)
	{
		PitchAdjust(pitch_adjustment);
	}
	WavegenSetVoice(voice);

	if(filename[0]==0)
	{
		if((optind < argc) && (flag_stdin == 0))
		{
			// there's a non-option parameter, and no -f or --stdin
			// use it as text
			p_text = argv[optind];
//			f_text = fmemopen(argv[optind],strlen(argv[optind]),"r");
		}
		else
		{
			f_text = stdin;
			if(flag_stdin == 0)
				option_linelength = -1;  // single input lines on stdin
		}
	}
	else
	{
		f_text = fopen(filename,"r");
	}

	if((f_text == NULL) && (p_text == NULL))
	{
		fprintf(stderr,"Failed to read file '%s'\n",filename);
		exit(1);
	}

	if(option_waveout || quiet)
	{
		if(quiet)
		{
			// no sound output
			OpenWaveFile(NULL,samplerate);
			option_waveout = 1;
		}
		else
		{
			// write sound output to a WAV file
			if(OpenWaveFile(wavefile,samplerate) != 0)
			{
				fprintf(stderr,"Can't write to output file '%s'\n'",wavefile);
				exit(3);
			}
		}

		InitText();
		SpeakNextClause(f_text,p_text,0);

		for(;;)
		{
			if(WavegenFile() != 0)
				break;   // finished, wavegen command queue is empty

			if(Generate(phoneme_list,n_phoneme_list,1)==0)
				SpeakNextClause(NULL,NULL,1);
		}

		CloseWaveFile(samplerate);
	}
	else
	{
		// Silence on ^C or SIGINT
//		signal(SIGINT,StopSpeak);

		// output sound using portaudio
		WavegenInitSound();

		InitText();
		SpeakNextClause(f_text,p_text,0);

#ifdef USE_PORTAUDIO
		speaking = 1;
		while(speaking)
		{
			// NOTE: if nanosleep() isn't recognised on your system, try replacing
			// this by  sleep(1);
#ifdef USE_NANOSLEEP
			struct timespec period;
			struct timespec remaining;
			period.tv_sec = 0;
			period.tv_nsec = 300000000;  // 0.3 sec
			nanosleep(&period,&remaining);
#else
			sleep(1);
#endif
			if(SynthOnTimer() != 0)
				speaking = 0;
		}
#endif
	}
	exit(0);
}
