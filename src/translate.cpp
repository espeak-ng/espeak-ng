/***************************************************************************
 *   Copyright (C) 2005, 2006 by Jonathan Duddington                       *
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

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"

#define WORD_STRESS_CHAR   '*'


Translator *translator = NULL;
int option_log_trans = 0;
int option_tone1 = 0;
int option_tone2 = 0;
int option_stress_rule = 0;
int option_words = 0;
int option_vowel_pause;
int option_unstressed_wd1;
int option_unstressed_wd2;
int option_phonemes = 0;
FILE *f_trans = NULL;

// these are overridden by defaults set in the "speak" file
int option_speed = 160;  // 168wpm = 90speed
int option_linelength = 70;
	

int n_replace_phonemes;
REPLACE_PHONEMES replace_phonemes[N_REPLACE_PHONEMES];

extern void SetSpeed(int speed, int amp);


static const char *brackets = "()[]{}<>\"'`";

Translator::Translator()
{//=====================

	char *p;
#ifdef PLATFORM_RISCOS
	static char *locale = "ISO8859-1";
#else
	static char *locale = "german";
#endif

	// set locale to ensure 8 bit character set, for isalpha(), tolower() etc
	prev_locale[0] = 0;
	if((p = setlocale(LC_CTYPE,locale)) != NULL)
		strncpy(prev_locale,p,sizeof(prev_locale));  // keep copy of previous locale

	dict_condition=0;

	dictionary_name[0] = 0;
	
	// only need lower case
	memset(letter_bits,0,sizeof(letter_bits));

	// 0-5 sets of characters matched by A B C E F G in pronunciation rules
	// these may be set differently for different languages
	SetLetterBits(0,"aeiouÊ‰ÎÔˆ¸ˇ·ÈÌÛ˙‡ËÏÚ˘‚ÍÓÙ˚„ı¯");  // A  vowels, except y
	SetLetterBits(1,"bcdfgjklmnpqstvxzﬂÁÒ");      // B  hard consonants, excluding h,r,w
	SetLetterBits(2,"bcdfghjklmnpqrstvwxzﬂÁÒ");  // C  all consonants
	
	SetLetterBits(7,"aeiouyÊ‰ÎÔˆ¸ˇ·ÈÌÛ˙‡ËÏÚ˘‚ÍÓÙ˚„ı¯");  // vowels, including y


	SetSpeed(option_speed,-1);   // 108

	option_stress_rule = 2;
	option_vowel_pause = 1;
   option_unstressed_wd1 = 1;
   option_unstressed_wd2 = 3;

}


Translator_Esperanto::Translator_Esperanto()
{//=========================================
	// set options for Esperanto pronunciation

	// use stress=2 for unstressed words
	static int stress_lengths2[8] = {160, 160,  180, 160,  220, 240,  320, 320};

	option_stress_rule = 2;
	option_words = 2;
	option_vowel_pause = 1;
	option_unstressed_wd1 = 2;
	option_unstressed_wd2 = 2;
	memcpy(stress_lengths,stress_lengths2,sizeof(stress_lengths));
}


Translator::~Translator(void)
{//==========================
	if(prev_locale[0] != 0)
		setlocale(LC_CTYPE,prev_locale);   // return locale to previous value
}



char Translator::GetC(void)
{//=======================
	char c;

	if(f_input != NULL)
		return(fgetc(f_input));

	if((c = ungot_char) != 0)
	{
		ungot_char = 0;
		return(c);
	}

	if(*p_input == 0)
	{
		end_of_input = 1;
		return(0);
	}

	if(!end_of_input)
	{
		c = *p_input++;
		return(c);
	}
	return(0);
}

void Translator::UngetC(char c)
{//============================
	if(f_input != 0)
		ungetc(c,f_input);

	ungot_char = c;
}

int Translator::Eof(void)
{//=======================
	if(f_input != 0)
		return(feof(f_input));

	return(end_of_input);
}

int Translator::Pos(void)
{//======================
// return the current index into the input text

	if(f_input != 0)
		return(ftell(f_input));

	return(p_input - input_start);	
}


int Translator::ReadClause(FILE *f_in, char *buf, int n_buf)
{//=========================================================
/* Find the end of the current clause.
	Write the clause into  buf

	returns: 0  not end of clause (dot from abbreviation or clause too long) 
				1  paragraph (or end of file)
				else the punctuation character

	Also checks for blank line (paragraph) as end-of-clause indicator.

	Does not end clause for:
		punctuation immediately followed by alphanumeric  eg.  1.23  !Speak  :path
		repeated punctuation, eg.   ...   !!!
*/
	int c1, c2;
	int parag;
	int ix = 0;
	int nl_count;
	int linelength = 0;
	int phoneme_mode = 0;
	static const char *punctuation = ",.?!:;";     // these can end a clause

	clause_upper_count = 0;
	clause_lower_count = 0;

f_input = f_in;  // for GetC etc

	c2 = GetC();
	while(!Eof())
	{
		c1 = c2;
		c2 = GetC();
		if((c2=='\n') && (option_linelength == -1))
		{
			// single-line mode, return immediately on NL
			if(strchr(punctuation,c1) == NULL)
			{
				buf[ix++] = c1;
				c1 = 1;
			}
			buf[ix] = ' ';
			buf[ix] = 0;
			return(c1);
		}

		if(Eof())
		{
			c2 = ' ';
		}

		if(isupper(c1))
			clause_upper_count++;
		if(islower(c1))
			clause_lower_count++;

		if((c1 == '[') && (c2 == '['))
			phoneme_mode = 1;         // input is phoneme mnemonics, so don't look for punctuation
		else
		if((c1 == ']') && (c2 == ']'))
			phoneme_mode = 0;
		else
		if(c1 == '\n')
		{
			parag = 0;

			while(!Eof() && isspace(c2))
			{
				if(c2 == '\n')
				{
					// 2nd newline, assume paragraph
					buf[ix] = ' ';
					buf[ix+1] = 0;
					return(1);
				}
				c2 = GetC();
			}

			if(linelength < option_linelength)
			{
				// treat lines shorter than a specified length as end-of-clause
				UngetC(c2);
				buf[ix] = ' ';
				buf[ix] = 0;
				return(':');
			}

			linelength = 0;
		}
		if((phoneme_mode==0) && (strchr(punctuation,c1) != NULL) &&
			(isspace(c2) || strchr(brackets,c2)!=NULL || (c2=='?') || (c2=='-') || Eof()))
		{
			// note: (c2='?') is for when a smart-quote has been replaced by '?'
			buf[ix] = ' ';
			buf[ix+1] = 0;

			nl_count = 0;
			while(!Eof() && isspace(c2))
			{
				if(c2 == '\n')
					nl_count++;
				c2 = GetC();   // skip past space(s)
			}
			UngetC(c2);

			if((nl_count==0) && (c1 == '.') && (islower(c2)))
			{
				c2 = ' ';
				continue;  // next word has no capital letter, this dot is probably from an abbreviation
			}
			if(nl_count > 1)
				return(1);
			return(c1);   // only recognise punctuation if followed by a blank
		}
			
		buf[ix++] = c1;
		if(isspace(c2) && (ix > (n_buf-30)))
		{
			// clause too long, getting near end of buffer, so break here
			buf[ix] = ' ';
			buf[ix+1] = 0;
			return(0);
		}
		if(ix >= (n_buf-2))
		{
			// reached end of buffer, must break now
			buf[n_buf-2] = ' ';
			buf[n_buf-1] = 0;
			UngetC(c2);
			return(0);
		}
	}
	buf[ix] = ' ';
	buf[ix+1] = 0;
	return(2);   //  end of file
}  //  end of ReadClause



void Translator::MakePhonemeList(int post_pause)
{//=============================================
/*  
*/
	int  ix=0;
	int  j;
	int  k;
	int  insert_ph = 0;
	PHONEME_LIST *phlist;
	PHONEME_LIST2 *plist2;
	PHONEME_TAB *ph;
	PHONEME_TAB *next, *next2;
	int unstress_count = 0;
	int word_has_stress = 0;
	int max_stress;
	int word_end;
	int ph_new;

	phlist = phoneme_list;
	
	// is the last word of the clause unstressed ?
	max_stress = 0;
	for(j=n_ph_list2-1; j>=0; j--)
	{
		if((ph_list2[j].stress & 0x7f) > max_stress)
			max_stress = ph_list2[j].stress & 0x7f;
		if(ph_list2[j].sourceix != 0)
			break;
	}
	if(max_stress < 4)
	{
		// the last word is unstressed, look for a previous word that can be stressed
		while(--j >= 0)
		{
			if(ph_list2[j].stress & 0x80)  // dictionary flags indicated that this stress can be promoted
			{
				ph_list2[j].stress = 4;   // promote to stressed
				break;
			}
			if((ph_list2[j].stress & 0x7f) >= 4)
			{
				// found a stressed syllable, so stop looking
				break;
			}
		}
	}

	// transfer all the phonemes of the clause into phoneme_list
	for(j=0; j<n_ph_list2; j++)
	{
		if(insert_ph != 0)
		{
			// we have a (linking) phoneme which we need to insert here
			j--;
			ph = &phoneme_tab[insert_ph];
			insert_ph = 0;
		}
		else
		{
			// otherwise get the next phoneme from the list
			plist2 = &ph_list2[j];
			ph = &phoneme_tab[plist2->phcode];
		}
		next = &phoneme_tab[(plist2+1)->phcode];      // the phoneme after this one

		word_end = 0;
		if((plist2+1)->sourceix || next->type == phPAUSE)
			word_end = 1;        // this phoneme is the end of a word

		// check whether a Voice has specified that we should replace this phoneme
		ph_new = 1;
		for(k=0; k<n_replace_phonemes; k++)
		{
			if(ph->code == replace_phonemes[k].old_ph)
			{
				if((replace_phonemes[k].type == 1) && (word_end == 0))
					continue;     // this replacement only occurs at the end of a word

				// substitute the replacement phoneme
				ph = &phoneme_tab[ph_new = replace_phonemes[k].new_ph];
			}
		}

		if(plist2->sourceix)
			word_has_stress = 0;   // start of a word

		if(ph_new == 0)
			continue;   // phoneme has been replaced by NULL

		if(ph->type == phVOWEL)
		{
			// check for consecutive unstressed syllables
			if(plist2->stress == 0)
			{
				// an unstressed vowel
				unstress_count++;
				if((unstress_count > 1) && ((unstress_count & 1)==0))
				{
					// in a sequence of unstressed syllables, reduce alternate syllables to 'diminished'
               // stress.  But not for the last phoneme of a stressed word
					if((word_has_stress) && ((plist2+1)->sourceix!=0))
					{
						// An unstressed final vowel of a stressed word
						unstress_count=1;    // try again for next syllable
					}
					else
					{
						plist2->stress = 1;    // change stress to 'diminished'
					}
				}
			}
			else
			{
				unstress_count = 0;
				if(plist2->stress > 3)
					word_has_stress = 1;   // word has a primary or a secondary stress
			}
		}

		if(ph->vowel_follows > 0)
		{
			// This phoneme changes if vowel follows, or doesn't follow, depending on its phNOTFOLLOWS flag
			if(ph->flags & phNOTFOLLOWS)
			{
				if(next->type != phVOWEL)
					ph = &phoneme_tab[ph->vowel_follows];
			}
			else
			if(ph->flags & phORPAUSEFOLLOWS)
			{
				if((next->type == phVOWEL) || (next->type == phPAUSE))
					ph = &phoneme_tab[ph->vowel_follows];
			}
			else
			{
				if(next->type == phVOWEL)
					ph = &phoneme_tab[ph->vowel_follows];
			}
		}
		
		if((ph->link_out != 0) && (option_words < 3))
		{
			// This phoneme can be linked to a following vowel by inserting a linking phoneme
			if(next->type == phVOWEL)
				insert_ph = ph->link_out;
			else
			if((next->type == phPAUSE) && !(next->flags & phNOLINK))
			{
				// Pause followed by Vowel, replace the Pause with the linking phoneme, unless
				// the Pause phoneme has the phNOLINK flag
				next2 = &phoneme_tab[(plist2+2)->phcode];
				if(next2->type == phVOWEL)
					(plist2+1)->phcode = ph->link_out;  // replace pause by linking phoneme
			}
		}
		
		if(ph->flags & phVOICED)
		{
			// check that a voiced consonant is preceded or followed by a vowel or liquid
			// and if not, add a short schwa

			// not yet implemented
		}

		phlist[ix].ph = ph;
		phlist[ix].type = ph->type;
		phlist[ix].env = PITCHfall;          // default, can be changed in the "intonation" module
		phlist[ix].synthflags = 0;
		phlist[ix].tone = plist2->stress & 0xf;
		phlist[ix].tone_ph = plist2->tone_number;
		phlist[ix].sourceix = 0;

		if(plist2->sourceix > 0)
		{
			phlist[ix].sourceix = plist2->sourceix;
			phlist[ix].newword = 1;     // this phoneme is the start of a word
		}
		else
			phlist[ix].newword = 0;
		phlist[ix].length = ph->std_length;

		if(ph->type==phVOWEL || ph->type==phLIQUID || ph->type==phNASAL || ph->type==phVSTOP || ph->type==phVFRICATIVE)
		{
			phlist[ix].length = 128;  // length_mod
			phlist[ix].env = PITCHfall;
		}

		phlist[ix].prepause = 0;
		phlist[ix].amp = 20;          // default, will be changed later
		phlist[ix].pitch1 = 0x400;
		phlist[ix].pitch2 = 0x400;
		ix++;
	}
	phlist[ix].newword = 2;     // end of clause
   phlist[ix].type = phPAUSE;  // terminate with 2 Pause phonemes
	phlist[ix].length = post_pause;  // length of the pause, depends on the punctuation
	phlist[ix].sourceix=0;
   phlist[ix++].ph = &phoneme_tab[phonPAUSE];
   phlist[ix].type = phPAUSE;
	phlist[ix].length = 0;
	phlist[ix].sourceix=0;
   phlist[ix++].ph = &phoneme_tab[phonPAUSE];
	
	n_phoneme_list = ix;
}  // end of MakePhonemeList




void Translator::TranslateLetter(char letter, char *phonemes)
{//==========================================================
// get pronunciation for an isolated letter

	static char single_letter[6] = {0,' ',' ',' ','9',0};
	static char stress_2[2] = {phonSTRESS_2,0};
	
	single_letter[2] = letter;
	strcat(phonemes,stress_2);
	TranslateRules(&single_letter[2], phonemes, NULL,0);
}



int Translator::TranslateWord(char *word1, int next_pause, int wflags)
{//==================================================================
	int length;
	int word_length;
	int posn;
	unsigned int dictionary_flags=0;
	unsigned int dictionary_flags2=0;
	int end_type=0;
	char *word;
	char phonemes[N_WORD_PHONEMES];
	char *phonemes_ptr;
	char prefix_phonemes[N_WORD_PHONEMES];
	char end_phonemes[N_WORD_PHONEMES];
	int found;
   int end_flags;
	char c_temp;
	char last_char = 0;
	int unpron_length;
	int add_plural_suffix = 0;
	int prefix_flags = 0;

	// translate these to get pronunciations of plural 's' suffix (different forms depending on
	// the preceding letter
	static char word_zz[4] = {0,'z','z',0};
	static char word_iz[4] = {0,'i','z',0};
	static char word_ss[4] = {0,'s','s',0};

	if(option_log_trans)
	{
		// write a log file of the pronunciation translation of just this one word
		// in contrast, the "speak" program sets f_trans = stdout
		char buf[80];
		sprintf(buf,"%s/log_trans.txt",getenv("HOME"));
		f_trans = fopen(buf,"w");
	}
	
	word = word1;
	prefix_phonemes[0] = 0;

	// count the length of the word
	for(word_length=0; word[word_length]!=0 && !isspace(word[word_length]); word_length++);
	if(word_length > 0)
		last_char = word[word_length-1];

	// try an initial lookup in the dictionary list, we may find a pronunciation specified, or
	// we may just find some flags
	found = LookupDictList(word,phonemes,&dictionary_flags,wflags << 16);

	if((wflags & FLAG_ALL_UPPER) && (clause_upper_count <= clause_lower_count) &&
		 !(dictionary_flags & FLAG_ABBREV) && (word_length>1) && (word_length<4) && isalpha(word1[0]))
	{
		// An upper case word in a lower case clause. This could be an abbreviation.
		// Speak as individual letters
		word = word1;
		posn = 0;
		phonemes[0] = 0;
		end_type = 0;
		
		while(!isspace(*word))
		{
			TranslateLetter(*word++, phonemes);
			if(++posn < 5)
			{
					/* stress the last vowel (assume one syllable each - not "w") */
					dictionary_flags = posn;
			}
		}
	}
	else
	if(found == 0)
	{
		// word's pronunciation is not given in the dictionary list, although
		// dictionary_flags may have ben set there

		posn = 0;
		length = 999;
		while(((length < 3) && (length > 0))|| (word_length > 1 && Unpronouncable(word)))
		{
			// This word looks "unpronouncable", so speak letters individually until we
			// find a remainder that we can pronounce.
			TranslateLetter(*word++,phonemes);
			if(++posn < 5)
			{
					/* stress last vowel (assume one syllable each - not "w") */
					dictionary_flags = posn;
			}
			if(memcmp(word,"'s ",3) == 0)
			{
				// remove a 's suffix and pronounce this separately (not as an individual letter)
				add_plural_suffix = 1;
				word[0] = ' ';
				word[1] = ' ';
				break;
			}

			length=0;
			while(word[length] != ' ') length++;
			word[-1] = ' ';            // prevent this affecting the pronunciation of the pronuncable part
		}

		// anything left ?
		if(*word != ' ')
		{
			// Translate the stem
			unpron_length = strlen(phonemes);
			end_type = TranslateRules(word, phonemes, end_phonemes,0);

			
			c_temp = word[-1];

			found = 0;
			while(end_type & SUFX_P)
			{
				// Found a standard prefix, remove it and retranslate
				strcat(prefix_phonemes,end_phonemes);

				word += (end_type & 0xf);
				c_temp = word[-1];
				word[-1] = ' ';
			
				end_type = 0;
				found = LookupDictList(word,phonemes,&dictionary_flags2,SUFX_P | (wflags << 16));
				if(dictionary_flags==0)
					dictionary_flags = dictionary_flags2;
				else
					prefix_flags = 1;
				if(found == 0)
				{
					end_type = TranslateRules(word, phonemes, end_phonemes,0);
				}
			}
			
			if((end_type != 0) && !(end_type & SUFX_P))
			{
				// The word has a standard ending, re-translate without this ending
				end_flags = RemoveEnding(word,end_type);

				phonemes_ptr = &phonemes[unpron_length];
				phonemes_ptr[0] = 0;

				if(prefix_phonemes[0] != 0)
				{
					// lookup the stem without the prefix removed
					word[-1] = c_temp;
					found = LookupDictList(word1,phonemes_ptr,&dictionary_flags2,end_flags | (wflags << 16));
					word[-1] = ' ';
					if(dictionary_flags==0)
						dictionary_flags = dictionary_flags2;
					if(found)
						prefix_phonemes[0] = 0;  // matched whole word, don't need prefix now
					if(found || (dictionary_flags2 != 0))
						prefix_flags = 1;
				}
				if(found == 0)
				{
					found = LookupDictList(word,phonemes_ptr,&dictionary_flags2,end_flags | (wflags << 16));
					if(dictionary_flags==0)
						dictionary_flags = dictionary_flags2;
				}
				if(found == 0)
				{
					TranslateRules(word, phonemes, NULL,end_flags);
				}

				AppendPhonemes(phonemes,end_phonemes);
			}
			word[-1] = c_temp;
		}
	}

	if((add_plural_suffix) || (wflags & FLAG_HAS_PLURAL))
	{
		// s or 's suffix, append [s], [z] or [Iz] depending on previous letter
		if(last_char == 'f')
			TranslateRules(&word_ss[1],phonemes,NULL,0);
		else
		if(strchr("hsx",last_char)==NULL)
			TranslateRules(&word_zz[1],phonemes,NULL,0);
		else
			TranslateRules(&word_iz[1],phonemes,NULL,0);
	}

		
	/* determine stress pattern for this word */
	/******************************************/
	/* NOTE: this also adds a single PAUSE if the previous word ended
				in a primary stress, and this one starts with one */
	if(prefix_flags || (strchr(prefix_phonemes,phonSTRESS_P)!=NULL))
	{
		// stress position affects the whole word, including prefix
		strcpy(word_phonemes,prefix_phonemes);
		strcat(word_phonemes,phonemes);
		SetWordStress(word_phonemes,dictionary_flags,-1,prev_last_stress);
	}
	else
	{
		if(prefix_phonemes[0] == 0)
			SetWordStress(phonemes,dictionary_flags,-1,prev_last_stress);
		else
			SetWordStress(phonemes,dictionary_flags,-1,0);
		strcpy(word_phonemes,prefix_phonemes);
		strcat(word_phonemes,phonemes);
	}
	
//	if(next_pause > 2)
	if(wflags & FLAG_LAST_WORD)
	{
		if(dictionary_flags & (FLAG_STRESS_END | FLAG_STRESS_END2))
			SetWordStress(word_phonemes,0,4,prev_last_stress);
		else
		if(dictionary_flags & FLAG_UNSTRESS_END)
			SetWordStress(word_phonemes,0,3,prev_last_stress);
	}
	if(wflags & FLAG_STRESSED_WORD)
	{
		// A word is indicated in the source text as stressed

		// we need to improve the intonation module to deal better with a clauses tonic
		// stress being early in the clause, before enabling this
		//SetWordStress(word_phonemes,0,5,prev_last_stress);
	}

	// dictionary flags for this word give a clue about which alternative pronunciations of
	// following words to use.
	if(end_type & SUFX_F)
	{
		// expect a verb form, with or without -s suffix
		expect_verb = 2;
		expect_verb_s = 2;
	}
	
	if(dictionary_flags & FLAG_PASTF)
	{
		/* expect perfect tense in next two words */
		expect_past = 3;
		expect_verb = 0;
	}
	else
	if(dictionary_flags & FLAG_VERBF)
	{
		/* expect a verb in the next word */
		expect_verb = 2;
		expect_verb_s = 0;   /* verb won't have -s suffix */
	}
	else
	if(dictionary_flags & FLAG_VERBSF)
	{
		// expect a verb, must have a -s suffix
		expect_verb = 0;
		expect_verb_s = 2;
		expect_past = 0;
	}
	else
	if(dictionary_flags & FLAG_NOUNF)
	{
		/* not expecting a verb next */
		expect_verb = 0;
		expect_verb_s = 0;
		expect_past = 0;
	}

	if((word[0] != 0) && (!(dictionary_flags & FLAG_VERB_EXT)))
	{
		if(expect_verb > 0)
			expect_verb -= 1;

		if(expect_verb_s > 0)
			expect_verb_s -= 1;

		if(expect_past > 0)
			expect_past -= 1;
	}

	if((word_length == 1) && isalpha(word1[0]) && (word1[0] != 'i'))
	{
// English Specific !!!!
		// any single letter before a dot is an abbreviation, except 'I'
		dictionary_flags |= FLAG_DOT;
	}
	
	if((f_trans != NULL) && (f_trans != stdout))
		fclose(f_trans);
	return(dictionary_flags);
}  //  end of TranslateWord



int Translator::TranslateWord2(char *word, int wflags, int pre_pause, int next_pause, int source_ix)
{//=================================================================================================
	int flags=0;
	int stress;
	unsigned char *p;
	int srcix;
	unsigned char ph_code;
	PHONEME_LIST2 *plist2;
	PHONEME_TAB *ph;
	int max_stress;
	int max_stress_ix=0;
	int prev_vowel = -1;
	char bad_phoneme[4];
	
	word_flags = wflags;

	if(prepause_timeout > 0)
		prepause_timeout--;

	if(wflags & FLAG_PHONEMES)
	{
		// The input is in phoneme mnemonics, not language text
		EncodePhonemes(word,word_phonemes,bad_phoneme);
	}
	else
	{
		// Translate language word into phonemes
		flags = translator->TranslateWord(word, next_pause, wflags);
		if((flags & FLAG_PREPAUSE) && (prepause_timeout == 0) && !(wflags & FLAG_LAST_WORD))
		{
			if(pre_pause < 2) pre_pause = 2;
			prepause_timeout = 3;
		}
	}
	p = (unsigned char *)translator->word_phonemes;
	
	plist2 = &ph_list2[n_ph_list2];
	stress = 0;
	srcix = 0;
	max_stress = -1;


	while((pre_pause-- > 0) && (n_ph_list2 < N_PHONEME_LIST-2))
	{
		// add pause phonemes here. Either because of punctuation (brackets or quotes) in the
		// text, or because the word is marked in the dictionary lookup as a conjunction
		ph_list2[n_ph_list2].phcode = phonPAUSE;
		ph_list2[n_ph_list2].stress = 0;
		ph_list2[n_ph_list2].tone_number = 0;
		ph_list2[n_ph_list2++].sourceix = 0;
	}

	while(((ph_code = *p++) != 0) && (n_ph_list2 < N_PHONEME_LIST-2))
	{
		if(ph_code == 255)
			continue;      // unknown phoneme

		// Add the phonemes to the first stage phoneme list (ph_list2)
		ph = &phoneme_tab[ph_code];
		if(ph->type == phSTRESS)
		{
			// don't add stress phonemes codes to the list, but give their stress
			// value to the next vowel phoneme 
			// std_length is used to hold stress number or (if >10) a tone number for a tone language
			if(ph->spect == 0)
				stress = ph->std_length;
			else
			{
				// for tone languages, the tone number for a syllable folows the vowel
				if(prev_vowel >= 0)
					ph_list2[prev_vowel].tone_number = ph_code;
			}
		}
		else
		if(ph_code == phonEND_WORD)
		{
			// a || symbol in a phoneme string was used to indicate a word boundary
			// Don't add this phoneme to the list, but make sure the next phoneme has
			// a newword indication
			srcix = source_ix+1;
		}
		else
		{
			ph_list2[n_ph_list2].phcode = ph_code;
			ph_list2[n_ph_list2].stress = stress;
			ph_list2[n_ph_list2].tone_number = 0;
			ph_list2[n_ph_list2].sourceix = srcix;
			srcix = 0;
			
			if(ph->type == phVOWEL)
			{
				prev_vowel = n_ph_list2;

				if(stress > max_stress)
				{
					max_stress = stress;
					max_stress_ix = n_ph_list2;
				}
				stress = 0;
			}
			n_ph_list2++;
		}
	}
	plist2->sourceix = source_ix;

	if(flags & FLAG_STRESS_END2)
	{
		// this's word's stress could be increased later
		ph_list2[max_stress_ix].stress |= 0x80;
	}
	return(flags);
}  //  end of TranslateWord2




char *Translator::TranslateClause(FILE *f_text, char *buf, int *tone_out)
{//======================================================================
	int ix;
	unsigned int source_index=0;
	unsigned char c;
	char cc;
	unsigned char prev_in;
	unsigned char prev_out;
	unsigned char prev_in2=0;
	int clause_pause;
	int pre_pause=0;
	int pre_pause_add=0;
	int all_upper_case=FLAG_ALL_UPPER;
	int finished;
	int skip_words;
	int single_quoted;
	int phoneme_mode = 0;
	int dict_flags;        // returned from dictionary lookup
	int word_flags;        // set here
	char *p;

	WORD_TAB words[N_CLAUSE_WORDS];
	int word_count=0;      // index into words

	char sbuf[512];
	static const char *breaks = "_";

	int terminator;
	int tone;
	
	f_input = f_text;
	p_input = buf;
	ungot_char = 0;
	end_of_input = 0;

	clause_start_index = Pos();
	terminator = translator->ReadClause(f_text,source,sizeof(source));

	clause_pause = 300;  // mS
	tone = 0;

	switch(terminator)
	{
	case 0:
		clause_pause = 0;
		tone = 4;  // no tonic
		break;
	case 1:       // paragraph
	case 2:       // eof
		clause_pause = 600;
		break;
	case '.':
		clause_pause = 350;
		break;
	case ':':
	case ';':
		clause_pause = 300;
		break;
	case '?':
		clause_pause = 350;
		tone = 2;
		break;
	case ',':
		clause_pause = 200;
		tone = 1;
	default:
		tone = 1;
		break;
	}

	ph_list2[0].phcode = phonPAUSE;
   ph_list2[0].stress = 0;
	ph_list2[0].tone_number = 0;
	ph_list2[0].sourceix = 0;
	n_ph_list2 = 1;
	prev_last_stress = 0;
	prepause_timeout = 0;
	word_count = 0;
	single_quoted = 0;
	word_flags = 0;
	expect_verb=0;
	expect_past=0;
	expect_verb_s=0;

	sbuf[0] = 0;
	sbuf[1] = ' ';
	ix = 2;
	prev_in = ' ';

	words[0].start = ix;
	words[0].sourceix = 0;
	words[0].flags = 0;
	finished = 0;

	while(!finished && (ix < (int)sizeof(sbuf))&& (n_ph_list2 < N_PHONEME_LIST-2))
	{
		prev_out = sbuf[ix-1];
		if(prev_in2 != 0)
		{
			prev_in = prev_in2;
			prev_in2 = 0;
		}
		else
		if(source_index > 0)
			prev_in = source[source_index-1];
		cc = source[source_index++];
		c = cc;

		if(phoneme_mode)
		{
			all_upper_case = FLAG_PHONEMES;

			if((c == ']') && (source[source_index] == ']'))
			{
				phoneme_mode = 0;
				source_index++;
				c = ' ';
			}
		}
		else
		{
			if(c == 0x92)
				c = '\'';    // 'mircosoft' quote

			if((c == '?') && isalpha(prev_out) && isalpha(source[source_index]))
			{
				// ? between two letters may be a smart-quote replaced by ?
				c = '\'';
			}

			if((c == WORD_STRESS_CHAR) && (prev_out == ' ') && isalpha(source[source_index]))
			{

				// does this character end the word as well?
				for(p = &source[source_index]; isalpha(*p); p++) ;
				if(*p == c)
				{
					*p = ' ';
					c = ' ';
					// following word has explicit stress
					word_flags |= FLAG_STRESSED_WORD;
				}
			}

			if(!isalpha(c) && !isspace(c) && (c != '\''))
			{
				if(isalpha(prev_out))
				{
					c = ' ';   // ensure we have an end-of-word terminator
					source_index--;
				}
			}
			if(isdigit(prev_out))
			{
				if(!isdigit(c) && (c != '.') && (c != ','))
				{
					c = ' ';   // terminate digit string with a space
					source_index--;
				}
			}

			if((c == '[') && (source[source_index] == '['))
			{
				phoneme_mode = FLAG_PHONEMES;
				source_index++;
				continue;
			}
	
			if(c == 0)
			{
				finished = 1;
				c = ' ';
			}
			else
			if(isalpha(c))
			{
				if(!isalpha(prev_out))
				{
					if(isupper(c))
						word_flags |= FLAG_FIRST_UPPER;

					if((prev_out != ' ') && (prev_out != '\''))
					{
						// start of word, insert space if not one there already
						c = ' ';
						source_index--;  // unget
					}
					else
					if((prev_out == ' ') && isdigit(sbuf[ix-2]) && !isdigit(prev_in))
					{
						// word, following a number, but with a space between
						// Add an extra space, to distinguish "2 a" from "2a"
						sbuf[ix++] = ' ';
						words[word_count].start++;
					}
				}

				if(isupper(c))
				{
					c = tolower(c);
					if(islower(prev_in))
					{
						c = ' ';      // lower case followed by upper case, treat as new word
						prev_in2 = c;
						source_index--;  // unget
					}
					else
					if((c != ' ') && isupper(prev_in) && islower(source[source_index]) &&
							(memcmp(&source[source_index],"s ",2) != 0))
					{
						c = ' ';      // change from upper to lower case, start new word at the last uppercase
						prev_in2 = c;
						source_index--;  // unget
					}
				}
				else
				{
					if((all_upper_case) && ((ix - words[word_count].start) > 1))
					{
						if((c == 's') && (source[source_index]==' '))
						{
							c = ' ';
							all_upper_case |= FLAG_HAS_PLURAL;

							if(sbuf[ix-1] == '\'')
								sbuf[ix-1] = ' ';
						}
						else
							all_upper_case = 0;  // current word contains lower case letters, not "'s"
					}
					else
						all_upper_case = 0;
				}
			}
			else
			if(c=='-')
			{
				if(isalpha(prev_in) && isalpha(source[source_index]))
				{
					// '-' between two letters is a hyphen, treat as a space
					c = ' ';
				}
				else
				if((prev_in==' ') && (source[source_index]==' '))
				{
					// ' - ' dash between two spaces, treat as pause
					c = ' ';
					pre_pause_add = 2;
				}
				else
				if(source[source_index]=='-')
				{
					// double hyphen, treat as pause
					source_index++;
					c = ' ';
					pre_pause_add = 2;
				}
			}
			else
			if(c == '\'')
			{
				if(isalnum(prev_in) && isalpha(source[source_index]))
				{
					// between two letters, consider apostrophe as part of the word
					single_quoted = 0;
				}
				else
				{
					if((prev_out == 's') && (single_quoted==0))
					{
						// looks like apostrophe after an 's'
						c = ' ';
					}
					else
					{
						if(isspace(prev_out))
							single_quoted = 1;
						else
							single_quoted = 0;
	
						pre_pause_add = 2;   // single quote
						c = ' ';
					}
				}
			}
			else
			if(strchr(brackets,c) != NULL)
			{
				pre_pause_add = 2;
				c = ' ';
			}
			else
			if(strchr(breaks,c) != NULL)
			{
				c = ' ';  // various characters to treat as space
			}
			else
			if(isdigit(c))
			{
				if((prev_out != ' ') && !isdigit(prev_out) && (prev_out != '.'))
				{
					c = ' ';
					source_index--;
				}
				else
				if((prev_out == ' ') && isalpha(sbuf[ix-2]) && !isalpha(prev_in))
				{
					// insert extra space between a word and a number, to distinguish 'a 2' from 'a2'
					sbuf[ix++] = ' ';
					words[word_count].start++;
				}
			}
		}

		if(isspace(c))
		{
			if(prev_out == ' ')
				continue;   // multiple spaces

			// end of 'word'
			sbuf[ix++] = ' ';
			
			if((ix > words[word_count].start) && (word_count < N_CLAUSE_WORDS-1))
			{
				words[word_count].pre_pause = pre_pause;
				words[word_count].flags |= (all_upper_case | word_flags);
				word_count++;
				words[word_count].start = ix;
				words[word_count].sourceix = source_index;
				words[word_count].flags = 0;
				word_flags = 0;
				pre_pause = 0;
				all_upper_case = FLAG_ALL_UPPER;
			}
		}
		else
		{
			sbuf[ix++] = c;
		}
		if(pre_pause_add > pre_pause)
			pre_pause = pre_pause_add;
		pre_pause_add = 0;
	}

	clause_end = &sbuf[ix-1];
	words[0].pre_pause = 0;  // don't add extra pause at beginning of clause
	words[word_count].pre_pause = 4;
	if(word_count > 0)
		words[word_count-1].flags |= FLAG_LAST_WORD;

	for(ix=0; ix<word_count; ix++)
	{
		dict_flags = TranslateWord2(&sbuf[words[ix].start], words[ix].flags, words[ix].pre_pause,
		 	words[ix+1].pre_pause, words[ix].sourceix + clause_start_index);
		skip_words = (dict_flags >> 29) & 3;
		ix += skip_words;

		if((dict_flags & FLAG_DOT) && (ix == word_count-1) && (terminator == '.'))
		{
			// probably an abbreviation such as Mr. or B. rather than end of sentence
			clause_pause = 50;
			tone = 4;
		}
	}

	for(ix=0; ix<2; ix++)
	{
		// terminate the clause with 2 PAUSE phonemes
		ph_list2[n_ph_list2+ix].phcode = phonPAUSE;
   	ph_list2[n_ph_list2+ix].stress = 0;
		ph_list2[n_ph_list2+ix].sourceix = 0;
	}

	MakePhonemeList(clause_pause);
	GetTranslatedPhonemeString(phon_out,sizeof(phon_out));
	*tone_out = tone;

	if(Eof() || (buf==NULL))
		return(NULL);
	return(p_input-1);
}  //  end of TranslateClause

