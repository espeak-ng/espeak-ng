/* Unicode Character Database API
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

#ifndef UNICODE_CHARACTER_DATA_H
#define UNICODE_CHARACTER_DATA_H

#include <stdint.h>

/** @brief Unicode Character Database
  */
namespace ucd
{
	/** @brief Represents a Unicode codepoint.
	  */
	typedef uint32_t codepoint_t;

	/** @brief Unicode General Category Groups
	  * @see   http://www.unicode.org/reports/tr44/
	  */
	enum category_group
	{
		C, /**< @brief Other */
		I, /**< @brief Invalid */
		L, /**< @brief Letter */
		M, /**< @brief Mark */
		N, /**< @brief Number */
		P, /**< @brief Punctuation */
		S, /**< @brief Symbol */
		Z, /**< @brief Separator */
	};

	/** @brief Get a string representation of the category_group enumeration value.
	  *
	  * @param c The value to get the string representation for.
	  *
	  * @return The string representation, or "-" if the value is not recognized.
	  */
	const char *get_category_group_string(category_group c);

	/** @brief Unicode General Category Values
	  * @see   http://www.unicode.org/reports/tr44/
	  */
	enum category
	{
		Cc, /**< @brief Control Character */
		Cf, /**< @brief Format Control Character */
		Cn, /**< @brief Unassigned */
		Co, /**< @brief Private Use */
		Cs, /**< @brief Surrogate Code Point */

		Ii, /**< @brief Invalid Unicode Codepoint */

		Ll, /**< @brief Lower Case Letter */
		Lm, /**< @brief Letter Modifier */
		Lo, /**< @brief Other Letter */
		Lt, /**< @brief Title Case Letter */
		Lu, /**< @brief Upper Case Letter */

		Mc, /**< @brief Spacing Mark */
		Me, /**< @brief Enclosing Mark */
		Mn, /**< @brief Non-Spacing Mark */

		Nd, /**< @brief Decimal Digit */
		Nl, /**< @brief Letter-Like Number */
		No, /**< @brief Other Number */

		Pc, /**< @brief Connector */
		Pd, /**< @brief Dash/Hyphen */
		Pe, /**< @brief Close Punctuation Mark */
		Pf, /**< @brief Final Quotation Mark */
		Pi, /**< @brief Initial Quotation Mark */
		Po, /**< @brief Other */
		Ps, /**< @brief Open Punctuation Mark */

		Sc, /**< @brief Currency Symbol */
		Sk, /**< @brief Modifier Symbol */
		Sm, /**< @brief Math Symbol */
		So, /**< @brief Other Symbol */

		Zl, /**< @brief Line Separator */
		Zp, /**< @brief Paragraph Separator */
		Zs, /**< @brief Space Separator */
	};

	/** @brief Get a string representation of the category enumeration value.
	  *
	  * @param c The value to get the string representation for.
	  *
	  * @return The string representation, or "--" if the value is not recognized.
	  */
	const char *get_category_string(category c);

	/** @brief Lookup the General Category Group for a General Category.
	  *
	  * @param c The General Category to lookup.
	  * @return  The General Category Group of the General Category.
	  */
	category_group lookup_category_group(category c);

	/** @brief Lookup the General Category Group for a Unicode codepoint.
	  *
	  * @param c The Unicode codepoint to lookup.
	  * @return  The General Category Group of the Unicode codepoint.
	  */
	category_group lookup_category_group(codepoint_t c);

	/** @brief Lookup the General Category for a Unicode codepoint.
	  *
	  * @param c The Unicode codepoint to lookup.
	  * @return  The General Category of the Unicode codepoint.
	  */
	category lookup_category(codepoint_t c);

	/** @brief Unicode Script
	  * @see   http://www.iana.org/assignments/language-subtag-registry
	  * @see   http://www.unicode.org/iso15924/iso15924-codes.html
	  */
	enum script
	{
		Afak, /**< @brief Afaka Script */
		Aghb, /**< @brief Caucasian Albanian Script */
		Ahom, /**< @brief Tai Ahom Script */
		Arab, /**< @brief Arabic Script */
		Armi, /**< @brief Imperial Aramaic Script */
		Armn, /**< @brief Armenian Script */
		Avst, /**< @brief Avestan Script */
		Bali, /**< @brief Balinese Script */
		Bamu, /**< @brief Bamum Script */
		Bass, /**< @brief Bassa Vah Script */
		Batk, /**< @brief Batak Script */
		Beng, /**< @brief Bengali Script */
		Blis, /**< @brief Blissymbols Script */
		Bopo, /**< @brief Bopomofo Script */
		Brah, /**< @brief Brahmi Script */
		Brai, /**< @brief Braille Script */
		Bugi, /**< @brief Buginese Script */
		Buhd, /**< @brief Buhid Script */
		Cakm, /**< @brief Chakma Script */
		Cans, /**< @brief Unified Canadian Aboriginal Syllabics */
		Cari, /**< @brief Carian Script */
		Cham, /**< @brief Cham Script */
		Cher, /**< @brief Cherokee Script */
		Cirt, /**< @brief Cirth Script */
		Copt, /**< @brief Coptic Script */
		Cprt, /**< @brief Cypriot Script */
		Cyrl, /**< @brief Cyrillic Script */
		Cyrs, /**< @brief Cyrillic (Old Church Slavonic variant) Script */
		Deva, /**< @brief Devanagari Script */
		Dsrt, /**< @brief Deseret Script */
		Dupl, /**< @brief Duployan Shorthand Script */
		Egyd, /**< @brief Egyptian Demotic Script */
		Egyh, /**< @brief Egyptian Hieratic Script */
		Egyp, /**< @brief Egyptian Hiegoglyphs */
		Elba, /**< @brief Elbasan Script */
		Ethi, /**< @brief Ethiopic Script */
		Geok, /**< @brief Khutsuri Script */
		Geor, /**< @brief Geirgian Script */
		Glag, /**< @brief Glagolitic Script */
		Goth, /**< @brief Gothic Script */
		Gran, /**< @brief Grantha Script */
		Grek, /**< @brief Greek Script */
		Gujr, /**< @brief Gujarati Script */
		Guru, /**< @brief Gurmukhi Script */
		Hang, /**< @brief Hangul Script */
		Hani, /**< @brief Han (Hanzi, Kanji, Hanja) Script */
		Hano, /**< @brief Hanunoo Script */
		Hans, /**< @brief Han (Simplified) Script */
		Hant, /**< @brief Han (Traditional) Script */
		Hatr, /**< @brief Hatran Script */
		Hebr, /**< @brief Hebrew Script */
		Hira, /**< @brief Hiragana Script */
		Hluw, /**< @brief Anatolian Hieroglyphs */
		Hmng, /**< @brief Pahawh Hmong Script */
		Hrkt, /**< @brief Japanese Syllabaries */
		Hung, /**< @brief Old Hungarian Script */
		Inds, /**< @brief Indus Script */
		Ital, /**< @brief Old Italic Script */
		Java, /**< @brief Javanese Script */
		Jpan, /**< @brief Japanese (Han + Hiragana + Katakana) Scripts */
		Jurc, /**< @brief Jurchen Script */
		Kali, /**< @brief Kayah Li Script */
		Kana, /**< @brief Katakana Script */
		Khar, /**< @brief Kharoshthi Script */
		Khmr, /**< @brief Khmer Script */
		Khoj, /**< @brief Khojki Script */
		Knda, /**< @brief Kannada Script */
		Kore, /**< @brief Korean (Hangul + Han) Scripts */
		Kpel, /**< @brief Kpelle Script */
		Kthi, /**< @brief Kaithi Script */
		Lana, /**< @brief Tai Tham Script */
		Laoo, /**< @brief Lao Script */
		Latf, /**< @brief Latin Script (Fractur Variant) */
		Latg, /**< @brief Latin Script (Gaelic Variant) */
		Latn, /**< @brief Latin Script */
		Lepc, /**< @brief Lepcha Script */
		Limb, /**< @brief Limbu Script */
		Lina, /**< @brief Linear A Script */
		Linb, /**< @brief Linear B Script */
		Lisu, /**< @brief Lisu Script */
		Loma, /**< @brief Loma Script */
		Lyci, /**< @brief Lycian Script */
		Lydi, /**< @brief Lydian Script */
		Mahj, /**< @brief Mahajani Script */
		Mand, /**< @brief Mandaic Script */
		Mani, /**< @brief Manichaean Script */
		Maya, /**< @brief Mayan Hieroglyphs */
		Mend, /**< @brief Mende Kikakui Script */
		Merc, /**< @brief Meroitic Cursive Script */
		Mero, /**< @brief Meroitic Hieroglyphs */
		Mlym, /**< @brief Malayalam Script */
		Modi, /**< @brief Modi Script */
		Mong, /**< @brief Mongolian Script */
		Moon, /**< @brief Moon Script */
		Mroo, /**< @brief Mro Script */
		Mtei, /**< @brief Meitei Mayek Script */
		Mult, /**< @brief Multani Script */
		Mymr, /**< @brief Myanmar (Burmese) Script */
		Narb, /**< @brief Old North Arabian Script */
		Nbat, /**< @brief Nabataean Script */
		Nkgb, /**< @brief Nakhi Geba Script */
		Nkoo, /**< @brief N'Ko Script */
		Nshu, /**< @brief Nushu Script */
		Ogam, /**< @brief Ogham Script */
		Olck, /**< @brief Ol Chiki Script */
		Orkh, /**< @brief Old Turkic Script */
		Orya, /**< @brief Oriya Script */
		Osma, /**< @brief Osmanya Script */
		Palm, /**< @brief Palmyrene Script */
		Pauc, /**< @brief Pau Cin Hau Script */
		Perm, /**< @brief Old Permic */
		Phag, /**< @brief Phags-Pa Script */
		Phli, /**< @brief Inscriptional Pahlavi Script */
		Phlp, /**< @brief Psalter Pahlavi Script */
		Phlv, /**< @brief Book Pahlavi Script */
		Phnx, /**< @brief Phoenician Script */
		Plrd, /**< @brief Miao Script */
		Prti, /**< @brief Inscriptional Parthian Script */
		Qaak, /**< @brief Klingon Script (Private Use) */
		Rjng, /**< @brief Rejang Script */
		Roro, /**< @brief Rongorongo Script */
		Runr, /**< @brief Runic Script */
		Samr, /**< @brief Samaritan Script */
		Sara, /**< @brief Sarati Script */
		Sarb, /**< @brief Old South Arabian Script */
		Saur, /**< @brief Saurashtra Script */
		Sgnw, /**< @brief Sign Writing */
		Shaw, /**< @brief Shavian Script */
		Shrd, /**< @brief Sharada Script */
		Sidd, /**< @brief Siddham Script */
		Sind, /**< @brief Sindhi Script */
		Sinh, /**< @brief Sinhala Script */
		Sora, /**< @brief Sora Sompeng Script */
		Sund, /**< @brief Sundanese Script */
		Sylo, /**< @brief Syloti Nagri Script */
		Syrc, /**< @brief Syriac Script */
		Syre, /**< @brief Syriac Script (Estrangelo Variant) */
		Syrj, /**< @brief Syriac Script (Western Variant) */
		Syrn, /**< @brief Syriac Script (Eastern Variant) */
		Tagb, /**< @brief Tagbanwa Script */
		Takr, /**< @brief Takri Script */
		Tale, /**< @brief Tai Le Script */
		Talu, /**< @brief New Tai Lue Script */
		Taml, /**< @brief Tamil Script */
		Tang, /**< @brief Tangut Script */
		Tavt, /**< @brief Tai Viet Script */
		Telu, /**< @brief Telugu Script */
		Teng, /**< @brief Tengwar Script */
		Tfng, /**< @brief Tifinagh Script */
		Tglg, /**< @brief Tagalog Script */
		Thaa, /**< @brief Thaana Script */
		Thai, /**< @brief Thai Script */
		Tibt, /**< @brief Tibetan Script */
		Tirh, /**< @brief Tirhuta Script */
		Ugar, /**< @brief Ugaritic Script */
		Vaii, /**< @brief Vai Script */
		Visp, /**< @brief Visible Speech Script */
		Wara, /**< @brief Warang Citi Script */
		Wole, /**< @brief Woleai Script */
		Xpeo, /**< @brief Old Persian Script */
		Xsux, /**< @brief Cuneiform Script */
		Yiii, /**< @brief Yi Script */
		Zinh, /**< @brief Inherited Script */
		Zmth, /**< @brief Mathematical Notation */
		Zsym, /**< @brief Symbols */
		Zxxx, /**< @brief Unwritten Documents */
		Zyyy, /**< @brief Undetermined Script */
		Zzzz, /**< @brief Uncoded Script */
	};

	/** @brief Get a string representation of the script enumeration value.
	  *
	  * @param s The value to get the string representation for.
	  *
	  * @return The string representation, or "----" if the value is not recognized.
	  */
	const char *get_script_string(script s);

	/** @brief Lookup the Script for a Unicode codepoint.
	  *
	  * @param c The Unicode codepoint to lookup.
	  * @return  The Script of the Unicode codepoint.
	  */
	script lookup_script(codepoint_t c);

	/** @brief Is the codepoint an alpha-numeric character?
	  *
	  * @param c The Unicode codepoint to check.
	  * @return  Non-zero if the codepoint is a letter or number, zero otherwise.
	  */
	int isalnum(codepoint_t c);

	/** @brief Is the codepoint a letter?
	  *
	  * @param c The Unicode codepoint to check.
	  * @return  Non-zero if the codepoint is a letter, zero otherwise.
	  */
	int isalpha(codepoint_t c);

	/** @brief Is the codepoint a control character?
	  *
	  * @param c The Unicode codepoint to check.
	  * @return  Non-zero if the codepoint is a control character, zero otherwise.
	  */
	int iscntrl(codepoint_t c);

	/** @brief Is the codepoint a numeric character?
	  *
	  * @param c The Unicode codepoint to check.
	  * @return  Non-zero if the codepoint is a number, zero otherwise.
	  */
	int isdigit(codepoint_t c);

	/** @brief Does the codepoint have a displayable glyph?
	  *
	  * @param c The Unicode codepoint to check.
	  * @return  Non-zero if the codepoint has a displayable glyph, zero otherwise.
	  */
	int isgraph(codepoint_t c);

	/** @brief Is the codepoint a lower-case letter?
	  *
	  * @param c The Unicode codepoint to check.
	  * @return  Non-zero if the codepoint is a lower-case letter, zero otherwise.
	  */
	int islower(codepoint_t c);

	/** @brief Is the codepoint a printable character?
	  *
	  * @param c The Unicode codepoint to check.
	  * @return  Non-zero if the codepoint is a printable character, zero otherwise.
	  */
	int isprint(codepoint_t c);

	/** @brief Is the codepoint a punctuation character?
	  *
	  * @param c The Unicode codepoint to check.
	  * @return  Non-zero if the codepoint is a punctuation character, zero otherwise.
	  */
	int ispunct(codepoint_t c);

	/** @brief Is the codepoint a whitespace character?
	  *
	  * @param c The Unicode codepoint to check.
	  * @return  Non-zero if the codepoint is a whitespace character, zero otherwise.
	  */
	int isspace(codepoint_t c);

	/** @brief Is the codepoint an upper-case letter?
	  *
	  * @param c The Unicode codepoint to check.
	  * @return  Non-zero if the codepoint is an upper-case letter, zero otherwise.
	  */
	int isupper(codepoint_t c);

	/** @brief Convert the Unicode codepoint to upper-case.
	  *
	  * This function only uses the simple case mapping present in the
	  * UnicodeData file. The data in SpecialCasing requires Unicode
	  * codepoints to be mapped to multiple codepoints.
	  *
	  * @param c The Unicode codepoint to convert.
	  * @return  The upper-case Unicode codepoint for this codepoint, or
	  *          this codepoint if there is no upper-case codepoint.
	  */
	codepoint_t toupper(codepoint_t c);

	/** @brief Convert the Unicode codepoint to lower-case.
	  *
	  * This function only uses the simple case mapping present in the
	  * UnicodeData file. The data in SpecialCasing requires Unicode
	  * codepoints to be mapped to multiple codepoints.
	  *
	  * @param c The Unicode codepoint to convert.
	  * @return  The lower-case Unicode codepoint for this codepoint, or
	  *          this codepoint if there is no upper-case codepoint.
	  */
	codepoint_t tolower(codepoint_t c);

	/** @brief Convert the Unicode codepoint to title-case.
	  *
	  * This function only uses the simple case mapping present in the
	  * UnicodeData file. The data in SpecialCasing requires Unicode
	  * codepoints to be mapped to multiple codepoints.
	  *
	  * @param c The Unicode codepoint to convert.
	  * @return  The title-case Unicode codepoint for this codepoint, or
	  *          this codepoint if there is no upper-case codepoint.
	  */
	codepoint_t totitle(codepoint_t c);
}

#endif
