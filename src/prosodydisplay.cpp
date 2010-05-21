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



#include "wx/wx.h"
#include "wx/numdlg.h"

#include "speak_lib.h"
#include "main.h"
#include "speech.h"
#include "phoneme.h"
#include "synthesize.h"
#include "prosodydisplay.h"
#include "translate.h"

extern MyFrame *myframe;
extern ChildFrProsody *prosodyframe;
extern ProsodyDisplay *prosodycanvas;

wxMenu *menu_prosody;


BEGIN_EVENT_TABLE(ProsodyDisplay, wxScrolledWindow)
	EVT_LEFT_DOWN(ProsodyDisplay::OnMouse)
	EVT_RIGHT_DOWN(ProsodyDisplay::OnMouse)
	EVT_CHAR(ProsodyDisplay::OnKey)
	EVT_MENU(-1, ProsodyDisplay::OnMenu)
END_EVENT_TABLE()


static wxPen PEN_PITCHENV(wxColour(0,0,255),1,wxSOLID);
static wxPen PEN_SAMPLED(wxColour(0,200,200),2,wxSOLID);
static wxPen PEN_PHSELECTED(wxColour(255,0,0),2,wxSOLID);

ProsodyDisplay::ProsodyDisplay(wxWindow *parent, const wxPoint& pos, const wxSize& size)
        : wxScrolledWindow(parent, -1, pos, size,
                           wxSUNKEN_BORDER | wxNO_FULL_REPAINT_ON_RESIZE)
{//=====================================================================

	linewidth = size.GetWidth();
	scalex = 0.5;
	scaley = double(LINESEP*6)/160.0;
	selected_ph = -1;
//	SetBackgroundColour(* wxWHITE);

	LayoutData(phoneme_list,n_phoneme_list);
}  // end of ProsodyDisplay::ProsodyDisplay


ProsodyDisplay::~ProsodyDisplay()
{//==========================
	prosodycanvas = NULL;
}


void InitProsodyDisplay()
{//======================
	wxMenu *menu_envelopes;

	menu_envelopes = new wxMenu;
	// entries match those in envelope_data[] in intonation.cpp
	menu_envelopes->Append(0x100,_T("Fall"));
	menu_envelopes->Append(0x102,_T("Rise"));
	menu_envelopes->Append(0x104,_T("Fall-rise"));
	menu_envelopes->Append(0x105,_T("Fall-rise (R)"));
	menu_envelopes->Append(0x106,_T("Fall-rise 2"));
	menu_envelopes->Append(0x107,_T("Fall-rise 2(R)"));
	menu_envelopes->Append(0x108,_T("Rise-fall"));

	menu_envelopes->Append(0x10a,_T("Fall-rise 3"));
	menu_envelopes->Append(0x10c,_T("Fall-rise 4"));
	menu_envelopes->Append(0x10e,_T("Fall 2"));
	menu_envelopes->Append(0x110,_T("Rise 2"));
	menu_envelopes->Append(0x112,_T("Rise-fall-rise"));

	menu_prosody = new wxMenu;
	menu_prosody->Append(1,_T("Pitch envelope"),menu_envelopes);
	menu_prosody->Append(2,_T("Amplitude"));
	menu_prosody->Append(3,_T("Length"));
	menu_prosody->Append(4,_T("Play	F2"));
}


void ProsodyDisplay::RefreshLine(int line)
{//=====================================
	int x,y;

	CalcScrolledPosition(0,line*FRAMEHEIGHT,&x,&y);
	RefreshRect(wxRect(0,y,linewidth,FRAMEHEIGHT));
}


int ProsodyDisplay::GetWidth(PHONEME_LIST *p)
{//========================================
	int  w;
	
	if(p->ph == NULL)
		return(0);

	w = (p->ph->std_length * 2);
	if(w == 0)  w = 60;

	if(p->length != 0)
		w = (w * p->length) / 256;

	return(int((w + p->prepause)* scalex) + 1);
}


void ProsodyDisplay::SelectPh(int index)
{//=====================================
	// A phoneme has been selected
	PHONEME_LIST *p;
	const char *emphasized;
	int y1, y2;
	char buf[120];

	if(index < 0) return;

	p = &phlist[index];
	if((p == NULL) || (p->ph == NULL)) return;

	emphasized = "";
	if(p->stresslevel & 8)
		emphasized = "*";

	y1 = p->pitch1;
	y2 = p->pitch2;
	sprintf(buf,"Stress %s%d   Amp %2d   LenMod %2d   Pitch %3d %3d [env=%d]   Flags %.2x ",
		emphasized,p->stresslevel&0x7,p->amp,p->length,y1,y2,p->env,p->ph->phflags);
	wxLogStatus(wxString(buf,wxConvLocal));
}


void ProsodyDisplay::ChangePh(int pitch1, int pitch2)
{//================================================
	PHONEME_LIST *p;
	int sign1;
	int sign2;

	if(selected_ph < 0)
		return;

	p = &phlist[selected_ph];
	sign1 = p->pitch1 - p->pitch2;
	p->pitch1 += pitch1;
	p->pitch2 += (pitch1 + pitch2);
	sign2 = p->pitch1 - p->pitch2;

	if((sign1 != 0) && ((sign1 * sign2) <= 0))
	{
		// change of sign, change rise to fall
		if(p->env == 1)
			p->env = 0;
		else
		if(p->env == 0)
			p->env = 1;
	}
}


void ProsodyDisplay::OnMenu(wxCommandEvent& event)
{//===============================================
	int id;
	int value;
	PHONEME_LIST *p;

	id = event.GetId();
	p = &phlist[selected_ph];

	if((id & 0xf00) == 0x100)
	{
		// selected a pitch envelope
		p->env = id - 0x100;
	}

	switch(id)
	{
	case 2:
		value = wxGetNumberFromUser(_T(""),_T("Amplitude"),_T(""),p->amp,0,40);
		if(value >= 0)
			p->amp = value;
		break;
	case 3:
		value = wxGetNumberFromUser(_T(""),_T("Length"),_T(""),p->length,1,500);
		if(value >= 0)
			p->length = value;
		break;
	case 4:
		MakeWave2(phlist,numph);
		break;
	}
	SelectPh(selected_ph);
	Refresh();
}


void ProsodyDisplay::OnMouse(wxMouseEvent& event)
{//============================================
	int line;
	int ix;
	int xpos=0;


	wxClientDC dc(this);
	PrepareDC(dc);
	wxPoint pt(event.GetLogicalPosition(dc));

	if(selected_ph >= 0)
	{
		// find line for previously selected phoneme
		for(line=0; line<num_lines; line++)
			if(linetab[line+1] > selected_ph) break;
		RefreshLine(line);
		selected_ph = -1;
	}

	line = pt.y / FRAMEHEIGHT;

	// find which phoneme is selected on this line
	for(ix=linetab[line]; (ix<linetab[line+1]) && (ix<numph); ix++)
	{
		xpos += GetWidth(&phlist[ix]);
		if(xpos > pt.x)
		{
			selected_ph = ix;
			SelectPh(selected_ph);
			break;
		}
	}

	RefreshLine(line);

	if(event.RightDown())
	{
		PopupMenu(menu_prosody);
	}

}  // end of ProsodyDisplay::OnMouse


void ProsodyDisplay::OnKey(wxKeyEvent& event)
{//========================================
	PHONEME_LIST *p;
	int display=1;
	
	if(selected_ph < 0)
		selected_ph = 0;

	p = &phlist[selected_ph];

	switch(event.GetKeyCode())
	{
	case WXK_F2:
		// make and play from this clause
		MakeWave2(phlist,numph);
		break;

	case WXK_LEFT:
		if(selected_ph > 1)
			selected_ph--;
		break;

	case WXK_RIGHT:
		if(selected_ph < (numph-2))
			selected_ph++;
		break;
		
	case WXK_UP:
		if(event.ControlDown())
			ChangePh(-1,2);
		else
			ChangePh(1,0);
		display = 1;
		break;

	case WXK_DOWN:
		if(event.ControlDown())
			ChangePh(1,-2);
		else
			ChangePh(-1,0);
		break;

	case ',':
	case '<':
		if(p->length > 0)
			p->length--;
		break;

	case '.':
	case '>':
		p->length++;
		break;

	case WXK_TAB:
		display = 0;
		event.Skip();
	transldlg->SetFocus();
		break;

	default:
		display = 0;
		event.Skip();
		break;
	}

	if(display)
	{
		Refresh();
		SelectPh(selected_ph);
	}
}  // end of ProsodyDisplay::OnKey


void ProsodyDisplay::DrawEnv(wxDC& dc, int x1, int y1, int width, PHONEME_LIST *ph)
{//==============================================================================
	int  pitchr;
	int  pitch;
	int  p1;
	int  ix;
	int  x,y;
	int  y2=0;
	unsigned char *env;
	PHONEME_DATA phdata_tone;

	if(width <= 0) return;
	
	if((pitchr = ph->pitch2 - ph->pitch1) < 0)
	{
		pitchr = -pitchr;
		p1 = ph->pitch2;
	}
	else
	{
		p1 = ph->pitch1;
	}


	dc.SetPen(PEN_PITCHENV);

	env = envelope_data[ph->env];
	if((ph->type == phVOWEL) && (ph->tone_ph != 0))
	{
		// the envelope is given by a Tone phoneme acting on this vowel
		InterpretPhoneme2(ph->tone_ph, &phdata_tone);
		env = LookupEnvelope(phdata_tone.pitch_env);
	}

	for(ix=0; ix<=width; ix+=4)
	{
		x = int((ix * 127.9)/width);
		pitch = p1 + (pitchr * env[x])/256;
		y = y1-int(pitch * scaley);
		if(ix > 0)
			dc.DrawLine(x1+ix-4,y2,x1+ix,y);
		y2 = y;
	}
}  // end of DrawEnv



static void GetPhonemeName(PHONEME_TAB *ph, wxString& string)
{//==========================================================
	string = wxString(WordToString(ph->mnemonic),wxConvLocal);
}


void ProsodyDisplay::DrawPitchline(wxDC& dc, int line, int x1, int x2)
{//=================================================================
	int  ix;
	int  endix;
	int  y;
	int  offy;
	int  xpos;
	int  width;    // total width, including pre-pause
	int  width2;   // width without pre-pause
	int  width_env;
	int  textwidth, textheight;
	wxString string;
	PHONEME_LIST *p;

	if(linetab[line] >= numph) return;

	offy = (line+1) * FRAMEHEIGHT - 1;

	y = LINEBASE+LINESEP;
	dc.SetPen(*wxLIGHT_GREY_PEN);
	for(ix=0; ix<5; ix++)
	{
		dc.DrawLine(0,offy-y,linewidth,offy-y);
		y += LINESEP;
	}

	endix = linetab[line+1];

	xpos = 0;
	for(ix=linetab[line]; ix<endix; ix++)
	{
		if(ix < 0 || ix > numph-2) break;
		if(xpos > x2) break;  // past the redraw region

		p = &phlist[ix];

		width = GetWidth(p);
		width2 = width - int(p->prepause * scalex) - 1;

		if(xpos+width < x1)
		{
			xpos += width;
			continue;    // before the redraw region
		}

		// is this phoneme selected ?
		if(ix == selected_ph)
		{
			dc.SetPen(PEN_PHSELECTED);
			dc.DrawLine(xpos,offy-LINEBASE,xpos+width,offy-LINEBASE);
		}

		// draw separator bar
		if(p->newword)
			dc.SetPen(*wxBLACK_PEN);   // word boundary
		else
			dc.SetPen(*wxLIGHT_GREY_PEN);

		dc.DrawLine(xpos,offy-LINEBASE,xpos,offy-LINEBASE-LINESEP*6);

		// draw pitch envelope
		if(((p->ph->phflags & phWAVE) == 0)  && (p->ph->type != phPAUSE))
		{
			if(!(p->synthflags & SFLAG_SEQCONTINUE))
			{
				width_env = width2;
				if(phlist[ix+1].synthflags & SFLAG_SEQCONTINUE)
					width_env += GetWidth(&phlist[ix+1]);
				DrawEnv(dc,xpos+1+(width-width2),offy-LINEBASE,width_env,p);
			}
		}
		else
		if(p->type != phPAUSE)
		{
			// sampled sound, draw a flat line
			dc.SetPen(PEN_SAMPLED);
			dc.DrawLine(xpos+1+(width-width2),offy-LINEBASE-LINESEP,
				xpos+1+width,offy-LINEBASE-LINESEP);
		}

		// show phoneme name from the PHONEME_TAB
		GetPhonemeName(p->ph,string);
		dc.GetTextExtent(string,&textwidth,&textheight);
		dc.DrawText(string,xpos+(width-textwidth/2)/2, offy-LINEBASE);

		xpos += width;
	}

	// draw the separator bar at the end of the line
	if(ix==endix && ix<numph && phlist[ix].newword)
		dc.SetPen(*wxLIGHT_GREY_PEN);
	else
		dc.SetPen(*wxBLACK_PEN);  // word boundary or end of list
	dc.DrawLine(xpos,offy-LINEBASE,xpos,offy-LINEBASE-LINESEP*6);

}  // end of ProsodyDisplay::DrawPitchline


void ProsodyDisplay::OnDraw(wxDC& dc)
{//================================
	int x1,y1;
	int vX,vY,vW,vH;        // Dimensions of client area in pixels

	int line, start, end;

	wxRegionIterator upd(GetUpdateRegion()); // get the update rect list

	while (upd)
	{
		vX = upd.GetX();
		vY = upd.GetY();
		vW = upd.GetW();
		vH = upd.GetH();

		CalcUnscrolledPosition(vX,vY,&x1,&y1);

		// Repaint this rectangle, find which lines to redraw
		start = y1/FRAMEHEIGHT;
		end = (y1+vH)/FRAMEHEIGHT;

		for(line=start; line<=end && line<num_lines; line++)
			DrawPitchline(dc,line,x1,x1+vW);

		upd ++ ;
	}
}  // end of ProsodyDisplay::OnDraw


void ProsodyDisplay::LayoutData(PHONEME_LIST *ph_list, int n_ph)
{//===========================================================
	// divide the phoneme list into lines for display
	int xpos;
	int w;
	int ix;
	PHONEME_LIST *p;



	numph = n_ph;
	phlist = ph_list;

	num_lines = 0;
	linetab[0] = 1;
	xpos = linewidth;

	// could improve this to do 'wordwrap' - only split on word boundary
	// or unvoiced phonemes

	for(ix=1; ix<numph-2; ix++)
	{
		p = &phlist[ix];
		w = GetWidth(p);

		if(w + xpos >= linewidth)
		{
			linetab[num_lines++] = ix;
			xpos = 0;
		}
		xpos += w;
	}
	linetab[num_lines]=numph-2;

	SetScrollbars(SCROLLUNITS,SCROLLUNITS,linewidth/SCROLLUNITS,
		(num_lines*FRAMEHEIGHT)/SCROLLUNITS+1);

	Refresh();
}  // end of ProsodyDisplay::LayoutData



void MyFrame::OnProsody(wxCommandEvent& WXUNUSED(event))
{//=====================================================

	// Open the Prosody display window
	// Make another frame, containing a canvas

	if(prosodyframe != NULL)
	{
		// The Prosody window is already open
		prosodyframe->Activate();
		return;
	}

	prosodyframe = new ChildFrProsody(myframe, _T(""),
                                      wxPoint(10, 200), wxSize(1000, 300),
                                      wxDEFAULT_FRAME_STYLE |
                                      wxNO_FULL_REPAINT_ON_RESIZE);

	prosodyframe->SetTitle(_T("Prosody"));

	// Give it a status line
	prosodyframe->CreateStatusBar();

	int width, height;
	wxMDIClientWindow *clientwin = this->GetClientWindow();
	clientwin->GetClientSize(&width, &height);

#ifdef deleted
	wxPanel *panel = new wxPanel(prosodyframe,-1,wxPoint(0,0), wxSize(width,50));
	ProsodyDisplay *canvas = new ProsodyDisplay(prosodyframe, wxPoint(0, 50), wxSize(width-2, height-50));
#else
	ProsodyDisplay *canvas = new ProsodyDisplay(prosodyframe, wxPoint(0, 50), wxSize(width-10, height));
#endif

	prosodycanvas = canvas;
	
	// Associate the menu bar with the frame
	prosodyframe->SetMenuBar(MakeMenu(2));
	prosodyframe->prosodycanvas = canvas;
	prosodyframe->Show(TRUE);

}

BEGIN_EVENT_TABLE(ChildFrProsody, wxMDIChildFrame)
  EVT_MENU(SPECTSEQ_CLOSE, ChildFrProsody::OnQuit)

//  EVT_ACTIVATE(ChildFrProsody::OnActivate)
END_EVENT_TABLE()

extern wxList my_children;

ChildFrProsody::ChildFrProsody(wxMDIParentFrame *parent, const wxString& title, const wxPoint& pos, const wxSize& size,
const long style):
  wxMDIChildFrame(parent, -1, title, pos, size, style)
{
	my_children.Append(this);
}

ChildFrProsody::~ChildFrProsody(void)
{
	wxWindow *w;
	my_children.DeleteObject(this);
	prosodycanvas = NULL;
	prosodyframe = NULL;

#ifndef PLATFORM_WINDOWS
	// bug in wxMDIChildFrame, we need to explicitly remove the ChildFrame from the ClientWindow
	w = myframe->GetClientWindow();
	w->RemoveChild(this);
#endif
}

void ChildFrProsody::OnQuit(wxCommandEvent& WXUNUSED(event))
{
      Close(TRUE);
}


