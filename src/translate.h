/***************************************************************************
 *   Copyright (C) 2005 to 2007 by Jonathan Duddington                     *
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


#define L(c1,c2)  (c1<<8)+c2          // combine two characters into an integer for translator name 

#define CTRL_EMBEDDED    0x01         // control character at the start of an embedded command
#define REPLACED_E       'E'          // 'e' replaced by silent e

#define N_WORD_PHONEMES  160          // max phonemes in a word
#define N_WORD_BYTES     160          // max bytes for the UTF8 characters in a word
#define N_CLAUSE_WORDS   300          // max words in a clause
#define N_RULE_GROUP2    120          // max num of two-letter rule chains
#define N_HASH_DICT     1024
#define N_CHARSETS        20
#define N_LETTER_GROUPS   26


/* dictionary flags, word 1 */
// bits 0-3  stressed syllable,  bit 6=unstressed
#define FLAG_SKIPWORDS        0x80
#define FLAG_PREPAUSE        0x100

#define FLAG_ONLY            0x200
#define FLAG_ONLY_S          0x400
#define BITNUM_FLAG_ONLY         9  // bit 9 is set
#define BITNUM_FLAG_ONLY_S      10  // bit 10 is set

#define FLAG_STRESS_END      0x800  /* full stress if at end of clause */
#define FLAG_STRESS_END2    0x1000  /* full stress if at end of clause, or only followed by unstressed */
#define FLAG_UNSTRESS_END   0x2000  /* reduce stress at end of clause */
#define FLAG_ATEND          0x4000  /* use this pronunciation if at end of clause */
#define FLAG_SPELLWORD      0x8000  // re-translate the word as individual letters, separated by spaces

#define FLAG_DOT           0x10000  /* ignore '.' after word (abbreviation) */
#define FLAG_ABBREV        0x20000  // spell as letters, even with a vowel, OR use specified pronunciation rather than split into letters
#define FLAG_STEM          0x40000  // must have a suffix

#define FLAG_DOUBLING      0x80000  // doubles the following consonant
#define FLAG_ALT_TRANS    0x100000  // language specific
#define FLAG_ALT2_TRANS   0x200000  // language specific

#define FLAG_MAX3       0x08000000  // limit to 3 repeats
#define FLAG_PAUSE1     0x10000000  // shorter prepause
#define FLAG_TEXTMODE   0x20000000  // word translates to replacement text, not phonemes
#define BITNUM_FLAG_TEXTMODE    29

#define FLAG_FOUND_ATTRIBUTES     0x40000000  // word was found in the dictionary list (has attributes)
#define FLAG_FOUND      0x80000000  // pronunciation was found in the dictionary list

// dictionary flags, word 2
#define FLAG_VERBF             0x1  /* verb follows */
#define FLAG_VERBSF            0x2  /* verb follows, may have -s suffix */
#define FLAG_NOUNF             0x4  /* noun follows */
#define FLAG_PASTF             0x8  /* past tense follows */
#define FLAG_VERB             0x10  /* pronunciation for verb */
#define FLAG_NOUN             0x20  /* pronunciation for noun */
#define FLAG_PAST             0x40  /* pronunciation for past tense */
#define FLAG_VERB_EXT        0x100  /* extend the 'verb follows' */
#define FLAG_CAPITAL         0x200  /* pronunciation if initial letter is upper case */
#define FLAG_ALLCAPS         0x400  // only if the word is all capitals
#define BITNUM_FLAG_ALLCAPS   0x2a
#define FLAG_ACCENT          0x800  // character name is base-character name + accent name
#define FLAG_HYPHENATED     0x1000  // multiple-words, but needs hyphen between parts 1 and 2
#define BITNUM_FLAG_HYPHENATED  0x2c


// wordflags, flags in source word
#define FLAG_ALL_UPPER     0x1    /* no lower case letters in the word */
#define FLAG_FIRST_UPPER   0x2    /* first letter is upper case */
#define FLAG_UPPERS        0x3    // FLAG_ALL_UPPER | FLAG_FIRST_UPPER
#define FLAG_HAS_PLURAL    0x4    /* upper-case word with s or 's lower-case ending */
#define FLAG_PHONEMES      0x8    /* word is phonemes */
#define FLAG_LAST_WORD     0x10   /* last word in clause */
#define FLAG_EMBEDDED      0x40   /* word is preceded by embedded commands */
#define FLAG_HYPHEN        0x80
#define FLAG_NOSPACE       0x100  // word is not seperated from previous word by a space
#define FLAG_FIRST_WORD    0x200  // first word in clause
#define FLAG_FOCUS         0x400   // the focus word of a clause
#define FLAG_EMPHASIZED    0x800
#define FLAG_EMPHASIZED2   0xc00  // FLAG_FOCUS | FLAG_EMPHASIZED
#define FLAG_DONT_SWITCH_TRANSLATOR  0x1000
#define FLAG_SUFFIX_REMOVED  0x2000
#define FLAG_HYPHEN_AFTER    0x4000

#define FLAG_NO_TRACE      0x10000   // passed to TranslateRules() to suppress dictionary lookup printout
#define FLAG_NO_PREFIX     0x20000

// prefix/suffix flags (bits 8 to 14, bits 16 to 22) don't use 0x8000, 0x800000
#define SUFX_E        0x0100   // e may have been added
#define SUFX_I        0x0200   // y may have been changed to i
#define SUFX_P        0x0400   // prefix
#define SUFX_V        0x0800   // suffix means use the verb form pronunciation
#define SUFX_D        0x1000   // previous letter may have been doubles
#define SUFX_F        0x2000   // verb follows
#define SUFX_Q        0x4000   // don't retranslate
#define SUFX_T        0x10000   // don't affect the stress position in the stem
#define SUFX_B        0x20000  // break, this character breaks the word into stem and suffix (used with SUFX_P)

#define FLAG_ALLOW_TEXTMODE  0x02  // allow dictionary to translate to text rather than phonemes
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
#define RULE_LINENUM		8  // next 2 bytes give a line number, for debugging purposes

#define RULE_SPACE		32   // ascii space
#define RULE_SYLLABLE	9
#define RULE_STRESSED	10
#define RULE_DOUBLE		11
#define RULE_INC_SCORE	12
#define RULE_DEL_FWD		13
#define RULE_ENDING		14
#define RULE_DIGIT		15   // D digit
#define RULE_NONALPHA	16   // Z non-alpha
#define RULE_LETTERGP   17   // A B C H F G Y   letter group number
#define RULE_LETTERGP2  18   // L + letter group number
#define RULE_CAPITAL    19   //    word starts with a capital letter
#define RULE_REPLACEMENTS 20  // section for character replacements
#define RULE_NO_SUFFIX  24   // N
#define RULE_NOTVOWEL   25   // K
#define RULE_IFVERB     26   // V
#define RULE_ALT1       28   // T word has $alt attribute
#define RULE_NOVOWELS   29   // X no vowels up to word boundary
#define RULE_SPELLING   31   // W while spelling letter-by-letter
#define RULE_LAST_RULE   31

#define LETTERGP_A	0
#define LETTERGP_B	1
#define LETTERGP_C	2
#define LETTERGP_H	3
#define LETTERGP_F	4
#define LETTERGP_G	5
#define LETTERGP_Y	6
#define LETTERGP_VOWEL2   7


// Punctuation types  returned by ReadClause()
// bits 0-7 pause x 10mS, bits 12-14 intonation type,
// bit 19=sentence, bit 18=clause,  bits 17=voice change
// bit 16 used to distinguish otherwise identical types
// bit 20= punctuation character can be inside a word (Armenian)
#define CLAUSE_BIT_SENTENCE  0x80000
#define CLAUSE_BIT_VOICE     0x20000
#define PUNCT_IN_WORD        0x100000

#define CLAUSE_NONE         0 + 0x04000
#define CLAUSE_PARAGRAPH   70 + 0x80000
#define CLAUSE_EOF         35 + 0x90000
#define CLAUSE_VOICE        0 + 0x24000
#define CLAUSE_PERIOD      35 + 0x80000
#define CLAUSE_COMMA       20 + 0x41000
#define CLAUSE_SHORTCOMMA   4 + 0x41000
#define CLAUSE_SHORTFALL    4 + 0x40000
#define CLAUSE_QUESTION    35 + 0x82000
#define CLAUSE_EXCLAMATION 40 + 0x83000
#define CLAUSE_COLON       30 + 0x40000
#ifdef PLATFORM_RISCOS
#define CLAUSE_SEMICOLON   30 + 0x40000
#else
#define CLAUSE_SEMICOLON   30 + 0x41000
#endif

#define SAYAS_CHARS     0x12
#define SAYAS_GLYPHS    0x13
#define SAYAS_SINGLE_CHARS 0x14
#define SAYAS_KEY       0x24
#define SAYAS_DIGITS    0x40  // + number of digits
#define SAYAS_DIGITS1   0xc1

#define CHAR_EMPHASIS   0x0530  // this is an unused character code

// Rule:
// [4] [match] [1 pre] [2 post] [3 phonemes] 0
//     match 1 pre 2 post 0     - use common phoneme string
//     match 1 pre 2 post 3 0   - empty phoneme string

typedef const char *  constcharptr;

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
	unsigned short flags;
	unsigned char pre_pause;
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


typedef struct {
	int type;
	int parameter[N_SPEECH_PARAM];
} PARAM_STACK;

extern PARAM_STACK param_stack[];
extern const int param_defaults[N_SPEECH_PARAM];



#define N_LOPTS      16
#define LOPT_DIERESES        1
 // 1=remove [:] from unstressed syllables, 2= remove from unstressed or non-penultimate syllables
 // bit 4=0, if stress < 4,  bit 4=1, if not the highest stress in the word
#define LOPT_IT_LENGTHEN        2

 // 1=german
#define LOPT_PREFIXES        3

 // non-zero, change voiced/unoiced to match last consonant in a cluster
 // bit 1=LANG=ru,  don't propagate over [v]
 // bit 2=don't propagate acress word boundaries
 // bit 3=LANG=pl,  propagate over liquids and nasals
#define LOPT_REGRESSIVE_VOICING  4

 // 0=default, 1=no check, other allow this character as an extra initial letter (default is 's')
#define LOPT_UNPRONOUNCABLE  5

 // select length_mods tables,  (length_mod_tab) + (length_mod_tab0 * 100)
#define LOPT_LENGTH_MODS    6

 // increase this to prevent sonorants being shortened before shortened (eg. unstressed) vowels
#define LOPT_SONORANT_MIN    7

 // don't break vowels at word boundary
#define LOPT_WORD_MERGE      8

 // max. amplitude for vowel at the end of a clause
#define LOPT_MAXAMP_EOC      9

 // bit 0=reduce even if phonemes are specified in the **_list file
 // bit 1=don't reduce the strongest vowel in a word which is marked 'unstressed'
#define LOPT_REDUCE  10

 // LANG=cs,sk  combine some prepositions with the following word, if the combination has N or fewer syllables
 // bits 0-3  N syllables
 // bit 4=only if the second word has $alt attribute
 // bit 5=not if the second word is end-of-sentence
#define LOPT_COMBINE_WORDS 11

 // change [t] when followed by unstressed vowel
#define LOPT_REDUCE_T 12

 // 1 = allow capitals inside a word
 // 2 = stressed syllable is indicated by capitals
#define LOPT_CAPS_IN_WORD  13

 // bit 0=Italian "syntactic doubling" of consoants in the word after a word marked with $double attribute
 // bit 1=also after a word which ends with a stressed vowel
#define LOPT_IT_DOUBLING    14

  // Call ApplySpecialAttributes() if $alt or $alt2 is set for a word
#define LOPT_ALT  15


typedef struct {
// bits0-2  separate words with (1=pause_vshort, 2=pause_short, 3=pause, 4=pause_long 5=[?] phonemme)
// bit 3=don't use linking phoneme
// bit4=longer pause before STOP, VSTOP,FRIC
// bit5=length of a final vowel doesn't depend on the next phoneme
	int word_gap;
	int vowel_pause;
	int stress_rule; // 1=first syllable, 2=penultimate,  3=last

// bit0=don't stress monosyllables, except at end of clause
// bit1=don't set diminished stress,
// bit2=mark unstressed final syllables as diminished
// bit4=don't allow secondary stress on last syllable
// bit5-don't use automatic secondary stress
// bit6=light syllable followed by heavy, move secondary stress to the heavy syllable. LANG=Finnish
// bit8=stress last syllable if it doesn't end in a vowel
// bit9=stress last syllable if it doesn't end in vowel or "s" or "n"  LANG=Spanish
// bit12= In a 2-syllable word, if one has primary stress then give the other secondary stress
// bit13= If there is only one syllable before the primary stress, give it a secondary stress
// bit15= Give stress to the first unstressed syllable
// bit16= Don't diminish consecutive syllables within a word.
// bit17= "priority" stress reduces other primary stress to "unstressed" not "secondary"
// bit18= don't lengthen short vowels more than long vowels at end-of-clause
// bit19=stress on final syllable if it has a long vowel, but previous syllable has a short vowel

	int stress_flags; 
	int unstressed_wd1; // stress for $u word of 1 syllable
	int unstressed_wd2; // stress for $u word of >1 syllable
	int param[N_LOPTS];
	unsigned char *length_mods;
	unsigned char *length_mods0;

#define NUM_ROMAN        0x20000
#define NUM_ROMAN_UC     0x40000
#define NUM_NOPAUSE      0x80000
#define NUM_ROMAN_AFTER 0x200000
#define NUM_VIGESIMAL   0x400000

	// bits0-1=which numbers routine to use.
	// bit2=  thousands separator must be space
	// bit3=  , decimal separator, not .
	// bit4=use three-and-twenty rather than twenty-three
	// bit5='and' between tens and units
	// bit6=add "and" after hundred or thousand
	// bit7=don't have "and" both after hundreds and also between tens and units
   // bit8=only one primary stress in tens+units
	// bit9=only one vowel betwen tens and units
	// bit10=omit "one" before "hundred"
	// bit11=say 19** as nineteen hundred
	// bit12=allow space as thousands separator (in addition to langopts.thousands_sep)
	// bits13-15  post-decimal-digits 0=single digits, 1=(LANG=it) 2=(LANG=pl) 3=(LANG=ro)
	// bit16=dot after number indicates ordinal
	// bit17=recognize roman numbers
	// bit18=Roman numbers only if upper case
	// bit19=don't add pause after a number
	// bit20='and' before hundreds
	// bit21= say "roman" after the number, not before
	// bit22= vigesimal number, if tens are not found
	int numbers;

#define NUM2_100000     0x800   // numbers for 100,000 and 10,000,000
#define NUM2_100000a    0xc00   // numbers for 100,000 and 1,000,000
	// bits 1-4  use variant form of numbers before thousands,millions,etc.
	// bit6=(LANG=pl) two forms of plural, M or MA
	// bit7=(LANG-ru) use MB for 1 thousand, million, etc
	// bit8=(LANG=cs,sk) two forms of plural, M or MA
	// bit9=(LANG=rw) say "thousand" and "million" before its number, not after
   // bit10=(LANG=sw) special word for 100,000 and 1,000,000
	// bit11=(LANG=hi) special word for 100,000 and 10,000,000
	int numbers2;

	int max_roman;
	int thousands_sep;
	int decimal_sep;

	// bit 0, accent name before the letter name, bit 1 "capital" after letter name
	int accents;

	int tone_language;          // 1=tone language
	int intonation_group;
	int long_stop;          // extra mS pause for a lengthened stop
	int phoneme_change;     // TEST, change phonemes, after translation
	char max_initial_consonants;
	char spelling_stress;   // 0=default, 1=stress first letter
	char tone_numbers;
	char ideographs;      // treat as separate words
	char textmode;          // the meaning of FLAG_TEXTMODE is reversed (to save data when *_list file is compiled)
	int testing;            // testing options: bit 1= specify stressed syllable in the form:  "outdoor/2"
	int listx;    // compile *_listx after *list
	const unsigned int *replace_chars;      // characters to be substitutes
	const char *ascii_language;  // switch to this language for Latin characters
} LANGUAGE_OPTIONS;


// a parameter of ChangePhonemes()
typedef struct {
	int flags;
	unsigned char stress;          // stress level of this vowel
	unsigned char stress_highest;  // the highest stress level of a vowel in this word
	unsigned char n_vowels;        // number of vowels in the word
	unsigned char vowel_this;      // syllable number of this vowel (counting from 1)
	unsigned char vowel_stressed;  // syllable number of the highest stressed vowel
} CHANGEPH;



#define NUM_SEP_DOT    0x0008    // . , for thousands and decimal separator
#define NUM_SEP_SPACE  0x1000    // allow space as thousands separator (in addition to langopts.thousands_sep)
#define NUM_DEC_IT     0x2000    // (LANG=it) speak post-decimal-point digits as a combined number not as single digits

struct Translator
{//=============

	LANGUAGE_OPTIONS langopts;
	int translator_name;
	int transpose_offset;
	int transpose_max;
	int transpose_min;

	char phon_out[300];
	char phonemes_repeat[20];
	int phonemes_repeat_count;

	unsigned char stress_amps[8];
	unsigned char stress_amps_r[8];
	short stress_lengths[8];
	int dict_condition;    // conditional apply some pronunciation rules and dict.lookups
	const unsigned short *charset_a0;   // unicodes for characters 0xa0 to oxff
	const wchar_t *char_plus_apostrophe;  // single chars + apostrophe treated as words
	const wchar_t *punct_within_word;   // allow these punctuation characters within words

// holds properties of characters: vowel, consonant, etc for pronunciation rules
	unsigned char letter_bits[256];
	int letter_bits_offset;
	const wchar_t *letter_groups[8];

	/* index1=option, index2 by 0=. 1=, 2=?, 3=! 4=none */
#define INTONATION_TYPES 8
#define PUNCT_INTONATIONS 6
	unsigned char punct_to_tone[INTONATION_TYPES][PUNCT_INTONATIONS];

	char *data_dictrules;     // language_1   translation rules file
	char *data_dictlist;      // language_2   dictionary lookup file
	char *dict_hashtab[N_HASH_DICT];   // hash table to index dictionary lookup file
	char *letterGroups[N_LETTER_GROUPS];

	// groups1 and groups2 are indexes into data_dictrules, set up by InitGroups()
	// the two-letter rules for each letter must be consecutive in the language_rules source
	
	char *groups1[256];         // translation rule lists, index by single letter
	char *groups2[N_RULE_GROUP2];   // translation rule lists, indexed by two-letter pairs
	unsigned int groups2_name[N_RULE_GROUP2];  // the two letter pairs for groups2[]
	int n_groups2;              // number of groups2[] entries used
	
	unsigned char groups2_count[256];    // number of 2 letter groups for this initial letter
	unsigned char groups2_start[256];    // index into groups2
	
	
	int expect_verb;
	int expect_past;    // expect past tense
	int expect_verb_s;
	int expect_noun;
	int prev_last_stress;
	char *clause_end;

	int word_vowel_count;     // number of vowels so far
	int word_stressed_count;  // number of vowels so far which could be stressed
	
	int clause_upper_count;   // number of upper case letters in the clause
	int clause_lower_count;   // number of lower case letters in the clause

	int prepause_timeout;
	int end_stressed_vowel;  // word ends with stressed vowel
	int prev_dict_flags;     // dictionary flags from previous word
}; //  end of class Translator


extern int option_tone2;
#define OPTION_EMPHASIZE_ALLCAPS  0x100
#define OPTION_EMPHASIZE_PENULTIMATE 0x200
extern int option_tone_flags;
extern int option_waveout;
extern int option_quiet;
extern int option_phonemes;
extern int option_phoneme_events;
extern int option_linelength;     // treat lines shorter than this as end-of-clause
extern int option_multibyte;
extern int option_capitals;
extern int option_punctuation;
extern int option_endpause;
extern int option_ssml;
extern int option_phoneme_input;   // allow [[phonemes]] in input text
extern int option_phoneme_variants;
extern int option_sayas;
extern int option_wordgap;

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
extern int pre_pause;



#define N_MARKER_LENGTH 50   // max.length of a mark name
extern char skip_marker[N_MARKER_LENGTH];

#define N_PUNCTLIST  60
extern wchar_t option_punctlist[N_PUNCTLIST];  // which punctuation characters to announce
extern unsigned char punctuation_to_tone[INTONATION_TYPES][PUNCT_INTONATIONS];

extern Translator *translator;
extern Translator *translator2;
extern const unsigned short *charsets[N_CHARSETS];
extern char dictionary_name[40];
extern char ctrl_embedded;    // to allow an alternative CTRL for embedded commands
extern unsigned char *p_textinput;
extern wchar_t *p_wchar_input;
extern int dictionary_skipwords;

extern int (* uri_callback)(int, const char *, const char *);
extern int (* phoneme_callback)(const char *);
extern void SetLengthMods(Translator *tr, int value);

void LoadConfig(void);
int TransposeAlphabet(char *text, int offset, int min, int max);
int utf8_in(int *c, const char *buf);
int utf8_in2(int *c, const char *buf, int backwards);
int utf8_out(unsigned int c, char *buf);
int utf8_nbytes(const char *buf);
int lookupwchar(const unsigned short *list,int c);
int Eof(void);
char *strchr_w(const char *s, int c);
int IsBracket(int c);
void InitNamedata(void);
void InitText(int flags);
void InitText2(void);
int IsDigit(unsigned int c);
int IsAlpha(unsigned int c);
int isspace2(unsigned int c);
int towlower2(unsigned int c);
void GetTranslatedPhonemeString(char *phon_out, int n_phon_out);

Translator *SelectTranslator(const char *name);
int SetTranslator2(const char *name);
void DeleteTranslator(Translator *tr);
int Lookup(Translator *tr, const char *word, char *ph_out);

int TranslateNumber(Translator *tr, char *word1, char *ph_out, unsigned int *flags, int wflags);
int TranslateRoman(Translator *tr, char *word, char *ph_out);

void ChangeWordStress(Translator *tr, char *word, int new_stress);
void SetSpellingStress(Translator *tr, char *phonemes, int control, int n_chars);
int TranslateLetter(Translator *tr, char *letter, char *phonemes, int control, int word_length);
void LookupLetter(Translator *tr, unsigned int letter, int next_byte, char *ph_buf);
void LookupAccentedLetter(Translator *tr, unsigned int letter, char *ph_buf);

int LoadDictionary(Translator *tr, const char *name, int no_error);
int LookupDictList(Translator *tr, char **wordptr, char *ph_out, unsigned int *flags, int end_flags, WORD_TAB *wtab);

void MakePhonemeList(Translator *tr, int post_pause, int new_sentence);
int ChangePhonemes_ru(Translator *tr, PHONEME_LIST2 *phlist, int n_ph, int index, PHONEME_TAB *ph, CHANGEPH *ch);
void ApplySpecialAttribute(Translator *tr, char *phonemes, int dict_flags);
void ApplySpecialAttribute2(Translator *tr, char *phonemes, int dict_flags);
void AppendPhonemes(Translator *tr, char *string, int size, const char *ph);

void CalcLengths(Translator *tr);
void CalcPitches(Translator *tr, int clause_tone);

int RemoveEnding(Translator *tr, char *word, int end_type, char *word_copy);
int Unpronouncable(Translator *tr, char *word);
void SetWordStress(Translator *tr, char *output, unsigned int &dictionary_flags, int tonic, int prev_stress);
int TranslateRules(Translator *tr, char *p, char *phonemes, int size, char *end_phonemes, int end_flags, unsigned int *dict_flags);
int TranslateWord(Translator *tr, char *word1, int next_pause, WORD_TAB *wtab);
void *TranslateClause(Translator *tr, FILE *f_text, const void *vp_input, int *tone, char **voice_change);
int ReadClause(Translator *tr, FILE *f_in, char *buf, short *charix, int *charix_top, int n_buf, int *tone_type);

void SetVoiceStack(espeak_VOICE *v);

extern FILE *f_trans;		// for logging
