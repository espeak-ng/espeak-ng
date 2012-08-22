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

#define N_WORD_PHONEMES  150          // max phonemes in a word
#define N_CLAUSE_WORDS   256          // max words in a clause
#define N_CHAINS2        120          // max num of two-letter rule chains
#define N_HASH_DICT     1024
#define N_CHARSETS        19

/* flags from word dictionary */
// bits 0-3  stressed syllable,  7=unstressed
#define FLAG_SKIPWORDS        0x20  /* bits 5,6,7  number of words to skip */
#define FLAG_PREPAUSE        0x100
#define FLAG_ONLY            0x200
#define BITNUM_FLAG_ONLY         9  // bit 9 is set
#define FLAG_ONLY_S          0x400
#define FLAG_STRESS_END      0x800  /* full stress if at end of clause */
#define FLAG_STRESS_END2    0x1000  /* full stress if at end of clause, or only followed by unstressed */
#define FLAG_UNSTRESS_END   0x2000  /* reduce stress at end of clause */
#define FLAG_ATEND          0x4000  /* use this pronunciation if at end of clause */

#define FLAG_CAPITAL        0x8000  /* pronunciation if initial letter is upper case */
#define FLAG_DOT           0x10000  /* ignore '.' after word (abbreviation) */
#define FLAG_ABBREV        0x20000  /* use this pronunciation rather than split into letters */
#define FLAG_STEM          0x40000  // must have a suffix

#define FLAG_DOUBLING      0x80000  // doubles the following consonant
#define FLAG_XX2          0x100000  // language specific
#define FLAG_XX3          0x200000

#define FLAG_VERBF        0x400000  /* verb follows */
#define FLAG_VERBSF       0x800000  /* verb follows, may have -s suffix */
#define FLAG_NOUNF       0x1000000  /* noun follows */
#define FLAG_VERB        0x2000000  /* pronunciation for verb */
#define FLAG_PAST        0x4000000  /* pronunciation for past tense */
#define FLAG_PASTF       0x8000000  /* past tense follows */
#define FLAG_VERB_EXT   0x10000000  /* extend the 'verb follows' */

#define FLAG_PAUSE1     0x40000000  // shorter prepause
#define FLAG_FOUND      0x80000000  /* pronunciation was found in the dictionary list */

// wordflags, flags in source word
#define FLAG_ALL_UPPER     0x1    /* no lower case letters in the word */
#define FLAG_FIRST_UPPER   0x2    /* first letter is upper case */
#define FLAG_HAS_PLURAL    0x4    /* upper-case word with s or 's lower-case ending */
#define FLAG_PHONEMES      0x8    /* word is phonemes */
#define FLAG_LAST_WORD     0x10   /* last word in clause */
#define FLAG_STRESSED_WORD 0x20   /* this word has explicit stress */
#define FLAG_EMBEDDED      0x40   /* word is preceded by embedded commands */
#define FLAG_HYPHEN        0x80
#define FLAG_DONT_SWITCH_TRANSLATOR  0x1000

// prefix/suffix flags
#define SUFX_E        0x0100   // e may have been added
#define SUFX_I        0x0200   // y may have been changed to i
#define SUFX_P        0x0400   // prefix
#define SUFX_V        0x0800   // suffix means use the verb form pronunciation
#define SUFX_D        0x1000   // previous letter may have been doubles
#define SUFX_F        0x2000   // verb follows
#define SUFX_Q        0x4000   // don't retranslate

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
#define RULE_LETTER4		20   // H spare
#define RULE_LETTER5    21   // F spare
#define RULE_LETTER6		22   // G spare
#define RULE_LETTER7    23   // Y spare
#define RULE_NO_SUFFIX  24
#define RULE_NOTVOWEL   25

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

#define SAYAS_CHARS     0x12
#define SAYAS_GLYPHS    0x13
#define SAYAS_SINGLE_CHARS 0x14
#define SAYAS_KEY       0x20
#define SAYAS_DIGITS1   0x21
#define SAYAS_DIGITS    0x30  // + number of digits

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
	unsigned char wmark;
	unsigned char length;
} WORD_TAB;

// a clause translated into phoneme codes (first stage)
typedef struct {
	unsigned char phcode;
	unsigned char stress;
	unsigned char tone_number; 
	unsigned char synthflags;
	unsigned short sourceix;
} PHONEME_LIST2;


#define N_SPEECH_PARAM  8
typedef struct {
	int type;
	int parameter[N_SPEECH_PARAM];
} PARAM_STACK;

extern PARAM_STACK param_stack[];
extern const int param_defaults[N_SPEECH_PARAM];



#define N_LOPTS      11
#define LOPT_DIERESES        1
 // 1=remove [:] from unstressed syllables
#define LOPT_IT_LENGTHEN        2
 // 1=german
#define LOPT_PREFIXES        3
 // 1=regressive,  change voiced/unoiced to match last consonant in a cluster
#define LOPT_REGRESSIVE_VOICING  4
 // 0=default, 1=no check, other allow this character as an extra initial letter (default is 's')
#define LOPT_UNPRONOUNCABLE  5
 // 0=default, 1=set length_mod0 = length_mod
#define LOPT_FINAL_SYLLABLE  6
 // increase this to prevent sonorants being shortened before shortened (eg. unstressed) vowels
#define LOPT_SONORANT_MIN    7
 // Italian "syntactic doubling"
#define LOPT_IT_DOUBLING     8
 // max. amplitude for vowel at the end of a clause
#define LOPT_MAXAMP_EOC      9
 // don't reduce the strongest vowel in a word which is marked 'unstressed'
#define LOPT_KEEP_UNSTR_VOWEL  10

typedef struct {
	int word_gap; // 0,  2=don't merge phonemes,  3= pause before stops and fricatives, 4= PAUSE_SHORT between words
	int vowel_pause;
	int stress_rule; // 1=first syllable, 2=penultimate,  3=last

// bit0=don't stress monosyllables,
// bit1=don't set diminished stress,
// bit2=mark unstressed final syllables as diminished
// bit3=stress last syllable if it doesn't end in vowel or "s" or "n"  LANG=Spanish
// bit4=don't allow secondary stress on last syllable
// bit5-don't use automatic secondary stress
// bit6=light syllable followed by heavy, move secondary stress to the heavy syllable. LANG=Finnish
	int stress_flags; 
	int unstressed_wd1; // stress for $u word of 1 syllable
	int unstressed_wd2; // stress for $u word of >1 syllable
	int param[N_LOPTS];
	unsigned char *length_mods;
	unsigned char *length_mods0;

	// bits0-2=which numbers routine to use. 0=none, 1=english/german
	// bit3=  . , for thousands and decimal separator
	// bit4=use three-and-twenty rather than twenty-three
	// bit5='and' between tens and units
	// bit6=add "and" after hundred and thousand
	// bit7=don't have "and" both after hundreds and also between tens and units
   // bit8=only one primary stress in tens+units
	// bit9=only one vowel betwen tens and units
	// bit10=omit "one" before "hundred"
	// bit12=allow space as thousands separator (in addition to langopts.thousands_sep)
	// bit13=(LANG=it) speak post-decimal-point digits as a combined number not as single digits
	// bit16=dot after number indicates ordinal
	int numbers;
	int thousands_sep;
	int decimal_sep;
	int intonation;    // 1=tone language
	int long_stop;     // extra mS pause for a lengthened stop
	int phoneme_change;  // TEST, change phonemes, after translation
	char max_initial_consonants;
	char spelling_stress;   // 0=default, 1=stress first letter
	int testing;   // testing options: bit 1= specify stressed syllable in the form:  "outdoor/2"
} LANGUAGE_OPTIONS;


#define NUM_SEP_DOT    0x0008    // . , for thousands and decimal separator
#define NUM_SEP_SPACE  0x1000    // allow space as thousands separator (in addition to langopts.thousands_sep)
#define NUM_DEC_IT     0x2000    // (LANG=it) speak post-decimal-point digits as a combined number not as single digits

class Translator
{//=============
public:
	Translator();
	virtual ~Translator();
	void *TranslateClause(FILE *f_text, const void *vp_input, int *tone, char **voice_change);
	int TranslateWord(char *word, int next_pause, int wflags, int wmark);
	int LoadDictionary(const char *name);
	void SetLetterBits(int group, const char *string);
	virtual void CalcLengths();
	virtual void CalcPitches(int clause_tone);
	
	LANGUAGE_OPTIONS langopts;
	int transpose_offset;
	int transpose_max;
	int transpose_min;

	char phon_out[300];
	char word_phonemes[N_WORD_PHONEMES];    // a word translated into phoneme codes

	int stress_amps[8];
	int stress_amps_r[8];
	int stress_lengths[8];
	int dict_condition;    // conditional apply some pronunciation rules and dict.lookups
	const unsigned short *charset_a0;   // unicodes for characters 0xa0 to oxff
	const wchar_t *char_plus_apostrophe;  // single chars + apostrophe treated as words

// holds properties of characters: vowel, consonant, etc for pronunciation rules
#define LETTERGP_VOWEL    0
#define LETTERGP_VOWEL2   7
	unsigned char letter_bits[256];
	int letter_bits_offset;

private:
	int TranslateWord2(char *word, int wflags, int pre_pause, int next_pause, int source_ix, int len, int wmark);
	int TranslateLetter(char *letter, char *phonemes, int control);
	void SetSpellingStress(char *phonemes, int control);
	void GetTranslatedPhonemeString(char *phon_out, int n_phon_out);
	void WriteMnemonic(int *ix, int mnem);
	void MakePhonemeList(int post_pause, int embedded, int new_sentence);
	int SubstitutePhonemes(PHONEME_LIST2 *plist_out);

	int ReadClause(FILE *f_in, char *buf, unsigned short *charix, int n_buf);
	int AnnouncePunctuation(int c1, int c2, char *buf, int ix);

	int LookupDict2(char *word, char *word2, char *phonetic, unsigned int *flags, int end_flags);
	const char *LookupSpecial(char *string);
	const char *LookupCharName(int c);
	int LookupNum2(int value, int control, char *ph_out);
	int LookupNum3(int value, int suppress_null, int thousandplex, char *ph_out);
	int LookupNum3(int value, char *ph_out, int suppress_null, int thousandplex, int prev_thousands);
	int LookupThousands(int value, int thousandplex, char *ph_out);
   int TranslateNumber_1(char *word1, char *ph_out, unsigned int *flags, int wflags);

	void InitGroups(void);
	void AppendPhonemes(char *string, const char *ph);
	char *DecodeRule(const char *group, char *rule);
	void MatchRule(char *word[], const char *group, char *rule, MatchRecord *match_out, int end_flags);
	int TranslateRules(char *p, char *phonemes, char *end_phonemes, int end_flags);

	int IsLetter(int letter, int group);

	void CalcPitches_Tone(int clause_tone);

protected:
	virtual int Unpronouncable(char *word);
	virtual void SetWordStress(char *output, unsigned int dictionary_flags, int tonic, int prev_stress);
	virtual int RemoveEnding(char *word, int end_type);
	virtual int TranslateChar(char *ptr, int prev_in, int c, int next_in);
   virtual int TranslateNumber(char *word1, char *ph_out, unsigned int *flags, int wflags);
	virtual int ChangePhonemes(PHONEME_LIST2 *phlist, int n_ph, int index, PHONEME_TAB *ph, int flags);

	int IsVowel(int letter);
	int LookupDictList(char *word1, char *ph_out, unsigned int *flags, int end_flags);
	int Lookup(char *word, char *ph_out);

	char *data_dictrules;     // language_1   translation rules file
	char *data_dictlist;      // language_2   dictionary lookup file
	char *dict_hashtab[N_HASH_DICT];   // hash table to index dictionary lookup file
	
	// groups1 and groups2 are indexes into data_dictrules, set up by InitGroups()
	// the two-letter rules for each letter must be consecutive in the language_rules source
	
	char *groups1[256];         // translation rule lists, index by single letter
	char *groups2[N_CHAINS2];   // translation rule lists, indexed by two-letter pairs
	unsigned int groups2_name[N_CHAINS2];  // the two letter pairs for groups2[]
	int n_groups2;              // number of groups2[] entries used
	
	unsigned char groups2_count[256];    // number of 2 letter groups for this initial letter
	unsigned char groups2_start[256];    // index into groups2
	

	int n_ph_list2;
	PHONEME_LIST2 ph_list2[N_PHONEME_LIST];	// first stage of text->phonemes

	

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
extern int option_linelength;     // treat lines shorter than this as end-of-clause
extern int option_harmonic1;
extern int option_multibyte;
extern int option_capitals;
extern int option_punctuation;
extern int option_endpause;
extern int option_ssml;
extern int option_phoneme_input;   // allow [[phonemes]] in input text
extern int option_phoneme_variants;
extern int option_sayas;

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
int CompileDictionary(const char *dsource, const char *dict_name, FILE *log, char *err_name);
void LoadConfig(void);
int PhonemeCode(unsigned int mnem);
void ChangeWordStress(char *word, int new_stress);
int TransposeAlphabet(char *text, int offset, int min, int max);
int utf8_in(int *c, char *buf, int backwards);
int utf8_out(unsigned int c, char *buf);
int lookupwchar(const short *list,int c);
int Eof(void);
char *strchr_w(const char *s, int c);
int IsBracket(int c);
void InitText(void);
void InitText2(void);

extern FILE *f_trans;		// for logging
