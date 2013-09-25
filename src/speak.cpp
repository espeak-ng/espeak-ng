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

#include "speech.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef PLATFORM_DOS
#ifdef PLATFORM_WINDOWS
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#include <winreg.h>
#else
#include <unistd.h>
#endif
#endif

#ifndef NEED_GETOPT
#include <getopt.h>
#endif
#include <time.h>
#include <signal.h>
#include <locale.h>
#include <sys/stat.h>

#include "speak_lib.h"
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "translate.h"



extern void Write4Bytes(FILE *f, int value);
char path_home[N_PATH_HOME];    // this is the espeak-data directory

char filetype[5];
char wavefile[200];
int (* uri_callback)(int, const char *, const char *) = NULL;
int (* phoneme_callback)(const char *) = NULL;

FILE *f_wave = NULL;
int quiet = 0;
unsigned int samples_total = 0;
unsigned int samples_split = 0;
unsigned int wavefile_count = 0;
int end_of_sentence = 0;

static const char *help_text =
"\nspeak [options] [\"<words>\"]\n\n"
"-f <text file>   Text file to speak\n"
"--stdin    Read text input from stdin instead of a file\n\n"
"If neither -f nor --stdin, then <words> are spoken, or if none then text\n"
"is spoken from stdin, each line separately.\n\n"
"-a <integer>\n"
"\t   Amplitude, 0 to 200, default is 100\n"
"-g <integer>\n"
"\t   Word gap. Pause between words, units of 10mS at the default speed\n"
"-k <integer>\n"
"\t   Indicate capital letters with: 1=sound, 2=the word \"capitals\",\n"
"\t   higher values indicate a pitch increase (try -k20).\n"
"-l <integer>\n"
"\t   Line length. If not zero (which is the default), consider\n"
"\t   lines less than this length as end-of-clause\n"
"-p <integer>\n"
"\t   Pitch adjustment, 0 to 99, default is 50\n"
"-s <integer>\n"
"\t   Speed in words per minute, 80 to 450, default is 175\n"
"-v <voice name>\n"
"\t   Use voice file of this name from espeak-data/voices\n"
"-w <wave file name>\n"
"\t   Write speech to this WAV file, rather than speaking it directly\n"
"-b\t   Input text encoding, 1=UTF8, 2=8 bit, 4=16 bit \n"
"-m\t   Interpret SSML markup, and ignore other < > tags\n"
"-q\t   Quiet, don't produce any speech (may be useful with -x)\n"
"-x\t   Write phoneme mnemonics to stdout\n"
"-X\t   Write phonemes mnemonics and translation trace to stdout\n"
"-z\t   No final sentence pause at the end of the text\n"
"--compile=<voice name>\n"
"\t   Compile pronunciation rules and dictionary from the current\n"
"\t   directory. <voice name> specifies the language\n"
"--ipa      Write phonemes to stdout using International Phonetic Alphabet\n"
"\t         --ipa=1 Use ties, --ipa=2 Use ZWJ, --ipa=3 Separate with _\n" 
"--path=\"<path>\"\n"
"\t   Specifies the directory containing the espeak-data directory\n"
"--pho      Write mbrola phoneme data (.pho) to stdout or to the file in --phonout\n"
"--phonout=\"<filename>\"\n"
"\t   Write phoneme output from -x -X --ipa and --pho to this file\n"
"--punct=\"<characters>\"\n"
"\t   Speak the names of punctuation characters during speaking.  If\n"
"\t   =<characters> is omitted, all punctuation is spoken.\n"
"--split=\"<minutes>\"\n"
"\t   Starts a new WAV file every <minutes>.  Used with -w\n"
"--stdout   Write speech output to stdout\n"
"--version  Shows version number and date, and location of espeak-data\n"
"--voices=<language>\n"
"\t   List the available voices for the specified language.\n"
"\t   If <language> is omitted, then list all voices.\n";


void DisplayVoices(FILE *f_out, char *language);

USHORT voice_pcnt[N_PEAKS+1][3];



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


void DisplayVoices(FILE *f_out, char *language)
{//============================================
	int ix;
	const char *p;
	int len;
	int count;
	int c;
	int j;
	const espeak_VOICE *v;
	const char *lang_name;
	char age_buf[12];
	char buf[80];
	const espeak_VOICE **voices;
	espeak_VOICE voice_select;

	static char genders[4] = {'-','M','F','-'};

	if((language != NULL) && (language[0] != 0))
	{
		// display only voices for the specified language, in order of priority
		voice_select.languages = language;
		voice_select.age = 0;
		voice_select.gender = 0;
		voice_select.name = NULL;
		voices = espeak_ListVoices(&voice_select);
	}
	else
	{
		voices = espeak_ListVoices(NULL);
	}

	fprintf(f_out,"Pty Language Age/Gender VoiceName          File          Other Languages\n");

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
				for(j=0; j < sizeof(buf); j++)
				{
					// replace spaces in the name
					if((c = v->name[j]) == ' ')
						c = '_';
					if((buf[j] = c) == 0)
						break;
				}
				fprintf(f_out,"%2d  %-12s%s%c  %-20s %-13s ",
               p[0],lang_name,age_buf,genders[v->gender],buf,v->identifier);
			}
			else
			{
				fprintf(f_out,"(%s %d)",lang_name,p[0]);
			}
			count++;
			p += len+2;
		}
		fputc('\n',f_out);
	}
}   //  end of DisplayVoices


void WVoiceChanged(voice_t *wvoice)
{
}

static int OpenWaveFile(const char *path, int rate)
//=================================================
{
	// Set the length of 0x7ffff000 for --stdout
	// This will be changed to the correct length for -w (write to file)
	static unsigned char wave_hdr[44] = {
		'R','I','F','F',0x24,0xf0,0xff,0x7f,'W','A','V','E','f','m','t',' ',
		0x10,0,0,0,1,0,1,0,  9,0x3d,0,0,0x12,0x7a,0,0,
		2,0,0x10,0,'d','a','t','a',  0x00,0xf0,0xff,0x7f};

	if(path == NULL)
		return(2);

	while(isspace(*path)) path++;

	f_wave = NULL;
	if(path[0] != 0)
	{
		if(strcmp(path,"stdout")==0)
		{
#ifdef PLATFORM_WINDOWS
// prevent Windows adding 0x0d before 0x0a bytes
			_setmode(_fileno(stdout), _O_BINARY);
#endif
			f_wave = stdout;
		}
		else
			f_wave = fopen(path,"wb");
	}

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




static void CloseWaveFile()
//=========================
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




void MarkerEvent(int type, unsigned int char_position, int value, int value2, unsigned char *out_ptr)
{//======================================================================================
// Do nothing in the command-line version.
	if(type == 2)
		end_of_sentence = 1;
}  // end of MarkerEvent


static int WavegenFile(void)
{//=========================
	int finished;
	unsigned char wav_outbuf[1024];
	char fname[210];

	out_ptr = out_start = wav_outbuf;
	out_end = wav_outbuf + sizeof(wav_outbuf);

	finished = WavegenFill(0);

	if(quiet)
		return(finished);

	if(f_wave == NULL)
	{
		sprintf(fname,"%s_%.2d%s",wavefile,++wavefile_count,filetype);
		if(OpenWaveFile(fname, samplerate) != 0)
			return(1);
	}

	if(end_of_sentence)
	{
		end_of_sentence = 0;
		if((samples_split > 0 ) && (samples_total > samples_split))
		{
			CloseWaveFile();
			samples_total = 0;
		}
	}

	if(f_wave != NULL)
	{
		samples_total += (out_ptr - wav_outbuf)/2;
		fwrite(wav_outbuf, 1, out_ptr - wav_outbuf, f_wave);
	}
	return(finished);
}  // end of WavegenFile



static void init_path(char *argv0, char *path_specified)
{//=====================================================

	if(path_specified)
	{
		sprintf(path_home,"%s/espeak-data",path_specified);
		return;
	}

#ifdef PLATFORM_WINDOWS
	HKEY RegKey;
	unsigned long size;
	unsigned long var_type;
	char *p;
	char *env;
	unsigned char buf[sizeof(path_home)-12];

	if(((env = getenv("ESPEAK_DATA_PATH")) != NULL) && ((strlen(env)+12) < sizeof(path_home)))
	{
		sprintf(path_home,"%s\\espeak-data",env);
		if(GetFileLength(path_home) == -2)
			return;   // an espeak-data directory exists in the directory specified by environment variable
	}

	strcpy(path_home,argv0);
	if((p = strrchr(path_home,'\\')) != NULL)
	{
		strcpy(&p[1],"espeak-data");
		if(GetFileLength(path_home) == -2)
			return;   // an espeak-data directory exists in the same directory as the espeak program
	}

	// otherwise, look in the Windows Registry
	buf[0] = 0;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Speech\\Voices\\Tokens\\eSpeak", 0, KEY_READ, &RegKey);
	size = sizeof(buf);
	var_type = REG_SZ;
	RegQueryValueEx(RegKey, "path", 0, &var_type, buf, &size);

	sprintf(path_home,"%s\\espeak-data",buf);
#else
#ifdef PLATFORM_DOS
		strcpy(path_home,PATH_ESPEAK_DATA);
#else
	char *env;
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
#endif
}


static int initialise(void)
{//========================
	int param;
	int result;
	int srate = 22050;   // default sample rate

	// It seems that the wctype functions don't work until the locale has been set
	// to something other than the default "C".  Then, not only Latin1 but also the
	// other characters give the correct results with iswalpha() etc.
#ifdef PLATFORM_RISCOS
   setlocale(LC_CTYPE,"ISO8859-1");
#else
	if(setlocale(LC_CTYPE,"en_US.UTF-8") == NULL)
	{
		if(setlocale(LC_CTYPE,"UTF-8") == NULL)
			setlocale(LC_CTYPE,"");
	}
#endif


	if((result = LoadPhData(&srate)) != 1)
	{
		if(result == -1)
		{
			fprintf(stderr,"Failed to load espeak-data\n");
			exit(1);
		}
		else
			fprintf(stderr,"Wrong version of espeak-data 0x%x (expects 0x%x) at %s\n",result,version_phdata,path_home);
	}
	WavegenInit(srate,0);
	LoadConfig();
	SetVoiceStack(NULL, "");
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



#ifdef NEED_GETOPT
	struct option {
		char *name;
		int has_arg;
		int *flag;
		int val;
	};
	int optind;
	static int optional_argument;
	static const char *arg_opts = "abfgklpsvw";  // which options have arguments
	static char *opt_string="";
#define no_argument 0
#define required_argument 1
#define optional_argument 2
#endif

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
		{"compile-debug", optional_argument, 0, 0x101},
		{"compile", optional_argument, 0, 0x102},
		{"punct",   optional_argument, 0, 0x103},
		{"voices",  optional_argument, 0, 0x104},
		{"stdout",  no_argument,       0, 0x105},
		{"split",   optional_argument, 0, 0x106},
		{"path",    required_argument, 0, 0x107},
		{"phonout", required_argument, 0, 0x108},
		{"pho",     no_argument,       0, 0x109},
		{"ipa",     optional_argument, 0, 0x10a},
		{"version", no_argument,       0, 0x10b},
		{0, 0, 0, 0}
		};

	static const char *err_load = "Failed to read ";

	FILE *f_text=NULL;
	const char *p_text=NULL;
	char *data_path = NULL;   // use default path for espeak-data

	int option_index = 0;
	int c;
	int value;
	int speed=175;
	int ix;
	char *optarg2;
	int amp = 100;     // default
	int wordgap = 0;
	int speaking = 0;
	int flag_stdin = 0;
	int flag_compile = 0;
	int pitch_adjustment = 50;
	espeak_VOICE voice_select;
	char filename[200];
	char voicename[40];

	voicename[0] = 0;
	mbrola_name[0] = 0;
	wavefile[0] = 0;
	filename[0] = 0;
	option_linelength = 0;
	option_phonemes = 0;
	option_waveout = 0;
	option_wordgap = 0;
	option_endpause = 1;
	option_phoneme_input = 1;
	option_multibyte = espeakCHARS_AUTO;  // auto
	f_trans = stdout;

#ifdef NEED_GETOPT
	optind = 1;
	opt_string = "";
	while(optind < argc)
	{
		int len;
		char *p;

		if((c = *opt_string) == 0)
		{
			opt_string = argv[optind];
			if(opt_string[0] != '-')
				break;

			optind++;
			opt_string++;
			c = *opt_string;
		}
		opt_string++;
		p = optarg2 = opt_string;

		if(c == '-')
		{
			if(p[0] == 0)
				break;   // -- means don't interpret further - as commands

			opt_string="";
			for(ix=0; ;ix++)
			{
				if(long_options[ix].name == 0)
					break;
				len = strlen(long_options[ix].name);
				if(memcmp(long_options[ix].name,p,len)==0)
				{
					c = long_options[ix].val;
					optarg2 = NULL;

					if((long_options[ix].has_arg != 0) && (p[len]=='='))
					{
						optarg2 = &p[len+1];
					}
					break;
				}
			}
		}
		else
		if(strchr(arg_opts,c) != NULL)
		{
			opt_string="";
			if(optarg2[0]==0)
			{
				// the option's value is in the next argument
				optarg2 = argv[optind++];
			}
		}
#else
	while(true)
	{
		c = getopt_long (argc, argv, "a:b:f:g:hk:l:p:qs:v:w:xXmz",   // NOTE: also change arg_opts to indicate which commands have a numeric value
					long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;
		optarg2 = optarg;
#endif

		switch (c)
		{
		case 'b':
			// input character encoding, 8bit, 16bit, UTF8
			option_multibyte = espeakCHARS_8BIT;
			if((sscanf(optarg2,"%d",&value) == 1) && (value <= 4))
				option_multibyte= value;
			break;

		case 'h':
			init_path(argv[0],data_path);
			printf("\nspeak text-to-speech: %s   Data at: %s\n%s",version_string,path_home,help_text);
			exit(0);

		case 'k':
			option_capitals = atoi(optarg2);
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
			pitch_adjustment = atoi(optarg2);
			if(pitch_adjustment > 99) pitch_adjustment = 99;
			break;

		case 'q':
			quiet = 1;
			break;

		case 'f':
			strncpy0(filename,optarg2,sizeof(filename));
			break;

		case 'l':
			value = 0;
			value = atoi(optarg2);
			option_linelength = value;
			break;

		case 'a':
			amp = atoi(optarg2);
			break;

		case 's':
			speed = atoi(optarg2);
			break;

		case 'g':
			wordgap = atoi(optarg2);
			break;

		case 'v':
			strncpy0(voicename,optarg2,sizeof(voicename));
			break;

		case 'w':
			option_waveout = 1;
			strncpy0(wavefile,optarg2,sizeof(wavefile));
			break;

		case 'z':
			option_endpause = 0;
			break;

		case 0x100:		// --stdin
			flag_stdin = 1;
			break;

		case 0x105:		// --stdout
			option_waveout = 1;
			strcpy(wavefile,"stdout");
			break;

		case 0x101:    // --compile-debug
		case 0x102:		// --compile
			if(optarg2 != NULL)
				strncpy0(voicename,optarg2,sizeof(voicename));
			flag_compile = c;
			break;

		case 0x103:		// --punct
			option_punctuation = 1;
			if(optarg2 != NULL)
			{
				ix = 0;
				while((ix < N_PUNCTLIST) && ((option_punctlist[ix] = optarg2[ix]) != 0)) ix++;
				option_punctlist[N_PUNCTLIST-1] = 0;
				option_punctuation = 2;
			}
			break;

		case 0x104:   // --voices
			init_path(argv[0],data_path);
			DisplayVoices(stdout,optarg2);
			exit(0);

		case 0x106:   // -- split
			if(optarg2 == NULL)
				samples_split = 30;  // default 30 minutes
			else
				samples_split = atoi(optarg2);
			break;

		case 0x107:  // --path
			data_path = optarg2;
			break;

		case 0x108:  // --phonout
			if((f_trans = fopen(optarg2,"w")) == NULL)
			{
				fprintf(stderr,"Can't write to: %s\n",optarg2);
				f_trans = stderr;
			}
			break;

		case 0x109:  // --pho
			option_mbrola_phonemes = 16;
			break;

		case 0x10a:  // --ipa
			option_phonemes = 3;
			if(optarg2 != NULL)
			{
				value = -1;
				sscanf(optarg2,"%d",&value);
				if((value<0) || (value>3))
				{
					fprintf(stderr,"Bad value for -ipa=\n");
					value = 0;
				}
				option_phonemes += value;
			}
			break;

		case 0x10b:  // --version
			init_path(argv[0],data_path);
			printf("speak text-to-speech: %s   Data at: %s\n",version_string,path_home);
			exit(0);

		default:
			exit(0);
		}
	}

	init_path(argv[0],data_path);
	initialise();

	if(voicename[0] == 0)
		strcpy(voicename,"default");

	if(SetVoiceByName(voicename) != EE_OK)
	{
		memset(&voice_select,0,sizeof(voice_select));
		voice_select.languages = voicename;
		if(SetVoiceByProperties(&voice_select) != EE_OK)
		{
			fprintf(stderr,"%svoice '%s'\n",err_load,voicename);
			exit(2);
		}
	}

	if(flag_compile)
	{
#ifdef PLATFORM_DOS
		char path_dsource[sizeof(path_home)+20];
		strcpy(path_dsource,path_home);
		path_dsource[strlen(path_home)-11] = 0;  // remove "espeak-data" from the end
		strcat(path_dsource,"dictsource\\");
		CompileDictionary(path_dsource,dictionary_name,NULL,NULL, flag_compile & 0x1);
#else
#ifdef PLATFORM_WINDOWS
		char path_dsource[sizeof(path_home)+20];
		strcpy(path_dsource,path_home);
		path_dsource[strlen(path_home)-11] = 0;  // remove "espeak-data" from the end
		strcat(path_dsource,"dictsource\\");
		CompileDictionary(path_dsource,dictionary_name,NULL,NULL, flag_compile & 0x1);
#else
		CompileDictionary(NULL,dictionary_name,NULL,NULL, flag_compile & 0x1);
#endif
#endif
		exit(0);
	}


	SetParameter(espeakRATE,speed,0);
	SetParameter(espeakVOLUME,amp,0);
	SetParameter(espeakCAPITALS,option_capitals,0);
	SetParameter(espeakPUNCTUATION,option_punctuation,0);
	SetParameter(espeakWORDGAP,wordgap,0);

	if(pitch_adjustment != 50)
	{
		SetParameter(espeakPITCH,pitch_adjustment,0);
	}
	DoVoiceChange(voice);

	if(filename[0]==0)
	{
		if((optind < argc) && (flag_stdin == 0))
		{
			// there's a non-option parameter, and no -f or --stdin
			// use it as text
			p_text = argv[optind];
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
		fprintf(stderr,"%sfile '%s'\n",err_load,filename);
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
			samples_split = (samplerate * samples_split) * 60;

			if(samples_split)
			{
				// don't open the wav file until we start generating speech
				char *extn;
				extn = strrchr(wavefile,'.');
				if((extn != NULL) && ((wavefile + strlen(wavefile) - extn) <= 4))
				{
					strcpy(filetype,extn);
					*extn = 0;
				}
			}
			else
			if(OpenWaveFile(wavefile,samplerate) != 0)
			{
				fprintf(stderr,"Can't write to output file '%s'\n'",wavefile);
				exit(3);
			}
		}

		InitText(0);
		SpeakNextClause(f_text,p_text,0);

		ix = 1;
		for(;;)
		{
			if(WavegenFile() != 0)
			{
				if(ix == 0)
					break;   // finished, wavegen command queue is empty
			}

			if(Generate(phoneme_list,&n_phoneme_list,1)==0)
			{
				ix = SpeakNextClause(NULL,NULL,1);
			}
		}

		CloseWaveFile();
	}
	else
	{
		// Silence on ^C or SIGINT
//		signal(SIGINT,StopSpeak);

		// output sound using portaudio
		WavegenInitSound();

		InitText(0);
		SpeakNextClause(f_text,p_text,0);

		if(option_quiet)
		{
			while(SpeakNextClause(NULL,NULL,1) != 0);
			return(0);
		}

#ifdef USE_PORTAUDIO
		speaking = 1;
		while(speaking)
		{
			// NOTE: if nanosleep() isn't recognised on your system, try replacing
			// this by  sleep(1);
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
				speaking = 0;
		}
#else
		fprintf(stderr,"-w option must be used because the program was built without a sound interface\n");
#endif  // USE_PORTAUDIO
	}

	if((f_trans != stdout) && (f_trans != stderr))
		fclose(f_trans);  // needed for WinCe
	return(0);
}
