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

#define N_WORD_PHONEMES  120          // max phonemes in a word
#define N_CLAUSE_WORDS   256          // max words in a clause
#define N_CHAINS2        120          // max num of two-letter rule chains
#define N_HASH_DICT     1024


/* flags from word dictionary */
#define FLAG_PREPAUSE        0x100
#define FLAG_ONLY            0x200
#define FLAG_ONLY_S          0x400
#define FLAG_STRESS_END      0x800  /* full stress if at end of clause */
#define FLAG_STRESS_END2    0x1000  /* full stress if at end of clause, or only followed by unstressed */
#define FLAG_UNSTRESS_END   0x2000  /* reduce stress at end of clause */
#define FLAG_ATEND          0x4000  /* use this pronunciation if at end of clause */

#define FLAG_CAPITAL        0x8000  /* pronunciation if initial letter is upepr case */
#define FLAG_DOT           0x10000  /* ignore '.' after word (abbreviation) */
#define FLAG_ABBREV        0x20000  /* use this pronunciation rather than split into letters */

#define FLAG_VERBF        0x400000  /* verb follows */
#define FLAG_VERBSF       0x800000  /* verb follows, may have -s suffix */
#define FLAG_NOUNF       0x1000000  /* noun follows */
#define FLAG_VERB        0x2000000  /* pronunciation for verb */
#define FLAG_PAST        0x4000000  /* pronunciation for past tense */
#define FLAG_PASTF       0x8000000  /* past tense follows */
#define FLAG_VERB_EXT   0x10000000  /* extend the 'verb follows' */

#define FLAG_SKIPWORDS  0x20000000  /* bits 29,30  number of words to skip */
#define FLAG_FOUND      0x80000000  /* pronunciation was found in the dictionary list */

// wordflags, flags in source word
#define FLAG_ALL_UPPER     0x1    /* no lower case letters in the word */
#define FLAG_FIRST_UPPER   0x2    /* first letter is upper case */
#define FLAG_HAS_PLURAL    0x4    /* upper-case word with s or 's lower-case ending */
#define FLAG_PHONEMES      0x8    /* word is phonemes */
#define FLAG_LAST_WORD    0x10    /* last word in clause */
#define FLAG_STRESSED_WORD  0x20    /* this word has explicit stress */

// prefix/suffix flags
#define SUFX_E        0x0100   // e may have been added
#define SUFX_I        0x0200   // y may have been changed to i
#define SUFX_P        0x0400   // prefix
#define SUFX_V        0x0800   // suffix means use the verb form pronunciation
#define SUFX_D        0x1000   // previous letter may have been doubles
#define SUFX_F        0x2000   // verb follows

#define FLAG_SUFX       0x04
#define FLAG_SUFX_S     0x08
#define FLAG_SUFX_E_ADDED 0x10


// codes in dictionary rules
#define RULE_PRE			1
#define RULE_POST			2
#define RULE_PHONEMES	3
#define RULE_PH_COMMON	4	// At start of rule. Its phoneme string is used by subsequent rules
#define RULE_CONDITION	5	// followed by condition number (byte)
#define RULE_GROUP_START 6
#define RULE_GROUP_END	7

#define RULE_SPACE		32   // ascii space
#define RULE_SYLLABLE	9
#define RULE_STRESSED	10
#define RULE_DOUBLE		11
#define RULE_INC_SCORE	12
#define RULE_DEL_FWD		13
#define RULE_ENDING		14
#define RULE_DIGIT		15
#define RULE_NONALPHA	16
#define RULE_LETTER1		17   // A vowels
#define RULE_LETTER2		18   // B 'hard' consonants 
#define RULE_LETTER3		19   // C all consonants
#define RULE_LETTER4		20   // E spare
#define RULE_LETTER5    21   // F spare
#define RULE_LETTER6    22   // G spare
#define RULE_NO_SUFFIX  23


// Rule:
// [4] [match] [1 pre] [2 post] [3 phonemes] 0
//     match 1 pre 2 post 0     - use common phoneme string
//     match 1 pre 2 post 3 0   - empty phoneme string


typedef struct {
	int points;
	const char *phonemes;
	int end_type;
	char *del_fwd;
} MatchRecord;
	

// used to mark words with the source[] buffer
typedef struct{
	unsigned short start;
	unsigned short sourceix;
	unsigned char pre_pause;
	unsigned char flags;
} WORD_TAB;

// a clause translated into phoneme codes (first stage)
typedef struct {
	unsigned char phcode;
	unsigned char stress;
	unsigned char tone_number; 
	short sourceix;
} PHONEME_LIST2;





class Translator
{//=============
public:
	Translator();
	virtual ~Translator();
	char *TranslateClause(FILE *f_text, char *buf, int *tone);
	int LoadDictionary(const char *name);
	virtual void CalcLengths();
	virtual void CalcPitches(int clause_tone);
	
	char *input_start;
	char phon_out[300];
	FILE *f_input;

protected:
	int TranslateWord(char *word, int next_pause, int wflags);
	int TranslateWord2(char *word, int wflags, int pre_pause, int next_pause, int source_ix);
	void TranslateLetter(char letter, char *phonemes);
	void GetTranslatedPhonemeString(char *phon_out, int n_phon_out);
	void MakePhonemeList(int post_pause);
	
	char GetC(void);
	void UngetC(char c);
	int Eof(void);
	int Pos(void);
	char *p_input;
	char ungot_char;
	int end_of_input;

	virtual int Unpronouncable(char *word);
	virtual void SetWordStress(char *output, unsigned int dictionary_flags, int tonic, int prev_stress);
	virtual int RemoveEnding(char *word, int end_type);

	int ReadClause(FILE *f_in, char *buf, int n_buf);

	int LookupDict2(char *word, char *word2, char *phonetic, unsigned int *flags, int end_flags);
	int LookupDictList(char *word1, char *ph_out, unsigned int *flags, int end_flags);

	void InitGroups(void);
	void AppendPhonemes(char *string, const char *ph);
	char *DecodeRule(const char *group, char *rule);
	void MatchRule(char *word[], const char *group, char *rule, MatchRecord *match_out, int end_flags);
	int TranslateRules(char *p, char *phonemes, char *end_phonemes, int end_flags);

	int IsLetter(unsigned char letter, int group);
	int IsVowel(unsigned char letter);
	void SetLetterBits(int group, const char *string);
	int GetVowelStress(char *phonemes, char *vowel_stress, int *vowel_count, int stressed_syllable);

   char prev_locale[40];	// previous value of locale before this translator was created

	char source[300];     // the source text of a single clause

	int n_ph_list2;
	PHONEME_LIST2 ph_list2[N_PHONEME_LIST];	// first stage of text->phonemes

	char word_phonemes[N_WORD_PHONEMES];    // a word translated into phoneme codes
	

	int expect_verb;
	int expect_past;    // expect past tense
	int expect_verb_s;
	int word_flags;     // word is all upper case
	int prev_last_stress;
	int prepause_timeout;
	char *clause_end;
	int clause_start_index;

	int word_vowel_count;     // number of vowels so far
	int word_stressed_count;  // number of vowels so far which could be stressed
	
	int clause_upper_count;   // number of upper case letters in the clause
	int clause_lower_count;   // number of lower case letters in the clause

	int dict_condition;    // conditional apply some pronunciation rules and dict.lookups

}; //  end of class Translator


class Translator_Default: public Translator
{//========================================


};  // end of class Translator_German



class Translator_English: public Translator
{//=======================================

public:
	Translator_English();
	int Unpronouncable(char *word);

};  // end of class Translator_English



class Translator_Esperanto: public Translator
{//==========================================

public:
	Translator_Esperanto();

};  // end of class Translator_Esperanto


class Translator_German: public Translator
{//=======================================

public:
	Translator_German();

//	void SetWordStress(char *output, unsigned int dictionary_flags, int tonic, int prev_stress);
//	int Unpronouncable(char *word);

};  // end of class Translator_German


class Translator_Tone: public Translator
{//==========================================

public:
	Translator_Tone();

private:
	void CalcPitches(int clause_tone);

};  // end of class Translator_Tone



// holds properties of characters: vowel, consonant, etc for pronunciation rules
#define LETTERGP_VOWEL    0
#define LETTERGP_VOWEL_Y  7
extern unsigned char letter_bits[256];


extern int stress_lengths[8];
extern int stress_amps[8];
extern int stress_amps_r[8];
extern int option_tone1;
extern int option_tone2;
extern int option_words;          // 0,  1=don't merge phonemes,  
                                  // 2= pause before stops and fricatives, 3= PAUSE_SHORT between words
extern int option_vowel_pause;    // 1=pause before words starting with vowel
extern int option_stress_rule;    // 1=first syllable, 2=penultimate,  3=last
extern int option_unstressed_wd1; // stress for $u word of 1 syllable
extern int option_unstressed_wd2; // stress for $u word of >1 syllable
extern int option_echo_delay;
extern int option_echo_amp;
extern int option_amplitude;
extern int option_waveout;
extern int option_phonemes;
extern int option_log_trans;      // log pronunciation translation
extern int option_linelength;     // treat lines shorter than this as end-of-clause

extern Translator *translator;
extern char dictionary_name[40];
extern int LoadVoice(char *voice_name, int reset);
extern void CompileDictionary(const char *dict_name, int log);
extern void strncpy0(char *to,const char *from, int size);

extern FILE *f_trans;		// for logging
