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
 *   along with this program; if not, write see:                           *
 *               <http://www.gnu.org/licenses/>.                           *
 ***************************************************************************/

#include "StdAfx.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>

#include "speak_lib.h"
#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"

//#define OPT_FORMAT         // format the text and write formatted copy to Log file 
//#define OUTPUT_FORMAT

extern void Write4Bytes(FILE *f, int value);
int HashDictionary(const char *string);

static FILE *f_log = NULL;
extern char *dir_dictionary;

extern char word_phonemes[N_WORD_PHONEMES];    // a word translated into phoneme codes

static int linenum;
static int error_count;
static int text_mode = 0;
static int debug_flag = 0;
static int error_need_dictionary = 0;

static int hash_counts[N_HASH_DICT];
static char *hash_chains[N_HASH_DICT];
static char letterGroupsDefined[N_LETTER_GROUPS];

MNEM_TAB mnem_rules[] = {
	{"unpr",   0x01},
	{"w_alt2", 0x12},
	{"w_alt3", 0x13},
	{"w_alt", 0x11},   // note: put longer names before their sub-strings
	{"p_alt2", 0x22},
	{"p_alt3", 0x23},
	{"p_alt", 0x21},
	{NULL, -1} };

MNEM_TAB mnem_flags[] = {
	// these in the first group put a value in bits0-3 of dictionary_flags
	{"$1", 0x41},           // stress on 1st syllable
	{"$2", 0x42},           // stress on 2nd syllable
	{"$3", 0x43},
	{"$4", 0x44},
	{"$5", 0x45},
	{"$6", 0x46},
	{"$7", 0x47},
	{"$u", 0x48},           // reduce to unstressed
	{"$u1", 0x49},
	{"$u2", 0x4a},
	{"$u3", 0x4b},
	{"$u+",  0x4c},           // reduce to unstressed, but stress at end of clause
	{"$u1+", 0x4d},
	{"$u2+", 0x4e},
	{"$u3+", 0x4f},


	// these set the corresponding numbered bit if dictionary_flags
	{"$pause",     8},    /* ensure pause before this word */
	{"$only",      9},    /* only match on this word without suffix */
	{"$onlys",     10},    /* only match with none, or with 's' suffix */
	{"$strend",    11},    /* full stress if at end of clause */
	{"$strend2",   12},    /* full stress if at end of clause, or only followed by unstressed */
	{"$unstressend",13},   /* reduce stress at end of clause */
	{"$atend",     14},    /* use this pronunciation if at end of clause */
	{"$atstart",   15},   // use this pronunciation at start of clause

	{"$abbrev",    17},    /* use this pronuciation rather than split into letters */
	{"$stem",      18},   // must have a suffix

// language specific
	{"$double",    19},   // IT double the initial consonant of next word
	{"$alt",       20},   // use alternative pronunciation
	{"$alt1",      20},   // synonym for $alt
	{"$alt2",      21},
	{"$alt3",      22},
	{"$combine",   23},   // Combine with the next word

	{"$dot",       24},   // ignore '.' after this word (abbreviation)
	{"$hasdot",    25},   // use this pronunciation if there is a dot after the word

	{"$max3",      27},   // limit to 3 repetitions
	{"$brk",       28},   // a shorter $pause
	{"$text",      29},   // word translates to replcement text, not phonemes

// flags in dictionary word 2
	{"$verbf",   0x20},    /* verb follows */
	{"$verbsf",  0x21},    /* verb follows, allow -s suffix */
	{"$nounf",   0x22},    /* noun follows */
	{"$pastf",   0x23},   /* past tense follows */
	{"$verb",    0x24},   /* use this pronunciation when its a verb */
	{"$noun",    0x25},   /* use this pronunciation when its a noun */
	{"$past",    0x26},   /* use this pronunciation when its past tense */
	{"$verbextend",0x28},   /* extend influence of 'verb follows' */
	{"$capital", 0x29},   /* use this pronunciation if initial letter is upper case */
	{"$allcaps", 0x2a},   /* use this pronunciation if initial letter is upper case */
	{"$accent",  0x2b},   // character name is base-character name + accent name
	{"$sentence",0x2d},   // only if this clause is a sentence (i.e. terminator is {. ? !} not {, ; :}

	// doesn't set dictionary_flags
	{"$?",        100},   // conditional rule, followed by byte giving the condition number

	{"$textmode",  200},
	{"$phonememode", 201},
	{NULL,   -1}
};


#define LEN_GROUP_NAME  12

typedef struct {
	char name[LEN_GROUP_NAME+1];
	unsigned int start;
	unsigned int length;
	int group3_ix;
} RGROUP;


int isspace2(unsigned int c)
{//=========================
// can't use isspace() because on Windows, isspace(0xe1) gives TRUE !
	int c2;

	if(((c2 = (c & 0xff)) == 0) || (c > ' '))
		return(0);
	return(1);
}



static FILE *fopen_log(const char *fname,const char *access)
{//==================================================
// performs fopen, but produces error message to f_log if it fails
	FILE *f;

	if((f = fopen(fname,access)) == NULL)
	{
		if(f_log != NULL)
			fprintf(f_log,"Can't access (%s) file '%s'\n",access,fname);
	}
	return(f);
}


const char *LookupMnemName(MNEM_TAB *table, const int value)
//==========================================================
/* Lookup a mnemonic string in a table, return its name */
{
   while(table->mnem != NULL)
   {
      if(table->value==value)
         return(table->mnem);
      table++;
   }
   return("");   /* not found */
}   /* end of LookupMnemValue */


char *print_dictionary_flags(unsigned int *flags)
{//==============================================
	static char buf[20];

	sprintf(buf,"%s  0x%x/%x",LookupMnemName(mnem_flags,(flags[0] & 0xf)+0x40), flags[0], flags[1]);
	return(buf);
}




char *DecodeRule(const char *group_chars, int group_length, char *rule, int control)
{//=================================================================================
/* Convert compiled match template to ascii */

   unsigned char rb;
	unsigned char c;
	char *p;
   int  ix;
	int  match_type;
	int  finished=0;
	int  value;
	int  linenum=0;
	int  flags;
	int  suffix_char;
	int  condition_num=0;
	int  at_start = 0;
	const char *name;
   char buf[60];
   char buf_pre[60];
	char suffix[20];
	static char output[60];

	static char symbols[] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
		'&','%','+','#','S','D','Z','A','L','!',' ','@','?','J','N','K','V','?','T','X','?','W'};

	static char symbols_lg[] = {'A','B','C','H','F','G','Y'};

	match_type = 0;
   buf_pre[0] = 0;

	for(ix=0; ix<group_length; ix++)
	{
		buf[ix] = group_chars[ix];
	}
	buf[ix] = 0;

	p = &buf[strlen(buf)];
   while(!finished)
   {
		rb = *rule++;

		if(rb <= RULE_LINENUM)
		{
			switch(rb)
			{
			case 0:
			case RULE_PHONEMES:
				finished=1;
				break;
			case RULE_PRE_ATSTART:
				at_start = 1;  // drop through to next case
			case RULE_PRE:
				match_type = RULE_PRE;
				*p = 0;
				p = buf_pre;
				break;
			case RULE_POST:
				match_type = RULE_POST;
				*p = 0;
				strcat(buf," (");
				p = &buf[strlen(buf)];
				break;
			case RULE_PH_COMMON:
				break;
			case RULE_CONDITION:
				/* conditional rule, next byte gives condition number */
				condition_num = *rule++;
				break;
			case RULE_LINENUM:
				value = (rule[1] & 0xff) - 1;
				linenum = (rule[0] & 0xff) - 1 + (value * 255);
				rule+=2;
				break;
			}
			continue;
		}
		
		if(rb == RULE_DOLLAR)
		{
			value = *rule++ & 0xff;
			if((value != 0x01) || (control & FLAG_UNPRON_TEST))
			{
				p[0] = '$';
				name = LookupMnemName(mnem_rules, value);
				strcpy(&p[1],name);
				p += (strlen(name)+1);
			}
			c = ' ';
		}
		else
		if(rb == RULE_ENDING)
		{
			static const char *flag_chars = "eipvdfq tba ";
			flags = ((rule[0] & 0x7f)<< 8) + (rule[1] & 0x7f);
			suffix_char = 'S';
			if(flags & (SUFX_P >> 8))
				suffix_char = 'P';
			sprintf(suffix,"%c%d",suffix_char,rule[2] & 0x7f);
			rule += 3;
			for(ix=0;ix<9;ix++)
			{
				if(flags & 1)
					sprintf(&suffix[strlen(suffix)],"%c",flag_chars[ix]);
				flags = (flags >> 1);
			}
			strcpy(p,suffix);
			p += strlen(suffix);
			c = ' ';
		}
		else
		if(rb == RULE_LETTERGP)
		{
			c = symbols_lg[*rule++ - 'A'];
		}
		else
		if(rb == RULE_LETTERGP2)
		{
			value = *rule++ - 'A';
			p[0] = 'L';
			p[1] = (value / 10) + '0';
			c = (value % 10) + '0';

			if(match_type == RULE_PRE)
			{
				p[0] = c;
				c = 'L';
			}
			p+=2;
		}
		else
		if(rb <= RULE_LAST_RULE)
			c = symbols[rb];
		else
		if(rb == RULE_SPACE)
			c = '_';
		else
			c = rb;
		*p++ = c;
	}
	*p = 0;

	p = output;
	if(linenum > 0)
	{
		sprintf(p,"%5d:\t",linenum);
		p += 7;
	}
	if(condition_num > 0)
	{
		sprintf(p,"?%d ",condition_num);
		p = &p[strlen(p)];
	}
	if(((ix = strlen(buf_pre)) > 0) || at_start)
	{
		if(at_start)
			*p++ = '_';
		while(--ix >= 0)
			*p++ = buf_pre[ix];
		*p++ = ')';
		*p++ = ' ';
	}
	*p = 0;
	strcat(p,buf);
	ix = strlen(output);
	while(ix < 8)
		output[ix++]=' ';
	output[ix]=0;
   return(output);
}   /* end of DecodeRule */




static int compile_line(char *linebuf, char *dict_line, int *hash)
{//===============================================================
// Compile a line in the language_list file
	unsigned char  c;
	char *p;
	char *word;
	char *phonetic;
	unsigned int  ix;
	int  step;
	unsigned int  n_flag_codes = 0;
	int  flag_offset;
	int  length;
	int  multiple_words = 0;
	int  multiple_numeric_hyphen = 0;
	char *multiple_string = NULL;
	char *multiple_string_end = NULL;
	
	int len_word;
	int len_phonetic;
	int text_not_phonemes;   // this word specifies replacement text, not phonemes
	unsigned int  wc;
	int all_upper_case;
	
	char *mnemptr;
	char *comment;
	unsigned char flag_codes[100];
	char encoded_ph[200];
	unsigned char bad_phoneme[4];
static char nullstring[] = {0};

	comment = NULL;
	text_not_phonemes = 0;
	phonetic = word = nullstring;

if(memcmp(linebuf,"_-",2)==0)
{
step=1;  // TEST
}
	p = linebuf;
//	while(isspace2(*p)) p++;

#ifdef deleted
	if(*p == '$')
	{
		if(memcmp(p,"$textmode",9) == 0)
		{
			text_mode = 1;
			return(0);
		}
		if(memcmp(p,"$phonememode",12) == 0)
		{
			text_mode = 0;
			return(0);
		}
	}
#endif

	step = 0;
	
	c = 0;
	while(c != '\n')
	{
		c = *p;
	
		if((c == '?') && (step==0))
		{
			// conditional rule, allow only if the numbered condition is set for the voice
			flag_offset = 100;

			p++;
			if(*p == '!')
			{
				// allow only if the numbered condition is NOT set
				flag_offset = 132;
				p++;
			}

			ix = 0;
			if(isdigit(*p))
			{
				ix += (*p-'0');
				p++;
			}
			if(isdigit(*p))
			{
				ix = ix*10 + (*p-'0');
				p++;
			}
			flag_codes[n_flag_codes++] = ix + flag_offset;
			c = *p;
		}
		
		if((c == '$') && isalnum(p[1]))
		{
			/* read keyword parameter */
			mnemptr = p;
			while(!isspace2(c = *p)) p++;
			*p = 0;
	
			ix = LookupMnem(mnem_flags,mnemptr);
			if(ix > 0)
			{
				if(ix == 200)
				{
					text_mode = 1;
				}
				else
				if(ix == 201)
				{
					text_mode = 0;
				}
				else
				if(ix == BITNUM_FLAG_TEXTMODE)
				{
					text_not_phonemes = 1;
				}
				else
				{
					flag_codes[n_flag_codes++] = ix;
				}
			}
			else
			{
				fprintf(f_log,"%5d: Unknown keyword: %s\n",linenum,mnemptr);
				error_count++;
			}
		}
	
		if((c == '/') && (p[1] == '/') && (multiple_words==0))
		{
			c = '\n';   /* "//" treat comment as end of line */
			comment = p;
		}
	
		switch(step)
		{
		case 0:
			if(c == '(')
			{
				multiple_words = 1;
				word = p+1;
				step = 1;
			}
			else
			if(!isspace2(c))
			{
				word = p;
				step = 1;
			}
			break;
	
		case 1:
			if((c == '-') && multiple_words)
			{
				if(isdigit(word[0]))
				{
					multiple_numeric_hyphen = 1;
				}
//				else  // ???
				{
					flag_codes[n_flag_codes++] = BITNUM_FLAG_HYPHENATED;
				}
				c = ' ';
			}
			if(isspace2(c))
			{
				p[0] = 0;   /* terminate english word */

				if(multiple_words)
				{
					multiple_string = multiple_string_end = p+1;
					step = 2;
				}
				else
				{
					step = 3;
				}
			}
			else
			if((c == ')') && multiple_words)
			{
				p[0] = 0;
				step = 3;
				multiple_words = 0;
			}
			break;

		case 2:
			if(isspace2(c))
			{
				multiple_words++;
			}
			else
			if(c == ')')
			{
				p[0] = ' ';   // terminate extra string
				multiple_string_end = p+1;
				step = 3;
			}
			break;
	
		case 3:
			if(!isspace2(c))
			{
				phonetic = p;
				step = 4;
			}
			break;
	
		case 4:
			if(isspace2(c))
			{
				p[0] = 0;   /* terminate phonetic */
				step = 5;
			}
			break;
	
		case 5:
			break;
		}
		p++;
	}
	
	if(word[0] == 0)
	{
#ifdef OPT_FORMAT
		if(comment != NULL)
			fprintf(f_log,"%s",comment);
		else
			fputc('\n',f_log);
#endif
		return(0);   /* blank line */
	}

	if(text_mode)
		text_not_phonemes = 1;

	if(text_not_phonemes)
	{
		if(word[0] == '_')
		{
			// This is a special word, used by eSpeak.  Translate this into phonemes now
			strcat(phonetic, " ");     // need a space to indicate word-boundary

	// PROBLEM  vowel reductions are not applied to the translated phonemes
	// condition rules are not applied
			TranslateWord(translator,phonetic,0,NULL);
			text_not_phonemes = 0;
			strncpy0(encoded_ph, word_phonemes, N_WORD_BYTES-4);

			if((word_phonemes[0] == 0) && (error_need_dictionary < 3))
			{
				// the dictionary was not loaded, we need a second attempt
				error_need_dictionary++;
				fprintf(f_log,"%5d: Need to compile dictionary again\n",linenum);
			}
{
//char decoded_phonemes[128];
//DecodePhonemes(word_phonemes,decoded_phonemes);
//printf("Translator %x  %s  [%s] [%s]\n",translator->translator_name,word,phonetic,decoded_phonemes);
}
		}
		else
		{
			// this is replacement text, so don't encode as phonemes. Restrict the length of the replacement word
			strncpy0(encoded_ph,phonetic,N_WORD_BYTES-4);
		}
	}
	else
	{
		EncodePhonemes(phonetic,encoded_ph,bad_phoneme);
		if(strchr(encoded_ph,phonSWITCH) != 0)
		{
			flag_codes[n_flag_codes++] = BITNUM_FLAG_ONLY_S;  // don't match on suffixes (except 's') when switching languages
		}

		// check for errors in the phonemes codes
		if(bad_phoneme[0] != 0)
		{
			// unrecognised phoneme, report error
			fprintf(f_log,"%5d: Bad phoneme [%c] (0x%x) in: %s  %s\n",linenum,bad_phoneme[0],bad_phoneme[0],word,phonetic);
			error_count++;
		}
	}

	if(text_not_phonemes != translator->langopts.textmode)
	{
		flag_codes[n_flag_codes++] = BITNUM_FLAG_TEXTMODE;
	}


	if(sscanf(word,"U+%x",&wc) == 1)
	{
		// Character code
		ix = utf8_out(wc, word);
		word[ix] = 0;
	}
	else
	if(word[0] != '_')
	{
		// convert to lower case, and note if the word is all-capitals
		int c2;

		all_upper_case = 1;
		p = word;
		for(p=word;;)
		{
			// this assumes that the lower case char is the same length as the upper case char
			// OK, except for Turkish "I", but use towlower() rather than towlower2()
			ix = utf8_in(&c2,p);
			if(c2 == 0)
				break;
			if(iswupper(c2))
			{
				utf8_out(towlower(c2),p);
			}
			else
			{
				all_upper_case = 0;
			}
			p += ix;
		}
		if(all_upper_case)
		{
			flag_codes[n_flag_codes++] = BITNUM_FLAG_ALLCAPS;
		}
	}

	len_word = strlen(word);

	if(translator->transpose_min > 0)
	{
		len_word = TransposeAlphabet(translator, word);
	}

	*hash = HashDictionary(word);
	len_phonetic = strlen(encoded_ph);
	
	dict_line[1] = len_word;   // bit 6 indicates whether the word has been compressed
	len_word &= 0x3f;

	memcpy(&dict_line[2],word,len_word);

	if(len_phonetic == 0)
	{
		// no phonemes specified. set bit 7
		dict_line[1] |= 0x80;
		length = len_word + 2;
	}
	else
	{
		length = len_word + len_phonetic + 3;
		strcpy(&dict_line[(len_word)+2],encoded_ph);
	}
	
	for(ix=0; ix<n_flag_codes; ix++)
	{
		dict_line[ix+length] = flag_codes[ix];
	}
	length += n_flag_codes;

	if((multiple_string != NULL) && (multiple_words > 0))
	{
		if(multiple_words > 10)
		{
			fprintf(f_log,"%5d: Two many parts in a multi-word entry: %d\n",linenum,multiple_words);
		}
		else
		{
			dict_line[length++] = 80 + multiple_words;
			ix = multiple_string_end - multiple_string;
			if(multiple_numeric_hyphen)
			{
				dict_line[length++] = ' ';   // ???
			}
			memcpy(&dict_line[length],multiple_string,ix);
			length += ix;
		}
	}
	dict_line[0] = length;

#ifdef OPT_FORMAT
	spaces = 16;
	for(ix=0; ix<n_flag_codes; ix++)
	{
		if(flag_codes[ix] >= 100)
		{
			fprintf(f_log,"?%d ",flag_codes[ix]-100);
			spaces -= 3;
		}
	}

	fprintf(f_log,"%s",word);
	spaces -= strlen(word);
	DecodePhonemes(encoded_ph,decoded_ph);
	while(spaces-- > 0) fputc(' ',f_log);
	spaces += (14 - strlen(decoded_ph));
	
	fprintf(f_log," %s",decoded_ph);
	while(spaces-- > 0) fputc(' ',f_log);
	for(ix=0; ix<n_flag_codes; ix++)
	{
		if(flag_codes[ix] < 100)
			fprintf(f_log," %s",lookup_mnem(mnem_flags,flag_codes[ix]));
	}
	if(comment != NULL)
		fprintf(f_log," %s",comment);
	else
		fputc('\n',f_log);
#endif

	return(length);
}  /* end of compile_line */



static void compile_dictlist_start(void)
{//=====================================
// initialise dictionary list
	int ix;
	char *p;
	char *p2;

	for(ix=0; ix<N_HASH_DICT; ix++)
	{
		p = hash_chains[ix];
		while(p != NULL)
		{
			memcpy(&p2,p,sizeof(char *));
			free(p);
			p = p2;
		}
		hash_chains[ix] = NULL;
		hash_counts[ix]=0;
	}
}


static void compile_dictlist_end(FILE *f_out)
{//==========================================
// Write out the compiled dictionary list
	int hash;
	int length;
	char *p;

	if(f_log != NULL)
	{
#ifdef OUTPUT_FORMAT
		for(hash=0; hash<N_HASH_DICT; hash++)
		{
			fprintf(f_log,"%8d",hash_counts[hash]);
			if((hash & 7) == 7)
				fputc('\n',f_log);
		}
		fflush(f_log);
#endif
	}
	
	for(hash=0; hash<N_HASH_DICT; hash++)
	{
		p = hash_chains[hash];
		hash_counts[hash] = (int)ftell(f_out);
	
		while(p != NULL)
		{
			length = *(p+sizeof(char *));
			fwrite(p+sizeof(char *),length,1,f_out);
			memcpy(&p,p,sizeof(char *));
		}
		fputc(0,f_out);
	}
}



static int compile_dictlist_file(const char *path, const char* filename)
{//=====================================================================
	int  length;
	int  hash;
	char *p;
	int  count=0;
	FILE *f_in;
	char buf[200];
	char fname[sizeof(path_home)+45];
	char dict_line[128];
	
	text_mode = 0;

	// try with and without '.txt' extension
	sprintf(fname,"%s%s.txt",path,filename);
	if((f_in = fopen(fname,"r")) == NULL)
	{
		sprintf(fname,"%s%s",path,filename);
		if((f_in = fopen(fname,"r")) == NULL)
			return(-1);
	}

	fprintf(f_log,"Compiling: '%s'\n",fname);

	linenum=0;
	
	while(fgets(buf,sizeof(buf),f_in) != NULL)
	{
		linenum++;

		length = compile_line(buf,dict_line,&hash);
		if(length == 0)  continue;   /* blank line */

		hash_counts[hash]++;
	
		p = (char *)malloc(length+sizeof(char *));
		if(p == NULL)
		{
			if(f_log != NULL)
			{
				fprintf(f_log,"Can't allocate memory\n");
				error_count++;
			}
			break;
		}
	
		memcpy(p,&hash_chains[hash],sizeof(char *));
		hash_chains[hash] = p;
		memcpy(p+sizeof(char *),dict_line,length);
		count++;
	}
	
	fprintf(f_log,"\t%d entries\n",count);
	fclose(f_in);
	return(0);
}   /* end of compile_dictlist_file */



static char rule_cond[80];
static char rule_pre[80];
static char rule_post[80];
static char rule_match[80];
static char rule_phonemes[80];
static char group_name[LEN_GROUP_NAME+1];
static int group3_ix;

#define N_RULES 2000		// max rules for each group



static void copy_rule_string(char *string, int &state)
{//===================================================
// state 0: conditional, 1=pre, 2=match, 3=post, 4=phonemes
	static char *outbuf[5] = {rule_cond, rule_pre, rule_match, rule_post, rule_phonemes};
	static int next_state[5] = {2,2,4,4,4};
	char *output;
	char *p;
	int ix;
	int len;
	char c;
	int  sxflags;
	int  value;
	int  literal;
	MNEM_TAB *mr;

	if(string[0] == 0) return;

	output = outbuf[state];
	if(state==4)
	{
		// append to any previous phoneme string, i.e. allow spaces in the phoneme string
		len = strlen(rule_phonemes);
		if(len > 0)
			rule_phonemes[len++] = ' ';
		output = &rule_phonemes[len];
	}
	sxflags = 0x808000;           // to ensure non-zero bytes
	
	for(p=string,ix=0;;)
	{
		literal = 0;
		c = *p++;
		if(c == '\\')
		{
			c = *p++;   // treat next character literally
			if((c >= '0') && (c <= '3') && (p[0] >= '0') && (p[0] <= '7') && (p[1] >= '0') && (p[1] <= '7'))
			{
				// character code given by 3 digit octal value;
				c = (c-'0')*64 + (p[0]-'0')*8 + (p[1]-'0');
				p += 2;
			}
			literal = 1;
		}

		if((state==1) || (state==3))
		{
			// replace special characters (note: 'E' is reserved for a replaced silent 'e')
			if(literal == 0)
			{
				static const char lettergp_letters[9] = {LETTERGP_A,LETTERGP_B,LETTERGP_C,0,0,LETTERGP_F,LETTERGP_G,LETTERGP_H,LETTERGP_Y};
				switch(c)
				{
				case '_':
					c = RULE_SPACE;
					break;

				case 'Y':
					c = 'I';   // drop through to next case
				case 'A':   // vowel
				case 'B':
				case 'C':
				case 'H':
				case 'F':
				case 'G':
					if(state == 1)
					{
						// pre-rule, put the number before the RULE_LETTERGP;
						output[ix++] = lettergp_letters[c-'A'] + 'A';
						c = RULE_LETTERGP;
					}
					else
					{
						output[ix++] = RULE_LETTERGP;
						c = lettergp_letters[c-'A'] + 'A';
					}
					break;
				case 'D':
					c = RULE_DIGIT;
					break;
				case 'K':
					c = RULE_NOTVOWEL;
					break;
				case 'N':
					c = RULE_NO_SUFFIX;
					break;
				case 'V':
					c = RULE_IFVERB;
					break;
				case 'Z':
					c = RULE_NONALPHA;
					break;
				case '+':
					c = RULE_INC_SCORE;
					break;
				case '@':
					c = RULE_SYLLABLE;
					break;
				case '&':
					c = RULE_STRESSED;
					break;
				case '%':
					c = RULE_DOUBLE;
					break;
				case '#':
					c = RULE_DEL_FWD;
					break;
				case '!':
					c = RULE_CAPITAL;
					break;
				case 'T':
					output[ix++] = RULE_DOLLAR;
					c = 0x11;
					break;
				case 'W':
					c = RULE_SPELLING;
					break;
				case 'X':
					c = RULE_NOVOWELS;
					break;
				case 'J':
					c = RULE_SKIPCHARS;
					break;
				case 'L':
					// expect two digits
					c = *p++ - '0';
					value = *p++ - '0';
					c = c * 10 + value;
					if((value < 0) || (value > 9))
					{
						c = 0;
						fprintf(f_log,"%5d: Expected 2 digits after 'L'\n",linenum);
						error_count++;
					}
					else
					if((c <= 0) || (c >= N_LETTER_GROUPS) || (letterGroupsDefined[(int)c] == 0))
					{
						fprintf(f_log,"%5d: Letter group L%.2d not defined\n",linenum,c);
						error_count++;
					}
					c += 'A';
					if(state == 1)
					{
						// pre-rule, put the group number before the RULE_LETTERGP command
						output[ix++] = c;
						c = RULE_LETTERGP2;
					}
					else
					{
						output[ix++] = RULE_LETTERGP2;
					}
					break;

				case '$':
					output[ix++] = RULE_DOLLAR;
					c = 0;
					mr = mnem_rules;
					while(mr->mnem != NULL)
					{
						len = strlen(mr->mnem);
						if(memcmp(p, mr->mnem, len) == 0)
						{
							c = mr->value;
							p += len;
							break;
						}
						mr++;
					}
					if(c == 0)
					{
						fprintf(f_log,"%5d: $ command not recognized\n",linenum);
						error_count++;
					}
					break;

				case 'P':
					sxflags |= SUFX_P;   // Prefix, now drop through to Suffix
				case 'S':
					output[ix++] = RULE_ENDING;
					value = 0;
					while(!isspace2(c = *p++) && (c != 0))
					{
						switch(c)
						{
						case 'e':
							sxflags |= SUFX_E;
							break;
						case 'i':
							sxflags |= SUFX_I;
							break;
						case 'p':	// obsolete, replaced by 'P' above
							sxflags |= SUFX_P;
							break;
						case 'v':
							sxflags |= SUFX_V;
							break;
						case 'd':
							sxflags |= SUFX_D;
							break;
						case 'f':
							sxflags |= SUFX_F;
							break;
						case 'q':
							sxflags |= SUFX_Q;
							break;
						case 't':
							sxflags |= SUFX_T;
							break;
						case 'b':
							sxflags |= SUFX_B;
							break;
						case 'a':
							sxflags |= SUFX_A;
							break;
						default:
							if(isdigit(c))
								value = (value*10) + (c - '0');
							break;
						}
					}
					p--;
					output[ix++] = sxflags >> 16;
					output[ix++] = sxflags >> 8;
					c = value | 0x80;
					break;
				}
			}
		}
		output[ix++] = c;
		if(c == 0) break;
	}

	state = next_state[state];
}  //  end of copy_rule_string



static char *compile_rule(char *input)
{//===================================
	int ix;
	unsigned char c;
	int wc;
	char *p;
	char *prule;
	int len;
	int len_name;
	int start;
	int state=2;
	int finish=0;
	int pre_bracket=0;
	char buf[80];
	char output[150];
	unsigned char bad_phoneme[4];

	buf[0]=0;
	rule_cond[0]=0;
	rule_pre[0]=0;
	rule_post[0]=0;
	rule_match[0]=0;
	rule_phonemes[0]=0;

	p = buf;
	
	for(ix=0; finish==0; ix++)
	{
		c = input[ix];

		switch(c = input[ix])
		{
		case ')':		// end of prefix section
			*p = 0;
			state = 1;
			pre_bracket = 1;
			copy_rule_string(buf,state);
			p = buf;
			break;
			
		case '(':		// start of suffix section
			*p = 0;
			state = 2;
			copy_rule_string(buf,state);
			state = 3;
			p = buf;
			if(input[ix+1] == ' ')
			{
				fprintf(f_log,"%5d: Syntax error. Space after (\n",linenum);
				error_count++;
			}
			break;
			
		case '\n':		// end of line
		case '\r':
		case 0:			// end of line
			*p = 0;
			copy_rule_string(buf,state);
			finish=1;
			break;
			
		case '\t':		// end of section section
		case ' ':
			*p = 0;
			copy_rule_string(buf,state);
			p = buf;
			break;
			
		case '?':
			if(state==2)
				state=0;
			else
				*p++ = c;
			break;

		default:
			*p++ = c;
			break;
		}
	}
	
	if(strcmp(rule_match,"$group")==0)
		strcpy(rule_match,group_name);

	if(rule_match[0]==0)
	{
		if(rule_post[0] != 0)
		{
			fprintf(f_log,"%5d: Syntax error\n",linenum);
			error_count++;
		}
		return(NULL);
	}

	EncodePhonemes(rule_phonemes,buf,bad_phoneme);
	if(bad_phoneme[0] != 0)
	{
		fprintf(f_log,"%5d: Bad phoneme [%c] in %s\n",linenum,bad_phoneme[0],input);
		error_count++;
	}
	strcpy(output,buf);
	len = strlen(buf)+1;
	
	len_name = strlen(group_name);
	if((len_name > 0) && (memcmp(rule_match,group_name,len_name) != 0))
	{
		utf8_in(&wc,rule_match);
		if((group_name[0] == '9') && IsDigit(wc))
		{
			// numeric group, rule_match starts with a digit, so OK
		}
		else
		{
			fprintf(f_log,"%5d: Wrong initial letters '%s' for group '%s'\n",linenum,rule_match,group_name);
			error_count++;
		}
	}
	strcpy(&output[len],rule_match);
	len += strlen(rule_match);

	if(debug_flag)
	{
		output[len] = RULE_LINENUM;
		output[len+1] = (linenum % 255) + 1;
		output[len+2] = (linenum / 255) + 1;
		len+=3;
	}

	if(rule_cond[0] != 0)
	{
		ix = -1;
		if(rule_cond[0] == '!')
		{
			// allow the rule only if the condition number is NOT set for the voice
			ix = atoi(&rule_cond[1]) + 32;
		}
		else
		{
			// allow the rule only if the condition number is set for the voice
			ix = atoi(rule_cond);
		}

		if((ix > 0) && (ix < 255))
		{
			output[len++] = RULE_CONDITION;
			output[len++] = ix;
		}
		else
		{
			fprintf(f_log,"%5d: bad condition number ?%d\n",linenum,ix);
			error_count++;
		}
	}
	if(rule_pre[0] != 0)
	{
		start = 0;
		if(rule_pre[0] == RULE_SPACE)
		{
			// omit '_' at the beginning of the pre-string and imply it by using RULE_PRE_ATSTART
			c = RULE_PRE_ATSTART;
			start = 1;
		}
		else
		{
			c = RULE_PRE;
		}
		output[len++] = c;

		// output PRE string in reverse order
		for(ix = strlen(rule_pre)-1; ix>=start; ix--)
			output[len++] = rule_pre[ix];
	}

	if(rule_post[0] != 0)
	{
		sprintf(&output[len],"%c%s",RULE_POST,rule_post);
		len += (strlen(rule_post)+1);
	}
	output[len++]=0;
	prule = (char *)malloc(len);
	memcpy(prule,output,len);
	return(prule);
}  //  end of compile_rule


int __cdecl string_sorter(char **a, char **b)
{//===========================================
	char *pa, *pb;
	int ix;

   if((ix = strcmp(pa = *a,pb = *b)) != 0)
	   return(ix);
	pa += (strlen(pa)+1);
	pb += (strlen(pb)+1);
   return(strcmp(pa,pb));
}   /* end of string_sorter */


static int __cdecl rgroup_sorter(RGROUP *a, RGROUP *b)
{//===================================================
// Sort long names before short names
	int ix;
	ix = strlen(b->name) - strlen(a->name);
	if(ix != 0) return(ix);
	ix = strcmp(a->name,b->name);
	if(ix != 0) return(ix);
	return(a->start-b->start);
}


#ifdef OUTPUT_FORMAT
static void print_rule_group(FILE *f_out, int n_rules, char **rules, char *name)
{//=============================================================================
	int rule;
	int ix;
	unsigned char c;
	int len1;
	int len2;
	int spaces;
	char *p;
	char *pout;
	int condition;
	char buf[80];
	char suffix[12];

	static unsigned char symbols[] = {'@','&','%','+','#','$','D','Z','A','B','C','F'};

	fprintf(f_out,"\n$group %s\n",name);

	for(rule=0; rule<n_rules; rule++)
	{
		p = rules[rule];
		len1 = strlen(p) + 1;
		p = &p[len1];
		len2 = strlen(p);
		
		rule_match[0]=0;
		rule_pre[0]=0;
		rule_post[0]=0;
		condition = 0;

		pout = rule_match;
		for(ix=0; ix<len2; ix++)
		{
			switch(c = p[ix])
			{
			case RULE_PRE:
				*pout = 0;
				pout = rule_pre;
				break;
			case RULE_POST:
				*pout = 0;
				pout = rule_post;
				break;
			case RULE_CONDITION:
				condition = p[++ix];
				break;
			case RULE_ENDING:
				sprintf(suffix,"$%d[%x]",(p[ix+2]),p[ix+1] & 0x7f);
				ix += 2;
				strcpy(pout,suffix);
				pout += strlen(suffix);
				break;
			default:
				if(c <= RULE_LETTER7)
					c = symbols[c-RULE_SYLLABLE];
				if(c == ' ')
					c = '_';
				*pout++ = c;
				break;
			}
		}
		*pout = 0;
		
		spaces = 12;
		if(condition > 0)
		{
			sprintf(buf,"?%d ",condition);
			spaces -= strlen(buf);
			fprintf(f_out,"%s",buf);
		}

		if(rule_pre[0] != 0)
		{
			p = buf;
			for(ix=strlen(rule_pre)-1;ix>=0;ix--)
				*p++ = rule_pre[ix];
			sprintf(p,") ");
			spaces -= strlen(buf);
			for(ix=0; ix<spaces; ix++)
			   fputc(' ',f_out);
			fprintf(f_out,"%s",buf);
			spaces = 0;
		}
		
		for(ix=0; ix<spaces; ix++)
			fputc(' ',f_out);
		
		spaces = 14;
		sprintf(buf," %s ",rule_match);
		if(rule_post[0] != 0)
		{
			p = &buf[strlen(buf)];
			sprintf(p,"(%s ",rule_post);
		}
		fprintf(f_out,"%s",buf);
		spaces -= strlen(buf);

		for(ix=0; ix<spaces; ix++)
			fputc(' ',f_out);
		DecodePhonemes(rules[rule],buf);
		fprintf(f_out,"%s\n",buf);   // phonemes
	}
}
#endif


//#define LIST_GROUP_INFO
static void output_rule_group(FILE *f_out, int n_rules, char **rules, char *name)
{//==============================================================================
	int ix;
	int len1;
	int len2;
	int len_name;
	char *p;
	char *p2, *p3;
	const char *common;

	short nextchar_count[256];
	memset(nextchar_count,0,sizeof(nextchar_count));

	len_name = strlen(name);

#ifdef OUTPUT_FORMAT
	print_rule_group(f_log,n_rules,rules,name);
#endif

	// sort the rules in this group by their phoneme string
	common = "";
	qsort((void *)rules,n_rules,sizeof(char *),(int (__cdecl *)(const void *,const void *))string_sorter);

	if(strcmp(name,"9")==0)
		len_name = 0;    //  don't remove characters from numeric match strings

	for(ix=0; ix<n_rules; ix++)
	{
		p = rules[ix];
		len1 = strlen(p) + 1;  // phoneme string
		p3 = &p[len1];
		p2 = p3 + len_name;        // remove group name from start of match string
		len2 = strlen(p2);

		nextchar_count[(unsigned char)(p2[0])]++;   // the next byte after the group name

		if((common[0] != 0) && (strcmp(p,common)==0))
		{
			fwrite(p2,len2,1,f_out);
			fputc(0,f_out);		// no phoneme string, it's the same as previous rule
		}
		else
		{
			if((ix < n_rules-1) && (strcmp(p,rules[ix+1])==0))
			{
				common = rules[ix];   // phoneme string is same as next, set as common
				fputc(RULE_PH_COMMON,f_out);
			}

			fwrite(p2,len2,1,f_out);
			fputc(RULE_PHONEMES,f_out);
			fwrite(p,len1,1,f_out);
		}
	}

#ifdef LIST_GROUP_INFO
	for(ix=32; ix<256; ix++)
	{
		if(nextchar_count[ix] > 30)
			printf("Group %s   %c  %d\n",name,ix,nextchar_count[ix]);
	}
#endif
}  //  end of output_rule_group



static int compile_lettergroup(char *input, FILE *f_out)
{//=====================================================
	char *p;
	char *p_start;
	int group;
	int ix;
	int n_items;
	int length;
	int max_length = 0;

	#define N_LETTERGP_ITEMS 200
	char *items[N_LETTERGP_ITEMS];
	char item_length[N_LETTERGP_ITEMS];

	p = input;
	if(!isdigit(p[0]) || !isdigit(p[1]))
	{
		fprintf(f_log,"%5d: Expected 2 digits after '.L'\n",linenum);
		error_count++;
		return(1);
	}

	group = atoi(&p[0]);
	if(group >= N_LETTER_GROUPS)
	{
		fprintf(f_log,"%5d: lettergroup out of range (01-%.2d)\n",linenum,N_LETTER_GROUPS-1);
		error_count++;
		return(1);
	}

	while(!isspace2(*p)) p++;

	fputc(RULE_GROUP_START,f_out);
	fputc(RULE_LETTERGP2,f_out);
	fputc(group + 'A', f_out);
	if(letterGroupsDefined[group] != 0)
	{
		fprintf(f_log,"%5d: lettergroup L%.2d is already defined\n",linenum,group);
		error_count++;
	}
	letterGroupsDefined[group] = 1;

	n_items = 0;
	while(n_items < N_LETTERGP_ITEMS)
	{
		while(isspace2(*p)) p++;
		if(*p == 0)
			break;

		items[n_items] = p_start = p;
		while((*p & 0xff) > ' ')
		{
			p++;
		}
		*p++ = 0;
		length = p - p_start;
		if(length > max_length)
			max_length = length;
		item_length[n_items++] = length;
	}

	// write out the items, longest first
	while(max_length > 1)
	{
		for(ix=0; ix < n_items; ix++)
		{
			if(item_length[ix] == max_length)
			{
				fwrite(items[ix],1,max_length,f_out);
			}
		}
		max_length--;
	}

	fputc(RULE_GROUP_END,f_out);

	return(0);
}


static int compile_dictrules(FILE *f_in, FILE *f_out, char *fname_temp)
{//====================================================================
	char *prule;
	unsigned char *p;
	int ix;
	int c;
	int gp;
	FILE *f_temp;
	int n_rules=0;
	int count=0;
	int different;
	int wc;
	const char *prev_rgroup_name;
	unsigned int char_code;
	int compile_mode=0;
	char *buf;
	char buf1[500];
	char *rules[N_RULES];

	int n_rgroups = 0;
	int n_groups3 = 0;
	RGROUP rgroup[N_RULE_GROUP2];
	
	linenum = 0;
	group_name[0] = 0;

	if((f_temp = fopen_log(fname_temp,"wb")) == NULL)
		return(1);

	for(;;)
	{
		linenum++;
		buf = fgets(buf1,sizeof(buf1),f_in);
		if(buf != NULL)
		{
			if((p = (unsigned char *)strstr(buf,"//")) != NULL)
				*p = 0;

			if(buf[0] == '\r') buf++;  // ignore extra \r in \r\n 
		}

		if((buf == NULL) || (buf[0] == '.'))
		{
			// next .group or end of file, write out the previous group

			if(n_rules > 0)
			{
				strcpy(rgroup[n_rgroups].name,group_name);
				rgroup[n_rgroups].group3_ix = group3_ix;
				rgroup[n_rgroups].start = ftell(f_temp);
				output_rule_group(f_temp,n_rules,rules,group_name);
				rgroup[n_rgroups].length = ftell(f_temp) - rgroup[n_rgroups].start;
				n_rgroups++;

				count += n_rules;
			}
			n_rules = 0;

			if(compile_mode == 2)
			{
				// end of the character replacements section
				fwrite(&n_rules,1,4,f_out);   // write a zero word to terminate the replacemenmt list
				compile_mode = 0;
			}

			if(buf == NULL) break;   // end of file

			if(memcmp(buf,".L",2)==0)
			{
				compile_lettergroup(&buf[2], f_out);
				continue;
			}

			if(memcmp(buf,".replace",8)==0)
			{
				compile_mode = 2;
				fputc(RULE_GROUP_START,f_out);
				fputc(RULE_REPLACEMENTS,f_out);

				// advance to next word boundary
				while((ftell(f_out) & 3) != 0)
					fputc(0,f_out);
			}

			if(memcmp(buf,".group",6)==0)
			{
				compile_mode = 1;

				p = (unsigned char *)&buf[6];
				while((p[0]==' ') || (p[0]=='\t')) p++;    // Note: Windows isspace(0xe1) gives TRUE !
				ix = 0;
				while((*p > ' ') && (ix < LEN_GROUP_NAME))
					group_name[ix++] = *p++;
				group_name[ix]=0;
				group3_ix = 0;

				if(sscanf(group_name,"0x%x",&char_code)==1)
				{
					// group character is given as a character code (max 16 bits)
					p = (unsigned char *)group_name;
	
					if(char_code > 0x100)
					{
						*p++ = (char_code >> 8);
					}
					*p++ = char_code;
					*p = 0;
				}
				else
				{
					if(translator->letter_bits_offset > 0)
					{
						utf8_in(&wc, group_name);
						if(((ix = (wc - translator->letter_bits_offset)) >= 0) && (ix < 128))
						{
							group3_ix = ix+1;   // not zero
						}
					}
				}
	
				if((group3_ix == 0) && (strlen(group_name) > 2))
				{
					if(utf8_in(&c,group_name) < 2)
					{
						fprintf(f_log,"%5d: Group name longer than 2 bytes (UTF8)",linenum);
						error_count++;
					}
	
					group_name[2] = 0;
				}
			}

			continue;
		}
		
		switch(compile_mode)
		{
		case 1:    //  .group
			prule = compile_rule(buf);
			if((prule != NULL) && (n_rules < N_RULES))
			{
				rules[n_rules++] = prule;
			}
			break;

		case 2:   //  .replace
			{
				int replace1;
				int replace2;
				char *p;

				p = buf;
				replace1 = 0;
				replace2 = 0;
				while(isspace2(*p)) p++;
				ix = 0;
				while((unsigned char)(*p) > 0x20)   // not space or zero-byte
				{
					p += utf8_in(&c,p);
					replace1 += (c << ix);
					ix += 16;
				}
				while(isspace2(*p)) p++;
				ix = 0;
				while((unsigned char)(*p) > 0x20)
				{
					p += utf8_in(&c,p);
					replace2 += (c << ix);
					ix += 16;
				}
				if(replace1 != 0)
				{
					Write4Bytes(f_out,replace1);   // write as little-endian
					Write4Bytes(f_out,replace2);   // if big-endian, reverse the bytes in LoadDictionary()
				}
			}
			break;
		}
	}
	fclose(f_temp);

	qsort((void *)rgroup,n_rgroups,sizeof(rgroup[0]),(int (__cdecl *)(const void *,const void *))rgroup_sorter);

	if((f_temp = fopen(fname_temp,"rb"))==NULL)
		return(2);

	prev_rgroup_name = "\n";

	for(gp = 0; gp < n_rgroups; gp++)
	{
		fseek(f_temp,rgroup[gp].start,SEEK_SET);

		if((different = strcmp(rgroup[gp].name, prev_rgroup_name)) != 0)
		{
			// not the same as the previous group
			if(gp > 0)
				fputc(RULE_GROUP_END,f_out);
			fputc(RULE_GROUP_START,f_out);

			if(rgroup[gp].group3_ix != 0)
			{
				n_groups3++;
				fputc(1,f_out);
				fputc(rgroup[gp].group3_ix, f_out);
			}
			else
			{
				fprintf(f_out, "%s", prev_rgroup_name = rgroup[gp].name);
			}
			fputc(0,f_out);
		}

		for(ix=rgroup[gp].length; ix>0; ix--)
		{
			c = fgetc(f_temp);
			fputc(c,f_out);
		}

		if(different)
		{
		}
	}
	fputc(RULE_GROUP_END,f_out);
	fputc(0,f_out);

	fclose(f_temp);
	remove(fname_temp);

	fprintf(f_log,"\t%d rules, %d groups (%d)\n\n",count,n_rgroups,n_groups3);
	return(0);
}  //  end of compile_dictrules



int CompileDictionary(const char *dsource, const char *dict_name, FILE *log, char *fname_err, int flags)
{//=====================================================================================================
// fname:  space to write the filename in case of error
// flags: bit 0:  include source line number information, for debug purposes.

	FILE *f_in;
	FILE *f_out;
	int offset_rules=0;
	int value;
	char fname_in[sizeof(path_home)+45];
	char fname_out[sizeof(path_home)+15];
	char fname_temp[sizeof(path_home)+15];
	char path[sizeof(path_home)+40];       // path_dsource+20

	error_count = 0;
	error_need_dictionary = 0;
	memset(letterGroupsDefined,0,sizeof(letterGroupsDefined));

	debug_flag = flags & 1;

	if(dsource == NULL)
		dsource = "";

	f_log = log;
//f_log = fopen("log2.txt","w");
	if(f_log == NULL)
		f_log = stderr;

	// try with and without '.txt' extension
	sprintf(path,"%s%s_",dsource,dict_name);
	sprintf(fname_in,"%srules.txt",path);
	if((f_in = fopen(fname_in,"r")) == NULL)
	{
		sprintf(fname_in,"%srules",path);
		if((f_in = fopen_log(fname_in,"r")) == NULL)
		{
			if(fname_err)
				strcpy(fname_err,fname_in);
			return(-1);
		}
	}

	sprintf(fname_out,"%s%c%s_dict",path_home,PATHSEP,dict_name);
	if((f_out = fopen_log(fname_out,"wb+")) == NULL)
	{
		if(fname_err)
			strcpy(fname_err,fname_in);
		return(-1);
	}
	sprintf(fname_temp,"%s%ctemp",path_home,PATHSEP);

	value = N_HASH_DICT;
	Write4Bytes(f_out,value);
	Write4Bytes(f_out,offset_rules);

	compile_dictlist_start();

	fprintf(f_log,"Using phonemetable: '%s'\n",phoneme_tab_list[phoneme_tab_number].name);
	compile_dictlist_file(path,"roots");
	if(translator->langopts.listx)
	{
		compile_dictlist_file(path,"list");
		compile_dictlist_file(path,"listx");
	}
	else
	{
		compile_dictlist_file(path,"listx");
		compile_dictlist_file(path,"list");
	}
	compile_dictlist_file(path,"extra");
	
	compile_dictlist_end(f_out);
	offset_rules = ftell(f_out);
	
	fprintf(f_log,"Compiling: '%s'\n",fname_in);

	compile_dictrules(f_in,f_out,fname_temp);
	fclose(f_in);

	fseek(f_out,4,SEEK_SET);
	Write4Bytes(f_out,offset_rules);
	fclose(f_out);

	LoadDictionary(translator, dict_name, 0);

	return(error_count);
}  //  end of compile_dictionary

