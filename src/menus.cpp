/***************************************************************************
 *   Copyright (C) 2005 to 2013 by Jonathan Duddington                     *
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

#include "main.h"

wxMenu *speak_menu;
wxMenu *data_menu = NULL;
wxMenuBar *menu_bar = NULL;


wxMenuBar *MakeMenu(int type, const char *dict_name)
{//===================================================
	char buf[100];
	wxMenu *old_menu;
	wxMenu *file_menu;
	wxMenu *help_menu;
	wxMenu *option_menu = NULL;
	wxMenu *paths_menu;
	wxMenu *tools_menu;
	wxMenu *voice_menu;
	wxMenu *vowelchart_menu;
	wxMenu *lexicon_menu;

	// FILE MENU
	file_menu = new wxMenu;
	file_menu->Append(MENU_SPECTRUM, _("&Open..."));
	file_menu->Append(MENU_SPECTRUM2, _("O&pen2..."));
//	file_menu->Append(MENU_PROSODY, _T("&Prosody window"));
	if(type<=1)
	{
	}
	else
	if(type==2)
	{
		file_menu->Append(SPECTSEQ_SAVE, _("&Save"));
		file_menu->Append(SPECTSEQ_SAVEAS, _("Save &As..."));
		file_menu->Append(SPECTSEQ_SAVESELECT, _("Save Selection"));
		file_menu->Append(SPECTSEQ_SAVEPITCH, _T("Save Pitch &Envelope"));
		file_menu->Append(SPECTSEQ_CLOSE, _("&Close"));
		file_menu->Append(MENU_CLOSE_ALL, _T("Close &All"));
	}
	else
	if(type==3)
	{
//		file_menu->Append(SPECTSEQ_SAVE, _("&Save"));
//		file_menu->Append(SPECTSEQ_SAVEAS, _("Save &As..."));
		file_menu->Append(SPECTSEQ_CLOSE, _("&Close"));
		file_menu->Append(MENU_CLOSE_ALL, _T("Close &All"));
	}
	file_menu->Append(MENU_QUIT, _("&Quit"));

    if(type==0)
    {
        // Initialization, make all the menus
        // SPEAK_MENU
        speak_menu = new wxMenu;
        speak_menu->Append(MENU_SPEAK_TRANSLATE, _("&Translate"));
        speak_menu->Append(MENU_SPEAK_RULES, _("Show &Rules"));
        speak_menu->Append(MENU_SPEAK_IPA, _("Show &IPA"));
        speak_menu->Append(MENU_SPEAK_TEXT, _("&Speak"));
        speak_menu->AppendSeparator();
        speak_menu->Append(MENU_SPEAK_FILE, _("Speak &file..."));
        speak_menu->Append(MENU_SPEAK_PAUSE, _("&Pause"));
        speak_menu->Append(MENU_SPEAK_STOP, _("St&op"));

        // VOICE MENU
        voice_menu = new wxMenu;
        voice_menu->Append(MENU_SPEAK_VOICE, _("Select &Voice.."));
        voice_menu->Append(MENU_SPEAK_VOICE_VARIANT, _("Select Voice Va&riant..."));

        // DATA MENU
        data_menu = new wxMenu;
        data_menu->Append(MENU_COMPILE_DICT, _("Compile &dictionary"));
        data_menu->Append(MENU_COMPILE_DICT_DEBUG, _("Compile dictionary (debu&g)"));
        data_menu->Append(MENU_COMPILE_PH, _("Compile &phoneme data 22050Hz"));
        data_menu->Append(MENU_COMPILE_PH2, _("Compile at sample &rate"));
        data_menu->AppendSeparator();
        data_menu->Append(MENU_COMPILE_MBROLA, _("Compile &mbrola phonemes list..."));
        data_menu->Append(MENU_COMPILE_INTONATION, _("Compile &intonation data"));
        data_menu->AppendSeparator();
        data_menu->Append(MENU_FORMAT_DICTIONARY, _("&Layout *_rules file"));
        data_menu->Append(MENU_SORT_DICTIONARY, _("&Sort *_rules file"));

        sprintf(buf,"Compile &dictionary '%s'",dict_name);
        data_menu->SetLabel(MENU_COMPILE_DICT, wxString(buf,wxConvLocal));
        sprintf(buf,"&Layout '%s_rules' file",dict_name);
        data_menu->SetLabel(MENU_FORMAT_DICTIONARY, wxString(buf,wxConvLocal));
        sprintf(buf,"&Sort '%s_rules' file",dict_name);
        data_menu->SetLabel(MENU_SORT_DICTIONARY, wxString(buf,wxConvLocal));

        // OPTIONS MENU
        paths_menu = new wxMenu;
        paths_menu->Append(MENU_PATH0, _("Master phonemes file..."));
        paths_menu->Append(MENU_PATH1, _("Phoneme data source..."));
        paths_menu->Append(MENU_PATH2, _("Dictionary data source..."));
        paths_menu->Append(MENU_PATH3, _("Synthesized sound WAV file..."));
        paths_menu->AppendSeparator();
        paths_menu->Append(MENU_PATH4, _("Voice file to modify formant peaks..."));

        option_menu = new wxMenu;
        option_menu->Append(MENU_PATHS, _("Set &paths"), paths_menu);
        option_menu->Append(MENU_OPT_SPEED, _("Speed..."));
        option_menu->Append(MENU_OPT_PUNCT, _("Speak punctuation"));
        option_menu->Append(MENU_OPT_SPELL, _("Speak character names"));

        // TOOLS
        vowelchart_menu = new wxMenu;
        vowelchart_menu->Append(MENU_VOWELCHART1, _("From compiled phoneme data"));
    //	vowelchart_menu->Append(MENU_VOWELCHART2, _("From list of formants..."));
        vowelchart_menu->Append(MENU_VOWELCHART3, _("From directory of vowel files..."));

        lexicon_menu = new wxMenu;
        lexicon_menu->Append(MENU_LEXICON_RU, _("Russian"));
        lexicon_menu->Append(MENU_LEXICON_BG, _("Bulgarian"));
        lexicon_menu->Append(MENU_LEXICON_DE, _("German"));
        lexicon_menu->Append(MENU_LEXICON_IT, _("Italian"));
    //	lexicon_menu->Append(MENU_LEXICON_TEST, _("Test"));

        tools_menu = new wxMenu;
        tools_menu->Append(MENU_VOWELCHART, _("Make &Vowels Chart"), vowelchart_menu);
    //	tools_menu->Append(MENU_LOAD_WAV,   _T("Analyse WAV file"));
        tools_menu->Append(MENU_LEXICON, _("Process &Lexicon"), lexicon_menu);
        tools_menu->Append(MENU_TO_UTF8, _("Convert file to &UTF8..."));
        tools_menu->Append(MENU_COUNT_WORDS, _("Count word &frequencies..."));
        tools_menu->Append(MENU_TEST    ,   _("Test (temporary)"));
    //	tools_menu->Append(MENU_TEST2    ,   _T("Test2"));

        // HELP MENU
        help_menu = new wxMenu;
        help_menu->Append(MENU_DOCS, _("eSpeak &Documentation"));
        help_menu->Append(MENU_ABOUT, _("&About"));


        menu_bar = new wxMenuBar;

        menu_bar->Append(file_menu, _("&File"));
        menu_bar->Append(speak_menu, _("&Speak"));
        menu_bar->Append(voice_menu, _("&Voice"));
        menu_bar->Append(option_menu, _("&Options"));
        menu_bar->Append(tools_menu, _("&Tools"));
        menu_bar->Append(data_menu, _("&Compile"));
        menu_bar->Append(help_menu, _("&Help"));
    }
    else
    {
        // Just replace the File menu
        old_menu = menu_bar->Replace(0, file_menu, _("&File"));
        delete old_menu;
    }
	return(menu_bar);
 }  //  end of MakeMenu


