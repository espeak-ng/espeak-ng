/***************************************************************************
 *   Copyright (C) 2005 to 2010 by Jonathan Duddington                     *
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



// phoneme types
#define phPAUSE   0
#define phSTRESS  1
#define phVOWEL   2
#define phLIQUID  3
#define phSTOP    4
#define phVSTOP   5
#define phFRICATIVE 6
#define phVFRICATIVE 7
#define phNASAL   8
#define phVIRTUAL 9
#define phDELETED 14
#define phINVALID 15


// phoneme properties
//   bits 16-19 give place of articulation
#define phARTICULATION 0xf0000
#define phWAVE     0x01
#define phUNSTRESSED 0x02
#define phFORTIS   0x08
#define phVOICED   0x10
#define phSIBILANT 0x20
#define phNOLINK   0x40
#define phTRILL    0x80
#define phVOWEL2   0x100   // liquid that is considered a vowel
#define phPALATAL  0x200
#define phSINGLE_INSTN  0x1000    // this phoneme has a single instruction program, with an implicit Return
#define phBRKAFTER 0x4000  // [*] add a post-pause

#define phNONSYLLABIC  0x100000   // don't count this vowel as a syllable when finding the stress position
#define phLONG         0x200000
#define phLENGTHENSTOP 0x400000  // make the pre-pause slightly longer
#define phRHOTIC       0x800000  // bit 23
#define phNOPAUSE     0x1000000
#define phPREVOICE    0x2000000  // for voiced stops

#define phFLAG1      0x10000000
#define phFLAG2      0x20000000
#define phFLAG3      0x40000000
#define phLOCAL      0x80000000  // used during compilation

// fixed phoneme code numbers, these can be used from the program code
#define phonCONTROL     1
#define phonSTRESS_U    2
#define phonSTRESS_D    3
#define phonSTRESS_2    4
#define phonSTRESS_3    5
#define phonSTRESS_P    6
#define phonSTRESS_P2   7    // priority stress within a word
#define phonSTRESS_PREV 8
#define phonPAUSE       9
#define phonPAUSE_SHORT 10
#define phonPAUSE_NOLINK 11
#define phonLENGTHEN    12
#define phonSCHWA       13
#define phonSCHWA_SHORT 14
#define phonEND_WORD    15
#define phonDEFAULTTONE 17
#define phonCAPITAL     18
#define phonGLOTTALSTOP 19
#define phonSYLLABIC    20
#define phonSWITCH      21
#define phonX1          22      // a language specific action
#define phonPAUSE_VSHORT 23
#define phonPAUSE_LONG  24
#define phonT_REDUCED   25
#define phonSTRESS_TONIC 26
#define phonPAUSE_CLAUSE 27
#define phonVOWELTYPES   28  // 28 to 33

extern const unsigned char pause_phonemes[8];  // 0, vshort, short, pause, long, glottalstop

// place of articulation
#define phPLACE        0xf0000
#define phPLACE_blb    0x10000
#define phPLACE_pla    0x60000

#define N_PHONEME_TABS     100     // number of phoneme tables
#define N_PHONEME_TAB      256     // max phonemes in a phoneme table
#define N_PHONEME_TAB_NAME  32     // must be multiple of 4

// main table of phonemes, index by phoneme number (1-254)

typedef struct {
	unsigned int  mnemonic;      // Up to 4 characters.  The first char is in the l.s.byte
	unsigned int  phflags;       // bits 16-19 place of articulation
	unsigned short program;      // index into phondata file
	unsigned char  code;         // the phoneme number
	unsigned char  type;         // phVOWEL, phPAUSE, phSTOP etc
	unsigned char  start_type;
	unsigned char  end_type;
	unsigned char  std_length;   // for vowels, in mS/2;  for phSTRESS phonemes, this is the stress/tone type
	unsigned char  length_mod;   // a length_mod group number, used to access length_mod_tab

} PHONEME_TAB;



// Several phoneme tables may be loaded into memory. phoneme_tab points to
// one for the current voice
extern int n_phoneme_tab;
extern int current_phoneme_table;
extern PHONEME_TAB *phoneme_tab[N_PHONEME_TAB];
extern unsigned char phoneme_tab_flags[N_PHONEME_TAB];  // bit 0: not inherited

typedef struct {
	char name[N_PHONEME_TAB_NAME];
	PHONEME_TAB *phoneme_tab_ptr;
	int n_phonemes;
	int includes;            // also include the phonemes from this other phoneme table
	int equivalence_tables;   // lists of equivalent phonemes to match other languages, byte index into phondata
} PHONEME_TAB_LIST;



// table of phonemes to be replaced with different phonemes, for the current voice
#define N_REPLACE_PHONEMES   60
typedef struct {
	unsigned char old_ph;
	unsigned char new_ph;
	char type;   // 0=always replace, 1=only at end of word
} REPLACE_PHONEMES;

extern int n_replace_phonemes;
extern REPLACE_PHONEMES replace_phonemes[N_REPLACE_PHONEMES];


// Table of phoneme programs and lengths.  Used by MakeVowelLists
typedef struct {
	unsigned int addr;
	unsigned int length;
} PHONEME_PROG_LOG;



#define PH(c1,c2)  (c2<<8)+c1          // combine two characters into an integer for phoneme name
#define PH3(c1,c2,c3) (c3<<16)+(c2<<8)+c1
#define PhonemeCode2(c1,c2)  PhonemeCode((c2<<8)+c1)
int LookupPhonemeString(const char *string);
int PhonemeCode(unsigned int mnem);

const char *EncodePhonemes(const char *p, char *outptr, int *bad_phoneme);
void DecodePhonemes(const char *inptr, char *outptr);

extern const char *WordToString(unsigned int word);

extern PHONEME_TAB_LIST phoneme_tab_list[N_PHONEME_TABS];
extern int phoneme_tab_number;
