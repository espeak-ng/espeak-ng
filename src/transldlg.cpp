/***************************************************************************
 *   Copyright (C) 2005,2006 by Jonathan Duddington                        *
 *   jonsd@users.sourceforge.net                                           *
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


#include "wx/wx.h"


#include "speak_lib.h"
#include "speech.h"
#include "phoneme.h"
#include "voice.h"
#include "main.h"
#include "synthesize.h"
#include "translate.h"
#include "prosodydisplay.h"

#include "wx/font.h"
#include "wx/textctrl.h"
#include "wx/button.h"
#include "wx/checkbox.h"
#include "wx/filename.h"
#include "wx/sound.h"

#define T_SOURCE    501
#define T_PHONETIC  502
#define T_TRANSLATE 503
#define T_PROCESS    504
#define T_RULES     505

TranslDlg *transldlg = NULL;
extern char *speech_to_phonemes(char *data, char *phout);
extern ProsodyDisplay *prosodycanvas;

BEGIN_EVENT_TABLE(TranslDlg, wxPanel)
	EVT_BUTTON(T_TRANSLATE,TranslDlg::OnCommand)
	EVT_BUTTON(T_PROCESS,TranslDlg::OnCommand)
	EVT_BUTTON(T_RULES,TranslDlg::OnCommand)
END_EVENT_TABLE()



class IPATextCtrl : public wxTextCtrl
{
public:
	void OnKey(wxKeyEvent& event);
	IPATextCtrl(wxWindow *parent,wxWindowID id,const wxPoint& pos,const wxSize& size);

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(IPATextCtrl, wxTextCtrl)
	EVT_CHAR(IPATextCtrl::OnKey)
END_EVENT_TABLE()


// using Kirschenbaum scheme to enter IPA from the keyboard
wchar_t ipa1[256] = {
0,0x250,32,32,32,32,32,32,32,32,0xa,0xb,0xc,0x26f,0x272,0x275,
32,32,0x25a,32,32,32,32,0x28d,32,32,32,32,32,32,32,32,
0x20,0x21,0x22,0x23,0x24,0x25,0x0e6,0x27,0x28,0x27e,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
0x252,0x31,0x32,0x25c,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x294,
0x259,0x251,0x3b2,0xe7,0xf0,0x25b,0x46,0x262,0x127,0x26a,0x25f,0x4b,0x26c,0x271,0x14b,0x254,
0x3a6,0x263,0x280,0x283,0x3b8,0x28a,0x28c,0x153,0x3c7,0xf8,0x292,0x32a,0x5c,0x5d,0x5e,0x5f,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x303,0x7f,
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff
};


IPATextCtrl::IPATextCtrl(wxWindow *parent,wxWindowID id,const wxPoint& pos,const wxSize& size) :
	wxTextCtrl(parent,id,_T(""),pos,size,wxTE_MULTILINE)
{
	wxTextAttr attr;
	wxFont font = wxFont(12,wxFONTFAMILY_ROMAN,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_LIGHT);

	attr.SetFont(font);
	SetDefaultStyle(attr);
}

void IPATextCtrl::OnKey(wxKeyEvent& event)
{
	long key;
	wchar_t wbuf[3];

	key = event.m_keyCode;
	key = event.GetKeyCode();

	if(event.ControlDown())
		key = key & 0x1f;

	if(key==0xd && !event.ControlDown())
		event.Skip();  // ENTER
	else
	if(key != 8 && key < 0x7f)
	{
		wbuf[0] = ipa1[key];
		wbuf[1] = 0;
		WriteText(wxString(wbuf));
	}
	else
		event.Skip();
}




void PlayWavFile(const char *fname)
{//================================
	char command[120];

#ifdef PLATFORM_WINDOWS
	wxSound(wxString(fname,wxConvLocal)).Play(wxSOUND_SYNC);
#else
	sprintf(command,"play %s",fname);
	if(system(command) == -1)
	{
		wxLogError(_T("Failed to run system command:\n\n"+wxString(command,wxConvLocal)));
	}
#endif
}


char *WavFileName(void)
{//====================
	static char f_speech[120];
	if(!wxDirExists(wxPathOnly(path_speech)))
	{
		path_speech = wxFileSelector(_T("Speech output file"),
			path_phsource,_T("speech.wav"),_T("*"),_T("*"),wxSAVE);
	}
	strcpy(f_speech,path_speech.mb_str(wxConvLocal));
	return(f_speech);
}


int OpenWaveFile2(const char *fname)
{//=================================
	int result;

	if((result = OpenWaveFile(fname, samplerate)) != 0)
	{
		wxLogStatus(_T("Can't write to WAV file: '"+wxString(fname,wxConvLocal))+_T("'"));
	}
	return(result);
}


void MakeWave2(PHONEME_LIST *p, int n_phonemes)
{//============================================
	int result;
	char *fname_speech;
	int n_ph;

	n_ph = n_phonemes;
	fname_speech = WavFileName();
	OpenWaveFile2(fname_speech);

	Generate(p,&n_ph,0);

	for(;;)
	{
		result = WavegenFile();
		if(result != 0)
			break;
		Generate(p,&n_ph,1);
	}

	CloseWaveFile(samplerate);
	PlayWavFile(fname_speech);

}  // end of MakeWave2




TranslDlg::TranslDlg(wxWindow *parent) : wxPanel(parent)
{//=====================================================

	t_source = new wxTextCtrl(this,T_SOURCE,_T(""),wxPoint(0,4),
		wxSize(200,250),wxTE_MULTILINE);
	t_phonetic = new wxTextCtrl(this,T_PHONETIC,_T(""),wxPoint(0,262),
		wxSize(200,350),wxTE_MULTILINE);

	t_translate = new wxButton(this,T_TRANSLATE,_T("Translate"),wxPoint(4,620));
	t_translate = new wxButton(this,T_RULES,_T("Show Rules"),wxPoint(4,652));
	t_process = new wxButton(this,T_PROCESS,_T("Speak"),wxPoint(100,620));

	t_source->SetFocus();
}  // end of TransDlg::TransDlg



void TranslDlg::SpeakFile(void)
{//============================
	wxString file;
	wxFileName fname;
	FILE *f_text;
	char buf[200];
	
	fname = wxFileName(path_speaktext);

	file = wxFileSelector(_T("Text file to speak"),fname.GetPath(),fname.GetName(),_T(""),_T("*"),wxOPEN);
	if(file == wxEmptyString) return;
	
	strcpy(buf,file.mb_str(wxConvLocal));
	f_text = fopen(buf,"r");
	if(f_text == NULL)
	{
		wxLogError(_T("Failed to read: ")+file);
		return;
	}
	
	path_speaktext = file;
	
	InitText(0);
	SpeakNextClause(f_text,NULL,0);
	return;
}  //  end of SpeakFile



void TranslDlg::ReadVoice()
{//========================
	wxString path;
	wxString filename;
	char fname[130];

	sprintf(fname,"%s/voices",path_home);
	path = wxFileSelector(_T("Load voice"),wxString(fname,wxConvLocal),_T(""),_T(""),_T("*"),wxOPEN);
	if(path.IsEmpty())
		return;

	filename = path.Mid(strlen(fname)+1);
	strcpy(fname,filename.mb_str(wxConvLocal));

	if(SetVoiceByName(fname) != EE_OK)
	{
		wxLogError(_T("Failed to load voice data"));
	}
	WavegenSetVoice(voice);
}


void TranslDlg::OnCommand(wxCommandEvent& event)
{//=============================================
	#define N_PH_LIST  N_PHONEME_LIST

	void *vp;
	char buf[500];
	char phon_out[N_PH_LIST*2];
	int clause_tone;
	int clause_count;
	FILE *f;
	char fname_temp[100];
	static int n_ph_list;
	static PHONEME_LIST ph_list[N_PH_LIST+1];

	option_phonemes = 0;

	switch(event.GetId())
	{
	case T_RULES:
	case MENU_SPEAK_RULES:
		option_phonemes = 2;
		strcpy(fname_temp,tmpnam(NULL));
		if((f = fopen(fname_temp,"w+")) != NULL)
		{
			f_trans = f;   // write translation rule trace to a temp file
		}
	case T_TRANSLATE:
	case MENU_SPEAK_TRANSLATE:
		SpeakNextClause(NULL,NULL,2);  // stop speaking file

		strncpy0(buf,t_source->GetValue().mb_str(wxConvLocal),sizeof(buf));
		phon_out[0] = 0;
		n_ph_list = 0;
		clause_count = 0;

		vp = buf;
		InitText(0);
		while((vp != NULL) && (n_ph_list < N_PH_LIST))
		{
			vp = translator->TranslateClause(NULL,vp,&clause_tone,NULL);

			if(clause_count++ > 0)
				strcat(phon_out," ||");
			strcat(phon_out,translator->phon_out);
			t_phonetic->SetValue(wxString(translator->phon_out,wxConvLocal));
			translator->CalcPitches(clause_tone);
			translator->CalcLengths();

			if((n_ph_list + n_phoneme_list) >= N_PH_LIST)
			{
				n_phoneme_list = N_PH_LIST - n_ph_list - n_phoneme_list;
			}

			memcpy(&ph_list[n_ph_list],phoneme_list,sizeof(PHONEME_LIST)*n_phoneme_list);
			n_ph_list += n_phoneme_list;
		}

		t_phonetic->Clear();
		if(option_phonemes == 2)
		{
			option_phonemes=0;
			rewind(f_trans);
			while(fgets(buf,sizeof(buf),f_trans) != NULL)
			{
				t_phonetic->AppendText(wxString(buf,wxConvLocal));
			}
			t_phonetic->AppendText(_T("---\n"));
			fclose(f_trans);
			remove(fname_temp);
		}
		t_phonetic->AppendText(wxString(phon_out,wxConvLocal));
// MbrolaTranslate(ph_list,n_ph_list,stdout);     // for Testing
		break;

	case T_PROCESS:
	case MENU_SPEAK_TEXT:
		if(prosodycanvas != NULL)
		{
			prosodycanvas->LayoutData(ph_list,n_ph_list);
		}
		else
		{
			myframe->OnProsody(event);
		}
		MakeWave2(ph_list,n_ph_list);
		break;
	}
}  // end of TranslDlg::OnCommand
