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

#include "wx/wx.h"


#include "wx/wfstream.h"
#include "wx/notebook.h"
#include "wx/image.h"
#include "wx/filename.h"
#include "wx/numdlg.h"
#include "wx/mdi.h"
#include "wx/laywin.h"
#include "wx/sashwin.h"
#include <locale.h>

#include "speak_lib.h"
#include "main.h"
#include "speech.h"
#include "voice.h"
#include "options.h"
#include "phoneme.h"
#include "synthesize.h"
#include "spect.h"
#include "translate.h"
#include "prosodydisplay.h"


static char *about_string = "espeakedit: %s\nAuthor: Jonathan Duddington (c) 2006";


const char *path_data = "/home/jsd1/speechdata/phsource";

extern void TestTest(int control);
extern void CompareLexicon(int);
extern void ConvertToUtf8();

extern void init_z();
extern void CompileInit(void);
extern void CompileMbrola();
extern void InitSpectrumDisplay();
extern void InitProsodyDisplay();
extern void InitWaveDisplay();

extern void VowelChart(int control, char *fname);
extern void MakeVowelLists(void);
extern void MakeWordFreqList();

extern const char *dict_name;
extern char voice_name[];

MyFrame *myframe = NULL;
SpectDisplay *currentcanvas = NULL;

ChildFrProsody *prosodyframe = NULL;
ProsodyDisplay *prosodycanvas = NULL;
wxNotebook *notebook = NULL;
wxProgressDialog *progress;
int progress_max;
int gui_flag = 0;

wxList my_children;

wxFont FONT_SMALL(8,wxSWISS,wxNORMAL,wxNORMAL);
wxFont FONT_MEDIUM(9,wxSWISS,wxNORMAL,wxNORMAL);
wxFont FONT_NORMAL(10,wxSWISS,wxNORMAL,wxNORMAL);

IMPLEMENT_APP(MyApp)

wxString AppName = _T("espeakedit");





int MyApp::OnExit()
{//================
	ConfigSave(1);
	return(0);
}


static const char *help_text =
"\n\nespeakedit\n"
"\tRun with GUI\n"
"espeakedit --compile\n"
"\tCompile phoneme data in espeak-data/phsource\n"
"\tand dictionary data in espeak-data/dictsource\n";


// Initialise this in OnInit, not statically
bool MyApp::OnInit(void)
{//=====================

int j;
wxChar *p;
char param[80];


if(argc > 1)
{
extern void VoiceReset(int control);

	p = argv[1];
	j = 0;
	while((param[j] = p[j]) != 0) j++;

	if((strcmp(param,"--help")==0) || (strcmp(param,"-h")==0))
	{
		printf(about_string,espeak_Info(NULL));
		printf(help_text);
		exit(0);
	}

	ConfigInit();
	VoiceReset(0);
	WavegenSetVoice(voice);
	WavegenInitSound();

	if(strcmp(param,"--compile")==0)
	{
		CompileInit();
		exit(0);
	}
	if(strcmp(param,"--lex=ru")==0)
	{
		if(LoadPhData() != 1)
		{
			fprintf(stderr,"Failed to load phoneme data\n");
			exit(1);
		}
		CompareLexicon(MENU_LEXICON_RU);
		exit(0);
	}
}

	gui_flag = 1;
	// It seems that the wctype functions don't work until the locale has been set
	// to something other than the default "C".  Then, not only Latin1 but also the
	// other characters give the correct results with iswalpha() etc.
	if(setlocale(LC_CTYPE,"en_US.UTF-8") == NULL)
		setlocale(LC_CTYPE,"");


  // Create the main frame window
  myframe = new MyFrame(NULL, -1, AppName, wxPoint(0, 0), wxSize(1024, 768),
                      wxDEFAULT_FRAME_STYLE |
                      wxNO_FULL_REPAINT_ON_RESIZE |
                      wxHSCROLL | wxVSCROLL);


  // Make a menubar
  myframe->SetMenuBar(MakeMenu(0));

  myframe->CreateStatusBar();

  myframe->Show(TRUE);

  SetTopWindow(myframe);
	wxInitAllImageHandlers();
//	wxImage::AddHandler(wxPNGHandler);
  return TRUE;
}

BEGIN_EVENT_TABLE(MyFrame, wxMDIParentFrame)
	EVT_CHAR(MyFrame::OnKey)
   EVT_MENU(MENU_ABOUT, MyFrame::OnAbout)
   EVT_MENU(MENU_SPECTRUM, MyFrame::OnNewWindow)
   EVT_MENU(MENU_SPECTRUM2, MyFrame::OnNewWindow)
   EVT_MENU(MENU_PROSODY, MyFrame::OnProsody)
   EVT_MENU(MENU_PARAMS, MyFrame::OnOptions)
   EVT_MENU(MENU_PATH1, MyFrame::OnOptions)
   EVT_MENU(MENU_PATH2, MyFrame::OnOptions)
   EVT_MENU(MENU_PATH3, MyFrame::OnOptions)
   EVT_MENU(MENU_COMPILE_PH, MyFrame::OnTools)
	EVT_MENU(MENU_COMPILE_DICT, MyFrame::OnTools)
	EVT_MENU(MENU_COMPILE_MBROLA, MyFrame::OnTools)
	EVT_MENU(MENU_CLOSE_ALL, MyFrame::OnQuit)
	EVT_MENU(MENU_QUIT, MyFrame::OnQuit)
	EVT_MENU(MENU_SPEAK_TRANSLATE, MyFrame::OnSpeak)
	EVT_MENU(MENU_SPEAK_RULES, MyFrame::OnSpeak)
	EVT_MENU(MENU_SPEAK_TEXT, MyFrame::OnSpeak)
	EVT_MENU(MENU_SPEAK_FILE, MyFrame::OnSpeak)
	EVT_MENU(MENU_SPEAK_STOP, MyFrame::OnSpeak)
	EVT_MENU(MENU_SPEAK_PAUSE, MyFrame::OnSpeak)
	EVT_MENU(MENU_SPEAK_VOICE, MyFrame::OnSpeak)
	EVT_MENU(MENU_LOAD_WAV, MyFrame::OnTools)
	EVT_MENU(MENU_VOWELCHART1, MyFrame::OnTools)
	EVT_MENU(MENU_VOWELCHART2, MyFrame::OnTools)
	EVT_MENU(MENU_VOWELCHART3, MyFrame::OnTools)
	EVT_MENU(MENU_LEXICON_RU, MyFrame::OnTools)
	EVT_MENU(MENU_TO_UTF8, MyFrame::OnTools)
	EVT_MENU(MENU_COUNT_WORDS, MyFrame::OnTools)
	EVT_MENU(MENU_TEST, MyFrame::OnTools)
	EVT_MENU(MENU_TEST2, MyFrame::OnTools)

	EVT_TIMER(1, MyFrame::OnTimer)
	EVT_SIZE(MyFrame::OnSize)
	EVT_SASH_DRAGGED_RANGE(ID_WINDOW_TOP, ID_WINDOW_BOTTOM, MyFrame::OnSashDrag)
END_EVENT_TABLE()


MyFrame::MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size,
    const long style):
  wxMDIParentFrame(parent, id, title, pos, size, style)
{//===================================================================================================================
// Main Frame constructor

	int error_flag = 0;
	int result;

  // Create some dummy layout windows
wxSashLayoutWindow *win;

  // Another window to the left of the client window
  win = new wxSashLayoutWindow(this, ID_WINDOW_LEFT2,
                               wxDefaultPosition, wxSize(200, 30),
                               wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
  win->SetDefaultSize(wxSize(208, 1000));
  win->SetOrientation(wxLAYOUT_VERTICAL);
  win->SetAlignment(wxLAYOUT_LEFT);
//  win->SetBackgroundColour(wxColour(0, 255, 255));
  win->SetSashVisible(wxSASH_RIGHT, TRUE);

  m_leftWindow2 = win;

	notebook = new wxNotebook(m_leftWindow2,-1);
//	notebook->AddPage(voicedlg,_T("Voice"),FALSE);
	formantdlg = new FormantDlg(notebook);
	notebook->AddPage(formantdlg,_T(" Spect"),FALSE);
	voicedlg = new VoiceDlg(notebook);

	transldlg = new TranslDlg(notebook);
	notebook->AddPage(transldlg,_T("Text"),TRUE);


	ConfigInit();
	WavegenInitSound();

	if((result = LoadPhData()) != 1)
	{
		if(result == -1)
			wxLogError(_T("Failed to load phoneme data,\nneeds espeak-data/phontab,phondata,phonindex"));
		else
			wxLogError(_T("Wrong version of espeak-data: 0x%x (expects 0x%x)"),result,version_phdata);

		error_flag = 1;
	}

	f_trans = stdout;
	option_ssml = 1;


//	if(LoadVoice(voice_name,0) == NULL)
	if(SetVoiceByName(voice_name) != EE_OK)
	{
		if(error_flag==0)
			wxLogError(_T("Failed to load voice data"));
		strcpy(dictionary_name,"en");
	}
	else
	{
		SetVoiceTitle(voice_name);
	}
	WavegenSetVoice(voice);

	SetParameter(espeakRATE,option_speed,0);

	SetSpeed(3);
	SynthesizeInit();

	InitSpectrumDisplay();
	InitProsodyDisplay();
//	InitWaveDisplay();
	espeak_ListVoices(NULL);

   m_timer.SetOwner(this,1);

   m_timer.Start(500);   /* 0.5 timer */

}  // end of MyFrame::MyFrame


void MyFrame::SetVoiceTitle(char *voice_name)
{//==========================================
	SetTitle(AppName + _T(" - ") + wxString(voice_name,wxConvLocal) + _T("  voice"));
}



void MyFrame::OnKey(wxKeyEvent& event)
{
	int key;

	key = event.GetKeyCode();

	if((currentcanvas != NULL) && (currentcanvas != FindFocus()))
	{
		if((key == WXK_F1) || (key == WXK_F2))
		{
			currentcanvas->OnKey(event);
			currentcanvas->SetFocus();
			return;
		}
	}

	event.Skip();
}

void MyFrame::OnTimer(wxTimerEvent &event)
//****************************************
{
   SynthOnTimer();
}


void MyFrame::OnQuit(wxCommandEvent& event)
{
	switch(event.GetId())
	{
	case MENU_QUIT:
		Close(TRUE);
		break;
	case MENU_CLOSE_ALL:
		break;
	}
}

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


void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{//===================================================
//CharsetToUnicode("ISO-8859-3");
//CharsetToUnicode("ISCII");
	char buf[120];

	sprintf(buf,about_string,espeak_Info(NULL));
	(void)wxMessageBox(wxString(buf,wxConvLocal), _T("About eSpeak Editor"));
}

void MyFrame::OnOptions(wxCommandEvent& event)
{
	wxString string;
	int value;

	switch(event.GetId())
	{
	case MENU_PARAMS:
		value = wxGetNumberFromUser(_T(""),_T(""),_T("Speed"),option_speed,80,320);
		if(value > 0)
		{
			option_speed = value;
			SetParameter(espeakRATE,option_speed,0);
			SetSpeed(3);
		}
		break;

	case MENU_PATH1:
		string = wxDirSelector(_T("Phoneme source directory"),path_phsource);
		if(!string.IsEmpty())
		{
			path_phsource = string;
		}
		break;

	case MENU_PATH2:
		string = wxDirSelector(_T("Dictionary source directory"),path_dictsource);
		if(!string.IsEmpty())
		{
			path_dictsource = string;
		}
		break;

	case MENU_PATH3:
		string = wxFileSelector(_T("Sound output file"),wxFileName(path_speech).GetPath(),
			_T(""),_T("WAV"),_T("*"),wxSAVE);
		if(!string.IsEmpty())
		{
			path_speech = string;
		}
		break;
	}

	ConfigSetPaths();
}


void MyFrame::OnTools(wxCommandEvent& event)
{//=========================================
	int err;
	FILE *log;
	char fname_log[80];
	char err_fname[130];

	switch(event.GetId())
	{
	case MENU_TEST:
		TestTest(0);
		break;

	case MENU_TEST2:
		TestTest(2);
		break;

	case MENU_TO_UTF8:
		ConvertToUtf8();
		break;

	case MENU_COUNT_WORDS:
		MakeWordFreqList();
		break;

	case MENU_LEXICON_RU:
		CompareLexicon(event.GetId());  // Compare a lexicon with _rules translation
		break;

	case MENU_COMPILE_PH:
		CompileInit();
		break;

	case MENU_COMPILE_MBROLA:
		CompileMbrola();
		break;
		
	case MENU_COMPILE_DICT:
		sprintf(fname_log,"%s%s",path_dsource,"dict_log");
		log = fopen(fname_log,"w");

		if((err = CompileDictionary(path_dsource,dictionary_name,log,err_fname)) < 0)
		{
			wxLogError(_T("Can't access file:\n")+wxString(err_fname,wxConvLocal));

			wxString dir = wxDirSelector(_T("Directory containing dictionary files"),path_dictsource);
			if(!dir.IsEmpty())
			{
				path_dictsource = dir;
				strncpy0(path_dsource,path_dictsource.mb_str(wxConvLocal),sizeof(path_dsource)-1);
				strcat(path_dsource,"/");
			}
			break;
		}
		wxLogStatus(_T("Compiled '")+wxString(dictionary_name,wxConvLocal)+_T("', %d errors"),err);

		if(log != NULL)
			fclose(log);
		break;

	case MENU_VOWELCHART1:
		MakeVowelLists();
		break;

	case MENU_VOWELCHART2:
		VowelChart(2,NULL);
		break;

	case MENU_VOWELCHART3:
		VowelChart(3,NULL);
		break;

	case MENU_LOAD_WAV:
//		LoadWavFile();
		break;
	}
}

extern wxMenu *speak_menu;

void MyFrame::OnSpeak(wxCommandEvent& event)
{//=========================================
	switch(event.GetId())
	{
	case MENU_SPEAK_TRANSLATE:
	case MENU_SPEAK_RULES:
	case MENU_SPEAK_TEXT:
		transldlg->OnCommand(event);
		break;

	case MENU_SPEAK_FILE:
		transldlg->SpeakFile();
		break;

	case MENU_SPEAK_STOP:
		SpeakNextClause(NULL,NULL,2);
		break;

	case MENU_SPEAK_PAUSE:
		SpeakNextClause(NULL,NULL,3);
		if(SynthStatus() & 2)
			speak_menu->SetLabel(MENU_SPEAK_PAUSE,_T("&Resume"));
		else
			speak_menu->SetLabel(MENU_SPEAK_PAUSE,_T("&Pause"));
		break;

	case MENU_SPEAK_VOICE:
		transldlg->ReadVoice();
		if(voicedlg != NULL)
			voicedlg->WriteParams();
		SetVoiceTitle(voice_name);
		break;
	}
}


void MyFrame::OnSashDrag(wxSashEvent& event)
{
    if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
        return;

    switch (event.GetId())
    {
        case ID_WINDOW_TOP:
        {
            m_topWindow->SetDefaultSize(wxSize(1000, event.GetDragRect().height));
            break;
        }
        case ID_WINDOW_LEFT1:
        {
            m_leftWindow1->SetDefaultSize(wxSize(event.GetDragRect().width, 1000));
            break;
        }
        case ID_WINDOW_LEFT2:
        {
            m_leftWindow2->SetDefaultSize(wxSize(event.GetDragRect().width, 1000));
            break;
        }
        case ID_WINDOW_BOTTOM:
        {
            m_bottomWindow->SetDefaultSize(wxSize(1000, event.GetDragRect().height));
            break;
        }
    }
    wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame(this);

    // Leaves bits of itself behind sometimes
    GetClientWindow()->Refresh();
}



void MyFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
    wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame(this);
}

// Note that SASHTEST_NEW_WINDOW and SASHTEST_ABOUT commands get passed
// to the parent window for processing, so no need to
// duplicate event handlers here.

BEGIN_EVENT_TABLE(MyChild, wxMDIChildFrame)
  EVT_MENU(SPECTSEQ_CLOSE, MyChild::OnQuit)
  EVT_MENU(SPECTSEQ_SAVE, MyChild::OnSave)
  EVT_MENU(SPECTSEQ_SAVEAS, MyChild::OnSaveAs)
  EVT_MENU(SPECTSEQ_SAVESELECT, MyChild::OnSaveSelect)
  EVT_MENU(SPECTSEQ_SAVEPITCH, MyChild::OnSavePitchenv)

  EVT_ACTIVATE(MyChild::OnActivate)
END_EVENT_TABLE()

MyChild::MyChild(wxMDIParentFrame *parent, const wxString& title, const wxPoint& pos, const wxSize& size,
const long style):
  wxMDIChildFrame(parent, -1, title, pos, size, style)
{
  canvas = NULL;
  my_children.Append(this);
}

MyChild::~MyChild(void)
{
	wxWindow *w;
	canvas = NULL;
#ifndef PLATFORM_WINDOWS
	// bug in wxMDIChildFrame, we need to explicitly remove the ChildFrame from the ClientWindow
	w = myframe->GetClientWindow();
	w->RemoveChild(this);
#endif
  my_children.DeleteObject(this);
}

//extern void CloseCanvas();

void MyChild::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Destroy();

}

void MyChild::OnSave(wxCommandEvent& WXUNUSED(event))
{
	canvas->Save(canvas->savepath);
}

void MyChild::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
	canvas->Save();
}

void MyChild::OnSaveSelect(wxCommandEvent& WXUNUSED(event))
{
	canvas->Save(_T(""),1);
}

void MyChild::OnSavePitchenv(wxCommandEvent& WXUNUSED(event))
{
	canvas->SavePitchenv(canvas->spectseq->pitchenv);
}

void MyChild::OnActivate(wxActivateEvent& event)
{
	if(canvas)
		canvas->OnActivate(event.GetActive());

		
}

