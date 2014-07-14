/* Enumeration types to string.
 *
 * Copyright (C) 2012-2014 Reece H. Dunn
 *
 * This file is part of ucd-tools.
 *
 * ucd-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ucd-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ucd-tools.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ucd/ucd.h"

const char *ucd::get_category_group_string(category_group c)
{
	switch (c)
	{
	case C:  return "C";
	case I:  return "I";
	case L:  return "L";
	case M:  return "M";
	case N:  return "N";
	case P:  return "P";
	case S:  return "S";
	case Z:  return "Z";
	default: return "-";
	}
}

const char *ucd::get_category_string(category c)
{
	switch (c)
	{
	case Cc: return "Cc";
	case Cf: return "Cf";
	case Cn: return "Cn";
	case Co: return "Co";
	case Cs: return "Cs";
	case Ii: return "Ii";
	case Ll: return "Ll";
	case Lm: return "Lm";
	case Lo: return "Lo";
	case Lt: return "Lt";
	case Lu: return "Lu";
	case Mc: return "Mc";
	case Me: return "Me";
	case Mn: return "Mn";
	case Nd: return "Nd";
	case Nl: return "Nl";
	case No: return "No";
	case Pc: return "Pc";
	case Pd: return "Pd";
	case Pe: return "Pe";
	case Pf: return "Pf";
	case Pi: return "Pi";
	case Po: return "Po";
	case Ps: return "Ps";
	case Sc: return "Sc";
	case Sk: return "Sk";
	case Sm: return "Sm";
	case So: return "So";
	case Zl: return "Zl";
	case Zp: return "Zp";
	case Zs: return "Zs";
	default: return "--";
	}
}

const char *ucd::get_script_string(script s)
{
	static const char *scripts[] =
	{
		"Afak",
		"Aghb",
		"Ahom",
		"Arab",
		"Armi",
		"Armn",
		"Avst",
		"Bali",
		"Bamu",
		"Bass",
		"Batk",
		"Beng",
		"Blis",
		"Bopo",
		"Brah",
		"Brai",
		"Bugi",
		"Buhd",
		"Cakm",
		"Cans",
		"Cari",
		"Cham",
		"Cher",
		"Cirt",
		"Copt",
		"Cprt",
		"Cyrl",
		"Cyrs",
		"Deva",
		"Dsrt",
		"Dupl",
		"Egyd",
		"Egyh",
		"Egyp",
		"Elba",
		"Ethi",
		"Geor",
		"Geok",
		"Glag",
		"Goth",
		"Gran",
		"Grek",
		"Gujr",
		"Guru",
		"Hang",
		"Hani",
		"Hano",
		"Hant",
		"Hatr",
		"Hebr",
		"Hira",
		"Hluw",
		"Hmng",
		"Hrkt",
		"Hung",
		"Inds",
		"Ital",
		"Java",
		"Jpan",
		"Jurc",
		"Kali",
		"Kana",
		"Khar",
		"Khmr",
		"Khoj",
		"Knda",
		"Kore",
		"Kpel",
		"Kthi",
		"Lana",
		"Laoo",
		"Latf",
		"Latg",
		"Latn",
		"Lepc",
		"Limb",
		"Lina",
		"Linb",
		"Lisu",
		"Loma",
		"Lyci",
		"Lydi",
		"Mahj",
		"Mand",
		"Mani",
		"Maya",
		"Mend",
		"Merc",
		"Mero",
		"Mlym",
		"Modi",
		"Mong",
		"Moon",
		"Mroo",
		"Mtei",
		"Mult",
		"Mymr",
		"Narb",
		"Nbat",
		"Nkgb",
		"Nkoo",
		"Nshu",
		"Ogam",
		"Olck",
		"Orkh",
		"Orya",
		"Osma",
		"Palm",
		"Pauc",
		"Perm",
		"Phag",
		"Phli",
		"Phlp",
		"Phlv",
		"Phnx",
		"Plrd",
		"Prti",
		"Qaak",
		"Rjng",
		"Roro",
		"Runr",
		"Samr",
		"Sara",
		"Sarb",
		"Saur",
		"Sgnw",
		"Shaw",
		"Shrd",
		"Sidd",
		"Sind",
		"Sinh",
		"Sora",
		"Sund",
		"Sylo",
		"Syrc",
		"Syre",
		"Syrj",
		"Syrn",
		"Tagb",
		"Takr",
		"Tale",
		"Talu",
		"Taml",
		"Tang",
		"Tavt",
		"Telu",
		"Teng",
		"Tfng",
		"Tglg",
		"Thaa",
		"Thai",
		"Tibt",
		"Tirh",
		"Ugar",
		"Vaii",
		"Visp",
		"Wara",
		"Wole",
		"Xpeo",
		"Xsux",
		"Yiii",
		"Zinh",
		"Zmth",
		"Zsym",
		"Zxxx",
		"Zyyy",
		"Zzzz",
	};

	if ((unsigned int)s >= (sizeof(scripts)/sizeof(scripts[0])))
		return "----";
	return scripts[(unsigned int)s];
}
