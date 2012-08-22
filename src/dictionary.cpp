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

#define LOG_TRANSLATE
 
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <wctype.h>

#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"



static char *data_dictrules = NULL;     // language_1   translation rules file
static char *data_dictlist = NULL;      // language_2   dictionary lookup file
static char *dict_hashtab[N_HASH_DICT];   // hash table to index dictionary lookup file

// groups1 and groups2 are indexes into data_dictrules, set up by InitGroups()
// the two-letter rules for each letter must be consecutive in the language_rules source

static char *groups1[256];         // translation rule lists, index by single letter
static char *groups2[N_CHAINS2];   // translation rule lists, indexed by two-letter pairs
static unsigned int groups2_name[N_CHAINS2];  // the two letter pairs for groups2[]
static int n_groups2;              // number of groups2[] entries used

static char groups2_count[256];    // number of 2 letter groups for this initial letter
static char groups2_start[256];    // index into groups2

char dictionary_name[40];

// flag bits for each character code, for vowel, consonant, etc
unsigned char letter_bits[256];

// convert characters to an approximate 7 bit ascii equivalent
// used for checking for vowels
static char remove_accent[] = {
'a','a','a','a','a','a','a','c','e','e','e','e','i','i','i','i',  // 0c0
'd','n','o','o','o','o','o', 0, 'o','u','u','u','u','y','t','s',  // 0d0
'a','a','a','a','a','a','a','c','e','e','e','e','i','i','i','i',  // 0e0
'd','n','o','o','o','o','o', 0 ,'o','u','u','u','u','y','t','y',  // 0f0

'a','a','a','a','a','a','c','c','c','c','c','c','c','c','d','d',  // 100
'd','d','e','e','e','e','e','e','e','e','e','e','g','g','g','g',  // 110
'g','g','g','g','h','h','h','h','i','i','i','i','i','i','i','i',  // 120
'i','i','i','i','j','j','k','k','k','l','l','l','l','l','l','l',  // 130
'l','l','l','n','n','n','n','n','n','n','n','n','o','o','o','o',  // 140
'o','o','o','o','r','r','r','r','r','r','s','s','s','s','s','s',  // 150
's','s','t','t','t','t','t','t','u','u','u','u','u','u','u','u',  // 160
'u','u','u','u','w','w','y','y','y','z','z','z','z','z','z','s',  // 170
'b','b','b','b', 0,  0, 'o','c','c','d','d','d','d','d','e','e',  // 180
'e','f','f','g','g','h','i','i','k','k','l','l','m','n','n','o',  // 190
'o','o','o','o','p','p','y', 0,  0, 's','s','t','t','t','t','u',  // 1a0
'u','u','v','y','y','z','z','z','z','z','z','z', 0,  0,  0, 'w',  // 1b0
't','t','t','k','d','d','d','l','l','l','n','n','n','a','a','i',  // 1c0
'i','o','o','u','u','u','u','u','u','u','u','u','u','e','a','a',  // 1d0
'a','a','a','a','g','g','g','g','k','k','o','o','o','o','z','z',  // 1e0
'j','d','d','d','g','g','w','w','n','n','a','a','a','a','o','o',  // 1f0

'a','a','a','a','e','e','e','e','i','i','i','i','o','o','o','o',  // 200
'r','r','r','r','u','u','u','u','s','s','t','t','y','y','h','h',  // 210
'n','d','o','o','z','z','a','a','e','e','o','o','o','o','o','o',  // 220
'o','o','y','y','l','n','t','j','d','q','a','c','c','l','t','s',  // 230
'z', 0 };




void strncpy0(char *to,const char *from, int size)
{//===============================================
	// strcpy with limit, ensures a zero terminator
	strncpy(to,from,size);
	to[size-1] = 0;
}




//=============================================================================================
//   Read pronunciation rules and pronunciation lookup dictionary
//
//=============================================================================================



static int LoadDictFile(const char *name, const char *suffix, char **data_ptr)
{//===========================================================================
// Load a pronunciation data file into memory
	FILE *f;
	char *ptr;
	int size;
	char fname[120];

	sprintf(fname,"%s%c%s%s",path_home,PATHSEP,name,suffix);
	size = GetFileLength(fname);

	f = fopen(fname,"rb");
	if(f == NULL)
	{
		fprintf(stderr,"Can't read data file: '%s'\n",fname);
		return(1);
	}
	
	if(*data_ptr != NULL)
		Free(*data_ptr);

	ptr = Alloc(size);
	fread(ptr,size,1,f);
	fclose(f);
	*data_ptr = ptr;
	return(0);
}  // end of LoadDictFile


#ifdef PLATFORM_RISCOS
void FreeDictionary(void)
{//======================
	Free(data_dictrules);
	Free(data_dictlist);
}
#endif


int Translator::LoadDictionary(const char *name)
{//=============================================
	int hash;
	char *p;
	int length;

	// load pronunciation rules
	if(LoadDictFile(name,"_1",&data_dictrules) != 0)
		return(2);
		
	// load pronunciation lookup dictionary
	if(LoadDictFile(name,"_2",&data_dictlist) != 0)
		return(1);

	// set up indices into data_dictrules
	InitGroups();
	if(groups1[0] == NULL)
	{
		fprintf(stderr,"Error in %s_rules, no default rule group\n",name);
	}

	// set up hash table for data_dictlist
	p = &data_dictlist[4];

	for(hash=0; hash<N_HASH_DICT; hash++)
	{
		dict_hashtab[hash] = p;
		while((length = *p) != 0)
		{
			p += length;
		}
		p++;   // skip over the zero which terminates the list for this hash value
	}

	strcpy(dictionary_name,name);   // currently loaded dictionary name
	return(0);
}  //  end of LoadDictionary


void Translator::InitGroups(void)
{//==============================
/* Called after dictionary 1 is loaded, to set up table of entry points for translation rule chains
	for single-letters and two-letter combinations
*/

	int  ix;
	char *p;
	char *p_name;
	unsigned char c, c2;
	int len;
	int rule_count;

	n_groups2 = 0;
	for(ix=0; ix<256; ix++)
	{
		groups1[ix]=NULL;
		groups2_count[ix]=0;
		groups2_start[ix]=-1;
	}

	p = data_dictrules;
	while(*p != 0)
	{
		if(*p != RULE_GROUP_START)
		{
			fprintf(stderr,"Bad rules data in '%s_1' at 0x%x\n",dictionary_name,p-data_dictrules);
			break;
		}
		p++;
		len = strlen(p);
		p_name = p;
		c = p_name[0];
		
		p += (len+1);
		if(len == 1)
		{
			groups1[c] = p;
		}
		else
		if(len == 0)
		{
			groups1[0] = p;
		}
		else
		{
			if(groups2_start[c] == -1)
				groups2_start[c] = n_groups2;

			groups2_count[c]++;
			groups2[n_groups2] = p;
			c2 = p_name[1];
			groups2_name[n_groups2++] = (c + (c2 << 8));
		}
		// skip over all the rules in this group
		rule_count = 0;
		while(*p != RULE_GROUP_END)
		{
			p += (strlen(p) + 1);
			rule_count++;
		}
		p++;
	}

}  //  end of InitGroups


int HashDictionary(const char *string)
//====================================
/* Generate a hash code from the specified string
	This is used to access the dictionary_2 word-lookup dictionary
*/
{
   int  c;
	int  chars=0;
   int  hash=0;

   while((c = (*string++ & 0xff)) != 0)
   {
      hash = hash * 8 + c;
      hash = (hash & 0x3ff) ^ (hash >> 8);    /* exclusive or */
		chars++;
   }

   return((hash+chars) & 0x3ff);  // a 10 bit hash code
}   //  end of HashDictionary



//=============================================================================================
//   Translate between internal representation of phonemes and a mnemonic form for display
//
//=============================================================================================



char *EncodePhonemes(char *p, char *outptr, char *bad_phoneme)
/************************************************************/
/* Translate a phoneme string from ascii mnemonics to internal phoneme numbers,
   from 'p' up to next blank .
   Returns advanced 'p'
   outptr contains encoded phonemes, unrecognised phonemes are encoded as 255
   bad_phoneme must point to char array of length 2 of more
*/
{
	PHONEME_TAB *ph_ptr;
	unsigned char  c;
	int  count;    /* num. of matching characters */
	int  max;      /* highest num. of matching found so far */
	int  max_ph;   /* corresponding phoneme with highest matching */
	int  consumed;
	unsigned int  mnemonic_word;

	bad_phoneme[0] = 0;

	// skip initial blanks
	while(isspace(*p))
	{
		p++;
	}

	while(((c = *p) != 0) && !isspace(c))
	{
		consumed = 0;

		switch(c)
		{
		case '|':
			// used to separate phoneme mnemonics if needed, to prevent characters being treated
			// as a multi-letter mnemonic

			if((c = p[1]) == '|')
			{
				// treat double || as a word-break symbol, drop through
            // to the default case with c = '|'
			}
			else
			{
				p++;
				break;
			}

		default:
			// lookup the phoneme mnemonic, find the phoneme with the highest number of
			// matching characters
			max= -1;
			max_ph= 0;
			for(ph_ptr = &phoneme_tab[1]; 
				(mnemonic_word = ph_ptr->mnemonic) != 0; ph_ptr++)
			{
				if(ph_ptr->type == phINVALID)
					continue;       // this phoneme is not defined for this language

				count = 0;

				while(((c = p[count]) > ' ') && (count < 4) &&
										(c == ((mnemonic_word >> (count*8)) & 0xff)))
					count++;

				if((count > max) &&
					((count == 4) || (((mnemonic_word >> (count*8)) & 0xff)==0)))
				{
					max = count;
					max_ph = ph_ptr->code;
				}
			}

			if(max_ph == 0)
			{
				max_ph = 255;   /* not recognised */
				bad_phoneme[0] = *p;
				bad_phoneme[1] = 0;
			}

			if(max <= 0)
				max = 1;
			p += (consumed + max);
			*outptr++ = (char)(max_ph);
			break;
		}
	}
	/* terminate the encoded string */
	*outptr = 0;
	return(p);
}   // end of EncodePhonemes



void DecodePhonemes(const char *inptr, char *outptr)
//==================================================
// Translate from internal phoneme codes into phoneme mnemonics
{
	unsigned char c;
	unsigned int  mnem;
	
	while((c = *inptr++) > 0)
	{
		if(c == 255)
			continue;     /* indicates unrecognised phoneme */
	
		mnem = phoneme_tab[c].mnemonic;
	
		while((*outptr = (mnem & 0xff)) != 0)
		{
			outptr++;
			mnem = mnem >> 8;
		}
	}
	*outptr = 0;    /* string terminator */
}   //  end of DecodePhonemes






void Translator::GetTranslatedPhonemeString(char *phon_out, int n_phon_out)
{//========================================================================
/* Can be called after a clause has been translated into phonemes, in order
   to display the clause in phoneme mnemonic form.
*/

	int  ix;
	int  phon_out_ix=0;
	unsigned char c;
	int  stress;
	unsigned int mnem;
	PHONEME_LIST *plist;
	
	static const char *stress_chars = "==,,'*";

	if(phon_out != NULL)
	{
		for(ix=1; ix<(n_phoneme_list-2) && (phon_out_ix < (n_phon_out - 6)); ix++)
		{
			plist = &phoneme_list[ix];
			if(plist->newword)
				phon_out[phon_out_ix++] = ' ';
			if(plist->type == phVOWEL)
			{
				if((stress = plist->tone) > 1)
				{
					if(stress > 5) stress = 5;
					phon_out[phon_out_ix++] = stress_chars[stress];
				}
			}
			mnem = plist->ph->mnemonic;
			while((c = mnem & 0xff) != 0)
			{
				phon_out[phon_out_ix++]= c;
			//	phon_out[phon_out_ix++]= ipa1[c];
				mnem = mnem >> 8;
			}
		}
	
		if(phon_out_ix >= n_phon_out)
			phon_out_ix = n_phon_out - 1;
		phon_out[phon_out_ix] = 0;
	}
}  // end of Translator::GetTranslatedPhonemeString



//=============================================================================================
//   Is a word Unpronouncable - and so should be spoken as individual letters
//
//=============================================================================================


#ifdef deleted
// this is the initials_bitmap for english
static char initials_bitmap[86] = {
 0x00, 0x00, 0x00, 0x00, 0x22, 0x08, 0x00, 0x88,  //  0
 0x20, 0x24, 0x20, 0x80, 0x10, 0x00, 0x00, 0x00,
 0x00, 0x28, 0x08, 0x00, 0x88, 0x22, 0x04, 0x00,  // 16
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x88, 0x22, 0x04, 0x00, 0x02, 0x00, 0x00,  // 32
 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x28, 0x8a, 0x03, 0x00, 0x00, 0x40, 0x00,  // 48
 0x02, 0x00, 0x41, 0xca, 0x9b, 0x06, 0x20, 0x80,
 0x91, 0x00, 0x00, 0x00, 0x00, 0x20, 0x08, 0x00,  // 64
 0x08, 0x20, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x22, 0x00, 0x01, 0x00, };
#endif


int Translator::Unpronouncable(char *word)
{//=======================================
/* Determines whether a word in 'unpronouncable', i.e. whether it should
	be spoken as individual letters.

	This function is language specific.
*/

	unsigned char  c;
	int  vowel_posn=9;
	int  index;
	int  ix;
	int  apostrophe=0;

	// words which we pass through to the dictionary, even though they look unpronouncable
	static const char *exceptions[] = {
		NULL };

	if((*word == ' ') || (*word == 0))
		return(0);

	for(ix=0; exceptions[ix] != NULL; ix++)
	{
		// Seemingly uncpronouncable words, but to be looked in the dictionary rules instead
		if(memcmp(word,exceptions[ix],3)==0)
			return(0);
	}

	index=0;
	while(((c = word[index++]) != 0) && !isspace(c))
	{
		if(IsVowel(c) || (c == 'y'))
		{
			vowel_posn = index;    // position of the first vowel
			break;
		}

		if(c == '\'')
			apostrophe = 1;
		else
		if((c < 'a') || (c > 'z'))
			return(0);        // letter (not vowel) outside a-z range or apostrophe, abort test
	}
	if((vowel_posn > 5) || ((word[0]!='s') && (vowel_posn > 4)))
		return(1);  // no vowel, or no vowel in first four letters

return(0);

// disable table lookup in the generic version of this function
#ifdef deleted
	/* there is at least one vowel, is the initial letter combination valid ? */

	if(vowel_posn < 3)
		return(0);   /* vowel in first two letters, OK */

	if(apostrophe)
		return(0);   // first two letters not a-z, abort test

	index = (word[0]-'a') * 26 + (word[1]-'a');
	if(initials_bitmap[index >> 3] & (1L << (index & 7)))
		return(0);
	else
		return(1);   // OK
#endif

}   /* end of Unpronounceable */





int Translator::IsLetter(int letter, int group)
{//============================================
	if(letter < 0x80)
		return(letter_bits[letter] & (1L << group));

	if((letter >= 0xc0) && (letter <= 0x241))
		return(letter_bits[remove_accent[letter-0xc0]] & (1L << group));

	return(0);
}


int Translator::IsVowel(int letter)
{//================================
	return(letter_bits[letter] & 0x01);
}


void Translator::SetLetterBits(int group, const char *string)
{//==========================================================
	int bits;
	unsigned char c;
	
	bits = (1L << group);
	while((c = *string++) != 0)
		letter_bits[c] |= bits;
}




//=============================================================================================
//   Determine the stress pattern of a word
//
//=============================================================================================



int Translator::GetVowelStress(char *phonemes, char *vowel_stress, int *vowel_count, int stressed_syllable)
{//=========================================================================================================

	unsigned char phcode;
	int count = 1;
	int max_stress = 0;
	int ix;
	int stress = 0;

	vowel_stress[0] = 0;
	while((phcode = *phonemes++) != 0)
	{
		if(phoneme_tab[phcode].type == phSTRESS)
		{
			/* stress marker, use this for the following vowel */

			if(phcode == phonSTRESS_PREV)
			{
				/* primary stress on preceeding vowel */
				if((count > 1) && (stressed_syllable == 0))
				{
					vowel_stress[count-1] = 4;
					max_stress = 4;

					/* reduce any preceding primary stress markers */
					for(ix=1; ix<(count-1); ix++)
					{
						if(vowel_stress[ix] == 4)
							vowel_stress[ix] = 3;
					}
				}
			}
			else
			{
				stress = phoneme_tab[phcode].std_length;

				if(stress > max_stress)
					max_stress = stress;
			}
			continue;
		}

		if(phoneme_tab[phcode].type == phVOWEL)
		{
			vowel_stress[count] = (char)stress;

			if((stress == 0) && (phoneme_tab[phcode].flags & phUNSTRESSED))
				vowel_stress[count] = 1;   /* weak vowel, must be unstressed */

			count++;
			stress = 0;
		}
	}
	vowel_stress[count] = 0;
	*vowel_count = count;

	/* has the position of the primary stress been specified ? */
	if(stressed_syllable > 0)
	{
		vowel_stress[stressed_syllable] = 4;
		max_stress = 4;
	}


	return(max_stress);
}  // end of GetVowelStress



void Translator::SetWordStress(char *output, unsigned int dictionary_flags, int tonic, int prev_stress)
{//===================================================================================================
/* Guess stress pattern of word.  This is language specific

   'dictionary_flags' has bits 0-3   position of stressed vowel (if > 0)
                                     or unstressed (if == 7)
                          bits 8...  dictionary flags

   If 'tonic' is set (>= 0), replace highest stress by this value.

   Parameter used for input and output
*/

	unsigned char phcode;
	char *p;
	int  stress;
	int  max_stress;
	int  vowel_count;
	int  v;
	int  v_stress;
	int  stressed_syllable;      // position of stressed syllable
	int  max_stress_posn;
	int  unstressed_word = 0;
	char *max_output;


	char vowel_stress[N_WORD_PHONEMES/2];
	char phonetic[N_WORD_PHONEMES];

	static char stress_phonemes[] = {phonSTRESS_U, phonSTRESS_D, phonSTRESS_2, phonSTRESS_3,
		phonSTRESS_P, phonSTRESS_TONIC, phonSTRESS_TONIC};

	/* stress numbers  STRESS_BASE +
		0  diminished, unstressed within a word
		1  unstressed, weak
		2
		3  secondary stress
		4  main stress */

	/* copy input string into internal buffer */
	strcpy(phonetic,output);
	max_output = output + (N_WORD_PHONEMES-3);   /* check for overrun */

	/* note the marked stress for all vowels */
	stressed_syllable = dictionary_flags & 0x7;
	if(stressed_syllable == 7)
	{
		/* this indicates a word with secondary stress only */
		stressed_syllable = 0;
		unstressed_word = 1;
	}

	max_stress = GetVowelStress(phonetic,vowel_stress,&vowel_count,stressed_syllable);

	if(option_stress_rule == 2)
	{
		// language (eg Welsh) with stress on penultimate vowel
		if(stressed_syllable == 0)
		{
			/* no explicit stress - stress the penultimate vowel */
			if(vowel_count > 2)
				stressed_syllable = vowel_count - 2;
			else
				stressed_syllable = 1;

			// don't stress if next and prev syllables are stressed
			if((vowel_stress[stressed_syllable-1] < 4) || (vowel_stress[stressed_syllable+1] < 4))
				vowel_stress[stressed_syllable] = 4;
			max_stress = 4;
		}
	}
   if(option_stress_rule == 3)
	{
		// stress on last vowel
		if(stressed_syllable == 0)
		{
			/* no explicit stress - stress the penultimate vowel */
			stressed_syllable = vowel_count - 1;
			vowel_stress[stressed_syllable] = 4;
			max_stress = 4;
		}
	}


	/* now guess the complete stress pattern */
	if(max_stress < 4)
		stress = 4;  /* no primary stress marked, use for 1st syllable */
	else
		stress = 3;



	for(v=1; v<vowel_count; v++)
	{
		if(vowel_stress[v] == 0)
		{
			if((vowel_stress[v-1] <= 1) && (vowel_stress[v+1] <= 1))
			{
				/* give stress to vowel surrounded by unstressed vowels */
				vowel_stress[v] = (char)stress;
				stress = 3;  /* use secondary stress for remaining syllables */
			}
		}
	}

	if((unstressed_word) && (tonic < 0))
	{
		if(vowel_count <= 2)
			tonic = option_unstressed_wd1;   /* monosyllable - unstressed */
		else
			tonic = option_unstressed_wd2;   /* more than one syllable, used secondary stress as the main stress */
	}

	if(tonic >= 0)
	{
		/* find position of highest stress, and replace it by 'tonic' */
		max_stress = 0;
		max_stress_posn = 0;
		for(v=1; v<vowel_count; v++)
		{
			if(vowel_stress[v] >= max_stress)
			{
				max_stress = vowel_stress[v];
				max_stress_posn = v;
			}
		}

		/* don't disturb an explicitly set stress by 'unstress-at-end' flag */
		if((tonic > max_stress) || (max_stress <= 4))
			vowel_stress[max_stress_posn] = (char)tonic;
	}


	/* produce output phoneme string */
	p = phonetic;
	v = 1;

	if(phoneme_tab[*p].type == phSTRESS)
		p++;
	else
	if(((vowel_stress[1] >= 4) && (prev_stress >= 4)) || (option_words >= 3))
	{
		/* two primary stresses together, insert a short pause */
		if(option_words == 4)
			*output++ = phonPAUSE;
		else
			*output++ = phonPAUSE_SHORT;
	}
	else
	if((option_vowel_pause) && (vowel_stress[1] >= 4) && (phoneme_tab[*p].type == phVOWEL))
	{
		// word starts with a vowel
		*output++ = phonPAUSE_SHORT;
	}

	p = phonetic;
	while(((phcode = *p++) != 0) && (output < max_output))
	{
		if(phoneme_tab[phcode].type == phSTRESS)
			continue;

		if(phoneme_tab[phcode].type == phPAUSE)
		{
			prev_last_stress = 0;
		}
		else
		if(phoneme_tab[phcode].type == phVOWEL)
		{
			v_stress = vowel_stress[v];
			prev_last_stress = v_stress;

			if((v_stress <= 1) && ((v == 1) || (v == (vowel_count-1))))
			{
				/* Unstressed, but not diminished for being within a word */
			}
			else
			{
				if(v_stress <= 1)
				{
					v_stress = 1;      /* diminished stress */

					if(!(dictionary_flags & FLAG_FOUND))
					{
						/* reduce unstressed-diminished to schwa */
						if((vowel_stress[v+1] > 1) || (v < (vowel_count-2)))
						{
							/* not if followed by another, terminating, unstessed syllable */
							if(phoneme_tab[phcode].reduce_to != 0)
							{
								phcode = phoneme_tab[phcode].reduce_to;

								if(*p == phonLENGTHEN)
								{
									/* delete length indicator after vowel now that it has been reduced */
									p++;
								}
							}
						}
					}
				}
				if(v_stress > 0)
					*output++ = stress_phonemes[v_stress];
			}
			v++;
		}

		*output++ = phcode;
	}
	*output++ = 0;

}  /* end of SetWordStress */




//=============================================================================================
//   Look up a word in the pronunciation rules
//
//=============================================================================================


#ifdef LOG_TRANSLATE
char *Translator::DecodeRule(const char *group, char *rule)
{//==================================================
/* Convert compiled match template to ascii */

   unsigned char rb;
	unsigned char c;
	char *p;
   int  ix;
	int  match_type;
	int  finished=0;
	int  condition_num=0;
   char buf[60];
   char buf_pre[60];
	char suffix[12];
	static char output[60];

	static char symbols[] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',
			'@','&','%','+','#','S','D','Z','A','B','C','E','F','G','N'};


	match_type = 0;
   buf_pre[0] = 0;
	strcpy(buf,group);
	p = &buf[strlen(buf)];
   while(!finished)
   {
		rb = *rule++;

		if(rb <= 5)
		{
			switch(rb)
			{
			case 0:
			case RULE_PHONEMES:
				finished=1;
				break;
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
			}
			continue;
		}
		
		if(rb == RULE_ENDING)
		{
			if(rule[0] & (SUFX_P>>8))
				sprintf(suffix,"P%d",rule[1]);
			else
				sprintf(suffix,"$%d(%x)",rule[1],rule[0] & 0x7f);
			rule += 2;
			strcpy(p,suffix);
			p += strlen(suffix);
			c = ' ';
		}
		else
		if(rb <= RULE_LETTER4)
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
	if(condition_num > 0)
	{
		sprintf(output,"?%d ",condition_num);
		p = &output[strlen(output)];
	}
	if((ix = strlen(buf_pre)) > 0)
	{
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
}   /* end of decode_match */
#endif



void Translator::AppendPhonemes(char *string, const char *ph)
{//==========================================================
/* Add new phoneme string "ph" to "string"
	Keeps count of the number of vowel phonemes in the word, and whether these
   can be stressed syllables.  These values can be used in translation rules
*/	
	const char *p;
	unsigned char  c;
	int  unstress_mark;
	
	/* any stressable vowel ? */
	unstress_mark = 0;
	p = ph;
	while((c = *p++) != 0)
	{
		if(phoneme_tab[c].type == phSTRESS)
		{
			if(phoneme_tab[c].std_length < 4)
				unstress_mark = 1;
		}
		else
		{
			if(phoneme_tab[c].type == phVOWEL)
			{
				if(((phoneme_tab[c].flags & phUNSTRESSED) == 0) &&
					(unstress_mark == 0))
				{
					word_stressed_count++;
				}
				unstress_mark = 0;
				word_vowel_count++;
			}
		}
	}
	
	if(string != NULL)
		strcat(string,ph);
}   /* end of AppendPhonemes */



void Translator::MatchRule(char *word[], const char *group, char *rule, MatchRecord *match_out, int end_flags)
{//==========================================================================================================
/* Checks a specified word against dictionary rules.
	Returns with phoneme code string, or NULL if no match found.

	word (indirect) points to current character group within the input word
			This is advanced by this procedure as characters are consumed

	group:  the initial characters used to choose the rules group

	rule:  address of dictionary rule data for this character group

	match_out:  returns best points score

	end_flags:  indicates whether this is a retranslation after a suffix has been removed
*/

	unsigned char rb;     // current instuction from rule
	unsigned char letter;   // current letter from input word, single byte
	int letter_w;         // current letter, wide character
	int letter_xbytes;    // number of extra bytes of multibyte character (num bytes - 1)
	unsigned char last_letter;

	char *pre_ptr;
	char *post_ptr;       /* pointer to first character after group */

	char *rule_start;       /* start of current match template */
	char *p;

	int  match_type;      /* left, right, or consume */
	int  failed;
	int  consumed;        /* number of letters consumed from input */
	int  count;           /* count through rules in the group */
	int  syllable_count;
	int  vowel;
	int  distance_right;
	int  distance_left;

	MatchRecord match;
	static MatchRecord best;

	int  total_consumed;  /* letters consumed for best match */
	int  group_length;

	unsigned char condition_num;
	char *common_phonemes;  /* common to a group of entries */



	if(rule == NULL)
	{
		match_out->points = 0;
		return;
	}


	total_consumed = 0;
	count = 0;
	common_phonemes = NULL;
	match_type = 0;

	best.points = 0;
	best.phonemes = "";
	best.end_type = 0;
	best.del_fwd = NULL;

	group_length = strlen(group);
	
	/* search through dictionary rules */
	while(rule[0] != RULE_GROUP_END)
	{
		match_type=0;
		consumed = 0;
		letter = 0;
		distance_right= -6;   /* used to reduce points the further away */
		distance_left= -2;
		count++;

		match.points = 1;
		match.end_type = 0;
		match.del_fwd = NULL;
		
		pre_ptr = *word;
		post_ptr = *word + group_length;

		/* work through next rule until end, or until no-match proved */
		rule_start = rule;
		failed = 0;
		while(!failed)
		{
			rb = *rule++;

			if(rb <= 5)
			{
				switch(rb)
				{
				case 0:  // no phoneme string for this rule, use previous common rule
					if(common_phonemes != NULL)
					{
						match.phonemes = common_phonemes;
						while(((rb = *match.phonemes++) != 0) && (rb != RULE_PHONEMES));
					}
					else
					{
						match.phonemes = "";
					}
					rule--;      // so we are still pointing at the 0
					failed=2;    // matched OK
					break;
				case RULE_PRE:
					match_type = RULE_PRE;
					break;
				case RULE_POST:
					match_type = RULE_POST;
					break;
				case RULE_PHONEMES:
					match.phonemes = rule;
					failed=2;     // matched OK
					break;
				case RULE_PH_COMMON:
					common_phonemes = rule;
					break;
				case RULE_CONDITION:
					/* conditional rule, next byte gives condition number */
					condition_num = *rule++;
					if((dict_condition & (1L << condition_num))==0)
						failed = 1;
					break;
				}
				continue;
			}

			switch(match_type)
			{
			case 0:
				/* match and consume this letter */
				last_letter = letter;
				letter = *post_ptr++;

				if((letter == rb) || ((letter==(unsigned char)REPLACED_E) && (rb=='e')))
				{
					match.points += 21;
					consumed++;
				}
				else
					failed = 1;
				break;


			case RULE_POST:
				/* continue moving fowards */
				distance_right += 6;
				if(distance_right > 18)
					distance_right = 19;
				last_letter = letter;
				letter_xbytes = utf8_in(&letter_w,post_ptr,0)-1;
				letter = *post_ptr++;

				switch(rb)
				{
				case RULE_LETTER1:
				case RULE_LETTER2:
				case RULE_LETTER3:
				case RULE_LETTER4:
					if(IsLetter(letter_w,rb-RULE_LETTER1))
					{
						match.points += (20-distance_right);
						post_ptr += letter_xbytes;
					}
					else
						failed = 1;
					break;

				case RULE_DIGIT:
					if(iswdigit(letter_w))
					{
						match.points += (21-distance_right);
						post_ptr += letter_xbytes;
					}
					else
						failed = 1;
					break;
					
				case RULE_NONALPHA:
					if(!iswalpha(letter_w))
					{
						match.points += (21-distance_right);
						post_ptr += letter_xbytes;
					}
					else
						failed = 1;
					break;

				case RULE_DOUBLE:
					if(letter == last_letter)
						match.points += (21-distance_right);
					else
						failed = 1;
					break;

				case RULE_SYLLABLE:
					/* more than specified number of vowel letters to the right */
					syllable_count = 1;
					while(*rule == RULE_SYLLABLE)
					{
						rule++;
						syllable_count+=1;   /* number of syllables to match */
					}
					vowel = 0;
					while(letter != RULE_SPACE)
					{
						if((vowel==0) && IsLetter(letter,LETTERGP_VOWEL_Y))
						{
							syllable_count--;
						}
						vowel = IsLetter(letter,LETTERGP_VOWEL_Y);
						letter = *post_ptr++;
					}
					if(syllable_count <= 0)
						match.points+= (19-distance_right);
					else
						failed = 1;
					break;

				case RULE_INC_SCORE:
					match.points += 20;      // force an increase in points
					break;

				case RULE_DEL_FWD:
					// find the next 'e' in the word and replace by ''
					for(p = *word + group_length; *p != ' '; p++)
					{
						if(*p == 'e')
						{
							match.del_fwd = p;
							break;
						}
					}
					break;

				case RULE_ENDING:
					// next 2 bytes are a (non-zero) ending type
					match.end_type = (rule[0] << 8) + rule[1];
					rule += 2;
					break;

				case RULE_NO_SUFFIX:
					if(end_flags & FLAG_SUFX)
						failed = 1;             // a suffix has been removed
					break;

				default:
					if(letter == rb)
					{
						if(letter == RULE_SPACE)
							match.points += (21-distance_right);
						else
							match.points += (21-distance_right);
					}
					else
						failed = 1;
					break;
				}
				break;


			case RULE_PRE:
				/* match backwards from start of current group */
				distance_left += 2;
				if(distance_left > 18)
					distance_left = 19;

				last_letter = *pre_ptr;
				pre_ptr--;
				letter_xbytes = utf8_in(&letter_w,pre_ptr,1)-1;
				letter = *pre_ptr;

				switch(rb)
				{
				case RULE_LETTER1:
				case RULE_LETTER2:
				case RULE_LETTER3:
				case RULE_LETTER4:
					if(IsLetter(letter_w,rb-RULE_LETTER1))
					{
						match.points += (20-distance_left);
						pre_ptr -= letter_xbytes;
					}
					else
						failed = 1;
					break;

				case RULE_DOUBLE:
					if(letter == last_letter)
						match.points += (21-distance_left);
					else
						failed = 1;
					break;

				case RULE_DIGIT:
					if(iswdigit(letter_w))
					{
						match.points += (21-distance_left);
						pre_ptr -= letter_xbytes;
					}
					else
						failed = 1;
					break;

				case RULE_NONALPHA:
					if(!iswalpha(letter_w))
					{
						match.points += (21-distance_right);
						pre_ptr -= letter_xbytes;
					}
					else
						failed = 1;
					break;

				case RULE_SYLLABLE:
					/* more than specified number of vowels to the left */
					syllable_count = 1;
					while(*rule == RULE_SYLLABLE)
					{
						rule++;
						syllable_count++;   /* number of syllables to match */
					}
					if(syllable_count <= word_vowel_count)
						match.points+= (19-distance_left);
					else
						failed = 1;
					break;

				case RULE_STRESSED:
					if(word_stressed_count > 0)
						match.points += 19;
					else
						failed = 1;
					break;

				case '.':
					// dot in pre- section, match on any dot before this point in the word
					for(p=pre_ptr; *p != ' '; p--)
					{
						if(*p == '.')
						{
							match.points +=50;
							break;
						}
					}
					if(*p == ' ')
						failed = 1;
					break;

				default:
					if(letter == rb)
					{
						if(letter == RULE_SPACE)
							match.points += 2;
						else
							match.points += (21-distance_left);
					}
					else
						failed = 1;
					break;
				}
				break;
			}
		}

		if(failed == 2)
		{
			/* matched OK, is this better than the last best match ? */
			if(match.points >= best.points)
			{
				memcpy(&best,&match,sizeof(match));
				total_consumed = consumed;
			}

#ifdef LOG_TRANSLATE
			if((f_trans != NULL) && (match.points > 0))
			{
				// show each rule that matches, and it's points score
				int pts;
				char decoded_phonemes[80];

				// note: 'count' contains the rule number, if we want to include it
				pts = match.points;
				if(group_length > 1)
					pts += 35;    // to account for an extra letter matching
				DecodePhonemes(match.phonemes,decoded_phonemes);
				fprintf(f_trans,"%3d   %s [%s]\n",pts,DecodeRule(group,rule_start),decoded_phonemes);
			}
#endif

		}

		/* skip phoneme string to reach start of next template */
		while(*rule++ != 0);
	}

#ifdef LOG_TRANSLATE
	if(f_trans != NULL)
	{
		if(group_length <= 1)
			fprintf(f_trans,"\n");
//		else
//			fprintf(f_trans," /");
	}
#endif

	/* advance input data pointer */
	total_consumed += group_length;
	if(total_consumed == 0)
		total_consumed = 1;     /* always advance over 1st letter */

	*word += total_consumed;

	if(best.points == 0)
		best.phonemes = "";
	memcpy(match_out,&best,sizeof(MatchRecord));
}   /* end of MatchRule */




int Translator::TranslateRules(char *p, char *phonemes, char *end_phonemes, int end_flags)
{//=======================================================================================
/* Translate a word bounded by space characters
   Append the result to 'phonemes' and any standard prefix/suffix in 'end_phonemes' */
	
	unsigned char  c, c2;
	unsigned int  c12;
	char *p2;           /* copy of p for use in double letter chain match */
	int  found;
	int  g;             /* group chain number */
	int  g1;            /* first group for this letter */
	int  n;
	int  letter;
	char *p_start;
	MatchRecord match1;
	MatchRecord match2;
	
	char group_name[4];

#ifdef LOG_TRANSLATE
	if(f_trans != NULL)
	{
		char wordbuf[120];
		int  ix;

		for(ix=0; ((c = p[ix]) != ' ') && (c != 0); ix++)
		{
			wordbuf[ix] = c;
		}
		wordbuf[ix] = 0;
		fprintf(f_trans,"Translate '%s'\n",wordbuf);
	}
#endif

	p_start = p;
	word_vowel_count = 0;
	word_stressed_count = 0;
	
	if(end_phonemes != NULL)
		end_phonemes[0] = 0;
	
	while(((c = *p) != ' ') && (c != 0))
	{
		n = groups2_count[c];
		if((c >= '0') && (c <= '9'))
		{
			MatchRule(&p, "", groups1[(unsigned char)'9'],&match1,end_flags);
		}
		else
		{
			found = 0;
	
			if(n > 0)
			{
				/* there are some 2 byte chains for this initial letter */
				c2 = p[1];
				c12 = c + (c2 << 8);   /* 2 characters */
	
				g1 = groups2_start[c];
				for(g=g1; g < (g1+n); g++)
				{
					if(groups2_name[g] == c12)
					{
						found = 1;

						group_name[0] = c;
						group_name[1] = c2;
						group_name[2] = 0;
						p2 = p;
						MatchRule(&p2, group_name, groups2[g], &match2, end_flags);
						if(match2.points > 0)
							match2.points += 35;   /* to acount for 2 letters matching */

						/* now see whether single letter chain gives a better match ? */
						group_name[1] = 0;
						MatchRule(&p, group_name, groups1[c], &match1, end_flags);

						if(match2.points >= match1.points)
						{
							// use match from the 2-letter group
							memcpy(&match1,&match2,sizeof(MatchRecord));
							p = p2;
						}
					}
				}
			}
	
			if(!found)
			{
				/* alphabetic, single letter chain */
				group_name[0] = c;
				group_name[1] = 0;
	
				if(groups1[c] != NULL)
					MatchRule(&p, group_name, groups1[c], &match1, end_flags);
				else
				{
					// no group for this letter, use default group
					MatchRule(&p, "", groups1[0],&match1,end_flags);

					if(match1.points == 0)
					{
						// no match, try removing the accent and re-translating the word
						n = utf8_in(&letter,p-1,0)-1;
						if((letter >= 0xc0) && (letter <= 0x241))
						{
							// within range of the remove_accent table
							p[-1] = remove_accent[letter-0xc0];
							while((p[0] = p[n]) != ' ')  p++;
							while(n-- > 0) *p++ = ' ';  // replacement character must be no longer than original
							phonemes[0] = 0;  // delete any phonemes which have been produced so far
							p = p_start;
							word_vowel_count = 0;
							word_stressed_count = 0;
							continue;  // start again at the beginning of the word
						}
					}
				}
			}
		}
	
		if(match1.phonemes == NULL)
			match1.phonemes = "";
	
		if((match1.end_type != 0) && (end_phonemes != NULL))
		{
			/* a standard ending has been found, re-translate the word without it */
			strcpy(end_phonemes,match1.phonemes);
			return(match1.end_type);
		}
		if(match1.del_fwd != NULL)
			*match1.del_fwd = REPLACED_E;
		AppendPhonemes(phonemes,match1.phonemes);
	}
	return(0);
}   /* end of TranslateRules */





//=============================================================================================
//   Look up a word in the pronunciation dictionary list
//   - exceptions which override the usual pronunciation rules, or which give a word
//     special properties, such as pronounce as unstressed
//=============================================================================================



int Translator::LookupDict2(char *word, char *word2, char *phonetic, unsigned int *flags, int end_flags)
//======================================================================================================
/* Find an entry in the word_dict file for a specified word.
   Returns 1 if an entry is found

	word   zero terminated word to match
	word2  pointer to next word(s) in the input text (terminated by space)

	flags:  returns dictionary flags which are associated with a matched word

	end_flags:  indicates whether this is a retranslation after removing a suffix
*/
{
	char *p;
	char *next;
	int  hash;
	int  phoneme_len;
	int  length;
	unsigned char flag;
	unsigned int  dictionary_flags;
	int  condition_failed=0;
	int  n_chars;
	char *word_end;

	hash = HashDictionary(word);
	p = dict_hashtab[hash];

	while(*p != 0)
	{
		next = p + p[0];

		if(strcmp(word,&p[1])!=0)
		{
			p = next;
		}
		else
		{
			/* found matching entry. Decode the phonetic string */
			word_end = word2;

			dictionary_flags = 0;

			length = strlen(&p[1]);
			p += (length + 2);
			strcpy(phonetic,p);
			phoneme_len = strlen(p);
			p += (phoneme_len + 1);

			while(p < next)
			{
				flag = *p++;
				if(flag >= 100)
				{
					// conditional rule
					condition_failed = 1;
				}
				else
				if(flag >= 29)
				{
					// flags 29,30,or 31  followed by further words to match
					// This comes after other flags
					n_chars = next - p;
					if(memcmp(word2,p,n_chars)==0)
					{
						dictionary_flags |= ((flag-28) << 29);   // set bits 29,30 to 1,2,or 3
						p = next;
						word_end = word2 + n_chars;
					}
					else
					{
						p = next;
						condition_failed = 1;
						break;
					}
				}
				else
				if(flag <= 7)
				{
					dictionary_flags = (dictionary_flags & ~0x7) | flag;   // stressed vowel number
				}
				else
				{
					dictionary_flags |= (1L << flag);
				}
			}

			if(condition_failed)
			{
				condition_failed=0;
				continue;
			}

			if(end_flags & FLAG_SUFX)
			{
				// a suffix was removed from the word
				if(dictionary_flags & FLAG_ONLY)
					continue;        // no match if any suffix

				if((dictionary_flags & FLAG_ONLY_S) && ((end_flags & FLAG_SUFX_S)==0))
				{
					// only a 's' suffix allowed, but the suffix wasn't 's'
					continue;
				}
			}

			if(dictionary_flags & FLAG_CAPITAL)
			{
				if(!(word_flags & FLAG_FIRST_UPPER))
				{
					continue;
				}
			}

			if((dictionary_flags & FLAG_ATEND) && (word_end < clause_end))
			{
				// only use this pronunciation if it's the last word of the clause
				continue;
			}

			if(dictionary_flags & FLAG_VERB)
			{
				// this is a verb-form pronunciation

				if(expect_verb || (expect_verb_s && (end_flags & FLAG_SUFX_S)))
				{
					// OK, we are expecting a verb
				}
				else
				{
					/* don't use the 'verb' pronunciation unless we are
						expecting a verb */
					continue;
				}
			}
			if(dictionary_flags & FLAG_PAST)
			{
				if(!expect_past)
				{
					/* don't use the 'past' pronunciation unless we are
						expecting past tense */
					continue;
				}
			}
			*flags = dictionary_flags;

			if(phoneme_len == 0)
			{
				if(f_trans != NULL)
				{
					fprintf(f_trans,"Flags:  %s  0x%x\n",word,*flags);
				}
				return(0);    // no phoneme translation found here, only flags. So use rules
			}

			*flags |= FLAG_FOUND;   // this flag indicates word was found in dictionary
			
			if(f_trans != NULL)
			{
				char ph_decoded[N_WORD_PHONEMES];
				DecodePhonemes(phonetic,ph_decoded);
				fprintf(f_trans,"Found: %s [%s] 0x%x\n",word,ph_decoded,*flags);
			}
	
			return(1);
		}
	}
	return(0);
}   //  end of LookupDict2



int Translator::LookupDictList(char *word1, char *ph_out, unsigned int *flags, int end_flags)
//===========================================================================================
/* Lookup a specified word in the word dictionary.
   Returns phonetic data in 'phonetic' and bits in 'flags'

   end_flags:  indicates if a suffix has been removed
*/
{
	int  length;
	int  found;
	unsigned char c;
	char word[N_WORD_PHONEMES];

	for(length=0; length<N_WORD_PHONEMES; length++)
	{
		if(((c = *word1++)==0) || (c == ' '))
			break;
		word[length] = c;
	}
	word[length] = 0;

	found = LookupDict2(word,word1,ph_out,flags,end_flags);

	if(found) return(1);
	ph_out[0] = 0;

	// try modifications to find a recognised word

	if((end_flags & FLAG_SUFX_E_ADDED) && (word[length-1] == 'e'))
	{
		// try removing an 'e' which has been added by RemoveEnding
		word[length-1] = 0;
		found = LookupDict2(word,word1,ph_out,flags,end_flags);
		if(found) return(1);
	}

	if((end_flags & SUFX_D) && (word[length-1] == word[length-2]))
	{
		// try removing a double letter
		word[length-1] = 0;
		found = LookupDict2(word,word1,ph_out,flags,end_flags);
		if(found) return(1);
	}

	ph_out[0] = 0;
	return(0);
}   //  end of LookupDictList



int Translator::RemoveEnding(char *word, int end_type)
{//===================================================
/* Removes a standard suffix from a word, once it has been indicated by the dictionary rules.
   end_type: bits 0-7  number of letters
             bits 8-14  suffix flags

	This routine is language specific.  In English it deals with reversing y->i and e-dropping
	that were done when the suffix was added to the original word.
*/
	
	int  i;
	char *word_end;
	int len_ending;
	int end_flags;
	const char *p;
	int  len;
	static char ending[12];
	
	// these lists are language specific, but are only relevent if the 'e' suffix flag is used
	static const char *add_e_exceptions[] = {
		"ion", NULL };

	static const char *add_e_additions[] = {
		"c", "rs", "ir", "ur", "ath", "ns", "lu", NULL };

	for(word_end = word; *word_end != ' '; word_end++)
	{
		/* replace discarded 'e's */
		if(*word_end == REPLACED_E)
			*word_end = 'e';
	}
	
	/* delete ending from word */
	len_ending = end_type & 0xf;
	
	word_end -= len_ending;
	
	for(i=0; i<len_ending; i++)
	{
		ending[i] = word_end[i];
		word_end[i] = ' ';
	}
	ending[i] = 0;
	word_end--;     /* now pointing at last character of stem */

	end_flags = (end_type & 0xfff0) | FLAG_SUFX;
	
	/* add an 'e' to the stem if appropriate,
		if  stem ends in vowel+consonant
		or  stem ends in 'c'  (add 'e' to soften it) */
	
	if(end_type & SUFX_I)
	{
		if(word_end[0] == 'i')
			word_end[0] = 'y';
	}
	
	if(end_type & SUFX_E)
	{
		// add 'e' to end of stem
		if(IsLetter(word_end[-1],LETTERGP_VOWEL_Y) && IsLetter(word_end[0],1))
		{
 			// vowel(incl.'y') + hard.consonant

			for(i=0; (p = add_e_exceptions[i]) != NULL; i++)
			{
				len = strlen(p);
				if(memcmp(p,&word_end[1-len],len)==0)
				{
					break;
				}
			}
			if(p == NULL)
				end_flags |= FLAG_SUFX_E_ADDED;  // no exception found
		}
		else
		{
			for(i=0; (p = add_e_additions[i]) != NULL; i++)
			{
				len = strlen(p);
				if(memcmp(p,&word_end[1-len],len)==0)
				{
					end_flags |= FLAG_SUFX_E_ADDED;
					break;
				}
			}
		}

		if(end_flags & FLAG_SUFX_E_ADDED)
		{
			word_end[1] = 'e';
#ifdef LOG_TRANSLATE
if(f_trans != NULL)
{
	fprintf(f_trans,"add e\n");
}
#endif
		}
	}

	if((end_type & SUFX_V) && (expect_verb==0))
		expect_verb = 1;         // this suffix indicates the verb pronunciation


	if((strcmp(ending,"s")==0) || (strcmp(ending,"es")==0))
		end_flags |= FLAG_SUFX_S;

	if(strcmp(ending,"'s")==0)
		end_flags &= ~FLAG_SUFX;  // don't consider 's as am added suffix

	return(end_flags);
}   /* end of RemoveEnding */


