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
 *   along with this program; if not, see:                                 *
 *               <http://www.gnu.org/licenses/>.                           *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#include "wx/wx.h"
#include "wx/wfstream.h"
#include "wx/dir.h"
#include "wx/filename.h"
#include <wx/numdlg.h>

#include "speak_lib.h"
#include "main.h"
#include "speech.h"
#include "options.h"
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "spect.h"

#ifdef PLATFORM_POSIX
#include <unistd.h>
#endif

extern void FindPhonemesUsed(void);
extern void DisplayErrorFile(const char *fname);
extern int utf8_in(int *c, const char *buf);
extern int utf8_out(unsigned int c, char *buf);
extern void DrawEnvelopes();
extern void ReadPhondataManifest();
char path_source[sizeof(path_home)+20];


typedef struct {   // total 48 bytes
	short frflags;
	unsigned char length;
	unsigned char rms;
	short ffreq[4];   // f0,1,2,3
	unsigned char ffreq2[6];     // nasal(*2)  f4,5,6,7,8(*20)
	unsigned char fheight[9];    // f0-8
	unsigned char fwidth[6];     // width/4  f0-5
	unsigned char fright[4];     // width/4  f0-3
	unsigned char bw[6];         // nasal, f1,2,3,4,5 (*2)
	unsigned char klattp[5];     // amp, tilt, kopen, asp,
} frame_t3;


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
	tPLACE,
	tSTATEMENT,
	tINSTRN1,
	tWHICH_PHONEME,
	tTEST,
};

static keywtab_t k_conditions[] = {
	{"AND",      0,  k_AND},
	{"OR",       0,  k_OR},
	{"THEN",     0,  k_THEN},
	{"NOT",      0,  k_NOT},
	{"prevPh",   tWHICH_PHONEME,  0},
	{"prevPhW",  tWHICH_PHONEME,  5},
	{"thisPh",   tWHICH_PHONEME,  1},
	{"nextPh",   tWHICH_PHONEME,  2},
	{"next2Ph",  tWHICH_PHONEME,  3},
	{"nextPhW",  tWHICH_PHONEME,  4},
	{"next2PhW",  tWHICH_PHONEME, 6},
	{"nextVowel",tWHICH_PHONEME,  7},
	{"prevVowel",tWHICH_PHONEME,  8},
//	{"next2PhW", tWHICH_PHONEME,  0x800},

//	{"numVowels",    tTEST,  0x000},

	{"PreVoicing",   tTEST,  0xf01},
	{"KlattSynth",   tTEST,  0xf02},
	{"MbrolaSynth",  tTEST,  0xf03},
	{NULL, 0, 0}
};



static keywtab_t k_properties[] = {
	{"isPause",      0,  phPAUSE},
	{"isVowel",      0,  phVOWEL},
	{"isNasal",      0,  phNASAL},
	{"isLiquid",     0,  phLIQUID},
	{"isUStop",      0,  phSTOP},
	{"isVFricative", 0,  phVFRICATIVE},

	{"isPalatal",    0,  i_isPalatal},
	{"isRhotic",     0,  i_isRhotic},
	{"isSibilant",   0,  i_isSibilant},
	{"isFlag1",      0,  i_isFlag1},
	{"isFlag2",      0,  i_isFlag2},
	{"isFlag3",      0,  i_isFlag3},

	{"isVel",        0,  i_isVel},

	{"isDiminished", 0,  i_isDiminished},
	{"isUnstressed", 0,  i_isUnstressed},
	{"isNotStressed",0,  i_isNotStressed},
	{"isStressed",   0,  i_isStressed},
	{"isMaxStress",  0,  i_isMaxStress},

	{"isBreak",      0,  i_isBreak},
	{"isPause2",     0,  i_isBreak},  // synonym for isBreak
	{"isWordStart",  0,  i_isWordStart},
	{"notWordStart", 0,  i_notWordStart},
	{"isWordEnd",    0,  i_isWordEnd},
	{"isAfterStress",0,  i_isAfterStress},
	{"isNotVowel",   0,  i_isNotVowel},
	{"isFinalVowel", 0,  i_isFinalVowel},
	{"isVoiced",     0,  i_isVoiced},   // voiced consonant, or vowel
	{"isFirstVowel", 0,  i_isFirstVowel},
	{"isSecondVowel", 0, i_isSecondVowel},
	{"isSeqFlag1",   0,  i_isSeqFlag1},
	{"isTranslationGiven", 0, i_IsTranslationGiven},   // phoneme translation given in **_list or as [[...]]

	{NULL, 0, 0}
};

enum {
	kPHONEMESTART = 1,
	kUTF8_BOM,
	kPROCEDURE,
	kENDPHONEME,
	kENDPROCEDURE,
	kEQUIVALENTS,
	kPHONEMENUMBER,
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

static const char utf8_bom[] = {0xef,0xbb,0xbf,0};

static keywtab_t k_intonation[] = {
	{"tune",      0,   kTUNE},
	{"endtune",   0,   kENDTUNE},
	{"prehead",   0,   kTUNE_PREHEAD},
	{"onset",     0,   kTUNE_ONSET},
	{"head",      0,   kTUNE_HEAD},
	{"headenv",   0,   kTUNE_HEADENV},
	{"headextend", 0,  kTUNE_HEADEXTEND},
	{"headlast",  0,   kTUNE_HEADLAST},
	{"nucleus0",  0,   kTUNE_NUCLEUS0},
	{"nucleus",   0,   kTUNE_NUCLEUS1},
	{"split",     0,   kTUNE_SPLIT},
	{NULL, 0, -1}
};

static keywtab_t keywords[] = {
	{"vowel",     tPHONEME_TYPE, phVOWEL},
	{"liquid",    tPHONEME_TYPE, phLIQUID},
	{"pause",     tPHONEME_TYPE, phPAUSE},
	{"stress",    tPHONEME_TYPE, phSTRESS},
	{"virtual",   tPHONEME_TYPE, phVIRTUAL},

	{"fricative", tPHONEME_TYPE, phFRICATIVE},
	{"vstop",     tPHONEME_TYPE, phVSTOP},
	{"vfricative",tPHONEME_TYPE, phVFRICATIVE},
	{"delete_phoneme", tPHONEME_TYPE, phDELETED},

	// type of consonant
	{"stop",      tPHONEME_TYPE, phSTOP},
	{"frc",       tPHONEME_TYPE, phFRICATIVE},
	{"nasal",     tPHONEME_TYPE, phNASAL},
	{"flp",       tPHONEME_TYPE, phVSTOP},
	{"afr",       tPHONEME_TYPE, phSTOP},      // treat as stop
	{"apr",       tPHONEME_TYPE, phFRICATIVE}, // [h] voiceless approximant


	// keywords
	{"phonemenumber",tSTATEMENT, kPHONEMENUMBER},
	{"phonemetable",tSTATEMENT, kPHONEMETABLE},
	{"include",     tSTATEMENT, kINCLUDE},
	{utf8_bom,      tSTATEMENT, kUTF8_BOM},

	{"phoneme",    tSTATEMENT, kPHONEMESTART},
	{"procedure",  tSTATEMENT, kPROCEDURE},
	{"endphoneme", tSTATEMENT, kENDPHONEME},
	{"endprocedure", tSTATEMENT, kENDPROCEDURE},
	{"equivalents", tSTATEMENT, kEQUIVALENTS},
	{"import_phoneme",  tSTATEMENT, kIMPORT_PH},
	{"stress_type",tSTATEMENT, kSTRESSTYPE},
	{"starttype",  tSTATEMENT, kSTARTTYPE},
	{"endtype",    tSTATEMENT, kENDTYPE},
	{"voicingswitch", tSTATEMENT, kVOICINGSWITCH},


	{"IF",         tSTATEMENT, kIF},
	{"ELSE",       tSTATEMENT, kELSE},
	{"ELIF",       tSTATEMENT, kELIF},
	{"ELSEIF",     tSTATEMENT, kELIF},  // same as ELIF
	{"ENDIF",      tSTATEMENT, kENDIF},
	{"CALL",       tSTATEMENT, kCALLPH},
	{"RETURN",     tSTATEMENT, kRETURN},

	{"PrevVowelEndings", tSTATEMENT, kSWITCH_PREVVOWEL},
	{"NextVowelStarts", tSTATEMENT, kSWITCH_NEXTVOWEL},
	{"EndSwitch",  tSTATEMENT, kENDSWITCH},

	{"Tone",       tSTATEMENT, kTONESPEC},
	{"FMT",        tSTATEMENT, kFMT},
	{"WAV",        tSTATEMENT, kWAV},
	{"VowelStart", tSTATEMENT, kVOWELSTART},
	{"VowelEnding",tSTATEMENT, kVOWELENDING},
	{"addWav",     tSTATEMENT, kANDWAV},


	{"Vowelin",    tSTATEMENT, kVOWELIN},
	{"Vowelout",   tSTATEMENT, kVOWELOUT},
	{"Continue",   tSTATEMENT, kCONTINUE},

	{"ChangePhoneme", tINSTRN1, i_CHANGE_PHONEME},
	{"ReplaceNextPhoneme", tINSTRN1, i_REPLACE_NEXT_PHONEME},
	{"InsertPhoneme", tINSTRN1, i_INSERT_PHONEME},
	{"AppendPhoneme", tINSTRN1, i_APPEND_PHONEME},
	{"IfNextVowelAppend", tINSTRN1, i_APPEND_IFNEXTVOWEL},
	{"ChangeIfDiminished", tINSTRN1, i_CHANGE_IF + 0},
	{"ChangeIfUnstressed", tINSTRN1, i_CHANGE_IF + 1},
	{"ChangeIfNotStressed", tINSTRN1, i_CHANGE_IF + 2},
	{"ChangeIfStressed", tINSTRN1, i_CHANGE_IF + 3},

	{"PauseBefore",tINSTRN1, i_PAUSE_BEFORE},
	{"PauseAfter", tINSTRN1, i_PAUSE_AFTER},
	{"Length",     tINSTRN1, i_SET_LENGTH},
	{"length",     tINSTRN1, i_SET_LENGTH},
	{"LongLength", tINSTRN1, i_LONG_LENGTH},
	{"LengthAdd",  tINSTRN1, i_ADD_LENGTH},
	{"Lengthmod",  tINSTRN1, i_LENGTH_MOD},
	{"lengthmod",  tINSTRN1, i_LENGTH_MOD},
	{"ipa",        tINSTRN1, i_IPA_NAME},

	// flags
	{"wavef",      tPHONEME_FLAG, phWAVE},
	{"unstressed", tPHONEME_FLAG, phUNSTRESSED},
	{"fortis",     tPHONEME_FLAG, phFORTIS},
	{"sibilant",   tPHONEME_FLAG, phSIBILANT},
	{"nolink",     tPHONEME_FLAG, phNOLINK},
	{"trill",      tPHONEME_FLAG, phTRILL},
	{"vowel2",     tPHONEME_FLAG, phVOWEL2},
	{"palatal",    tPHONEME_FLAG, phPALATAL},
	{"long",       tPHONEME_FLAG, phLONG},
	{"brkafter",   tPHONEME_FLAG, phBRKAFTER},
	{"rhotic",     tPHONEME_FLAG, phRHOTIC},
	{"nonsyllabic",tPHONEME_FLAG, phNONSYLLABIC},
	{"lengthenstop",tPHONEME_FLAG, phLENGTHENSTOP},
	{"nopause",    tPHONEME_FLAG, phNOPAUSE},
	{"prevoice",   tPHONEME_FLAG, phPREVOICE},

	{"flag1",      tPHONEME_FLAG, phFLAG1},
	{"flag2",      tPHONEME_FLAG, phFLAG2},
	{"flag3",      tPHONEME_FLAG, phFLAG3},

	// voiced / unvoiced
	{"vcd",	  tPHONEME_FLAG, phVOICED},
	{"vls",	  tPHONEME_FLAG, phFORTIS},

	// place of articulation, set bits 16-19 of phflags
	{"blb",	  tPLACE, 1},
	{"lbd",	  tPLACE, 2},
	{"dnt",	  tPLACE, 3},
	{"alv",	  tPLACE, 4},
	{"rfx",	  tPLACE, 5},
	{"pla",	  tPLACE, 6},
	{"pal",	  tPLACE, 7},
	{"vel",	  tPLACE, 8},
	{"lbv",	  tPLACE, 9},
	{"uvl",	  tPLACE, 10},
	{"phr",	  tPLACE, 11},
	{"glt",	  tPLACE, 12},

	// vowel transition attributes
	{"len=",	  tTRANSITION, 1},
	{"rms=",	  tTRANSITION, 2},
	{"f1=",	  tTRANSITION, 3},
	{"f2=",    tTRANSITION, 4},
	{"f3=",	  tTRANSITION, 5},
	{"brk",    tTRANSITION, 6},
	{"rate",   tTRANSITION, 7},
	{"glstop", tTRANSITION, 8},
	{"lenadd", tTRANSITION, 9},
	{"f4",     tTRANSITION, 10},
	{"gpaus",  tTRANSITION, 11},
	{"colr=",  tTRANSITION, 12},
	{"amp=",   tTRANSITION, 13},  // set rms of 1st frame as fraction of rms of 2nd frame  (1/30ths)

	{NULL, 0, -1}
};


static keywtab_t *keyword_tabs[] = {
keywords, k_conditions, k_properties, k_intonation };


static PHONEME_TAB *phoneme_out;

static int n_phcodes_list[N_PHONEME_TABS];
static PHONEME_TAB_LIST phoneme_tab_list2[N_PHONEME_TABS];
static PHONEME_TAB *phoneme_tab2;
static int phoneme_flags;
static int place_articulation;

static char *p_equivalence;
static char equivalence_buf[20000];


#define N_PROCS 50
int n_procs;
int  proc_addr[N_PROCS];
char proc_names[40][N_PROCS];

#define MAX_PROG_BUF 2000
USHORT *prog_out;
USHORT *prog_out_max;
USHORT prog_buf[MAX_PROG_BUF+20];


static const char *KeyToMnem(keywtab_t *ktab, int type, int value)
{//===============================================================
	while(ktab->mnem != NULL)
	{
		if(ktab->data == value)
		{
			if((type == -1) || (type == ktab->type))
				return(ktab->mnem);
		}
		ktab++;
	}
	return(NULL);
}

static void DecompilePhoneme(FILE *f_out, PHONEME_TAB *ph, int compile_phoneme)
{//============================================================================
	USHORT *pc;
	int instn;
	int instn_category;
	int address, address2;
	int data1;
	int type2;
	int ix;
	int any;
	const char *name;
	char buf[120];

	static const char *INV = "Invalid";

	static const char *instn_category_string[16] = {
		"", "", "IF", "IF OR",
		"", "", "", "",
		"", "", "", "FMT",
		"WAV", "NextVowelStart", "PrevVowelEnd", "+wav" };

	static const char *nextPh_string[6] = {
		"prevPh", "thisPh", "nextPh", "next2Ph", "nextPhW", "**",
	};

	static const char *instn0_string[] = {
		"invalid", "RETURN", "Continue", "DeleteNextPhoneme",
	};

	static const char *instn10_string[] = {
		"", "VowelIn", "VowelOut", "Tone", "",
	};

	static const char *instn_jumps[] = {
		"JMP", INV, INV, INV,
		"JMP false", "SwitchNextVowelType", "SwitchPrevVowelType", INV};

	static char instn1_paramtype[] = {
		0, 3, 3, 3, 3, 3, 3, 1,
		1, 1, 1, 1, 1, 0, 0, 0,
		3, 3, 3, 3, 3, 3, 3, 3,
		0, 0, 0, 0, 0, 0, 0, 0};

return;

	if(compile_phoneme)
	{
		fprintf(f_out,"\nPhoneme %s (%d)\n",WordToString(ph->mnemonic),ph->code);
	}
	else
	{
		fprintf(f_out,"\nProcedure %s\n",proc_names[n_procs]);
	}

	pc = prog_buf;
	while(pc < prog_out)
	{
		instn = *pc++;
		instn_category = (instn >> 12) & 0xf;
		data1 = instn & 0xff;
		type2 = (instn >> 8) & 0xf;
		fprintf(f_out, "  %.3x: %.4x  %s",(unsigned int)(pc-prog_buf),instn,instn_category_string[instn_category]);

		switch(instn_category)
		{
		case 0:
		case 1:
			type2 = instn >> 8;

			if(instn < 0x100)
			{
				if(data1 > 2)
					data1 = 0;
				fprintf(f_out,"%s",instn0_string[data1]);
			}
			else
			if(type2 == i_IPA_NAME)
			{
				for(ix = 0; ix < data1; ix += 2)
				{
					instn = *pc++;
					buf[ix] = instn >> 8;
					buf[ix+1] = instn & 0xff;
				}
				buf[ix] = 0;
				fprintf(f_out,"ipa %s",buf);
			}
			else
			{
				fprintf(f_out,"%s(",KeyToMnem(keywords, tINSTRN1, type2));
				switch(instn1_paramtype[type2])
				{
				case 0:
					fprintf(f_out,"%.4x",instn);
					break;
				case 1:
					fprintf(f_out,"%d",data1);
					break;
				case 3:
					fprintf(f_out,"%s",WordToString(phoneme_tab2[data1].mnemonic));
					break;
				}
				fprintf(f_out,")");
			}
			break;

		case 2:
		case 3:
			if(type2 < 12)
			{
				fprintf(f_out," %s(",nextPh_string[type2 % 6]);
				if(type2 >= 6)
				{
					switch(data1 >> 5)
					{
					case 0:
						name = KeyToMnem(keywords, tPHONEME_TYPE, (data1 & 0x1f));
						if(name != NULL)
							fprintf(f_out,"is%s",name);
						else
							fprintf(f_out,"%d %d",(data1 >> 5),(data1 & 0x1f));
						break;
					case 1:
						fprintf(f_out,"%d %d",(data1 >> 5),(data1 & 0x1f));
						break;
					case 2:
						fprintf(f_out,"%d %d",(data1 >> 5),(data1 & 0x1f));
						break;
					case 4:
						name = KeyToMnem(k_properties, -1, 0x80+(data1 & 0x1f));
						if(name != NULL)
							fprintf(f_out,"%s",name);
						else
							fprintf(f_out,"%d %d",(data1 >> 5),(data1 & 0x1f));
						break;
					default:
						fprintf(f_out,"%d %d",(data1 >> 5),(data1 & 0x1f));
						break;
					}
				}
				else
				{
					fprintf(f_out,"%s",WordToString(phoneme_tab2[data1].mnemonic));
				}
			}
			else
			if(type2 == 8)
			{
				// list of numbers
				fprintf(f_out," StressLevel(");
				any = 0;
				for(ix=0; ix<8; ix++)
				{
					if(data1 & (1 << ix))
					{
						if(any)
							fputc(',',f_out);
						any = 1;
						fprintf(f_out,"%d",ix);
					}
				}
			}
			else
			{
				name = KeyToMnem(k_conditions, tTEST, instn & 0xfff);
				fprintf(f_out,"%s(",name);
			}
			fprintf(f_out,")");
			break;

		case 6:
			fprintf(f_out,"%s",instn_jumps[(instn >> 9) & 7]);
			fprintf(f_out," %d",instn & 0x1ff);
			break;

		case 9:
			address = ((data1 & 0xf) << 4) + *pc++;
			fprintf(f_out,"CALL %.5x",address);
			break;

		case 10:
			fprintf(f_out,"%s",instn10_string[type2]);
			switch(type2)
			{
			case 1:
			case 2:
				address = (data1 << 16) + pc[0];
				address2 = (pc[1] << 16) + pc[2];
				pc += 3;
				fprintf(f_out, " %.6x %.8x",address,address2);
				break;
			case 3:
				address = ((instn & 0xf) << 16) + *pc++;
				address2 = ((instn & 0xf0) << 12) + *pc++;
				fprintf(f_out, " %.5x %.5x",address,address2);
				break;
			}
			break;

		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			address = ((instn & 0xf) << 16) + *pc++;
			fprintf(f_out, " %d %.5x",(instn >> 4) & 0xff,address*4);
			break;
		}
		fprintf(f_out,"\n");
	}
}





static int n_phoneme_tabs;
static int n_phcodes;

// outout files
static FILE *f_phdata;
static FILE *f_phindex;
static FILE *f_phtab;
static FILE *f_phcontents;
static FILE *f_errors = stderr;
static FILE *f_prog_log = NULL;
static FILE *f_report;


static FILE *f_in;
static int f_in_linenum;
static int f_in_displ;


static int linenum;
static int count_references = 0;
static int duplicate_references = 0;
static int count_frames = 0;
static int error_count = 0;
static int resample_count = 0;
static int resample_fails = 0;
static int then_count = 0;
static int after_if = 0;

static char current_fname[80];

static int markers_used[8];

typedef struct {
	void *link;
	int value;
	int ph_mnemonic;
	short ph_table;
	char string[1];
} REF_HASH_TAB;

static REF_HASH_TAB *ref_hash_tab[256];


#define N_ENVELOPES  30
int n_envelopes = 0;
char envelope_paths[N_ENVELOPES][80];
unsigned char envelope_dat[N_ENVELOPES][ENV_LEN];


typedef struct {
	FILE *file;
	int  linenum;
	char fname[80];
} STACK;

#define N_STACK  12
int stack_ix;
STACK stack[N_STACK];

#define N_IF_STACK 12
int if_level;
typedef struct {
	USHORT *p_then;
	USHORT *p_else;
	int  returned;
} IF_STACK;
IF_STACK if_stack[N_IF_STACK];


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

int item_type;
int item_terminator;
#define N_ITEM_STRING 256
char item_string[N_ITEM_STRING];



static int ref_sorter(char **a, char **b)
{//======================================
	int ix;

	REF_HASH_TAB *p1 = (REF_HASH_TAB *)(*a);
	REF_HASH_TAB *p2 = (REF_HASH_TAB *)(*b);

	ix = strcoll(p1->string,p2->string);
	if(ix != 0)
		return ix;

	ix = p1->ph_table - p2->ph_table;
	if(ix != 0)
		return ix;

	return(p1->ph_mnemonic - p2->ph_mnemonic);
}   /* end of ref_sorter */



static void CompileReport(void)
{//============================
	int ix;
	int hash;
	int n;
	REF_HASH_TAB *p;
	REF_HASH_TAB **list;
	const char *data_path;
	int prev_table;
	int prev_mnemonic;

    if(f_report == NULL)
        return;

	// make a list of all the references and sort it
	list = (REF_HASH_TAB **)malloc((count_references)* sizeof(REF_HASH_TAB *));
	if(list == NULL)
		return;

	fprintf(f_report,"\n%d phoneme tables\n",n_phoneme_tabs);
    fprintf(f_report,"          new total\n");
	for(ix=0; ix<n_phoneme_tabs; ix++)
	{
		fprintf(f_report,"%8s %3d %4d\n",phoneme_tab_list2[ix].name, phoneme_tab_list2[ix].n_phonemes, n_phcodes_list[ix]+1);
	}
	fputc('\n',f_report);

	fprintf(f_report,"Data file      Used by\n");
	ix = 0;
	for(hash=0; (hash < 256) && (ix < count_references); hash++)
	{
		p = ref_hash_tab[hash];
		while(p != NULL)
		{
			list[ix++] = p;
			p = (REF_HASH_TAB *)(p->link);
		}
	}
	n = ix;
	qsort((void *)list,n,sizeof(REF_HASH_TAB *),(int (*)(const void *,const void *))ref_sorter);

	data_path = "";
	prev_mnemonic = 0;
	prev_table = 0;
	for(ix=0; ix<n; ix++)
	{
		int j = 0;

		if(strcmp(list[ix]->string, data_path) != 0)
		{
			data_path = list[ix]->string;
			j = strlen(data_path);
			fprintf(f_report,"%s",data_path);
		}
		else
		{
			if((list[ix]->ph_table == prev_table) && (list[ix]->ph_mnemonic == prev_mnemonic))
				continue;   // same phoneme, don't list twice
		}

		while(j < 14)
		{
			fputc(' ',f_report);  // pad filename with spaces
			j++;
		}

		fprintf(f_report,"  [%s] %s",WordToString(prev_mnemonic = list[ix]->ph_mnemonic), phoneme_tab_list2[prev_table = list[ix]->ph_table].name);
		fputc('\n',f_report);
	}

	for(ix=0; ix<n; ix++)
	{
		free(list[ix]);
		list[ix] = NULL;
	}

	free(list);
	list = NULL;
	fclose(f_report);
}  // end of CompileReport


#ifdef PLATFORM_WINDOWS
int strcasecmp(const char *s1, const char *s2)
{
	int ix=0;
	int diff;

	for(;;)
	{
		if((diff = (tolower(s1[ix]) - tolower(s2[ix]))) != 0)
			return(diff);

		if((s1[ix] == 0) || (s2[ix] == 0))
			return(diff);

		ix++;
	}
}
#endif


static int ph_sorter(char **a, char **b)
{//======================================
	int ix;
	int t1, t2;
	char mnem1[6];

	PHONEME_TAB *p1 = (PHONEME_TAB *)(*a);
	PHONEME_TAB *p2 = (PHONEME_TAB *)(*b);

	t1 = p1->type;
	if(t1 > phVOWEL) t1 = phVOWEL+1;

	t2 = p2->type;
	if(t2 > phVOWEL) t2 = phVOWEL+1;

	if((ix = t1 - t2) != 0)
		return(ix);

	strcpy(mnem1,WordToString(p1->mnemonic));
  return(strcasecmp(mnem1,WordToString(p2->mnemonic)));
}   /* end of ph_sorter */



static void PrintPhonemesUsed(FILE *f, const char *dsource, const char *dictname)
{//==============================================================================
	int ix;
	PHONEME_TAB *ph;
	PHONEME_TAB *ph_tab[N_PHONEME_TAB];
	int count = 0;
	int n_ph = 0;
	int section = 0;
	time_t mod_time;
	char fname[sizeof(path_home)+45];
	struct stat statbuf;
	char time_string[20];
	const char *files[] = {"rules","list","listx"};

	// find the date-stamp of the dictionary source files
	mod_time = 0;
	for(ix=0; ix<3; ix++)
	{
		sprintf(fname,"%s%s_%s",dsource, dictname, files[ix]);
		if(stat(fname,&statbuf) == 0)
		{
			if(statbuf.st_mtime > mod_time)
				mod_time = statbuf.st_mtime;
		}
	}

	if(mod_time > 0)
	{
		strftime(time_string, sizeof(time_string), "%F", localtime(&mod_time));
		fprintf(f,"\n\nDictionary %s_dict  %s\n",dictname, time_string);
	}
	else
	{
		fprintf(f,"\n\nDictionary %s_dict\n",dictname);
	}
	fflush(f);

	for(ix=0; (ix<N_PHONEME_TAB) && (phoneme_tab[ix] != NULL); ix++)
	{
		if(phoneme_tab_flags[ix] & 2)
		{
			ph_tab[n_ph++] = phoneme_tab[ix];
		}
	}

	qsort((void *)ph_tab,n_ph,sizeof(PHONEME_TAB *),(int (*)(const void *,const void *))ph_sorter);

	for(ix=0; ix<n_ph; ix++)
	{
		ph = ph_tab[ix];

		if(ph->type > 1)
		{
			if((ph->type > phVOWEL) && (section == 0))
			{
				section = 1;
				count = 0;
				fputc('\n',f);
			}
			if((count & 0x7) == 0)
				fputc('\n',f);
			fprintf(f,"%-4s ",WordToString(ph->mnemonic));
			count++;
		}
	}
	fputc('\n',f);
}  // end of  PrintPhonemesUsed



static wxString CompileAllDictionaries()
{//=====================================
	wxString filename;
	wxFileName fname;
	wxString dictstr;
	wxString filetype;
	wxString report = _T("");
	int err;
	int errors = 0;
	int dict_count = 0;
	FILE *f_in;
	FILE *log;
	FILE *f_phused;
	char dictname[80];
	char fname_log[sizeof(path_dsource)+20];
	char save_voice_name[80];
	char path[sizeof(path_home)+40];       // path_dsource+20
	char buf[200];
	char voicename[80];

	if(!wxDirExists(path_dictsource))
	{
		if(gui_flag)
		{
			wxString dirname = wxDirSelector(_T("Directory of dictionary files"),path_phsource);
			if(!dirname.IsEmpty())
			{
				path_dictsource = dirname;
				strncpy0(path_dsource,path_dictsource.mb_str(wxConvLocal),sizeof(path_dsource)-1);
				strcat(path_dsource,"/");
			}
		}
		else
		{
			fprintf(stderr,"Can't find dictionary files: %s\n",path_dsource);
		}
	}

	wxDir dir(path_dictsource);
	if(!dir.IsOpened())
	{
		return(_T(" No dictionaries"));
	}

	strcpy(save_voice_name,voice_name2);

	sprintf(fname_log,"%s%s",path_dsource,"dict_log");
	log = fopen(fname_log,"w");
	sprintf(fname_log,"%s%s",path_dsource,"dict_phonemes");
	f_phused = fopen(fname_log,"w");

	if(f_phused)
	{
		fprintf(f_phused,"Phonemes which are used in the *_rules and *_list files\n");
	}

	bool cont = dir.GetFirst(&filename, _T("*_rules*"), wxDIR_FILES);
	while ( cont )
	{
		fname = wxFileName(filename);
		filetype = fname.GetName().AfterLast('_');
		if((filetype != _T("rules")) && (filetype != _T("rules.txt")))
		{
			cont = dir.GetNext(&filename);
			continue;
		}

		dictstr = fname.GetName().BeforeLast('_');
		strcpy(dictname,dictstr.mb_str(wxConvLocal));

		dict_count++;
		strcpy(voicename,dictname);

		// read the *_rules file to see if a phoneme table is specified though a voice name
		sprintf(path,"%s%s_rules.txt",path_dsource,dictname);
		if((f_in = fopen(path,"r")) == NULL)
		{
			sprintf(path,"%s%s_rules",path_dsource,dictname);
			f_in = fopen(path,"r");
		}

		if(f_in != NULL)
		{
			unsigned int ix;
			unsigned int c;

			for(ix=0; ix<20; ix++)
			{
				if(fgets(buf,sizeof(buf),f_in) == NULL)
					break;

				if(memcmp(buf,"//voice=",8)==0)
				{
					for(ix=0; ix<sizeof(voicename); ix++)
					{
						if(isspace(c = buf[ix+8]))
						{
							break;
						}
						voicename[ix] = c;
					}
					voicename[ix] = 0;
					break;
				}
			}
			fclose(f_in);
		}

		LoadVoice(voicename,0);

		if((err = CompileDictionary(path_dsource, dictname,log,NULL,0)) > 0)
		{
			report = report + dictstr + wxString::Format(_T(" %d, "),err);
			errors += err;
		}

		if(f_phused != NULL)
		{
			memset(phoneme_tab_flags,0,sizeof(phoneme_tab_flags));
			FindPhonemesUsed();
			PrintPhonemesUsed(f_phused, path_dsource, dictname);
		}

		cont = dir.GetNext(&filename);
	}
	if(log != NULL)
		fclose(log);
	if(f_phused != NULL)
		fclose(f_phused);

	LoadVoiceVariant(save_voice_name,0);

	if(errors == 0)
		return(wxString::Format(_T(" Compiled %d dictionaries"),dict_count));
	else
	{
		return(_T(" Dictionary errors: ") + report);
	}

}  // end of CompileAllDictionaries




static void error(const char *format, const char *string)
{//======================================================
	if(string==NULL)
		string = "";
	fprintf(f_errors,"%4d:  ",linenum-1);
	fprintf(f_errors,format,string);
	fprintf(f_errors,"\n");
	error_count++;
}

static void Error(const char *string)
{//==================================
	error("%s",string);
}

static FILE *fopen_log(FILE *f_log, const char *fname,const char *access)
{//=====================================================================
// performs fopen, but produces error message to f_log if it fails
	FILE *f;

	if((f = fopen(fname,access)) == NULL)
	{
		if(f_log != NULL)
			fprintf(f_log,"Can't access (%s) file '%s'\n",access,fname);
	}
	return(f);
}


static unsigned int StringToWord(const char *string)
{//=================================================
// Pack 4 characters into a word
	int  ix;
	unsigned char c;
	unsigned int word;

	word = 0;
	for(ix=0; ix<4; ix++)
	{
		if(string[ix]==0) break;
		c = string[ix];
		word |= (c << (ix*8));
	}
	return(word);
}


static MNEM_TAB reserved_phonemes[] = {
	{"_\001",  phonCONTROL},      // NOT USED
	{"%",  phonSTRESS_U},
	{"%%", phonSTRESS_D},
	{",",  phonSTRESS_2},
	{",,", phonSTRESS_3},
	{"'",  phonSTRESS_P},
	{"''", phonSTRESS_P2},
	{"=",  phonSTRESS_PREV},  //  stress previous syllable
	{"_:", phonPAUSE},        //  pause
	{"_",  phonPAUSE_SHORT},  //  short pause
	{"_!", phonPAUSE_NOLINK}, //  short pause, no link
	{":",  phonLENGTHEN},
	{"@",  phonSCHWA},
	{"@-", phonSCHWA_SHORT},
	{"||", phonEND_WORD},
	{"1",  phonDEFAULTTONE},  // (numeral 1)  default tone (for tone language)
	{"#X1",phonCAPITAL},      // capital letter indication
	{"?",  phonGLOTTALSTOP},  // glottal stop
	{"-",  phonSYLLABIC},     // syllabic consonant
	{"_^_",phonSWITCH},       //  Change language
	{"_X1",phonX1},           // a language specific action
	{"_|", phonPAUSE_VSHORT}, // very short pause
	{"_::",phonPAUSE_LONG},   // long pause
	{"t#", phonT_REDUCED},    // reduced version of [t]
	{"'!", phonSTRESS_TONIC}, // stress - emphasized
	{"_;_",phonPAUSE_CLAUSE}, // clause pause

	{"#@", phonVOWELTYPES},   // vowel type groups, these must be consecutive
	{"#a", phonVOWELTYPES+1},
	{"#e", phonVOWELTYPES+2},
	{"#i", phonVOWELTYPES+3},
	{"#o", phonVOWELTYPES+4},
	{"#u", phonVOWELTYPES+5},
	{NULL, 0} };


static void ReservePhCodes()
{//=========================
// Reserve phoneme codes which have fixed numbers so that they can be
// referred to from the program code.
	unsigned int word;
	MNEM_TAB *p;

	p = reserved_phonemes;
	while(p->mnem != NULL)
	{
		word = StringToWord(p->mnem);
		phoneme_tab2[p->value].mnemonic = word;
		phoneme_tab2[p->value].code = p->value;
		if(n_phcodes <= p->value)
			n_phcodes = p->value+1;
		p++;
	}
}  // end of ReservePhCodes


static int LookupPhoneme(const char *string, int control)
{//======================================================
// control = 0   explicit declaration
// control = 1   declare phoneme if not found
// control = 2   start looking after control & stress phonemes

	int  ix;
	int  start;
	int  use;
	unsigned int word;

	if(strcmp(string,"NULL")==0)
		return(1);

	ix = strlen(string);
	if((ix==0) || (ix> 4))
	{
		error("Bad phoneme name '%s'",string);
	}
	word = StringToWord(string);

	// don't use phoneme number 0, reserved for string terminator
	start = 1;

	if(control==2)
		start = 8;   // don't look for control and stress phonemes (allows these characters to be
		             // used for other purposes)

	use = 0;
	for(ix=start; ix<n_phcodes; ix++)
	{
		if(phoneme_tab2[ix].mnemonic == word)
			return(ix);

		if((use==0) && (phoneme_tab2[ix].mnemonic == 0))
		{
			use = ix;
		}
	}

	if(use == 0)
	{
		if(control == 0)
			return(-1);
		if(n_phcodes >= N_PHONEME_TAB-1)
			return(-1);   // phoneme table is full
		use = n_phcodes++;
	}

	// add this phoneme to the phoneme table
	phoneme_tab2[use].mnemonic = word;
	phoneme_tab2[use].type = phINVALID;
	phoneme_tab2[use].program = linenum;  // for error report if the phoneme remains undeclared
	return(use);
}  //  end of Compile::LookupPhoneme



static unsigned int get_char()
{//===========================
	unsigned int c;
	c = fgetc(f_in);
	if(c == '\n')
		linenum++;
	return(c);
}

static void unget_char(unsigned int c)
{//===================================
	ungetc(c,f_in);
	if(c == '\n')
		linenum--;
}


int CheckNextChar()
{//================
	int c;
	while(((c = get_char()) == ' ') || (c == '\t'));
	unget_char(c);
	return(c);
}  // end of CheckNextChar


static int NextItem(int type)
{//==========================
	int  acc;
	unsigned char  c=0;
	unsigned char c2;
	int  ix;
	int  sign;
	char *p;
	keywtab_t *pk;

	item_type = -1;

	f_in_displ = ftell(f_in);
	f_in_linenum = linenum;

	while(!feof(f_in))
	{
		c = get_char();
		if(c=='/')
		{
			if((c2 = get_char())=='/')
			{
				// comment, ignore to end of line
				while(!feof(f_in) && ((c = get_char()) != '\n'));
			}
			else
			{
				unget_char(c2);
			}
		}
		if(!isspace(c))
		{
			break;
		}
	}
	if(feof(f_in))
		return(-2);

	if(c == '(')
	{
		if(type == tOPENBRACKET)
			return(1);
		return(-1);
	}

	ix = 0;
	while(!feof(f_in) && !isspace(c) && (c != '(') && (c != ')') && (c != ','))
	{
		if(c == '\\')
			c = get_char();
		item_string[ix++] = c;
		c = get_char();
		if(feof(f_in))
			break;
		if(item_string[ix-1] == '=')
			break;
	}
	item_string[ix] = 0;

	while(isspace(c))
	{
		c = get_char();
	}

	item_terminator = ' ';
	if((c == ')') || (c == '(') || (c == ','))
		item_terminator = c;

	if((c == ')') || (c == ','))
		c = ' ';

	if(!feof(f_in))
		unget_char(c);

	if(type == tSTRING)
	{
		return(0);
	}

	if((type == tNUMBER) || (type == tSIGNEDNUMBER))
	{
		acc = 0;
		sign = 1;
		p = item_string;

		if((*p == '-') && (type == tSIGNEDNUMBER))
		{
			sign = -1;
			p++;
		}
		if(!isdigit(*p))
		{
			if((type == tNUMBER) && (*p == '-'))
				error("Expected an unsigned number",NULL);
			else
				error("Expected a number",NULL);
		}
		while(isdigit(*p))
		{
			acc *= 10;
			acc += (*p - '0');
			p++;
		}
		return(acc * sign);
	}


	if((type >= tKEYWORD) && (type <= tINTONATION))
	{
		pk = keyword_tabs[type-tKEYWORD];
		while(pk->mnem != NULL)
		{
			if(strcmp(item_string,pk->mnem)==0)
			{
				item_type = pk->type;
				return(pk->data);
			}
			pk++;
		}
		item_type = -1;
		return(-1);   // keyword not found
	}
	if(type == tPHONEMEMNEM)
	{
		return(LookupPhoneme(item_string,2));
	}
	return(-1);
}  //  end of NextItem



static int NextItemMax(int max)
{//==============================
// Get a number, but restrict value to max
    int value;
    char msg[80];

    value = NextItem(tNUMBER);
    if(value > max)
    {
        sprintf(msg, "Value %d is greater than maximum %d", value, max);
        error(msg, NULL);
        value = max;
    }
    return(value);
}



static int NextItemBrackets(int type, int control)
{//===============================================
// Expect a parameter inside parantheses
// control: bit 0  0= need (
//          bit 1  1= allow comma

	int value;

	if((control & 1) == 0)
	{
		if(!NextItem(tOPENBRACKET))
		{
			error("Expected '('", NULL);
		}
	}

	value = NextItem(type);
	if((control & 2) && (item_terminator == ','))
		return(value);

	if(item_terminator != ')')
	{
		error("Expected ')'", NULL);
	}
	return(value);

}


static void UngetItem()
{//====================
	fseek(f_in,f_in_displ,SEEK_SET);
	linenum = f_in_linenum;
}  //  end of UngetItem




static int Range(int value, int divide, int min, int max)
{//======================================================
	if(value < 0)
		value -= divide/2;
	else
		value += divide/2;
	value = value / divide;

	if(value > max)
		value = max;
	if(value < min)
		value = min;
	return(value - min);
}


int CompileVowelTransition(int which)
{//==================================
// Compile a vowel transition
	int key;
	int len=0;
	int rms=0;
	int f1=0;
	int f2=0;
	int f2_min=0;
	int f2_max=0;
	int f3_adj=0;
	int f3_amp=0;
	int flags=0;
	int vcolour=0;
	int x;
	int instn = i_VOWELIN;
	int word1;
	int word2;

	if(which==1)
	{
//		instn = i_VOWELIN;
		len = 50 / 2;  // defaults for transition into vowel
		rms = 25 / 2;

		if(phoneme_out->type == phSTOP)
		{
			len = 42 / 2;  // defaults for transition into vowel
			rms = 30 / 2;
		}
	}
	else
	if(which==2)
	{
		instn = i_VOWELOUT;
		len = 36 / 2;  // defaults for transition out of vowel
		rms = 16 / 2;
	}

	for(;;)
	{
		key = NextItem(tKEYWORD);
		if(item_type != tTRANSITION)
		{
			UngetItem();
			break;
		}

		switch(key & 0xf)
		{
		case 1:
			len = Range(NextItem(tNUMBER), 2, 0, 63) & 0x3f;
			flags |= 1;
			break;
		case 2:
			rms = Range(NextItem(tNUMBER), 2, 0, 31) & 0x1f;
			flags |= 1;
			break;
		case 3:
			f1 = NextItem(tNUMBER);
			break;
		case 4:
			f2 = Range(NextItem(tNUMBER), 50, 0, 63) & 0x3f;
			f2_min = Range(NextItem(tSIGNEDNUMBER), 50, -15, 15) & 0x1f;
			f2_max = Range(NextItem(tSIGNEDNUMBER), 50, -15, 15) & 0x1f;
			if(f2_min > f2_max)
			{
				x = f2_min;
				f2_min = f2_max;
				f2_max = x;
			}
			break;
		case 5:
			f3_adj = Range(NextItem(tSIGNEDNUMBER), 50, -15, 15) & 0x1f;
			f3_amp = Range(NextItem(tNUMBER), 8, 0, 15) & 0x1f;
			break;
		case 6:
			flags |= 2;   // break
			break;
		case 7:
			flags |= 4;   // rate
			break;
		case 8:
			flags |= 8;   // glstop
			break;
		case 9:
			flags |= 16;  // lenadd
			break;
		case 10:
			flags |= 32;  // f4
			break;
		case 11:
			flags |= 64;  // paus
			break;
		case 12:
			vcolour = NextItem(tNUMBER);
			break;
		case 13:
			// set rms of 1st frame as fraction of rms of 2nd frame  (1/30ths)
			rms = (Range(NextItem(tNUMBER), 1, 0, 31) & 0x1f) | 0x20;
			flags |= 1;
			break;
		}
	}
	word1 = len + (rms << 6) + (flags << 12);
	word2 =  f2 + (f2_min << 6) + (f2_max << 11) + (f3_adj << 16) + (f3_amp << 21) + (f1 << 26) + (vcolour << 29);
	prog_out[0] = instn + ((word1 >> 16) & 0xff);
	prog_out[1] = word1;
	prog_out[2] = word2 >> 16;
	prog_out[3] = word2;
	prog_out += 4;

	return(0);
}  // end of VowelTransition



int LoadSpect(const char *path, int control)
{//=========================================
	SpectSeq *spectseq;
	int peak;
	int displ;
	int frame;
	int n_frames;
	int ix;
	int x, x2;
	int rms;
	float total;
	float pkheight;
	int marker1_set=0;
	int frame_vowelbreak=0;
	int klatt_flag=0;
	SpectFrame *fr;
	frame_t *fr_out;
	wxString path_sep = _T("/");

	SPECT_SEQ seq_out;
	SPECT_SEQK seqk_out;

	// create SpectSeq and import data
	spectseq = new SpectSeq;
	if(spectseq == NULL)
	{
		Error("Failed to create SpectSeq");
		return(0);
	}

	wxString filename = path_phsource + path_sep + wxString(path,wxConvLocal);
	wxFileInputStream stream(filename);

	if(stream.Ok() == FALSE)
	{
		error("Failed to open: '%s'",path);
		return(0);
	}
	spectseq->Load(stream);

	// do we need additional klatt data ?
	for(frame=0; frame < spectseq->numframes; frame++)
	{
		for(ix=5; ix<N_KLATTP2; ix++)
		{
			if(spectseq->frames[frame]->klatt_param[ix] != 0)
				klatt_flag = FRFLAG_KLATT;
		}
	}

	displ = ftell(f_phdata);

	seq_out.n_frames=0;
	seq_out.sqflags=0;
	seq_out.length_total=0;

	total = 0;
	for(frame=0; frame < spectseq->numframes; frame++)
	{

#ifdef deleted
for(ix=0; ix<8; ix++)
{
	// find which files have certain markers set
	if(spectseq->frames[frame]->markers & (1<<ix))
	{
		markers_used[ix]++;
		if((ix==3) || (ix==4))
		{
			fprintf(f_errors,"Marker %d: %s\n",ix,path);
		}
	}
}
#endif

		if(spectseq->frames[frame]->keyframe)
		{
			if(seq_out.n_frames == 1)
			{
				frame_vowelbreak = frame;
			}
			if(spectseq->frames[frame]->markers & 0x2)
			{
				// marker 1 is set
				marker1_set = 1;
			}

			seq_out.n_frames++;
			if(frame > 0)
				total += spectseq->frames[frame-1]->length;
		}
	}
	seq_out.length_total = int(total);

	if((control & 1) && (marker1_set == 0))
	{
		// This is a vowel, but no Vowel Break marker is set
		// set a marker flag for the second frame of a vowel
		spectseq->frames[frame_vowelbreak]->markers |= FRFLAG_VOWEL_CENTRE;
	}

	n_frames = 0;
	for(frame=0; frame < spectseq->numframes; frame++)
	{
		fr = spectseq->frames[frame];

		if(fr->keyframe)
		{
			if(klatt_flag)
				fr_out = &seqk_out.frame[n_frames];
			else
				fr_out = (frame_t *)&seq_out.frame[n_frames];

			x = int(fr->length + 0.5);  // round to nearest mS
			if(x > 255) x = 255;
			fr_out->length = x;

			fr_out->frflags = fr->markers | klatt_flag;

			rms = int(fr->GetRms(spectseq->amplitude));
			if(rms > 255) rms = 255;
			fr_out->rms = rms;

			if(n_frames == (seq_out.n_frames-1))
				fr_out->length = 0;    // give last frame zero length

			// write: peak data
			count_frames++;
			for(peak=0; peak < 8; peak++)
			{
				if(peak < 7)
					fr_out->ffreq[peak] = fr->peaks[peak].pkfreq;

				pkheight = spectseq->amplitude * fr->amp_adjust * fr->peaks[peak].pkheight;
				pkheight = pkheight/640000;
				if(pkheight > 255) pkheight = 255;
				fr_out->fheight[peak] = int(pkheight);

				if(peak < 6)
				{
					x =  fr->peaks[peak].pkwidth/4;
					if(x > 255) x = 255;
					fr_out->fwidth[peak] = x;

					if(peak < 3)
					{
						x2 =  fr->peaks[peak].pkright/4;
						if(x2 > 255) x2 = 255;
						fr_out->fright[peak] = x2;
					}
				}

				if(peak < 4)
				{
					x = fr->peaks[peak].klt_bw / 2;
					if(x > 255) x = 255;
					fr_out->bw[peak] = x;
				}
			}

			for(ix=0; ix<5; ix++)
			{
				fr_out->klattp[ix] = fr->klatt_param[ix];

				fr_out->klattp[KLATT_FNZ] = fr->klatt_param[KLATT_FNZ] / 2;
			}

			if(klatt_flag)
			{
				// additional klatt parameters
				for(ix=0; ix<5; ix++)
				{
					fr_out->klattp2[ix] = fr->klatt_param[ix+5];
				}

				for(peak=0; peak<7; peak++)
				{
					fr_out->klatt_ap[ix] = fr->peaks[peak].klt_ap;

					x = fr->peaks[peak].klt_bp / 2;
					if(x > 255) x = 255;
					fr_out->klatt_bp[ix] = x;
				}
			}

			if(fr_out->bw[1] == 0)
			{
				fr_out->bw[0] = 89 / 2;
				fr_out->bw[1] = 90 / 2;
				fr_out->bw[2] = 140 / 2;
				fr_out->bw[3] = 260 / 2;
			}

			n_frames++;
		}
	}

	if(klatt_flag)
	{
		seqk_out.n_frames = seq_out.n_frames;
		seqk_out.sqflags = seq_out.sqflags;
		seqk_out.length_total = seq_out.length_total;

		ix = (char *)(&seqk_out.frame[seqk_out.n_frames]) - (char *)(&seqk_out);
		ix = (ix+3) & 0xfffc;   // round up to multiple of 4 bytes
		fwrite(&seqk_out,ix,1,f_phdata);
	}
	else
	{
		ix = (char *)(&seq_out.frame[seq_out.n_frames]) - (char *)(&seq_out);
		ix = (ix+3) & 0xfffc;   // round up to multiple of 4 bytes
		fwrite(&seq_out,ix,1,f_phdata);
	}

	delete spectseq;
	return(displ);
}  //  end of LoadSpect



static int LoadWavefile(FILE *f, const char *fname)
{//================================================
	int displ;
	unsigned char c1;
	unsigned char c3;
	int c2;
	int sample;
	int sample2;
	float x;
	int max = 0;
	int length;
	int sr1, sr2;
	int failed;
	int len;
	int resample_wav = 0;
	const char *fname2;
	char fname_temp[100];
	char msg[120];
	int scale_factor=0;

	fseek(f,24,SEEK_SET);
	sr1 = Read4Bytes(f);
	sr2 = Read4Bytes(f);
	fseek(f,40,SEEK_SET);

	if((sr1 != samplerate_native) || (sr2 != sr1*2))
	{
        int fd_temp;
        char command[sizeof(path_source)+200];

	    failed = 0;

#ifdef PLATFORM_POSIX
		strcpy(fname_temp,"/tmp/espeakXXXXXX");
		if((fd_temp = mkstemp(fname_temp)) >= 0)
		{
			close(fd_temp);
		}
#else
		strcpy(fname_temp,tmpnam(NULL));
#endif

        fname2 = fname;
        len = strlen(fname);
        if(strcmp(&fname[len-4], ".wav") == 0)
        {
            strcpy(msg, fname);
            msg[len-4] = 0;
            fname2 = msg;
        }

        sprintf(command,"sox \"%s%s.wav\" -r %d -c1 -t wav %s\n",path_source,fname2,samplerate_native, fname_temp);
        if(system(command) < 0)
        {
            failed = 1;
        }


        if(failed || (GetFileLength(fname_temp) <= 0))
        {
            if(resample_fails < 2)
                error("Resample command failed: %s", command);
            resample_fails++;

            if(sr1 != samplerate_native)
            {
                sprintf(msg, "Can't resample (%d to %d): %s", sr1, samplerate_native, fname);
                error("%s", msg);
            }
            else
            {
                error("WAV file is not mono: %s", fname);
            }
            remove(fname_temp);
            return(0);
        }

        f = fopen(fname_temp,"rb");
        if(f == NULL)
        {
            error("Can't read temp file: %s",fname_temp);
            return(0);
        }
        if(f_report != NULL)
            fprintf(f_report, "resampled  %s\n", fname);
        resample_count++;
        resample_wav = 1;
        fseek(f,40,SEEK_SET);  // skip past the WAV header, up to before "data length"
	}

	displ = ftell(f_phdata);

	// data contains:  4 bytes of length (n_samples * 2), followed by 2-byte samples (lsb byte first)
	length = Read4Bytes(f);

	while(!feof(f))
	{
		c1 = fgetc(f);
		c3 = fgetc(f);
		if(feof(f)) break;

		c2 = c3 << 24;
		c2 = c2 >> 16;  // sign extend

		sample = (c1 & 0xff) + c2;

		if(sample > max)
			max = sample;
		else
		if(sample < -max)
			max = -sample;

	}

	scale_factor = (max / 127) + 1;

//fprintf(f_errors," sample len=%d max=%4x shift=%d\n",length,max,scale_factor);

#define MIN_FACTOR   -1  // was 6,  disable use of 16 bit samples
	if(scale_factor > MIN_FACTOR)
	{
		length = length/2 + (scale_factor << 16);
	}

	Write4Bytes(f_phdata,length);
//	fwrite(&length,4,1,f_phdata);
	fseek(f,44,SEEK_SET);

	while(!feof(f))
	{
		c1 = fgetc(f);
		c3 = fgetc(f);
		c2 = c3 << 24;
		c2 = c2 >> 16;  // sign extend

		sample = (c1 & 0xff) + c2;

		if(feof(f)) break;

      if(scale_factor <= MIN_FACTOR)
		{
			fputc(sample & 0xff,f_phdata);
			fputc(sample >> 8,f_phdata);
		}
		else
		{
			x = (float(sample) / scale_factor) + 0.5;
			sample2= int(x);
			if(sample2 > 127)
				sample2 = 127;
			if(sample2 < -128)
				sample2 = -128;
			fputc(sample2,f_phdata);
		}
	}

	length = ftell(f_phdata);
	while((length & 3) != 0)
	{
		// pad to a multiple of 4 bytes
		fputc(0,f_phdata);
		length++;
	}

	if(resample_wav != 0)
	{
		fclose(f);
		remove(fname_temp);
	}
	return(displ | 0x800000);  // set bit 23 to indicate a wave file rather than a spectrum
}  //  end of LoadWavefile



static int LoadEnvelope(FILE *f, const char *fname)
{//================================================
	int displ;
	char buf[128];

	displ = ftell(f_phdata);

	fseek(f,12,SEEK_SET);
	if(fread(buf,128,1,f) == 0)
	{
		error("Failed to read envelope: %s",fname);
	}
	fwrite(buf,128,1,f_phdata);

	if(n_envelopes < N_ENVELOPES)
	{
		strncpy0(envelope_paths[n_envelopes],fname,sizeof(envelope_paths[0]));
		memcpy(envelope_dat[n_envelopes],buf,sizeof(envelope_dat[0]));
		n_envelopes++;
	}

	return(displ);
}  // end of LoadEnvelope



static int Hash8(const char *string)
//==================================
/* Generate a hash code from the specified string */
{
   int  c;
	int  chars=0;
   int  hash=0;

   while((c = *string++) != 0)
   {
      c = tolower(c) - 'a';
      hash = hash * 8 + c;
      hash = (hash & 0x1ff) ^ (hash >> 8);    /* exclusive or */
		chars++;
   }

   return((hash+chars) & 0xff);
}   //  end of Hash8



static int LoadEnvelope2(FILE *f, const char *fname)
{//===================================================
	int ix, ix2;
	int n;
	int x, y;
	int displ;
	int n_points;
	double yy;
	char line_buf[128];
	float env_x[20];
	float env_y[20];
	int env_lin[20];
	unsigned char env[ENV_LEN];

	n_points = 0;
	fgets(line_buf,sizeof(line_buf),f);   // skip first line
	while(!feof(f))
	{
		if(fgets(line_buf,sizeof(line_buf),f) == NULL)
			break;

		env_lin[n_points] = 0;
		n = sscanf(line_buf,"%f %f %d",&env_x[n_points],&env_y[n_points],&env_lin[n_points]);
		if(n >= 2)
		{
			env_x[n_points] *= (float)1.28;  // convert range 0-100 to 0-128
			n_points++;
		}
	}
	env_x[n_points] = env_x[n_points-1];
	env_y[n_points] = env_y[n_points-1];

	ix = -1;
	ix2 = 0;
	for(x=0; x<ENV_LEN; x++)
	{
		if(x > env_x[ix+4])
			ix++;
		if(x >= env_x[ix2+1])
			ix2++;

		if(env_lin[ix2] > 0)
		{
			yy = env_y[ix2] + (env_y[ix2+1] - env_y[ix2]) * (float(x) - env_x[ix2]) / (env_x[ix2+1] - env_x[ix2]);
			y = int(yy * 2.55);
		}
		else
		if(n_points > 3)
			y = (int)(polint(&env_x[ix],&env_y[ix],4,x) * 2.55);  // convert to range 0-255
		else
			y = (int)(polint(&env_x[ix],&env_y[ix],3,x) * 2.55);
		if(y < 0) y = 0;
		if(y > 255) y = 255;
		env[x] = y;
	}


	if(n_envelopes < N_ENVELOPES)
	{
		strncpy0(envelope_paths[n_envelopes],fname,sizeof(envelope_paths[0]));
		memcpy(envelope_dat[n_envelopes],env,ENV_LEN);
		n_envelopes++;
	}

	displ = ftell(f_phdata);
	fwrite(env,1,128,f_phdata);

	return(displ);
}

static int LoadDataFile(const char *path, int control)
{//===================================================
	// load spectrum sequence or sample data from a file.
	// return index into spect or sample data area. bit 23=1 if a sample

	FILE *f;
	int id;
	int hash;
	int addr = 0;
	int type_code=' ';
	REF_HASH_TAB *p, *p2;
	char buf[sizeof(path_source)+120];

	if(strcmp(path,"NULL")==0)
		return(0);
	if(strcmp(path,"DFT")==0)
		return(1);

	count_references++;

	hash = Hash8(path);
	p = ref_hash_tab[hash];
	while(p != NULL)
	{
		if(strcmp(path,p->string)==0)
		{
			duplicate_references++;
			addr = p->value;    // already loaded this data
			break;
		}
		p = (REF_HASH_TAB *)p->link;
	}

	if(addr == 0)
	{
		sprintf(buf,"%s%s",path_source,path);

		if((f = fopen(buf,"rb")) == NULL)
		{
			sprintf(buf,"%s%s.wav",path_source,path);
			if((f = fopen(buf,"rb")) == NULL)
			{
				error("Can't read file: %s",path);
				return(0);
			}
		}

		id = Read4Bytes(f);
		rewind(f);

		if(id == 0x43455053)
		{
			addr = LoadSpect(path, control);
			type_code = 'S';
		}
		else
		if(id == 0x46464952)
		{
			addr = LoadWavefile(f,path);
			type_code = 'W';
		}
		else
		if(id == 0x43544950)
		{
			addr = LoadEnvelope(f,path);
			type_code = 'E';
		}
		else
		if(id == 0x45564E45)
		{
			addr = LoadEnvelope2(f,path);
			type_code = 'E';
		}
		else
		{
			error("File not SPEC or RIFF: %s",path);
			addr = -1;
		}
		fclose(f);

		if(addr > 0)
		{
			fprintf(f_phcontents,"%c  0x%.5x  %s\n",type_code,addr & 0x7fffff,path);
		}
	}

	// add this item to the hash table
	if(addr > 0)
	{
		p = ref_hash_tab[hash];
		p2 = (REF_HASH_TAB *)malloc(sizeof(REF_HASH_TAB)+strlen(path)+1);
		p2->value = addr;
		p2->ph_mnemonic = phoneme_out->mnemonic;   // phoneme which uses this file
		p2->ph_table = n_phoneme_tabs-1;
		strcpy(p2->string,path);
		p2->link = (char *)p;
		ref_hash_tab[hash] = p2;
	}

	return(addr);
}   //  end of LoadDataFile


static int CompileToneSpec(void)
{//=============================
	int pitch1=0;
	int pitch2=0;
	int pitch_env = 0;
	int amp_env = 0;

	pitch1 = NextItemBrackets(tNUMBER,2);
	pitch2 = NextItemBrackets(tNUMBER,3);

	if(item_terminator == ',')
	{
		NextItemBrackets(tSTRING,3);
		pitch_env = LoadDataFile(item_string, 0);
	}

	if(item_terminator == ',')
	{
		NextItemBrackets(tSTRING,1);
		amp_env = LoadDataFile(item_string, 0);
	}

	if(pitch1 < pitch2)
	{
		phoneme_out->start_type = pitch1;
		phoneme_out->end_type = pitch2;
	}
	else
	{
		phoneme_out->start_type = pitch2;
		phoneme_out->end_type = pitch1;
	}

	if(pitch_env != 0)
	{
		*prog_out++ = i_PITCHENV + ((pitch_env >> 16) & 0xf);
		*prog_out++ = pitch_env;
	}
	if(amp_env != 0)
	{
		*prog_out++ = i_AMPENV + ((amp_env >> 16) & 0xf);
		*prog_out++ = amp_env;
	}

	return(0);
}  // end of CompileToneSpec



int CompileSound(int keyword, int isvowel)
{//=======================================
	int addr;
	int value = 0;
	char path[N_ITEM_STRING];
	static int sound_instns[] = {i_FMT, i_WAV, i_VWLSTART, i_VWLENDING, i_WAVADD};

	NextItemBrackets(tSTRING,2);
	strcpy(path, item_string);
	if(item_terminator == ',')
	{
		if((keyword == kVOWELSTART) || (keyword == kVOWELENDING))
		{
			value = NextItemBrackets(tSIGNEDNUMBER,1);
			if(value > 127)
			{
				value = 127;
				error("Parameter > 127",NULL);
			}
			if(value < -128)
			{
				value = -128;
				error("Parameter < -128",NULL);
			}
		}
		else
		{
			value = NextItemBrackets(tNUMBER,1);
			if(value > 255)
			{
				value = 255;
				error("Parameter > 255",NULL);
			}
		}
	}
	addr = LoadDataFile(path, isvowel);
	addr = addr / 4;   // addr is words not bytes

	*prog_out++ = sound_instns[keyword-kFMT] + ((value & 0xff) << 4) + ((addr >> 16) & 0xf);
	*prog_out++ = addr & 0xffff;
	return(0);
}  // end of CompileSound



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

int CompileIf(int elif)
{//====================
	int key;
	int finish = 0;
	int word = 0;
	int word2;
	int data;
	int bitmap;
	int brackets;
	int not_flag;
	USHORT *prog_last_if = NULL;

	then_count = 2;
	after_if = 1;

	while(!finish)
	{
		not_flag = 0;
		word2 = 0;
		if(prog_out >= prog_out_max)
		{
			error("Phoneme program too large", NULL);
			return(0);
		}

		if((key = NextItem(tCONDITION)) < 0)
			error("Expected a condition, not '%s'",item_string);

		if((item_type == 0) && (key == k_NOT))
		{
			not_flag = 1;
			if((key = NextItem(tCONDITION)) < 0)
				error("Expected a condition, not '%s'",item_string);
		}

		if(item_type == tWHICH_PHONEME)
		{
			// prevPh(), thisPh(), nextPh(), next2Ph() etc
			if(key >= 6)
			{
			    // put the 'which' code in the next instruction
			    word2 = key;
			    key = 6;
			}
			key = key << 8;

			data = NextItemBrackets(tPROPERTIES,0);
			if(data >= 0)
			{
				word = key + data + 0x700;
			}
			else
			{
				data = LookupPhoneme(item_string,2);
				word = key + data;
			}
		}
		else
		if(item_type == tTEST)
		{
			if(key == kTHISSTRESS)
			{
				bitmap = 0;
				brackets = 2;
				do {
					data = NextItemBrackets(tNUMBER,brackets);
					if(data > 7)
						error("Expected list of stress levels",NULL);
					bitmap |= (1 << data);

					brackets = 3;
				} while(item_terminator == ',');
				word = i_StressLevel | bitmap;
			}
			else
			{
				word = key;
			}
		}
		else
		{
			error("Unexpected keyword '%s'",item_string);
		}

		// output the word
		prog_last_if = prog_out;
		*prog_out++ = word | i_CONDITION;

		if(word2 != 0)
			*prog_out++ = word2;
		if(not_flag)
			*prog_out++ = i_NOT;

		// expect AND, OR, THEN
		switch(NextItem(tCONDITION))
		{
		case k_AND:
			break;
		case k_OR:
			if(prog_last_if != NULL)
				*prog_last_if |=  i_OR;
			break;
		case k_THEN:
			finish = 1;
			break;
		default:
			error("Expected AND, OR, THEN",NULL);
			break;
		}
	}

	if(finish != 1)
	{
	}

	if(elif == 0)
	{
		if_level++;
		if_stack[if_level].p_else = NULL;
	}

	if_stack[if_level].returned = 0;
	if_stack[if_level].p_then = prog_out;
	*prog_out++ = i_JUMP_FALSE;

	return(0);
}  // end of CompileIf


void FillThen(int add)
{//===================
	USHORT *p;
	int offset;

	p = if_stack[if_level].p_then;
	if(p != NULL)
	{
		offset = prog_out - p + add;

		if((then_count == 1) && (if_level == 1))
		{
			// The THEN part only contains one statement, we can remove the THEN jump
			// and the interpreter will implicitly skip the statement.
			while(p < prog_out)
			{
				p[0] = p[1];
				p++;
			}
			prog_out--;
		}
		else
		{
			if(offset > MAX_JUMP)
			{
				error("IF block is too long",NULL);
			}
			*p = i_JUMP_FALSE + offset;
		}
		if_stack[if_level].p_then = NULL;
	}

	then_count = 0;
}  // end of FillThen


int CompileElse(void)
{//==================
	USHORT *ref;
	USHORT *p;

	if(if_level < 1)
	{
		error("ELSE not expected",NULL);
		return(0);
	}

	if(if_stack[if_level].returned == 0)
	{
		FillThen(1);
	}
	else
	{
		FillThen(0);
	}

	if(if_stack[if_level].returned == 0)
	{
		ref = prog_out;
		*prog_out++ = 0;

		if((p = if_stack[if_level].p_else) != NULL)
		{
			*ref = ref - p;    // backwards offset to the previous else
		}
		if_stack[if_level].p_else = ref;
	}

	return(0);
}  // end of CompileElse


int CompileElif(void)
{//===================
	if(if_level < 1)
	{
		error("ELIF not expected",NULL);
		return(0);
	}

	CompileElse();
	CompileIf(1);
	return(0);
}


int CompileEndif(void)
{//===================
	USHORT *p;
	int chain;
	int offset;

	if(if_level < 1)
	{
		error("ENDIF not expected",NULL);
		return(0);
	}

	FillThen(0);

	if((p = if_stack[if_level].p_else) != NULL)
	{
		do
		{
			chain = *p;   // a chain of previous else links

			offset = prog_out - p;
			if(offset > MAX_JUMP)
			{
				error("IF block is too long",NULL);
			}
			*p = i_JUMP + offset;

			p -= chain;
		} while (chain > 0);
	}

	if_level--;
	return(0);
}  // end of CompileEndif


static int CompileSwitch(int type)
{//===============================
// Type 0:  EndSwitch
//      1:  SwitchPrevVowelType
//      2:  SwitchNextVowelType

	if(type == 0)
	{
		// check the instructions in the Switch
		return(0);
	}

//	count_VowelStart = 0;
//	count_VowelEnding = 0;

	if(type == 1)
		*prog_out++ = i_SWITCH_PREVVOWEL+6;
	if(type == 2)
		*prog_out++ = i_SWITCH_NEXTVOWEL+6;
	return(0);
}  // end of CompileSwitch



static PHONEME_TAB_LIST *FindPhonemeTable(const char *string)
{//==========================================================
	int ix;

	for(ix=0; ix<n_phoneme_tabs; ix++)
	{
		if(strcmp(phoneme_tab_list2[ix].name, string) == 0)
		{
			return(&phoneme_tab_list2[ix]);
		}
	}
	error("Unknown phoneme table: '%s'",string);
	return(NULL);
}  // end of FindPhonemeTable



static PHONEME_TAB *FindPhoneme(const char *string)
{//================================================
	PHONEME_TAB_LIST *phtab = NULL;
	int ix;
	unsigned int mnem;
	char *phname;
	char buf[200];

	// is this the name of a phoneme which is in scope
	if((strlen(string) <= 4) && ((ix = LookupPhoneme(string,0)) != -1))
	{
		return(&phoneme_tab2[ix]);
	}

	// no, treat the name as phonemetable/phoneme
	strcpy(buf,string);
	if((phname = strchr(buf,'/')) != 0)
	{
		*phname++ = 0;
	}

	phtab = FindPhonemeTable(buf);
	if(phtab == NULL)
	{
		return(NULL);  // phoneme table not found
	}

	mnem = StringToWord(phname);
	for(ix=1; ix<256; ix++)
	{
		if(mnem == phtab->phoneme_tab_ptr[ix].mnemonic)
		{
			return(&phtab->phoneme_tab_ptr[ix]);
		}
	}

	error("Phoneme reference not found: '%s'",string);
	return(NULL);
}


static void ImportPhoneme(void)
{//============================
	unsigned int ph_mnem;
	unsigned int ph_code;
	PHONEME_TAB *ph;

	NextItem(tSTRING);

	if((ph = FindPhoneme(item_string)) == NULL)
		return;

	ph_mnem = phoneme_out->mnemonic;
	ph_code = phoneme_out->code;
	memcpy(phoneme_out,ph,sizeof(PHONEME_TAB));
	phoneme_out->mnemonic = ph_mnem;
	phoneme_out->code = ph_code;
	if(phoneme_out->type != phVOWEL)
	{
		phoneme_out->end_type = 0;  // voicingswitch, this must be set later to refer to a local phoneme
	}

}  // end of ImportPhoneme


static void CallPhoneme(void)
{//==========================
	PHONEME_TAB *ph;
	int ix;
	int addr = 0;

	NextItem(tSTRING);

	// first look for a procedure name
	for(ix=0; ix<n_procs; ix++)
	{
		if(strcmp(proc_names[ix],item_string) == 0)
		{
			addr = proc_addr[ix];
			break;
		}
	}
	if(ix == n_procs)
	{
		// procedure not found, try a phoneme name
		if((ph = FindPhoneme(item_string)) == NULL)
			return;
		addr = ph->program;

		if(phoneme_out->type == phINVALID)
		{
			// Phoneme type has not been set. Copy it from the called phoneme
			phoneme_out->type = ph->type;
			phoneme_flags = ph->phflags & ~phARTICULATION;
			place_articulation = (ph->phflags & phARTICULATION) >> 16;
			phoneme_out->start_type = ph->start_type;
			phoneme_out->end_type = ph->end_type;
			phoneme_out->std_length = ph->std_length;
			phoneme_out->length_mod = ph->length_mod;
		}
	}

	*prog_out++ = i_CALLPH + (addr >> 16);
	*prog_out++ = addr;
}  // end of CallPhoneme


static void DecThenCount()
{//=======================
	if(then_count >0)
		then_count--;
}

static void InstnPlusPhoneme(int instn)
{//====================================
	int phcode;
	DecThenCount();

	phcode = NextItemBrackets(tPHONEMEMNEM,0);
	*prog_out++ = instn + phcode;
}


int CompilePhoneme(int compile_phoneme)
{//====================================
	int endphoneme = 0;
	int keyword;
	int value;
	int phcode = 0;
	int flags;
	int ix;
	int start;
	int count;
	int c;
	char *p;
	char number_buf[12];
	char ipa_buf[N_ITEM_STRING+1];
	PHONEME_TAB phoneme_out2;
	PHONEME_PROG_LOG phoneme_prog_log;

	prog_out = prog_buf;
	prog_out_max = &prog_buf[MAX_PROG_BUF-1];
	if_level = 0;
	if_stack[0].returned = 0;
	after_if = 0;
	phoneme_flags = 0;
	place_articulation = 0;

	NextItem(tSTRING);
	if(compile_phoneme)
	{
		phcode = LookupPhoneme(item_string,1);    // declare phoneme if not already there
		if(phcode == -1) return(0);
		phoneme_out = &phoneme_tab2[phcode];
	}
	else
	{
		// declare a procedure
		if(n_procs >= N_PROCS)
		{
			error("Too many procedures",NULL);
			return(0);
		}
		strcpy(proc_names[n_procs], item_string);
		phoneme_out = &phoneme_out2;
	}

	phoneme_out->code = phcode;
	phoneme_out->program = 0;
	phoneme_out->type = phINVALID;
	phoneme_out->std_length = 0;
	phoneme_out->start_type = 0;
	phoneme_out->end_type = 0;
	phoneme_out->length_mod = 0;

	while(!endphoneme && !feof(f_in))
	{
		if((keyword = NextItem(tKEYWORD)) < 0)
		{
			if(keyword == -2)
			{
				error("Missing 'endphoneme' before end-of-file",NULL);   // end of file
				break;
			}
			error("Bad keyword in phoneme definition '%s'",item_string);
			continue;
		}

		switch(item_type)
		{
		case tPHONEME_TYPE:
			if(phoneme_out->type != phINVALID)
				error("More than one phoneme type: %s",item_string);
			phoneme_out->type = keyword;
			break;

		case tPLACE:
			if(place_articulation > 0)
				error("Place of articulation has already been given: %s",item_string);
			place_articulation = keyword;
			break;

		case tPHONEME_FLAG:
			phoneme_flags |= keyword;
			break;

		case tINSTRN1:
// instruction group 0, with 8 bit operands which set data in PHONEME_DATA
			switch(keyword)
			{
			case i_CHANGE_PHONEME:
			case i_APPEND_PHONEME:
			case i_APPEND_IFNEXTVOWEL:
			case i_INSERT_PHONEME:
			case i_REPLACE_NEXT_PHONEME:
			case i_VOICING_SWITCH:
			case i_CHANGE_IF+0:
			case i_CHANGE_IF+1:
			case i_CHANGE_IF+2:
			case i_CHANGE_IF+3:
				InstnPlusPhoneme(keyword << 8);
				break;

			case i_PAUSE_BEFORE:
				value = NextItemMax(255);
				*prog_out++ = (i_PAUSE_BEFORE << 8) + value;
				break;

			case i_PAUSE_AFTER:
				value = NextItemMax(255);
				*prog_out++ = (i_PAUSE_AFTER << 8) + value;
				break;

			case i_SET_LENGTH:
				value = NextItemMax(511);
				if(after_if == 0)
				{
					phoneme_out->std_length = value/2;
				}
				else
				{
					*prog_out++ = (i_SET_LENGTH << 8) + value/2;
					DecThenCount();
				}
				break;

			case i_ADD_LENGTH:
				value = NextItem(tSIGNEDNUMBER);
				*prog_out++ = (i_ADD_LENGTH << 8) + value/2;
				DecThenCount();
				break;

			case i_LENGTH_MOD:
				value = NextItem(tNUMBER);
				phoneme_out->length_mod = value;
				break;

			case i_IPA_NAME:
				NextItem(tSTRING);

				if(strcmp(item_string,"NULL")==0)
					strcpy(item_string," ");

				// copy the string, recognize characters in the form U+9999
				flags = 0;
				count = 0;
				ix = 1;

				for(p=item_string; *p != 0;)
				{
				    p += utf8_in(&c, p);

				    if((c == '|') && (count > 0))
				    {
				        // '|' means don't allow a tie or joiner before this letter
				        flags |= (1 << (count -1));
				    }
				    else
					if((c=='U') && (p[0]=='+'))
					{
					    int j;
						// U+9999
						p++;
						memcpy(number_buf,p,4);  // U+ should be followed by 4 hex digits
						number_buf[4] = 0;
						c = '#';
						sscanf(number_buf,"%x",&c);

						// move past the 4 hexdecimal digits
						for(j=0; j<4; j++)
						{
						    if(!isalnum(*p))
                                break;
                            p++;
						}
						ix += utf8_out(c, &ipa_buf[ix]);
						count++;
					}
					else
					{
					    ix += utf8_out(c, &ipa_buf[ix]);
						count++;
					}
				}
				ipa_buf[0] = flags;
				ipa_buf[ix] = 0;

                start = 1;
                if(flags != 0)
                    start = 0;   // only include the flags byte if bits are set
				value = strlen(&ipa_buf[start]);   // number of UTF-8 bytes

				*prog_out++ = (i_IPA_NAME << 8) + value;
				for(ix=0; ix < value; ix += 2)
				{
					*prog_out++ = (ipa_buf[ix+start] << 8) + (ipa_buf[ix+start+1] & 0xff);
				}
				break;
			}
			break;

		case tSTATEMENT:
			switch(keyword)
			{
			case kIMPORT_PH:
				ImportPhoneme();
				phoneme_flags = phoneme_out->phflags;
				break;

			case kSTARTTYPE:
				phcode = NextItem(tPHONEMEMNEM);
				if(phcode == -1)
					phcode = LookupPhoneme(item_string,1);
				phoneme_out->start_type = phcode;
				break;

			case kENDTYPE:
				phcode = NextItem(tPHONEMEMNEM);
				if(phcode == -1)
					phcode = LookupPhoneme(item_string,1);
				if(phoneme_out->type == phVOWEL)
				{
					phoneme_out->end_type = phcode;
				}
				else
				{
					if(phcode != phoneme_out->start_type)
					{
						error("endtype must equal starttype for consonants",NULL);
					}
				}
				break;

			case kVOICINGSWITCH:
				phcode = NextItem(tPHONEMEMNEM);
				if(phcode == -1)
					phcode = LookupPhoneme(item_string,1);
				phoneme_out->end_type = phcode;       // use end_type field for consonants as voicing_switch
				break;

			case kSTRESSTYPE:
				value = NextItem(tNUMBER);
				phoneme_out->std_length = value;
				if(prog_out > prog_buf)
				{
					error("stress phonemes can't contain program instructions",NULL);
					prog_out = prog_buf;
				}
				break;

			case kIF:
				endphoneme = CompileIf(0);
				break;

			case kELSE:
				endphoneme = CompileElse();
				break;

			case kELIF:
				endphoneme = CompileElif();
				break;

			case kENDIF:
				endphoneme = CompileEndif();
				break;

			case kENDSWITCH:
				break;

			case kSWITCH_PREVVOWEL:
				endphoneme = CompileSwitch(1);
				break;

			case kSWITCH_NEXTVOWEL:
				endphoneme = CompileSwitch(2);
				break;

			case kCALLPH:
				CallPhoneme();
				break;

			case kFMT:
				if_stack[if_level].returned = 1;
				DecThenCount();
				if(phoneme_out->type == phVOWEL)
					endphoneme = CompileSound(keyword,1);
				else
					endphoneme = CompileSound(keyword,0);
				break;

			case kWAV:
				if_stack[if_level].returned = 1;
			case kVOWELSTART:
			case kVOWELENDING:
			case kANDWAV:
				DecThenCount();
				endphoneme = CompileSound(keyword,0);
				break;

			case kVOWELIN:
				DecThenCount();
				endphoneme = CompileVowelTransition(1);
				break;

			case kVOWELOUT:
				DecThenCount();
				endphoneme = CompileVowelTransition(2);
				break;

			case kTONESPEC:
				endphoneme = CompileToneSpec();
				break;

			case kCONTINUE:
				*prog_out++ = i_CONTINUE;
				DecThenCount();
				break;

			case kRETURN:
				*prog_out++ = i_RETURN;
				break;

			case kINCLUDE:
			case kPHONEMENUMBER:
			case kPHONEMETABLE:
				error("Missing 'endphoneme' before '%s'",item_string);  // drop through to endphoneme
			case kENDPHONEME:
			case kENDPROCEDURE:
				endphoneme = 1;
				if(if_level > 0)
				{
					error("Missing ENDIF",NULL);
				}
				if((prog_out > prog_buf) && (if_stack[0].returned == 0))
				{
					*prog_out++ = i_RETURN;
				}
				break;
			}

			break;
		}
	}

	if(endphoneme != 1)
	{
		error("'endphoneme' not expected here",NULL);
	}

	if(compile_phoneme)
	{
		if(phoneme_out->type == phINVALID)
		{
			error("Phoneme type is missing",NULL);
			phoneme_out->type = 0;
		}
		phoneme_out->phflags = place_articulation << 16;
		phoneme_out->phflags |= phoneme_flags;

		if(phoneme_out->phflags & phVOICED)
		{
			if(phoneme_out->type == phSTOP)
				phoneme_out->type = phVSTOP;
			else
			if(phoneme_out->type == phFRICATIVE)
				phoneme_out->type = phVFRICATIVE;
		}

		if(phoneme_out->std_length == 0)
		{
			if(phoneme_out->type == phVOWEL)
				phoneme_out->std_length = 180/2;  // default length for vowel
		}

		phoneme_out->phflags |= phLOCAL;  //declared in this phoneme table

		if(phoneme_out->type == phDELETED)
		{
			phoneme_out->mnemonic = 0x01;  // will not be recognised
		}
	}

	DecompilePhoneme(f_errors, phoneme_out, compile_phoneme);

	if(prog_out > prog_buf)
	{
		// write out the program for this phoneme
		fflush(f_phindex);
		phoneme_out->program = ftell(f_phindex) / sizeof(USHORT);

		if(f_prog_log != NULL)
		{
			phoneme_prog_log.addr = phoneme_out->program;
			phoneme_prog_log.length = prog_out - prog_buf;
			fwrite(&phoneme_prog_log, 1, sizeof(phoneme_prog_log), f_prog_log);
		}

		if(compile_phoneme == 0)
		{
			proc_addr[n_procs++] =  ftell(f_phindex) / sizeof(USHORT);
		}
		fwrite(prog_buf, sizeof(USHORT), prog_out - prog_buf, f_phindex);
	}

	return(0);
}  // end of CompilePhoneme





static void WritePhonemeTables()
{//=============================
	int ix;
	int j;
	int n;
	int value;
	int count;
	PHONEME_TAB *p;

	value = n_phoneme_tabs;
	fputc(value,f_phtab);
	fputc(0,f_phtab);
	fputc(0,f_phtab);
	fputc(0,f_phtab);

	for(ix=0; ix<n_phoneme_tabs; ix++)
	{
		p = phoneme_tab_list2[ix].phoneme_tab_ptr;
		n = n_phcodes_list[ix];
		p[n].mnemonic = 0;   // terminate the phoneme table

		// count number of locally declared phonemes
		count=0;
		for(j=0; j<n; j++)
		{
			if(ix==0)
				p[j].phflags |= phLOCAL;  // write all phonemes in the base phoneme table

			if(p[j].phflags & phLOCAL)
				count++;
		}
		phoneme_tab_list2[ix].n_phonemes = count+1;

		fputc(count+1,f_phtab);
		fputc(phoneme_tab_list2[ix].includes,f_phtab);
		fputc(0,f_phtab);
		fputc(0,f_phtab);
		Write4Bytes(f_phtab, phoneme_tab_list2[ix].equivalence_tables);   // byte index into phondata for equivalence tables

		fwrite(phoneme_tab_list2[ix].name,1,N_PHONEME_TAB_NAME,f_phtab);

		for(j=0; j<n; j++)
		{
			if(p[j].phflags & phLOCAL)
			{
				// this bit is set temporarily to incidate a local phoneme, declared in
				// in the current phoneme file
				p[j].phflags &= ~phLOCAL;
				fwrite(&p[j],sizeof(PHONEME_TAB),1,f_phtab);
			}
		}
		fwrite(&p[n],sizeof(PHONEME_TAB),1,f_phtab);  // include the extra list-terminator phoneme entry
		free(p);
	}
}  // end of WritePhonemeTables


static void EndPhonemeTable()
{//==========================
	int  ix;
	int *pw;
	int length;

	if(n_phoneme_tabs == 0)
		return;

	fprintf(f_errors,"\n");

	// check that all referenced phonemes have been declared
	for(ix=0; ix<n_phcodes; ix++)
	{
		if(phoneme_tab2[ix].type == phINVALID)
		{
			fprintf(f_errors,"%3d: Phoneme [%s] not declared, referenced at line %d\n",linenum,
				WordToString(phoneme_tab2[ix].mnemonic),int(phoneme_tab2[ix].program));
			error_count++;
			phoneme_tab2[ix].type = 0;   // prevent the error message repeating
		}
	}

	n_phcodes_list[n_phoneme_tabs-1] = n_phcodes;

	if((length = p_equivalence - equivalence_buf) > 0)
	{
		// terminate the list of phoneme equivalence tables
		pw = (int *)p_equivalence;
		pw[0] = 0;

		// write the equivalence data into phondata, and remember it's address
		ix = ftell(f_phdata);
		fprintf(f_phcontents,"Q  0x%.5x  %s\n", ix, phoneme_tab_list2[n_phoneme_tabs-1].name);
		phoneme_tab_list2[n_phoneme_tabs-1].equivalence_tables = ix;
		fwrite(equivalence_buf, length+4, 1, f_phdata);
	}
}


static void StartPhonemeTable(const char *name)
{//============================================
	int ix;
	int j;
	PHONEME_TAB *p;

	fprintf(f_errors,"______________________________\nPhoneme Table: '%s'\n",name);

	if(n_phoneme_tabs >= N_PHONEME_TABS-1)
	{
		Error("Too many phonemetables");
		return;
	}
	p = (PHONEME_TAB *)calloc(sizeof(PHONEME_TAB),N_PHONEME_TAB);

	if(p == NULL)
	{
		Error("Out of memory");
		return;
	}

	if(gui_flag)
		progress->Update(n_phoneme_tabs);

	memset(&phoneme_tab_list2[n_phoneme_tabs], 0, sizeof(PHONEME_TAB_LIST));
	phoneme_tab_list2[n_phoneme_tabs].phoneme_tab_ptr = phoneme_tab2 = p;
	strncpy0(phoneme_tab_list2[n_phoneme_tabs].name, name, N_PHONEME_TAB_NAME);
	n_phcodes = 1;
	phoneme_tab_list2[n_phoneme_tabs].includes = 0;
	p_equivalence = equivalence_buf;

	if(n_phoneme_tabs > 0)
	{
		NextItem(tSTRING);	// name of base phoneme table
		for(ix=0; ix<n_phoneme_tabs; ix++)
		{
			if(strcmp(item_string,phoneme_tab_list2[ix].name)==0)
			{
				phoneme_tab_list2[n_phoneme_tabs].includes = ix+1;

				// initialise the new phoneme table with the contents of this one
				memcpy(phoneme_tab2,phoneme_tab_list2[ix].phoneme_tab_ptr,sizeof(PHONEME_TAB)*N_PHONEME_TAB);
				n_phcodes = n_phcodes_list[ix];

				// clear "local phoneme" bit"
				for(j=0; j<n_phcodes; j++)
					phoneme_tab2[j].phflags &= ~phLOCAL;
				break;
			}
		}
		if(ix == n_phoneme_tabs)
		{
			error("Can't find base phonemetable '%s'",item_string);
		}
	}
	else
	{
		ReservePhCodes();
	}

	n_phoneme_tabs++;
}  // end of StartPhonemeTable


static void CompileEquivalents()
{//=============================
	// a list of phonemes in another language and the equivalent phoneme strings in this language

	int ix;
	int n_names;
	int n_bytes;
	int foreign_phoneme;
	int foreign_error = 0;
	int remove_stress = 0;
	char *p_start;
	char *p;
	int foreign_table;
	char foreign_table_name[40];
	char line_buf[80];
	char names[6][80];
	char phcode[7];
	char save_voice_name[80];

	NextItem(tSTRING);
	strcpy(foreign_table_name, item_string);
	strcpy(save_voice_name,voice_name2);

	if((foreign_table = SelectPhonemeTableName(foreign_table_name)) < 0)
	{
		if(strcmp(foreign_table_name, "NULL") != 0)
		{
			error("Unknown phoneme table '%s'", foreign_table_name);
		}
		foreign_error = 1;
		foreign_phoneme = 0;
	}

	p_start = p_equivalence;
	p_equivalence += 8;

	p_start[0] = foreign_table;

	linenum--;
	while(!feof(f_in))
	{
		linenum++;
		if(fgets(line_buf, sizeof(line_buf), f_in) == NULL)
			break;

		if((p = strstr(line_buf,"//")) != NULL)
			*p = 0;

		for(ix=0; ix<6; ix++)
			names[ix][0] = 0;
		n_names = sscanf(line_buf,"%s %s %s %s %s %s",names[0],names[1],names[2],names[3],names[4],names[5]);
		if(n_names < 1)
			continue;

		if(strcmp(names[0],"endphoneme") == 0)
			break;

		if(foreign_error)
			continue;

		if(strcmp(names[0],"remove_stress") == 0)
		{
			remove_stress = 1;
			continue;
		}

		if(p_equivalence > &equivalence_buf[sizeof(equivalence_buf) - 16])
		{
			error("'equivalents' tables are too large",NULL);
			break;
		}

		if(foreign_error == 0)
		{
			phcode[0] = foreign_phoneme = LookupPhonemeString(names[0]);
			if(foreign_phoneme == 0)
			{
				sprintf(line_buf,"%s/%s", foreign_table_name, names[0]);
				error("Unknown phoneme '%s'", line_buf);
			}
		}

		for(ix=1; ix<n_names; ix++)
		{
			phcode[ix] = LookupPhoneme(names[ix], 1);
		}

		// only write a translation if it has an effect
		if((n_names > 2) || (phcode[0] != phcode[1]))
		{
			// write: foreign phoneme number, then a string of local phoneme numbers
			memcpy(p_equivalence, phcode, n_names);
			p_equivalence += n_names;
			*p_equivalence++ = 0;
		}
	}
	*p_equivalence++ = 0;

	p_equivalence = (char *)((long int)(p_equivalence + 3) & ~0x3);  // align to word boundary
	n_bytes = p_equivalence - p_start;
	p_start[1] = remove_stress;
	n_bytes = n_bytes / 4;
	p_start[2] = n_bytes >> 8;    // index of next table
	p_start[3] = n_bytes;

    if(gui_flag != 0)
    {
        LoadVoice(voice_name2,0);  // reset the original phoneme table
        LoadVoiceVariant(save_voice_name,0);
    }
}  // end of CompileEquivalents



static void CompilePhonemeFiles()
{//==============================
	int item;
	FILE *f;
	char buf[sizeof(path_source)+120];

	linenum = 1;

	count_references = 0;
	duplicate_references = 0;
	count_frames = 0;
	n_procs = 0;

	for(;;)
	{
		if(feof(f_in))
		{
			// end of file, go back to previous from, from which this was included

			if(stack_ix == 0)
				break;   // end of top level, finished
			fclose(f_in);
			f_in = stack[--stack_ix].file;
			strcpy(current_fname,stack[stack_ix].fname);
			linenum = stack[stack_ix].linenum;
			fprintf(f_errors,"\n\n");
		}

		item = NextItem(tKEYWORD);

		switch(item)
		{
        case kUTF8_BOM:
            break;  // ignore bytes 0xef 0xbb 0xbf

		case kINCLUDE:
			NextItem(tSTRING);
			sprintf(buf,"%s%s",path_source,item_string);

			if((stack_ix < N_STACK) && (f = fopen_log(f_errors,buf,"rb")) != NULL)
			{
				fprintf(f_errors,"include %s\n",item_string);
				stack[stack_ix].linenum = linenum;
				strcpy(stack[stack_ix].fname,current_fname);
				stack[stack_ix++].file = f_in;

				f_in = f;
				strncpy0(current_fname,item_string,sizeof(current_fname));
				linenum = 1;
			}
			break;

		case kPHONEMETABLE:
			EndPhonemeTable();
			NextItem(tSTRING);	// name of the new phoneme table
			StartPhonemeTable(item_string);
			break;

		case kPHONEMESTART:
			if(n_phoneme_tabs == 0)
			{
				Error("phonemetable is missing");
				return;
			}
			CompilePhoneme(1);
			break;

		case kPROCEDURE:
			CompilePhoneme(0);
			break;

		case kEQUIVALENTS:
			CompileEquivalents();
			break;

		default:
			if(!feof(f_in))
				Error("Keyword 'phoneme' expected");
			break;
		}
	}
	phoneme_tab2[n_phcodes+1].mnemonic = 0;  // terminator
}  //  end of CompilePhonemeFiles



static void CompilePhonemeData2(const char *source)
{//================================================
	char fname[sizeof(path_source)+40];
	wxString report;
	wxString report_dict;

#ifdef MAKE_ENVELOPES
make_envs();
#endif

	n_envelopes = 0;
	error_count = 0;
	resample_count = 0;
memset(markers_used,0,sizeof(markers_used));

	f_errors = stderr;

	if(!wxDirExists(path_phsource))
	{
		if(gui_flag)
		{
			wxString dirname = wxDirSelector(_T("Phoneme source directory"),path_phsource);
			if(!dirname.IsEmpty())
			{
				path_phsource = dirname;
				strncpy0(path_source,path_phsource.mb_str(wxConvLocal),sizeof(path_source)-1);
				strcat(path_source,"/");
			}
		}
		else
		{
			fprintf(stderr,"Can't find phoneme source directory: %s\n",path_source);
		}
	}

	strncpy0(current_fname,source,sizeof(current_fname));

	strncpy0(fname,path_phfile.mb_str(wxConvLocal),sizeof(fname));
	f_in = fopen_log(f_errors,fname,"rb");
	if(f_in == NULL)
	{
		if(gui_flag)
		{
			wxString phfile = wxFileSelector(_T("Master phonemes file"),path_phsource,
			_T(""),_T(""),_T("*"),wxOPEN);

			if(!phfile.IsEmpty())
			{
				path_phfile = phfile;
			}
		}

		strncpy0(fname,path_phfile.mb_str(wxConvLocal),sizeof(fname));
		f_in = fopen_log(f_errors,fname,"rb");
		if(f_in == NULL)
		{
			wxLogError(_T("Can't read master phonemes file:\n") + wxString(fname,wxConvLocal));
			return;
		}
	}

	progress_max = 0;
	while(fgets(fname,sizeof(fname),f_in) != NULL)
	{
		// count the number of phoneme tables declared in the master phonemes file
		if(memcmp(fname,"phonemetable",12)==0)
			progress_max++;
	}
	rewind(f_in);

	sprintf(fname,"%s%s",path_source,"error_log");
	if((f_errors = fopen_log(f_errors,fname,"w")) == NULL)
		f_errors = stderr;

	sprintf(fname,"%s%s",path_source,"compile_report");
	f_report = fopen_log(f_errors, fname,"w");


	sprintf(fname,"%s/%s",path_home,"phondata-manifest");
	if((f_phcontents = fopen_log(f_phcontents,fname,"w")) == NULL)
		f_phcontents = stderr;

	fprintf (f_phcontents,
"# This file lists the type of data that has been compiled into the\n"
"# phondata file\n"
"#\n"
"# The first character of a line indicates the type of data:\n"
"#   S - A SPECT_SEQ structure\n"
"#   W - A wavefile segment\n"
"#   E - An envelope\n"
"#   Q - Phoneme equivalence tables\n"
"#\n"
"# Address is the displacement within phondata of this item\n"
"#\n"
"#  Address  Data file\n"
"#  -------  ---------\n");


	sprintf(fname,"%s/%s",path_home,"phondata");
	f_phdata = fopen_log(f_errors,fname,"wb");

	sprintf(fname,"%s/%s",path_home,"phonindex");
	f_phindex = fopen_log(f_errors,fname,"wb");

	sprintf(fname,"%s/%s",path_home,"phontab");
	f_phtab = fopen_log(f_errors,fname,"wb");

	if(f_phdata==NULL || f_phindex==NULL || f_phtab==NULL)
	{
		return;
	}

	sprintf(fname,"%scompile_prog_log",path_source);
	f_prog_log = fopen_log(f_errors,fname,"wb");

	if(gui_flag)
	{
		progress = new wxProgressDialog(_T("Compiling"),_T(""),progress_max);
	}
	else
	{
		fprintf(stderr,"Compiling phoneme data: %s\n",path_source);
	}

	// write a word so that further data doesn't start at displ=0
	Write4Bytes(f_phdata,version_phdata);
	Write4Bytes(f_phdata,samplerate_native);
	Write4Bytes(f_phindex,version_phdata);

	memset(ref_hash_tab,0,sizeof(ref_hash_tab));

	n_phoneme_tabs = 0;
	stack_ix = 0;
	StartPhonemeTable("base");
	CompilePhonemeFiles();

	EndPhonemeTable();
	WritePhonemeTables();

fprintf(f_errors,"\nRefs %d,  Reused %d\n",count_references,duplicate_references);
	fclose(f_in);
	fclose(f_phdata);
    if(f_prog_log != NULL)
        fclose(f_prog_log);
	fclose(f_phindex);
	fclose(f_phtab);
	fclose(f_phcontents);

	LoadPhData(NULL);

	if(gui_flag != 0)
        LoadVoice(voice_name2,0);

	CompileReport();
	report_dict = CompileAllDictionaries();
	DrawEnvelopes();


	if(gui_flag)
	{
		delete progress;
	}

	if(resample_count > 0)
	{
	    fprintf(f_errors, "\n%d WAV files resampled to %d Hz\n", resample_count, samplerate_native);
        report.Printf(_T("Compiled phonemes: %d errors, %d files resampled to %d Hz. "),error_count, resample_count, samplerate_native);
	}
	else
	{
        report.Printf(_T("Compiled phonemes: %d errors."),error_count);
	}

	if(f_errors != stderr)
        fclose(f_errors);

	if(error_count > 0)
	{
		report += _T(" See file: 'phsource/error_log'.");
		wxLogError(report);
	}
	wxLogStatus(report + report_dict);

	if(gui_flag == 0)
	{
		strncpy0(fname,(report+report_dict).mb_str(wxConvLocal),sizeof(fname));
		fprintf(stderr,"%s\n",fname);

	}

    ReadPhondataManifest();
}  // end of CompilePhonemeData



void CompileMbrola()
{//=================
	char *p;
	FILE *f_in;
	FILE *f_out;
	int percent;
	int n;
	int *pw;
	int *pw_end;
	int count = 0;
	int control;
	char phoneme[40];
	char phoneme2[40];
	char name1[40];
	char name2[40];
	char mbrola_voice[40];
	char buf[sizeof(path_home)+30];
	int mbrola_ctrl = 20;   // volume in 1/16 ths
	MBROLA_TAB data[N_PHONEME_TAB];

	wxString filepath = wxFileSelector(_T("Read Mbrola phonemes file"),path_phsource+_T("/mbrola"),_T(""),_T(""),_T("*"),wxOPEN);
	strcpy(buf,filepath.mb_str(wxConvLocal));
	if((f_in = fopen(buf,"r")) == NULL)
	{
		wxLogError(_T("Can't read: ")+filepath);
		return;
	}

	while(fgets(buf,sizeof(phoneme),f_in) != NULL)
	{
		buf[sizeof(phoneme)-1] = 0;

		if((p = strstr(buf,"//")) != NULL)
			*p = 0;   // truncate line at comment

		if(memcmp(buf,"volume",6)==0)
		{
			mbrola_ctrl = atoi(&buf[6]);
			continue;
		}

		n = sscanf(buf,"%d %s %s %d %s %s",&control,phoneme,phoneme2,&percent,name1,name2);
		if(n >= 5)
		{
			data[count].name = StringToWord(phoneme);
			if(strcmp(phoneme2,"NULL")==0)
				data[count].next_phoneme = 0;
			else
			if(strcmp(phoneme2,"VWL")==0)
				data[count].next_phoneme = 2;
			else
				data[count].next_phoneme = StringToWord(phoneme2);
			data[count].mbr_name = 0;
			data[count].mbr_name2 = 0;
			data[count].percent = percent;
			data[count].control = control;
			if(strcmp(name1,"NULL")!=0)
				data[count].mbr_name = StringToWord(name1);
			if(n == 6)
				data[count].mbr_name2 = StringToWord(name2);

			count++;
		}
	}
	fclose(f_in);

	wxFileName filename = wxFileName(filepath);
	strcpy(mbrola_voice,filename.GetName().mb_str(wxConvLocal));
	sprintf(buf,"%s/mbrola_ph/%s_phtrans",path_home,mbrola_voice);
	if((f_out = fopen(buf,"wb")) == NULL)
	{
		wxLogError(_T("Can't write to: ")+wxString(buf,wxConvLocal));
		return;
	}

	data[count].name = 0;  // list terminator
	Write4Bytes(f_out, mbrola_ctrl);

	pw_end = (int *)(&data[count+1]);
	for(pw = (int *)data; pw < pw_end; pw++)
	{
		Write4Bytes(f_out, *pw);
	}
	fclose(f_out);
	wxLogStatus(_T("Mbrola translation file: %d phonemes"),count);
}  // end of CompileMbrola



static const char *preset_tune_names[] = {
"s1", "c1", "q1", "e1", NULL};

static const TUNE default_tune = {
{0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0},
{0, 40, 24, 8, 0, 0, 0, 0},
46, 57, PITCHfall, 16, 0, 0,
255, 78, 50, 255,
3, 5,
{-7,-7,-7}, {-7,-7,-7},
PITCHfall, 64, 8,
PITCHfall, 70, 18, 24, 12,
PITCHfall, 70, 18, 24, 12, 0,
{0,0,0,0,0,0,0,0}, 0
};

#define N_TUNE_NAMES  100

MNEM_TAB envelope_names[] = {
	{"fall", 0},
	{"rise", 2},
	{"fall-rise", 4},
	{"fall-rise2", 6},
	{"rise-fall",  8},
	{"fall-rise3",10},
	{"fall-rise4",12},
	{"fall2", 14},
	{"rise2", 16},
	{"rise-fall-rise", 18},
	{NULL, -1} };


//	env_fallrise3, env_fallrise3,
//	env_fallrise4, env_fallrise4,
//	env_fall2, env_fall2,
//	env_rise2, env_rise2,
//	env_risefallrise, env_risefallrise

int LookupEnvelope(const char *name)
{//=================================
	return(LookupMnem(envelope_names, name));
}


void CompileIntonation()
{//=====================
	int ix;
	char *p;
	char c;
	int keyword;
	int compiling_tune = 0;
	int n_tune_names = 0;
	int done_split;
	int done_onset = 0;
	int done_last = 0;
	int n_preset_tunes = 0;
	int found;
	int tune_number = 0;
	FILE *f_out;
	wxString report;
	TUNE *tune_data;
	TUNE new_tune;


	char name[12];
	char fname_errors[sizeof(path_source)+120];
	char tune_names[N_TUNE_NAMES][12];
	char buf[sizeof(path_source)+120];

	error_count = 0;

	sprintf(fname_errors,"%s%s",path_source,"error_intonation");
	if((f_errors = fopen(fname_errors,"w")) == NULL)
		f_errors = stderr;

	sprintf(buf,"%sintonation.txt",path_source);
	if((f_in = fopen(buf, "r")) == NULL)
	{
		sprintf(buf,"%sintonation",path_source);
		if((f_in = fopen_log(f_errors, buf, "r")) == NULL)
		{
			wxLogError(_T("Can't read file: ") + wxString(buf,wxConvLocal));
			fclose(f_errors);
			return;
		}
	}


	for(ix=0; preset_tune_names[ix] != NULL; ix++)
	{
		strcpy(tune_names[ix], preset_tune_names[ix]);
	}
	n_tune_names = ix;
	n_preset_tunes = ix;

	// make a list of the tune names
	while(!feof(f_in))
	{
		if(fgets(buf,sizeof(buf),f_in) == NULL)
			break;

		if((memcmp(buf,"tune",4)==0) && isspace(buf[4]))
		{
			p = &buf[5];
			while(isspace(*p)) p++;

			ix = 0;
			while((ix < int(sizeof(name) - 1)) && !isspace(*p))
			{
				name[ix++] = *p++;
			}
			name[ix] = 0;

			found = 0;
			for(ix = 0; ix < n_tune_names; ix++)
			{
				if(strcmp(name, tune_names[ix]) == 0)
				{
					found = 1;
					break;
				}
			}

			if(found == 0)
			{
				strncpy0(tune_names[n_tune_names++], name, sizeof(name));

				if(n_tune_names >= N_TUNE_NAMES)
					break;
			}
		}
	}
	rewind(f_in);
	linenum = 1;

	tune_data = (TUNE *)calloc(sizeof(TUNE), n_tune_names);
	if(tune_data == NULL)
	{
		fprintf(f_errors, "Failed to allocate data for tunes\n");
		fclose(f_in);
		fclose(f_errors);
		return;
	}

	sprintf(buf,"%s/intonations",path_home);
	f_out = fopen_log(f_errors, buf, "wb");
	if(f_out == NULL)
	{
		fclose(f_in);
		fclose(f_errors);
		return;
	}

	while(!feof(f_in))
	{
		keyword = NextItem(tINTONATION);

		switch(keyword)
		{
		case kTUNE:
			if(compiling_tune)
			{
			}
			compiling_tune = 1;
			done_split = 0;

			memcpy(&new_tune, &default_tune, sizeof(TUNE));
			NextItem(tSTRING);
			strncpy0(new_tune.name, item_string, sizeof(new_tune.name));

			found = 0;
			tune_number = 0;
			for(ix=0; ix < n_tune_names; ix++)
			{
				if(strcmp(new_tune.name, tune_names[ix]) == 0)
				{
					found = 1;
					tune_number = ix;

					if(tune_data[ix].name[0] != 0)
						found = 2;
					break;
				}
			}
//			fprintf(f_errors,"tune %s  (%d)\n", new_tune.name, tune_number);
			if(found == 2)
			{
				error("Duplicate tune name: '%s'",new_tune.name);
			}
			if(found == 0)
			{
				error("Bad tune name: '%s;", new_tune.name);
			}
			break;

		case kENDTUNE:
			compiling_tune = 0;

			if(done_onset == 0)
			{
				new_tune.unstr_start[0] = new_tune.unstr_start[1];
				new_tune.unstr_end[0] = new_tune.unstr_end[1];
			}
			if(done_last == 0)
			{
				new_tune.unstr_start[2] = new_tune.unstr_start[1];
				new_tune.unstr_end[2] = new_tune.unstr_end[1];
			}
			memcpy(&tune_data[tune_number], &new_tune, sizeof(TUNE));
			break;

		case kTUNE_PREHEAD:
			new_tune.prehead_start = NextItem(tNUMBER);
			new_tune.prehead_end = NextItem(tNUMBER);
			break;

		case kTUNE_ONSET:
			new_tune.onset = NextItem(tNUMBER);
			new_tune.unstr_start[0] = NextItem(tSIGNEDNUMBER);
			new_tune.unstr_end[0] = NextItem(tSIGNEDNUMBER);
			done_onset = 1;
			break;

		case kTUNE_HEADLAST:
			new_tune.head_last = NextItem(tNUMBER);
			new_tune.unstr_start[2] = NextItem(tSIGNEDNUMBER);
			new_tune.unstr_end[2] = NextItem(tSIGNEDNUMBER);
			done_last = 1;
			break;

		case kTUNE_HEADENV:
			NextItem(tSTRING);
			if((ix = LookupEnvelope(item_string)) < 0)
				error("Bad envelope name: '%s'",item_string);
			else
				new_tune.stressed_env = ix;
			new_tune.stressed_drop = NextItem(tNUMBER);
			break;

		case kTUNE_HEAD:
			new_tune.head_max_steps = NextItem(tNUMBER);
			new_tune.head_start = NextItem(tNUMBER);
			new_tune.head_end = NextItem(tNUMBER);
			new_tune.unstr_start[1] = NextItem(tSIGNEDNUMBER);
			new_tune.unstr_end[1] = NextItem(tSIGNEDNUMBER);
			break;

		case kTUNE_HEADEXTEND:
			// up to 8 numbers
			for(ix=0; ix < int(sizeof(new_tune.head_extend)); ix++)
			{
				if(!isdigit(c = CheckNextChar()) && (c != '-'))
					break;

				new_tune.head_extend[ix] = (NextItem(tSIGNEDNUMBER) * 64) / 100;  // convert from percentage to 64ths
			}
			new_tune.n_head_extend = ix;    // number of values
			break;

		case kTUNE_NUCLEUS0:
			NextItem(tSTRING);
			if((ix = LookupEnvelope(item_string)) < 0)
			{
				error("Bad envelope name: '%s'",item_string);
				break;
			}
			new_tune.nucleus0_env = ix;
			new_tune.nucleus0_max = NextItem(tNUMBER);
			new_tune.nucleus0_min = NextItem(tNUMBER);
			break;

		case kTUNE_NUCLEUS1:
			NextItem(tSTRING);
			if((ix = LookupEnvelope(item_string)) < 0)
			{
				error("Bad envelope name: '%s'",item_string);
				break;
			}
			new_tune.nucleus1_env = ix;
			new_tune.nucleus1_max = NextItem(tNUMBER);
			new_tune.nucleus1_min = NextItem(tNUMBER);
			new_tune.tail_start = NextItem(tNUMBER);
			new_tune.tail_end = NextItem(tNUMBER);

			if(!done_split)
			{
				// also this as the default setting for 'split'
				new_tune.split_nucleus_env = ix;
				new_tune.split_nucleus_max = new_tune.nucleus1_max;
				new_tune.split_nucleus_min = new_tune.nucleus1_min;
				new_tune.split_tail_start = new_tune.tail_start;
				new_tune.split_tail_end = new_tune.tail_end;
			}
			break;

		case kTUNE_SPLIT:
			NextItem(tSTRING);
			if((ix = LookupEnvelope(item_string)) < 0)
			{
				error("Bad envelope name: '%s'",item_string);
				break;
			}
			done_split = 1;
			new_tune.split_nucleus_env = ix;
			new_tune.split_nucleus_max = NextItem(tNUMBER);
			new_tune.split_nucleus_min = NextItem(tNUMBER);
			new_tune.split_tail_start = NextItem(tNUMBER);
			new_tune.split_tail_end = NextItem(tNUMBER);
			NextItem(tSTRING);
			item_string[12] = 0;
			for(ix=0; ix < n_tune_names; ix++)
			{
				if(strcmp(item_string, tune_names[ix]) == 0)
					break;
			}

			if(ix == n_tune_names)
				error("Tune '%s' not found",item_string);
			else
				new_tune.split_tune = ix;
			break;

		default:
			error("Unexpected: '%s'",item_string);
			break;
		}
	}

	for(ix = 0; ix < n_preset_tunes; ix++)
	{
		if(tune_data[ix].name[0] == 0)
		{
			error("Tune '%s' not defined", preset_tune_names[ix]);
		}
	}
	fwrite(tune_data, n_tune_names, sizeof(TUNE), f_out);
	fclose(f_in);
	fclose(f_out);

	report.Printf(_T("Compiled %d intonation tunes: %d errors."),n_tune_names, error_count);
	wxLogStatus(report);

	if(error_count > 0)
	{
		DisplayErrorFile(fname_errors);
	}
	LoadPhData(NULL);

}   // end of CompileIntonation



void CompilePhonemeData()
{
    WavegenInit(22050, 0);
	WavegenSetVoice(voice);
	CompilePhonemeData2("phonemes");
}


void CompileSampleRate()
{
    long value;
    value = wxGetNumberFromUser(_T("Compile phoneme data with a specified sample rate"), _T("Sample rate"), _T("Resample (needs sox)"), 22050, 5000, 48000);

    if(value > 1000)
    {
        WavegenInit(value, 0);
        WavegenSetVoice(voice);
        CompilePhonemeData2("phonemes");
    }
}

