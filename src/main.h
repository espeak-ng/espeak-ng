
#include "wx/toolbar.h"
#include "wx/laywin.h"
#include "wx/progdlg.h"

typedef unsigned short USHORT;
typedef unsigned char  UCHAR;



extern wxMenuBar *MakeMenu(int type);

extern wxString path_phsource;
extern wxString path_speaktext;
extern wxString path_speech;
extern wxProgressDialog *progress;
extern int progress_max;
extern int gui_flag;

extern char *WavFileName();

class SpectDisplay;


// Define a new application
class MyApp: public wxApp
{
	public:
	bool OnInit(void);
	int OnExit(void);
};

// Define a new frame
class MyFrame: public wxMDIParentFrame
{
  public:

    MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, const long style);

    void OnSize(wxSizeEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnOptions(wxCommandEvent& event);
	 void OnSpeak(wxCommandEvent& event);
    void OnTools(wxCommandEvent& event);
    void OnSashDrag(wxSashEvent& event);
    void OnKey(wxKeyEvent& event);
    void OnTimer(wxTimerEvent& event);
    void LoadWavFile(void);

    void OnNewWindow(wxCommandEvent& event);
    void OnProsody(wxCommandEvent& event);


protected:
    wxSashLayoutWindow* m_topWindow;
    wxSashLayoutWindow* m_leftWindow1;
    wxSashLayoutWindow* m_leftWindow2;
    wxSashLayoutWindow* m_bottomWindow;
	void SetVoiceTitle(char *voice_name);
    wxTimer m_timer;

DECLARE_EVENT_TABLE()
};

class MyChild: public wxMDIChildFrame
{
  public:
    SpectDisplay *canvas;
    MyChild(wxMDIParentFrame *parent, const wxString& title, const wxPoint& pos, const wxSize& size, const long style);
    ~MyChild(void);
    void OnActivate(wxActivateEvent& event);
	 
    void OnQuit(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnSaveSelect(wxCommandEvent& event);
    void OnSavePitchenv(wxCommandEvent& event);
//    void OnVoiceDlg(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};



class TranslDlg : public wxPanel
{//=============================
public:
	TranslDlg(wxWindow *parent);

	void OnKey(wxKeyEvent& event);
	void SpeakFile(void);
	void ReadVoice(void);
	void OnCommand(wxCommandEvent& event);

private:
	wxButton *t_translate;
	wxButton *t_process;
	wxTextCtrl* t_source;
	wxTextCtrl* t_phonetic;


	DECLARE_EVENT_TABLE()
};

extern MyFrame *myframe;
extern TranslDlg *transldlg;
extern wchar_t ipa1[256];



extern wxFont FONT_SMALL;
extern wxFont FONT_MEDIUM;
extern wxFont FONT_NORMAL;

enum {
	MENU_QUIT = 1,
	MENU_SPECTRUM,
	MENU_SPECTRUM2,
	MENU_CLOSE_ALL,
	MENU_ABOUT,

	MENU_OPTIONS,
	MENU_PARAMS,
	MENU_PATHS,
	MENU_PATH1,
	MENU_PATH2,
	MENU_PATH3,
	MENU_PATH4,

	MENU_PROSODY,
	MENU_COMPILE_PH,
	MENU_COMPILE_DICT,
	MENU_COMPILE_MBROLA,

	MENU_SPEAK_TRANSLATE,
	MENU_SPEAK_RULES,
	MENU_SPEAK_TEXT,
	MENU_SPEAK_FILE,
	MENU_SPEAK_PAUSE,
	MENU_SPEAK_STOP,
	MENU_SPEAK_VOICE,
	MENU_LOAD_WAV,

	MENU_VOWELCHART,
	MENU_VOWELCHART1,
	MENU_VOWELCHART2,
	MENU_VOWELCHART3,
	MENU_LEXICON,
	MENU_LEXICON_RU,
	MENU_LEXICON_DE,
	MENU_TO_UTF8,
	MENU_COUNT_WORDS,
	MENU_TEST,
	MENU_TEST2,

	SPECTSEQ_CLOSE,
	SPECTSEQ_SAVE,
	SPECTSEQ_SAVEAS,
	SPECTSEQ_SAVESELECT,
	SPECTSEQ_SAVEPITCH,
	SPECTSEQ_VOICEDLG
};

#define ID_WINDOW_TOP       100
#define ID_WINDOW_LEFT1     101
#define ID_WINDOW_LEFT2     102
#define ID_WINDOW_BOTTOM    103



