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
#include "voice.h"
#include "translate.h"




void Translator::LookupLetter(unsigned int letter, int next_byte, char *ph_buf1)
{//=============================================================================
	int len;
	unsigned char *p;
	static char single_letter[10] = {0,0};
	char ph_stress[2];
	char ph_buf3[30];

	if((letter <= 32) || iswspace(letter))
	{
		// lookup space as _&32 etc.
		sprintf(&single_letter[1],"_#%d ",letter);
		Lookup(&single_letter[1],ph_buf1);
		return;
	}


	len = utf8_out(letter,&single_letter[2]);
	single_letter[len+2] = ' ';

	if(next_byte != ' ')
		next_byte = RULE_SPELLING;
	single_letter[3+len] = next_byte;   // follow by space-space if the end of the word, or space-0x31

	single_letter[1] = '_';
	if(Lookup(&single_letter[1],ph_buf3) == 0)
	{
		single_letter[1] = ' ';
		if(Lookup(&single_letter[2],ph_buf3) == 0)
		{
			TranslateRules(&single_letter[2], ph_buf3, sizeof(ph_buf3), NULL,0,0);
		}
	}

	if(ph_buf3[0] == 0)
	{
		ph_buf1[0] = 0;
		return;
	}

	// at a stress marker at the start of the letter name, unless one is already marked
	ph_stress[0] = phonSTRESS_P;
	ph_stress[1] = 0;

	for(p=(unsigned char *)ph_buf3; *p != 0; p++)
	{
		if(phoneme_tab[*p]->type == phSTRESS)
			ph_stress[0] = 0;  // stress is already marked
	}
	sprintf(ph_buf1,"%s%s",ph_stress,ph_buf3);
}



int Translator::TranslateLetter(char *word, char *phonemes, int control)
{//=====================================================================
// get pronunciation for an isolated letter
// return number of bytes used by the letter
// control 2=say-as glyphs, 3-say-as chars
	int n_bytes;
	int letter;
	int len;
	int phoneme_tab_en;
	char *p2;
	char *pbuf;
	char capital[20];
	char ph_buf[60];
	char ph_buf2[60];
	char ph_buf_en[60];
	char hexbuf[6];
	static char single_letter[10] = {0,0};

	ph_buf[0] = 0;
	capital[0] = 0;

	n_bytes = utf8_in(&letter,word,0);

	if((letter & 0xfff00) == 0x0e000)
	{
		letter &= 0xff;   // uncode private usage area
	}

	if(control > 2)
	{
		// include CAPITAL information
		if(iswupper(letter))
		{
			Lookup("_cap",capital);
		}
	}
	letter = towlower(letter);

	LookupLetter(letter, word[n_bytes], ph_buf);

	if(ph_buf[0] == phonSWITCH)
	{
		strcpy(phonemes,ph_buf);
		return(0);
	}

	if(ph_buf[0] == 0)
	{
		phoneme_tab_en = SetTranslator2("en");
		translator2->LookupLetter(letter, word[n_bytes], ph_buf_en);
		if(ph_buf_en[0] != 0)
		{
			sprintf(ph_buf,"%c%c%s%c%c",phonSWITCH2, phoneme_tab_en + phonTOP, ph_buf_en, phonSWITCH2, voice->phoneme_tab_ix + phonTOP);
		}
		SelectPhonemeTable(voice->phoneme_tab_ix);  // revert to original phoneme table
	}

	if(ph_buf[0] == 0)
	{
		// character name not found
		if(iswalpha(letter))
			Lookup("_?A",ph_buf);

		if((ph_buf[0]==0) && !iswspace(letter))
			Lookup("_??",ph_buf);

		if((control==4) && (ph_buf[0] != 0))
		{
			// speak the hexadecimal number of the character code
			sprintf(hexbuf,"%x",letter);
			pbuf = ph_buf;
			for(p2 = hexbuf; *p2 != 0; p2++)
			{
				pbuf += strlen(pbuf);
				LookupLetter(*p2, 0, pbuf);
			}
		}
	}

	len = strlen(phonemes);
	sprintf(ph_buf2,"%c%s%s",0xff,capital,ph_buf);  // the 0xff marker will be removed or replaced in SetSpellingStress()
	if((len + strlen(ph_buf2)) < N_WORD_PHONEMES)
	{
		strcpy(&phonemes[len],ph_buf2);
	}
	return(n_bytes);
}  // end of TranslateLetter



void Translator::SetSpellingStress(char *phonemes, int control, int n_chars)
{//=========================================================================
// Individual letter names, reduce the stress of some.
	int ix;
	unsigned int c;
	int n_stress=0;
	int count;
	unsigned char buf[N_WORD_PHONEMES];

	for(ix=0; (c = phonemes[ix]) != 0; ix++)
	{
		if(c == phonSTRESS_P)
		{
			n_stress++;
		}
		buf[ix] = c;
	}
	buf[ix] = 0;

	count = 0;
	for(ix=0; (c = buf[ix]) != 0; ix++)
	{
		if((c == phonSTRESS_P) && (n_chars > 1))
		{
			count++;

			if(langopts.spelling_stress == 1)
			{
				// stress on initial letter when spelling
				if(count > 1)
					c = phonSTRESS_3;
			}
			else
			{
				if(count != n_stress)
				{
					if(((count % 3) != 0) || (count == n_stress-1))
						c = phonSTRESS_3;   // reduce to secondary stress
				}
			}
		}
		else
		if(c == 0xff)
		{
			if((control < 2) || (ix==0))
				continue;   // don't insert pauses

			if(control == 4)
				c = phonPAUSE;    // pause after each character
			if(((count % 3) == 0) || (control == 4))
				c = phonPAUSE_SHORT;  // pause following a primary stress
			else
				continue;       // remove marker
		}
		*phonemes++ = c;
	}
	if(control >= 2)
		*phonemes++ = phonPAUSE_NOLINK;
	*phonemes = 0;
}  // end of SetSpellingStress




int Translator::TranslateRoman(char *word, char *ph_out)
{//=====================================================
	int c;
	char *p;
	const char *p2;
	int acc;
	int prev;
	int value;
	int subtract;
	int repeat = 0;
	unsigned int flags;
	char number_chars[N_WORD_BYTES];

	static const char *roman_numbers = "ixcmvld";
	static int roman_values[] = {1,10,100,1000,5,50,500};
 
	acc = 0;
	prev = 0;
	subtract = 0x7fff;

	while((c = *word++) != ' ')
	{
		if((p2 = strchr(roman_numbers,c)) == NULL)
			return(0);

		value = roman_values[p2 - roman_numbers];
		if(value == prev)
		{
			repeat++;
			if(repeat >= 3)
				return(0);
		}
		else
			repeat = 0;

		if((prev==5) || (prev==50) || (prev==500))
		{
			if(value >= prev)
				return(0);
		}
		if((prev != 0) && (prev < value))
		{
			if(((acc % 10) != 0) || ((prev*10) < value))
				return(0);
			subtract = prev;
			value -= subtract;
		}
		else
		if(value >= subtract)
			return(0);
		else
			acc += prev;
		prev = value;
	}
	acc += prev;
	if(acc < 2)
		return(0);

	if(acc > langopts.max_roman)
		return(0);

	Lookup("_roman",ph_out);   // precede by "roman" if _rom is defined in *_list
	p = &ph_out[strlen(ph_out)];

	sprintf(number_chars," %d ",acc);
	TranslateNumber(&number_chars[1],p,&flags,0);
	return(1);
}  // end of TranslateRoman


int Translator::LookupNum2(int value, int control, char *ph_out)
{//=============================================================
// Lookup a 2 digit number
// control bit 0: use special form of '1'
// control bit 2: use feminine form of '2'

	int found;
	int ix;
	int units;
	int used_and=0;
	int next_phtype;
	char string[12];  // for looking up entries in de_list
	char ph_tens[50];
	char ph_digits[50];
	char ph_and[12];

	if((value == 1) && (control & 1))
	{
		if(Lookup("_1a",ph_out) != 0)
			return(0);
	}
	// is there a special pronunciation for this 2-digit number
	found = 0;
	if(control & 4)
	{
		sprintf(string,"_%df",value);
		found = Lookup(string,ph_digits);
	}
	if(found == 0)
	{
		sprintf(string,"_%d",value);
		found = Lookup(string,ph_digits);
	}

	// no, speak as tens+units
	if((control & 2) && (value < 10))
	{
		// speak leading zero
		Lookup("_0",ph_tens);
	}
	else
	{
		if(found)
		{
			strcpy(ph_out,ph_digits);
			return(0);
		}

		if((value % 10) == 0)
		{
			sprintf(string,"_%d0",value / 10);
			found = Lookup(string,ph_tens);
		}
		if(!found)
		{
			sprintf(string,"_%dX",value / 10);
			Lookup(string,ph_tens);
		}

		if((value % 10) == 0)
		{
			strcpy(ph_out,ph_tens);
			return(0);
		}

		found = 0;
		units = (value % 10);
		if(control & 4)
		{
			// is there a variant form of this number?
			sprintf(string,"_%df",units);
			found = Lookup(string,ph_digits);
		}
		if(found == 0)
		{
			sprintf(string,"_%d",units);
			Lookup(string,ph_digits);
		}
	}

	if(langopts.numbers & 0x30)
	{
		Lookup("_0and",ph_and);
		if(langopts.numbers & 0x10)
			sprintf(ph_out,"%s%s%s",ph_digits,ph_and,ph_tens);
		else
			sprintf(ph_out,"%s%s%s",ph_tens,ph_and,ph_digits);
		used_and = 1;
	}
	else
	{
		if(langopts.numbers & 0x200)
		{
			// remove vowel from the end of tens if units starts with a vowel (LANG=Italian)
			if((ix = strlen(ph_tens)-1) >= 0)
			{
				if((next_phtype = phoneme_tab[(unsigned int)(ph_digits[0])]->type) == phSTRESS)
					next_phtype = phoneme_tab[(unsigned int)(ph_digits[1])]->type;
	
				if((phoneme_tab[(unsigned int)(ph_tens[ix])]->type == phVOWEL) && (next_phtype == phVOWEL))
					ph_tens[ix] = 0;
			}
		}
		sprintf(ph_out,"%s%s",ph_tens,ph_digits);
	}

	if(langopts.numbers & 0x100)
	{
		// only one primary stress
		found = 0;
		for(ix=strlen(ph_out)-1; ix>=0; ix--)
		{
			if(ph_out[ix] == phonSTRESS_P)
			{
				if(found)
					ph_out[ix] = phonSTRESS_3;
				else
					found = 1;
			}
		}
	}
	return(used_and);
}  // end of LookupNum2


int Translator::LookupNum3(int value, char *ph_out, int suppress_null, int thousandplex, int prev_thousands)
{//=========================================================================================================
// Translate a 3 digit number
	int found;
	int hundreds;
	int x;
	char string[12];  // for looking up entries in **_list
	char buf1[100];
	char buf2[100];
	char ph_100[20];
	char ph_10T[20];
	char ph_digits[50];
	char ph_thousands[50];
	char ph_hundred_and[12];
	char ph_thousand_and[12];
	
	hundreds = value / 100;
	buf1[0] = 0;

	if(hundreds > 0)
	{
		ph_thousands[0] = 0;
		ph_thousand_and[0] = 0;

		Lookup("_0C",ph_100);

		if((hundreds >= 10) && ((langopts.numbers & 0x0800) || (hundreds != 19)))
		{
			ph_digits[0] = 0;

			if(LookupThousands(hundreds / 10, thousandplex+1, ph_10T) == 0)
			{
				x = 0;
				if(langopts.numbers2 & (1 << (thousandplex+1)))
					x = 4;
				LookupNum2(hundreds/10, x, ph_digits);
			}

			sprintf(ph_thousands,"%s%s%c",ph_digits,ph_10T,phonPAUSE_NOLINK);
			hundreds %= 10;
			if(hundreds == 0)
				ph_100[0] = 0;
			suppress_null = 1;
		}

		ph_digits[0] = 0;
		if(hundreds > 0)
		{
			if((langopts.numbers & 0x100000) && (prev_thousands || (ph_thousands[0] != 0)))
			{
				Lookup("_0and",ph_thousand_and);
			}

			suppress_null = 1;

			found = 0;
			if((value % 1000) == 100)
			{
				// is there a special pronunciation for exactly 100 ?
				found = Lookup("_1C0",ph_digits);
			}
			if(!found)
			{
				sprintf(string,"_%dC",hundreds);
				found = Lookup(string,ph_digits);  // is there a specific pronunciation for n-hundred ?
			}

			if(found)
			{
				ph_100[0] = 0;
			}
			else
			{
				if((hundreds > 1) || ((langopts.numbers & 0x400) == 0))
				{
					LookupNum2(hundreds,0,ph_digits);
				}
			}
		}

		sprintf(buf1,"%s%s%s%s",ph_thousands,ph_thousand_and,ph_digits,ph_100);
	}

	ph_hundred_and[0] = 0;
	if((langopts.numbers & 0x40) && ((value % 100) != 0))
	{
		if((value > 100) || (prev_thousands && (thousandplex==0)))
		{
			Lookup("_0and",ph_hundred_and);
		}
	}


	buf2[0] = 0;
	value = value % 100;

	if(value == 0)
	{
		if(suppress_null == 0)
			Lookup("_0",buf2);
	}
	else
	{
		x = 0;
		if(thousandplex==0)
			x = 1;   // allow "eins" for 1 rather than "ein"
		else
		{
			if(langopts.numbers2 & (1 << thousandplex))
				x = 4;   // use variant (feminine) for before thousands and millions
		}

		if(LookupNum2(value,x,buf2) != 0)
		{
			if(langopts.numbers & 0x80)
				ph_hundred_and[0] = 0;  // don't put 'and' after 'hundred' if there's 'and' between tens and units
		}
	}

	sprintf(ph_out,"%s%s%s",buf1,ph_hundred_and,buf2);

	return(0);
}  // end of LookupNum3



static const char *M_Variant(int value)
{//====================================
	// returns M, or perhaps MA for some cases
	
	if(((value % 100)>20) || ((value % 100)<10))   // but not teens, 10 to 19
	{
		if ((translator->langopts.numbers2 & 0x40) &&
			((value % 10)>=2) &&
			((value % 10)<=4))
		{
		// for Polish language - two forms of plural!
			return("0MA");
		}

		if((translator->langopts.numbers2 & 0x80) &&
			((value % 10)==1))
		{
			return("1MA");
		}

	}
	return("0M");
}


int Translator::LookupThousands(int value, int thousandplex, char *ph_out)
{//=======================================================================
	int found;
	char string[12];
	char ph_of[12];
	char ph_thousands[40];

	ph_of[0] = 0;

	// first look fora match with the exact value of thousands
	sprintf(string,"_%dM%d",value,thousandplex);

	if((found = Lookup(string,ph_thousands)) == 0)
	{
		if((value % 100) >= 20) 
		{
			Lookup("_0of",ph_of);
		}

		sprintf(string,"_%s%d",M_Variant(value),thousandplex);

		if(Lookup(string,ph_thousands) == 0)
		{
			// repeat "thousand" if higher order names are not available
			sprintf(string,"_%dM1",value);
			if((found = Lookup(string,ph_thousands)) == 0)
				Lookup("_0M1",ph_thousands);
		}
	}
	sprintf(ph_out,"%s%s",ph_of,ph_thousands);
	return(found);
}


int Translator::TranslateNumber_1(char *word, char *ph_out, unsigned int *flags, int wflags)
{//=========================================================================================
//  Number translation with various options
// the "word" may be up to 4 digits
// "words" of 3 digits may be preceded by another number "word" for thousands or millions

	int n_digits;
	int value;
	int ix;
	unsigned char c;
	int suppress_null = 0;
	int decimal_point = 0;
	int thousandplex = 0;
	int thousands_inc = 0;
	int prev_thousands = 0;
	int this_value;
	static int prev_value;
	int decimal_count;
	int max_decimal_count;
	char string[12];  // for looking up entries in de_list
	char buf1[100];
	char ph_append[50];
	char ph_buf[200];
	char ph_buf2[50];

	static const char str_pause[2] = {phonPAUSE_NOLINK,0};

	for(ix=0; isdigit(word[ix]); ix++) ;
	n_digits = ix;
	value = this_value = atoi(word);

	ph_append[0] = 0;
	ph_buf2[0] = 0;

	// is there a previous thousands part (as a previous "word") ?
	if((n_digits == 3) && (word[-2] == langopts.thousands_sep) && isdigit(word[-3]))
	{
		prev_thousands = 1;
	}
	else
	if((langopts.thousands_sep == ' ') || (langopts.numbers & 0x1000))
	{
		// thousands groups can be separated by spaces
		if((n_digits == 3) && isdigit(word[-2]))
		{
			prev_thousands = 1;
		}
	}

	if((word[0] == '0') && (prev_thousands == 0) && (word[1] != langopts.decimal_sep))
	{
		if((n_digits == 2) && (word[3] == ':') && isdigit(word[5]) && isspace(word[7]))
		{
			// looks like a time 02:30, omit the leading zero
		}
		else
		{
			return(0);     // number string with leading zero, speak as individual digits
		}
	}

	if((langopts.numbers & 0x1000) && (word[n_digits] == ' '))
		thousands_inc = 1;
	else
	if(word[n_digits] == langopts.thousands_sep)
		thousands_inc = 2;

	if(thousands_inc > 0)
	{
		// if the following "words" are three-digit groups, count them and add
		// a "thousand"/"million" suffix to this one

		ix = n_digits + thousands_inc;
		while(isdigit(word[ix]) && isdigit(word[ix+1]) && isdigit(word[ix+2]))
		{
			thousandplex++;
			if(word[ix+3] == langopts.thousands_sep)
				ix += (3 + thousands_inc);
			else
				break;
		}
	}

	if((value == 0) && prev_thousands)
	{
		suppress_null = 1;
	}

	if((word[n_digits] == langopts.decimal_sep) && isdigit(word[n_digits+1]))
	{
		// this "word" ends with a decimal point
		Lookup("_dpt",ph_append);
		decimal_point = 1;
	}
	else
	if(suppress_null == 0)
	{
		if(thousands_inc > 0)
		{
			if((thousandplex > 0) && (value < 1000))
			{
				if(langopts.numbers2 & 0x100)
				{
					if((thousandplex == 1) && (value >= 100))
					{
						// special word for 100,000's
						char ph_buf3[20];
						sprintf(string,"_%dL",value / 100);
						if(Lookup(string,ph_buf2) == 0)
						{
							LookupNum2(value/100,0,ph_buf2);
							Lookup("_0L",ph_buf3);
							strcat(ph_buf2,ph_buf3);
						}
						value %= 100;
						if(value == 0)
							suppress_null = 1;
					}
				}
				if((suppress_null == 0) && (LookupThousands(value,thousandplex,ph_append)))
				{
					// found an exact match for N thousand
					value = 0;
					suppress_null = 1;
				}
			}
		}
	}
	else
	if((thousandplex > 1) && prev_thousands && (prev_value > 0))
	{
		sprintf(string,"_%s%d",M_Variant(value),thousandplex+1);
		if(Lookup(string,buf1)==0)
		{
			// speak this thousandplex if there was no word for the previous thousandplex
			sprintf(string,"_0M%d",thousandplex);
			Lookup(string,ph_append);
		}
	}

	if((ph_append[0] == 0) && (word[n_digits] == '.') && (thousandplex == 0))
	{
		Lookup("_.",ph_append);
	}

	LookupNum3(value, ph_buf, suppress_null, thousandplex, prev_thousands);
	sprintf(ph_out,"%s%s%s",ph_buf2,ph_buf,ph_append);


	while(decimal_point)
	{
		n_digits++;

		decimal_count = 0;
		while(isdigit(word[n_digits+decimal_count]))
			decimal_count++;

		if(decimal_count > 1)
		{
			max_decimal_count = 2;
			switch(langopts.numbers & 0xe000)
			{
			case 0x8000:
				max_decimal_count = 5;
			case 0x4000:
				// French/Polish decimal fraction
				while(word[n_digits] == '0')
				{
					Lookup("_0",buf1);
					strcat(ph_out,buf1);
					decimal_count--;
					n_digits++;
				}
				if(decimal_count <= max_decimal_count)
				{
					LookupNum3(atoi(&word[n_digits]),buf1,0,0,0);
					strcat(ph_out,buf1);
					n_digits += decimal_count;
				}
				break;

			case 0x2000:
				// Italian decimal fractions
				if((decimal_count < 4) || ((decimal_count==4) && (word[n_digits] != '0')))
				{
					LookupNum3(atoi(&word[n_digits]),buf1,0,0,0);
					strcat(ph_out,buf1);
					if(word[n_digits]=='0')
					{
						// decimal part has leading zeros, so add a "hundredths" or "thousandths" suffix
						sprintf(string,"_0Z%d",decimal_count);
						Lookup(string,buf1);
						strcat(ph_out,buf1);
					}
					n_digits += decimal_count;
				}
				break;

			case 0x6000:
				// Romanian decimal fractions
				if((decimal_count <= 4) && (word[n_digits] != '0'))
				{
						LookupNum3(atoi(&word[n_digits]),buf1,0,0,0);
						strcat(ph_out,buf1);
						n_digits += decimal_count;
				}
				break;
			}
		}

		while(isdigit(c = word[n_digits]) && (strlen(ph_out) < (N_WORD_PHONEMES - 10)))
		{
			value = word[n_digits++] - '0';
			LookupNum2(value, 1, buf1);
			strcat(ph_out,buf1);
		}

		// something after the decimal part ?
		if(Lookup("_dpt2",buf1))
			strcat(ph_out,buf1);

		if(c == langopts.decimal_sep)
		{
			Lookup("_dpt",buf1);
			strcat(ph_out,buf1);
		}
		else
		{
			decimal_point = 0;
		}
	}
	if((ph_out[0] != 0) && (ph_out[0] != phonSWITCH))
	{
		int next_char;
		char *p;
		p = &word[n_digits+1];

		p += utf8_in(&next_char,p,0);
		if((langopts.numbers & NUM_NOPAUSE) && (next_char == ' '))
			utf8_in(&next_char,p,0);

		if(!iswalpha(next_char))
			strcat(ph_out,str_pause);  // don't add pause for 100s,  6th, etc.
	}

	*flags = FLAG_FOUND;
	prev_value = this_value;
	return(1);
}  // end of TranslateNumber_1



int Translator::TranslateNumber(char *word1, char *ph_out, unsigned int *flags, int wflags)
{//=======================================================================================
	if(option_sayas == SAYAS_DIGITS1)
		return(0);  // speak digits individually

	if((langopts.numbers & 0x3) == 1)
		return(TranslateNumber_1(word1,ph_out,flags,wflags));

	return(0);
}  // end of TranslateNumber

