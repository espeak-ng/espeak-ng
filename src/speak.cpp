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
#include "sys/stat.h"

#include "speech.h"
#include "voice.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"

int VoicePhonemes(const char *name);


FILE *f_text;

char path_home[120];
char path_source[80] = "";
char wavefile[120];

static const char *version = "Speak text-to-speech: 1.09g  27.Apr.2006";

static const char *help_text =
"\nspeak [options] [\"<words>\"]\n\n"
"-f <text file>   Text file to speak\n"
"--stdin    Read text input from stdin instead of a file\n\n"
"If neither -f nor --stdin, <words> are spoken, or if none then text is\n"
"spoken from stdin, each line separately.\n\n"
"-q\t   Quiet, don't produce any speech (may be useful with -x)\n"
"-a <integer>\n"
"\t   Amplitude, 0 to 20, default is 10\n"
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
"-x\t   Write phoneme mnemonics to stdout\n"
"-X\t   Write phonemes mnemonics and translation trace to stdout\n"
"--stdout   Write speech output to stdout\n"
"--compile=<voice name>\n"
"\t   Compile the pronunciation rules and dictionary in the current\n"
"\t   directory. =<voice name> is optional and specifies which language\n"
"--punct=\"<characters>\"\n"
"\t   Speak the names of punctuation characters during speaking.  If\n"
"\t   =<characters> is omitted, all punctuation is spoken.\n"
"-k <integer>\n"
"\t   Indicate capital letters with: 1=sound, 2=the word \"capitals\",\n"
"\t   higher values = a pitch increase (try -k20).\n";


voice_t voice_data;
USHORT voice_pcnt[N_PEAKS+1][3];
voice_t *voice;


int GetFileLength(const char *filename)
{//====================================
	struct stat statbuf;

	stat(filename,&statbuf);
	return(statbuf.st_size);
}


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





void PitchAdjust(int pitch_adjustment)
{//===================================
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



int initialise(void)
{//=================
	sprintf(path_home,"%s/espeak-data",getenv("HOME"));
	if(access(path_home,R_OK) != 0)
	{
		strcpy(path_home,"/usr/share/espeak-data");
	}

	WavegenInit(22050,0);   // 22050
	LoadPhData();
	LoadConfig();
	SynthesizeInit();
	return(0);
}




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
		{0, 0, 0, 0}
		};

	int option_index = 0;
	int c;
	int value;
	int amp = 10;     // default
	int speaking = 0;
	int quiet = 0;
	int flag_stdin = 0;
	int flag_compile = 0;
	int pitch_adjustment = 50;
	int error;
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
	option_utf8 = 2;
	f_trans = NULL;

	while(true)
	{
		c = getopt_long (argc, argv, "a:f:hk:l:p:qs:v:w:xX",
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
			f_trans = stdout;
			break;

		case 'p':
			pitch_adjustment = atoi(optarg);
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
			global_speed = atoi(optarg);
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
			if(optarg != NULL)
				strncpy0(option_punctlist,optarg,sizeof(option_punctlist));
			option_punctuation = 1;
			break;

		default:
			abort();
		}
	}

	initialise();

	if(LoadVoice(voicename,0) == NULL)
	{
		fprintf(stderr,"Failed to load voice '%s'\n",voicename);
		exit(2);
	}

	if(flag_compile)
	{
		CompileDictionary(dictionary_name,0);
		exit(0);
	}
	SetSpeed(global_speed,3);
	SetAmplitude(amp);

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
			f_text = fmemopen(argv[optind],strlen(argv[optind]),"r");
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

	if(f_text == NULL)
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

		SpeakNextClause(f_text,NULL,0);

		for(;;)
		{
			if(WavegenFile() != 0)
				break;   // finished, wavegen command queue is empty

			if(Generate(phoneme_list,1)==0)
				SpeakNextClause(NULL,NULL,1);
		}

		CloseWaveFile(samplerate);
	}
	else
	{
		// output sound using portaudio
		WavegenInitSound();

		SpeakNextClause(f_text,NULL,0);

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
	}
	exit(0);
}
