/***************************************************************************
 *   Copyright (C) 2006 to 2007 by Jonathan Duddington                     *
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

#include "wx/wx.h"
#include <wx/dirdlg.h>
#include "wx/filename.h"
#include "wx/sound.h"
#include "wx/dir.h"

#include <sys/stat.h>

#include "speak_lib.h"
#include "main.h"
#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "spect.h"
#include "translate.h"
#include "options.h"

extern char word_phonemes[N_WORD_PHONEMES];    // a word translated into phoneme codes

//******************************************************************************************************


FILE *f_wavtest = NULL;
FILE *f_events = NULL;

FILE *OpenWaveFile3(const char *path)
/***********************************/
{
	int *p;
	FILE *f;

	static unsigned char wave_hdr[44] = {
		'R','I','F','F',0,0,0,0,'W','A','V','E','f','m','t',' ',
		0x10,0,0,0,1,0,1,0,  9,0x3d,0,0,0x12,0x7a,0,0,
		2,0,0x10,0,'d','a','t','a',  0,0,0,0 };


	if(path == NULL)
		return(NULL);

	// set the sample rate in the header
	p = (int *)(&wave_hdr[24]);
	p[0] = samplerate;
	p[1] = samplerate * 2;

	f = fopen(path,"wb");

	if(f != NULL)
	{
		fwrite(wave_hdr,1,sizeof(wave_hdr),f);
	}
	return(f);
}   //  end of OpenWaveFile




void CloseWaveFile3(FILE *f)
/*************************/
{
   unsigned int pos;
   static int value;

	if(f == NULL)
		return;

   fflush(f);
   pos = ftell(f);

   value = pos - 8;
   fseek(f,4,SEEK_SET);
   fwrite(&value,4,1,f);

	value = samplerate;
	fseek(f,24,SEEK_SET);
	fwrite(&value,4,1,f);

	value = samplerate*2;
	fseek(f,28,SEEK_SET);
	fwrite(&value,4,1,f);

   value = pos - 44;
   fseek(f,40,SEEK_SET);
   fwrite(&value,4,1,f);

   fclose(f);

} // end of CloseWaveFile3


int TestUriCallback(int type, const char *uri, const char *base)
{//=============================================================

	if(strcmp(uri,"hello")==0)
		return(1);
	return(0);
}



int TestSynthCallback(short *wav, int numsamples, espeak_EVENT *events)
{//====================================================================
	int type;
	f_events = fopen("/home/jsd1/speechdata/text/events","a");

	fprintf(f_events,"--\n");

	while((type = events->type) != 0)
	{
		fprintf(f_events,"%2d (%4d %4ld)   %5d %5d  (%3d) ",type,events->unique_identifier,(long)events->user_data,events->audio_position,events->text_position,events->length);

		if((type==3) || (type==4))
			fprintf(f_events,"'%s'\n",events->id.name);
		else
		if(type==espeakEVENT_PHONEME)
			fprintf(f_events,"[%s]\n",WordToString(events->id.number));
		else
			fprintf(f_events,"%d\n",events->id.number);

		events++;
	}

	if((wav == NULL) && (f_wavtest != NULL))
	{
		fprintf(f_events,"Finished\n");
		CloseWaveFile3(f_wavtest);
		f_wavtest = NULL;
	}
	fclose(f_events);

	if(f_wavtest == NULL) return(0);
	fwrite(wav,numsamples*2,1,f_wavtest);
	return(0);
}

//******************************************************************************************************


#ifdef deleted
static int RuLex_sorter(char **a, char **b)
{//=======================================
	char *pa, *pb;
	int xa, xb;
	int ix;

	pa = *a;
	pb = *b;

	xa = strlen(pa)-1;
	xb = strlen(pb)-1;

	while((xa >= 0) && (xb >= 0))
	{
		if((ix = (pa[xa] - pb[xb])) != 0)
			return(ix);

		xa--;
		xb--;
	}
	return(pa - pb);
}   /* end of strcmp2 */
#endif


static const unsigned short KOI8_R[0x60] = {
   0x2550, 0x2551, 0x2552, 0x0451, 0x2553, 0x2554, 0x2555, 0x2556, // a0
   0x2557, 0x2558, 0x2559, 0x255a, 0x255b, 0x255c, 0x255d, 0x255e, // a8
   0x255f, 0x2560, 0x2561, 0x0401, 0x2562, 0x2563, 0x2564, 0x2565, // b0
   0x2566, 0x2567, 0x2568, 0x2569, 0x256a, 0x256b, 0x256c, 0x00a9, // b8
   0x044e, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433, // c0
   0x0445, 0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e, // c8
   0x043f, 0x044f, 0x0440, 0x0441, 0x0442, 0x0443, 0x0436, 0x0432, // d0
   0x044c, 0x044b, 0x0437, 0x0448, 0x044d, 0x0449, 0x0447, 0x044a, // d8
   0x042e, 0x0410, 0x0411, 0x0426, 0x0414, 0x0415, 0x0424, 0x0413, // e0
   0x0425, 0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e, // e8
   0x041f, 0x042f, 0x0420, 0x0421, 0x0422, 0x0423, 0x0416, 0x0412, // f0
   0x042c, 0x042b, 0x0417, 0x0428, 0x042d, 0x0429, 0x0427, 0x042a, // f8
};


#define N_CHARS  34
int *p_unicode;
int unicode[80];
#define PH(c1,c2)  (c2<<8)+c1          // combine two characters into an integer for phoneme name 



static void DecodePhonemes2(const char *inptr, char *outptr)
//===================================================
// Translate from internal phoneme codes into phoneme mnemonics
// This version is for Lexicon_De()
{
	unsigned char phcode;
	unsigned char c;
	unsigned int  mnem;
	PHONEME_TAB *ph;
	const char *p;
	int ix;
	int j;
	int start;
	static const char *stress_chars = "==,,'*  ";

	unsigned int replace_ph[] = {',',PH('@','-'),'W','3','y','A',PH('A',':'),'*',PH('_','!'),PH('_','|'),PH('O','I'),PH('Y',':'),PH('p','F'),PH('E','2'),0};
	const char *replace_ph2[] = {NULL,NULL,"9","@r","Y","a:",  "a:",        "r",   "?",        "?",        "OY",       "2:",   "pf" ,"E",NULL};


	start = 1;
	for(ix=0; (phcode = inptr[ix]) != 0; ix++)
	{
		if(phcode == 255)
			continue;     /* indicates unrecognised phoneme */
		if((ph = phoneme_tab[phcode]) == NULL)
			continue;
	
		if((ph->type == phSTRESS) && (ph->std_length <= 4) && (ph->spect == 0))
		{
			if(ph->std_length > 2)
				*outptr++ = stress_chars[ph->std_length];
		}
		else
		{
			mnem = ph->mnemonic;
			if(ph->type == phPAUSE)
			{
				if(start)
					continue;   // omit initial [?]

				if(inptr[ix+1] == phonSCHWA_SHORT)
					continue;   // omit [?] before [@-*]
			}

			start = 0;
			p = NULL;

			for(j=0;;j++)
			{
				if(replace_ph[j] == 0)
					break;

				if(mnem == replace_ph[j])
				{
					p = replace_ph2[j];
					if(p == NULL)
						mnem = 0;
					break;
				}
			}

			if(p != NULL)
			{
				while((c = *p++) != 0)
				{
					*outptr++ = c;
				}
			}
			else
			if(mnem != 0)
			{
				while((c = (mnem & 0xff)) != 0)	
				{
					*outptr++ = c;
					mnem = mnem >> 8;
				}
			}
		}
	}
	*outptr = 0;    /* string terminator */
}   //  end of DecodePhonemes2


void Lexicon_De()
{//==============
// Compare eSpeak's translation of German words with a pronunciation lexicon
	FILE *f_in;
	FILE *f_out;
	int ix;
	int c;
	int c2;
	char *p;
	int stress;
	int count=0;
	int start;
	int matched=0;
	int defer_stress = 0;
	char buf[200];
	char word[80];
	char word2[80];
	char type[80];
	char pronounce[80];
	char pronounce2[80];
	char phonemes[80];
	char phonemes2[80];
	WORD_TAB winfo;

	static const char *vowels = "aeiouyAEIOUY29@";

	wxString fname = wxFileSelector(_T("German Lexicon"),path_dir1,_T(""),_T(""),_T("*"),wxOPEN);

	strcpy(buf,fname.mb_str(wxConvLocal));
	if((f_in = fopen(buf,"r")) == NULL)
	{
		wxLogError(_T("Can't read file ")+fname);
		return;
	}
	path_dir1 = wxFileName(fname).GetPath();
	
	if((f_out = fopen("compare_de","w")) == NULL)
	{
		wxLogError(_T("Can't write file "));
		return;
	}
	LoadVoice("de",0);

	word2[0] = ' ';
	while(!feof(f_in))
	{
		count++;

		if(fgets(buf,sizeof(buf),f_in) == NULL)
			break;

		sscanf(buf,"%s %s %s",word,type,pronounce);

		// convert word to lower-case
		for(ix=0, p=&word2[1];;)
		{
			ix += utf8_in(&c,&word[ix]);
			c = towlower(c);
			p += utf8_out(c,p);
			if(c == 0)
				break;
		}
		strcpy(word,&word2[1]);
		strcat(&word2[1],"  ");

		// remove | syllable boundaries
		stress=0;
		start=1;
		for(ix=0, p=pronounce2;;ix++)
		{
			c = pronounce[ix];
			if(c == '\'')
			{
				stress=4;
				continue;
			}
			if(c == ',')
			{
				stress=3;
				continue;
			}
			if(c == '|')
				continue;

			if((c == '?') && start)
				continue;      // omit initial [?]

			if(c == '<')
			{
				if((c2 = pronounce[ix+1]) == 'i')
				{
					defer_stress =1;
#ifdef deleted
					if(stress == 4)
					{
						*p++ = 'i';
						c =':';
					}
					else
#endif
					{
						c = 'I';
					}
					ix++;
				}
			}

			start =0;
			if(defer_stress)
			{
				defer_stress = 0;
			}
			else
			if(stress && (strchr(vowels,c) != NULL))
			{
				if(stress == 4)
					*p++ = '\'';
				if(stress == 3)
					*p++ = ',';
				stress = 0;
			}

			*p++ = c;
			if(c == 0)
				break;

			if(strchr("eiouy",c) && pronounce[ix+1] != ':')
				*p++ = ':';    // ensure [;] after these vowels
		}

		// translate
		memset(&winfo,0,sizeof(winfo));
		TranslateWord(translator,&word2[1],0,&winfo);

		DecodePhonemes2(word_phonemes,phonemes);  // also need to change some phoneme names

		if(strcmp(phonemes,pronounce2) == 0)
		{
			matched++;
		}
		else
		{
			// remove secondary stress
			strcpy(phonemes2,phonemes);
			p = phonemes;
			for(ix=0; ;ix++)
			{
				if((c = phonemes2[ix]) != ',')
					*p++ = c;
				if(c == 0)
					break;
			}

			if(strcmp(phonemes,pronounce2) == 0)
			{
				matched++;
			}
			else
			{
				if(strlen(word) < 8)
					strcat(word,"\t");
				fprintf(f_out,"%s\t%s\t%s\n",word,phonemes,pronounce2);
			}
		}
	}

	fclose(f_in);
	fclose(f_out);
	wxLogStatus(_T("Completed, equal=%d  different=%d"),matched,count-matched);
}


void Lexicon_Ru()
{//==============
	// compare stress markings in Russian RuLex file with lookup in ru_rules
	int ix;
	char *p;
	int  c;
	FILE *f_in;
	FILE *f_out;
	FILE *f_log;
	FILE *f_roots;
	PHONEME_TAB *ph;
	int vcount;
	int ru_stress;
	int max_stress;
	int max_stress_posn;
	int n_words=0;
	int n_wrong=0;
	int wlength;
	int input_length;

	int sfx;
	const char *suffix;
	int wlen;
	int len;
	int check_root;
	WORD_TAB winfo;

	char word[80];
	char word2[80];
	int counts[20][20][10];

	char phonemes[N_WORD_PHONEMES];
	char buf[200];
	char fname[sizeof(path_dsource)+20];

	// KOI8-R codes for Russian vowels
	static unsigned char vowels[] = {0xa3,0xc0,0xc1,0xc5,0xc9,0xcf,0xd1,0xd5,0xd9,0xdc,0};

	typedef struct {
		const char *suffix;
		int  syllables;
	} SUFFIX;

	static SUFFIX suffixes[] = {
{NULL,0},
	{"ичу",2},
	{"ского",2},
	{"ская",2},
	{"ски",1},
	{"ские",2},
	{"ский",1},
	{"ским",1},
	{"ское",2},
	{"ской",1},
	{"ском",1},
	{"скую",2},

	{"а",1},
	{"е",1},
	{"и",1},

	{NULL,0}};


	memset(counts,0,sizeof(counts));

	if(gui_flag)
	{
		wxString fname = wxFileSelector(_T("Read lexicon.dict"),path_dictsource,
			_T(""),_T(""),_T("*"),wxOPEN);
		if(fname.IsEmpty())
			return;
		strcpy(buf,fname.mb_str(wxConvLocal));
	}
	else
	{
		strcpy(buf,"lexicon.dict");
	}

	if((f_in = fopen(buf,"r")) == NULL)
	{
		if(gui_flag)
			wxLogError(_T("Can't read file: ") + wxString(buf,wxConvLocal));
		else
			fprintf(stderr,"Can't read file: %s\n",buf);
		return;
	}
	input_length = GetFileLength(buf);

	sprintf(fname,"%s%c%s",path_dsource,PATHSEP,"ru_listx_1");
	if((f_out = fopen(fname,"w")) == NULL)
	{
		wxLogError(_T("Can't write to: ")+wxString(fname,wxConvLocal));
		fclose(f_in);
		return;
	}

	sprintf(fname,"%s%c%s",path_dsource,PATHSEP,"ru_log");
	f_log = fopen(fname,"w");
	sprintf(fname,"%s%c%s",path_dsource,PATHSEP,"ru_roots_1");
	f_roots = fopen(fname,"w");

	LoadVoice("ru",0);


	if(gui_flag)
		progress = new wxProgressDialog(_T("Lexicon"),_T(""),input_length);
	else
		fprintf(stderr,"Processing lexicon.dict\n");

	for(;;)
	{
		if(((n_words & 0x3ff) == 0) && gui_flag)
		{
			progress->Update(ftell(f_in));
		}

		if(fgets(buf,sizeof(buf),f_in) == NULL)
			break;

		if(isspace2(buf[0]))
			continue;

		// convert word from KOI8-R to UTF8
		p = buf;
		ix = 0;
		wlength = 0;
p_unicode = unicode;
		while(!isspace2(c = (*p++ & 0xff)))
		{
			if(c >= 0xa0)
			{
				c = KOI8_R[c-0xa0];
*p_unicode++ = c;
			}


			wlength++;
			ix += utf8_out(c,&word[ix]);
		}
		word[ix] = 0;
*p_unicode=0;

		sprintf(word2," %s ",word);

		// find the marked stress position
		vcount = 0;
		ru_stress = 0;
		while(*p == ' ') p++;

		while((c = (*p++ & 0xff)) != '\n')
		{
			if(c == '+')
			{
				ru_stress = vcount;
				break;
			}
			if(strchr((char *)vowels,c) != NULL)
			{
				vcount++;
			}
		}

		// translate
		memset(&winfo,0,sizeof(winfo));
		TranslateWord(translator, &word2[1],0,&winfo);
		DecodePhonemes(word_phonemes,phonemes);

		// find the stress position in the translation
		max_stress = 0;
		max_stress_posn = -1;
		vcount = 0;
		check_root = 0;

		ph = phoneme_tab[phonPAUSE];
		for(p=word_phonemes; *p != 0; p++)
		{
			ph = phoneme_tab[(unsigned int)*p];
			if(ph == NULL)
				continue;

			if(ph->type == phVOWEL)
				vcount++;
			if(ph->type == phSTRESS)
			{
				if(ph->std_length > max_stress)
				{
					max_stress = ph->std_length;
					max_stress_posn = vcount+1;
				}
			}
		}

		n_words++;
		if(ru_stress > vcount)
		{
			if(f_log != NULL)
			{
				fprintf(f_log,"%s\t $%d\t // %s\n",word,ru_stress,phonemes);
			}
		}
		else
		{
			counts[vcount][ru_stress][ph->type]++;

			if((vcount > 1) && (ru_stress != max_stress_posn))
			{
				n_wrong++;
				if((ru_stress==0) || (ru_stress > 7))
					fprintf(f_out,"// ");   // we only have $1 to $7 to indicate stress position
				else
					check_root = 1;

#define X_COMPACT

				fprintf(f_out,"%s",word);
#ifdef X_COMPACT
				if(wlength < 8) fputc('\t',f_out);
				if(wlength < 16) fputc('\t',f_out);
				fprintf(f_out," $%d\n",ru_stress);
#else
				while(wlength++ < 20)
					fputc(' ',f_out);
				fprintf(f_out," $%d  //%d  %s\n",ru_stress,max_stress_posn,phonemes);
#endif
//CharStats();
			}
		}

		if(check_root)
		{
			// does this word match any suffixes ?
			wlen = strlen(word);
			for(sfx=0;(suffix = suffixes[sfx].suffix) != NULL; sfx++)
			{
				len = strlen(suffix);
				if(len >= (wlen-2))
					continue;
	
				if(ru_stress > (vcount - suffixes[sfx].syllables))
					continue;
				
				if(strcmp(suffix,&word[wlen-len])==0)
				{
					strcpy(word2,word);
					word2[wlen-len] = 0;
//					fprintf(f_roots,"%s\t $%d\t\\ %s\n",word2,ru_stress,suffix);
					fprintf(f_roots,"%s\t $%d\n",word2,ru_stress);
				}
			}
		}
	}

	fclose(f_in);
	fclose(f_out);
	fclose(f_roots);

	sprintf(buf,"Lexicon: Total %d  OK %d  wrong %d",n_words,n_words-n_wrong,n_wrong);
	if(gui_flag)
	{
		delete progress;
		wxLogStatus(wxString(buf,wxConvLocal));
	}
	else
	{
		fprintf(stderr,"%s\n",buf);
	}

	if(f_log != NULL)
	{

#ifdef deleted
		// list tables of frequency of stress position for words of different syllable lengths
		int j,k;
		for(ix=0; ix<12; ix++)
		{
			fprintf(f_log,"%2d syl: ",ix);
			for(k=0; k<10; k++)
			{
				fprintf(f_log,"  %2d :",k);
				for(j=0; j<10; j++)
				{
					fprintf(f_log,"%6d ",counts[ix][j][k]);
				}
				fprintf(f_log,"\n");
			}
			fprintf(f_log,"\n\n");
		}
#endif
		fclose(f_log);
	}

}  // end of Lexicon_Ru



void CompareLexicon(int id)
{//========================
	switch(id)
	{
	case MENU_LEXICON_RU:
		Lexicon_Ru();
		break;
	case MENU_LEXICON_DE:
		Lexicon_De();
		break;
	}
}  // end of CompareLexicon


//******************************************************************************************************
extern int HashDictionary(const char *string);
static int n_words;

struct wcount {
	struct wcount *link;
	int count;
	char *word;
};



static int wfreq_sorter(wcount **p1, wcount **p2)
{//==============================================
	int x;
	wcount *a, *b;
	a = *p1;
	b = *p2;
	if((x = b->count - a->count) != 0)
		return(x);
	return(strcmp(a->word,b->word));
}


static void wfreq_add(const char *word, wcount **hashtab)
{//======================================================
	wcount *p;
	wcount **p2;
	int len;
	int hash;

	hash = HashDictionary(word);
	p2 = &hashtab[hash];
	p = *p2;

	while(p != NULL)
	{
		if(strcmp(p->word,word)==0)
		{
			p->count++;
			return;
		}
		p2 = &p->link;
		p = *p2;
	}

	// word not found, add it to the list
	len = strlen(word) + 1;
	if((p = (wcount *)malloc(sizeof(wcount)+len)) == NULL)
		return;

	p->count = 1;
	p->link = NULL;
	p->word = (char *)p + sizeof(wcount);
	strcpy(p->word,word);
	*p2 = p;
	n_words++;
}


void CountWordFreq(wxString path, wcount **hashtab)
{//================================================
	// Count the occurances of words in this file
	FILE *f_in;
	unsigned char c;
	int wc;
	unsigned int ix, j, k;
	int n_chars;
	char buf[80];
	char wbuf[80];

	if((f_in = fopen(path.mb_str(wxConvLocal),"rb")) == NULL)
		return;

	while(!feof(f_in))
	{
		while((c = fgetc(f_in)) < 'A')
		{
			// skip leading spaces, numbers, etc
			if(feof(f_in)) break;
		}
	
		// read utf8 bytes until a space, number or punctuation
		ix = 0;
		while(!feof(f_in) && (c >= 'A') && (ix < sizeof(buf)-1))
		{
			buf[ix++] = c;
			c = fgetc(f_in);
		}
		buf[ix++] = 0;
		buf[ix] = 0;
	
		// the buf may contain non-alphabetic characters
		j = 0;
		n_chars = 0;
		for(k=0; k<ix; )
		{
			k += utf8_in(&wc,&buf[k]);
			wc = towlower(wc);       // convert to lower case
			if(iswalpha(wc))
			{
				j += utf8_out(wc,&wbuf[j]);
				n_chars++;
			}
			else
			{
				wbuf[j] = 0;
				if(n_chars > 2)
				{
					wfreq_add(wbuf,hashtab);
				}
				j = 0;
				n_chars = 0;
			}
		}
	}
	fclose(f_in);
	
}   // end of CountWordFreq


void MakeWordFreqList()
{//====================
// Read text files from a specified directory and make a list of the most frequently occuring words.
	struct wcount *whashtab[N_HASH_DICT];
	wcount **w_list;
	int ix;
	int j;
	int hash;
	wcount *p;
	FILE *f_out;
	char buf[200];
	char buf2[200];

	wxString dir = wxDirSelector(_T("Directory of text files"),path_speaktext);
	if(dir.IsEmpty()) return;

	memset(whashtab,0,sizeof(whashtab));

	wxString path = wxFindFirstFile(dir+_T("/*"),wxFILE);

	while (!path.empty())
	{
		if(path.AfterLast(PATHSEP) != _T("!wordcounts"))
		{
			CountWordFreq(path,whashtab);
			path = wxFindNextFile();
		}
	}

	// put all the words into a list and then sort it
	w_list = (wcount **)malloc(sizeof(wcount *) * n_words);

	ix = 0;
	for(hash=0; hash < N_HASH_DICT; hash++)
	{
		p = whashtab[hash];
		while((p != NULL) && (ix < n_words))
		{
			w_list[ix++] = p;
			p = p->link;
		}
	}

	qsort((void *)w_list,ix,sizeof(wcount *),(int(*)(const void *,const void *))wfreq_sorter);

	// write out the sorted list
	strcpy(buf,dir.mb_str(wxConvLocal));
	sprintf(buf2,"%s/!wordcounts",buf);
	if((f_out = fopen(buf2,"w")) == NULL)
		return;

	for(j=0; j<ix; j++)
	{
		p = w_list[j];
		fprintf(f_out,"%5d  %s\n",p->count,p->word);
		free(p);
	}
	fclose(f_out);
	
}  // end of Make WorkFreqList



//******************************************************************************************************

void ConvertToUtf8()
{//=================
// Convert a file from 8bit to UTF8, according to the current voice
	unsigned int c;
	int ix;
	FILE *f_in;
	FILE *f_out;
	char buf[80];

	wxString fname = wxFileSelector(_T("Convert file to UTF8"),wxString(path_home,wxConvLocal),
		_T(""),_T(""),_T("*"),wxOPEN);
	if(fname.IsEmpty())
		return;
	strcpy(buf,fname.mb_str(wxConvLocal));
	f_in = fopen(buf,"r");
	if(f_in == NULL)
	{
		wxLogError(_T("Can't read file: ")+fname);
		return;
	}

	strcat(buf,"_1");
	f_out = fopen(buf,"w");
	if(f_out == NULL)
	{
		wxLogError(_T("Can't create file: ")+wxString(buf,wxConvLocal));
		fclose(f_in);
		return;
	}

	while(!feof(f_in))
	{
		c = fgetc(f_in);
		if(c >= 0xa0)
			c = translator->charset_a0[c-0xa0];

		ix = utf8_out(c,buf);
		fwrite(buf,ix,1,f_out);
	}
	fclose(f_in);
	fclose(f_out);

	wxLogStatus(_T("Written to: ")+fname+_T("_1"));

}  // end of ConvertToItf8

//******************************************************************************************************



//#define calcspeedtab
#ifdef calcspeedtab
// used to set up the presets in the speed_lookup table
// interpolate between a set of measured wpm values
void SetSpeedTab(void)
{//===================
#define N_WPM   19

	// Interpolation table to translate from words-per-minute to internal speed
	// words-per-minute values (measured)
	static float wpm1[N_WPM] =
		{0, 82, 96, 108, 124, 134, 147, 162, 174, 189, 224, 259, 273, 289, 307, 326, 346, 361, 370 }; 
	// corresponding internal speed values
	static float wpm2[N_WPM] = 
		{0,253,200, 170, 140, 125, 110,  95,  85,  75,  55,  40,  35,  30,  25,  20,  15,  10,  5 };

	unsigned char speed_lookup[290];
	unsigned int ix;
	float x;
	int speed_wpm;
	FILE *f;

	// convert from word-per-minute to internal speed code
	for(speed_wpm=80; speed_wpm<370; speed_wpm++)
	{
		for(ix=2; ix<N_WPM-2; ix++)
		{
			if(speed_wpm < wpm1[ix])
				break;
		}
		x = polint(&wpm1[ix-1], &wpm2[ix-1], 3, speed_wpm);

		speed_lookup[speed_wpm-80] = (unsigned char)x;
	}
	f = fopen("speed_lookup","w");
	if(f == NULL) return;
	for(ix=0; ix<sizeof(speed_lookup); ix++)
	{
		fprintf(f,"%4d,",speed_lookup[ix]);
		if((ix % 5) == 4)
			fprintf(f,"\t//%4d\n\t",(ix / 5)*5 + 80);
	}
	fclose(f);
}	// end of SetSpeedTab
#endif

//#define xcharset
#ifdef xcharset
#include "iconv.h"
void CharsetToUnicode(const char *charset)
{//=======================================
// write a 8bit charset to unicode translation table to file
// charset:  eg. "ISO-8859-1"
	iconv_t cd;
	unsigned char inbuf[4];
	size_t n_inbuf;
	unsigned char outbuf[12];
	size_t n_outbuf;
	int n;
	int ix;
	int x, y;
	FILE *f;
	char *p_inbuf;
	char *p_outbuf;

	f = fopen("/home/jsd1/tmp1/unicode1","a");

	cd = iconv_open("WCHAR_T",charset);
	if (cd == (iconv_t) -1)
	{
		fprintf(stderr,"Error - iconv_open\n");
		return;
	}

	fprintf(f,"towlower_tab\n   ");
	for(ix=0x80; ix<=0x241; ix++)
	{
		y = 0;
		if(iswalpha(ix))
		{
			x = towlower(ix);
			if(x == ix)
				y = 0xff;
			else
				y = x - ix;
		}
		if((y == 0xff) || (y < 0))
			fprintf(f,"0xff,");  // ignore the 5 obscure cases where uc > lc
		else
		{
			fprintf(f,"%4d,",y);
		}
		if((ix&15)==15)
			fprintf(f,"  // %x\n   ",ix & ~15);
	}

	fprintf(f,"\n%s\n   ",charset);
	for(ix=0x80; ix<0x100; ix++)
	{
		inbuf[0] = ix;
		inbuf[1] = 0;
		inbuf[2] = 0;
		outbuf[0] = 0;
		outbuf[1] = 0;
		n_inbuf = 2;
		n_outbuf = sizeof(outbuf);
		p_inbuf = (char *)inbuf;
		p_outbuf = (char *)outbuf;

		n = iconv(cd, &p_inbuf, &n_inbuf, &p_outbuf, &n_outbuf);

		fprintf(f,"0x%.2x%.2x, ",outbuf[1],outbuf[0]);
		if((ix&7)==7)
			fprintf(f,"// %.2x\n   ",ix & ~7);
	}
	fclose(f);

	iconv_close(cd);
}
#endif




#ifdef deleted
void Test2()
{
// 
	char buf[120];
	FILE *f;
	FILE *f_out;
	unsigned char *p;

	f = fopen("/home/jsd1/tmp1/list","r");
	if(f == NULL) return;
	f_out = fopen("/home/jsd1/tmp1/list_out","w");
	if(f_out == NULL) return;

	while(!feof(f))
	{
		if(fgets(buf,sizeof(buf),f) == NULL)
			break;

		p = (unsigned char *)buf;
		while(*p > ' ') p++;
		*p = 0;
		fprintf(f_out,"%s . . .\n",buf);
	}
	fclose(f);
	fclose(f_out);
}

#endif



const char* text1 = "Hello world. Testing.";
extern void TestCompile2(void);

void TestTest(int control)
{//=======================
	FILE *f;
	unsigned int c;
	unsigned int ix=0;
	char textbuf[2000];
	espeak_VOICE voice;
	static unsigned int unique_identifier= 123;
	static int user_data = 456;

//CharsetToUnicode("ISO-8859-4");
//CharsetToUnicode("ISCII");

TestCompile2();
return;

if(control==2)
{
	return;
}
	memset(&voice,0,sizeof(voice));

	f = fopen("/home/jsd1/speechdata/text/test.txt","r");
	if(f==NULL)
		return;
	

	while(!feof(f) && (ix < sizeof(textbuf)-2))
	{
		c = fgetc(f);
		if(!feof(f))
			textbuf[ix++] = c;
	}
	textbuf[ix] = 0;
	fclose(f);

	f_wavtest = OpenWaveFile3("/home/jsd1/speechdata/text/test.wav");
	f_events = fopen("/home/jsd1/speechdata/text/events","w");
	fprintf(f_events,"Type             Audio  Text  Length Id\n");
	fclose(f_events);

	espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL,1000,NULL,1);
	espeak_SetSynthCallback(TestSynthCallback);
	espeak_SetUriCallback(TestUriCallback);

  espeak_Synth(text1, strlen(text1)+1, 0, POS_CHARACTER, 0,  espeakSSML|espeakCHARS_UTF8, &unique_identifier, (void *)user_data);
  espeak_Synth(text1, strlen(text1)+1, 0, POS_CHARACTER, 0,  espeakSSML|espeakCHARS_UTF8, &unique_identifier, (void *)(user_data+1));

  espeak_SetParameter(espeakPUNCTUATION, 1, 0);
  espeak_Synchronize();
  //  espeak_Cancel();
  espeak_SetParameter(espeakPUNCTUATION, 1, 0);

}


