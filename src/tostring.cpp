/* Enumeration types to string.
 *
 * Copyright (C) 2012-2013 Reece H. Dunn
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
	switch (s)
	{
	case Arab: return "Arab";
	case Armi: return "Armi";
	case Armn: return "Armn";
	case Avst: return "Avst";
	case Bali: return "Bali";
	case Bamu: return "Bamu";
	case Batk: return "Batk";
	case Beng: return "Beng";
	case Bopo: return "Bopo";
	case Brah: return "Brah";
	case Brai: return "Brai";
	case Bugi: return "Bugi";
	case Buhd: return "Buhd";
	case Cans: return "Cans";
	case Cari: return "Cari";
	case Cakm: return "Cakm";
	case Cham: return "Cham";
	case Cher: return "Cher";
	case Copt: return "Copt";
	case Cprt: return "Cprt";
	case Cyrl: return "Cyrl";
	case Deva: return "Deva";
	case Dsrt: return "Dsrt";
	case Egyp: return "Egyp";
	case Ethi: return "Ethi";
	case Geor: return "Geor";
	case Glag: return "Glag";
	case Goth: return "Goth";
	case Grek: return "Grek";
	case Gujr: return "Gujr";
	case Guru: return "Guru";
	case Hang: return "Hang";
	case Hano: return "Hano";
	case Hant: return "Hant";
	case Hebr: return "Hebr";
	case Hira: return "Hira";
	case Ital: return "Ital";
	case Java: return "Java";
	case Kali: return "Kali";
	case Kana: return "Kana";
	case Khar: return "Khar";
	case Khmr: return "Khmr";
	case Knda: return "Knda";
	case Kthi: return "Kthi";
	case Lana: return "Lana";
	case Laoo: return "Laoo";
	case Latn: return "Latn";
	case Lepc: return "Lepc";
	case Limb: return "Limb";
	case Linb: return "Linb";
	case Lisu: return "Lisu";
	case Lyci: return "Lyci";
	case Lydi: return "Lydi";
	case Mand: return "Mand";
	case Merc: return "Merc";
	case Mero: return "Mero";
	case Mlym: return "Mlym";
	case Mong: return "Mong";
	case Mtei: return "Mtei";
	case Mymr: return "Mymr";
	case Nkoo: return "Nkoo";
	case Ogam: return "Ogam";
	case Olck: return "Olck";
	case Orkh: return "Orkh";
	case Orya: return "Orya";
	case Osma: return "Osma";
	case Phag: return "Phag";
	case Phli: return "Phli";
	case Phnx: return "Phnx";
	case Plrd: return "Plrd";
	case Prti: return "Prti";
	case Qaak: return "Qaak";
	case Rjng: return "Rjng";
	case Runr: return "Runr";
	case Samr: return "Samr";
	case Sarb: return "Sarb";
	case Saur: return "Saur";
	case Shaw: return "Shaw";
	case Shrd: return "Shrd";
	case Sinh: return "Sinh";
	case Sora: return "Sora";
	case Sund: return "Sund";
	case Sylo: return "Sylo";
	case Syrn: return "Syrn";
	case Tagb: return "Tagb";
	case Takr: return "Takr";
	case Tale: return "Tale";
	case Talu: return "Talu";
	case Taml: return "Taml";
	case Tavt: return "Tavt";
	case Telu: return "Telu";
	case Tfng: return "Tfng";
	case Tglg: return "Tglg";
	case Thaa: return "Thaa";
	case Thai: return "Thai";
	case Tibt: return "Tibt";
	case Ugar: return "Ugar";
	case Vaii: return "Vaii";
	case Xpeo: return "Xpeo";
	case Xsux: return "Xsux";
	case Yiii: return "Yiii";
	case Zyyy: return "Zyyy";
	case Zzzz: return "Zzzz";
	default:   return "----";
	}
}
