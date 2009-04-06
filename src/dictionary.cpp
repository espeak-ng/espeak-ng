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
 *   along with this program; if not, write see:                           *
 *               <http://www.gnu.org/licenses/>.                           *
 ***************************************************************************/

#include "StdAfx.h"

#define LOG_TRANSLATE
 
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <wctype.h>
#include <wchar.h>

#include "speak_lib.h"
#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"


int dictionary_skipwords;
char dictionary_name[40];

extern char *print_dictionary_flags(unsigned int *flags);

// accented characters which indicate (in some languages) the start of a separate syllable
//static const unsigned short diereses_list[7] = {L'ä',L'ë',L'ï',L'ö',L'ü',L'ÿ',0};
static const unsigned short diereses_list[7] = {0xe4,0xeb,0xef,0xf6,0xfc,0xff,0};

// convert characters to an approximate 7 bit ascii equivalent
// used for checking for vowels
static unsigned char remove_accent[] = {
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


static int reverse_word_bytes(int word)
{//=============================
	// reverse the order of bytes from little-endian to big-endian
#ifdef ARCH_BIG
	int ix;
	int word2 = 0;

	for(ix=0; ix<=24; ix+=8)
	{
		word2 = word2 << 8;
		word2 |= (word >> ix) & 0xff;
	}
	return(word2);
#else
	return(word);
#endif
}


int LookupMnem(MNEM_TAB *table, char *string)
{//==========================================
	while(table->mnem != NULL)
	{
		if(strcmp(string,table->mnem)==0)
			return(table->value);
		table++;
	}
	return(table->value);
}



//=============================================================================================
//   Read pronunciation rules and pronunciation lookup dictionary
//
//=============================================================================================


static void InitGroups(Translator *tr)
{//===================================
/* Called after dictionary 1 is loaded, to set up table of entry points for translation rule chains
	for single-letters and two-letter combinations
*/

	int  ix;
	char *p;
	char *p_name;
	unsigned int *pw;
	unsigned char c, c2;
	int len;

	tr->n_groups2 = 0;
	for(ix=0; ix<256; ix++)
	{
		tr->groups1[ix]=NULL;
		tr->groups2_count[ix]=0;
		tr->groups2_start[ix]=255;  // indicates "not set"
	}
	memset(tr->letterGroups,0,sizeof(tr->letterGroups));

	p = tr->data_dictrules;
	while(*p != 0)
	{
		if(*p != RULE_GROUP_START)
		{
			fprintf(stderr,"Bad rules data in '%s_dict' at 0x%x\n",dictionary_name,(unsigned int)(p - tr->data_dictrules));
			break;
		}
		p++;

		if(p[0] == RULE_REPLACEMENTS)
		{
			pw = (unsigned int *)(((long)p+4) & ~3);  // advance to next word boundary
			tr->langopts.replace_chars = pw;
			while(pw[0] != 0)
			{
				pw += 2;   // find the end of the replacement list, each entry is 2 words.
			}
			p = (char *)(pw+1);

#ifdef ARCH_BIG
			pw = (unsigned int *)(tr->langopts.replace_chars);
			while(*pw != 0)
			{
				*pw = reverse_word_bytes(*pw);
				pw++;
				*pw = reverse_word_bytes(*pw);
				pw++;
			}
#endif
			continue;
		}

		if(p[0] == RULE_LETTERGP2)
		{
			ix = p[1] - 'A';
			p += 2;
			if((ix >= 0) && (ix < N_LETTER_GROUPS))
			{
				tr->letterGroups[ix] = p;
			}
		}
		else
		{
			len = strlen(p);
			p_name = p;
			c = p_name[0];
			
			p += (len+1);
			if(len == 1)
			{
				tr->groups1[c] = p;
			}
			else
			if(len == 0)
			{
				tr->groups1[0] = p;
			}
			else
			{
				if(tr->groups2_start[c] == 255)
					tr->groups2_start[c] = tr->n_groups2;
	
				tr->groups2_count[c]++;
				tr->groups2[tr->n_groups2] = p;
				c2 = p_name[1];
				tr->groups2_name[tr->n_groups2++] = (c + (c2 << 8));
			}
		}

		// skip over all the rules in this group
		while(*p != RULE_GROUP_END)
		{
			p += (strlen(p) + 1);
		}
		p++;
	}

}  //  end of InitGroups



int LoadDictionary(Translator *tr, const char *name, int no_error)
{//===============================================================
	int hash;
	char *p;
	int *pw;
	int length;
	FILE *f;
	unsigned int size;
	char fname[sizeof(path_home)+20];

	strcpy(dictionary_name,name);   // currently loaded dictionary name

	if(no_error)   // don't load dictionary, just set the dictionary_name
		return(1);

	// Load a pronunciation data file into memory
	// bytes 0-3:  offset to rules data
	// bytes 4-7:  number of hash table entries
	sprintf(fname,"%s%c%s_dict",path_home,PATHSEP,name);
	size = GetFileLength(fname);

	f = fopen(fname,"rb");
	if((f == NULL) || (size <= 0))
	{
		if(no_error == 0)
		{
			fprintf(stderr,"Can't read dictionary file: '%s'\n",fname);
		}
		return(1);
	}

	if(tr->data_dictlist != NULL)
		Free(tr->data_dictlist);

	tr->data_dictlist = Alloc(size);
	fread(tr->data_dictlist,size,1,f);
	fclose(f);


	pw = (int *)(tr->data_dictlist);
	length = reverse_word_bytes(pw[1]);

	if(size <= (N_HASH_DICT + sizeof(int)*2))
	{
		fprintf(stderr,"Empty _dict file: '%s\n",fname);
		return(2);
	}

	if((reverse_word_bytes(pw[0]) != N_HASH_DICT) ||
	   (length <= 0) || (length > 0x8000000))
	{
		fprintf(stderr,"Bad data: '%s' (%x length=%x)\n",fname,reverse_word_bytes(pw[0]),length);
		return(2);
	}
	tr->data_dictrules = &(tr->data_dictlist[length]);

	// set up indices into data_dictrules
	InitGroups(tr);
	if(tr->groups1[0] == NULL)
	{
		fprintf(stderr,"Error in %s_rules, no default rule group\n",name);
	}

	// set up hash table for data_dictlist
	p = &(tr->data_dictlist[8]);

	for(hash=0; hash<N_HASH_DICT; hash++)
	{
		tr->dict_hashtab[hash] = p;
		while((length = *p) != 0)
		{
			p += length;
		}
		p++;   // skip over the zero which terminates the list for this hash value
	}

	return(0);
}  //  end of LoadDictionary


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



char *EncodePhonemes(char *p, char *outptr, unsigned char *bad_phoneme)
/*********************************************************************/
/* Translate a phoneme string from ascii mnemonics to internal phoneme numbers,
   from 'p' up to next blank .
   Returns advanced 'p'
   outptr contains encoded phonemes, unrecognised phonemes are encoded as 255
   bad_phoneme must point to char array of length 2 of more
*/
{
	int ix;
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

			for(ix=1; ix<n_phoneme_tab; ix++)
			{
				if(phoneme_tab[ix] == NULL)
					continue;
				if(phoneme_tab[ix]->type == phINVALID)
					continue;       // this phoneme is not defined for this language

				count = 0;
				mnemonic_word = phoneme_tab[ix]->mnemonic;

				while(((c = p[count]) > ' ') && (count < 4) &&
										(c == ((mnemonic_word >> (count*8)) & 0xff)))
					count++;

				if((count > max) &&
					((count == 4) || (((mnemonic_word >> (count*8)) & 0xff)==0)))
				{
					max = count;
					max_ph = phoneme_tab[ix]->code;
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

			if(max_ph == phonSWITCH)
			{
				// Switch Language: this phoneme is followed by a text string
				char *p_lang = outptr;
				while(!isspace(c = *p) && (c != 0))
				{
					p++;
					*outptr++ = tolower(c);
				}
				*outptr = 0;
				if(c == 0)
				{
					if(strcmp(p_lang,"en")==0)
					{
						*p_lang = 0;   // don't need "en", it's assumed by default
						return(p);
					}
				}
				else
				{
					*outptr++ = '|';  // more phonemes follow, terminate language string with separator
				}
			}
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
	unsigned char phcode;
	unsigned char c;
	unsigned int  mnem;
	PHONEME_TAB *ph;
	static const char *stress_chars = "==,,'*  ";

	while((phcode = *inptr++) > 0)
	{
		if(phcode == 255)
			continue;     /* indicates unrecognised phoneme */
		if((ph = phoneme_tab[phcode]) == NULL)
			continue;
	
		if((ph->type == phSTRESS) && (ph->std_length <= 4) && (ph->spect == 0))
		{
			if(ph->std_length > 1)
				*outptr++ = stress_chars[ph->std_length];
		}
		else
		{
			mnem = ph->mnemonic;

			while((c = (mnem & 0xff)) != 0)	
			{
				*outptr++ = c;
				mnem = mnem >> 8;
			}
			if(phcode == phonSWITCH)
			{
				while(isalpha(*inptr))
				{
					*outptr++ = *inptr++;
				}
			}
		}
	}
	*outptr = 0;    /* string terminator */
}   //  end of DecodePhonemes



static void WriteMnemonic(char *phon_out, int *ix, int mnem)
{//=========================================================
	unsigned char c;

	while((c = mnem & 0xff) != 0)
	{
		if((c == '/') && (option_phoneme_variants==0))
			break;      // discard phoneme variant indicator
		phon_out[(*ix)++]= c;
	//	phon_out[phon_out_ix++]= ipa1[c];
		mnem = mnem >> 8;
	}
}



void GetTranslatedPhonemeString(char *phon_out, int n_phon_out)
{//============================================================
/* Can be called after a clause has been translated into phonemes, in order
   to display the clause in phoneme mnemonic form.
*/

	int  ix;
	int  phon_out_ix=0;
	int  stress;
	char *p;
	PHONEME_LIST *plist;
	
	static const char *stress_chars = "==,,''";

	if(phon_out != NULL)
	{
		for(ix=1; ix<(n_phoneme_list-2) && (phon_out_ix < (n_phon_out - 6)); ix++)
		{
			plist = &phoneme_list[ix];
			if(plist->newword)
				phon_out[phon_out_ix++] = ' ';

			if(plist->synthflags & SFLAG_SYLLABLE)
			{
				if((stress = plist->stresslevel) > 1)
				{
					if(stress > 5) stress = 5;
					phon_out[phon_out_ix++] = stress_chars[stress];
				}
			}
			WriteMnemonic(phon_out, &phon_out_ix, plist->ph->mnemonic);

			if(plist->synthflags & SFLAG_LENGTHEN)
			{
				WriteMnemonic(phon_out, &phon_out_ix, phoneme_tab[phonLENGTHEN]->mnemonic);
			}
			if((plist->synthflags & SFLAG_SYLLABLE) && (plist->type != phVOWEL))
			{
				// syllablic consonant
				WriteMnemonic(phon_out, &phon_out_ix, phoneme_tab[phonSYLLABIC]->mnemonic);
			}
			if(plist->ph->code == phonSWITCH)
			{
				// the tone_ph field contains a phoneme table number
				p = phoneme_tab_list[plist->tone_ph].name;
				while(*p != 0)
				{
					phon_out[phon_out_ix++] = *p++;
				}
				phon_out[phon_out_ix++] = ' ';
			}
			else
			if(plist->tone_ph > 0)
			{
				WriteMnemonic(phon_out, &phon_out_ix, phoneme_tab[plist->tone_ph]->mnemonic);
			}
		}
	
		if(phon_out_ix >= n_phon_out)
			phon_out_ix = n_phon_out - 1;
		phon_out[phon_out_ix] = 0;
	}
}  // end of GetTranslatedPhonemeString



//=============================================================================================
//   Is a word Unpronouncable - and so should be spoken as individual letters
//
//=============================================================================================



static int IsLetterGroup(Translator *tr, char *word, int group, int pre)
{//=====================================================================
	// match the word against a list of utf-8 strings
	char *p;
	char *w;
	int  len=0;

	p = tr->letterGroups[group];
	if(p == NULL)
		return(0);

	while(*p != RULE_GROUP_END)
	{
		if(pre)
		{
			len = strlen(p);
			w = word - len + 1;
		}
		else
		{
			w = word;
		}
		while(*p == *w)
		{
			w++;
			p++;
		}
		if(*p == 0)
		{
			if(pre)
				return(len);
			return(w-word);   // matched a complete string
		}

		while(*p++ != 0);  // skip to end of string
	}
	return(0);
}


static int IsLetter(Translator *tr, int letter, int group)
{//=======================================================
	int letter2;

	if(tr->letter_groups[group] != NULL)
	{
		if(wcschr(tr->letter_groups[group],letter))
			return(1);
		return(0);
	}

	if(group > 7)
		return(0);

	if(tr->letter_bits_offset > 0)
	{
		if(((letter2 = (letter - tr->letter_bits_offset)) > 0) && (letter2 < 0x80))
				letter = letter2;
		else
			return(0);
	}
	else
	{
		if((letter >= 0xc0) && (letter <= 0x241))
			return(tr->letter_bits[remove_accent[letter-0xc0]] & (1L << group));
	}

	if((letter >= 0) && (letter < 0x80))
		return(tr->letter_bits[letter] & (1L << group));

	return(0);
}


static int IsVowel(Translator *tr, int letter)
{//===========================================
	return(IsLetter(tr, letter, 0));
}




static int Unpronouncable_en(Translator *tr, char *word)
{//=====================================================
/* Determines whether a word in 'unpronouncable', i.e. whether it should
	be spoken as individual letters.

	This function is language specific.
*/

	int  c;
	int  vowel_posn=9;
	int  index;
	int  count;
	int  ix;
	int  apostrophe=0;

	static unsigned char initials_bitmap[86] = {
	0x00, 0x00, 0x00, 0x00, 0x22, 0x08, 0x00, 0x88,  //  0
	0x20, 0x24, 0x20, 0x80, 0x10, 0x00, 0x00, 0x00,
	0x00, 0x28, 0x08, 0x00, 0x88, 0x22, 0x04, 0x00,  // 16
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x88, 0x22, 0x04, 0x00, 0x02, 0x00, 0x04,  // 32
	0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x28, 0x8a, 0x03, 0x00, 0x00, 0x40, 0x00,  // 48
	0x02, 0x00, 0x41, 0xca, 0xbb, 0x06, 0x20, 0x80,
	0x91, 0x00, 0x00, 0x00, 0x00, 0x20, 0x08, 0x00,  // 64
	0x08, 0x20, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x22, 0x00, 0x01, 0x00, };


	// words which we pass through to the dictionary, even though they look unpronouncable
	static const char *exceptions[] = {
		"'s ", "st ","nd ","rd ","th ",NULL };

	if((*word == ' ') || (*word == 0))
		return(0);

	for(ix=0; exceptions[ix] != NULL; ix++)
	{
		// Seemingly uncpronouncable words, but to be looked in the dictionary rules instead
		if(memcmp(word,exceptions[ix],3)==0)
			return(0);
	}

	index=0;
	count=0;
	for(;;)
	{
		index += utf8_in(&c,&word[index]);
		count++;

		if((c==0) || (c==' '))
			break;

		if(IsVowel(tr, c) || (c == 'y'))
		{
			vowel_posn = count;
			break;
		}

		if(c == '\'')
			apostrophe = 1;
		else
		if(!IsAlpha(c))
			return(0);        // letter (not vowel) outside Latin character range or apostrophe, abort test
	}
	if((vowel_posn > 5) || ((word[0]!='s') && (vowel_posn > 4)))
		return(1);  // no vowel, or no vowel in first four letters

	/* there is at least one vowel, is the initial letter combination valid ? */

	if(vowel_posn < 3)
		return(0);   /* vowel in first two letters, OK */

	if(apostrophe)
		return(0);   // first two letters not a-z, abort test

	index = (word[0]-'a') * 26 + (word[1]-'a');
	if(initials_bitmap[index >> 3] & (1L << (index & 7)))
		return(0);
	else
		return(1);   /****/
}   /* end of Unpronounceable */




int Unpronouncable(Translator *tr, char *word)
{//===========================================
/* Determines whether a word in 'unpronouncable', i.e. whether it should
	be spoken as individual letters.

	This function may be language specific. This is a generic version.
*/

	int  c;
	int  c1=0;
	int  vowel_posn=9;
	int  index;
	int  count;
	int  apostrophe=0;

	if(tr->translator_name == L('e','n'))
	{
		return(Unpronouncable_en(tr,word));
	}

	utf8_in(&c,word);
	if((tr->letter_bits_offset > 0) && (c < 0x241))
	{
		// Latin characters for a language with a non-latin alphabet
		return(0);  // so we can re-translate the word as English
	}

	if(tr->langopts.param[LOPT_UNPRONOUNCABLE] == 1)
		return(0);

	if((*word == ' ') || (*word == 0))
		return(0);

	index = 0;
	count = 0;
	for(;;)
	{
		index += utf8_in(&c,&word[index]);
		if((c==0) || (c==' '))
			break;

		if(count==0)
			c1 = c;
		count++;

		if(IsVowel(tr, c))
		{
			vowel_posn = count;    // position of the first vowel
			break;
		}

		if(c == '\'')
			apostrophe = 1;
		else
		if(!iswalpha(c))
			return(0);        // letter (not vowel) outside a-z range or apostrophe, abort test
	}

	if((vowel_posn < 9) && (tr->langopts.param[LOPT_UNPRONOUNCABLE] == 2))
		return(0);   // option means allow any word with a vowel

	if(c1 == tr->langopts.param[LOPT_UNPRONOUNCABLE])
		vowel_posn--;   // disregard this as the initial letter when counting

	if(vowel_posn > (tr->langopts.max_initial_consonants+1))
		return(1);  // no vowel, or no vowel in first four letters

return(0);

}   /* end of Unpronounceable */



//=============================================================================================
//   Determine the stress pattern of a word
//
//=============================================================================================



static int GetVowelStress(Translator *tr, unsigned char *phonemes, unsigned char *vowel_stress, int &vowel_count, int &stressed_syllable, int control)
{//====================================================================================================================================================
// control = 1, set stress to 1 for forced unstressed vowels
	unsigned char phcode;
	PHONEME_TAB *ph;
	unsigned char *ph_out = phonemes;
	int count = 1;
	int max_stress = 0;
	int ix;
	int j;
	int stress = 0;
	int primary_posn = 0;

	vowel_stress[0] = 0;
	while(((phcode = *phonemes++) != 0) && (count < (N_WORD_PHONEMES/2)-1))
	{
		if((ph = phoneme_tab[phcode]) == NULL)
			continue;

		if((ph->type == phSTRESS) && (ph->spect == 0))
		{
			/* stress marker, use this for the following vowel */

			if(phcode == phonSTRESS_PREV)
			{
				/* primary stress on preceeding vowel */
				j = count - 1;
				while((j > 0) && (stressed_syllable == 0) && (vowel_stress[j] < 4))
				{
					if(vowel_stress[j] != 1)
					{
						// don't promote a phoneme which must be unstressed
						vowel_stress[j] = 4;

						if(max_stress < 4)
						{
							max_stress = 4;
							primary_posn = j;
						}
	
						/* reduce any preceding primary stress markers */
						for(ix=1; ix<j; ix++)
						{
							if(vowel_stress[ix] == 4)
								vowel_stress[ix] = 3;
						}
						break;
					}
					j--;
				}
			}
			else
			{
				if((ph->std_length < 4) || (stressed_syllable == 0))
				{
					stress = ph->std_length;

					if(stress > max_stress)
						max_stress = stress;
				}
			}
			continue;
		}

		if((ph->type == phVOWEL) && !(ph->phflags & phNONSYLLABIC))
		{
			vowel_stress[count] = (char)stress;
			if((stress >= 4) && (stress >= max_stress))
			{
				primary_posn = count;
				max_stress = stress;
			}

			if((stress == 0) && (control & 1) && (ph->phflags & phUNSTRESSED))
				vowel_stress[count] = 1;   /* weak vowel, must be unstressed */

			count++;
			stress = 0;
		}
		else
		if(phcode == phonSYLLABIC)
		{
			// previous consonant phoneme is syllablic
			vowel_stress[count] = (char)stress;
			if((stress == 0) && (control & 1))
				vowel_stress[count++] = 1;    // syllabic consonant, usually unstressed
		}

		*ph_out++ = phcode;
	}
	vowel_stress[count] = 0;
	*ph_out = 0;

	/* has the position of the primary stress been specified by $1, $2, etc? */
	if(stressed_syllable > 0)
	{
		if(stressed_syllable >= count)
			stressed_syllable = count-1;   // the final syllable

		vowel_stress[stressed_syllable] = 4;
		max_stress = 4;
		primary_posn = stressed_syllable;
	}

	if(max_stress == 5)
	{
		// priority stress, replaces any other primary stress marker
		for(ix=1; ix<count; ix++)
		{
			if(vowel_stress[ix] == 4)
			{
				if(tr->langopts.stress_flags & 0x20000)
					vowel_stress[ix] = 0;
				else
					vowel_stress[ix] = 3;
			}

			if(vowel_stress[ix] == 5)
			{
				vowel_stress[ix] = 4;
				primary_posn = ix;
			}
		}
		max_stress = 4;
	}

	stressed_syllable = primary_posn;
	vowel_count = count;
	return(max_stress);
}  // end of GetVowelStress



static char stress_phonemes[] = {phonSTRESS_U, phonSTRESS_D, phonSTRESS_2, phonSTRESS_3,
		phonSTRESS_P, phonSTRESS_P2, phonSTRESS_TONIC};


void ChangeWordStress(Translator *tr, char *word, int new_stress)
{//==============================================================
	int ix;
	unsigned char *p;
	int  max_stress;
	int  vowel_count;              // num of vowels + 1
	int  stressed_syllable=0;      // position of stressed syllable
	unsigned char phonetic[N_WORD_PHONEMES];
	unsigned char vowel_stress[N_WORD_PHONEMES/2];

	strcpy((char *)phonetic,word);
	max_stress = GetVowelStress(tr, phonetic, vowel_stress, vowel_count, stressed_syllable, 0);

	if(new_stress >= 4)
	{
		// promote to primary stress
		for(ix=1; ix<vowel_count; ix++)
		{
			if(vowel_stress[ix] >= max_stress)
			{
				vowel_stress[ix] = new_stress;
				break;
			}
		}
	}
	else
	{
		// remove primary stress
		for(ix=1; ix<vowel_count; ix++)
		{
			if(vowel_stress[ix] > new_stress)   // >= allows for diminished stress (=1)
				vowel_stress[ix] = new_stress;
		}
	}

	// write out phonemes
	ix = 1;
	p = phonetic;
	while(*p != 0)
	{
		if((phoneme_tab[*p]->type == phVOWEL) && !(phoneme_tab[*p]->phflags & phNONSYLLABIC))
		{
			if(vowel_stress[ix] != 0)
				*word++ = stress_phonemes[vowel_stress[ix]];

			ix++;
		}
		*word++ = *p++;
	}
	*word = 0;
}  // end of ChangeWordStress



void SetWordStress(Translator *tr, char *output, unsigned int &dictionary_flags, int tonic, int prev_stress)
{//=========================================================================================================
/* Guess stress pattern of word.  This is language specific

   'dictionary_flags' has bits 0-3   position of stressed vowel (if > 0)
                                     or unstressed (if == 7) or syllables 1 and 2 (if == 6)
                          bits 8...  dictionary flags

   If 'tonic' is set (>= 0), replace highest stress by this value.

   Parameter used for input and output
*/

	unsigned char phcode;
	unsigned char *p;
	PHONEME_TAB *ph;
	int  stress;
	int  max_stress;
	int  vowel_count;      // num of vowels + 1
	int  ix;
	int  v;
	int  v_stress;
	int  stressed_syllable;      // position of stressed syllable
	int  max_stress_posn;
	int  unstressed_word = 0;
	char *max_output;
	int final_ph;
	int final_ph2;
	int mnem;
	int mnem2;
	int post_tonic;
	int opt_length;
	int done;
	int stressflags;

	unsigned char vowel_stress[N_WORD_PHONEMES/2];
	char syllable_weight[N_WORD_PHONEMES/2];
	char vowel_length[N_WORD_PHONEMES/2];
	unsigned char phonetic[N_WORD_PHONEMES];

	static char consonant_types[16] = {0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0};


	/* stress numbers  STRESS_BASE +
		0  diminished, unstressed within a word
		1  unstressed, weak
		2
		3  secondary stress
		4  main stress */

	stressflags = tr->langopts.stress_flags;

	/* copy input string into internal buffer */
	for(ix=0; ix<N_WORD_PHONEMES; ix++)
	{
		phonetic[ix] = output[ix];
		// check for unknown phoneme codes
		if(phonetic[ix] >= n_phoneme_tab)
			phonetic[ix] = phonSCHWA;
		if(phonetic[ix] == 0)
			break;
	}
	if(ix == 0) return;
	final_ph = phonetic[ix-1];
	final_ph2 =  phonetic[ix-2];

	max_output = output + (N_WORD_PHONEMES-3);   /* check for overrun */

	// any stress position marked in the xx_list dictionary ? 
	stressed_syllable = dictionary_flags & 0x7;
	if(dictionary_flags & 0x8)
	{
		// this indicates a word without a primary stress
		stressed_syllable = dictionary_flags & 0x3;
		unstressed_word = 1;
	}

	max_stress = GetVowelStress(tr, phonetic, vowel_stress, vowel_count, stressed_syllable, 1);

	if((max_stress == 0) && (tr->langopts.stress_flags & 1) && (vowel_count == 2))
	{
		// option: don't stress monosyllables except at end-of-clause
		vowel_stress[1] = 1;
		dictionary_flags |= FLAG_STRESS_END2;
	}

	// heavy or light syllables
	ix = 1;
	for(p = phonetic; *p != 0; p++)
	{
		if((phoneme_tab[p[0]]->type == phVOWEL) && !(phoneme_tab[p[0]]->phflags & phNONSYLLABIC))
		{
			int weight = 0;
			int lengthened = 0;

			if(phoneme_tab[p[1]]->code == phonLENGTHEN)
				lengthened = 1;

			if(lengthened || (phoneme_tab[p[0]]->phflags & phLONG))
			{
				// long vowel, increase syllable weight
				weight++;
			}
			vowel_length[ix] = weight;

			if(lengthened) p++;  // advance over phonLENGTHEN

			if(consonant_types[phoneme_tab[p[1]]->type] && ((phoneme_tab[p[2]]->type != phVOWEL) || (phoneme_tab[p[1]]->phflags & phLONG)))
			{
				// followed by two consonants, a long consonant, or consonant and end-of-word
				weight++;
			}
			syllable_weight[ix] = weight;
			ix++;
		}
	}
	
	switch(tr->langopts.stress_rule)
	{
	case 8:
		// stress on first syllable, unless it is a light syllable
		if(syllable_weight[1] > 0)
			break;
		// else drop through to case 1
	case 1:
		// stress on second syllable
		if((stressed_syllable == 0) && (vowel_count > 2))
		{
			stressed_syllable = 2;
			if(max_stress == 0)
			{
				vowel_stress[stressed_syllable] = 4;
			}
			max_stress = 4;
		}
		break;

	case 2:
		// a language with stress on penultimate vowel

		if(stressed_syllable == 0)
		{
			/* no explicit stress - stress the penultimate vowel */
			max_stress = 4;

			if(vowel_count > 2)
			{
				stressed_syllable = vowel_count - 2;

				if(stressflags & 0x300)
				{
					// LANG=Spanish, stress on last vowel if the word ends in a consonant other than 'n' or 's'
					if(phoneme_tab[final_ph]->type != phVOWEL)
					{
						if(stressflags & 0x100)
						{
							stressed_syllable = vowel_count - 1;
						}
						else
						{
							mnem = phoneme_tab[final_ph]->mnemonic;
							mnem2 = phoneme_tab[final_ph2]->mnemonic;

							if((mnem == 's') && (mnem2 == 'n'))
							{
								// -ns  stress remains on penultimate syllable
							}
							else
							if(((mnem != 'n') && (mnem != 's')) || (phoneme_tab[final_ph2]->type != phVOWEL))
							{
								stressed_syllable = vowel_count - 1;
							}
						}
					}
				}
				if(stressflags & 0x80000)
				{
					// stress on last syllable if it has a long vowel, but previous syllable has a short vowel
					if(vowel_length[vowel_count - 1] > vowel_length[vowel_count - 2])
					{
						stressed_syllable = vowel_count - 1;
					}
				}

				if(vowel_stress[stressed_syllable] == 1)
				{
					// but this vowel is explicitly marked as unstressed
					if(stressed_syllable > 1)
						stressed_syllable--;
					else
						stressed_syllable++;
				}
			}
			else
			{
				stressed_syllable = 1;
			}

			// only set the stress if it's not already marked explicitly
			if(vowel_stress[stressed_syllable] == 0)
			{
				// don't stress if next and prev syllables are stressed
				if((vowel_stress[stressed_syllable-1] < 4) || (vowel_stress[stressed_syllable+1] < 4))
					vowel_stress[stressed_syllable] = max_stress;
			}
		}
		break;

   case 3:
		// stress on last vowel
		if(stressed_syllable == 0)
		{
			/* no explicit stress - stress the final vowel */
			stressed_syllable = vowel_count - 1;
			if(max_stress == 0)
			{
				while(stressed_syllable > 0)
				{
					if(vowel_stress[stressed_syllable] == 0)
					{
						vowel_stress[stressed_syllable] = 4;
						break;
					}
					else
						stressed_syllable--;
				}
			}
			max_stress = 4;
		}
		break;

	case 4:   // stress on antipenultimate vowel
		if(stressed_syllable == 0)
		{
			stressed_syllable = vowel_count - 3;
			if(stressed_syllable < 1)
				stressed_syllable = 1;

			if(max_stress == 0)
			{
				vowel_stress[stressed_syllable] = 4;
			}
			max_stress = 4;
		}
		break;

	case 5:
		// LANG=Russian
		if(stressed_syllable == 0)
		{
			/* no explicit stress - guess the stress from the number of syllables */
			static char guess_ru[16] =   {0,0,1,1,2,3,3,4,5,6,7,7,8,9,10,11};
			static char guess_ru_v[16] = {0,0,1,1,2,2,3,3,4,5,6,7,7,8,9,10};  // for final phoneme is a vowel
			static char guess_ru_t[16] = {0,0,1,2,3,3,3,4,5,6,7,7,7,8,9,10};  // for final phoneme is an unvoiced stop

			stressed_syllable = vowel_count - 3;
			if(vowel_count < 16)
			{
				if(phoneme_tab[final_ph]->type == phVOWEL)
					stressed_syllable = guess_ru_v[vowel_count];
				else
				if(phoneme_tab[final_ph]->type == phSTOP)
					stressed_syllable = guess_ru_t[vowel_count];
				else
					stressed_syllable = guess_ru[vowel_count];
			}
			vowel_stress[stressed_syllable] = 4;
			max_stress = 4;
		}
		break;

	case 6:    // LANG=hi stress on the last heaviest syllable
		if(stressed_syllable == 0)
		{
			int wt;
			int max_weight = -1;
			int prev_stressed;

			// find the heaviest syllable, excluding the final syllable
			for(ix = 1; ix < (vowel_count-1); ix++)
			{
				if(vowel_stress[ix] == 0)
				{
					if((wt = syllable_weight[ix]) >= max_weight)
					{
						max_weight = wt;
						prev_stressed = stressed_syllable;
						stressed_syllable = ix;
					}
				}
			}

			if((syllable_weight[vowel_count-1] == 2) &&  (max_weight< 2))
			{
				// the only double=heavy syllable is the final syllable, so stress this
				stressed_syllable = vowel_count-1;
			}
			else
			if(max_weight <= 0)
			{
				// all syllables, exclusing the last, are light. Stress the first syllable
				stressed_syllable = 1;
			}

			vowel_stress[stressed_syllable] = 4;
			max_stress = 4;
		}
		break;

	case 7:  // LANG=tr, the last syllable for any vowel markes explicitly as unstressed
		if(stressed_syllable == 0)
		{
			stressed_syllable = vowel_count - 1;
			for(ix=1; ix < vowel_count; ix++)
			{
				if(vowel_stress[ix] == 1)
				{
					stressed_syllable = ix-1;
					break;
				}
			}
			vowel_stress[stressed_syllable] = 4;
			max_stress = 4;
		}
		break;

	case 9:  // mark all as stressed
		for(ix=1; ix<vowel_count; ix++)
		{
			if(vowel_stress[ix] == 0)
				vowel_stress[ix] = 4;
		}
		break;
	}

	/* now guess the complete stress pattern */
	if(max_stress < 4)
		stress = 4;  /* no primary stress marked, use for 1st syllable */
	else
		stress = 3;


	if((stressflags & 0x1000) && (vowel_count == 2))
	{
		// Two syllable word, if one syllable has primary stress, then give the other secondary stress
		if(vowel_stress[1] == 4)
			vowel_stress[2] = 3;
		if(vowel_stress[2] == 4)
			vowel_stress[1] = 3;
	}
#if deleted
	if((stressflags & 0x2000) && (vowel_stress[1] == 0))
	{
		// If there is only one syllable before the primary stress, give it a secondary stress
		if((vowel_count > 2) && (vowel_stress[2] >= 4))
		{
			vowel_stress[1] = 3;
		}
	}
#endif

	done = 0;
	for(v=1; v<vowel_count; v++)
	{
		if(vowel_stress[v] == 0)
		{
			if((stressflags & 0x10) && (stress < 4) && (v == vowel_count-1))
			{
				// flag: don't give secondary stress to final vowel
			}
			else
			if((stressflags & 0x8000) && (done == 0))
			{
				vowel_stress[v] = (char)stress;
				done =1;
				stress = 3;  /* use secondary stress for remaining syllables */
			}
			else
			if((vowel_stress[v-1] <= 1) && ((vowel_stress[v+1] <= 1) || ((stress == 4) && (vowel_stress[v+1] <= 2))))
			{
				/* trochaic: give stress to vowel surrounded by unstressed vowels */

				if((stress == 3) && (stressflags & 0x20))
					continue;      // don't use secondary stress

				if((v > 1) && (stressflags & 0x40) && (syllable_weight[v]==0) && (syllable_weight[v+1]>0))
				{
					// don't put secondary stress on a light syllable which is followed by a heavy syllable
					continue;
				}

// should start with secondary stress on the first syllable, or should it count back from
// the primary stress and put secondary stress on alternate syllables?
				vowel_stress[v] = (char)stress;
				done =1;
				stress = 3;  /* use secondary stress for remaining syllables */
			}
		}
	}

	if((unstressed_word) && (tonic < 0))
	{
		if(vowel_count <= 2)
			tonic = tr->langopts.unstressed_wd1;   /* monosyllable - unstressed */
		else
			tonic = tr->langopts.unstressed_wd2;   /* more than one syllable, used secondary stress as the main stress */
	}

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

	if(tonic >= 0)
	{
		/* find position of highest stress, and replace it by 'tonic' */

		/* don't disturb an explicitly set stress by 'unstress-at-end' flag */
		if((tonic > max_stress) || (max_stress <= 4))
			vowel_stress[max_stress_posn] = (char)tonic;
		max_stress = tonic;
	}


	/* produce output phoneme string */
	p = phonetic;
	v = 1;

	if((ph = phoneme_tab[*p]) != NULL)
	{

		if(ph->type == phSTRESS)
			ph = phoneme_tab[p[1]];

#ifdef deleted
		int gap = tr->langopts.word_gap & 0x700;
		if((gap) && (vowel_stress[1] >= 4) && (prev_stress >= 4))
		{
			/* two primary stresses together, insert a short pause */
			*output++ = pause_phonemes[gap >> 8];
		}
		else
#endif
		if((tr->langopts.vowel_pause & 0x30) && (ph->type == phVOWEL))
		{
			// word starts with a vowel

			if((tr->langopts.vowel_pause & 0x20) && (vowel_stress[1] >= 4))
			{
					*output++ = phonPAUSE_NOLINK;   // not to be replaced by link
			}
			else
			{
				*output++ = phonPAUSE_VSHORT;     // break, but no pause
			}
		}
	}

	p = phonetic;
	post_tonic = 0;
	while(((phcode = *p++) != 0) && (output < max_output))
	{
		if((ph = phoneme_tab[phcode]) == NULL)
			continue;

//		if(ph->type == phSTRESS)
//			continue;

		if(ph->type == phPAUSE)
		{
			tr->prev_last_stress = 0;
		}
		else
		if(((ph->type == phVOWEL) && !(ph->phflags & phNONSYLLABIC)) || (*p == phonSYLLABIC))
		{
			// a vowel, or a consonant followed by a syllabic consonant marker

			v_stress = vowel_stress[v];
			tr->prev_last_stress = v_stress;

			if(vowel_stress[v-1] >= max_stress)
				post_tonic = 1;

			if(v_stress <= 1)
			{
				if((v > 1) && (max_stress >= 4) && (stressflags & 4) && (v == (vowel_count-1)))
				{
					// option: mark unstressed final syllable as diminished
					v_stress = 1;
				}
				else
				if((stressflags & 2) || (v == 1) || (v == (vowel_count-1)))
				{
					// first or last syllable, or option 'don't set diminished stress'
					v_stress = 0;
				}
				else
				if((v == (vowel_count-2)) && (vowel_stress[vowel_count-1] <= 1))
				{
					// penultimate syllable, followed by an unstressed final syllable
					v_stress = 0;
				}
				else
				{
					// unstressed syllable within a word
					if((vowel_stress[v-1] != 1) || ((stressflags & 0x10000) == 0))
					{
						v_stress = 1;      /* change from 0 (unstressed) to 1 (diminished stress) */
						vowel_stress[v] = v_stress;
					}
				}
			}

			if(v_stress > 0)
				*output++ = stress_phonemes[v_stress];  // mark stress of all vowels except 0 (unstressed)


			if(vowel_stress[v] > max_stress)
			{
				max_stress = vowel_stress[v];
			}

			if((*p == phonLENGTHEN) && ((opt_length = tr->langopts.param[LOPT_IT_LENGTHEN]) & 1))
			{
				// remove lengthen indicator from non-stressed syllables
				int shorten=0;

				if(opt_length & 0x10)
				{
					// only allow lengthen indicator on the highest stress syllable in the word
					if(v != max_stress_posn)
						shorten = 1;
				}
				else
				if(v_stress < 4)
				{
					// only allow lengthen indicator if stress >= 4.
					shorten = 1;
				}

				if(shorten)
					p++;
			}

			if((v_stress >= 4) && (tr->langopts.param[LOPT_IT_LENGTHEN] == 2))
			{
				// LANG=Italian, lengthen penultimate stressed vowels, unless followed by 2 consonants
				if((v == (vowel_count - 2)) && (syllable_weight[v] == 0))
				{
					*output++ = phcode;
					phcode = phonLENGTHEN;
				}
			}

			v++;
		}

		if(phcode != 1)
			*output++ = phcode;
	}
	*output++ = 0;

}  /* end of SetWordStress */




//=============================================================================================
//   Look up a word in the pronunciation rules
//
//=============================================================================================


#ifdef LOG_TRANSLATE
static char *DecodeRule(const char *group, char *rule)
{//==================================================
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
   char buf[60];
   char buf_pre[60];
	char suffix[20];
	static char output[60];

	static char symbols[] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',
			'@','&','%','+','#','S','D','Z','A','L',' ',' ',' ',' ',' ','N','K','V',' ','T','X','?','W'};

	static char symbols_lg[] = {'A','B','C','H','F','G','Y'};

	match_type = 0;
   buf_pre[0] = 0;
	strcpy(buf,group);
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
		
		if(rb == RULE_ENDING)
		{
			static const char *flag_chars = "ei vtfq t";
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



void AppendPhonemes(Translator *tr, char *string, int size, const char *ph)
{//========================================================================
/* Add new phoneme string "ph" to "string"
	Keeps count of the number of vowel phonemes in the word, and whether these
   can be stressed syllables.  These values can be used in translation rules
*/	
	const char *p;
	unsigned char  c;
	int  unstress_mark;
	int length;

	length = strlen(ph) + strlen(string);
	if(length >= size)
	{
		return;
	}

	/* any stressable vowel ? */
	unstress_mark = 0;
	p = ph;
	while((c = *p++) != 0)
	{
		if(c >= n_phoneme_tab) continue;

		if(phoneme_tab[c]->type == phSTRESS)
		{
			if(phoneme_tab[c]->std_length < 4)
				unstress_mark = 1;
		}
		else
		{
			if(phoneme_tab[c]->type == phVOWEL)
			{
				if(((phoneme_tab[c]->phflags & phUNSTRESSED) == 0) &&
					(unstress_mark == 0))
				{
					tr->word_stressed_count++;
				}
				unstress_mark = 0;
				tr->word_vowel_count++;
			}
		}
	}
	
	if(string != NULL)
		strcat(string,ph);
}   /* end of AppendPhonemes */



static void MatchRule(Translator *tr, char *word[], const char *group, char *rule, MatchRecord *match_out, int word_flags, int dict_flags)
{//=======================================================================================================================================
/* Checks a specified word against dictionary rules.
	Returns with phoneme code string, or NULL if no match found.

	word (indirect) points to current character group within the input word
			This is advanced by this procedure as characters are consumed

	group:  the initial characters used to choose the rules group

	rule:  address of dictionary rule data for this character group

	match_out:  returns best points score

	word_flags:  indicates whether this is a retranslation after a suffix has been removed
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
	int  letter_group;
	int  distance_right;
	int  distance_left;
	int  lg_pts;
	int  n_bytes;
	int add_points;

	MatchRecord match;
	static MatchRecord best;

	int  total_consumed;  /* letters consumed for best match */
	int  group_length;

	unsigned char condition_num;
	char *common_phonemes;  /* common to a group of entries */



	if(rule == NULL)
	{
		match_out->points = 0;
		(*word)++;
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
		distance_right= -6;   /* used to reduce points for matches further away the current letter */
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

			if(rb <= RULE_LINENUM)
			{
				switch(rb)
				{
				case 0:  // no phoneme string for this rule, use previous common rule
					if(common_phonemes != NULL)
					{
						match.phonemes = common_phonemes;
						while(((rb = *match.phonemes++) != 0) && (rb != RULE_PHONEMES))
						{
							if(rb == RULE_CONDITION)
								match.phonemes++;  // skip over condition number
						}
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
					
					if(condition_num >= 32)
					{
						// allow the rule only if the condition number is NOT set
						if((tr->dict_condition & (1L << (condition_num-32))) != 0)
							failed = 1;
					}
					else
					{
						// allow the rule only if the condition number is set
						if((tr->dict_condition & (1L << condition_num)) == 0)
							failed = 1;
					}

					if(!failed)
						match.points++;  // add one point for a matched conditional rule
					break;
				case RULE_LINENUM:
					rule+=2;
					break;
				}
				continue;
			}

			add_points = 0;

			switch(match_type)
			{
			case 0:
				/* match and consume this letter */
				last_letter = letter;
				letter = *post_ptr++;

				if((letter == rb) || ((letter==(unsigned char)REPLACED_E) && (rb=='e')))
				{
					add_points = 21;
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
				letter_xbytes = utf8_in(&letter_w,post_ptr)-1;
				letter = *post_ptr++;

				switch(rb)
				{
				case RULE_LETTERGP:
					letter_group = *rule++ - 'A';
					if(IsLetter(tr, letter_w, letter_group))
					{
						lg_pts = 20;
						if(letter_group==2)
							lg_pts = 19;  // fewer points for C, general consonant
						add_points = (lg_pts-distance_right);
						post_ptr += letter_xbytes;
					}
					else
						failed = 1;
					break;

				case RULE_LETTERGP2:   // match against a list of utf-8 strings
					letter_group = *rule++ - 'A';
					if((n_bytes = IsLetterGroup(tr, post_ptr-1,letter_group,0)) >0)
					{
						add_points = (20-distance_right);
						post_ptr += (n_bytes-1);
					}
					else
						failed =1;
					break;

				case RULE_NOTVOWEL:
					if(!IsLetter(tr, letter_w,0))
					{
						add_points = (20-distance_right);
						post_ptr += letter_xbytes;
					}
					else
						failed = 1;
					break;

				case RULE_DIGIT:
					if(IsDigit(letter_w))
					{
						add_points = (20-distance_right);
						post_ptr += letter_xbytes;
					}
					else
					if(tr->langopts.tone_numbers)
					{
						// also match if there is no digit
						add_points = (20-distance_right);
						post_ptr--;
					}
					else
						failed = 1;
					break;
					
				case RULE_NONALPHA:
					if(!iswalpha(letter_w))
					{
						add_points = (21-distance_right);
						post_ptr += letter_xbytes;
					}
					else
						failed = 1;
					break;

				case RULE_DOUBLE:
					if(letter == last_letter)
						add_points = (21-distance_right);
					else
						failed = 1;
					break;

				case RULE_ALT1:
					if(dict_flags & FLAG_ALT_TRANS)
						add_points = 1;
					else
						failed = 1;
					break;

				case '-':
					if((letter == '-') || ((letter == ' ') && (word_flags & FLAG_HYPHEN_AFTER)))
					{
						add_points = (22-distance_right);    // one point more than match against space
					}
					else
						failed = 1;
					break;

				case RULE_SYLLABLE:
					{
						/* more than specified number of vowel letters to the right */
						char *p = post_ptr + letter_xbytes;

						syllable_count = 1;
						while(*rule == RULE_SYLLABLE)
						{
							rule++;
							syllable_count+=1;   /* number of syllables to match */
						}
						vowel = 0;
						while(letter_w != RULE_SPACE)
						{
							if((vowel==0) && IsLetter(tr, letter_w,LETTERGP_VOWEL2))
							{
								// this is counting vowels which are separated by non-vowels
								syllable_count--;
							}
							vowel = IsLetter(tr, letter_w,LETTERGP_VOWEL2);
							p += utf8_in(&letter_w,p);
						}
						if(syllable_count <= 0)
							add_points = (19-distance_right);
						else
							failed = 1;
					}
					break;

				case RULE_NOVOWELS:
					{
						char *p = post_ptr + letter_xbytes;
						while(letter_w != RULE_SPACE)
						{
							if(IsLetter(tr, letter_w,LETTERGP_VOWEL2))
							{
								failed = 1;
								break;
							}
							p += utf8_in(&letter_w,p);
						}
						if(!failed)
							add_points = (19-distance_right);
					}
					break;

				case RULE_INC_SCORE:
					add_points = 20;      // force an increase in points
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
					// next 3 bytes are a (non-zero) ending type. 2 bytes of flags + suffix length
					match.end_type = (rule[0] << 16) + ((rule[1] & 0x7f) << 8) + (rule[2] & 0x7f);
					rule += 3;
					break;

				case RULE_NO_SUFFIX:
					if(word_flags & FLAG_SUFFIX_REMOVED)
						failed = 1;             // a suffix has been removed
					else
						add_points = 1;
					break;

				default:
					if(letter == rb)
					{
						if(letter == RULE_SPACE)
							add_points = (21-distance_right);
						else
							add_points = (21-distance_right);
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
				letter_xbytes = utf8_in2(&letter_w,pre_ptr,1)-1;
				letter = *pre_ptr;

				switch(rb)
				{
				case RULE_LETTERGP:
					letter_group = *rule++ - 'A';
					if(IsLetter(tr, letter_w,letter_group))
					{
						lg_pts = 20;
						if(letter_group==2)
							lg_pts = 19;  // fewer points for C, general consonant
						add_points = (lg_pts-distance_left);
						pre_ptr -= letter_xbytes;
					}
					else
						failed = 1;
					break;

				case RULE_LETTERGP2:   // match against a list of utf-8 strings
					letter_group = *rule++ - 'A';
					if((n_bytes = IsLetterGroup(tr, pre_ptr,letter_group,1)) >0)
					{
						add_points = (20-distance_right);
						pre_ptr -= (n_bytes-1);
					}
					else
						failed =1;
					break;

				case RULE_NOTVOWEL:
					if(!IsLetter(tr, letter_w,0))
					{
						add_points = (20-distance_left);
						pre_ptr -= letter_xbytes;
					}
					else
						failed = 1;
					break;

				case RULE_DOUBLE:
					if(letter == last_letter)
						add_points = (21-distance_left);
					else
						failed = 1;
					break;

				case RULE_DIGIT:
					if(IsDigit(letter_w))
					{
						add_points = (21-distance_left);
						pre_ptr -= letter_xbytes;
					}
					else
						failed = 1;
					break;

				case RULE_NONALPHA:
					if(!iswalpha(letter_w))
					{
						add_points = (21-distance_right);
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
					if(syllable_count <= tr->word_vowel_count)
						add_points = (19-distance_left);
					else
						failed = 1;
					break;

				case RULE_STRESSED:
					if(tr->word_stressed_count > 0)
						add_points = 19;
					else
						failed = 1;
					break;

				case RULE_NOVOWELS:
					{
						char *p = pre_ptr - letter_xbytes - 1;
						while(letter_w != RULE_SPACE)
						{
							if(IsLetter(tr, letter_w,LETTERGP_VOWEL2))
							{
								failed = 1;
								break;
							}
							p -= utf8_in2(&letter_w,p,1);
						}
						if(!failed)
							add_points = 3;
					}
					break;

				case RULE_IFVERB:
					if(tr->expect_verb)
						add_points = 1;
					else
						failed = 1;
					break;

				case RULE_CAPITAL:
					if(word_flags & FLAG_FIRST_UPPER)
						add_points = 1;
					else
						failed = 1;
					break;

				case '.':
					// dot in pre- section, match on any dot before this point in the word
					for(p=pre_ptr; *p != ' '; p--)
					{
						if(*p == '.')
						{
							add_points = 50;
							break;
						}
					}
					if(*p == ' ')
						failed = 1;
					break;

				case '-':
					if((letter == '-') || ((letter == ' ') && (word_flags & FLAG_HYPHEN)))
					{
						add_points = (22-distance_right);       // one point more than match against space
					}
					else
						failed = 1;
					break;

				default:
					if(letter == rb)
					{
						if(letter == RULE_SPACE)
							add_points = 4;
						else
							add_points = (21-distance_left);
					}
					else
						failed = 1;
					break;
				}
				break;
			}

			if(failed == 0)
				match.points += add_points;
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
			if((option_phonemes == 2) && (match.points > 0) && ((word_flags & FLAG_NO_TRACE) == 0))
			{
				// show each rule that matches, and it's points score
				int pts;
				char decoded_phonemes[80];

				// note: 'count' contains the rule number, if we want to include it
				pts = match.points;
				if(group_length > 1)
					pts += 35;    // to account for an extra letter matching
				DecodePhonemes(match.phonemes,decoded_phonemes);
				fprintf(f_trans,"%3d\t%s [%s]\n",pts,DecodeRule(group,rule_start),decoded_phonemes);
			}
#endif

		}

		/* skip phoneme string to reach start of next template */
		while(*rule++ != 0);
	}

#ifdef LOG_TRANSLATE
	if((option_phonemes == 2) && ((word_flags & FLAG_NO_TRACE)==0))
	{
		if(group_length <= 1)
			fprintf(f_trans,"\n");
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




int TranslateRules(Translator *tr, char *p_start, char *phonemes, int ph_size, char *end_phonemes, int word_flags, unsigned int *dict_flags)
{//=====================================================================================================================================
/* Translate a word bounded by space characters
   Append the result to 'phonemes' and any standard prefix/suffix in 'end_phonemes' */

	unsigned char  c, c2;
	unsigned int  c12;
	int wc=0;
	int wc_prev;
	int wc_bytes;
	char *p2;           /* copy of p for use in double letter chain match */
	int  found;
	int  g;             /* group chain number */
	int  g1;            /* first group for this letter */
	int  n;
	int  letter;
	int  any_alpha=0;
	int  ix;
	unsigned int  digit_count=0;
	char *p;
	int  dict_flags0=0;
	MatchRecord match1;
	MatchRecord match2;
	char ph_buf[40];
	char word_copy[N_WORD_BYTES];
	static const char str_pause[2] = {phonPAUSE_NOLINK,0};

	char group_name[4];

	if(tr->data_dictrules == NULL)
		return(0);

	if(dict_flags != NULL)
		dict_flags0 = dict_flags[0];

	for(ix=0; ix<(N_WORD_BYTES-1);)
	{
		c = p_start[ix];
		word_copy[ix++] = c;
		if(c == 0)
			break;
	}
	word_copy[ix] = 0;


#ifdef LOG_TRANSLATE
	if((option_phonemes == 2) && ((word_flags & FLAG_NO_TRACE)==0))
	{
		char wordbuf[120];
		int  ix;

		for(ix=0; ((c = p_start[ix]) != ' ') && (c != 0); ix++)
		{
			wordbuf[ix] = c;
		}
		wordbuf[ix] = 0;
		fprintf(f_trans,"Translate '%s'\n",wordbuf);
	}
#endif

	p = p_start;
	tr->word_vowel_count = 0;
	tr->word_stressed_count = 0;
	
	if(end_phonemes != NULL)
		end_phonemes[0] = 0;
	
	while(((c = *p) != ' ') && (c != 0))
	{
		wc_prev = wc;
		wc_bytes = utf8_in(&wc,p);
		if(IsAlpha(wc))
			any_alpha++;

		n = tr->groups2_count[c];
		if(IsDigit(wc) && ((tr->langopts.tone_numbers == 0) || !any_alpha))
		{
			// lookup the number in *_list not *_rules
	char string[8];
	char buf[40];
			string[0] = '_';
			memcpy(&string[1],p,wc_bytes);
			string[1+wc_bytes] = 0;
			Lookup(tr, string,buf);
			if(++digit_count >= 2)
			{ 
				strcat(buf,str_pause);
				digit_count=0;
			}
			AppendPhonemes(tr,phonemes,ph_size,buf);
			p += wc_bytes;
			continue;
		}
		else
		{
			digit_count = 0;
			found = 0;
	
			if(n > 0)
			{
				/* there are some 2 byte chains for this initial letter */
				c2 = p[1];
				c12 = c + (c2 << 8);   /* 2 characters */
	
				g1 = tr->groups2_start[c];
				for(g=g1; g < (g1+n); g++)
				{
					if(tr->groups2_name[g] == c12)
					{
						found = 1;

						group_name[0] = c;
						group_name[1] = c2;
						group_name[2] = 0;
						p2 = p;
						MatchRule(tr, &p2, group_name, tr->groups2[g], &match2, word_flags, dict_flags0);
						if(match2.points > 0)
							match2.points += 35;   /* to acount for 2 letters matching */

						/* now see whether single letter chain gives a better match ? */
						group_name[1] = 0;
						MatchRule(tr, &p, group_name, tr->groups1[c], &match1, word_flags, dict_flags0);

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
	
				if(tr->groups1[c] != NULL)
					MatchRule(tr, &p, group_name, tr->groups1[c], &match1, word_flags, dict_flags0);
				else
				{
					// no group for this letter, use default group
					MatchRule(tr, &p, "", tr->groups1[0], &match1, word_flags, dict_flags0);

					if((match1.points == 0) && ((option_sayas & 0x10) == 0))
					{
						n = utf8_in(&letter,p-1)-1;

						if(tr->letter_bits_offset > 0)
						{
							// not a Latin alphabet, switch to the default Latin alphabet language
							if((letter <= 0x241) && iswalpha(letter))
							{
								sprintf(phonemes,"%c%s",phonSWITCH,tr->langopts.ascii_language);
								return(0);
							}
						}
#ifdef deleted
// can't switch to a tone language, because the tone-phoneme numbers are not valid for the original language
						if((letter >= 0x4e00) && (letter < 0xa000) && (tr->langopts.ideographs != 1))
						{
							// Chinese ideogram
							sprintf(phonemes,"%czh",phonSWITCH);
							return(0);
						}
#endif

						// is it a bracket ?
						if(IsBracket(letter))
						{
							if(pre_pause < 4)
								pre_pause = 4;
						}

						// no match, try removing the accent and re-translating the word
						if((letter >= 0xc0) && (letter <= 0x241) && ((ix = remove_accent[letter-0xc0]) != 0))
						{
							// within range of the remove_accent table
							if((p[-2] != ' ') || (p[n] != ' '))
							{
								// not the only letter in the word
								p2 = p-1;
								p[-1] = ix;
								while((p[0] = p[n]) != ' ')  p++;
								while(n-- > 0) *p++ = ' ';  // replacement character must be no longer than original
	
								if(tr->langopts.param[LOPT_DIERESES] && (lookupwchar(diereses_list,letter) > 0))
								{
									// vowel with dieresis, replace and continue from this point
									p = p2;
									continue;
								}
	
								phonemes[0] = 0;  // delete any phonemes which have been produced so far
								p = p_start;
								tr->word_vowel_count = 0;
								tr->word_stressed_count = 0;
								continue;  // start again at the beginning of the word
							}
						}
						else
						if((letter >= 0x3200) && (letter < 0xa700) && (end_phonemes != NULL))
						{
							// ideograms
							// outside the range of the accent table, speak the unknown symbol sound
							Lookup(tr, "_??", ph_buf);
							match1.phonemes = ph_buf;
							match1.points = 1;
							p += (wc_bytes-1);
						}
					}
				}

				if(match1.points == 0)
				{
					if((wc >= 0x300) && (wc <= 0x36f))
					{
						// combining accent inside a word, ignore
					}
					else
					if(IsAlpha(wc))
					{
						if((any_alpha > 1) || (p[wc_bytes-1] > ' '))
						{
							// an unrecognised character in a word, abort and then spell the word
							phonemes[0] = 0;
							if(dict_flags != NULL)
								dict_flags[0] |= FLAG_SPELLWORD;
							break;
						}
					}
					else
					{
						LookupLetter(tr, wc, -1, ph_buf);
						if(ph_buf[0])
						{
							match1.phonemes = ph_buf;
							match1.points = 1;
						}
					}
					p += (wc_bytes-1);
				}
				else
				{
					tr->phonemes_repeat_count = 0;
				}
			}
		}

		if(match1.phonemes == NULL)
			match1.phonemes = "";
	
		if(match1.points > 0)
		{
			if((match1.phonemes[0] == phonSWITCH) && ((word_flags & FLAG_DONT_SWITCH_TRANSLATOR)==0))
			{
				// an instruction to switch language, return immediately so we can re-translate
				strcpy(phonemes,match1.phonemes);
				return(0);
			}
	
			if((match1.end_type != 0) && (end_phonemes != NULL))
			{
				/* a standard ending has been found, re-translate the word without it */
				if((match1.end_type & SUFX_P) && (word_flags & FLAG_NO_PREFIX))
				{
					// ignore the match on a prefix
				}
				else
				{
					if((match1.end_type & SUFX_P) && ((match1.end_type & 0x7f) == 0))
					{
						// no prefix length specified
						match1.end_type |= p - p_start;
					}
					strcpy(end_phonemes,match1.phonemes);
					memcpy(p_start,word_copy,strlen(word_copy));
					return(match1.end_type);
				}
			}
			if(match1.del_fwd != NULL)
				*match1.del_fwd = REPLACED_E;
			AppendPhonemes(tr,phonemes,ph_size,match1.phonemes);
		}
	}

	// any language specific changes ?
	ApplySpecialAttribute(tr,phonemes,dict_flags0);
	memcpy(p_start,word_copy,strlen(word_copy));

	return(0);
}   /* end of TranslateRules */


void ApplySpecialAttribute2(Translator *tr, char *phonemes, int dict_flags)
{//========================================================================
	// apply after the translation is complete
	int ix;
	int len;
	char *p;

	len = strlen(phonemes);

	switch(tr->translator_name)
	{
	case L('i','t'):
		for(ix=0; ix<(len-1); ix++)
		{
			if(phonemes[ix] == phonSTRESS_P)
			{
				p = &phonemes[ix+1];
				if((dict_flags & FLAG_ALT2_TRANS) != 0)
				{
					if(*p == PhonemeCode('E'))
						*p = PhonemeCode('e');
					if(*p == PhonemeCode('O'))
						*p = PhonemeCode('o');
				}
				else
				{
					if(*p == PhonemeCode('e'))
						*p = PhonemeCode('E');
					if(*p == PhonemeCode('o'))
						*p = PhonemeCode('O');
				}
				break;
			}
		}
		break;
	}
}  // end of ApplySpecialAttribute2


void ApplySpecialAttribute(Translator *tr, char *phonemes, int dict_flags)
{//=======================================================================
// Amend the translated phonemes according to an attribute which is specific for the language.
	int len;
	int ix;
	char *p_end;
	int phoneme_1;

	if((dict_flags & (FLAG_ALT_TRANS | FLAG_ALT2_TRANS)) == 0)
		return;

	len = strlen(phonemes);
	p_end = &phonemes[len-1];

	switch(tr->translator_name)
	{
	case L('d','e'):
		if(p_end[0] == PhonemeCode2('i',':'))
		{
			// words ends in ['i:], change to [=I@]
			p_end[-1] = phonSTRESS_PREV;
			p_end[0] = PhonemeCode('I');
			p_end[1] = phonSCHWA;
			p_end[2] = 0;
		}
		break;

	case L('p','t'):
		phoneme_1 = PhonemeCode('o');
		for(ix=0; ix<(len-1); ix++)
		{
			if(phonemes[ix] == phoneme_1)
			{
				phonemes[ix] = PhonemeCode('O');
				break;
			}
		}
		break;

	case L('r','o'):
		if(p_end[0] == PhonemeCode('j'))
		{
			// word end in [j], change to ['i]
			p_end[0] = phonSTRESS_P;
			p_end[1] = PhonemeCode('i');
			p_end[2] = 0;
		}
		break;
	}
}  // end of ApplySpecialAttribute



//=============================================================================================
//   Look up a word in the pronunciation dictionary list
//   - exceptions which override the usual pronunciation rules, or which give a word
//     special properties, such as pronounce as unstressed
//=============================================================================================

// common letter pairs, encode these as a single byte
static const short pairs_ru[] = { 
0x010c, //  ла   21052  0x23
0x010e, //  на   18400
0x0113, //  та   14254
0x0301, //  ав   31083
0x030f, //  ов   13420
0x060e, //  не   21798
0x0611, //  ре   19458
0x0903, //  ви   16226
0x0b01, //  ак   14456
0x0b0f, //  ок   17836
0x0c01, //  ал   13324
0x0c09, //  ил   16877
0x0e01, //  ан   15359
0x0e06, //  ен   13543  0x30
0x0e09, //  ин   17168
0x0e0e, //  нн   15973
0x0e0f, //  он   22373
0x0e1c, //  ын   15052
0x0f03, //  во   24947
0x0f11, //  ро   13552
0x0f12, //  со   16368
0x100f, //  оп   19054
0x1011, //  рп   17067
0x1101, //  ар   23967
0x1106, //  ер   18795
0x1109, //  ир   13797
0x110f, //  ор   21737
0x1213, //  тс   25076
0x1220, //  яс   14310
0x7fff};
//0x040f  ог   12976
//0x1306  ет   12826
//0x0f0d  мо   12688


int TransposeAlphabet(char *text, int offset, int min, int max)
{//============================================================
// transpose cyrillic alphabet (for example) into ascii (single byte) character codes
// return: number of bytes, bit 6: 1=used compression
	int c;
	int c2;
	int ix;
	char *p = text;
	char *p2 = text;
	int all_alpha=1;
	int bits;
	int acc;

	do {
		p += utf8_in(&c,p);
		if((c >= min) && (c <= max))
		{
			*p2++ = c - offset;
		}
		else
		if(c != 0)
		{
			p2 += utf8_out(c,p2);
			all_alpha=0;
		}
	} while (c != 0);
	*p2 = 0;

	if(all_alpha)
	{
		// compress to 6 bits per character
		acc=0;
		bits=0;

		p = text;
		p2 = text;
		while((c = *p++) != 0)
		{
			c2 = c + (*p << 8);
			for(ix=0; c2 >= pairs_ru[ix]; ix++)
			{
				if(c2 == pairs_ru[ix])
				{
					// found an encoding for a 2-character pair
					c = ix + 0x23;   // 2-character codes start at 0x23
					p++;
					break;
				}
			}
			acc = (acc << 6) + (c & 0x3f);
			bits += 6;

			if(bits >= 8)
			{
				bits -= 8;
				*p2++ = (acc >> bits); 
			}
		}
		if(bits > 0)
		{
			*p2++ = (acc << (8-bits));
		}
		*p2 = 0;
		return((p2 - text) | 0x40);  // bit 6 indicates compressed characters
	}
	return(p2 - text);
}  // end of TransposeAlphabet




static const char *LookupDict2(Translator *tr, const char *word, const char *word2,
		char *phonetic, unsigned int *flags, int end_flags, WORD_TAB *wtab)
//=====================================================================================
/* Find an entry in the word_dict file for a specified word.
   Returns NULL if no match, else returns 'word_end'

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
	int  wlen;
	unsigned char flag;
	unsigned int  dictionary_flags;
	unsigned int  dictionary_flags2;
	int  condition_failed=0;
	int  n_chars;
	int  no_phonemes;
	int  skipwords;
	int  ix;
	const char *word_end;
	const char *word1;
	int wflags = 0;
	char word_buf[N_WORD_BYTES];

	if(wtab != NULL)
	{
		wflags = wtab->flags;
	}

	word1 = word;
	if(tr->transpose_offset > 0)
	{
		strcpy(word_buf,word);
		wlen = TransposeAlphabet(word_buf, tr->transpose_offset, tr->transpose_min, tr->transpose_max);
		word = word_buf;
	}
	else
	{
		wlen = strlen(word);
	}

	hash = HashDictionary(word);
	p = tr->dict_hashtab[hash];

	if(p == NULL)
	{
		if(flags != NULL)
			*flags = 0;
		return(0);
	}

	// Find the first entry in the list for this hash value which matches.
	// This corresponds to the last matching entry in the *_list file.

	while(*p != 0)
	{
		next = p + p[0];

		if(((p[1] & 0x7f) != wlen) || (memcmp(word,&p[2],wlen & 0x3f) != 0))
		{
			// bit 6 of wlen indicates whether the word has been compressed; so we need to match on this also.
			p = next;
			continue;
		}

		/* found matching entry. Decode the phonetic string */
		word_end = word2;

		dictionary_flags = 0;
		dictionary_flags2 = 0;
		no_phonemes = p[1] & 0x80;

		p += ((p[1] & 0x3f) + 2);

		if(no_phonemes)
		{
			phonetic[0] = 0;
			phoneme_len = 0;
		}
		else
		{
			strcpy(phonetic,p);
			phoneme_len = strlen(p);
			p += (phoneme_len + 1);
		}

		while(p < next)
		{
			// examine the flags which follow the phoneme string

			flag = *p++;
			if(flag >= 100)
			{
				// conditional rule
				if(flag >= 132)
				{
					// fail if this condition is set
					if((tr->dict_condition & (1 << (flag-132))) != 0)
						condition_failed = 1;
				}
				else
				{
					// allow only if this condition is set
					if((tr->dict_condition & (1 << (flag-100))) == 0)
						condition_failed = 1;
				}
			}
			else
			if(flag > 80)
			{
				// flags 81 to 90  match more than one word
				// This comes after the other flags
				n_chars = next - p;
				skipwords = flag - 80;

				// don't use the contraction if any of the words are emphasized
				for(ix=0; ix <= skipwords; ix++)
				{
					if(wflags & FLAG_EMPHASIZED2)
					{
						condition_failed = 1;
					}
				}

				if(memcmp(word2,p,n_chars) != 0)
					condition_failed = 1;

				if(condition_failed)
				{
					p = next;
					break;
				}

				dictionary_flags |= FLAG_SKIPWORDS;
				dictionary_skipwords = skipwords;
				p = next;
				word_end = word2 + n_chars;
			}
			else
			if(flag > 64)
			{
				// stressed syllable information, put in bits 0-3
				dictionary_flags = (dictionary_flags & ~0xf) | (flag & 0xf);
				if((flag & 0xc) == 0xc)
					dictionary_flags |= FLAG_STRESS_END;
			}
			else
			if(flag >= 32)
			{
				dictionary_flags2 |= (1L << (flag-32));
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

		if((end_flags & FLAG_SUFX)==0)
		{
			// no suffix has been removed
			if(dictionary_flags & FLAG_STEM)
				continue;   // this word must have a suffix
		}

		if((end_flags & SUFX_P) && (dictionary_flags & (FLAG_ONLY | FLAG_ONLY_S)))
			continue;    // $only or $onlys, don't match if a prefix has been removed

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

		if(dictionary_flags2 & FLAG_HYPHENATED)
		{
			if(!(wflags & FLAG_HYPHEN_AFTER))
			{
				continue;
			}
		}
		if(dictionary_flags2 & FLAG_CAPITAL)
		{
			if(!(wflags & FLAG_FIRST_UPPER))
			{
				continue;
			}
		}
		if(dictionary_flags2 & FLAG_ALLCAPS)
		{
			if(!(wflags & FLAG_ALL_UPPER))
			{
				continue;
			}
		}

		if((dictionary_flags & FLAG_ATEND) && (word_end < tr->clause_end))
		{
			// only use this pronunciation if it's the last word of the clause
			continue;
		}

		if(dictionary_flags2 & FLAG_VERB)
		{
			// this is a verb-form pronunciation

			if(tr->expect_verb || (tr->expect_verb_s && (end_flags & FLAG_SUFX_S)))
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
		if(dictionary_flags2 & FLAG_PAST)
		{
			if(!tr->expect_past)
			{
				/* don't use the 'past' pronunciation unless we are
					expecting past tense */
				continue;
			}
		}
		if(dictionary_flags2 & FLAG_NOUN)
		{
			if(!tr->expect_noun)
			{
				/* don't use the 'noun' pronunciation unless we are
					expecting a noun */
				continue;
			}
		}

		if(flags != NULL)
		{
			flags[0] = dictionary_flags | FLAG_FOUND_ATTRIBUTES;
			flags[1] = dictionary_flags2;
		}

		if(phoneme_len == 0)
		{
			if(option_phonemes == 2)
			{
				fprintf(f_trans,"Flags:  %s  %s\n",word1,print_dictionary_flags(flags));
			}
			return(0);    // no phoneme translation found here, only flags. So use rules
		}

		if(flags != NULL)
			flags[0] |= FLAG_FOUND;   // this flag indicates word was found in dictionary
		
		if(option_phonemes == 2)
		{
			unsigned int flags1 = 0;
			char ph_decoded[N_WORD_PHONEMES];
			int textmode;

			DecodePhonemes(phonetic,ph_decoded);
			if(flags != NULL)
				flags1 = flags[0];

			if((dictionary_flags & FLAG_TEXTMODE) == 0)
				textmode = 0;
			else
				textmode = 1;

			if(textmode == translator->langopts.textmode)
			{
				// only show this line if the word translates to phonemes, not replacement text
				fprintf(f_trans,"Found: %s [%s]  %s\n",word1,ph_decoded,print_dictionary_flags(flags));
			}
		}
		return(word_end);

	}
	return(0);
}   //  end of LookupDict2



int LookupDictList(Translator *tr, char **wordptr, char *ph_out, unsigned int *flags, int end_flags, WORD_TAB *wtab)
//==================================================================================================================
/* Lookup a specified word in the word dictionary.
   Returns phonetic data in 'phonetic' and bits in 'flags'

   end_flags:  indicates if a suffix has been removed
*/
{
	int  length;
	const char *found;
	const char *word1;
	const char *word2;
	unsigned char c;
	int  nbytes;
	int  len;
	char word[N_WORD_BYTES];
	static char word_replacement[N_WORD_BYTES];

	length = 0;
	word2 = word1 = *wordptr;

	while((word2[nbytes = utf8_nbytes(word2)]==' ') && (word2[nbytes+1]=='.'))
	{
		// look for an abbreviation of the form a.b.c
		// try removing the spaces between the dots and looking for a match
		memcpy(&word[length],word2,nbytes);
		length += nbytes;
		word[length++] = '.';
		word2 += nbytes+3;
	}
	if(length > 0)
	{
		// found an abbreviation containing dots
		nbytes = 0;
		while(((c = word2[nbytes]) != 0) && (c != ' '))
		{
			nbytes++;
		}
		memcpy(&word[length],word2,nbytes);
		word[length+nbytes] = 0;
		found =  LookupDict2(tr, word, word2, ph_out, flags, end_flags, wtab);
		if(found)
		{
			// set the skip words flag
			flags[0] |= FLAG_SKIPWORDS;
			dictionary_skipwords = length;
			return(1);
		}
	}

	for(length=0; length<N_WORD_BYTES; length++)
	{
		if(((c = *word1++)==0) || (c == ' '))
			break;
		word[length] = c;
	}
	word[length] = 0;

	found = LookupDict2(tr, word, word1, ph_out, flags, end_flags, wtab);

	if(flags[0] & FLAG_MAX3)
	{
		if(strcmp(ph_out, tr->phonemes_repeat) == 0)
		{
			tr->phonemes_repeat_count++;
			if(tr->phonemes_repeat_count > 3)
			{
				ph_out[0] = 0;
			}
		}
		else
		{
			strncpy0(tr->phonemes_repeat, ph_out, sizeof(tr->phonemes_repeat));
			tr->phonemes_repeat_count = 1;
		}
	}
	else
	{
		tr->phonemes_repeat_count = 0;
	}


	if((found == 0) && (flags[1] & FLAG_ACCENT))
	{
		int letter;
		word2 = word;
		if(*word2 == '_') word2++;
		len = utf8_in(&letter, word2);
		LookupAccentedLetter(tr,letter, ph_out);
		found = word2 + len;
	}

	if(found == 0)
	{
		ph_out[0] = 0;
	
		// try modifications to find a recognised word
	
		if((end_flags & FLAG_SUFX_E_ADDED) && (word[length-1] == 'e'))
		{
			// try removing an 'e' which has been added by RemoveEnding
			word[length-1] = 0;
			found = LookupDict2(tr, word, word1, ph_out, flags, end_flags, wtab);
		}
		else
		if((end_flags & SUFX_D) && (word[length-1] == word[length-2]))
		{
			// try removing a double letter
			word[length-1] = 0;
			found = LookupDict2(tr, word, word1, ph_out, flags, end_flags, wtab);
		}
	}

	if(found)
	{
		// if textmode is the default, then words which have phonemes are marked.
		if(tr->langopts.textmode)
			*flags ^= FLAG_TEXTMODE;

		if(*flags & FLAG_TEXTMODE)
		{
			// the word translates to replacement text, not to phonemes

			if(end_flags & FLAG_ALLOW_TEXTMODE)
			{
				// only use replacement text if this is the original word, not if a prefix or suffix has been removed
				word_replacement[0] = 0;
				word_replacement[1] = ' ';
				sprintf(&word_replacement[2],"%s ",ph_out);   // replacement word, preceded by zerochar and space

				word1 = *wordptr;
				*wordptr = &word_replacement[2];

				if(option_phonemes == 2)
				{
					len = found - word1;
					memcpy(word,word1,len);   // include multiple matching words
					word[len] = 0;
					fprintf(f_trans,"Replace: %s  %s\n",word,*wordptr);
				}
			}

			ph_out[0] = 0;
			return(0);
		}

		return(1);
	}

	ph_out[0] = 0;
	return(0);
}   //  end of LookupDictList



int Lookup(Translator *tr, const char *word, char *ph_out)
{//===================================================
	unsigned int flags[2];
	flags[0] = flags[1] = 0;
	char *word1 = (char *)word;
	return(LookupDictList(tr, &word1, ph_out, flags, 0, NULL));
}



int RemoveEnding(Translator *tr, char *word, int end_type, char *word_copy)
{//========================================================================
/* Removes a standard suffix from a word, once it has been indicated by the dictionary rules.
   end_type: bits 0-6  number of letters
             bits 8-14  suffix flags

	word_copy: make a copy of the original word
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
	i = word_end - word;
	memcpy(word_copy,word,i);
	word_copy[i] = 0;

	// look for multibyte characters to increase the number of bytes to remove
	for(len_ending = i = (end_type & 0x3f); i>0 ;i--)   // num.of characters of the suffix
	{
		word_end--;
		while((*word_end & 0xc0) == 0x80)
		{
			word_end--;  // for multibyte characters
			len_ending++;
		}
	}
	
	// remove bytes from the end of the word and replace them by spaces
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
		if(IsLetter(tr, word_end[-1],LETTERGP_VOWEL2) && IsLetter(tr, word_end[0],1))
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
if(option_phonemes == 2)
{
	fprintf(f_trans,"add e\n");
}
#endif
		}
	}

	if((end_type & SUFX_V) && (tr->expect_verb==0))
		tr->expect_verb = 1;         // this suffix indicates the verb pronunciation


	if((strcmp(ending,"s")==0) || (strcmp(ending,"es")==0))
		end_flags |= FLAG_SUFX_S;

	if(strcmp(ending,"'s")==0)
		end_flags &= ~FLAG_SUFX;  // don't consider 's as an added suffix

	return(end_flags);
}   /* end of RemoveEnding */


