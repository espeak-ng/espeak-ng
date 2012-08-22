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

#include "speech.h"
#include "voice.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"

extern int VoicePhonemes(const char *name);
extern void SetSpeed(int speed, int amp);


int verbose_flag;
FILE *f_text;

char path_home[120];
char path_source[80] = "";
char wavefile[120];

const char *version = "Speak text-to-speech: 1.05  16.Feb.2006";

const char *help_text =
"-f <text file>\n"
"\t   Text file to speak\n"
"--stdin\n"
"\t   Read text input from stdin instead of a file\n"
"   If neither -f nor --stdin, text is spoken from stdin, each line separately.\n\n"
"-p\t   Write phoneme mnemonics to stdout\n"
"-P\t   Write phonemes mnemonics and translation trace to stdout\n"
"-q\t   Quiet, don't produce any speech (can use with -p)\n"
"-a <integer>\n"
"\t   Amplitude, 0 to 20, default is 10\n"
"-l <integer>\n"
"\t   Line length. If not zero (which is the default), consider\n"
"\t   lines less than this length as and-of-clause\n"
"-s <integer>\n"
"\t   Speed in words per minute, default is 160\n"
"-v <voice name>\n"
"\t   Use voice file of this name from speak-data/voices\n"
"-w <wave file name>\n"
"\t   Write output to this WAV file, rather than speaking it directly\n"
"--compile=<voice name>\n"
"\t   Compile the pronunciation rules and dictionary in the current\n"
"\t   directory. =<voice name> is optional and specifies which language\n";

voice_t voice_data;
voice_t *voice;


static void VoiceSetup()
{//=====================
	int pk;
	voice_t *v = &voice_data;

	// modifications to formants, set up the data which is used by wavegen 
	for(pk=0; pk<N_PEAKS; pk++)
	{
		v->freq[pk] = int(v->pcnt[pk][0] * 2.56001 * v->pcnt[N_PEAKS][0] / 100.0);
		v->height[pk] = int(v->pcnt[pk][1] * 2.56001 * v->pcnt[N_PEAKS][1] / 100.0);
		v->width[pk] = int(v->pcnt[pk][2] * 2.56001 * v->pcnt[N_PEAKS][2] / 100.0);
	}
}


static void VoiceInit()
{//====================
	int  pk, j;
	voice_t *v;

	v = &voice_data;

	v->pitch_base = 71 << 12;
	v->pitch_range =  0x1000;     // default = 0x1000
	
	// initialise formant modifications to 100%
	for(pk=0; pk<N_PEAKS+1; pk++)
	{
		for(j=0; j<3; j++)
			v->pcnt[pk][j] = 100;
	}
	VoiceSetup();

	voice = v;
}  // end of voice_init



int initialise(void)
{//=================
	sprintf(path_home,"%s/speak-data",getenv("HOME"));
	if(access(path_home,R_OK) != 0)
	{
		strcpy(path_home,"/usr/share/speak-data");
	}

	WavegenInit(22050,0);
	VoiceInit();
	LoadPhData();
	synth = new Synthesize();
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
		{"compile", optional_argument,       0, 0x101},
		{0, 0, 0, 0}
		};

	int option_index = 0;
	int c;
	int value;
	int speed = 160;  // default
	int amp = 10;     // default
	int speaking = 0;
	int quiet = 0;
	int flag_stdin = 0;
	int flag_compile = 0;
	int error;
	char filename[120];
	char voicename[40];
	char dictname[40];
	struct timespec period;
	struct timespec remaining;

	voicename[0] = 0;
	dictname[0] = 0;
	wavefile[0] = 0;
	filename[0] = 0;
	option_linelength = 0;
	option_phonemes = 0;
	option_waveout = 0;
	f_trans = NULL;

	while(true)
	{
		c = getopt_long (argc, argv, "a:f:hl:pPqs:v:w:",
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

		case 'p':
			option_phonemes = 1;
			break;

		case 'P':
			option_phonemes = 2;
			f_trans = stdout;
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

		case 0x101:		// --compile
			if(optarg != NULL)
				strncpy0(voicename,optarg,sizeof(voicename));
			flag_compile = 1;
			break;

		default:
			abort();
		}
	}
	
	initialise();

	if((error = LoadVoice(voicename,0)) != 0)
	{
		fprintf(stderr,"Failed to load voice '%s'\n",voicename);
		if(error < 0)
			exit(error);
	}

	if(flag_compile)
	{
		CompileDictionary(dictionary_name,0);
		exit(0);
	}
	SetSpeed(speed,amp);

	if(filename[0]==0)
	{
		f_text = stdin;
		if(flag_stdin == 0)
			option_linelength = -1;  // single input lines on stdin
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

	if((option_waveout) || quiet)
	{
		if(quiet)
		{
			// no sound output
			OpenWaveFile(NULL);
			option_waveout = 1;
		}
		else
		{
			// write sound output to a WAV file
			if(OpenWaveFile(wavefile) != 0)
			{
				fprintf(stderr,"Can't write to output file '%s'\n'",wavefile);
				exit(3);
			}
		}

		synth->SpeakNextClause(f_text,0);

		for(;;)
		{
			if(WavegenFile() != 0)
				break;   // finished, wavegen command queue is empty

			if(synth->Generate(phoneme_list,1)==0)
				synth->SpeakNextClause(NULL,0);
		}

		CloseWaveFile();
	}
	else
	{
		// output sound using portaudio
		WavegenInitSound();

		synth->SpeakNextClause(f_text,0);

		speaking = 1;
		while(speaking)
		{
			period.tv_sec = 0;
			period.tv_nsec = 300000000;  // 0.3 sec
			nanosleep(&period,&remaining);
			if(synth->OnTimer() != 0)
				speaking = 0;
		}
	}
	exit(0);
}
