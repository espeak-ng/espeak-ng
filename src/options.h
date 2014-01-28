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

#ifndef OPTIONS_H
#define OPTIONS_H

#include "wx/dialog.h"
#include "wx/spinctrl.h"

/**
  *@author Jonathan Duddington
  */

extern void ConfigInit();
extern void ConfigSave(int end);
extern void ConfigSetPaths();

extern wxString path_espeakdata;
extern wxString path_spectload;
extern wxString path_spectload2;
extern wxString path_pitches;
extern wxString path_phfile;
extern wxString path_phsource;
extern wxString path_dictsource;
extern wxString path_modifiervoice;
extern wxString path_dir1;
extern int frame_x, frame_y, frame_w, frame_h;

extern char path_source[sizeof(path_home)+20];
extern char path_dsource[sizeof(path_home)+20];

extern int option_speed;

extern void OnOptions2(int event_id);

// not currently used
class Options : public wxDialog  {
public:
	Options(wxWindow *parent);
	~Options();

private:
	void OnCommand(wxCommandEvent& event);

	wxButton *m_save;
	wxButton *m_close;
	wxStaticText *m_lab[10];

	wxTextCtrl *m_samplerate;

	DECLARE_EVENT_TABLE()
};

#endif
