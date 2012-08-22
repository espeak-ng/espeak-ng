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

#define CTRL_EMBEDDED    0x01         // control character at the start of an embedded command
#define REPLACED_E       'E'          // 'e' replaced by silent e

#define N_WORD_PHONEMES  120          // max phonemes in a word
#define N_CLAUSE_WORDS   256          // max words in a clause
#define N_CHAINS2        120          // max num of two-letter rule chains
#define N_HASH_DICT     1024
#define N_CHARSETS        16

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

#define FLAG_XX1           0x80000  // language specific
#define FLAG_XX2          0x100000
#define FLAG_XX3          0x200000

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
#define FLAG_EMBEDDED      0x40   /* word is preceded by embedded commands */
#define FLAG_DONT_SWITCH_TRANSLATOR  0x1000

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
#define RULE_NOTVOWEL   24

// Punctuation types  returned by ReadClause()
// bits 0-7 pause x 10mS, bits 8-11 intonation type,
// bit 15=sentence, bit 14=clause,  bits 13=voice change
// bit 12 used to distinguish otherwise identical types
#define CLAUSE_BIT_SENTENCE  0x8000
#define CLAUSE_BIT_VOICE     0x2000

#define CLAUSE_NONE         0 + 0x0400
#define CLAUSE_PARAGRAPH   70 + 0x8000
#define CLAUSE_EOF         35 + 0x9000
#define CLAUSE_VOICE        0 + 0x2400
#define CLAUSE_PERIOD      35 + 0x8000
#define CLAUSE_COMMA       20 + 0x4100
#define CLAUSE_QUESTION    35 + 0x8200
#define CLAUSE_EXCLAMATION 40 + 0x8300
#define CLAUSE_COLON       30 + 0x4000
#ifdef PLATFORM_RISCOS
#define CLAUSE_SEMICOLON   30 + 0x4000
#else
#define CLAUSE_SEMICOLON   30 + 0x4100
#endif

#define SAYAS_GLYPHS    1
#define SAYAS_CHAR      2
#define SAYAS_KEY       3
#define SAYAS_DIGITS    4

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
	unsigned char synthflags;
	short sourceix;
} PHONEME_LIST2;


#define N_SPEECH_PARAM  8
typedef struct {
	int type;
	int parameter[N_SPEECH_PARAM];
} PARAM_STACK;

extern PARAM_STACK param_stack[];
extern const int param_defaults[N_SPEECH_PARAM];



#define N_LOPTS      9
#define LOPT_DIERESES        1
 // 1=remove [:] from unstressed syllables
#define LOPT_IT_LENGTHEN        2
 // 1=german
#define LOPT_PREFIXES        3
 // 1=regressive,  change voiced/unoiced to match last consonant in a cluster
#define LOPT_REGRESSIVE_VOICING  4
 // 0=default, 1=no check
#define LOPT_UNPRONOUNCABLE  5
 // 0=default, 1=set length_mod0 = length_mod
#define LOPT_FINAL_SYLLABLE  6
 // increase this to prevent sonorants being shortened before shortened (eg. unstressed) vowels
#define LOPT_SONORANT_MIN    7
 // Italian "syntactic doubling"
#define LOPT_IT_DOUBLING     8


typedef struct {
	int word_gap; // 0,  2=don't merge phonemes,  3= pause before stops and fricatives, 4= PAUSE_SHORT between words
	int vowel_pause;
	int stress_rule; // 1=first syllable, 2=penultimate,  3=last

// bit0=don't stress monosyllables,
// bit1=don't set diminished stress,
// bit2=mark unstressed final syllables as diminished
// bit3=stress last syllable if it doesn't end in vowel or "s" or "n" 
// bit4=don't allow secondary stress on last syllable
	int stress_flags; 
	int unstressed_wd1; // stress for $u word of 1 syllable
	int unstressed_wd2; // stress for $u word of >1 syllable
	int param[N_LOPTS];
	unsigned char *length_mods;
	unsigned char *length_mods0;
} LANGUAGE_OPTIONS;



class Translator
{//=============
public:
	Translator();
	virtual ~Translator();
	void *TranslateClause(FILE *f_text, const void *vp_input, int *tone, char **voice_change);
	int LoadDictionary(const char *name);
	void SetLetterBits(int group, const char *string);
	virtual void CalcLengths();
	virtual void CalcPitches(int clause_tone);
	
	LANGUAGE_OPTIONS langopts;
	char phon_out[300];

	int stress_amps[8];
	int stress_amps_r[8];
	int stress_lengths[8];
	int dict_condition;    // conditional apply some pronunciation rules and dict.lookups
	const unsigned short *charset_a0;   // unicodes for characters 0xa0 to oxff
	const wchar_t *char_plus_apostrophe;  // single chars + apostrophe treated as words

// holds properties of characters: vowel, consonant, etc for pronunciation rules
#define LETTERGP_VOWEL    0
#define LETTERGP_VOWEL_Y  7
	unsigned char letter_bits[256];
	int letter_bits_offset;

private:
	int TranslateWord(char *word, int next_pause, int wflags);
	int TranslateWord2(char *word, int wflags, int pre_pause, int next_pause, int source_ix);
	int TranslateLetter(char *letter, char *phonemes);
	void GetTranslatedPhonemeString(char *phon_out, int n_phon_out);
	void WriteMnemonic(int *ix, int mnem);
	void MakePhonemeList(int post_pause, int embedded, int new_sentence);

	int ReadClause(FILE *f_in, char *buf, unsigned short *charix, int n_buf);
	int AnnouncePunctuation(int c1, int c2, char *buf, int ix);

	int LookupDict2(char *word, char *word2, char *phonetic, unsigned int *flags, int end_flags);
	int LookupDictList(char *word1, char *ph_out, unsigned int *flags, int end_flags);
	const char *LookupSpecial(char *string);
	const char *LookupCharName(int c);

	void InitGroups(void);
	void AppendPhonemes(char *string, const char *ph);
	char *DecodeRule(const char *group, char *rule);
	void MatchRule(char *word[], const char *group, char *rule, MatchRecord *match_out, int end_flags);
	int TranslateRules(char *p, char *phonemes, char *end_phonemes, int end_flags);

	int IsLetter(int letter, int group);
	int GetVowelStress(unsigned char *phonemes, char *vowel_stress, int &vowel_count, int &stressed_syllable);

protected:
	virtual int Unpronouncable(char *word);
	virtual void SetWordStress(char *output, unsigned int dictionary_flags, int tonic, int prev_stress);
	virtual int RemoveEnding(char *word, int end_type);
	virtual int TranslateChar(char *ptr, int prev_in, int c, int next_in);
	virtual int SubstitutePhonemes(PHONEME_LIST2 *plist_out);

	int IsVowel(int letter);

	char *data_dictrules;     // language_1   translation rules file
	char *data_dictlist;      // language_2   dictionary lookup file
	char *dict_hashtab[N_HASH_DICT];   // hash table to index dictionary lookup file
	
	// groups1 and groups2 are indexes into data_dictrules, set up by InitGroups()
	// the two-letter rules for each letter must be consecutive in the language_rules source
	
	char *groups1[256];         // translation rule lists, index by single letter
	char *groups2[N_CHAINS2];   // translation rule lists, indexed by two-letter pairs
	unsigned int groups2_name[N_CHAINS2];  // the two letter pairs for groups2[]
	int n_groups2;              // number of groups2[] entries used
	
	char groups2_count[256];    // number of 2 letter groups for this initial letter
	char groups2_start[256];    // index into groups2
	

	int n_ph_list2;
	PHONEME_LIST2 ph_list2[N_PHONEME_LIST];	// first stage of text->phonemes

	char word_phonemes[N_WORD_PHONEMES];    // a word translated into phoneme codes
	

	int expect_verb;
	int expect_past;    // expect past tense
	int expect_verb_s;
	int word_flags;     // word is all upper case
	int prev_last_stress;
	int prepause_timeout;
	int end_stressed_vowel;  // word ends with stressed vowel
	int prev_dict_flags;     // dictionary flags from previous word
	char *clause_end;

	int word_vowel_count;     // number of vowels so far
	int word_stressed_count;  // number of vowels so far which could be stressed
	
	int clause_upper_count;   // number of upper case letters in the clause
	int clause_lower_count;   // number of lower case letters in the clause


}; //  end of class Translator


extern int option_tone1;
extern int option_tone2;
extern int option_waveout;
extern int option_phonemes;
extern int option_log_trans;      // log pronunciation translation
extern int option_linelength;     // treat lines shorter than this as end-of-clause
extern int option_harmonic1;
extern int option_multibyte;
extern int option_capitals;
extern int option_punctuation;
extern int option_endpause;
extern int option_ssml;
extern int option_phoneme_input;   // allow [[phonemes]] in input text
extern int option_phoneme_variants;

extern int count_characters;
extern int count_words;
extern int count_sentences;
extern int skip_characters;
extern int skip_words;
extern int skip_sentences;
extern int skipping_text;
extern int end_character_position;
extern int clause_start_char;
extern int clause_start_word;
extern char *namedata;

#define N_MARKER_LENGTH 50   // max.length of a mark name
extern char skip_marker[N_MARKER_LENGTH];

#define N_PUNCTLIST  60
extern wchar_t option_punctlist[N_PUNCTLIST];

#define espeakSILENCE   0
#define espeakEMPHASIS  7
extern int speech_parameters[];

extern Translator *translator;
extern const unsigned short *charsets[N_CHARSETS];
extern char dictionary_name[40];
extern char ctrl_embedded;    // to allow an alternative CTRL for embedded commands
extern char *p_textinput;
extern wchar_t *p_wchar_input;
extern int ungot_char;
extern int (* uri_callback)(int, const char *, const char *);

Translator *SelectTranslator(const char *name);
int CompileDictionary(const char *dict_name, int log, char *err_name);
void LoadConfig(void);
int utf8_in(int *c, char *buf, int backwards);
int utf8_out(unsigned int c, char *buf);
int lookupwchar(const short *list,int c);
int Eof(void);
char *strchr_w(const char *s, int c);
int IsBracket(int c);
void InitText(void);
void InitText2(void);

extern FILE *f_trans;		// for logging
