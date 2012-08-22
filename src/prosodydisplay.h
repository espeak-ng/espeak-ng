


#define FRAMEHEIGHT  120
#define LINEBASE  20
#define LINESEP   16

#define SCROLLUNITS 20




class ProsodyDisplay: public wxScrolledWindow
{//========================================
	public:
		ProsodyDisplay(wxWindow *parent, const wxPoint& pos, const wxSize& size);
		~ProsodyDisplay();
		
		virtual void OnDraw(wxDC& dc);
		void OnMouse(wxMouseEvent& event);
		void OnKey(wxKeyEvent& event);
		void OnMenu(wxCommandEvent& event);

      void Save(const wxString &path=_T(""));
		void LayoutData(PHONEME_LIST *phlist, int n_ph);

	private:
		void Redraw(wxDC& dc, int x1, int y1, int x2, int y2);
		void DrawEnv(wxDC& dc, int x1, int y1, int width, PHONEME_LIST *ph);
		void DrawPitchline(wxDC& dc, int line, int x1, int x2);
		void RefreshLine(int line);
		int  GetWidth(PHONEME_LIST *p);
		void  ChangePh(int pitch1, int pitch2);
		void SelectPh(int index);
		
		int linewidth;   // height for drawing pitch
		double scalex;  // scale length values
		double scaley;  // scale pitch values
		int selected_ph;

		int numph;
		PHONEME_LIST *phlist;
		int num_lines;
		short linetab[N_PHONEME_LIST/2];

	DECLARE_EVENT_TABLE()
};  //  end class ProsodyDisplay


class ChildFrProsody: public wxMDIChildFrame
{
  public:
    ProsodyDisplay *prosodycanvas;
    ChildFrProsody(wxMDIParentFrame *parent, const wxString& title, const wxPoint& pos, const wxSize& size, const long style);
    ~ChildFrProsody(void);
    void OnActivate(wxActivateEvent& event);

    void OnQuit(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

