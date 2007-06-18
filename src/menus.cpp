/***************************************************************************
 *   Copyright (C) 2005, 2006 by Jonathan Duddington                       *
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

#include "main.h"


wxMenu *speak_menu;

wxMenuBar *MakeMenu(int type)
{//==========================

	wxMenu *file_menu;
	wxMenu *help_menu;
	wxMenu *option_menu = NULL;
	wxMenu *paths_menu;
	wxMenu *data_menu = NULL;
	wxMenu *tools_menu;
	wxMenu *voice_menu;
	wxMenu *vowelchart_menu;
	wxMenu *lexicon_menu;

	// FILE MENU
	file_menu = new wxMenu;
	file_menu->Append(MENU_SPECTRUM, _T("&Open"));
	file_menu->Append(MENU_SPECTRUM2, _T("O&pen2"));
//	file_menu->Append(MENU_PROSODY, _T("&Prosody window"));
	if(type==0)
	{
	}
	else
	if(type==1)
	{
		file_menu->Append(SPECTSEQ_SAVE, _T("&Save"));
		file_menu->Append(SPECTSEQ_SAVEAS, _T("Save &As"));
		file_menu->Append(SPECTSEQ_SAVESELECT, _T("Save Selection"));
//		file_menu->Append(SPECTSEQ_SAVEPITCH, _T("Save Pitch &Envelope"));
		file_menu->Append(SPECTSEQ_CLOSE, _T("&Close"));
//		file_menu->Append(MENU_CLOSE_ALL, _T("Close &All"));
	}
	else
	if(type==2)
	{
		file_menu->Append(SPECTSEQ_SAVE, _T("&Save"));
		file_menu->Append(SPECTSEQ_SAVEAS, _T("Save &As"));
//		file_menu->Append(SPECTSEQ_SAVEPITCH, _T("Save Pitch &Envelope"));
		file_menu->Append(SPECTSEQ_CLOSE, _T("&Close"));
//		file_menu->Append(MENU_CLOSE_ALL, _T("Close &All"));
	}
	file_menu->Append(MENU_QUIT, _T("&Quit"));

	// SPEAK_MENU
	speak_menu = new wxMenu;
	speak_menu->Append(MENU_SPEAK_TRANSLATE, _T("&Translate"));
	speak_menu->Append(MENU_SPEAK_RULES, _T("Show &Rules"));
	speak_menu->Append(MENU_SPEAK_TEXT, _T("&Speak"));
	speak_menu->AppendSeparator();
	speak_menu->Append(MENU_SPEAK_FILE, _T("Speak &file"));
	speak_menu->Append(MENU_SPEAK_PAUSE, _T("&Pause"));
	speak_menu->Append(MENU_SPEAK_STOP, _T("St&op"));

	// VOICE MENU
	voice_menu = new wxMenu;
	voice_menu->Append(MENU_SPEAK_VOICE, _T("Select &Voice"));

	// DATA MENU
	data_menu = new wxMenu;
	data_menu->Append(MENU_COMPILE_PH, _T("Compile &phoneme data"));
	data_menu->Append(MENU_COMPILE_DICT, _T("Compile &dictionary"));
	data_menu->Append(MENU_COMPILE_MBROLA, _T("Compile &mbrola phonemes list"));
	
	// OPTIONS MENU
	paths_menu = new wxMenu;
	paths_menu->Append(MENU_PATH1, _T("Phoneme data source"));
	paths_menu->Append(MENU_PATH2, _T("Dictionary data source"));
	paths_menu->Append(MENU_PATH3, _T("Synthesized sound WAV file"));
	speak_menu->AppendSeparator();
	paths_menu->Append(MENU_PATH4, _T("Voice file to modify formant peaks"));

	option_menu = new wxMenu;
	option_menu->Append(MENU_PATHS, _T("Set &paths"), paths_menu);
	option_menu->Append(MENU_PARAMS, _T("Speed"));

	// TOOLS
	vowelchart_menu = new wxMenu;
	vowelchart_menu->Append(MENU_VOWELCHART1, _T("From compiled phoneme data"));
	vowelchart_menu->Append(MENU_VOWELCHART2, _T("From list of formants"));
	vowelchart_menu->Append(MENU_VOWELCHART3, _T("From directory of vowel files"));

	lexicon_menu = new wxMenu;
	lexicon_menu->Append(MENU_LEXICON_RU, _T("Russian (lexicon.dict)"));
	lexicon_menu->Append(MENU_LEXICON_DE, _T("German"));

	tools_menu = new wxMenu;
	tools_menu->Append(MENU_VOWELCHART, _T("Make &Vowels Chart"), vowelchart_menu);
//	tools_menu->Append(MENU_LOAD_WAV,   _T("Analyse WAV file"));
	tools_menu->Append(MENU_LEXICON, _T("Compare &Lexicon"), lexicon_menu);
	tools_menu->Append(MENU_TO_UTF8, _T("Convert file to &UTF8"));
	tools_menu->Append(MENU_COUNT_WORDS, _T("Count word &frequencies"));
	tools_menu->Append(MENU_TEST    ,   _T("Test (temporary)"));
//	tools_menu->Append(MENU_TEST2    ,   _T("Test2"));

	// HELP MENU
	help_menu = new wxMenu;
	help_menu->Append(MENU_ABOUT, _T("&About"));


	wxMenuBar *menu_bar = new wxMenuBar;

	menu_bar->Append(file_menu, _T("&File"));
	menu_bar->Append(speak_menu, _T("&Speak"));
	menu_bar->Append(voice_menu, _T("&Voice"));
	menu_bar->Append(option_menu, _T("&Options"));
	menu_bar->Append(tools_menu, _T("&Tools"));
	menu_bar->Append(data_menu, _T("&Compile"));
	menu_bar->Append(help_menu, _T("&Help"));

	return(menu_bar);
 }  //  end of MakeMenu


