/*
 * Copyright (C) 2005 to 2013 by Jonathan Duddington
 * email: jonsd@users.sourceforge.net
 * Copyright (C) 2015-2016 Reece H. Dunn
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see: <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <espeak-ng/espeak_ng.h>

#include "speech.h"

#include <stdbool.h>
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

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#include <time.h>
#include <signal.h>
#include <locale.h>
#include <sys/stat.h>

#include <espeak-ng/espeak_ng.h>
#include <espeak/speak_lib.h>

#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "translate.h"

extern void Write4Bytes(FILE *f, int value);
char path_home[N_PATH_HOME]; // this is the espeak-data directory

char filetype[5];
char wavefile[200];

FILE *f_wave = NULL;
int quiet = 0;
unsigned int samples_total = 0;
int samples_split = 0;
unsigned int wavefile_count = 0;
int end_of_sentence = 0;

static const char *help_text =
    "\nspeak-ng [options] [\"<words>\"]\n\n"
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
    "\t   Speed in approximate words per minute. The default is 175\n"
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
    "--compile-debug=<voice name>\n"
    "\t   Compile pronunciation rules and dictionary from the current\n"
    "\t   directory, including line numbers for use with -X.\n"
    "\t   <voice name> specifies the language\n"
    "--ipa      Write phonemes to stdout using International Phonetic Alphabet\n"
    "--path=\"<path>\"\n"
    "\t   Specifies the directory containing the espeak-data directory\n"
    "--pho      Write mbrola phoneme data (.pho) to stdout or to the file in --phonout\n"
    "--phonout=\"<filename>\"\n"
    "\t   Write phoneme output from -x -X --ipa and --pho to this file\n"
    "--punct=\"<characters>\"\n"
    "\t   Speak the names of punctuation characters during speaking.  If\n"
    "\t   =<characters> is omitted, all punctuation is spoken.\n"
    "--sep=<character>\n"
    "\t   Separate phonemes (from -x --ipa) with <character>.\n"
    "\t   Default is space, z means ZWJN character.\n"
    "--split=<minutes>\n"
    "\t   Starts a new WAV file every <minutes>.  Used with -w\n"
    "--stdout   Write speech output to stdout\n"
    "--tie=<character>\n"
    "\t   Use a tie character within multi-letter phoneme names.\n"
    "\t   Default is U+361, z means ZWJ character.\n"
    "--version  Shows version number and date, and location of espeak-data\n"
    "--voices=<language>\n"
    "\t   List the available voices for the specified language.\n"
    "\t   If <language> is omitted, then list all voices.\n";

void DisplayVoices(FILE *f_out, char *language);

USHORT voice_pcnt[N_PEAKS+1][3];

void DisplayVoices(FILE *f_out, char *language)
{
	int ix;
	const char *p;
	int len;
	int count;
	int c;
	size_t j;
	const espeak_VOICE *v;
	const char *lang_name;
	char age_buf[12];
	char buf[80];
	const espeak_VOICE **voices;
	espeak_VOICE voice_select;

	static char genders[4] = { '-', 'M', 'F', '-' };

	if ((language != NULL) && (language[0] != 0)) {
		// display only voices for the specified language, in order of priority
		voice_select.languages = language;
		voice_select.age = 0;
		voice_select.gender = 0;
		voice_select.name = NULL;
		voices = espeak_ListVoices(&voice_select);
	} else
		voices = espeak_ListVoices(NULL);

	fprintf(f_out, "Pty Language Age/Gender VoiceName          File          Other Languages\n");

	for (ix = 0; (v = voices[ix]) != NULL; ix++) {
		count = 0;
		p = v->languages;
		while (*p != 0) {
			len = strlen(p+1);
			lang_name = p+1;

			if (v->age == 0)
				strcpy(age_buf, "   ");
			else
				sprintf(age_buf, "%3d", v->age);

			if (count == 0) {
				for (j = 0; j < sizeof(buf); j++) {
					// replace spaces in the name
					if ((c = v->name[j]) == ' ')
						c = '_';
					if ((buf[j] = c) == 0)
						break;
				}
				fprintf(f_out, "%2d  %-12s%s%c  %-20s %-13s ",
				        p[0], lang_name, age_buf, genders[v->gender], buf, v->identifier);
			} else
				fprintf(f_out, "(%s %d)", lang_name, p[0]);
			count++;
			p += len+2;
		}
		fputc('\n', f_out);
	}
}

static int OpenWaveFile(const char *path, int rate)
{
	// Set the length of 0x7ffff000 for --stdout
	// This will be changed to the correct length for -w (write to file)
	static unsigned char wave_hdr[44] = {
		'R', 'I', 'F', 'F', 0x24, 0xf0, 0xff, 0x7f, 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ',
		0x10, 0, 0, 0, 1, 0, 1, 0,  9, 0x3d, 0, 0, 0x12, 0x7a, 0, 0,
		2, 0, 0x10, 0, 'd', 'a', 't', 'a',  0x00, 0xf0, 0xff, 0x7f
	};

	if (path == NULL)
		return 2;

	while (isspace(*path)) path++;

	f_wave = NULL;
	if (path[0] != 0) {
		if (strcmp(path, "stdout") == 0) {
#ifdef PLATFORM_WINDOWS
			// prevent Windows adding 0x0d before 0x0a bytes
			_setmode(_fileno(stdout), _O_BINARY);
#endif
			f_wave = stdout;
		} else
			f_wave = fopen(path, "wb");
	}

	if (f_wave != NULL) {
		fwrite(wave_hdr, 1, 24, f_wave);
		Write4Bytes(f_wave, rate);
		Write4Bytes(f_wave, rate * 2);
		fwrite(&wave_hdr[32], 1, 12, f_wave);
		return 0;
	}
	return 1;
}

static void CloseWaveFile()
{
	int pos;

	if ((f_wave == NULL) || (f_wave == stdout))
		return;

	fflush(f_wave);
	pos = ftell(f_wave);

	fseek(f_wave, 4, SEEK_SET);
	Write4Bytes(f_wave, pos - 8);

	fseek(f_wave, 40, SEEK_SET);
	Write4Bytes(f_wave, pos - 44);


	fclose(f_wave);
	f_wave = NULL;
}

static int WavegenFile(void)
{
	int finished;
	unsigned char wav_outbuf[1024];
	char fname[210];

	out_ptr = out_start = wav_outbuf;
	out_end = wav_outbuf + sizeof(wav_outbuf);

	finished = WavegenFill();

	if (quiet)
		return finished;

	if (f_wave == NULL) {
		sprintf(fname, "%s_%.2d%s", wavefile, ++wavefile_count, filetype);
		if (OpenWaveFile(fname, samplerate) != 0)
			return 1;
	}

	if (end_of_sentence) {
		end_of_sentence = 0;
		if ((samples_split > 0 ) && (samples_total > samples_split)) {
			CloseWaveFile();
			samples_total = 0;
		}
	}

	if (f_wave != NULL) {
		samples_total += (out_ptr - wav_outbuf)/2;
		fwrite(wav_outbuf, 1, (size_t)(out_ptr - wav_outbuf), f_wave);
	}
	return finished;
}

#ifndef HAVE_GETOPT_H
struct option {
	char *name;
	int has_arg;
	int *flag;
	int val;
};
int optind;
static int optional_argument;
static const char *arg_opts = "abfgklpsvw";      // which options have arguments
static char *opt_string = "";
#define no_argument 0
#define required_argument 1
#define optional_argument 2
#endif

int main(int argc, char **argv)
{
	static struct option long_options[] = {
		{ "help",    no_argument,       0, 'h' },
		{ "stdin",   no_argument,       0, 0x100 },
		{ "compile-debug", optional_argument, 0, 0x101 },
		{ "compile", optional_argument, 0, 0x102 },
		{ "punct",   optional_argument, 0, 0x103 },
		{ "voices",  optional_argument, 0, 0x104 },
		{ "stdout",  no_argument,       0, 0x105 },
		{ "split",   optional_argument, 0, 0x106 },
		{ "path",    required_argument, 0, 0x107 },
		{ "phonout", required_argument, 0, 0x108 },
		{ "pho",     no_argument,       0, 0x109 },
		{ "ipa",     optional_argument, 0, 0x10a },
		{ "version", no_argument,       0, 0x10b },
		{ "sep",     optional_argument, 0, 0x10c },
		{ "tie",     optional_argument, 0, 0x10d },
		{ "compile-mbrola", optional_argument, 0, 0x10e },
		{ "compile-intonations", no_argument, 0, 0x10f },
		{ "compile-phonemes", no_argument, 0, 0x110 },
		{ 0, 0, 0, 0 }
	};

	FILE *f_text = NULL;
	const char *p_text = NULL;
	char *data_path = NULL; // use default path for espeak-data

	int option_index = 0;
	int c;
	int value;
	int speed = 175;
	int ix;
	char *optarg2;
	int amp = 100; // default
	int wordgap = 0;
	int flag_stdin = 0;
	int flag_compile = 0;
	int pitch_adjustment = 50;
	int phoneme_options = 0;
	int phonemes_separator = 0;
	espeak_VOICE voice_select;
	char filename[200];
	char voicename[40];
	int speaking = 0;

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
	option_multibyte = espeakCHARS_AUTO;
	f_trans = stdout;

#ifndef HAVE_GETOPT_H
	optind = 1;
	opt_string = "";
	while (optind < argc) {
		int len;
		char *p;

		if ((c = *opt_string) == 0) {
			opt_string = argv[optind];
			if (opt_string[0] != '-')
				break;

			optind++;
			opt_string++;
			c = *opt_string;
		}
		opt_string++;
		p = optarg2 = opt_string;

		if (c == '-') {
			if (p[0] == 0)
				break; // -- means don't interpret further - as commands

			opt_string = "";
			for (ix = 0;; ix++) {
				if (long_options[ix].name == 0)
					break;
				len = strlen(long_options[ix].name);
				if (memcmp(long_options[ix].name, p, len) == 0) {
					c = long_options[ix].val;
					optarg2 = NULL;

					if ((long_options[ix].has_arg != 0) && (p[len] == '='))
						optarg2 = &p[len+1];
					break;
				}
			}
		} else if (strchr(arg_opts, c) != NULL) {
			opt_string = "";
			if (optarg2[0] == 0) {
				// the option's value is in the next argument
				optarg2 = argv[optind++];
			}
		}
#else
	while (true) {
		c = getopt_long(argc, argv, "a:b:f:g:hk:l:p:qs:v:w:xXmz", // NOTE: also change arg_opts to indicate which commands have a numeric value
		                long_options, &option_index);

		// Detect the end of the options.
		if (c == -1)
			break;
		optarg2 = optarg;
#endif

		switch (c)
		{
		case 'b':
			// input character encoding, 8bit, 16bit, UTF8
			option_multibyte = espeakCHARS_8BIT;
			if ((sscanf(optarg2, "%d", &value) == 1) && (value <= 4))
				option_multibyte = value;
			break;
		case 'h':
			espeak_ng_InitializePath(data_path);
			printf("\nspeak text-to-speech: %s   Data at: %s\n%s", version_string, path_home, help_text);
			exit(0);
		case 'k':
			option_capitals = atoi(optarg2);
			break;
		case 'x':
			phoneme_options |= espeakPHONEMES_SHOW;
			break;
		case 'X':
			phoneme_options |= espeakPHONEMES_TRACE;
			break;
		case 'm':
			option_ssml = 1;
			break;
		case 'p':
			pitch_adjustment = atoi(optarg2);
			if (pitch_adjustment > 99) pitch_adjustment = 99;
			break;
		case 'q':
			quiet = 1;
			break;
		case 'f':
			strncpy0(filename, optarg2, sizeof(filename));
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
			strncpy0(voicename, optarg2, sizeof(voicename));
			break;
		case 'w':
			option_waveout = 1;
			strncpy0(wavefile, optarg2, sizeof(wavefile));
			break;
		case 'z':
			option_endpause = 0;
			break;
		case 0x100: // --stdin
			flag_stdin = 1;
			break;
		case 0x105: // --stdout
			option_waveout = 1;
			strcpy(wavefile, "stdout");
			break;
		case 0x101: // --compile-debug
		case 0x102: // --compile
			if (optarg2 != NULL)
				strncpy0(voicename, optarg2, sizeof(voicename));
			flag_compile = c;
			break;
		case 0x103: // --punct
			option_punctuation = 1;
			if (optarg2 != NULL) {
				ix = 0;
				while ((ix < N_PUNCTLIST) && ((option_punctlist[ix] = optarg2[ix]) != 0)) ix++;
				option_punctlist[N_PUNCTLIST-1] = 0;
				option_punctuation = 2;
			}
			break;
		case 0x104: // --voices
			espeak_ng_InitializePath(data_path);
			DisplayVoices(stdout, optarg2);
			exit(0);
		case 0x106: // -- split
			if (optarg2 == NULL)
				samples_split = 30; // default 30 minutes
			else
				samples_split = atoi(optarg2);
			break;
		case 0x107: // --path
			data_path = optarg2;
			break;
		case 0x108: // --phonout
			if ((f_trans = fopen(optarg2, "w")) == NULL) {
				fprintf(stderr, "Can't write to: %s\n", optarg2);
				f_trans = stderr;
			}
			break;
		case 0x109: // --pho
			phoneme_options |= espeakPHONEMES_MBROLA;
			break;
		case 0x10a: // --ipa
			phoneme_options |= espeakPHONEMES_IPA;
			if (optarg2 != NULL) {
				// deprecated and obsolete
				switch (atoi(optarg2))
				{
				case 1:
					phonemes_separator = '_';
					break;
				case 2:
					phonemes_separator = 0x0361;
					phoneme_options |= espeakPHONEMES_TIE;
					break;
				case 3:
					phonemes_separator = 0x200d; // ZWJ
					phoneme_options |= espeakPHONEMES_TIE;
					break;
				}
			}
			break;
		case 0x10b: // --version
			espeak_ng_InitializePath(data_path);
			printf("speak text-to-speech: %s   Data at: %s\n", version_string, path_home);
			exit(0);
		case 0x10c: // --sep
			phoneme_options |= espeakPHONEMES_SHOW;
			if (optarg2 == 0)
				phonemes_separator = ' ';
			else
				utf8_in(&phonemes_separator, optarg2);
			if (phonemes_separator == 'z')
				phonemes_separator = 0x200c; // ZWNJ
			break;
		case 0x10d: // --tie
			phoneme_options |= (espeakPHONEMES_SHOW | espeakPHONEMES_TIE);
			if (optarg2 == 0)
				phonemes_separator = 0x0361; // default: combining-double-inverted-breve
			else
				utf8_in(&phonemes_separator, optarg2);
			if (phonemes_separator == 'z')
				phonemes_separator = 0x200d; // ZWJ
			break;
		case 0x10e: // --compile-mbrola
		{
			espeak_ng_InitializePath(data_path);
			espeak_ng_ERROR_CONTEXT context = NULL;
			espeak_ng_STATUS result = espeak_ng_CompileMbrolaVoice(optarg2, stdout, &context);
			if (result != ENS_OK) {
				espeak_ng_PrintStatusCodeMessage(result, stderr, context);
				espeak_ng_ClearErrorContext(&context);
				return EXIT_FAILURE;
			}
			return EXIT_SUCCESS;
		}
		case 0x10f: // --compile-intonations
		{
			espeak_ng_InitializePath(data_path);
			espeak_ng_ERROR_CONTEXT context = NULL;
			espeak_ng_STATUS result = espeak_ng_CompileIntonation(stdout, &context);
			if (result != ENS_OK) {
				espeak_ng_PrintStatusCodeMessage(result, stderr, context);
				espeak_ng_ClearErrorContext(&context);
				return EXIT_FAILURE;
			}
			return EXIT_SUCCESS;
		}
		case 0x110: // --compile-phonemes
		{
			espeak_ng_InitializePath(data_path);
			espeak_ng_ERROR_CONTEXT context = NULL;
			espeak_ng_STATUS result = espeak_ng_CompilePhonemeData(22050, stdout, &context);
			if (result != ENS_OK) {
				espeak_ng_PrintStatusCodeMessage(result, stderr, context);
				espeak_ng_ClearErrorContext(&context);
				return EXIT_FAILURE;
			}
			return EXIT_SUCCESS;
		}
		default:
			exit(0);
		}
	}

	espeak_ng_InitializePath(data_path);
	espeak_ng_ERROR_CONTEXT context = NULL;
	espeak_ng_STATUS result = espeak_ng_Initialize(&context);
	if (result != ENS_OK) {
		espeak_ng_PrintStatusCodeMessage(result, stderr, context);
		espeak_ng_ClearErrorContext(&context);
		exit(1);
	}

	if (voicename[0] == 0)
		strcpy(voicename, "default");

	result = espeak_ng_SetVoiceByName(voicename);
	if (result != ENS_OK) {
		memset(&voice_select, 0, sizeof(voice_select));
		voice_select.languages = voicename;
		result = espeak_ng_SetVoiceByProperties(&voice_select);
		if (result != ENS_OK) {
			espeak_ng_PrintStatusCodeMessage(result, stderr, NULL);
			exit(EXIT_FAILURE);
		}
	}

	if (flag_compile) {
		espeak_ng_ERROR_CONTEXT context = NULL;
#if defined(PLATFORM_DOS) || defined(PLATFORM_WINDOWS)
		char path_dsource[sizeof(path_home)+20];
		strcpy(path_dsource, path_home);
		path_dsource[strlen(path_home)-11] = 0; // remove "espeak-data" from the end
		strcat(path_dsource, "dictsource\\");

		espeak_ng_STATUS status = espeak_ng_CompileDictionary(path_dsource, dictionary_name, NULL, flag_compile & 0x1, &context);
#else
		espeak_ng_STATUS status = espeak_ng_CompileDictionary(NULL, dictionary_name, NULL, flag_compile & 0x1, &context);
#endif
		if (status != ENS_OK) {
			espeak_ng_PrintStatusCodeMessage(status, stderr, context);
			espeak_ng_ClearErrorContext(&context);
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}

	SetParameter(espeakRATE, speed, 0);
	SetParameter(espeakVOLUME, amp, 0);
	SetParameter(espeakCAPITALS, option_capitals, 0);
	SetParameter(espeakPUNCTUATION, option_punctuation, 0);
	SetParameter(espeakWORDGAP, wordgap, 0);

	option_phonemes = phoneme_options | (phonemes_separator << 8);

	if (pitch_adjustment != 50)
		SetParameter(espeakPITCH, pitch_adjustment, 0);
	DoVoiceChange(voice);

	if (filename[0] == 0) {
		if ((optind < argc) && (flag_stdin == 0)) {
			// there's a non-option parameter, and no -f or --stdin
			// use it as text
			p_text = argv[optind];
		} else {
			f_text = stdin;
			if (flag_stdin == 0)
				option_linelength = -1; // single input lines on stdin
		}
	} else {
		f_text = fopen(filename, "r");
		if (f_text == NULL) {
			fprintf(stderr, "Failed to read file '%s'\n", filename);
			exit(EXIT_FAILURE);
		}
	}

	if (option_waveout || quiet) {
		if (quiet) {
			// no sound output
			OpenWaveFile(NULL, samplerate);
			option_waveout = 1;
		} else {
			// write sound output to a WAV file
			samples_split = (samplerate * samples_split) * 60;

			if (samples_split) {
				// don't open the wav file until we start generating speech
				char *extn;
				extn = strrchr(wavefile, '.');
				if ((extn != NULL) && ((wavefile + strlen(wavefile) - extn) <= 4)) {
					strcpy(filetype, extn);
					*extn = 0;
				}
			} else if (OpenWaveFile(wavefile, samplerate) != 0) {
				fprintf(stderr, "Can't write to output file '%s'\n'", wavefile);
				exit(EXIT_FAILURE);
			}
		}

		InitText(0);
		SpeakNextClause(f_text, p_text, 0);

		ix = 1;
		for (;;) {
			if (WavegenFile() != 0) {
				if (ix == 0)
					break; // finished, wavegen command queue is empty
			}

			if (Generate(phoneme_list, &n_phoneme_list, 1) == 0)
				ix = SpeakNextClause(NULL, NULL, 1);
		}

		CloseWaveFile();
	} else {
		WavegenInitSound();

		InitText(0);
		SpeakNextClause(f_text, p_text, 0);

		if (option_quiet) {
			while (SpeakNextClause(NULL, NULL, 1) != 0) ;
			return 0;
		}

		speaking = 1;
		while (speaking) {
			// NOTE: if nanosleep() isn't recognised on your system, try replacing
			// this by  sleep(1);
#ifdef PLATFORM_WINDOWS
			Sleep(300); // 0.3s
#else
#ifdef USE_NANOSLEEP
			struct timespec period;
			struct timespec remaining;
			period.tv_sec = 0;
			period.tv_nsec = 300000000; // 0.3 sec
			nanosleep(&period, &remaining);
#else
			sleep(1);
#endif
#endif
			if (SynthOnTimer() != 0)
				speaking = 0;
		}
	}

	if ((f_trans != stdout) && (f_trans != stderr))
		fclose(f_trans);
	return 0;
}
