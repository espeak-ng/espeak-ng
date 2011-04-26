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
#include <wchar.h>

#include "speak_lib.h"
#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "translate.h"


int dictionary_skipwords;
char dictionary_name[40];

extern char *print_dictionary_flags(unsigned int *flags);
extern char *DecodeRule(const char *group_chars, int group_length, char *rule, int control);

// accented characters which indicate (in some languages) the start of a separate syllable
//static const unsigned short diereses_list[7] = {L'ä',L'ë',L'ï',L'ö',L'ü',L'ÿ',0};
static const unsigned short diereses_list[7] = {0xe4,0xeb,0xef,0xf6,0xfc,0xff,0};

// convert characters to an approximate 7 bit ascii equivalent
// used for checking for vowels (up to 0x259=schwa)
#define N_REMOVE_ACCENT  0x25e
static unsigned char remove_accent[N_REMOVE_ACCENT] = {
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
'z', 0,  0, 'b','u','v','e','e','j','j','q','q','r','r','y','y',  // 240
'a','a','a','b','o','c','d','d','e','e','e','e','e','e' };





void strncpy0(char *to,const char *from, int size)
{//===============================================
	// strcpy with limit, ensures a zero terminator
	strncpy(to,from,size);
	to[size-1] = 0;
}


int Reverse4Bytes(int word)
{//========================
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


int LookupMnem(MNEM_TAB *table, const char *string)
{//================================================
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
	memset(tr->groups3,0,sizeof(tr->groups3));

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
				*pw = Reverse4Bytes(*pw);
				pw++;
				*pw = Reverse4Bytes(*pw);
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
			c2 = p_name[1];

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
			if(c == 1)
			{
				// index by offset from letter base
				tr->groups3[c2 - 1] = p;
			}
			else
			{
				if(tr->groups2_start[c] == 255)
					tr->groups2_start[c] = tr->n_groups2;
	
				tr->groups2_count[c]++;
				tr->groups2[tr->n_groups2] = p;
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
	strcpy(tr->dictionary_name, name);

	// Load a pronunciation data file into memory
	// bytes 0-3:  offset to rules data
	// bytes 4-7:  number of hash table entries
	sprintf(fname,"%s%c%s_dict",path_home,PATHSEP,name);
	size = GetFileLength(fname);

	if(tr->data_dictlist != NULL)
	{
		Free(tr->data_dictlist);
		tr->data_dictlist = NULL;
	}

	f = fopen(fname,"rb");
	if((f == NULL) || (size <= 0))
	{
		if(no_error == 0)
		{
			fprintf(stderr,"Can't read dictionary file: '%s'\n",fname);
		}
		return(1);
	}

	tr->data_dictlist = Alloc(size);
	size = fread(tr->data_dictlist,1,size,f);
	fclose(f);


	pw = (int *)(tr->data_dictlist);
	length = Reverse4Bytes(pw[1]);

	if(size <= (N_HASH_DICT + sizeof(int)*2))
	{
		fprintf(stderr,"Empty _dict file: '%s\n",fname);
		return(2);
	}

	if((Reverse4Bytes(pw[0]) != N_HASH_DICT) ||
	   (length <= 0) || (length > 0x8000000))
	{
		fprintf(stderr,"Bad data: '%s' (%x length=%x)\n",fname,Reverse4Bytes(pw[0]),length);
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
   outptr contains encoded phonemes, unrecognized phoneme stops the encoding
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
				// not recognised, report and ignore
				bad_phoneme[0] = *p;
				bad_phoneme[1] = 0;
				*outptr++ = 0;
				return(p+1);
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
	
		if((ph->type == phSTRESS) && (ph->std_length <= 4) && (ph->program == 0))
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


// using Kirschenbaum to IPA translation, ascii 0x20 to 0x7f
unsigned short ipa1[96] = {
0x20,0x21,0x22,0x2b0,0x24,0x25,0x0e6,0x2c8,0x28,0x27e,0x2a,0x2b,0x2cc,0x2d,0x2e,0x2f,
0x252,0x31,0x32,0x25c,0x34,0x35,0x36,0x37,0x275,0x39,0x2d0,0x2b2,0x3c,0x3d,0x3e,0x294,
0x259,0x251,0x3b2,0xe7,0xf0,0x25b,0x46,0x262,0x127,0x26a,0x25f,0x4b,0x4c,0x271,0x14b,0x254,
0x3a6,0x263,0x280,0x283,0x3b8,0x28a,0x28c,0x153,0x3c7,0xf8,0x292,0x32a,0x5c,0x5d,0x5e,0x5f,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x261,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x303,0x7f
};


static void WritePhMnemonic(char *phon_out, int *ix, PHONEME_TAB *ph, PHONEME_LIST *plist)
{//=======================================================================================
	int c;
	int mnem;
	int len;
	int first;
	unsigned int ipa_control=0;  // first byte of ipa string may control the phoneme name interpretation. 0x20 = ignore this phoneme
	PHONEME_DATA phdata;

	if(option_phonemes == 3)
	{
		// has an ipa name been defined for this phoneme ?
		phdata.ipa_string[0] = 0;

		if(plist == NULL)
		{
			InterpretPhoneme2(ph->code, &phdata);
		}
		else
		{
			InterpretPhoneme(NULL, 0, plist, &phdata);
		}

		len = strlen(phdata.ipa_string);
		if(len > 0)
		{
			if((ipa_control = phdata.ipa_string[0]) > 0x20)
			{
				strcpy(&phon_out[*ix], phdata.ipa_string);
				*ix += len;
			}
			if(ipa_control >= 0x20)
				return;  // 0x20 = ignore phoneme
		}
	}

	first = 1;
	for(mnem = ph->mnemonic; (c = mnem & 0xff) != 0; mnem = mnem >> 8)
	{
		if((c == '/') && (option_phoneme_variants==0))
			break;      // discard phoneme variant indicator

		if(option_phonemes == 3)
		{
			// convert from ascii to ipa
			if(first && (c == '_'))
				break;    // don't show pause phonemes

			if((c == '#') && (ph->type == phVOWEL))
				break;   // # is subscript-h, but only for consonants

			// ignore digits after the first character
			if(!first && isdigit(c))
				continue;

			if((c >= 0x20) && (c < 128))
				c = ipa1[c-0x20];

			*ix += utf8_out(c, &phon_out[*ix]);
		}
		else
		{
			phon_out[(*ix)++]= c;
		}
		first = 0;
	}
}  // end of WritePhMnemonic



void GetTranslatedPhonemeString(char *phon_out, int n_phon_out)
{//============================================================
/* Can be called after a clause has been translated into phonemes, in order
   to display the clause in phoneme mnemonic form.
*/

	int  ix;
	int  phon_out_ix=0;
	int  stress;
	unsigned int c;
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
					c = 0;
					if(stress > 5) stress = 5;

					if(option_phonemes == 3)
					{
						c = 0x2cc;  // ipa, secondary stress
						if(stress > 3)
							c = 0x02c8;  // ipa, primary stress
					}
					else
					{
						c = stress_chars[stress];
					}

					if(c != 0)
					{
						phon_out_ix += utf8_out(c, &phon_out[phon_out_ix]);
					}
				}
			}

			if(plist->ph->code == phonSWITCH)
			{
				// the tone_ph field contains a phoneme table number
				p = phoneme_tab_list[plist->tone_ph].name;

				sprintf(&phon_out[phon_out_ix], "(%s)", p);
				phon_out_ix += (strlen(p) + 2);
			}
			else
			{
				WritePhMnemonic(phon_out, &phon_out_ix, plist->ph, plist);
	
				if(plist->synthflags & SFLAG_LENGTHEN)
				{
					WritePhMnemonic(phon_out, &phon_out_ix, phoneme_tab[phonLENGTHEN], NULL);
				}
				if((plist->synthflags & SFLAG_SYLLABLE) && (plist->type != phVOWEL))
				{
					// syllablic consonant
					WritePhMnemonic(phon_out, &phon_out_ix, phoneme_tab[phonSYLLABIC], NULL);
				}
				if(plist->tone_ph > 0)
				{
					WritePhMnemonic(phon_out, &phon_out_ix, phoneme_tab[plist->tone_ph], NULL);
				}
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
		if((letter >= 0xc0) && (letter < N_REMOVE_ACCENT))
			return(tr->letter_bits[remove_accent[letter-0xc0]] & (1L << group));
	}

	if((letter >= 0) && (letter < 0x80))
		return(tr->letter_bits[letter] & (1L << group));

	return(0);
}


int IsVowel(Translator *tr, int letter)
{//====================================
	return(IsLetter(tr, letter, LETTERGP_VOWEL2));
}




static int Unpronouncable2(Translator *tr, char *word)
{//===================================================
	int  c;
	int end_flags;
	char ph_buf[N_WORD_PHONEMES];

	ph_buf[0] = 0;
	c = word[-1];
	word[-1] = ' ';   // ensure there is a space before the "word"
	end_flags = TranslateRules(tr, word, ph_buf, sizeof(ph_buf), NULL, FLAG_UNPRON_TEST, NULL);
	word[-1] = c;
	if((end_flags == 0) || (end_flags & SUFX_UNPRON))
		return(1);
	return(0);
}


int Unpronouncable(Translator *tr, char *word, int posn)
{//=====================================================
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

	utf8_in(&c,word);
	if((tr->letter_bits_offset > 0) && (c < 0x241))
	{
		// Latin characters for a language with a non-latin alphabet
		return(0);  // so we can re-translate the word as English
	}

	if(tr->langopts.param[LOPT_UNPRONOUNCABLE] == 1)
		return(0);

	if(((c = *word) == ' ') || (c == 0) || (c == '\''))
		return(0);

	index = 0;
	count = 0;
	for(;;)
	{
		index += utf8_in(&c,&word[index]);
		if((c==0) || (c==' '))
			break;

		if((c=='\'') && ((count > 1) || (posn > 0)))
			break;   // "tv'" but not "l'"

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
			return(0);
	}

	if((vowel_posn > 2) && (tr->langopts.param[LOPT_UNPRONOUNCABLE] == 2))
	{
		// Lookup unpronounable rules in *_rules
		return(Unpronouncable2(tr, word));
	}

	if(c1 == tr->langopts.param[LOPT_UNPRONOUNCABLE])
		vowel_posn--;   // disregard this as the initial letter when counting

	if(vowel_posn > (tr->langopts.max_initial_consonants+1))
		return(1);  // no vowel, or no vowel in first few letters

return(0);

}   /* end of Unpronounceable */



//=============================================================================================
//   Determine the stress pattern of a word
//
//=============================================================================================



static int GetVowelStress(Translator *tr, unsigned char *phonemes, signed char *vowel_stress, int &vowel_count, int &stressed_syllable, int control)
{//=================================================================================================================================================
// control = 1, set stress to 1 for forced unstressed vowels
	unsigned char phcode;
	PHONEME_TAB *ph;
	unsigned char *ph_out = phonemes;
	int count = 1;
	int max_stress = -1;
	int ix;
	int j;
	int stress = -1;
	int primary_posn = 0;

	vowel_stress[0] = 1;
	while(((phcode = *phonemes++) != 0) && (count < (N_WORD_PHONEMES/2)-1))
	{
		if((ph = phoneme_tab[phcode]) == NULL)
			continue;

		if((ph->type == phSTRESS) && (ph->program == 0))
		{
			/* stress marker, use this for the following vowel */

			if(phcode == phonSTRESS_PREV)
			{
				/* primary stress on preceeding vowel */
				j = count - 1;
				while((j > 0) && (stressed_syllable == 0) && (vowel_stress[j] < 4))
				{
					if((vowel_stress[j] != 0) && (vowel_stress[j] != 1))
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

			if((stress < 0) && (control & 1) && (ph->phflags & phUNSTRESSED))
				vowel_stress[count] = 1;   /* weak vowel, must be unstressed */

			count++;
			stress = -1;
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
	vowel_stress[count] = 1;
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
					vowel_stress[ix] = 1;
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



static char stress_phonemes[] = {phonSTRESS_D, phonSTRESS_U, phonSTRESS_2, phonSTRESS_3,
		phonSTRESS_P, phonSTRESS_P2, phonSTRESS_TONIC};


void ChangeWordStress(Translator *tr, char *word, int new_stress)
{//==============================================================
	int ix;
	unsigned char *p;
	int  max_stress;
	int  vowel_count;              // num of vowels + 1
	int  stressed_syllable=0;      // position of stressed syllable
	unsigned char phonetic[N_WORD_PHONEMES];
	signed char vowel_stress[N_WORD_PHONEMES/2];

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
			if((vowel_stress[ix] == 0) || (vowel_stress[ix] > 1))
				*word++ = stress_phonemes[(unsigned char)vowel_stress[ix]];

			ix++;
		}
		*word++ = *p++;
	}
	*word = 0;
}  // end of ChangeWordStress



void SetWordStress(Translator *tr, char *output, unsigned int *dictionary_flags, int tonic, int control)
{//=====================================================================================================
/* Guess stress pattern of word.  This is language specific

   'output' is used for input and output

   'dictionary_flags' has bits 0-3   position of stressed vowel (if > 0)
                                     or unstressed (if == 7) or syllables 1 and 2 (if == 6)
                          bits 8...  dictionary flags

   If 'tonic' is set (>= 0), replace highest stress by this value.

  control:  bit 0   This is an individual symbol, not a word
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
	int dflags = 0;
	int first_primary;

	signed char vowel_stress[N_WORD_PHONEMES/2];
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

	if(dictionary_flags != NULL)
		dflags = dictionary_flags[0];

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
	stressed_syllable = dflags & 0x7;
	if(dflags & 0x8)
	{
		// this indicates a word without a primary stress
		stressed_syllable = dflags & 0x3;
		unstressed_word = 1;
	}

	max_stress = GetVowelStress(tr, phonetic, vowel_stress, vowel_count, stressed_syllable, 1);
	if((max_stress < 0) && dictionary_flags)
	{
		if((tr->langopts.stress_flags & 1) && (vowel_count == 2))
		{
			// lang=fr: don't stress monosyllables except at end-of-clause
			vowel_stress[1] = 0;
			dictionary_flags[0] |= FLAG_STRESS_END2;
		}
		max_stress = 0;
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
		// stress on first syllable, unless it is a light syllable followed by a heavy syllable
		if((syllable_weight[1] > 0) || (syllable_weight[2] == 0))
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

	case 10:  // penultimate, but final if only 1 or 2 syllables
		if(stressed_syllable == 0)
		{
			if(vowel_count < 4)
			{
				vowel_stress[vowel_count - 1] = 4;
				max_stress = 4;
				break;
			}
		}
		// drop through to next case
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

				if((vowel_stress[stressed_syllable] == 0) || (vowel_stress[stressed_syllable] == 1))
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
			if(vowel_stress[stressed_syllable] < 0)
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

			while(stressed_syllable > 0)
			{
				// find the last vowel which is not unstressed
				if(vowel_stress[stressed_syllable] < 0)
				{
					vowel_stress[stressed_syllable] = 4;
					break;
				}
				else
					stressed_syllable--;
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
				if(vowel_stress[ix] < 0)
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

	case 7:  // LANG=tr, the last syllable for any vowel marked explicitly as unstressed
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
			if(vowel_stress[ix] < 0)
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

	if((stressflags & 0x2000) && (vowel_stress[1] < 0))
	{
		// If there is only one syllable before the primary stress, give it a secondary stress
		if((vowel_count > 2) && (vowel_stress[2] >= 4))
		{
			vowel_stress[1] = 3;
		}
	}

	done = 0;
	first_primary = 0;
	for(v=1; v<vowel_count; v++)
	{
		if(vowel_stress[v] < 0)
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

		if(vowel_stress[v] >= 4)
		{
			if(first_primary == 0)
				first_primary = v;
			else
			if(stressflags & S_FIRST_PRIMARY)
			{
				// reduce primary stresses after the first to secondary
				vowel_stress[v] = 3;
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

	if(!(control & 1) && ((ph = phoneme_tab[*p]) != NULL))
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
					v_stress = 0;
				}
				else
				if((stressflags & 2) || (v == 1) || (v == (vowel_count-1)))
				{
					// first or last syllable, or option 'don't set diminished stress'
					v_stress = 1;
				}
				else
				if((v == (vowel_count-2)) && (vowel_stress[vowel_count-1] <= 1))
				{
					// penultimate syllable, followed by an unstressed final syllable
					v_stress = 1;
				}
				else
				{
					// unstressed syllable within a word
					if((vowel_stress[v-1] < 0) || ((stressflags & 0x10000) == 0))
					{
						v_stress = 0;      /* change to 0 (diminished stress) */
						vowel_stress[v] = v_stress;
					}
				}
			}

			if((v_stress == 0) || (v_stress > 1))
				*output++ = stress_phonemes[v_stress];  // mark stress of all vowels except 1 (unstressed)


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

	return;
}  /* end of SetWordStress */




//=============================================================================================
//   Look up a word in the pronunciation rules
//
//=============================================================================================




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



static void MatchRule(Translator *tr, char *word[], char *word_start, int group_length, char *rule, MatchRecord *match_out, int word_flags, int dict_flags)
{//========================================================================================================================================================
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
	int  ix;

	int  match_type;      /* left, right, or consume */
	int  failed;
	int  unpron_ignore;
	int  consumed;        /* number of letters consumed from input */
	int  syllable_count;
	int  vowel;
	int  letter_group;
	int  distance_right;
	int  distance_left;
	int  lg_pts;
	int  n_bytes;
	int add_points;
	int command;
	int check_atstart;

	MatchRecord match;
	static MatchRecord best;

	int  total_consumed;  /* letters consumed for best match */

	unsigned char condition_num;
	char *common_phonemes;  /* common to a group of entries */
	char *group_chars;
	char word_buf[N_WORD_BYTES];

	group_chars = *word;

	if(rule == NULL)
	{
		match_out->points = 0;
		(*word)++;
		return;
	}


	total_consumed = 0;
	common_phonemes = NULL;
	match_type = 0;

	best.points = 0;
	best.phonemes = "";
	best.end_type = 0;
	best.del_fwd = NULL;

	/* search through dictionary rules */
	while(rule[0] != RULE_GROUP_END)
	{
		unpron_ignore = word_flags & FLAG_UNPRON_TEST;
		match_type=0;
		consumed = 0;
		letter = 0;
		distance_right= -6;   /* used to reduce points for matches further away the current letter */
		distance_left= -2;
		check_atstart = 0;

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
							if(rb == RULE_LINENUM)
								match.phonemes += 2;  // skip over line number
						}
					}
					else
					{
						match.phonemes = "";
					}
					rule--;      // so we are still pointing at the 0
					failed=2;    // matched OK
					break;

				case RULE_PRE_ATSTART:   // pre rule with implied 'start of word'
					check_atstart = 1;
					unpron_ignore = 0;
					match_type = RULE_PRE;
					break;

				case RULE_PRE:
					match_type = RULE_PRE;
					if(word_flags & FLAG_UNPRON_TEST)
					{
						// checking the start of the word for unpronouncable character sequences, only
						// consider rules which explicitly match the start of a word
						// Note: Those rules now use RULE_PRE_ATSTART
						failed = 1;
					}
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
					if((letter & 0xc0) != 0x80)
						add_points = 21;    // don't add point for non-initial UTF-8 bytes
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
					if(IsLetter(tr, letter_w, 0) || ((letter_w == ' ') && (word_flags & FLAG_SUFFIX_VOWEL)))
					{
						failed = 1;
					}
					else
					{
						add_points = (20-distance_right);
						post_ptr += letter_xbytes;
					}
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

				case RULE_DOLLAR:
					command = *rule++;
					if(command == 0x01)
					{
						match.end_type = SUFX_UNPRON;    // $unpron
					}
					else
					if((command & 0xf0) == 0x10)
					{
						// $w_alt
						if(dict_flags & (1 << (BITNUM_FLAG_ALT + (command & 0xf))))
							add_points = 23;
						else
							failed = 1;
					}
					else
					if((command & 0xf0) == 0x20)
					{
						// $p_alt
						// make a copy of the word up to the post-match characters
						ix = *word - word_start + consumed + group_length + 1;
						memcpy(word_buf, word_start-1, ix);
						word_buf[ix] = ' ';
						word_buf[ix+1] = 0;

						if(LookupFlags(tr, &word_buf[1]) & (1 << (BITNUM_FLAG_ALT + (command & 0xf))))
							add_points = 23;
						else
							failed = 1;
					}
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
						int vowel_count=0;

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
								// this is counting vowels which are separated by non-vowel letters
								vowel_count++;
							}
							vowel = IsLetter(tr, letter_w,LETTERGP_VOWEL2);
							p += utf8_in(&letter_w,p);
						}
						if(syllable_count <= vowel_count)
							add_points = (18+syllable_count-distance_right);
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

				case RULE_SKIPCHARS:
					{
						// Used for lang=Tamil, used to match on the next word after an unknown word ending
						// only look until the end of the word (including the end-of-word marker)
						// Jx  means 'skip characters until x', where 'x' may be '_' for 'end of word'
						char *p = post_ptr + letter_xbytes;
						char *p2 = p;
						int rule_w;            // skip characters until this
						utf8_in(&rule_w,rule);
						while((letter_w != rule_w) && (letter_w != RULE_SPACE))
						{
							p2 = p;
							p += utf8_in(&letter_w,p);
						}
						if(letter_w == rule_w)
						{
							post_ptr = p2;
						}
					}
					break;

				case RULE_INC_SCORE:
					add_points = 20;      // force an increase in points
					break;

				case RULE_DEL_FWD:
					// find the next 'e' in the word and replace by 'E'
					for(p = *word + group_length; p < post_ptr; p++)
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
						if((letter & 0xc0) != 0x80)
						{
							// not for non-initial UTF-8 bytes
							add_points = (21-distance_right);
						}
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
						add_points = (18+syllable_count-distance_left);
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
						{
							if((letter & 0xc0) != 0x80)
							{
								// not for non-initial UTF-8 bytes
								add_points = (21-distance_left);
							}
						}
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

		if((failed == 2) && (unpron_ignore == 0))
		{
			// do we also need to check for 'start of word' ?
			if((check_atstart==0) || (pre_ptr[-1] == ' '))
			{
				if(check_atstart)
					match.points += 4;

				/* matched OK, is this better than the last best match ? */
				if(match.points >= best.points)
				{
					memcpy(&best,&match,sizeof(match));
					total_consumed = consumed;
				}
	
				if((option_phonemes == 2) && (match.points > 0) && ((word_flags & FLAG_NO_TRACE) == 0))
				{
					// show each rule that matches, and it's points score
					int pts;
					char decoded_phonemes[80];
	
					pts = match.points;
					if(group_length > 1)
						pts += 35;    // to account for an extra letter matching
					DecodePhonemes(match.phonemes,decoded_phonemes);
					fprintf(f_trans,"%3d\t%s [%s]\n",pts,DecodeRule(group_chars, group_length, rule_start, word_flags), decoded_phonemes);
				}
			}
		}

		/* skip phoneme string to reach start of next template */
		while(*rule++ != 0);
	}

	if((option_phonemes == 2) && ((word_flags & FLAG_NO_TRACE)==0))
	{
		if(group_length <= 1)
			fprintf(f_trans,"\n");
	}

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
	unsigned int  c12, c123;
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


	if((option_phonemes == 2) && ((word_flags & FLAG_NO_TRACE)==0))
	{
		char wordbuf[120];
		int  ix;

		for(ix=0; ((c = p_start[ix]) != ' ') && (c != 0); ix++)
		{
			wordbuf[ix] = c;
		}
		wordbuf[ix] = 0;
		if(word_flags & FLAG_UNPRON_TEST)
			fprintf(f_trans,"Unpronouncable? '%s'\n",wordbuf);
		else
			fprintf(f_trans,"Translate '%s'\n",wordbuf);
	}

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

			if(((ix = wc - tr->letter_bits_offset) >= 0) && (ix < 128))
			{
				if(tr->groups3[ix] != NULL)
				{
					MatchRule(tr, &p, p_start, wc_bytes, tr->groups3[ix], &match1, word_flags, dict_flags0);
					found = 1;
				}
			}

			if(!found && (n > 0))
			{
				/* there are some 2 byte chains for this initial letter */
				c2 = p[1];
				c12 = c + (c2 << 8);   /* 2 characters */
				c123 = c12 + (p[2] << 16);
	
				g1 = tr->groups2_start[c];
				for(g=g1; g < (g1+n); g++)
				{
					if(tr->groups2_name[g] == c12)
					{
						found = 1;

						p2 = p;
						MatchRule(tr, &p2, p_start, 2, tr->groups2[g], &match2, word_flags, dict_flags0);
						if(match2.points > 0)
							match2.points += 35;   /* to acount for 2 letters matching */

						/* now see whether single letter chain gives a better match ? */
						MatchRule(tr, &p, p_start, 1, tr->groups1[c], &match1, word_flags, dict_flags0);

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
				if(tr->groups1[c] != NULL)
					MatchRule(tr, &p, p_start, 1, tr->groups1[c], &match1, word_flags, dict_flags0);
				else
				{
					// no group for this letter, use default group
					MatchRule(tr, &p, p_start, 0, tr->groups1[0], &match1, word_flags, dict_flags0);

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
						if(letter == 0xe000+'(')
						{
							if(pre_pause < tr->langopts.param2[LOPT_BRACKET_PAUSE])
								pre_pause = tr->langopts.param2[LOPT_BRACKET_PAUSE];  // a bracket, aleady spoken by AnnouncePunctuation()
						}
						if(IsBracket(letter))
						{
							if(pre_pause < tr->langopts.param[LOPT_BRACKET_PAUSE])
								pre_pause = tr->langopts.param[LOPT_BRACKET_PAUSE];
						}

						// no match, try removing the accent and re-translating the word
						if((letter >= 0xc0) && (letter < N_REMOVE_ACCENT) && ((ix = remove_accent[letter-0xc0]) != 0))
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
						LookupLetter(tr, wc, -1, ph_buf, 0);
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
			if(word_flags & FLAG_UNPRON_TEST)
				return(match1.end_type | 1);

			if((match1.phonemes[0] == phonSWITCH) && ((word_flags & FLAG_DONT_SWITCH_TRANSLATOR)==0))
			{
				// an instruction to switch language, return immediately so we can re-translate
				strcpy(phonemes,match1.phonemes);
				return(0);
			}

			match1.end_type &= ~SUFX_UNPRON;

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

	if(tr->langopts.param[LOPT_ALT] & 2)
	{
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
	}
}  // end of ApplySpecialAttribute2


void ApplySpecialAttribute(Translator *tr, char *phonemes, int dict_flags)
{//=======================================================================
// Amend the translated phonemes according to an attribute which is specific for the language.
	int len;
	char *p_end;

	if((dict_flags & (FLAG_ALT_TRANS | FLAG_ALT2_TRANS)) == 0)
		return;

	len = strlen(phonemes);
	p_end = &phonemes[len-1];

	switch(tr->translator_name)
	{
#ifdef deleted
// this is now done in de_rules
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
#endif

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


int TransposeAlphabet(Translator *tr, char *text)
{//==============================================
// transpose cyrillic alphabet (for example) into ascii (single byte) character codes
// return: number of bytes, bit 6: 1=used compression
	int c;
	int c2;
	int ix;
	int offset;
	int min;
	int max;
	char *p = text;
	char *p2 = text;
	int all_alpha=1;
	int bits;
	int acc;
	int pairs_start;
	const short *pairs_list;

	offset = tr->transpose_min - 1;
	min = tr->transpose_min;
	max = tr->transpose_max;

	pairs_start = max - min + 2;

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
			if((pairs_list = tr->frequent_pairs) != NULL)
			{
				c2 = c + (*p << 8);
				for(ix=0; c2 >= pairs_list[ix]; ix++)
				{
					if(c2 == pairs_list[ix])
					{
						// found an encoding for a 2-character pair
						c = ix + pairs_start;   // 2-character codes start after the single letter codes
						p++;
						break;
					}
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
	if(tr->transpose_min > 0)
	{
		strcpy(word_buf,word);
		wlen = TransposeAlphabet(tr, word_buf);
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
		if(dictionary_flags & FLAG_NEEDS_DOT)
		{
			if(!(wflags & FLAG_HAS_DOT))
				continue;
		}

		if((dictionary_flags & FLAG_ATEND) && (word_end < tr->clause_end))
		{
			// only use this pronunciation if it's the last word of the clause
			continue;
		}

		if((dictionary_flags & FLAG_ATSTART) && !(wtab->flags & FLAG_FIRST_WORD))
		{
			// only use this pronunciation if it's the first word of a clause
			continue;
		}

		if((dictionary_flags2 & FLAG_SENTENCE) && !(tr->clause_terminator & CLAUSE_BIT_SENTENCE))
		{
			// only uis this clause is a sentence , i.e. terminator is {. ? !} not {, : :}
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
		if(dictionary_flags & FLAG_ALT2_TRANS)
		{
			// language specific
			if((tr->translator_name == L('h','u')) && !(tr->prev_dict_flags & FLAG_ALT_TRANS))
				continue;
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
				if((dictionary_skipwords) && (wtab != NULL))
				{
					// matched more than one word
					// (check for wtab prevents showing RULE_SPELLING byte when speaking individual letters)
					memcpy(word_buf,word2,word_end-word2);
					word_buf[word_end-word2-1] = 0;
					fprintf(f_trans,"Found: '%s %s",word1,word_buf);
				}
				else
				{
					fprintf(f_trans,"Found: '%s",word1);
				}
				fprintf(f_trans,"' [%s]  %s\n",ph_decoded,print_dictionary_flags(flags));
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

	for(length=0; length<(N_WORD_BYTES-1); length++)
	{
		if(((c = *word1++)==0) || (c == ' '))
			break;

		if((c=='.') && (length > 0) && (isdigit(word[length-1])))
			break;  // needed for lang=hu, eg. "december 2.-ig"

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
			else
			{
//				flags[0] &= ~FLAG_SKIPWORDS;  // check lang=hu  január 21.-ig  (error: suffix repeated ??)
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

int LookupFlags(Translator *tr, const char *word)
{//==============================================
	char buf[100];
	static unsigned int flags[2];

	flags[0] = flags[1] = 0;
	char *word1 = (char *)word;
	LookupDictList(tr, &word1, buf, flags, 0, NULL);
	return(flags[0]);
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
//		"c", "rs", "ir", "ur", "ath", "ns", "lu", NULL };
		"c", "rs", "ir", "ur", "ath", "ns", "u", NULL };

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
		if(tr->translator_name == L('e','n'))
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
		}
		else
		if(tr->langopts.suffix_add_e != 0)
		{
			end_flags |= FLAG_SUFX_E_ADDED;
		}

		if(end_flags & FLAG_SUFX_E_ADDED)
		{
			utf8_out(tr->langopts.suffix_add_e, &word_end[1]);

		if(option_phonemes == 2)
		{
			fprintf(f_trans,"add e\n");
		}
		}
	}

	if((end_type & SUFX_V) && (tr->expect_verb==0))
		tr->expect_verb = 1;         // this suffix indicates the verb pronunciation


	if((strcmp(ending,"s")==0) || (strcmp(ending,"es")==0))
		end_flags |= FLAG_SUFX_S;

//	if(strcmp(ending,"'s")==0)
	if(ending[0] == '\'')
		end_flags &= ~FLAG_SUFX;  // don't consider 's as an added suffix

	return(end_flags);
}   /* end of RemoveEnding */


