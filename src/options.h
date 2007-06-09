/***************************************************************************
                          options.h  -  description
                             -------------------
    begin                : Sat Mar 13 2004
    copyright            : (C) 2004 by Jonathan Duddington
    email                : jsd@jsd.clara.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
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

extern wxString path_spectload;
extern wxString path_spectload2;
extern wxString path_pitches;
extern wxString path_voices;
extern wxString path_phsource;
extern wxString path_dictsource;
extern wxString path_dir1;

extern char path_source[80];
extern char path_dsource[120];

extern int option_speed;

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
