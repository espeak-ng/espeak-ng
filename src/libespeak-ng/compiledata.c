/*
 * Copyright (C) 2005 to 2014 by Jonathan Duddington
 * email: jonsd@users.sourceforge.net
 * Copyright (C) 2013-2017 Reece H. Dunn
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

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/speak_lib.h>
#include <espeak-ng/encoding.h>

#include "common.h"                    // for GetFileLength, strncpy0, ...c
#include "error.h"                    // for create_file_error_context
#include "mnemonics.h"               // for LookupMnemName, MNEM_TAB
#include "phoneme.h"                  // for PHONEME_TAB, PHONEME_TAB_LIST
#include "spect.h"                    // for SpectFrame, peak_t, SpectSeq
#include "speech.h"			// for path_home, GetFileLength
#include "synthdata.h"                // for LoadPhData
#include "synthesize.h"               // for TUNE, frame_t, CONDITION_IS_OTHER
#include "translate.h"                // for utf8_out, utf8_in
#include "voice.h"                    // for LoadVoice, voice
#include "wavegen.h"                  // for WavegenInit, WavegenSetVoice

static int CalculateSample(unsigned char c3, int c1);

#define N_ITEM_STRING 256

typedef struct {
	unsigned int value;
	char *name;
} NAMETAB;

typedef struct {
	const char *mnem;
	int type;
	int data;
} keywtab_t;

#define k_AND     1
#define k_OR      2
#define k_THEN    3
#define k_NOT     4

#define kTHISSTRESS 0x800

// keyword types
enum {
	tPHONEME_TYPE = 1,
	tPHONEME_FLAG,
	tTRANSITION,
	tSTATEMENT,
	tINSTRN1,
	tWHICH_PHONEME,
	tTEST,
};

static const keywtab_t k_conditions[] = {
	{ "AND",  0, k_AND },
	{ "OR",   0, k_OR },
	{ "THEN", 0, k_THEN },
	{ "NOT",  0, k_NOT },

	{ "prevPh",    tWHICH_PHONEME,  0 },
	{ "thisPh",    tWHICH_PHONEME,  1 },
	{ "nextPh",    tWHICH_PHONEME,  2 },
	{ "next2Ph",   tWHICH_PHONEME,  3 },
	{ "nextPhW",   tWHICH_PHONEME,  4 },
	{ "prevPhW",   tWHICH_PHONEME,  5 },
	{ "next2PhW",  tWHICH_PHONEME,  6 },
	{ "nextVowel", tWHICH_PHONEME,  7 },
	{ "prevVowel", tWHICH_PHONEME,  8 },
	{ "next3PhW",  tWHICH_PHONEME,  9 },
	{ "prev2PhW",  tWHICH_PHONEME, 10 },

	{ "PreVoicing",  tTEST, 0xf01 },
	{ "KlattSynth",  tTEST, 0xf02 },
	{ "MbrolaSynth", tTEST, 0xf03 },

	{ NULL, 0, 0 }
};

static const keywtab_t k_properties[] = {
	{ "isPause",      0, CONDITION_IS_PHONEME_TYPE | phPAUSE },
	{ "isVowel",      0, CONDITION_IS_PHONEME_TYPE | phVOWEL },
	{ "isNasal",      0, CONDITION_IS_PHONEME_TYPE | phNASAL },
	{ "isLiquid",     0, CONDITION_IS_PHONEME_TYPE | phLIQUID },
	{ "isUStop",      0, CONDITION_IS_PHONEME_TYPE | phSTOP },
	{ "isVStop",      0, CONDITION_IS_PHONEME_TYPE | phVSTOP },
	{ "isVFricative", 0, CONDITION_IS_PHONEME_TYPE | phVFRICATIVE },

	{ "isPalatal",  0, CONDITION_IS_PHFLAG_SET | phFLAGBIT_PALATAL },
	{ "isLong",     0, CONDITION_IS_PHFLAG_SET | phFLAGBIT_LONG },
	{ "isRhotic",   0, CONDITION_IS_PHFLAG_SET | phFLAGBIT_RHOTIC },
	{ "isSibilant", 0, CONDITION_IS_PHFLAG_SET | phFLAGBIT_SIBILANT },
	{ "isFlag1",    0, CONDITION_IS_PHFLAG_SET | phFLAGBIT_FLAG1 },
	{ "isFlag2",    0, CONDITION_IS_PHFLAG_SET | phFLAGBIT_FLAG2 },

	{ "isVelar", 0, CONDITION_IS_PLACE_OF_ARTICULATION | phPLACE_VELAR },

	{ "isDiminished",  0, CONDITION_IS_OTHER | STRESS_IS_DIMINISHED },
	{ "isUnstressed",  0, CONDITION_IS_OTHER | STRESS_IS_UNSTRESSED },
	{ "isNotStressed", 0, CONDITION_IS_OTHER | STRESS_IS_NOT_STRESSED },
	{ "isStressed",    0, CONDITION_IS_OTHER | STRESS_IS_SECONDARY },
	{ "isMaxStress",   0, CONDITION_IS_OTHER | STRESS_IS_PRIMARY },

	{ "isPause2",           0, CONDITION_IS_OTHER | isBreak },
	{ "isWordStart",        0, CONDITION_IS_OTHER | isWordStart },
	{ "isWordEnd",          0, CONDITION_IS_OTHER | isWordEnd },
	{ "isAfterStress",      0, CONDITION_IS_OTHER | isAfterStress },
	{ "isNotVowel",         0, CONDITION_IS_OTHER | isNotVowel },
	{ "isFinalVowel",       0, CONDITION_IS_OTHER | isFinalVowel },
	{ "isVoiced",           0, CONDITION_IS_OTHER | isVoiced },
	{ "isFirstVowel",       0, CONDITION_IS_OTHER | isFirstVowel },
	{ "isSecondVowel",      0, CONDITION_IS_OTHER | isSecondVowel },
	{ "isTranslationGiven", 0, CONDITION_IS_OTHER | isTranslationGiven },

	{ NULL, 0, 0 }
};

enum {
	kPHONEMESTART = 1,
	kUTF8_BOM,
	kPROCEDURE,
	kENDPHONEME,
	kENDPROCEDURE,
	kPHONEMETABLE,
	kINCLUDE,
	kIMPORT_PH,

	kSTARTTYPE,
	kENDTYPE,
	kSTRESSTYPE,
	kVOICINGSWITCH,

	kIF,
	kELSE,
	kELIF,
	kENDIF,
	kCALLPH,

	kSWITCH_PREVVOWEL,
	kSWITCH_NEXTVOWEL,
	kENDSWITCH,

	kFMT,
	kWAV,
	kVOWELSTART,
	kVOWELENDING,
	kANDWAV,

	kVOWELIN,
	kVOWELOUT,
	kTONESPEC,

	kRETURN,
	kCONTINUE,
};

enum {
	kTUNE = 1,
	kENDTUNE,
	kTUNE_PREHEAD,
	kTUNE_ONSET,
	kTUNE_HEAD,
	kTUNE_HEADENV,
	kTUNE_HEADEXTEND,
	kTUNE_HEADLAST,
	kTUNE_NUCLEUS0,
	kTUNE_NUCLEUS1,
	kTUNE_SPLIT,
};

static const unsigned char utf8_bom[] = { 0xef, 0xbb, 0xbf, 0 };

static const keywtab_t k_intonation[] = {
	{ "tune",       0, kTUNE },
	{ "endtune",    0, kENDTUNE },
	{ "prehead",    0, kTUNE_PREHEAD },
	{ "onset",      0, kTUNE_ONSET },
	{ "head",       0, kTUNE_HEAD },
	{ "headenv",    0, kTUNE_HEADENV },
	{ "headextend", 0, kTUNE_HEADEXTEND },
	{ "headlast",   0, kTUNE_HEADLAST },
	{ "nucleus0",   0, kTUNE_NUCLEUS0 },
	{ "nucleus",    0, kTUNE_NUCLEUS1 },
	{ "split",      0, kTUNE_SPLIT },

	{ NULL, 0, -1 }
};

static const keywtab_t keywords[] = {
	{ "liquid",  tPHONEME_TYPE, phLIQUID },
	{ "pause",   tPHONEME_TYPE, phPAUSE },
	{ "stress",  tPHONEME_TYPE, phSTRESS },
	{ "virtual", tPHONEME_TYPE, phVIRTUAL },
	{ "delete_phoneme", tPHONEME_TYPE, phDELETED },

	// keywords
	{ "phonemetable",         tSTATEMENT, kPHONEMETABLE },
	{ "include",              tSTATEMENT, kINCLUDE },
	{ (const char *)utf8_bom, tSTATEMENT, kUTF8_BOM },

	{ "phoneme",        tSTATEMENT, kPHONEMESTART },
	{ "procedure",      tSTATEMENT, kPROCEDURE },
	{ "endphoneme",     tSTATEMENT, kENDPHONEME },
	{ "endprocedure",   tSTATEMENT, kENDPROCEDURE },
	{ "import_phoneme", tSTATEMENT, kIMPORT_PH },
	{ "stress_type",    tSTATEMENT, kSTRESSTYPE },
	{ "starttype",      tSTATEMENT, kSTARTTYPE },
	{ "endtype",        tSTATEMENT, kENDTYPE },
	{ "voicingswitch",  tSTATEMENT, kVOICINGSWITCH },

	{ "IF",     tSTATEMENT, kIF },
	{ "ELSE",   tSTATEMENT, kELSE },
	{ "ELIF",   tSTATEMENT, kELIF },
	{ "ELSEIF", tSTATEMENT, kELIF },  // same as ELIF
	{ "ENDIF",  tSTATEMENT, kENDIF },
	{ "CALL",   tSTATEMENT, kCALLPH },
	{ "RETURN", tSTATEMENT, kRETURN },

	{ "PrevVowelEndings", tSTATEMENT, kSWITCH_PREVVOWEL },
	{ "NextVowelStarts",  tSTATEMENT, kSWITCH_NEXTVOWEL },
	{ "EndSwitch",        tSTATEMENT, kENDSWITCH },

	{ "Tone",        tSTATEMENT, kTONESPEC },
	{ "FMT",         tSTATEMENT, kFMT },
	{ "WAV",         tSTATEMENT, kWAV },
	{ "VowelStart",  tSTATEMENT, kVOWELSTART },
	{ "VowelEnding", tSTATEMENT, kVOWELENDING },
	{ "addWav",      tSTATEMENT, kANDWAV },

	{ "Vowelin",  tSTATEMENT, kVOWELIN },
	{ "Vowelout", tSTATEMENT, kVOWELOUT },
	{ "Continue", tSTATEMENT, kCONTINUE },

	{ "ChangePhoneme",       tINSTRN1, i_CHANGE_PHONEME },
	{ "ChangeNextPhoneme",   tINSTRN1, i_REPLACE_NEXT_PHONEME },
	{ "InsertPhoneme",       tINSTRN1, i_INSERT_PHONEME },
	{ "AppendPhoneme",       tINSTRN1, i_APPEND_PHONEME },
	{ "IfNextVowelAppend",   tINSTRN1, i_APPEND_IFNEXTVOWEL },
	{ "ChangeIfDiminished",  tINSTRN1, i_CHANGE_IF | STRESS_IS_DIMINISHED },
	{ "ChangeIfUnstressed",  tINSTRN1, i_CHANGE_IF | STRESS_IS_UNSTRESSED },
	{ "ChangeIfNotStressed", tINSTRN1, i_CHANGE_IF | STRESS_IS_NOT_STRESSED },
	{ "ChangeIfStressed",    tINSTRN1, i_CHANGE_IF | STRESS_IS_SECONDARY },
	{ "ChangeIfStressed",    tINSTRN1, i_CHANGE_IF | STRESS_IS_PRIMARY },

	{ "PauseBefore", tINSTRN1, i_PAUSE_BEFORE },
	{ "PauseAfter",  tINSTRN1, i_PAUSE_AFTER },
	{ "length",      tINSTRN1, i_SET_LENGTH },
	{ "LongLength",  tINSTRN1, i_LONG_LENGTH },
	{ "LengthAdd",   tINSTRN1, i_ADD_LENGTH },
	{ "lengthmod",   tINSTRN1, i_LENGTH_MOD },
	{ "ipa",         tINSTRN1, i_IPA_NAME },

	// flags
	{ "unstressed",   tPHONEME_FLAG, phUNSTRESSED },
	{ "nolink",       tPHONEME_FLAG, phNOLINK },
	{ "brkafter",     tPHONEME_FLAG, phBRKAFTER },
	{ "rhotic",       tPHONEME_FLAG, phRHOTIC },
	{ "lengthenstop", tPHONEME_FLAG, phLENGTHENSTOP },
	{ "nopause",      tPHONEME_FLAG, phNOPAUSE },
	{ "prevoice",     tPHONEME_FLAG, phPREVOICE },

	{ "flag1", tPHONEME_FLAG, phFLAG1 },
	{ "flag2", tPHONEME_FLAG, phFLAG2 },

	// vowel transition attributes
	{ "len=",   tTRANSITION,  1 },
	{ "rms=",   tTRANSITION,  2 },
	{ "f1=",    tTRANSITION,  3 },
	{ "f2=",    tTRANSITION,  4 },
	{ "f3=",    tTRANSITION,  5 },
	{ "brk",    tTRANSITION,  6 },
	{ "rate",   tTRANSITION,  7 },
	{ "glstop", tTRANSITION,  8 },
	{ "lenadd", tTRANSITION,  9 },
	{ "f4",     tTRANSITION, 10 },
	{ "gpaus",  tTRANSITION, 11 },
	{ "colr=",  tTRANSITION, 12 },
	{ "amp=",   tTRANSITION, 13 },  // set rms of 1st frame as fraction of rms of 2nd frame  (1/30ths)

	{ NULL, 0, -1 }
};

static const keywtab_t *keyword_tabs[] = {
	keywords, k_conditions, k_properties, k_intonation
};

typedef struct {
	void *link;
	int value;
	int ph_mnemonic;
	short ph_table;
	char string[1];
} REF_HASH_TAB;

typedef struct {
	FILE *file;
	int linenum;
	char fname[80];
} STACK;

typedef struct {
	unsigned short *p_then;
	unsigned short *p_else;
	bool returned;
} IF_STACK;

enum {
	tENDFILE = 1,
	tSTRING,
	tNUMBER,
	tSIGNEDNUMBER,
	tPHONEMEMNEM,
	tOPENBRACKET,
	tKEYWORD,
	tCONDITION,
	tPROPERTIES,
	tINTONATION,
};

typedef struct CompileContext {
	PHONEME_TAB *phoneme_out;

	int n_phcodes_list[N_PHONEME_TABS];
	PHONEME_TAB_LIST phoneme_tab_list2[N_PHONEME_TABS];
	PHONEME_TAB *phoneme_tab2;
	int phoneme_flags;

	#define N_PROCS 50
	int n_procs;
	int proc_addr[N_PROCS];
	char proc_names[N_ITEM_STRING+1][N_PROCS];

	#define MAX_PROG_BUF 2000
	unsigned short *prog_out;
	unsigned short *prog_out_max;
	unsigned short prog_buf[MAX_PROG_BUF+20];

	int n_phoneme_tabs;
	int n_phcodes;

	// outout files
	FILE *f_phdata;
	FILE *f_phindex;
	FILE *f_phtab;
	FILE *f_phcontents;
	FILE *f_errors;
	FILE *f_prog_log;

	FILE *f_in;
	int f_in_linenum;
	int f_in_displ;

	int linenum;
	int count_references;
	int duplicate_references;
	int count_frames;
	int error_count;
	int then_count;
	bool after_if;

	char current_fname[80];

	REF_HASH_TAB *ref_hash_tab[256];

	#define N_STACK  12
	int stack_ix;
	STACK stack[N_STACK];

	#define N_IF_STACK 12
	int if_level;
	IF_STACK if_stack[N_IF_STACK];

	int item_type;
	int item_terminator;
	char item_string[N_ITEM_STRING];

	NAMETAB *manifest;
	int n_manifest;
	char phsrc[sizeof(path_home)+40]; // Source: path to the 'phonemes' source file.
} CompileContext;

static void clean_context(CompileContext *ctx) {
	for (int i = 0; i < 256; i++) {
		REF_HASH_TAB *p;
		while ((p = ctx->ref_hash_tab[i])) {
			ctx->ref_hash_tab[i] = (REF_HASH_TAB*)p->link;
			free(p);
		}
	}
	for (int i = 0; i < ctx->n_manifest; i++) {
		free(ctx->manifest[i].name);
	}
	free(ctx->manifest);
	free(ctx);
}

static void error(CompileContext *ctx, const char *format, ...)
{
	va_list args;
	va_start(args, format);

	fprintf(ctx->f_errors, "%s(%d): ", ctx->current_fname, ctx->linenum-1);
	vfprintf(ctx->f_errors, format, args);
	fprintf(ctx->f_errors, "\n");
	ctx->error_count++;

	va_end(args);
}

static void error_from_status(CompileContext *ctx, espeak_ng_STATUS status, const char *context)
{
	char message[512];
	espeak_ng_GetStatusCodeMessage(status, message, sizeof(message));
	if (context)
		error(ctx, "%s: '%s'.", message, context);
	else
		error(ctx, "%s.", message);
}

static espeak_ng_STATUS ReadPhondataManifest(CompileContext *ctx, espeak_ng_ERROR_CONTEXT *context)
{
	// Read the phondata-manifest file
	FILE *f;
	int n_lines = 0;
	char *p;
	unsigned int value;
	char buf[sizeof(path_home)+40];
	char name[120];

	sprintf(buf, "%s%c%s", path_home, PATHSEP, "phondata-manifest");
	if ((f = fopen(buf, "r")) == NULL)
		return create_file_error_context(context, errno, buf);

	while (fgets(buf, sizeof(buf), f) != NULL)
		n_lines++;

	rewind(f);

	if (ctx->manifest != NULL) {
		for (int ix = 0; ix < ctx->n_manifest; ix++)
			free(ctx->manifest[ix].name);
	}

	if (n_lines == 0) {
		fclose(f);
		return ENS_EMPTY_PHONEME_MANIFEST;
	}

	NAMETAB *new_manifest = (NAMETAB *)realloc(ctx->manifest, n_lines * sizeof(NAMETAB));
	if (new_manifest == NULL) {
		fclose(f);
		free(ctx->manifest);
		return ENOMEM;
	} else
		ctx->manifest = new_manifest;

	ctx->n_manifest = 0;
	while (fgets(buf, sizeof(buf), f) != NULL) {
		if (!isalpha(buf[0]))
			continue;

		if (sscanf(&buf[2], "%x %s", &value, name) == 2) {
			if ((p = (char *)malloc(strlen(name)+1)) != NULL) {
				strcpy(p, name);
				ctx->manifest[ctx->n_manifest].value = value;
				ctx->manifest[ctx->n_manifest].name = p;
				ctx->n_manifest++;
			}
		}
	}
	fclose(f);

	return ENS_OK;
}

static const MNEM_TAB reserved_phonemes[] = {
	{ "_\001",  phonCONTROL },      // NOT USED
	{ "%",      phonSTRESS_U },
	{ "%%",     phonSTRESS_D },
	{ ",",      phonSTRESS_2 },
	{ ",,",     phonSTRESS_3 },
	{ "'",      phonSTRESS_P },
	{ "''",     phonSTRESS_P2 },
	{ "=",      phonSTRESS_PREV },  //  stress previous syllable
	{ "_:",     phonPAUSE },        //  pause
	{ "_",      phonPAUSE_SHORT },  //  short pause
	{ "_!",     phonPAUSE_NOLINK }, //  short pause, no link
	{ ":",      phonLENGTHEN },
	{ "@",      phonSCHWA },
	{ "@-",     phonSCHWA_SHORT },
	{ "||",     phonEND_WORD },
	{ "1",      phonDEFAULTTONE },  // (numeral 1)  default tone (for tone language)
	{ "#X1",    phonCAPITAL },      // capital letter indication
	{ "?",      phonGLOTTALSTOP },  // glottal stop
	{ "-",      phonSYLLABIC },     // syllabic consonant
	{ "_^_",    phonSWITCH },       //  Change language
	{ "_X1",    phonX1 },           // a language specific action
	{ "_|",     phonPAUSE_VSHORT }, // very short pause
	{ "_::",    phonPAUSE_LONG },   // long pause
	{ "t#",     phonT_REDUCED },    // reduced version of [t]
	{ "'!",     phonSTRESS_TONIC }, // stress - emphasized
	{ "_;_",    phonPAUSE_CLAUSE }, // clause pause

	{ "#@",     phonVOWELTYPES },   // vowel type groups, these must be consecutive
	{ "#a",     phonVOWELTYPES+1 },
	{ "#e",     phonVOWELTYPES+2 },
	{ "#i",     phonVOWELTYPES+3 },
	{ "#o",     phonVOWELTYPES+4 },
	{ "#u",     phonVOWELTYPES+5 },

	{ NULL, 0 }
};

static void ReservePhCodes(CompileContext *ctx)
{
	// Reserve phoneme codes which have fixed numbers so that they can be
	// referred to from the program code.
	const MNEM_TAB *p;

	p = reserved_phonemes;
	while (p->mnem != NULL) {
		ctx->phoneme_tab2[p->value].mnemonic = StringToWord(p->mnem);
		ctx->phoneme_tab2[p->value].code = p->value;
		if (ctx->n_phcodes <= p->value)
			ctx->n_phcodes = p->value+1;
		p++;
	}
}

static int LookupPhoneme(CompileContext *ctx, const char *string, int control)
{
	// control = 0   explicit declaration
	// control = 1   declare phoneme if not found
	// control = 2   start looking after control & stress phonemes

	int ix;
	int start;
	int use;
	unsigned int word;

	if (strcmp(string, "NULL") == 0)
		return 1;

	ix = strlen(string);
	if ((ix == 0) || (ix > 4))
		error(ctx, "Bad phoneme name '%s'", string);
	word = StringToWord(string);

	// don't use phoneme number 0, reserved for string terminator
	start = 1;

	if (control == 2) {
		// don't look for control and stress phonemes (allows these characters to be
		// used for other purposes)
		start = 8;
	}

	use = 0;
	for (ix = start; ix < ctx->n_phcodes; ix++) {
		if (ctx->phoneme_tab2[ix].mnemonic == word)
			return ix;

		if ((use == 0) && (ctx->phoneme_tab2[ix].mnemonic == 0))
			use = ix;
	}

	if (use == 0) {
		if (control == 0)
			return -1;
		if (ctx->n_phcodes >= N_PHONEME_TAB-1)
			return -1; // phoneme table is full
		use = ctx->n_phcodes++;
	}

	// add this phoneme to the phoneme table
	ctx->phoneme_tab2[use].mnemonic = word;
	ctx->phoneme_tab2[use].type = phINVALID;
	ctx->phoneme_tab2[use].program = ctx->linenum; // for error report if the phoneme remains undeclared
	return use;
}

static unsigned int get_char(CompileContext *ctx)
{
	unsigned int c;
	c = fgetc(ctx->f_in);
	if (c == '\n')
		ctx->linenum++;
	return c;
}

static void unget_char(CompileContext *ctx, unsigned int c)
{
	ungetc(c, ctx->f_in);
	if (c == '\n')
		ctx->linenum--;
}

static int CheckNextChar(CompileContext *ctx)
{
	int c;
	while (((c = get_char(ctx)) == ' ') || (c == '\t'))
		;
	unget_char(ctx, c);
	return c;
}

static int NextItem(CompileContext *ctx, int type)
{
	unsigned char c = 0;
	unsigned char c2;
	int ix;
	const keywtab_t *pk;

	ctx->item_type = -1;

	ctx->f_in_displ = ftell(ctx->f_in);
	ctx->f_in_linenum = ctx->linenum;

	while (!feof(ctx->f_in)) {
		c = get_char(ctx);
		if (c == '/') {
			if ((c2 = get_char(ctx)) == '/') {
				// comment, ignore to end of line
				while (!feof(ctx->f_in) && ((c = get_char(ctx)) != '\n'))
					;
			} else
				unget_char(ctx, c2);
		}
		if (!isspace(c))
			break;
	}
	if (feof(ctx->f_in))
		return -2;

	if (c == '(') {
		if (type == tOPENBRACKET)
			return 1;
		return -1;
	}

	ix = 0;
	while (!feof(ctx->f_in) && !isspace(c) && (c != '(') && (c != ')') && (c != ',')) {
		if (c == '\\')
			c = get_char(ctx);
		ctx->item_string[ix++] = c;
		c = get_char(ctx);
		if (feof(ctx->f_in))
			break;
		if (ctx->item_string[ix-1] == '=')
			break;
	}
	ctx->item_string[ix] = 0;

	while (isspace(c))
		c = get_char(ctx);

	ctx->item_terminator = ' ';
	if ((c == ')') || (c == '(') || (c == ','))
		ctx->item_terminator = c;

	if (!feof(ctx->f_in) && !(c == ')' || c == ','))
		unget_char(ctx, c);

	if (type == tSTRING)
		return 0;

	if ((type == tNUMBER) || (type == tSIGNEDNUMBER)) {
		int acc = 0;
		int sign = 1;
		char *p;
		 p = ctx->item_string;

		if ((*p == '-') && (type == tSIGNEDNUMBER)) {
			sign = -1;
			p++;
		}
		if (!isdigit(*p)) {
			if ((type == tNUMBER) && (*p == '-'))
				error(ctx, "Expected an unsigned number");
			else
				error(ctx, "Expected a number");
		}
		while (isdigit(*p)) {
			acc *= 10;
			acc += (*p - '0');
			p++;
		}
		return acc * sign;
	}

	if ((type >= tKEYWORD) && (type <= tINTONATION)) {
		pk = keyword_tabs[type-tKEYWORD];
		while (pk->mnem != NULL) {
			if (strcmp(ctx->item_string, pk->mnem) == 0) {
				ctx->item_type = pk->type;
				return pk->data;
			}
			pk++;
		}
		ctx->item_type = -1;
		return -1; // keyword not found
	}
	if (type == tPHONEMEMNEM)
		return LookupPhoneme(ctx, ctx->item_string, 2);
	return -1;
}

static int NextItemMax(CompileContext *ctx, int max)
{
	// Get a number, but restrict value to max
	int value;

	value = NextItem(ctx, tNUMBER);
	if (value > max) {
		error(ctx, "Value %d is greater than maximum %d", value, max);
		value = max;
	}
	return value;
}

static int NextItemBrackets(CompileContext *ctx, int type, int control)
{
	// Expect a parameter inside parentheses
	// control: bit 0  0= need (
	//          bit 1  1= allow comma

	int value;

	if ((control & 1) == 0) {
		if (!NextItem(ctx, tOPENBRACKET))
			error(ctx, "Expected '('");
	}

	value = NextItem(ctx, type);
	if ((control & 2) && (ctx->item_terminator == ','))
		return value;

	if (ctx->item_terminator != ')')
		error(ctx, "Expected ')'");
	return value;
}

static void UngetItem(CompileContext *ctx)
{
	fseek(ctx->f_in, ctx->f_in_displ, SEEK_SET);
	ctx->linenum = ctx->f_in_linenum;
}

static int Range(int value, int divide, int min, int max)
{
	if (value < 0)
		value -= divide/2;
	else
		value += divide/2;
	value = value / divide;

	if (value > max)
		value = max;
	if (value < min)
		value = min;
	return value - min;
}

static int CompileVowelTransition(CompileContext *ctx, int which)
{
	// Compile a vowel transition
	int len = 0;
	int rms = 0;
	int f1 = 0;
	int f2 = 0;
	int f2_min = 0;
	int f2_max = 0;
	int f3_adj = 0;
	int f3_amp = 0;
	int flags = 0;
	int vcolour = 0;
	int x;
	int instn = i_VOWELIN;
	int word1;
	int word2;

	if (which == 1) {
		len = 50 / 2; // defaults for transition into vowel
		rms = 25 / 2;

		if (ctx->phoneme_out->type == phSTOP) {
			len = 42 / 2; // defaults for transition into vowel
			rms = 30 / 2;
		}
	} else if (which == 2) {
		instn = i_VOWELOUT;
		len = 36 / 2; // defaults for transition out of vowel
		rms = 16 / 2;
	}

	for (;;) {
		int key = NextItem(ctx, tKEYWORD);
		if (ctx->item_type != tTRANSITION) {
			UngetItem(ctx);
			break;
		}

		switch (key & 0xf)
		{
		case 1:
			len = Range(NextItem(ctx, tNUMBER), 2, 0, 63) & 0x3f;
			flags |= 1;
			break;
		case 2:
			rms = Range(NextItem(ctx, tNUMBER), 2, 0, 31) & 0x1f;
			flags |= 1;
			break;
		case 3:
			f1 = NextItem(ctx, tNUMBER);
			break;
		case 4:
			f2 = Range(NextItem(ctx, tNUMBER), 50, 0, 63) & 0x3f;
			f2_min = Range(NextItem(ctx, tSIGNEDNUMBER), 50, -15, 15) & 0x1f;
			f2_max = Range(NextItem(ctx, tSIGNEDNUMBER), 50, -15, 15) & 0x1f;
			if (f2_min > f2_max) {
				x = f2_min;
				f2_min = f2_max;
				f2_max = x;
			}
			break;
		case 5:
			f3_adj = Range(NextItem(ctx, tSIGNEDNUMBER), 50, -15, 15) & 0x1f;
			f3_amp = Range(NextItem(ctx, tNUMBER), 8, 0, 15) & 0x1f;
			break;
		case 6:
			flags |= 2; // break
			break;
		case 7:
			flags |= 4; // rate
			break;
		case 8:
			flags |= 8; // glstop
			break;
		case 9:
			flags |= 16; // lenadd
			break;
		case 10:
			flags |= 32;  // f4
			break;
		case 11:
			flags |= 64;  // pause
			break;
		case 12:
			vcolour = NextItem(ctx, tNUMBER);
			break;
		case 13:
			// set rms of 1st frame as fraction of rms of 2nd frame  (1/30ths)
			rms = (Range(NextItem(ctx, tNUMBER), 1, 0, 31) & 0x1f) | 0x20;
			flags |= 1;
			break;
		}
	}
	word1 = len + (rms << 6) + (flags << 12);
	word2 =  f2 + (f2_min << 6) + (f2_max << 11) + (f3_adj << 16) + (f3_amp << 21) + (f1 << 26) + (vcolour << 29);
	ctx->prog_out[0] = instn + ((word1 >> 16) & 0xff);
	ctx->prog_out[1] = word1;
	ctx->prog_out[2] = word2 >> 16;
	ctx->prog_out[3] = word2;
	ctx->prog_out += 4;

	return 0;
}

static espeak_ng_STATUS LoadSpect(CompileContext *ctx, const char *path, int control, int *addr)
{
	SpectSeq *spectseq;
	int peak;
	int frame;
	int n_frames;
	int ix;
	int x, x2;
	int rms;
	float total;
	float pkheight;
	int marker1_set = 0;
	int frame_vowelbreak = 0;
	int klatt_flag = 0;
	SpectFrame *fr;
	frame_t *fr_out;
	char filename[sizeof(path_home)+20];

	SPECT_SEQ seq_out;
	SPECT_SEQK seqk_out;

	// create SpectSeq and import data
	spectseq = SpectSeqCreate();
	if (spectseq == NULL)
		return ENOMEM;

	snprintf(filename, sizeof(filename), "%s/%s", ctx->phsrc, path);
	espeak_ng_STATUS status = LoadSpectSeq(spectseq, filename);
	if (status != ENS_OK) {
		error(ctx, "Bad vowel file: '%s'", path);
		SpectSeqDestroy(spectseq);
		return status;
	}

	// do we need additional klatt data ?
	for (frame = 0; frame < spectseq->numframes; frame++) {
		for (ix = 5; ix < N_KLATTP2; ix++) {
			if (spectseq->frames[frame]->klatt_param[ix] != 0)
				klatt_flag = FRFLAG_KLATT;
		}
	}

	*addr = ftell(ctx->f_phdata);

	seq_out.n_frames = 0;
	seq_out.sqflags = 0;
	seq_out.length_total = 0;

	total = 0;
	for (frame = 0; frame < spectseq->numframes; frame++) {
		if (spectseq->frames[frame]->keyframe) {
			if (seq_out.n_frames == 1)
				frame_vowelbreak = frame;
			if (spectseq->frames[frame]->markers & 0x2) {
				// marker 1 is set
				marker1_set = 1;
			}

			seq_out.n_frames++;
			if (frame > 0)
				total += spectseq->frames[frame-1]->length;
		}
	}
	seq_out.length_total = (int)total;

	if ((control & 1) && (marker1_set == 0)) {
		// This is a vowel, but no Vowel Break marker is set
		// set a marker flag for the second frame of a vowel
		spectseq->frames[frame_vowelbreak]->markers |= FRFLAG_VOWEL_CENTRE;
	}

	n_frames = 0;
	for (frame = 0; frame < spectseq->numframes; frame++) {
		fr = spectseq->frames[frame];

		if (fr->keyframe) {
			if (klatt_flag)
				fr_out = &seqk_out.frame[n_frames];
			else
				fr_out = (frame_t *)&seq_out.frame[n_frames];

			x = (int)(fr->length + 0.5); // round to nearest mS
			if (x > 255) x = 255;
			fr_out->length = x;

			fr_out->frflags = fr->markers | klatt_flag;

			rms = (int)GetFrameRms(fr, spectseq->amplitude);
			if (rms > 255) rms = 255;
			fr_out->rms = rms;

			if (n_frames == (seq_out.n_frames-1))
				fr_out->length = 0; // give last frame zero length

			// write: peak data
			ctx->count_frames++;
			for (peak = 0; peak < 8; peak++) {
				if (peak < 7)
					fr_out->ffreq[peak] = fr->peaks[peak].pkfreq;

				pkheight = spectseq->amplitude * fr->amp_adjust * fr->peaks[peak].pkheight;
				pkheight = pkheight/640000;
				if (pkheight > 255) pkheight = 255;
				fr_out->fheight[peak] = (int)pkheight;

				if (peak < 6) {
					x =  fr->peaks[peak].pkwidth/4;
					if (x > 255) x = 255;
					fr_out->fwidth[peak] = x;

					if (peak < 3) {
						x2 =  fr->peaks[peak].pkright/4;
						if (x2 > 255) x2 = 255;
						fr_out->fright[peak] = x2;
					}
				}

				if (peak < 4) {
					x = fr->peaks[peak].klt_bw / 2;
					if (x > 255) x = 255;
					fr_out->bw[peak] = x;
				}
			}

			for (ix = 0; ix < 5; ix++) {
				fr_out->klattp[ix] = fr->klatt_param[ix];

				fr_out->klattp[KLATT_FNZ] = fr->klatt_param[KLATT_FNZ] / 2;
			}

			if (klatt_flag) {
				// additional klatt parameters
				for (ix = 0; ix < 5; ix++)
					fr_out->klattp2[ix] = fr->klatt_param[ix+5];

				for (peak = 0; peak < 7; peak++) {
					fr_out->klatt_ap[peak] = fr->peaks[peak].klt_ap;

					x = fr->peaks[peak].klt_bp / 2;
					if (x > 255) x = 255;
					fr_out->klatt_bp[peak] = x;
				}
				fr_out->spare = 0;
			}

			if (fr_out->bw[1] == 0) {
				fr_out->bw[0] = 89 / 2;
				fr_out->bw[1] = 90 / 2;
				fr_out->bw[2] = 140 / 2;
				fr_out->bw[3] = 260 / 2;
			}

			n_frames++;
		}
	}

	if (klatt_flag) {
		seqk_out.n_frames = seq_out.n_frames;
		seqk_out.sqflags = seq_out.sqflags;
		seqk_out.length_total = seq_out.length_total;

		ix = (char *)(&seqk_out.frame[seqk_out.n_frames]) - (char *)(&seqk_out);
		fwrite(&seqk_out, ix, 1, ctx->f_phdata);
		while (ix & 3)
		{
			// round up to multiple of 4 bytes
			fputc(0, ctx->f_phdata);
			ix++;
		}
	} else {
		ix = (char *)(&seq_out.frame[seq_out.n_frames]) - (char *)(&seq_out);
		fwrite(&seq_out, ix, 1, ctx->f_phdata);
		while (ix & 3)
		{
			// round up to multiple of 4 bytes
			fputc(0, ctx->f_phdata);
			ix++;
		}
	}

	SpectSeqDestroy(spectseq);
	return ENS_OK;
}

static int LoadWavefile(CompileContext *ctx, FILE *f, const char *fname)
{
	int displ;
	unsigned char c1;
	int sample;
	int sample2;
	float x;
	int max = 0;
	int length;
	int sr1, sr2;
	int scale_factor = 0;

	fseek(f, 24, SEEK_SET);
	sr1 = Read4Bytes(f);
	sr2 = Read4Bytes(f);
	fseek(f, 40, SEEK_SET);

	if ((sr1 != samplerate) || (sr2 != sr1*2)) {
		if (sr1 != samplerate)
			error(ctx, "Can't resample (%d to %d): %s", sr1, samplerate, fname);
		else
			error(ctx, "WAV file is not mono: %s", fname);
		return 0;
	}

	displ = ftell(ctx->f_phdata);

	// data contains:  4 bytes of length (n_samples * 2), followed by 2-byte samples (lsb byte first)
	length = Read4Bytes(f);

	while (true) {
		int c;

		if ((c = fgetc(f)) == EOF)
			break;
		c1 = (unsigned char)c;

		if ((c = fgetc(f)) == EOF)
			break;

		sample = CalculateSample((unsigned char) c, c1);

		if (sample > max)
			max = sample;
		else if (sample < -max)
			max = -sample;
	}

	scale_factor = (max / 127) + 1;

	#define MIN_FACTOR   -1 // was 6, disable use of 16 bit samples
	if (scale_factor > MIN_FACTOR) {
		length = length/2 + (scale_factor << 16);
	}

	Write4Bytes(ctx->f_phdata, length);
	fseek(f, 44, SEEK_SET);

	while (!feof(f)) {
		c1 = fgetc(f);
		unsigned char c3 = fgetc(f);

		sample = CalculateSample(c3, c1);

		if (feof(f)) break;

		if (scale_factor <= MIN_FACTOR) {
			fputc(sample & 0xff, ctx->f_phdata);
			fputc(sample >> 8, ctx->f_phdata);
		} else {
			x = ((float)sample / scale_factor) + 0.5;
			sample2 = (int)x;
			if (sample2 > 127)
				sample2 = 127;
			if (sample2 < -128)
				sample2 = -128;
			fputc(sample2, ctx->f_phdata);
		}
	}

	length = ftell(ctx->f_phdata);
	while ((length & 3) != 0) {
		// pad to a multiple of 4 bytes
		fputc(0, ctx->f_phdata);
		length++;
	}

	return displ | 0x800000; // set bit 23 to indicate a wave file rather than a spectrum
}

static espeak_ng_STATUS LoadEnvelope(CompileContext *ctx, FILE *f, int *displ)
{
	char buf[128];

	if (displ)
		*displ = ftell(ctx->f_phdata);

	if (fseek(f, 12, SEEK_SET) == -1)
		return errno;

	if (fread(buf, 128, 1, f) != 128)
		return errno;
	fwrite(buf, 128, 1, ctx->f_phdata);

	return ENS_OK;
}

// Generate a hash code from the specified string
static int Hash8(const char *string)
{
	int c;
	int chars = 0;
	int hash = 0;

	while ((c = *string++) != 0) {
		c = tolower(c) - 'a';
		hash = hash * 8 + c;
		hash = (hash & 0x1ff) ^ (hash >> 8); // exclusive or
		chars++;
	}

	return (hash+chars) & 0xff;
}

static int LoadEnvelope2(CompileContext *ctx, FILE *f)
{
	int displ;
	int n_points;
	char line_buf[128];
	float env_x[20];
	float env_y[20];
	int env_lin[20];
	unsigned char env[ENV_LEN];

	n_points = 0;
	if (fgets(line_buf, sizeof(line_buf), f) != NULL) { ; // skip first line, then loop
		while (!feof(f)) {
			if (fgets(line_buf, sizeof(line_buf), f) == NULL)
				break;

			env_lin[n_points] = 0;
			int n = sscanf(line_buf, "%f %f %d", &env_x[n_points], &env_y[n_points], &env_lin[n_points]);
			if (n >= 2) {
				env_x[n_points] *= (float)1.28; // convert range 0-100 to 0-128
				n_points++;
			}
		}
	}

	if (n_points > 0) {
		env_x[n_points] = env_x[n_points-1];
    	env_y[n_points] = env_y[n_points-1];

		int ix = 0;
    	int ix2 = 0;
		for (int x = 0; x < ENV_LEN; x++) {
		if (n_points > 3 && x > env_x[ix+3])
			ix++;
		if (n_points > 2 && x >= env_x[ix2+1])
			ix2++;

		int y;
		if (env_lin[ix2] > 0) {
			y = (env_y[ix2] + (env_y[ix2+1] - env_y[ix2]) * ((float)x - env_x[ix2]) / (env_x[ix2+1] - env_x[ix2])) * 2.55;
		} else if (n_points > 3)
			y = (int)(polint(&env_x[ix], &env_y[ix], 4, x) * 255 / 100); // convert to range 0-255
		else
			y = (int)(polint(&env_x[ix], &env_y[ix], 3, x) * 255 / 100);
		if (y < 0) y = 0;
		if (y > 255) y = 255;
		env[x] = y;
	}
	}

	displ = ftell(ctx->f_phdata);
	fwrite(env, 1, ENV_LEN, ctx->f_phdata);

	return displ;
}

static espeak_ng_STATUS LoadDataFile(CompileContext *ctx, const char *path, int control, int *addr)
{
	// load spectrum sequence or sample data from a file.
	// return index into spect or sample data area. bit 23=1 if a sample

	int hash;
	REF_HASH_TAB *p, *p2;

	if (strcmp(path, "NULL") == 0)
		return ENS_OK;
	if (strcmp(path, "DFT") == 0) {
		*addr = 1;
		return ENS_OK;
	}

	ctx->count_references++;

	hash = Hash8(path);
	p = ctx->ref_hash_tab[hash];
	while (p != NULL) {
		if (strcmp(path, p->string) == 0) {
			ctx->duplicate_references++;
			*addr = p->value; // already loaded this data
			break;
		}
		p = (REF_HASH_TAB *)p->link;
	}

	if (*addr == 0) {
		char buf[sizeof(path_home)+150];
		sprintf(buf, "%s/%s", ctx->phsrc, path);

		FILE *f;
		if ((f = fopen(buf, "rb")) == NULL) {
			sprintf(buf, "%s/%s.wav", ctx->phsrc, path);
			if ((f = fopen(buf, "rb")) == NULL) {
				error(ctx, "Can't read file: %s", path);
				return errno;
			}
		}

		int id = Read4Bytes(f);
		rewind(f);

		espeak_ng_STATUS status = ENS_OK;
		int type_code = ' ';
		if (id == 0x43455053) {
			status = LoadSpect(ctx, path, control, addr);
			type_code = 'S';
		} else if (id == 0x46464952) {
			*addr = LoadWavefile(ctx, f, path);
			type_code = 'W';
		} else if (id == 0x43544950) {
			status = LoadEnvelope(ctx, f, addr);
			type_code = 'E';
		} else if (id == 0x45564E45) {
			*addr = LoadEnvelope2(ctx, f);
			type_code = 'E';
		} else {
			error(ctx, "File not SPEC or RIFF: %s", path);
			*addr = -1;
			status = ENS_UNSUPPORTED_PHON_FORMAT;
		}
		fclose(f);

		if (status != ENS_OK)
			return status;

		if (*addr > 0)
			fprintf(ctx->f_phcontents, "%c  0x%.5x  %s\n", type_code, *addr & 0x7fffff, path);
	}

	// add this item to the hash table
	if (*addr > 0) {
		p = ctx->ref_hash_tab[hash];
		p2 = (REF_HASH_TAB *)malloc(sizeof(REF_HASH_TAB)+strlen(path)+1);
		if (p2 == NULL)
			return ENOMEM;
		p2->value = *addr;
		p2->ph_mnemonic = ctx->phoneme_out->mnemonic; // phoneme which uses this file
		p2->ph_table = ctx->n_phoneme_tabs-1;
		strcpy(p2->string, path);
		p2->link = (char *)p;
		ctx->ref_hash_tab[hash] = p2;
	}

	return ENS_OK;
}

static void CompileToneSpec(CompileContext *ctx)
{
	int pitch1 = 0;
	int pitch2 = 0;
	int pitch_env = 0;
	int amp_env = 0;

	pitch1 = NextItemBrackets(ctx, tNUMBER, 2);
	pitch2 = NextItemBrackets(ctx, tNUMBER, 3);

	if (ctx->item_terminator == ',') {
		NextItemBrackets(ctx, tSTRING, 3);
		LoadDataFile(ctx, ctx->item_string, 0, &pitch_env);
	}

	if (ctx->item_terminator == ',') {
		NextItemBrackets(ctx, tSTRING, 1);
		LoadDataFile(ctx, ctx->item_string, 0, &amp_env);
	}

	if (pitch1 < pitch2) {
		ctx->phoneme_out->start_type = pitch1;
		ctx->phoneme_out->end_type = pitch2;
	} else {
		ctx->phoneme_out->start_type = pitch2;
		ctx->phoneme_out->end_type = pitch1;
	}

	if (pitch_env != 0) {
		*ctx->prog_out++ = i_PITCHENV + ((pitch_env >> 16) & 0xf);
		*ctx->prog_out++ = pitch_env;
	}
	if (amp_env != 0) {
		*ctx->prog_out++ = i_AMPENV + ((amp_env >> 16) & 0xf);
		*ctx->prog_out++ = amp_env;
	}
}



static void CompileSound(CompileContext *ctx, int keyword, int isvowel)
{
	int addr = 0;
	int value = 0;
	char path[N_ITEM_STRING];
	static const int sound_instns[] = { i_FMT, i_WAV, i_VWLSTART, i_VWLENDING, i_WAVADD };

	NextItemBrackets(ctx, tSTRING, 2);
	strcpy(path, ctx->item_string);
	if (ctx->item_terminator == ',') {
		if ((keyword == kVOWELSTART) || (keyword == kVOWELENDING)) {
			value = NextItemBrackets(ctx, tSIGNEDNUMBER, 1);
			if (value > 127) {
				value = 127;
				error(ctx, "Parameter > 127");
			}
			if (value < -128) {
				value = -128;
				error(ctx, "Parameter < -128");
			}
		} else {
			value = NextItemBrackets(ctx, tNUMBER, 1);
			if (value > 255) {
				value = 255;
				error(ctx, "Parameter > 255");
			}
		}
	}
	LoadDataFile(ctx, path, isvowel, &addr);
	addr = addr / 4; // addr is words not bytes

	*ctx->prog_out++ = sound_instns[keyword-kFMT] + ((value & 0xff) << 4) + ((addr >> 16) & 0xf);
	*ctx->prog_out++ = addr & 0xffff;
}

/*
   Condition
   bits 14,15   1
   bit 13       1 = AND, 0 = OR
   bit 12       spare
   bit 8-11
   =0-3       p,t,n,n2   data=phoneme code
   =4-7       p,t,n,n2   data=(bits5-7: phtype, place, property, special) (bits0-4: data)
   =8         data = stress bitmap
   =9         special tests
 */
static int CompileIf(CompileContext *ctx, int elif)
{
	bool finish = false;
	int word = 0;
	int data;
	int bitmap;
	int brackets;
	unsigned short *prog_last_if = NULL;

	ctx->then_count = 2;
	ctx->after_if = true;

	while (!finish) {
		bool not_flag = false;
		int word2 = 0;
		if (ctx->prog_out >= ctx->prog_out_max) {
			error(ctx, "Phoneme program too large");
			return 0;
		}

		int key;
		if ((key = NextItem(ctx, tCONDITION)) < 0)
			error(ctx, "Expected a condition, not '%s'", ctx->item_string);

		if ((ctx->item_type == 0) && (key == k_NOT)) {
			not_flag = true;
			if ((key = NextItem(ctx, tCONDITION)) < 0)
				error(ctx, "Expected a condition, not '%s'", ctx->item_string);
		}

		if (ctx->item_type == tWHICH_PHONEME) {
			// prevPh(), thisPh(), nextPh(), next2Ph() etc
			if (key >= 6) {
				// put the 'which' code in the next instruction
				word2 = key;
				key = 6;
			}
			key = key << 8;

			data = NextItemBrackets(ctx, tPROPERTIES, 0);
			if (data >= 0)
				word = key + data + 0x700;
			else {
				data = LookupPhoneme(ctx, ctx->item_string, 2);
				word = key + data;
			}
		} else if (ctx->item_type == tTEST) {
			if (key == kTHISSTRESS) {
				bitmap = 0;
				brackets = 2;
				do {
					data = NextItemBrackets(ctx, tNUMBER, brackets);
					if (data > 7)
						error(ctx, "Expected list of stress levels");
					bitmap |= (1 << data);

					brackets = 3;
				} while (ctx->item_terminator == ',');
				word = i_StressLevel | bitmap;
			} else
				word = key;
		} else {
			error(ctx, "Unexpected keyword '%s'", ctx->item_string);

			if ((strcmp(ctx->item_string, "phoneme") == 0) || (strcmp(ctx->item_string, "endphoneme") == 0))
				return -1;
		}

		// output the word
		prog_last_if = ctx->prog_out;
		*ctx->prog_out++ = word | i_CONDITION;

		if (word2 != 0)
			*ctx->prog_out++ = word2;
		if (not_flag)
			*ctx->prog_out++ = i_NOT;

		// expect AND, OR, THEN
		switch (NextItem(ctx, tCONDITION))
		{
		case k_AND:
			break;
		case k_OR:
			if (prog_last_if != NULL)
				*prog_last_if |=  i_OR;
			break;
		case k_THEN:
			finish = true;
			break;
		default:
			error(ctx, "Expected AND, OR, THEN");
			break;
		}
	}

	if (elif == 0) {
		ctx->if_level++;
		ctx->if_stack[ctx->if_level].p_else = NULL;
	}

	ctx->if_stack[ctx->if_level].returned = false;
	ctx->if_stack[ctx->if_level].p_then = ctx->prog_out;
	*ctx->prog_out++ = i_JUMP_FALSE;

	return 0;
}

static void FillThen(CompileContext *ctx, int add)
{
	unsigned short *p;

	p = ctx->if_stack[ctx->if_level].p_then;
	if (p != NULL) {
		int offset = ctx->prog_out - p + add;

		if ((ctx->then_count == 1) && (ctx->if_level == 1)) {
			// The THEN part only contains one statement, we can remove the THEN jump
			// and the interpreter will implicitly skip the statement.
			while (p < ctx->prog_out) {
				p[0] = p[1];
				p++;
			}
			ctx->prog_out--;
		} else {
			if (offset > MAX_JUMP)
				error(ctx, "IF block is too long");
			*p = i_JUMP_FALSE + offset;
		}
		ctx->if_stack[ctx->if_level].p_then = NULL;
	}

	ctx->then_count = 0;
}

static int CompileElse(CompileContext *ctx)
{
	unsigned short *ref;

	if (ctx->if_level < 1) {
		error(ctx, "ELSE not expected");
		return 0;
	}

	if (ctx->if_stack[ctx->if_level].returned == false)
		FillThen(ctx, 1);
	else
		FillThen(ctx, 0);

	if (ctx->if_stack[ctx->if_level].returned == false) {
		ref = ctx->prog_out;
		*ctx->prog_out++ = 0;

		unsigned short *p;
		if ((p = ctx->if_stack[ctx->if_level].p_else) != NULL)
			*ref = ref - p; // backwards offset to the previous else
		ctx->if_stack[ctx->if_level].p_else = ref;
	}

	return 0;
}

static int CompileElif(CompileContext *ctx)
{
	if (ctx->if_level < 1) {
		error(ctx, "ELIF not expected");
		return 0;
	}

	CompileElse(ctx);
	CompileIf(ctx, 1);
	return 0;
}

static int CompileEndif(CompileContext *ctx)
{
	unsigned short *p;

	if (ctx->if_level < 1) {
		error(ctx, "ENDIF not expected");
		return 0;
	}

	FillThen(ctx, 0);

	if ((p = ctx->if_stack[ctx->if_level].p_else) != NULL) {
		int chain;
		do {
			chain = *p; // a chain of previous else links

			int offset = ctx->prog_out - p;
			if (offset > MAX_JUMP)
				error(ctx, "IF block is too long");
			*p = i_JUMP + offset;

			p -= chain;
		} while (chain > 0);
	}

	ctx->if_level--;
	return 0;
}

static int CompileSwitch(CompileContext *ctx, int type)
{
	// Type 0:  EndSwitch
	//      1:  SwitchPrevVowelType
	//      2:  SwitchNextVowelType

	if (type == 0) {
		// check the instructions in the Switch
		return 0;
	}

	if (type == 1)
		*ctx->prog_out++ = i_SWITCH_PREVVOWEL+6;
	if (type == 2)
		*ctx->prog_out++ = i_SWITCH_NEXTVOWEL+6;
	return 0;
}

static PHONEME_TAB_LIST *FindPhonemeTable(CompileContext *ctx, const char *string)
{
	int ix;

	for (ix = 0; ix < ctx->n_phoneme_tabs; ix++) {
		if (strcmp(ctx->phoneme_tab_list2[ix].name, string) == 0)
			return &ctx->phoneme_tab_list2[ix];
	}
	error(ctx, "compile: unknown phoneme table: '%s'", string);
	return NULL;
}

static PHONEME_TAB *FindPhoneme(CompileContext *ctx, const char *string)
{
	PHONEME_TAB_LIST *phtab = NULL;
	int ix;
	unsigned int mnem;
	char *phname;
	char buf[200];

	// is this the name of a phoneme which is in scope
	if ((strlen(string) <= 4) && ((ix = LookupPhoneme(ctx, string, 0)) != -1))
		return &ctx->phoneme_tab2[ix];

	// no, treat the name as phonemetable/phoneme
	strcpy(buf, string);
	if ((phname = strchr(buf, '/')) != 0)
		*phname++ = 0;

	phtab = FindPhonemeTable(ctx, buf);
	if (phtab == NULL)
		return NULL; // phoneme table not found

	mnem = StringToWord(phname);
	for (ix = 1; ix < 256; ix++) {
		if (mnem == phtab->phoneme_tab_ptr[ix].mnemonic)
			return &phtab->phoneme_tab_ptr[ix];
	}

	error(ctx, "Phoneme reference not found: '%s'", string);
	return NULL;
}

static void ImportPhoneme(CompileContext *ctx)
{
	unsigned int ph_mnem;
	unsigned int ph_code;
	PHONEME_TAB *ph;

	NextItem(ctx, tSTRING);

	if ((ph = FindPhoneme(ctx, ctx->item_string)) == NULL) {
		error(ctx, "Cannot find phoneme '%s' to import.", ctx->item_string);
		return;
	}

	if (ctx->phoneme_out->phflags != 0 ||
	    ctx->phoneme_out->type != phINVALID ||
	    ctx->phoneme_out->start_type != 0 ||
	    ctx->phoneme_out->end_type != 0 ||
	    ctx->phoneme_out->std_length != 0 ||
	    ctx->phoneme_out->length_mod != 0) {
		error(ctx, "Phoneme import will override set properties.");
	}

	ph_mnem = ctx->phoneme_out->mnemonic;
	ph_code = ctx->phoneme_out->code;
	memcpy(ctx->phoneme_out, ph, sizeof(PHONEME_TAB));
	ctx->phoneme_out->mnemonic = ph_mnem;
	ctx->phoneme_out->code = ph_code;
	if (ctx->phoneme_out->type != phVOWEL)
		ctx->phoneme_out->end_type = 0;  // voicingswitch, this must be set later to refer to a local phoneme
}

static void CallPhoneme(CompileContext *ctx)
{
	PHONEME_TAB *ph;
	int ix;
	int addr = 0;

	NextItem(ctx, tSTRING);

	// first look for a procedure name
	for (ix = 0; ix < ctx->n_procs; ix++) {
		if (strcmp(ctx->proc_names[ix], ctx->item_string) == 0) {
			addr = ctx->proc_addr[ix];
			break;
		}
	}
	if (ix == ctx->n_procs) {
		// procedure not found, try a phoneme name
		if ((ph = FindPhoneme(ctx, ctx->item_string)) == NULL)
			return;
		addr = ph->program;

		if (ctx->phoneme_out->type == phINVALID) {
			// Phoneme type has not been set. Copy it from the called phoneme
			ctx->phoneme_out->type = ph->type;
			ctx->phoneme_out->start_type = ph->start_type;
			ctx->phoneme_out->end_type = ph->end_type;
			ctx->phoneme_out->std_length = ph->std_length;
			ctx->phoneme_out->length_mod = ph->length_mod;

			ctx->phoneme_flags = ph->phflags & ~phARTICULATION;
		}
	}

	*ctx->prog_out++ = i_CALLPH + (addr >> 16);
	*ctx->prog_out++ = addr;
}

static void DecThenCount(CompileContext *ctx)
{
	if (ctx->then_count > 0)
		ctx->then_count--;
}

static int CompilePhoneme(CompileContext *ctx, int compile_phoneme)
{
	int endphoneme = 0;
	int value;
	int phcode = 0;
	int flags;
	int ix;
	int start;
	int count;
	int c;
	char *p;
	int vowel_length_factor = 100; // for testing
	char number_buf[12];
	char ipa_buf[N_ITEM_STRING+1];
	PHONEME_TAB phoneme_out2;
	PHONEME_PROG_LOG phoneme_prog_log;

	ctx->prog_out = ctx->prog_buf;
	ctx->prog_out_max = &ctx->prog_buf[MAX_PROG_BUF-1];
	ctx->if_level = 0;
	ctx->if_stack[0].returned = false;
	ctx->after_if = false;
	ctx->phoneme_flags = 0;

	NextItem(ctx, tSTRING);
	if (compile_phoneme) {
		phcode = LookupPhoneme(ctx, ctx->item_string, 1); // declare phoneme if not already there
		if (phcode == -1) return 0;
		ctx->phoneme_out = &ctx->phoneme_tab2[phcode];
	} else {
		// declare a procedure
		if (ctx->n_procs >= N_PROCS) {
			error(ctx, "Too many procedures");
			return 0;
		}
		strcpy(ctx->proc_names[ctx->n_procs], ctx->item_string);
		ctx->phoneme_out = &phoneme_out2;
		sprintf(number_buf, "%.3dP", ctx->n_procs);
		ctx->phoneme_out->mnemonic = StringToWord(number_buf);
	}

	ctx->phoneme_out->code = phcode;
	ctx->phoneme_out->program = 0;
	ctx->phoneme_out->type = phINVALID;
	ctx->phoneme_out->std_length = 0;
	ctx->phoneme_out->start_type = 0;
	ctx->phoneme_out->end_type = 0;
	ctx->phoneme_out->length_mod = 0;
	ctx->phoneme_out->phflags = 0;

	while (!endphoneme && !feof(ctx->f_in)) {
		int keyword;
		if ((keyword = NextItem(ctx, tKEYWORD)) < 0) {
			if (keyword == -2) {
				error(ctx, "Missing 'endphoneme' before end-of-file"); // end of file
				break;
			}

			phoneme_feature_t feature = phoneme_feature_from_string(ctx->item_string);
			espeak_ng_STATUS status = phoneme_add_feature(ctx->phoneme_out, feature);
			if (status == ENS_OK)
				continue;
			error_from_status(ctx, status, ctx->item_string);
			continue;
		}

		switch (ctx->item_type)
		{
		case tPHONEME_TYPE:
			if (ctx->phoneme_out->type != phINVALID) {
				if (ctx->phoneme_out->type == phFRICATIVE && keyword == phLIQUID)
					; // apr liquid => ok
				else
					error(ctx, "More than one phoneme type: %s", ctx->item_string);
			}
			ctx->phoneme_out->type = keyword;
			break;
		case tPHONEME_FLAG:
			ctx->phoneme_flags |= keyword;
			break;
		case tINSTRN1:
			// instruction group 0, with 8 bit operands which set data in PHONEME_DATA
			switch (keyword)
			{
			case i_CHANGE_PHONEME:
			case i_APPEND_PHONEME:
			case i_APPEND_IFNEXTVOWEL:
			case i_INSERT_PHONEME:
			case i_REPLACE_NEXT_PHONEME:
			case i_VOICING_SWITCH:
			case i_CHANGE_IF | STRESS_IS_DIMINISHED:
			case i_CHANGE_IF | STRESS_IS_UNSTRESSED:
			case i_CHANGE_IF | STRESS_IS_NOT_STRESSED:
			case i_CHANGE_IF | STRESS_IS_SECONDARY:
			case i_CHANGE_IF | STRESS_IS_PRIMARY:
				value = NextItemBrackets(ctx, tPHONEMEMNEM, 0);
				*ctx->prog_out++ = (keyword << 8) + value;
				DecThenCount(ctx);
				break;
			case i_PAUSE_BEFORE:
				value = NextItemMax(ctx, 255);
				*ctx->prog_out++ = (i_PAUSE_BEFORE << 8) + value;
				DecThenCount(ctx);
				break;
			case i_PAUSE_AFTER:
				value = NextItemMax(ctx, 255);
				*ctx->prog_out++ = (i_PAUSE_AFTER << 8) + value;
				DecThenCount(ctx);
				break;
			case i_SET_LENGTH:
				value = NextItemMax(ctx, 511);
				if (ctx->phoneme_out->type == phVOWEL)
					value = (value * vowel_length_factor)/100;

				if (ctx->after_if == false)
					ctx->phoneme_out->std_length = value/2;
				else {
					*ctx->prog_out++ = (i_SET_LENGTH << 8) + value/2;
					DecThenCount(ctx);
				}
				break;
			case i_ADD_LENGTH:
				value = NextItem(ctx, tSIGNEDNUMBER) / 2;
				*ctx->prog_out++ = (i_ADD_LENGTH << 8) + (value & 0xff);
				DecThenCount(ctx);
				break;
			case i_LENGTH_MOD:
				value = NextItem(ctx, tNUMBER);
				assert(value < LENGTH_MOD_LIMIT);
				ctx->phoneme_out->length_mod = value;
				break;
			case i_IPA_NAME:
				NextItem(ctx, tSTRING);

				if (strcmp(ctx->item_string, "NULL") == 0)
					strcpy(ctx->item_string, " ");

				// copy the string, recognize characters in the form U+9999
				flags = 0;
				count = 0;
				ix = 1;

				for (p = ctx->item_string; *p != 0;) {
					p += utf8_in(&c, p);

					if ((c == '|') && (count > 0)) {
						// '|' means don't allow a tie or joiner before this letter
						flags |= (1 << (count -1));
					} else if ((c == 'U') && (p[0] == '+')) {
						int j;
						// U+9999
						p++;
						memcpy(number_buf, p, 4); // U+ should be followed by 4 hex digits
						number_buf[4] = 0;
						c = '#';
						sscanf(number_buf, "%x", (unsigned int *)&c);

						// move past the 4 hexdecimal digits
						for (j = 0; j < 4; j++) {
							if (!isalnum(*p))
								break;
							p++;
						}
						ix += utf8_out(c, &ipa_buf[ix]);
						count++;
					} else {
						ix += utf8_out(c, &ipa_buf[ix]);
						count++;
					}
				}
				ipa_buf[0] = flags;
				ipa_buf[ix] = 0;

				start = 1;
				if (flags != 0)
					start = 0; // only include the flags byte if bits are set
				value = strlen(&ipa_buf[start]); // number of UTF-8 bytes

				*ctx->prog_out++ = (i_IPA_NAME << 8) + value;
				for (ix = 0; ix < value; ix += 2)
					*ctx->prog_out++ = (ipa_buf[ix+start] << 8) + (ipa_buf[ix+start+1] & 0xff);
				DecThenCount(ctx);
				break;
			}
			break;
		case tSTATEMENT:
			switch (keyword)
			{
			case kIMPORT_PH:
				ImportPhoneme(ctx);
				ctx->phoneme_flags = ctx->phoneme_out->phflags;
				break;
			case kSTARTTYPE:
				phcode = NextItem(ctx, tPHONEMEMNEM);
				if (phcode == -1)
					phcode = LookupPhoneme(ctx, ctx->item_string, 1);
				ctx->phoneme_out->start_type = phcode;
				if (ctx->phoneme_out->type == phINVALID)
					error(ctx, "a phoneme type or manner of articulation must be specified before starttype");
				break;
			case kENDTYPE:
				phcode = NextItem(ctx, tPHONEMEMNEM);
				if (phcode == -1)
					phcode = LookupPhoneme(ctx, ctx->item_string, 1);
				if (ctx->phoneme_out->type == phINVALID)
					error(ctx, "a phoneme type or manner of articulation must be specified before endtype");
				else if (ctx->phoneme_out->type == phVOWEL)
					ctx->phoneme_out->end_type = phcode;
				else if (phcode != ctx->phoneme_out->start_type)
					error(ctx, "endtype must equal starttype for consonants");
				break;
			case kVOICINGSWITCH:
				phcode = NextItem(ctx, tPHONEMEMNEM);
				if (phcode == -1)
					phcode = LookupPhoneme(ctx, ctx->item_string, 1);
				if (ctx->phoneme_out->type == phVOWEL)
					error(ctx, "voicingswitch cannot be used on vowels");
				else
					ctx->phoneme_out->end_type = phcode; // use end_type field for consonants as voicing_switch
				break;
			case kSTRESSTYPE:
				value = NextItem(ctx, tNUMBER);
				ctx->phoneme_out->std_length = value;
				if (ctx->prog_out > ctx->prog_buf) {
					error(ctx, "stress phonemes can't contain program instructions");
					ctx->prog_out = ctx->prog_buf;
				}
				break;
			case kIF:
				endphoneme = CompileIf(ctx, 0);
				break;
			case kELSE:
				endphoneme = CompileElse(ctx);
				break;
			case kELIF:
				endphoneme = CompileElif(ctx);
				break;
			case kENDIF:
				endphoneme = CompileEndif(ctx);
				break;
			case kENDSWITCH:
				break;
			case kSWITCH_PREVVOWEL:
				endphoneme = CompileSwitch(ctx, 1);
				break;
			case kSWITCH_NEXTVOWEL:
				endphoneme = CompileSwitch(ctx, 2);
				break;
			case kCALLPH:
				CallPhoneme(ctx);
				DecThenCount(ctx);
				break;
			case kFMT:
				ctx->if_stack[ctx->if_level].returned = true;
				DecThenCount(ctx);
				if (ctx->phoneme_out->type == phVOWEL)
					CompileSound(ctx, keyword, 1);
				else
					CompileSound(ctx, keyword, 0);
				break;
			case kWAV:
				ctx->if_stack[ctx->if_level].returned = true;
				// fallthrough:
			case kVOWELSTART:
			case kVOWELENDING:
			case kANDWAV:
				DecThenCount(ctx);
				CompileSound(ctx, keyword, 0);
				break;
			case kVOWELIN:
				DecThenCount(ctx);
				endphoneme = CompileVowelTransition(ctx, 1);
				break;
			case kVOWELOUT:
				DecThenCount(ctx);
				endphoneme = CompileVowelTransition(ctx, 2);
				break;
			case kTONESPEC:
				DecThenCount(ctx);
				CompileToneSpec(ctx);
				break;
			case kCONTINUE:
				*ctx->prog_out++ = INSTN_CONTINUE;
				DecThenCount(ctx);
				break;
			case kRETURN:
				*ctx->prog_out++ = INSTN_RETURN;
				DecThenCount(ctx);
				break;
			case kINCLUDE:
			case kPHONEMETABLE:
				error(ctx, "Missing 'endphoneme' before '%s'", ctx->item_string);  // drop through to endphoneme
				// fallthrough:
			case kENDPHONEME:
			case kENDPROCEDURE:
				endphoneme = 1;
				if (ctx->if_level > 0)
					error(ctx, "Missing ENDIF");
				if ((ctx->prog_out > ctx->prog_buf) && (ctx->if_stack[0].returned == false))
					*ctx->prog_out++ = INSTN_RETURN;
				break;
			}
			break;
		}
	}

	if (endphoneme != 1)
		error(ctx, "'endphoneme' not expected here");

	if (compile_phoneme) {
		if (ctx->phoneme_out->type == phINVALID) {
			error(ctx, "Phoneme type is missing");
			ctx->phoneme_out->type = 0;
		}
		ctx->phoneme_out->phflags |= ctx->phoneme_flags;

		if (ctx->phoneme_out->phflags & phVOICED) {
			if (ctx->phoneme_out->type == phSTOP)
				ctx->phoneme_out->type = phVSTOP;
			else if (ctx->phoneme_out->type == phFRICATIVE)
				ctx->phoneme_out->type = phVFRICATIVE;
		}

		if (ctx->phoneme_out->std_length == 0) {
			if (ctx->phoneme_out->type == phVOWEL)
				ctx->phoneme_out->std_length = 180/2; // default length for vowel
		}

		ctx->phoneme_out->phflags |= phLOCAL; // declared in this phoneme table

		if (ctx->phoneme_out->type == phDELETED)
			ctx->phoneme_out->mnemonic = 0x01; // will not be recognised
	}

	if (ctx->prog_out > ctx->prog_buf) {
		// write out the program for this phoneme
		fflush(ctx->f_phindex);
		ctx->phoneme_out->program = ftell(ctx->f_phindex) / sizeof(unsigned short);

		if (ctx->f_prog_log != NULL) {
			phoneme_prog_log.addr = ctx->phoneme_out->program;
			phoneme_prog_log.length = ctx->prog_out - ctx->prog_buf;
			fwrite(&phoneme_prog_log, 1, sizeof(phoneme_prog_log), ctx->f_prog_log);
		}

		if (compile_phoneme == 0)
			ctx->proc_addr[ctx->n_procs++] =  ftell(ctx->f_phindex) / sizeof(unsigned short);
		fwrite(ctx->prog_buf, sizeof(unsigned short), ctx->prog_out - ctx->prog_buf, ctx->f_phindex);
	}

	return 0;
}

static void WritePhonemeTables(CompileContext *ctx)
{
	int ix;
	int j;
	int value;
	PHONEME_TAB *p;

	value = ctx->n_phoneme_tabs;
	fputc(value, ctx->f_phtab);
	fputc(0, ctx->f_phtab);
	fputc(0, ctx->f_phtab);
	fputc(0, ctx->f_phtab);

	for (ix = 0; ix < ctx->n_phoneme_tabs; ix++) {
		p = ctx->phoneme_tab_list2[ix].phoneme_tab_ptr;
		int n = ctx->n_phcodes_list[ix];
		memset(&p[n], 0, sizeof(p[n]));
		p[n].mnemonic = 0; // terminate the phoneme table

		// count number of locally declared phonemes
		int count = 0;
		for (j = 0; j < n; j++) {
			if (ix == 0)
				p[j].phflags |= phLOCAL; // write all phonemes in the base phoneme table

			if (p[j].phflags & phLOCAL)
				count++;
		}
		ctx->phoneme_tab_list2[ix].n_phonemes = count+1;

		fputc(count+1, ctx->f_phtab);
		fputc(ctx->phoneme_tab_list2[ix].includes, ctx->f_phtab);
		fputc(0, ctx->f_phtab);
		fputc(0, ctx->f_phtab);

		fwrite(ctx->phoneme_tab_list2[ix].name, 1, N_PHONEME_TAB_NAME, ctx->f_phtab);

		for (j = 0; j < n; j++) {
			if (p[j].phflags & phLOCAL) {
				// this bit is set temporarily to incidate a local phoneme, declared in
				// in the current phoneme file
				p[j].phflags &= ~phLOCAL;
				fwrite(&p[j], sizeof(PHONEME_TAB), 1, ctx->f_phtab);
			}
		}
		fwrite(&p[n], sizeof(PHONEME_TAB), 1, ctx->f_phtab); // include the extra list-terminator phoneme entry
		free(p);
	}
}

static void EndPhonemeTable(CompileContext *ctx)
{
	int ix;
	char buf[5];

	if (ctx->n_phoneme_tabs == 0)
		return;

	// check that all referenced phonemes have been declared
	for (ix = 0; ix < ctx->n_phcodes; ix++) {
		if (ctx->phoneme_tab2[ix].type == phINVALID) {
			error(ctx, "Phoneme [%s] not declared, referenced at line %d",
			      WordToString(buf, ctx->phoneme_tab2[ix].mnemonic), (int)(ctx->phoneme_tab2[ix].program));
			ctx->error_count++;
			ctx->phoneme_tab2[ix].type = 0; // prevent the error message repeating
		}
	}

	ctx->n_phcodes_list[ctx->n_phoneme_tabs-1] = ctx->n_phcodes;
}

static void StartPhonemeTable(CompileContext *ctx, const char *name)
{
	PHONEME_TAB *p;

	if (ctx->n_phoneme_tabs >= N_PHONEME_TABS-1) {
		error(ctx, "Too many phonemetables");
		return;
	}
	p = (PHONEME_TAB *)calloc(sizeof(PHONEME_TAB), N_PHONEME_TAB);

	if (p == NULL) {
		error(ctx, "Out of memory");
		return;
	}

	memset(&ctx->phoneme_tab_list2[ctx->n_phoneme_tabs], 0, sizeof(PHONEME_TAB_LIST));
	ctx->phoneme_tab_list2[ctx->n_phoneme_tabs].phoneme_tab_ptr = ctx->phoneme_tab2 = p;
	memset(ctx->phoneme_tab_list2[ctx->n_phoneme_tabs].name, 0, sizeof(ctx->phoneme_tab_list2[ctx->n_phoneme_tabs].name));
	strncpy0(ctx->phoneme_tab_list2[ctx->n_phoneme_tabs].name, name, N_PHONEME_TAB_NAME);
	ctx->n_phcodes = 1;
	ctx->phoneme_tab_list2[ctx->n_phoneme_tabs].includes = 0;

	if (ctx->n_phoneme_tabs > 0) {
		NextItem(ctx, tSTRING); // name of base phoneme table
		int ix;
		for (ix = 0; ix < ctx->n_phoneme_tabs; ix++) {
			if (strcmp(ctx->item_string, ctx->phoneme_tab_list2[ix].name) == 0) {
				ctx->phoneme_tab_list2[ctx->n_phoneme_tabs].includes = ix+1;

				// initialise the new phoneme table with the contents of this one
				memcpy(ctx->phoneme_tab2, ctx->phoneme_tab_list2[ix].phoneme_tab_ptr, sizeof(PHONEME_TAB)*N_PHONEME_TAB);
				ctx->n_phcodes = ctx->n_phcodes_list[ix];

				// clear "local phoneme" bit"
				int j;
				for (j = 0; j < ctx->n_phcodes; j++)
					ctx->phoneme_tab2[j].phflags &= ~phLOCAL;
				break;
			}
		}
		if (ix == ctx->n_phoneme_tabs && strcmp(ctx->item_string, "_") != 0)
			error(ctx, "Can't find base phonemetable '%s'", ctx->item_string);
	} else
		ReservePhCodes(ctx);

	ctx->n_phoneme_tabs++;
}

static void CompilePhonemeFiles(CompileContext *ctx)
{
	FILE *f;
	char buf[sizeof(path_home)+120];

	ctx->linenum = 1;

	ctx->count_references = 0;
	ctx->duplicate_references = 0;
	ctx->count_frames = 0;
	ctx->n_procs = 0;

	for (;;) {
		if (feof(ctx->f_in)) {
			// end of file, go back to previous from, from which this was included

			if (ctx->stack_ix == 0)
				break; // end of top level, finished
			fclose(ctx->f_in);
			ctx->f_in = ctx->stack[--ctx->stack_ix].file;
			strcpy(ctx->current_fname, ctx->stack[ctx->stack_ix].fname);
			ctx->linenum = ctx->stack[ctx->stack_ix].linenum;
		}

		int item = NextItem(ctx, tKEYWORD);

		switch (item)
		{
		case kUTF8_BOM:
			break; // ignore bytes 0xef 0xbb 0xbf
		case kINCLUDE:
			NextItem(ctx, tSTRING);
			sprintf(buf, "%s/%s", ctx->phsrc, ctx->item_string);

			if ((ctx->stack_ix < N_STACK) && (f = fopen(buf, "rb")) != NULL) {
				ctx->stack[ctx->stack_ix].linenum = ctx->linenum;
				strcpy(ctx->stack[ctx->stack_ix].fname, ctx->current_fname);
				ctx->stack[ctx->stack_ix++].file = ctx->f_in;

				ctx->f_in = f;
				strncpy0(ctx->current_fname, ctx->item_string, sizeof(ctx->current_fname));
				ctx->linenum = 1;
			} else
				error(ctx, "Missing file: %s", ctx->item_string);
			break;
		case kPHONEMETABLE:
			EndPhonemeTable(ctx);
			NextItem(ctx, tSTRING); // name of the new phoneme table
			StartPhonemeTable(ctx, ctx->item_string);
			break;
		case kPHONEMESTART:
			if (ctx->n_phoneme_tabs == 0) {
				error(ctx, "phonemetable is missing");
				return;
			}
			CompilePhoneme(ctx, 1);
			break;
		case kPROCEDURE:
			CompilePhoneme(ctx, 0);
			break;
		default:
			if (!feof(ctx->f_in))
				error(ctx, "Keyword 'phoneme' expected");
			break;
		}
	}
	memset(&ctx->phoneme_tab2[ctx->n_phcodes+1], 0, sizeof(ctx->phoneme_tab2[ctx->n_phcodes+1]));
	ctx->phoneme_tab2[ctx->n_phcodes+1].mnemonic = 0; // terminator
}

#pragma GCC visibility push(default)

espeak_ng_STATUS
espeak_ng_CompilePhonemeData(long rate,
                             FILE *log,
                             espeak_ng_ERROR_CONTEXT *context)
{
	return espeak_ng_CompilePhonemeDataPath(rate, NULL, NULL, log, context);
}

espeak_ng_STATUS
espeak_ng_CompilePhonemeDataPath(long rate,
                                 const char *source_path,
                                 const char *destination_path,
                                 FILE *log,
                                 espeak_ng_ERROR_CONTEXT *context)
{
	if (!log) log = stderr;

	char fname[sizeof(path_home)+40];
	char phdst[sizeof(path_home)+40]; // Destination: path to the phondata/phontab/phonindex output files.

	CompileContext *ctx = calloc(1, sizeof(CompileContext));
	if (!ctx) return ENOMEM;

	if (source_path) {
		sprintf(ctx->phsrc, "%s", source_path);
	} else {
		sprintf(ctx->phsrc, "%s/../phsource", path_home);
	}

	if (destination_path) {
		sprintf(phdst, "%s", destination_path);
	} else {
		sprintf(phdst, "%s", path_home);
	}

	samplerate = rate;
	LoadPhData(NULL, NULL);
	if (LoadVoice("", 8/*compiling phonemes*/) == NULL) {
		clean_context(ctx);
		return ENS_VOICE_NOT_FOUND;
	}

	WavegenInit(rate, 0);
	WavegenSetVoice(voice);

	ctx->error_count = 0;
	ctx->f_errors = log;

	strncpy0(ctx->current_fname, "phonemes", sizeof(ctx->current_fname));

	sprintf(fname, "%s/phonemes", ctx->phsrc);
	fprintf(log, "Compiling phoneme data: %s\n", fname);
	ctx->f_in = fopen(fname, "rb");
	if (ctx->f_in == NULL) {
		clean_context(ctx);
		return create_file_error_context(context, errno, fname);
	}

	sprintf(fname, "%s/%s", phdst, "phondata-manifest");
	if ((ctx->f_phcontents = fopen(fname, "w")) == NULL)
		ctx->f_phcontents = stderr;

	fprintf(ctx->f_phcontents,
	        "# This file lists the type of data that has been compiled into the\n"
	        "# phondata file\n"
	        "#\n"
	        "# The first character of a line indicates the type of data:\n"
	        "#   S - A SPECT_SEQ structure\n"
	        "#   W - A wavefile segment\n"
	        "#   E - An envelope\n"
	        "#\n"
	        "# Address is the displacement within phondata of this item\n"
	        "#\n"
	        "#  Address  Data file\n"
	        "#  -------  ---------\n");

	sprintf(fname, "%s/%s", phdst, "phondata");
	ctx->f_phdata = fopen(fname, "wb");
	if (ctx->f_phdata == NULL) {
		int error = errno;
		fclose(ctx->f_in);
		fclose(ctx->f_phcontents);
		clean_context(ctx);
		return create_file_error_context(context, error, fname);
	}

	sprintf(fname, "%s/%s", phdst, "phonindex");
	ctx->f_phindex = fopen(fname, "wb");
	if (ctx->f_phindex == NULL) {
		int error = errno;
		fclose(ctx->f_in);
		fclose(ctx->f_phcontents);
		fclose(ctx->f_phdata);
		clean_context(ctx);
		return create_file_error_context(context, error, fname);
	}

	sprintf(fname, "%s/%s", phdst, "phontab");
	ctx->f_phtab = fopen(fname, "wb");
	if (ctx->f_phtab == NULL) {
		int error = errno;
		fclose(ctx->f_in);
		fclose(ctx->f_phcontents);
		fclose(ctx->f_phdata);
		fclose(ctx->f_phindex);
		clean_context(ctx);
		return create_file_error_context(context, error, fname);
	}

	sprintf(fname, "%s/compile_prog_log", ctx->phsrc);
	ctx->f_prog_log = fopen(fname, "wb");

	// write a word so that further data doesn't start at displ=0
	Write4Bytes(ctx->f_phdata, version_phdata);
	Write4Bytes(ctx->f_phdata, samplerate);
	Write4Bytes(ctx->f_phindex, version_phdata);

	memset(ctx->ref_hash_tab, 0, sizeof(ctx->ref_hash_tab));

	ctx->n_phoneme_tabs = 0;
	MAKE_MEM_UNDEFINED(ctx->n_phcodes_list, sizeof(ctx->n_phcodes_list));
	MAKE_MEM_UNDEFINED(ctx->phoneme_tab_list2, sizeof(ctx->phoneme_tab_list2));

	ctx->stack_ix = 0;
	MAKE_MEM_UNDEFINED(ctx->stack, sizeof(ctx->stack));

	StartPhonemeTable(ctx, "base");
	CompilePhonemeFiles(ctx);

	EndPhonemeTable(ctx);
	WritePhonemeTables(ctx);

	fprintf(ctx->f_errors, "\nRefs %d,  Reused %d\n", ctx->count_references, ctx->duplicate_references);

	fclose(ctx->f_in);
	fclose(ctx->f_phcontents);
	fclose(ctx->f_phdata);
	fclose(ctx->f_phindex);
	fclose(ctx->f_phtab);
	if (ctx->f_prog_log != NULL)
		fclose(ctx->f_prog_log);

	LoadPhData(NULL, NULL);

	WavegenFini();

	fprintf(log, "Compiled phonemes: %d errors.\n", ctx->error_count);

	if (ctx->f_errors != stderr && ctx->f_errors != stdout)
		fclose(ctx->f_errors);

	espeak_ng_STATUS status = ReadPhondataManifest(ctx, context);
	int res = ctx->error_count > 0 ? ENS_COMPILE_ERROR : ENS_OK;
	clean_context(ctx);
	return (status != ENS_OK) ? status : res;
}

#pragma GCC visibility pop

static const char *preset_tune_names[] = {
	"s1", "c1", "q1", "e1", NULL
};

static const TUNE default_tune = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 40, 24, 8, 0, 0, 0, 0 },
	46, 57, PITCHfall, 16, 0, 0,
	255, 78, 50, 255,
	3, 5,
	{ -7, -7, -7 }, { -7, -7, -7 },
	PITCHfall, 64, 8,
	PITCHfall, 70, 18, 24, 12,
	PITCHfall, 70, 18, 24, 12, 0,
	{ 0, 0, 0, 0, 0, 0, 0, 0 }, 0
};

#define N_TUNE_NAMES  100

static const MNEM_TAB envelope_names[] = {
	{ "fall", 0 },
	{ "rise", 2 },
	{ "fall-rise", 4 },
	{ "fall-rise2", 6 },
	{ "rise-fall",  8 },
	{ "fall-rise3", 10 },
	{ "fall-rise4", 12 },
	{ "fall2", 14 },
	{ "rise2", 16 },
	{ "rise-fall-rise", 18 },
	{ NULL, -1 }
};

static int LookupEnvelopeName(const char *name)
{
	return LookupMnem(envelope_names, name);
}

#pragma GCC visibility push(default)

espeak_ng_STATUS espeak_ng_CompileIntonation(FILE *log, espeak_ng_ERROR_CONTEXT *context)
{
	return espeak_ng_CompileIntonationPath(NULL, NULL, log, context);
}

espeak_ng_STATUS
espeak_ng_CompileIntonationPath(const char *source_path,
                                const char *destination_path,
                                FILE *log,
                                espeak_ng_ERROR_CONTEXT *context
                                )
{
	if (!log) log = stderr;
	if (!source_path) source_path = path_home;
	if (!destination_path) destination_path = path_home;

	int ix;
	char *p;
	char c;
	int n_tune_names = 0;
	bool done_split = false;
	bool done_onset = false;
	bool done_last = false;
	int n_preset_tunes = 0;
	int found = 0;
	int tune_number = 0;
	FILE *f_out;
	TUNE *tune_data;
	TUNE new_tune;

	char name[12];
	char tune_names[N_TUNE_NAMES][12];
	char buf[sizeof(path_home)+150];

	CompileContext *ctx = calloc(1, sizeof(CompileContext));
	if (!ctx) return ENOMEM;

	ctx->error_count = 0;
	ctx->f_errors = log;

	sprintf(buf, "%s/../phsource/intonation.txt", source_path);
	if ((ctx->f_in = fopen(buf, "r")) == NULL) {
		sprintf(buf, "%s/../phsource/intonation", source_path);
		if ((ctx->f_in = fopen(buf, "r")) == NULL) {
			int error = errno;
			fclose(ctx->f_errors);
			clean_context(ctx);
			return create_file_error_context(context, error, buf);
		}
	}

	for (ix = 0; preset_tune_names[ix] != NULL; ix++)
		strcpy(tune_names[ix], preset_tune_names[ix]);
	n_tune_names = ix;
	n_preset_tunes = ix;

	// make a list of the tune names
	while (!feof(ctx->f_in)) {
		if (fgets(buf, sizeof(buf), ctx->f_in) == NULL)
			break;

		if ((memcmp(buf, "tune", 4) == 0) && isspace(buf[4])) {
			p = &buf[5];
			while (isspace(*p)) p++;

			ix = 0;
			while ((ix < (int)(sizeof(name) - 1)) && !isspace(*p))
				name[ix++] = *p++;
			name[ix] = 0;

			found = 0;
			for (ix = 0; ix < n_tune_names; ix++) {
				if (strcmp(name, tune_names[ix]) == 0) {
					found = 1;
					break;
				}
			}

			if (found == 0) {
				strncpy0(tune_names[n_tune_names++], name, sizeof(name));

				if (n_tune_names >= N_TUNE_NAMES)
					break;
			}
		}
	}
	rewind(ctx->f_in);
	ctx->linenum = 1;

	tune_data = (n_tune_names == 0) ? NULL : (TUNE *)calloc(n_tune_names, sizeof(TUNE));
	if (tune_data == NULL) {
		fclose(ctx->f_in);
		fclose(ctx->f_errors);
		clean_context(ctx);
		return ENOMEM;
	}

	sprintf(buf, "%s/intonations", destination_path);
	f_out = fopen(buf, "wb");
	if (f_out == NULL) {
		int error = errno;
		fclose(ctx->f_in);
		fclose(ctx->f_errors);
		free(tune_data);
		clean_context(ctx);
		return create_file_error_context(context, error, buf);
	}

	while (!feof(ctx->f_in)) {
		int keyword = NextItem(ctx, tINTONATION);

		switch (keyword)
		{
		case kTUNE:
			done_split = false;

			memcpy(&new_tune, &default_tune, sizeof(TUNE));
			NextItem(ctx, tSTRING);
			strncpy0(new_tune.name, ctx->item_string, sizeof(new_tune.name));

			found = 0;
			tune_number = 0;
			for (ix = 0; ix < n_tune_names; ix++) {
				if (strcmp(new_tune.name, tune_names[ix]) == 0) {
					found = 1;
					tune_number = ix;

					if (tune_data[ix].name[0] != 0)
						found = 2;
					break;
				}
			}
			if (found == 2)
				error(ctx, "Duplicate tune name: '%s'", new_tune.name);
			if (found == 0)
				error(ctx, "Bad tune name: '%s;", new_tune.name);
			break;
		case kENDTUNE:
			if (!found) continue;
			if (done_onset == false) {
				new_tune.unstr_start[0] = new_tune.unstr_start[1];
				new_tune.unstr_end[0] = new_tune.unstr_end[1];
			}
			if (done_last == false) {
				new_tune.unstr_start[2] = new_tune.unstr_start[1];
				new_tune.unstr_end[2] = new_tune.unstr_end[1];
			}
			memcpy(&tune_data[tune_number], &new_tune, sizeof(TUNE));
			break;
		case kTUNE_PREHEAD:
			new_tune.prehead_start = NextItem(ctx, tNUMBER);
			new_tune.prehead_end = NextItem(ctx, tNUMBER);
			break;
		case kTUNE_ONSET:
			new_tune.onset = NextItem(ctx, tNUMBER);
			new_tune.unstr_start[0] = NextItem(ctx, tSIGNEDNUMBER);
			new_tune.unstr_end[0] = NextItem(ctx, tSIGNEDNUMBER);
			done_onset = true;
			break;
		case kTUNE_HEADLAST:
			new_tune.head_last = NextItem(ctx, tNUMBER);
			new_tune.unstr_start[2] = NextItem(ctx, tSIGNEDNUMBER);
			new_tune.unstr_end[2] = NextItem(ctx, tSIGNEDNUMBER);
			done_last = true;
			break;
		case kTUNE_HEADENV:
			NextItem(ctx, tSTRING);
			if ((ix = LookupEnvelopeName(ctx->item_string)) < 0)
				error(ctx, "Bad envelope name: '%s'", ctx->item_string);
			else
				new_tune.stressed_env = ix;
			new_tune.stressed_drop = NextItem(ctx, tNUMBER);
			break;
		case kTUNE_HEAD:
			new_tune.head_max_steps = NextItem(ctx, tNUMBER);
			new_tune.head_start = NextItem(ctx, tNUMBER);
			new_tune.head_end = NextItem(ctx, tNUMBER);
			new_tune.unstr_start[1] = NextItem(ctx, tSIGNEDNUMBER);
			new_tune.unstr_end[1] = NextItem(ctx, tSIGNEDNUMBER);
			break;
		case kTUNE_HEADEXTEND:
			// up to 8 numbers
			for (ix = 0; ix < (int)(sizeof(new_tune.head_extend)); ix++) {
				if (!isdigit(c = CheckNextChar(ctx)) && (c != '-'))
					break;

				new_tune.head_extend[ix] = (NextItem(ctx, tSIGNEDNUMBER) * 64) / 100; // convert from percentage to 64ths
			}
			new_tune.n_head_extend = ix; // number of values
			break;
		case kTUNE_NUCLEUS0:
			NextItem(ctx, tSTRING);
			if ((ix = LookupEnvelopeName(ctx->item_string)) < 0) {
				error(ctx, "Bad envelope name: '%s'", ctx->item_string);
				break;
			}
			new_tune.nucleus0_env = ix;
			new_tune.nucleus0_max = NextItem(ctx, tNUMBER);
			new_tune.nucleus0_min = NextItem(ctx, tNUMBER);
			break;
		case kTUNE_NUCLEUS1:
			NextItem(ctx, tSTRING);
			if ((ix = LookupEnvelopeName(ctx->item_string)) < 0) {
				error(ctx, "Bad envelope name: '%s'", ctx->item_string);
				break;
			}
			new_tune.nucleus1_env = ix;
			new_tune.nucleus1_max = NextItem(ctx, tNUMBER);
			new_tune.nucleus1_min = NextItem(ctx, tNUMBER);
			new_tune.tail_start = NextItem(ctx, tNUMBER);
			new_tune.tail_end = NextItem(ctx, tNUMBER);

			if (!done_split) {
				// also this as the default setting for 'split'
				new_tune.split_nucleus_env = ix;
				new_tune.split_nucleus_max = new_tune.nucleus1_max;
				new_tune.split_nucleus_min = new_tune.nucleus1_min;
				new_tune.split_tail_start = new_tune.tail_start;
				new_tune.split_tail_end = new_tune.tail_end;
			}
			break;
		case kTUNE_SPLIT:
			NextItem(ctx, tSTRING);
			if ((ix = LookupEnvelopeName(ctx->item_string)) < 0) {
				error(ctx, "Bad envelope name: '%s'", ctx->item_string);
				break;
			}
			done_split = true;
			new_tune.split_nucleus_env = ix;
			new_tune.split_nucleus_max = NextItem(ctx, tNUMBER);
			new_tune.split_nucleus_min = NextItem(ctx, tNUMBER);
			new_tune.split_tail_start = NextItem(ctx, tNUMBER);
			new_tune.split_tail_end = NextItem(ctx, tNUMBER);
			NextItem(ctx, tSTRING);
			ctx->item_string[12] = 0;
			for (ix = 0; ix < n_tune_names; ix++) {
				if (strcmp(ctx->item_string, tune_names[ix]) == 0)
					break;
			}

			if (ix == n_tune_names)
				error(ctx, "Tune '%s' not found", ctx->item_string);
			else
				new_tune.split_tune = ix;
			break;
		default:
			error(ctx, "Unexpected: '%s'", ctx->item_string);
			break;
		}
	}

	for (ix = 0; ix < n_preset_tunes; ix++) {
		if (tune_data[ix].name[0] == 0)
			error(ctx, "Tune '%s' not defined", preset_tune_names[ix]);
	}
	fwrite(tune_data, n_tune_names, sizeof(TUNE), f_out);
	free(tune_data);
	fclose(ctx->f_in);
	fclose(f_out);

	fprintf(log, "Compiled %d intonation tunes: %d errors.\n", n_tune_names, ctx->error_count);

	LoadPhData(NULL, NULL);

	int res = ctx->error_count > 0 ? ENS_COMPILE_ERROR : ENS_OK;
	clean_context(ctx);
	return res;
}

#pragma GCC visibility pop

static int CalculateSample(unsigned char c3, int c1) {
	int c2 = c3 << 24;
	c2 = c2 >> 16; // sign extend

	return (c1 & 0xff) + c2;
}
