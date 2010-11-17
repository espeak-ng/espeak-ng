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

#include "stdafx.h"
#include "TtsEngObj.h"

#include "src/speak_lib.h"
#include "stdio.h"

#define CTRL_EMBEDDED  1

CTTSEngObj *m_EngObj;
ISpTTSEngineSite* m_OutputSite;
FILE *f_log2=NULL;
ULONGLONG event_interest;

extern int AddNameData(const char *name, int wide);
extern void InitNamedata(void);

int master_volume = 100;
int master_rate = 0;

int initialised = 0;
int gVolume = 100;
int gSpeed = -1;
int gPitch = -1;
int gRange = -1;
int gEmphasis = 0;
int gSayas = 0;
char g_voice_name[80];


char *path_install = NULL;

unsigned long audio_offset = 0;
unsigned long audio_latest = 0;
int prev_phoneme = 0;
int prev_phoneme_position = 0;
unsigned long prev_phoneme_time = 0;

unsigned int gBufSize = 0;
wchar_t *TextBuf=NULL;

typedef struct {
	unsigned int bufix;
	unsigned int textix;
	unsigned int cmdlen;
} FRAG_OFFSET;

int srate;   // samplerate, Hz/50
int n_frag_offsets = 0;
int frag_ix = 0;
int frag_count=0;
FRAG_OFFSET *frag_offsets = NULL;


//#define TEST_INPUT    // printf input text received from SAPI to espeak_text_log.txt
#ifdef TEST_INPUT
static int utf8_out(unsigned int c, char *buf)
{//====================================
// write a unicode character into a buffer as utf8
// returns the number of bytes written
	int n_bytes;
	int j;
	int shift;
	static char unsigned code[4] = {0,0xc0,0xe0,0xf0};

	if(c < 0x80)
	{
		buf[0] = c;
		return(1);
	}
	if(c >= 0x110000)
	{
		buf[0] = ' ';      // out of range character code
		return(1);
	}
	if(c < 0x0800)
		n_bytes = 1;
	else
	if(c < 0x10000)
		n_bytes = 2;
	else
		n_bytes = 3;

	shift = 6*n_bytes;
	buf[0] = code[n_bytes] | (c >> shift);
	for(j=0; j<n_bytes; j++)
	{
		shift -= 6;
		buf[j+1] = 0x80 + ((c >> shift) & 0x3f);
	}
	return(n_bytes+1);
}  // end of utf8_out
#endif


int VisemeCode(unsigned int phoneme_name)
{//======================================
// Convert eSpeak phoneme name into a SAPI viseme code

	int ix;
	unsigned int ph;
	unsigned int ph_name;

#define PH(c1,c2)  (c2<<8)+c1          // combine two characters into an integer for phoneme name 

	const unsigned char initial_to_viseme[128] = {
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,19, 0, 0, 0, 0, 0,
		 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,255,
		 4, 2,18,16,17, 4,18,20,12, 6,16,20,14,21,20, 3,
		21,20,13,16,17, 4, 1, 5,20, 7,16, 0, 0, 0, 0, 0,
		 0, 1,21,16,19, 4,18,20,12, 6, 6,20,14,21,19, 8,
		21,20,13,15,19, 7,18, 7,20, 7,15, 0, 0, 0, 0, 0 };

	const unsigned int viseme_exceptions[] = {
		PH('a','I'), 11,
		PH('a','U'),  9,
		PH('O','I'), 10,
		PH('t','S'), 16,
		PH('d','Z'), 16,
		PH('_','|'), 255,
		0
	};
	
	ph_name = phoneme_name & 0xffff;
	for(ix=0; (ph = viseme_exceptions[ix]) != 0; ix+=2)
	{
		if(ph == ph_name)
		{
			return(viseme_exceptions[ix+1]);
		}
	}
	return(initial_to_viseme[phoneme_name & 0x7f]);
}


int SynthCallback(short *wav, int numsamples, espeak_EVENT *events);

int SynthCallback(short *wav, int numsamples, espeak_EVENT *events)
{//================================================================
	int hr;
	wchar_t *tailptr;
	unsigned int text_offset;
	int length;
	int phoneme_duration;
	int this_viseme;

	espeak_EVENT *event;
#define N_EVENTS 100
	int n_Events = 0;
	SPEVENT *Event;
	SPEVENT Events[N_EVENTS];

	if(m_OutputSite->GetActions() & SPVES_ABORT)
		return(1);

	m_EngObj->CheckActions(m_OutputSite);

	// return the events
	for(event=events; (event->type != 0) && (n_Events < N_EVENTS); event++)
	{

		audio_latest = event->audio_position + audio_offset;

		if((event->type == espeakEVENT_WORD) && (event->length > 0))
		{
			while(((frag_ix+1) < frag_count) &&
				((event->text_position -1 + frag_offsets[frag_ix+1].cmdlen) >= frag_offsets[frag_ix+1].bufix))
			{
				frag_ix++;
			}
			text_offset = frag_offsets[frag_ix].textix + 
				event->text_position -1 - frag_offsets[frag_ix].bufix + frag_offsets[frag_ix].cmdlen;
			length = event->length - frag_offsets[frag_ix].cmdlen;
			frag_offsets[frag_ix].cmdlen = 0;

			if(text_offset < 0)
				text_offset = 0;

			Event = &Events[n_Events++];
			Event->eEventId             = SPEI_WORD_BOUNDARY;
			Event->elParamType          = SPET_LPARAM_IS_UNDEFINED;
			Event->ullAudioStreamOffset = ((event->audio_position + audio_offset) * srate)/10;  // ms -> bytes
			Event->lParam               = text_offset;
			Event->wParam               = length;
		}
		if(event->type == espeakEVENT_MARK)
		{
			Event = &Events[n_Events++];
			Event->eEventId             = SPEI_TTS_BOOKMARK;
			Event->elParamType          = SPET_LPARAM_IS_STRING;
			Event->ullAudioStreamOffset = ((event->audio_position + audio_offset) * srate)/10;  // ms -> bytes
			Event->lParam               = (long)event->id.name;
			Event->wParam               = wcstol((wchar_t *)event->id.name,&tailptr,10);
		}
		if(event->type == espeakEVENT_PHONEME)
		{
			if(event_interest & SPEI_VISEME)
			{
				phoneme_duration = audio_latest - prev_phoneme_time;

				// ignore some phonemes (which translate to viseme=255)
				if((this_viseme = VisemeCode(event->id.number)) != 255)
				{
					Event = &Events[n_Events++];
					Event->eEventId             = SPEI_VISEME;
					Event->elParamType          = SPET_LPARAM_IS_UNDEFINED;
					Event->ullAudioStreamOffset = ((prev_phoneme_position + audio_offset) * srate)/10;  // ms -> bytes
					Event->lParam               = phoneme_duration << 16 | this_viseme;
					Event->wParam               = VisemeCode(prev_phoneme);

					prev_phoneme = event->id.number;
					prev_phoneme_time = audio_latest;
					prev_phoneme_position = event->audio_position;
				}
			}
		}
#ifdef deleted
		if(event->type == espeakEVENT_SENTENCE)
		{
			Event = &Events[n_Events++];
			Event->eEventId             = SPEI_SENTENCE_BOUNDARY;
			Event->elParamType          = SPET_LPARAM_IS_UNDEFINED;
			Event->ullAudioStreamOffset = ((event->audio_position + audio_offset) * srate)/10;  // ms -> bytes
			Event->lParam               = 0;
			Event->wParam               = 0;  // TEMP
		}
#endif
	}
	if(n_Events > 0)
		m_OutputSite->AddEvents(Events, n_Events );

	// return the sound data
	hr = m_OutputSite->Write(wav, numsamples*2, NULL);
	return(hr);
}



static int ConvertRate(int new_rate)
{//=================================

	int rate;

	static int rate_table[21] = {80,110,124,135,145,155,165,173,180,187,196,
				     208,220,245,270,300,335,370,410,450,500 };

	rate = new_rate + master_rate;
	if(rate < -10) rate = -10;
	if(rate > 10) rate = 10;
	return(rate_table[rate+10]);
}  // end of ConvertRate


static int ConvertPitch(int pitch)
{//===============================
	static int pitch_table[41] =
                {0, 0, 0, 0, 0, 0, 0, 0, 4, 8,12,16,20,24,28,32,36,40,44,47,50,
		54,58,62,66,70,74,78,82,84,88,92,96,99,99,99,99,99,99,99,99};
//		{0,3,5,8,10,13,15,18,20,23,25,28,30,33,35,38,40,43,45,48,50,
//		53,55,58,60,63,65,68,70,73,75,78,80,83,85,88,90,93,95,97,99};
	if(pitch < -20) pitch = -20;
	if(pitch > 20) pitch = 20;
	return(pitch_table[pitch+20]);
}

static int ConvertRange(int range)
{//===============================
	static int range_table[21] = {16,28,39,49,58,66,74,81,88,94,100,105,110,115,120,125,130,135,140,145,150};
	if(range < -10) range = -10;
	if(range > 10) range = 10;
	return(range_table[range+10]/2);
}

HRESULT CTTSEngObj::FinalConstruct()
{//=================================
    SPDBG_FUNC( "CTTSEngObj::FinalConstruct" );
    HRESULT hr = S_OK;

#ifdef LOG_DEBUG
f_log2=fopen("C:\\log_espeak","a");
if(f_log2) fprintf(f_log2,"\n****\n");
#endif

    //--- Init vars
    m_hVoiceData = NULL;
    m_pVoiceData = NULL;
    m_pWordList  = NULL;
    m_ulNumWords = 0;

	m_EngObj = this;

    return hr;
} /* CTTSEngObj::FinalConstruct */


void CTTSEngObj::FinalRelease()
{//============================
    SPDBG_FUNC( "CTTSEngObj::FinalRelease" );

    delete m_pWordList;

#ifdef LOG_DEBUG
if(f_log2!=NULL) fclose(f_log2);
#endif

    if( m_pVoiceData )
    {
        ::UnmapViewOfFile( (void*)m_pVoiceData );
    }

    if( m_hVoiceData )
    {
        ::CloseHandle( m_hVoiceData );
    }

} /* CTTSEngObj::FinalRelease */


//
//=== ISpObjectWithToken Implementation ======================================
//

void WcharToChar(char *out, const wchar_t *in, int len)
{//====================================================
	int ix;

	for(ix=0; ix<len; ix++)
	{
		if((out[ix] = (char)in[ix]) == 0)
			break;
	}
	out[len-1] = 0;
}


/*****************************************************************************
* CTTSEngObj::SetObjectToken *
*----------------------------*
*   Description:
*   Read the "VoiceName" attribute from the registry, and use it to select
*   an eSpeak voice file
*****************************************************************************/
STDMETHODIMP CTTSEngObj::SetObjectToken(ISpObjectToken * pToken)
{
	strcpy(voice_name,"default");


	SPDBG_FUNC( "CTTSEngObj::SetObjectToken" );
	HRESULT hr = SpGenericSetObjectToken(pToken, m_cpToken);

	if( SUCCEEDED( hr ) )
	{
		CSpDynamicString voicename;
		CSpDynamicString path;
		HRESULT hr2;
		int len;

		hr2 = m_cpToken->GetStringValue( L"VoiceName", &voicename);
		if( SUCCEEDED(hr2) )
		{
			WcharToChar(voice_name,voicename,sizeof(voice_name));
		}


		hr2 = m_cpToken->GetStringValue( L"Path", &path);
		if( SUCCEEDED(hr2) )
		{
			len = wcslen(path)+1;
			path_install = (char *)malloc(len);
			WcharToChar(path_install,path,len);
		}
	}

	gVolume = 100;
	gSpeed = -1;
	gPitch = -1;
	gRange = -1;
	gEmphasis = 0;
	gSayas = 0;

	if(initialised==0)
	{
		espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS,100,path_install,1);
		espeak_SetSynthCallback(SynthCallback);
		initialised = 1;
//	    g_voice_name[0] = 0;
	}

	strcpy(g_voice_name, voice_name);
	espeak_SetVoiceByName(g_voice_name);
	
	return hr;
} /* CTTSEngObj::SetObjectToken */

//
//=== ISpTTSEngine Implementation ============================================
//

#define L(c1,c2)  (c1<<8)+c2          // combine two characters into an integer

static char *phoneme_names_en[] = {
	NULL,NULL,NULL," ",NULL,NULL,NULL,NULL,"'",",",
	"A:","a","V","0","aU","@","aI",
	"b","tS","d","D","E","3:","eI",
	"f","g","h","I","i:","dZ","k",
	"l","m","n","N","oU","OI","p",
	"r","s","S","t","T","U","u:",
	"v","w","j","z","Z",
	NULL
 };



int CTTSEngObj::WritePhonemes(SPPHONEID *phons, wchar_t *pW)
{//=========================================================
	int ph;
	int ix=2;
	int skip=0;
	int maxph = 49;
	char *p;
	int j;
	int lang;
	char **phoneme_names;
	char phbuf[200];
	espeak_VOICE *voice;

	voice = espeak_GetCurrentVoice();
	lang = (voice->languages[1] << 8) + (voice->languages[2]);

	phoneme_names = phoneme_names_en;
	maxph = 0;

	if(lang == L('e','n'))
	{
		phoneme_names = phoneme_names_en;
		maxph = 49;
	}

	if(maxph == 0)
		return(0);

	strcpy(phbuf,"[[");
	while(((ph = *phons++) != 0) && (ix < (sizeof(phbuf) - 3)))
	{
		if(skip)
		{
			skip = 0;
			continue;
		}
		if(ph > maxph)
			continue;

		p = phoneme_names[phons[0]];  // look at the phoneme after this one
		if(p != NULL)
		{
			if(p[0] == '\'')
			{
				phbuf[ix++] = '\'';  // primary stress, put before the vowel, not after
				skip=1;
			}
			if(p[0] == ',')
			{
				phbuf[ix++] = ',';  // secondary stress
				skip=1;
			}
		}

		p = phoneme_names[ph];  // look at this phoneme

		if(p != NULL)
		{
			strcpy(&phbuf[ix],p);
			ix += strlen(p);
		}
	}
	strcpy(&phbuf[ix],"]]");
	ix += 2;

	if(pW != NULL)
	{
		for(j=0; j<=ix; j++)
		{
			pW[j] = phbuf[j];
		}
	}
	return(strlen(phbuf));
}



int CTTSEngObj::ProcessFragList(const SPVTEXTFRAG* pTextFragList, wchar_t *pW_start, ISpTTSEngineSite* pOutputSite, int *n_text)
{//============================================================================================================================

	int action;
	int control;
	wchar_t *pW;
	const SPVSTATE *state;
	unsigned int ix;
	unsigned int len;
	unsigned int total=0;
	char cmdbuf[50];
	wchar_t markbuf[32];

	int speed;
	int volume;
	int pitch;
	int range;
	int emphasis;
	int sayas;

	unsigned int text_offset = 0;

	frag_count = 0;
	frag_ix = 0;
	pW = pW_start;

	// check that the current voice is correct for this request
	if(strcmp(voice_name, g_voice_name) != 0)
	{
		strcpy(g_voice_name, voice_name);
		espeak_SetVoiceByName(g_voice_name);
	}

	while(pTextFragList != NULL)
	{
		action = pTextFragList->State.eAction;
		control = pOutputSite->GetActions();
		len = pTextFragList->ulTextLen;


		if(control & SPVES_ABORT)
			break;

		CheckActions(pOutputSite);
		sayas = 0;
		state = &pTextFragList->State;

		switch(action)
		{
		case SPVA_SpellOut:
			sayas = 0x12;   // SAYAS_CHARS;  // drop through to SPVA_Speak
		case SPVA_Speak:
			text_offset = pTextFragList->ulTextSrcOffset;
			audio_offset = audio_latest;

#ifdef deleted
// attempt to recognise when JAWS is spelling, it doesn't use SPVA_SpellOut
			if((pW != NULL) && (*n_text == 1) && ((len == 1) || ((len==2) && (pTextFragList->pTextStart[1]==' '))))
			{
				// A single text fragment with one character. Speak as a character, not a word
				sayas = 0x11;
				gSayas = 0;
			}
#endif

			if(frag_count >= n_frag_offsets)
			{
				if((frag_offsets = (FRAG_OFFSET *)realloc(frag_offsets,sizeof(FRAG_OFFSET)*(frag_count+500))) != NULL)
				{
					n_frag_offsets = frag_count+500;
				}
			}

			// first set the volume, rate, pitch
			volume = (state->Volume * master_volume)/100;
			speed = ConvertRate(state->RateAdj);
			pitch = ConvertPitch(state->PitchAdj.MiddleAdj);
			range = ConvertRange(state->PitchAdj.RangeAdj);
			emphasis = state->EmphAdj;
			if(emphasis != 0)
				emphasis = 3;

			len = 0;
			if(volume != gVolume)
			{
				sprintf(&cmdbuf[len],"%c%dA",CTRL_EMBEDDED,volume);
				len += strlen(&cmdbuf[len]);
			}
			if(speed != gSpeed)
			{
				sprintf(&cmdbuf[len],"%c%dS",CTRL_EMBEDDED,speed);
				len += strlen(&cmdbuf[len]);
			}
			if(pitch != gPitch)
			{
				sprintf(&cmdbuf[len],"%c%dP",CTRL_EMBEDDED,pitch);
				len += strlen(&cmdbuf[len]);
			}
			if(range != gRange)
			{
				sprintf(&cmdbuf[len],"%c%dR",CTRL_EMBEDDED,range);
				len += strlen(&cmdbuf[len]);
			}
			if(emphasis != gEmphasis)
			{
				sprintf(&cmdbuf[len],"%c%dF",CTRL_EMBEDDED,emphasis);
				len += strlen(&cmdbuf[len]);
			}
			if(sayas != gSayas)
			{
				sprintf(&cmdbuf[len],"%c%dY",CTRL_EMBEDDED,sayas);
				len += strlen(&cmdbuf[len]);
			}

			gVolume = volume;
			gSpeed = speed;
			gPitch = pitch;
			gRange = range;
			gEmphasis = emphasis;
			gSayas = sayas;

			total += (len + pTextFragList->ulTextLen);
			if(pTextFragList->ulTextLen > 0)
			{
				total++;
			}

			if(pW != NULL)
			{
				for(ix=0; ix<len; ix++)
				{
					*pW++ = cmdbuf[ix];
				}

				frag_offsets[frag_count].textix = text_offset;
				frag_offsets[frag_count].bufix = pW - pW_start;
				frag_offsets[frag_count].cmdlen = len;

#ifdef TEST_INPUT
{
FILE *f;
unsigned int c;
int n;
char buf[10];

f = fopen("C:\\espeak_text_log.txt","a");
if(f != NULL)
{
	fprintf(f,"----------\n");
	for(ix=0; ix<pTextFragList->ulTextLen; ix++)
	{
		c = pTextFragList->pTextStart[ix];
		n = utf8_out(c,buf);
		buf[n] = 0;
		fprintf(f,"%s",buf);
	}
	fprintf(f,"\n");
	fclose(f);
}
}
#endif
				for(ix=0; ix<pTextFragList->ulTextLen; ix++)
				{
					
					*pW++ = pTextFragList->pTextStart[ix];
				}
				if(pTextFragList->ulTextLen > 0)
				{
					*pW++ = ' ';
				}
			}
			frag_count++;
			break;

		case SPVA_Bookmark:
			total += (2 + pTextFragList->ulTextLen);

			if(pW != NULL)
			{
				int index;

				for(ix=0; ix<pTextFragList->ulTextLen; ix++)
				{
					markbuf[ix] = (char )pTextFragList->pTextStart[ix];
				}
				markbuf[ix] = 0;

				if((index = AddNameData((const char *)markbuf,1)) >= 0)
				{
					sprintf(cmdbuf,"%c%dM",CTRL_EMBEDDED,index);
					len = strlen(cmdbuf);
					for(ix=0; ix<len; ix++)
					{
						*pW++ = cmdbuf[ix];
					}
				}
			}
			break;

		case SPVA_Pronounce:
			total += WritePhonemes(state->pPhoneIds, pW);
			if(pW != NULL)
			{
				pW += total;
			}
			break;
		}


		pTextFragList = pTextFragList->pNext;
	}

	if(pW != NULL)
	{
		*pW = 0;
	}
	*n_text = frag_count;

	return(total);
}   // end of ProcessFragList



/*****************************************************************************
* CTTSEngObj::Speak *
*-------------------*
*   Description:
*       This is the primary method that SAPI calls to render text.
*-----------------------------------------------------------------------------
*   Input Parameters
*
*   pUser
*       Pointer to the current user profile object. This object contains
*       information like what languages are being used and this object
*       also gives access to resources like the SAPI master lexicon object.
*
*   dwSpeakFlags
*       This is a set of flags used to control the behavior of the
*       SAPI voice object and the associated engine.
*
*   VoiceFmtIndex
*       Zero based index specifying the output format that should
*       be used during rendering.
*
*   pTextFragList
*       A linked list of text fragments to be rendered. There is
*       one fragement per XML state change. If the input text does
*       not contain any XML markup, there will only be a single fragment.
*
*   pOutputSite
*       The interface back to SAPI where all output audio samples and events are written.
*
*   Return Values
*       S_OK - This should be returned after successful rendering or if
*              rendering was interrupted because *pfContinue changed to FALSE.
*       E_INVALIDARG 
*       E_OUTOFMEMORY
*
*****************************************************************************/
STDMETHODIMP CTTSEngObj::Speak( DWORD dwSpeakFlags,
                                REFGUID rguidFormatId,
                                const WAVEFORMATEX * pWaveFormatEx,
                                const SPVTEXTFRAG* pTextFragList,
                                ISpTTSEngineSite* pOutputSite )
{
    SPDBG_FUNC( "CTTSEngObj::Speak" );
    HRESULT hr = S_OK;

	unsigned int size;

	int xVolume;
	int xSpeed;
	int xPitch;
	int xRange;
	int xEmphasis;
	int xSayas;
	int punctuation;
	int n_text_frag=0;

    //--- Check args
    if( SP_IS_BAD_INTERFACE_PTR( pOutputSite ) ||
        SP_IS_BAD_READ_PTR( pTextFragList )  )
    {
        hr = E_INVALIDARG;
    }
    else
    {
		InitNamedata();

        //--- Init some vars
        m_pCurrFrag   = pTextFragList;
        m_pNextChar   = m_pCurrFrag->pTextStart;
        m_pEndChar    = m_pNextChar + m_pCurrFrag->ulTextLen;
        m_ullAudioOff = 0;

		m_OutputSite = pOutputSite;
		pOutputSite->GetEventInterest(&event_interest);

		xVolume = gVolume;
		xSpeed = gSpeed;
		xPitch = gPitch;
		xRange = gRange;
		xEmphasis = gEmphasis;
		xSayas = gSayas;

		// find the size of the text buffer needed for this Speak() request
		size = ProcessFragList(pTextFragList,NULL,pOutputSite,&n_text_frag);

		gVolume = xVolume;
		gSpeed = xSpeed;
		gPitch = xPitch;
		gRange = xRange;
		gEmphasis = xEmphasis;
		gSayas = xSayas;

		punctuation = 0;
		if(dwSpeakFlags & SPF_NLP_SPEAK_PUNC)
			punctuation = 1;

		espeak_SetParameter(espeakPUNCTUATION,punctuation,0);

		size = (size + 50)*sizeof(wchar_t);

		if(size > gBufSize)
		{
			size += 1000;  // some extra so we don't need to realloc() again too often
			TextBuf = (wchar_t *)realloc(TextBuf,size);
			if(TextBuf == NULL)
			{
				gBufSize=0;
				return(1);
			}
			gBufSize = size;
		}

		audio_latest = 0;
		prev_phoneme = 0;
		prev_phoneme_time = 0;
		prev_phoneme_position = 0;

		size = ProcessFragList(pTextFragList,TextBuf,pOutputSite,&n_text_frag);

		if(size > 0)
		{
			espeak_Synth(TextBuf,0,0,POS_CHARACTER,0,espeakCHARS_WCHAR | espeakKEEP_NAMEDATA | espeakPHONEMES,NULL,NULL);
		}
	}
    return hr;
} /* CTTSEngObj::Speak */





HRESULT CTTSEngObj::CheckActions( ISpTTSEngineSite* pOutputSite )
{//==============================================================
	int control;
	USHORT volume;
	long rate;

	control = pOutputSite->GetActions();

	if(control & SPVES_VOLUME)
	{
		if(pOutputSite->GetVolume(&volume) == S_OK)
		{
			master_volume = volume;
		}
	}
	if(control & SPVES_RATE)
	{
		if(pOutputSite->GetRate(&rate) == S_OK)
		{
			master_rate = rate;
		}
	}

	return(S_OK);
}  // end of CTTSEngObj::CheckActions



STDMETHODIMP CTTSEngObj::GetOutputFormat( const GUID * pTargetFormatId, const WAVEFORMATEX * pTargetWaveFormatEx,
                                          GUID * pDesiredFormatId, WAVEFORMATEX ** ppCoMemDesiredWaveFormatEx )
{//========================================================================
    SPDBG_FUNC( "CTTSEngObj::GetVoiceFormat" );
    HRESULT hr = S_OK;
	enum SPSTREAMFORMAT sample_rate = SPSF_22kHz16BitMono;

	srate = 441;
	if(espeak_GetParameter(espeakVOICETYPE,1) == 1)
	{
		srate = 320;
		sample_rate = SPSF_16kHz16BitMono;   // an mbrola voice
	}

    hr = SpConvertStreamFormatEnum(sample_rate, pDesiredFormatId, ppCoMemDesiredWaveFormatEx);

    return hr;
} /* CTTSEngObj::GetVoiceFormat */



int FAR PASCAL CompileDictionary(const char *voice, const char *path_log)
{//===========================================================
	FILE *f_log3;
	char fname[120];

	f_log3 = fopen(path_log,"w");
	sprintf(fname,"%s/",path_install);

	espeak_SetVoiceByName(voice);
	espeak_CompileDictionary(fname,f_log3,0);
	fclose(f_log3);

	return(0);
}


