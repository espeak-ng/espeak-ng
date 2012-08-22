/***************************************************************************
 *   Copyright (C) 2005,2006 by Jonathan Duddington                        *
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




class Translator_English: public Translator
{//=======================================

public:
	Translator_English();
	int Unpronouncable(char *word);

};  // end of class Translator_English




class Translator_Esperanto: public Translator
{//==========================================
public:
	Translator_Esperanto();
private:
	int TranslateChar(char *ptr, int prev_in, int c, int next_in);
};



class Translator_German: public Translator
{//=======================================

public:
	Translator_German();

//	void SetWordStress(char *output, unsigned int dictionary_flags, int tonic, int prev_stress);
//	int Unpronouncable(char *word);

};  // end of class Translator_German


class Translator_Afrikaans: public Translator
{//==========================================

public:
	Translator_Afrikaans();
private:
	int TranslateChar(char *ptr, int prev_in, int c, int next_in);

};  // end of class Translator_Afrikaans




class Translator_Tone: public Translator
{//==========================================

public:
	Translator_Tone();
private:
	void CalcPitches(int clause_tone);

};  // end of class Translator_Tone

