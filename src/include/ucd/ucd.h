/* Unicode Character Database API
 *
 * Copyright (C) 2012 Reece H. Dunn
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

	/** @name  Unicode General Category
	  * @brief These functions query the General Category property of Unicode codepoints.
	  */
	//@{

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
		Ps, /**< @brief Open PUnctuation Mark */

		Sc, /**< @brief Currency Symbol */
		Sk, /**< @brief Modifier Symbol */
		Sm, /**< @brief Math Symbol */
		So, /**< @brief Other Symbol */

		Zl, /**< @brief Line Separator */
		Zp, /**< @brief Paragraph Separator */
		Zs, /**< @brief Space Separator */
	};

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


	//@}
	/** @name  Unicode Script
	  * @brief These functions query the Script property of Unicode codepoints.
	  */
	//@{


	/** @brief Unicode Script
	  * @see   http://www.iana.org/assignments/language-subtag-registry
	  * @see   http://www.unicode.org/iso15924/iso15924-codes.html
	  */
	enum script
	{
		Arab, /**< @brief Arabic Script */
		Armi, /**< @brief Imperial Aramaic Script */
		Armn, /**< @brief Armenian Script */
		Avst, /**< @brief Avestan Script */
		Bali, /**< @brief Balinese Script */
		Bamu, /**< @brief Bamum Script */
		Batk, /**< @brief Batak Script */
		Beng, /**< @brief Bengali Script */
		Bopo, /**< @brief Bopomofo Script */
		Brah, /**< @brief Brahmi Script */
		Brai, /**< @brief Braille Script */
		Bugi, /**< @brief Buginese Script */
		Buhd, /**< @brief Buhid Script */
		Cans, /**< @brief Unified Canadian Aboriginal Syllabics */
		Cari, /**< @brief Carian Script */
		Cakm, /**< @brief Chakma Script */
		Cham, /**< @brief Cham Script */
		Cher, /**< @brief Cherokee Script */
		Copt, /**< @brief Coptic Script */
		Cprt, /**< @brief Cypriot Script */
		Cyrl, /**< @brief Cyrillic Script */
		Deva, /**< @brief Devanagari Script */
		Dsrt, /**< @brief Deseret Script */
		Egyp, /**< @brief Egyptian Hiegoglyphs */
		Ethi, /**< @brief Ethiopic Script */
		Geor, /**< @brief Geirgian Script */
		Glag, /**< @brief Glagolitic Script */
		Goth, /**< @brief Gothic Script */
		Grek, /**< @brief Greek Script */
		Gujr, /**< @brief Gujarati Script */
		Guru, /**< @brief Gurmukhi Script */
		Hang, /**< @brief Hangul Script */
		Hano, /**< @brief Hanunoo Script */
		Hant, /**< @brief Han (Traditional) Script */
		Hebr, /**< @brief Hebrew Script */
		Hira, /**< @brief Hiragana Script */
		Ital, /**< @brief Old Italic Script */
		Java, /**< @brief Javanese Script */
		Kali, /**< @brief Kayah Li Script */
		Kana, /**< @brief Katakana Script */
		Khar, /**< @brief Kharoshthi Script */
		Khmr, /**< @brief Khmer Script */
		Knda, /**< @brief Kannada Script */
		Kthi, /**< @brief Kaithi Script */
		Lana, /**< @brief Tai Tham Script */
		Laoo, /**< @brief Lao Script */
		Latn, /**< @brief Latin Script */
		Lepc, /**< @brief Lepcha Script */
		Limb, /**< @brief Limbu Script */
		Linb, /**< @brief Linear B Script */
		Lisu, /**< @brief Lisu Script */
		Lyci, /**< @brief Lycian Script */
		Lydi, /**< @brief Lydian Script */
		Mand, /**< @brief Mandaic Script */
		Merc, /**< @brief Meroitic Cursive Script */
		Mero, /**< @brief Meroitic Hieroglyphs */
		Mlym, /**< @brief Malayalam Script */
		Mong, /**< @brief Mongolian Script */
		Mtei, /**< @brief Meitei Mayek Script */
		Mymr, /**< @brief Myanmar Script */
		Nkoo, /**< @brief N'Ko Script */
		Ogam, /**< @brief Ogham Script */
		Olck, /**< @brief Ol Chiki Script */
		Orkh, /**< @brief Old Turkic Script */
		Orya, /**< @brief Oriya Script */
		Osma, /**< @brief Osmanya Script */
		Phag, /**< @brief Phags-Pa Script */
		Phli, /**< @brief Inscriptional Pahlavi Script */
		Phnx, /**< @brief Phoenician Script */
		Plrd, /**< @brief Miao Script */
		Prti, /**< @brief Inscriptional Parthian Script */
		Rjng, /**< @brief Rejang Script */
		Runr, /**< @brief Runic Script */
		Samr, /**< @brief Samaritan Script */
		Sarb, /**< @brief Old South Arabian Script */
		Saur, /**< @brief Saurashtra Script */
		Shaw, /**< @brief Shavian Script */
		Shrd, /**< @brief Sharada Script */
		Sinh, /**< @brief Sinhala Script */
		Sora, /**< @brief Sora Sompeng Script */
		Sund, /**< @brief Sundanese Script */
		Sylo, /**< @brief Syloti Nagri Script */
		Syrn, /**< @brief Syriatic (Eastern) Script */
		Tagb, /**< @brief Tagbanwa Script */
		Takr, /**< @brief Takri Script */
		Tale, /**< @brief Tai Le Script */
		Talu, /**< @brief New Tai Lue Script */
		Taml, /**< @brief Tamil Script */
		Tavt, /**< @brief Tai Viet Script */
		Telu, /**< @brief Telugu Script */
		Tfng, /**< @brief Tifinagh Script */
		Tglg, /**< @brief Tagalog Script */
		Thaa, /**< @brief Thaana Script */
		Thai, /**< @brief Thai Script */
		Tibt, /**< @brief Tibetan Script */
		Ugar, /**< @brief Ugaritic Script */
		Vaii, /**< @brief Vai Script */
		Xpeo, /**< @brief Old Persian Script */
		Xsux, /**< @brief Cuneiform Script */
		Yiii, /**< @brief Yi Script */
		Zyyy, /**< @brief Inherited Script */
		Zzzz, /**< @brief Unknown Script */
	};

	/** @brief Lookup the Script for a Unicode codepoint.
	  *
	  * @param c The Unicode codepoint to lookup.
	  * @return  The Script of the Unicode codepoint.
	  */
	script lookup_script(codepoint_t c);


	//@}
	/** @name  ctype-style APIs
	  * @brief These functions provide wctype compatible functions using the UCD data.
	  */
	//@{


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


	//@}
	/** @name  Case Conversion APIs
	  * @brief These functions convert Unicode codepoints between lower, upper and title case.
	  */
	//@{


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


	//@}
}

#endif
