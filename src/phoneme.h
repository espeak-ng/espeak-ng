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

#define tNUMBER   1
#define tSTRING   2
#define tPHONEMEMNEM  3
#define tKEYWORD  4

#define tPHONEMESTART 1
#define tEND      2
#define tSPECT    3
#define tWAVE   4
#define tSTARTTYPE 5
#define tENDTYPE  6
#define tBEFORE   7
#define tAFTER    8
#define tLENGTHMOD 11
#define tLENGTH   12
#define tLONGLENGTH 13
#define tOLDNAME  14
#define tREDUCETO 15
#define tFIXEDCODE 16
#define tBEFOREVOWEL 17
#define tBEFOREVOWELPAUSE 18
#define tBEFORENOTVOWEL 19
#define tLINKOUT  20
#define tPHONEMETABLE  30
#define tINCLUDE  31


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
#define phINVALID 15


// phoneme properties
//   bits 12-15 give place of articulation (not currently used)
#define phWAVE  0x01
#define phUNSTRESSED 0x02
#define phFORTIS  0x08
#define phVOICED  0x10
#define phSIBILANT 0x20
#define phNOLINK   0x40
#define phMODULATE 0x80

#define phNOTFOLLOWS     0x0400
#define phORPAUSEFOLLOWS 0x0800

// fixed phoneme code numbers, these can be used from the program code
#define phonCONTROL     1
#define phonSTRESS_U    2
#define phonSTRESS_D    3
#define phonSTRESS_2    4
#define phonSTRESS_3    5
#define phonSTRESS_P    6
#define phonSTRESS_TONIC 7
#define phonSTRESS_PREV 8
#define phonPAUSE       9
#define phonPAUSE_SHORT 10
#define phonPAUSE_LONG  11
#define phonLENGTHEN    12
#define phonSCHWA       13
#define phonSCHWA_SHORT 14
#define phonEND_WORD    15
#define phonSONORANT    16



#define N_PHONEME_TABS      20
#define N_PHONEME_TAB      256
#define N_PHONEME_TAB_NAME  32

// main table of phonemes, index by phoneme number (1-254)
typedef struct {
	unsigned char  code;
	unsigned char  type;          /* vowel, etc */
	unsigned char  start_type;
	unsigned char  end_type;
	
	unsigned char  length_mod;
	unsigned char  reduce_to;
	unsigned char  vowel_follows;  // change to this if a vowel follows
	unsigned char  link_out;       // insert linking phoneme if a vowel follows
	short std_length;              // for vowels, in mS
	unsigned short  spect;
	unsigned short  before;
	unsigned short  after;
	
	unsigned int mnemonic;      /* 1st char in l.s.byte */
	unsigned int flags;
} PHONEME_TAB;

// Several phoneme tables may be loaded into memory. phoneme_tab points to
// one for the current voice
extern PHONEME_TAB *phoneme_tab;



// table of phonemes to be replaced with different phonemes, for the current voice
#define N_REPLACE_PHONEMES   40
typedef struct {
	unsigned char old_ph;
	unsigned char new_ph;
	char type;   // 0=always replace, 1=only at end of word
} REPLACE_PHONEMES;

extern int n_replace_phonemes;
extern REPLACE_PHONEMES replace_phonemes[N_REPLACE_PHONEMES];



extern char *EncodePhonemes(char *p, char *outptr, char *bad_phoneme);
extern void DecodePhonemes(const char *inptr, char *outptr);
extern const char *PhonemeTabName(void);

