#include "wx/spinctrl.h"
#include "wx/notebook.h"

#define PROGRAM_NAME _T("Voice Editor")

#define FRAME_WIDTH  1000  // max width for 8000kHz frame
#define MAX_DISPLAY_FREQ 9500
#define FRAME_HEIGHT 240


#define T_ZOOMOUT  301
#define T_ZOOMIN   302
#define T_USEPITCHENV 303
#define T_SAMPRATE 304
#define T_PITCH1   305
#define T_PITCH2   306
#define T_DURATION 307
#define T_AMPLITUDE 308
#define T_AMPFRAME  309
#define T_TIMEFRAME 310
#define T_TIMESEQ   311

#define FILEID1_SPECTSEQ 0x43455053
#define FILEID2_SPECTSEQ 0x51455354

#define FILEID1_SPC2     0x32435053  // an old format for spectrum files

#define FILEID1_PITCHENV 0x43544950
#define FILEID2_PITCHENV 0x564e4548

#define FILEID1_PRAATSEQ 0x41415250
#define FILEID2_PRAATSEQ 0x51455354


class MyFrame;

typedef struct {
	unsigned short pitch1;
	unsigned short pitch2;
	unsigned char env[128];
} PitchEnvelope;

typedef struct {
   short freq;
   short bandw;
}  formant_t;


//===============================================================================================
// Old "SPC2" format

typedef struct {
   unsigned char  freq;      /* *factor for f0-f7 of [4,5,11,20,20,25,32,32] */
   unsigned char  height;
   unsigned char  width_l;   /* * 8 Hz */
   unsigned char  width_r;   /* * 8 Hz */
   } PEAKS7;

/* flags: bit 0      label is present in data[]
          bit 1      mark cycle - blue background
          bit 2      mark cycle - green square
          bit 3      mark cycle - 'lengthen' indicator
          bit 4      mark cycle - 'standard'
          bit 7      hide peaks
*/

typedef struct {
   short pitch;     /* pitch of fundamental, 1/16 Hz */
   short length;    /* length in 1/15625 sec */
   unsigned char  n_harm;    /* number of harmonic data, h1 ... hn */
   unsigned char  flags;     /* bits 0 */
   unsigned char  aspiration;
   unsigned char  hf_voicing;
   unsigned char  spare1;
   unsigned char  spare2;
   unsigned char  spare3;
   unsigned char  spare4;
   PEAKS7 peak_data[7];
   unsigned char  data[1];   /* firstly harmonic data, then f3,f4,f5 data */
   } CYCLE;         /* VARIABLE LENGTH Structure */
	
extern int SPC2_size_cycle(CYCLE *cy);
//==============================================================================================



class SpectFrame
{//=============
public:
	SpectFrame(SpectFrame *copy=NULL);
	~SpectFrame();
   int Import(wxInputStream &stream);
   int ImportSPC2(wxInputStream &stream, float &time_acc);
	int Load(wxInputStream &stream);
	int Save(wxOutputStream &stream);
   void Draw(wxDC &dc, int offy, int frame_width, double scalex, double scaley);

	void ZeroPeaks();
	void CopyPeaks(SpectFrame *sf);
	void ToggleMarker(int n);
	void ApplyVoiceMods();
	
	void MakeWave(int peaks, PitchEnvelope &pitch, int amplitude, int duration);
	void MakeHtab(int numh, int *htab, int pitch);
	double GetRms(int amp);

	int selected;
	int keyframe;
	int amp_adjust;
	float length_adjust;
	double rms;

   float time;
   float pitch;
   float length;
   float dx;
   int  nx;
   int  markers;
	int  max_y;
   USHORT *spect;    // sqrt of harmonic amplitudes,  1-nx at 'pitch'

   formant_t formants[N_PEAKS];   // this is just the estimate given by Praat
   peak_t peaks[N_PEAKS];

private:
   void DrawPeaks(wxDC *dc, int offy, int frame_width, int seq_amplitude, double scalex);

};

class SpectSeq
{//===========
public:
	SpectSeq(int nframes=0);
	~SpectSeq();
	int Import(wxInputStream& stream);
	int ImportSPC2(wxInputStream& stream);
	int Save(wxOutputStream& stream, int selection);
	int Load(wxInputStream& stream);
	void Draw(wxDC &dc, int start_y, int end_y);
	void MakePitchenv(PitchEnvelope &pitch);
	void MakeWave(int start, int end, PitchEnvelope &pitch);

	void InterpolatePeaks(int on);
	void InterpolateAdjacent(void);
	void CopyDown(int frame, int direction);
	void SelectAll(int yes);
	int  CountSelected();
	void DeleteSelected();
	void ClipboardCopy();
	int  ClipboardInsert(int insert_at);
	float GetFrameLength(int frame, int plus, int *original_mS);
	float GetKeyedLength();
	void SetFrameLengths();
	void ConstructVowel(void);
      
	int  numframes;
	int  amplitude;
	int  spare;
	wxString name;

	SpectFrame **frames;
	PitchEnvelope pitchenv;
	int  pitch1;
	int  pitch2;
	int  duration;
	int  grid;
	int  bass_reduction;
	int max_x;
	int max_y;

private:
	void Load2(wxInputStream& stream, int import, int n);
	void InterpolatePeak(int peak);
	void ApplyAmp_adjust(SpectFrame *sp, peak_t *peaks);
	double scalex;
	double scaley;

};


class SpectDisplay: public wxScrolledWindow
{//========================================
	public:
		SpectDisplay(wxWindow *parent, const wxPoint& pos, const wxSize& size, SpectSeq *spect);
		~SpectDisplay();
		virtual void OnDraw(wxDC& dc);
		void OnMouse(wxMouseEvent& event);
		void OnKey(wxKeyEvent& event);
		void OnActivate(int active);
		void OnMenu(wxCommandEvent& event);

      void Save(const wxString &path=_T(""),int selection=0);
      void SavePitchenv(PitchEnvelope &pitch);

		void OnZoom(int command);

		SpectSeq *spectseq;
		float zoomx;
		float zoomy;
		int zoom;
		wxString savepath;
 		void ReadDialogValues();
		void WriteDialogValues();
		void WriteDialogLength();
		void RefreshDialogValues(int type);
		int ScrollToFrame(int frame, int centre);
		void SelectFrame(int frame);

	private:
		void RefreshFrame(int frame);
		void SetKeyframe(SpectFrame *sf, int yes);
		void PlayChild(int number, PitchEnvelope pitchenv);
		void SetExtent();
		int sframe;
		int pk_num;


    DECLARE_EVENT_TABLE()
};


class ByteGraph: public wxScrolledWindow
{//=====================================
public:
	ByteGraph(wxWindow *parent, const wxPoint& pos, const wxSize &size);
   virtual void OnDraw(wxDC &dc);
	void SetData(int nx, unsigned char *data);
	void ShowSpectrum(int yes);
private:
	int npoints;
	unsigned char *graph;
	int show_spectrum;
	double spectrum_scale;
	void DrawSpectrum(wxDC &dc);
	void OnMouse(wxMouseEvent& event);

	DECLARE_EVENT_TABLE()
};


class FormantDlg : public wxPanel
{//==============================
	public:
		FormantDlg(wxWindow *parent);
		void ShowFrame(SpectSeq *spectseq, int frame, int pk, int field);
      void OnCommand(wxCommandEvent& event);
      void OnSpin(wxSpinEvent& event);

		wxCheckBox *usepitchenv;

		wxSpinCtrl *t_amplitude;
		wxSpinCtrl *t_ampframe;
		wxSpinCtrl *t_timeframe;
		wxSpinCtrl *t_timeseq;
      wxStaticText *t_pitch;
		wxStaticText *t_orig_frame;
		wxStaticText *t_orig_seq;

	private:
		wxStaticText *t_lab[10];
      wxStaticText *t_labpk[N_PEAKS];
		wxTextCtrl *t_pkfreq[N_PEAKS];
		wxTextCtrl *t_pkheight[N_PEAKS];
		wxTextCtrl *t_pkwidth[N_PEAKS];
		wxTextCtrl *t_pkright[N_PEAKS];
		wxRadioButton *t_select_peak[N_PEAKS];

		wxButton *t_zoomout;
		wxButton *t_zoomin;

	DECLARE_EVENT_TABLE()
};



class VoiceDlg : public wxPanel
{//=============================
public:
	VoiceDlg(wxWindow *parent);
	void ReadParams();
	void WriteParams();
	void Save();
	void Load();
	void SetFromSpect(SpectSeq *spect);

	voice_t *voice_p;
	PitchEnvelope pitchenv;
	wxSpinCtrl* vd_pitch1;
	wxSpinCtrl* vd_pitch2;
	wxTextCtrl* vd_duration;

private:
	wxButton *vd_defaults;
	wxButton *vd_load;
	wxButton *vd_save;

	wxStaticText* vd_labpk[N_PEAKS+1];
	wxSpinCtrl* vd[N_PEAKS+1][3];
	wxCheckBox* vd_usepitch;

	wxStaticText* vd_lab[3];
	ByteGraph *vd_pitchgraph;
	wxComboBox *vd_pitchchoice;

	int pitch1;
	int pitch2;

	void OnCommand(wxCommandEvent& event);
	void OnSpin(wxSpinEvent& event);

	DECLARE_EVENT_TABLE()
};



extern SpectSeq *clipboard_spect;
extern int pk_select;
extern int samplerate;
extern unsigned char env_fall[];
extern FormantDlg* formantdlg;
extern VoiceDlg* voicedlg;
extern wxNotebook* notebook;
extern ByteGraph* pitchgraph;
extern SpectDisplay *currentcanvas;
extern float polint(float xa[],float ya[],int n,float x);
extern void WavegenInit(int samplerate, int wavemult_fact);
extern void WavegenInitPkData(int);  // initialise envelope data
extern void SetPitch(int length, unsigned char *env, int pitch1, int pitch2);
extern void SetSynthHtab(int length_mS, USHORT *ht1, int nh1, float pitch1, USHORT *ht2, int nh2, float pitch2);
extern void MakeWaveFile();
extern void MakeWaveBuf(int length, UCHAR *buf);

extern int OpenWaveFile(const char *path, int samplerate);
extern int OpenWaveFile2(const char *path);
extern void CloseWaveFile(int rate);
extern void PlayWavFile(const char *fname);

extern void SetSpinCtrl(wxSpinCtrl *t, int value);
extern int GetNumeric(wxTextCtrl *t);
extern void SetNumeric(wxTextCtrl *t, int value);


