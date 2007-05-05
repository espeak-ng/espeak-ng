/***************************************************************************
 *   Copyright (C) 2004 by Jonathan Duddington                             *
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

#ifndef _VOICEEDIT_H_
#define _VOICEEDIT_H_

/**
 * @short Application Main Window
 * @author Jonathan Duddington <jsd@clara.co.uk>
 * @version 0.1
 */

class 
voiceeditapp : public wxApp
{
	public:
		virtual bool OnInit();
};

class 
voiceeditFrame : public wxFrame
{
	public:
		voiceeditFrame( const wxString& title, const wxPoint& pos, const wxSize& pos );
		void OnQuit( wxCommandEvent& event );
		void OnAbout( wxCommandEvent& event );

	private:
		DECLARE_EVENT_TABLE()
};

enum
{
	Menu_File_Quit = 100,
	Menu_File_About
};

#endif // _VOICEEDIT_H_
