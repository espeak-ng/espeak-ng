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



#include "wx/wx.h"


#include "speak_lib.h"
#include "speech.h"
#include "phoneme.h"
#include "main.h"
#include "synthesize.h"
#include "voice.h"
#include "translate.h"
#include "prosodydisplay.h"

#ifdef PLATFORM_POSIX
#include <unistd.h>
#endif


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
#define T_TRANSLATE_IPA 506

TranslDlg *transldlg = NULL;
extern char *speech_to_phonemes(char *data, char *phout);
extern ProsodyDisplay *prosodycanvas;
extern void Write4Bytes(FILE *f, int value);

BEGIN_EVENT_TABLE(TranslDlg, wxPanel)
	EVT_BUTTON(T_TRANSLATE,TranslDlg::OnCommand)
	EVT_BUTTON(T_PROCESS,TranslDlg::OnCommand)
	EVT_BUTTON(T_RULES,TranslDlg::OnCommand)
	EVT_BUTTON(T_TRANSLATE_IPA,TranslDlg::OnCommand)
END_EVENT_TABLE()


FILE *f_wave = NULL;
wxFont font_phonetic;
wxTextAttr style_phonetic;
wxFont font_phonetic_large;
wxTextAttr style_phonetic_large;


#ifdef deleted

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
#endif



void PlayWavFile(const char *fname)
{//================================
	char command[120];

#ifdef PLATFORM_WINDOWS
	wxSound(wxString(fname,wxConvLocal)).Play(wxSOUND_SYNC);
#else
//	wxSound(wxString(fname,wxConvLocal)).Play(wxSOUND_SYNC);  // This gives a click at the start of each play
	sprintf(command,"aplay %s",fname);
	if(system(command) == -1)
	{
		sprintf(command,"play %s",fname);
		if(system(command) == -1)
		{
			wxLogError(_T("Failed to run system command:\n\n"+wxString(command,wxConvLocal)));
		}
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



int OpenWaveFile2(const char *path)
/*********************************/
{
	// Set the length of 0x7fffffff for --stdout
	// This will be changed to the correct length for -w (write to file)
	static unsigned char wave_hdr[44] = {
		'R','I','F','F',0,0,0,0,'W','A','V','E','f','m','t',' ',
		0x10,0,0,0,1,0,1,0,  9,0x3d,0,0,0x12,0x7a,0,0,
		2,0,0x10,0,'d','a','t','a',  0xff,0xff,0xff,0x7f};

	if(path == NULL)
		return(2);

	if(strcmp(path,"stdout")==0)
		f_wave = stdout;
	else
		f_wave = fopen(path,"wb");

	if(f_wave != NULL)
	{
		fwrite(wave_hdr,1,24,f_wave);
		Write4Bytes(f_wave,samplerate);
		Write4Bytes(f_wave,samplerate * 2);
		fwrite(&wave_hdr[32],1,12,f_wave);
		return(0);
	}
	wxLogStatus(_T("Can't write to WAV file: '"+wxString(path,wxConvLocal))+_T("'"));
	return(1);
}   //  end of OpenWaveFile




void CloseWaveFile2()
/******************/
{
   unsigned int pos;

   if((f_wave == NULL) || (f_wave == stdout))
      return;

   fflush(f_wave);
   pos = ftell(f_wave);

	fseek(f_wave,4,SEEK_SET);
	Write4Bytes(f_wave,pos - 8);

	fseek(f_wave,40,SEEK_SET);
	Write4Bytes(f_wave,pos - 44);


   fclose(f_wave);
   f_wave = NULL;

} // end of CloseWaveFile




int WavegenFile2(void)
{//==================
	int finished;
	unsigned char wav_outbuf[1024];

	out_ptr = out_start = wav_outbuf;
	out_end = wav_outbuf + sizeof(wav_outbuf);

	finished = WavegenFill(0);

	if(f_wave != NULL)
	{
		fwrite(wav_outbuf, 1, out_ptr-wav_outbuf, f_wave);
	}
	return(finished);
}  // end of WavegenFile


void MakeWave2(PHONEME_LIST *p, int n_phonemes)
{//============================================
	int result;
	char *fname_speech;
	int n_ph;

#ifdef KLATT_TEST
	KlattInit();
#endif
	n_ph = n_phonemes;
	fname_speech = WavFileName();
	OpenWaveFile2(fname_speech);

	Generate(p,&n_ph,0);

	for(;;)
	{
		result = WavegenFile2();
		if(result != 0)
			break;
		Generate(p,&n_ph,1);
	}

	CloseWaveFile2();
	PlayWavFile(fname_speech);

}  // end of MakeWave2




TranslDlg::TranslDlg(wxWindow *parent) : wxPanel(parent)
{//=====================================================

	int height;
	int width;
	int x,y;
	int font_size;
	int height_ph = 350;


	wxTextAttr attr;
	wxFont font = wxFont(12,wxFONTFAMILY_ROMAN,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_LIGHT,false,_T(""),wxFONTENCODING_SYSTEM);

	attr.SetFont(font);

	wxClientDisplayRect(&x,&y,&width, &height);
#ifdef PLATFORM_WINDOWS
	if(height <= 768)
		height_ph = height - 416;
#else
	if(height <= 800)
		height_ph = 280;
#endif


	t_source = new wxTextCtrl(this,T_SOURCE,_T(""),wxPoint(0,4),
		wxSize(298,250),wxTE_MULTILINE,wxDefaultValidator,_T("Text input window"));
	t_source->SetDefaultStyle(attr);


	t_phonetic = new wxTextCtrl(this,T_PHONETIC,_T(""),wxPoint(0,262),
		wxSize(298,height_ph),wxTE_MULTILINE | wxTE_READONLY, wxDefaultValidator,_T("Phoneme translation window"));

	style_phonetic = t_phonetic->GetDefaultStyle();
	font_phonetic = style_phonetic.GetFont();
	font_size = font_phonetic.GetPointSize();
	font_phonetic_large = font_phonetic;
	style_phonetic_large = style_phonetic;
//font_phonetic_large.SetFamily(wxFONTFAMILY_SWISS);
	font_phonetic_large.SetPointSize(font_size+1);
	style_phonetic_large.SetFont(font_phonetic_large);

	y = height_ph + 270;
	t_translate = new wxButton(this,T_TRANSLATE,_T("Translate"),wxPoint(4,y));
	t_translate = new wxButton(this,T_RULES,_T("Show Rules"),wxPoint(4,y+32));
	t_translate = new wxButton(this,T_TRANSLATE_IPA,_T("Show IPA"),wxPoint(100,y+32));
	t_process = new wxButton(this,T_PROCESS,_T("Speak"),wxPoint(100,y));

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



void TranslDlg::ReadVoice(int variant)
{//===================================
	wxString path;
	wxString filename;
	char *p;
	char vname[40];
	char fname[sizeof(path_home)+10];

	if(variant)
	{
		// remove variant from the previous voice name
		if((p = strchr(voice_name2,'+')) != NULL)
			*p = 0;

		sprintf(fname,"%s/voices/!v",path_home);
		path = wxFileSelector(_T("Load voice variant"),wxString(fname,wxConvLocal),_T(""),_T(""),_T("*"),wxOPEN);
		if(path.IsEmpty())
		{
			strcpy(fname,voice_name2);
		}
		else
		{
			filename = path.Mid(strlen(fname)+1);
			strcpy(vname,filename.mb_str(wxConvLocal));
			sprintf(fname,"%s+%s",voice_name2,vname);
		}
	}
	else
	{
		sprintf(fname,"%s/voices",path_home);
		path = wxFileSelector(_T("Load voice"),wxString(fname,wxConvLocal),_T(""),_T(""),_T("*"),wxOPEN);
		if(path.IsEmpty())
			return;

		filename = path.Mid(strlen(fname)+1);
		strcpy(fname,filename.mb_str(wxConvLocal));
	}

	if(SetVoiceByName(fname) != EE_OK)
	{
		wxLogError(_T("Failed to load voice data"));
	}
	else
	{
		strcpy(voice_name2,fname);
	}
	WavegenSetVoice(voice);
}


void TranslDlg::OnCommand(wxCommandEvent& event)
{//=============================================
	#define N_PH_LIST  N_PHONEME_LIST

	void *vp;
	int translate_text = 0;
	char buf[1000];
	char phon_out[N_PH_LIST*2];
	int clause_tone;
	int clause_count;
	int use_ipa = 0;
	FILE *f;
	int  fd_temp;
	char fname_temp[100];
	static int n_ph_list;
	static PHONEME_LIST ph_list[N_PH_LIST+1];

	if(translator==NULL)
	{
		wxLogError(_T("Voice not set"));
		return;
	}

	option_phonemes = 0;

	switch(event.GetId())
	{
	case T_RULES:
	case MENU_SPEAK_RULES:
#ifdef PLATFORM_POSIX
		strcpy(fname_temp,"/tmp/espeakXXXXXX");
		if((fd_temp = mkstemp(fname_temp)) >= 0)
		{
			close(fd_temp);

			if((f = fopen(fname_temp,"w+")) != NULL)
			{
				f_trans = f;   // write translation rule trace to a temp file
			}
		}
#else
		strcpy(fname_temp,tmpnam(NULL));
		if((f = fopen(fname_temp,"w+")) != NULL)
		{
			f_trans = f;   // write translation rule trace to a temp file
		}
#endif
		t_phonetic->SetDefaultStyle(style_phonetic);
		translate_text = 2;
		break;

	case T_TRANSLATE:
	case MENU_SPEAK_TRANSLATE:
		t_phonetic->SetDefaultStyle(style_phonetic);
		translate_text = 1;
		break;

	case T_TRANSLATE_IPA:
	case MENU_SPEAK_IPA:
		t_phonetic->SetDefaultStyle(style_phonetic_large);

		translate_text = 3;
		use_ipa = 1;
		break;

	case T_PROCESS:
	case MENU_SPEAK_TEXT:
		if(prosodycanvas == NULL)
		{
			myframe->OnProsody(event);
		}
		prosodycanvas->LayoutData(ph_list,n_ph_list);
		option_phoneme_events = espeakINITIALIZE_PHONEME_EVENTS;
		option_log_frames = 1;
		MakeWave2(ph_list,n_ph_list);
		option_log_frames = 0;
		break;
	}

	if(translate_text)
	{
		option_phonemes = translate_text;

		option_multibyte = espeakCHARS_AUTO;
		SpeakNextClause(NULL,NULL,2);  // stop speaking file

		strncpy0(buf,t_source->GetValue().mb_str(wxConvUTF8),sizeof(buf));
		phon_out[0] = 0;
		n_ph_list = 0;
		clause_count = 0;

		vp = buf;
		InitText(0);
		while((vp != NULL) && (n_ph_list < N_PH_LIST))
		{
			vp = TranslateClause(translator,NULL,vp,&clause_tone,NULL);
			CalcPitches(translator,clause_tone);
			CalcLengths(translator);

			GetTranslatedPhonemeString(translator->phon_out, sizeof(translator->phon_out), use_ipa);
			if(clause_count++ > 0)
				strcat(phon_out," ||");
			strcat(phon_out,translator->phon_out);
			t_phonetic->SetValue(wxString(translator->phon_out,wxConvUTF8));

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
				t_phonetic->AppendText(wxString(buf,wxConvUTF8));
			}
			t_phonetic->AppendText(_T("---\n"));
			if(f_trans != NULL)
				fclose(f_trans);
			remove(fname_temp);
		}
		t_phonetic->AppendText(wxString(phon_out,wxConvUTF8));
	}
}  // end of TranslDlg::OnCommand
