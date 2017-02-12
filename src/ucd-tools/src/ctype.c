/* ctype-style APIs.
 *
 * Copyright (C) 2012-2017 Reece H. Dunn
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

static int other_alphabetic_MnMcSo(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x0300:
		return c == 0x0345;
	case 0x0500:
		return (c >= 0x05B0 && c <= 0x05BD)
		    ||  c == 0x05BF
		    || (c >= 0x05C1 && c <= 0x05C2)
		    || (c >= 0x05C4 && c <= 0x05C5)
		    ||  c == 0x05C7;
	case 0x0600:
		return (c >= 0x0610 && c <= 0x061A)
		    || (c >= 0x064B && c <= 0x0657)
		    || (c >= 0x0659 && c <= 0x065F)
		    ||  c == 0x0670
		    || (c >= 0x06D6 && c <= 0x06DC)
		    || (c >= 0x06E1 && c <= 0x06E4)
		    || (c >= 0x06E7 && c <= 0x06E8)
		    ||  c == 0x06ED;
	case 0x0700:
		return  c == 0x0711
		    || (c >= 0x0730 && c <= 0x073F)
		    || (c >= 0x07A6 && c <= 0x07B0);
	case 0x0800:
		return (c >= 0x0816 && c <= 0x0817)
		    || (c >= 0x081B && c <= 0x0823)
		    || (c >= 0x0825 && c <= 0x0827)
		    || (c >= 0x0829 && c <= 0x082C)
		    || (c >= 0x08D4 && c <= 0x08DF)
		    || (c >= 0x08E3 && c <= 0x08E9)
		    ||  c >= 0x08F0;
	case 0x0900:
		return (c >= 0x0900 && c <= 0x0903)
		    || (c >= 0x093A && c <= 0x093B)
		    || (c >= 0x093E && c <= 0x094C)
		    || (c >= 0x094E && c <= 0x094F)
		    || (c >= 0x0955 && c <= 0x0957)
		    || (c >= 0x0962 && c <= 0x0963)
		    || (c >= 0x0981 && c <= 0x0983)
		    || (c >= 0x0981 && c <= 0x0983)
		    || (c >= 0x09BE && c <= 0x09C4)
		    || (c >= 0x09C7 && c <= 0x09C8)
		    || (c >= 0x09CB && c <= 0x09CC)
		    ||  c == 0x09D7
		    || (c >= 0x09E2 && c <= 0x09E3);
	case 0x0A00:
		return (c >= 0x0A01 && c <= 0x0A03)
		    || (c >= 0x0A3E && c <= 0x0A42)
		    || (c >= 0x0A47 && c <= 0x0A48)
		    || (c >= 0x0A4B && c <= 0x0A4C)
		    ||  c == 0x0A51
		    || (c >= 0x0A70 && c <= 0x0A71)
		    ||  c == 0x0A75
		    || (c >= 0x0A81 && c <= 0x0A83)
		    || (c >= 0x0ABE && c <= 0x0AC5)
		    || (c >= 0x0AC7 && c <= 0x0AC9)
		    || (c >= 0x0ACB && c <= 0x0ACC)
		    || (c >= 0x0AE2 && c <= 0x0AE3);
	case 0x0B00:
		return (c >= 0x0B01 && c <= 0x0B03)
		    || (c >= 0x0B3E && c <= 0x0B44)
		    || (c >= 0x0B47 && c <= 0x0B48)
		    || (c >= 0x0B4B && c <= 0x0B4C)
		    || (c >= 0x0B56 && c <= 0x0B57)
		    || (c >= 0x0B62 && c <= 0x0B63)
		    ||  c == 0x0B82
		    || (c >= 0x0BBE && c <= 0x0BC2)
		    || (c >= 0x0BC6 && c <= 0x0BC8)
		    || (c >= 0x0BCA && c <= 0x0BCC)
		    ||  c == 0x0BD7;
	case 0x0C00:
		return (c >= 0x0C00 && c <= 0x0C03)
		    || (c >= 0x0C3E && c <= 0x0C44)
		    || (c >= 0x0C46 && c <= 0x0C48)
		    || (c >= 0x0C4A && c <= 0x0C4C)
		    || (c >= 0x0C55 && c <= 0x0C56)
		    || (c >= 0x0C62 && c <= 0x0C63)
		    || (c >= 0x0C81 && c <= 0x0C83)
		    || (c >= 0x0CBE && c <= 0x0CBF)
		    || (c >= 0x0CC0 && c <= 0x0CC4)
		    || (c >= 0x0CC6 && c <= 0x0CC8)
		    || (c >= 0x0CCA && c <= 0x0CCC)
		    || (c >= 0x0CD5 && c <= 0x0CD6)
		    || (c >= 0x0CE2 && c <= 0x0CE3);
	case 0x0D00:
		return (c >= 0x0D01 && c <= 0x0D03)
		    || (c >= 0x0D3E && c <= 0x0D44)
		    || (c >= 0x0D46 && c <= 0x0D48)
		    || (c >= 0x0D4A && c <= 0x0D4C)
		    ||  c == 0x0D57
		    || (c >= 0x0D62 && c <= 0x0D63)
		    || (c >= 0x0D82 && c <= 0x0D83)
		    || (c >= 0x0DCF && c <= 0x0DD4)
		    ||  c == 0x0DD6
		    || (c >= 0x0DD8 && c <= 0x0DDF)
		    || (c >= 0x0DF2 && c <= 0x0DF3);
	case 0x0E00:
		return  c == 0x0E31
		    || (c >= 0x0E34 && c <= 0x0E3A)
		    ||  c == 0x0E4D
		    ||  c == 0x0EB1
		    || (c >= 0x0EB4 && c <= 0x0EB9)
		    || (c >= 0x0EBB && c <= 0x0EBD)
		    ||  c == 0x0ECD;
	case 0x0F00:
		return (c >= 0x0F71 && c <= 0x0F7F)
		    || (c >= 0x0F80 && c <= 0x0F81)
		    || (c >= 0x0F8D && c <= 0x0F97)
		    || (c >= 0x0F99 && c <= 0x0FBC);
	case 0x1000:
		return (c >= 0x102B && c <= 0x1036)
		    ||  c == 0x1038
		    || (c >= 0x103B && c <= 0x103E)
		    || (c >= 0x1056 && c <= 0x1059)
		    || (c >= 0x105E && c <= 0x1060)
		    ||  c == 0x1062
		    || (c >= 0x1067 && c <= 0x1068)
		    || (c >= 0x1071 && c <= 0x1074)
		    || (c >= 0x1082 && c <= 0x1086)
		    || (c >= 0x109C && c <= 0x109D);
	case 0x1300:
		return c == 0x135F;
	case 0x1700:
		return (c >= 0x1712 && c <= 0x1713)
		    || (c >= 0x1732 && c <= 0x1733)
		    || (c >= 0x1752 && c <= 0x1753)
		    || (c >= 0x1772 && c <= 0x1773)
		    || (c >= 0x17B6 && c <= 0x17C8);
	case 0x1800:
		return (c >= 0x1885 && c <= 0x1886)
		    ||  c == 0x18A9;
	case 0x1900:
		return (c >= 0x1920 && c <= 0x192B)
		    || (c >= 0x1930 && c <= 0x1938);
	case 0x1A00:
		return (c >= 0x1A17 && c <= 0x1A1B)
		    || (c >= 0x1A55 && c <= 0x1A5E)
		    || (c >= 0x1A61 && c <= 0x1A74);
	case 0x1B00:
		return (c >= 0x1B00 && c <= 0x1B04)
		    || (c >= 0x1B35 && c <= 0x1B43)
		    || (c >= 0x1B80 && c <= 0x1B82)
		    || (c >= 0x1BA1 && c <= 0x1BA9)
		    || (c >= 0x1BAC && c <= 0x1BAD)
		    || (c >= 0x1BE7 && c <= 0x1BF1);
	case 0x1C00:
		return (c >= 0x1C24 && c <= 0x1C35)
		    || (c >= 0x1CF2 && c <= 0x1CF3);
	case 0x1D00:
		return (c >= 0x1DE7 && c <= 0x1DF4);
	case 0x2400:
		return (c >= 0x24B6 && c <= 0x24E9);
	case 0x2D00:
		return (c >= 0x2DE0 && c <= 0x2DFF);
	case 0xA600:
		return (c >= 0xA674 && c <= 0xA67B)
		    || (c >= 0xA69E && c <= 0xA69F);
	case 0xA800:
		return (c >= 0xA823 && c <= 0xA827)
		    || (c >= 0xA880 && c <= 0xA881)
		    || (c >= 0xA8B4 && c <= 0xA8C3)
		    ||  c == 0xA8C5;
	case 0xA900:
		return (c >= 0xA926 && c <= 0xA92A)
		    || (c >= 0xA947 && c <= 0xA952)
		    || (c >= 0xA980 && c <= 0xA983)
		    || (c >= 0xA9B4 && c <= 0xA9BF);
	case 0xAA00:
		return (c >= 0xAA29 && c <= 0xAA36)
		    ||  c == 0xAA43
		    || (c >= 0xAA4C && c <= 0xAA4D)
		    ||  c == 0xAAB0
		    || (c >= 0xAAB2 && c <= 0xAAB4)
		    || (c >= 0xAAB7 && c <= 0xAAB8)
		    ||  c == 0xAABE
		    || (c >= 0xAAEB && c <= 0xAAEF)
		    ||  c == 0xAAF5;
	case 0xAB00:
		return (c >= 0xABE3 && c <= 0xABEA);
	case 0xFB00:
		return c == 0xFB1E;
	case 0x10300:
		return (c >= 0x10376 && c <= 0x1037A);
	case 0x10A00:
		return (c >= 0x10A01 && c <= 0x10A03)
		    || (c >= 0x10A05 && c <= 0x10A06)
		    || (c >= 0x10A0C && c <= 0x10A0F);
	case 0x11000:
		return (c >= 0x11000 && c <= 0x11002)
		    || (c >= 0x11038 && c <= 0x11045)
		    ||  c == 0x11082
		    || (c >= 0x110B0 && c <= 0x110B8);
	case 0x11100:
		return (c >= 0x11100 && c <= 0x11102)
		    || (c >= 0x11127 && c <= 0x11132)
		    || (c >= 0x11180 && c <= 0x11182)
		    || (c >= 0x111B3 && c <= 0x111BF);
	case 0x11200:
		return (c >= 0x1122C && c <= 0x11234)
		    ||  c == 0x11237
		    ||  c == 0x1123E
		    || (c >= 0x112DF && c <= 0x112E8);
	case 0x11300:
		return (c >= 0x11300 && c <= 0x11303)
		    || (c >= 0x1133E && c <= 0x11344)
		    || (c >= 0x11347 && c <= 0x11348)
		    || (c >= 0x1134B && c <= 0x1134C)
		    ||  c == 0x11357
		    || (c >= 0x11362 && c <= 0x11363);
	case 0x11400:
		return (c >= 0x11435 && c <= 0x11441)
		    || (c >= 0x11443 && c <= 0x11445)
		    || (c >= 0x114B0 && c <= 0x114C1);
	case 0x11500:
		return (c >= 0x115AF && c <= 0x115B5)
		    || (c >= 0x115B8 && c <= 0x115BE)
		    || (c >= 0x115DC && c <= 0x115DD);
	case 0x11600:
		return (c >= 0x11630 && c <= 0x1163E)
		    ||  c == 0x11640
		    || (c >= 0x116AB && c <= 0x116B5);
	case 0x11700:
		return (c >= 0x1171D && c <= 0x1172A);
	case 0x11C00:
		return (c >= 0x11C2F && c <= 0x11C36)
		    || (c >= 0x11C38 && c <= 0x11C3E)
		    || (c >= 0x11C92 && c <= 0x11CA7)
		    || (c >= 0x11CA9 && c <= 0x11CB6);
	case 0x16B00:
		return (c >= 0x16B30 && c <= 0x16B36);
	case 0x16F00:
		return (c >= 0x16F51 && c <= 0x16F7E);
	case 0x1BC00:
		return c == 0x1BC9E;
	case 0x1E000:
		return (c >= 0x1E000 && c <= 0x1E006)
		    || (c >= 0x1E008 && c <= 0x1E018)
		    || (c >= 0x1E01B && c <= 0x1E021)
		    || (c >= 0x1E023 && c <= 0x1E024)
		    || (c >= 0x1E026 && c <= 0x1E02A);
	case 0x1E900:
		return c == 0x1E947;
	case 0x1F100:
		return (c >= 0x01F130 && c <= 0x01F149)
		    || (c >= 0x01F150 && c <= 0x01F169)
		    || (c >= 0x01F170 && c <= 0x01F189);
	default:
		return 0;
	}
}

int ucd_isalnum(codepoint_t c)
{
	switch (ucd_lookup_category(c))
	{
	case UCD_CATEGORY_Lu:
	case UCD_CATEGORY_Ll:
	case UCD_CATEGORY_Lt:
	case UCD_CATEGORY_Lm:
	case UCD_CATEGORY_Lo:
	case UCD_CATEGORY_Nl:
	case UCD_CATEGORY_Nd:
	case UCD_CATEGORY_No:
		return 1;
	case UCD_CATEGORY_Mn:
	case UCD_CATEGORY_Mc:
	case UCD_CATEGORY_So:
		return other_alphabetic_MnMcSo(c);
	default:
		return 0;
	}
}

int ucd_isalpha(codepoint_t c)
{
	switch (ucd_lookup_category(c))
	{
	case UCD_CATEGORY_Lu:
	case UCD_CATEGORY_Ll:
	case UCD_CATEGORY_Lt:
	case UCD_CATEGORY_Lm:
	case UCD_CATEGORY_Lo:
	case UCD_CATEGORY_Nl:
		return 1;
	case UCD_CATEGORY_Mn:
	case UCD_CATEGORY_Mc:
	case UCD_CATEGORY_So:
		return other_alphabetic_MnMcSo(c);
	default:
		return 0;
	}
}

int ucd_isblank(codepoint_t c)
{
	switch (ucd_lookup_category(c))
	{
	case UCD_CATEGORY_Zs:
		switch (c) // Exclude characters with the <noBreak> DispositionType
		{
		case 0x00A0: // U+00A0 : NO-BREAK SPACE
		case 0x2007: // U+2007 : FIGURE SPACE
		case 0x202F: // U+202F : NARROW NO-BREAK SPACE
			return 0;
		}
		return 1;
	case UCD_CATEGORY_Cc:
		return c == 0x09; // U+0009 : CHARACTER TABULATION
	default:
		return 0;
	}
}

int ucd_iscntrl(codepoint_t c)
{
	return ucd_lookup_category(c) == UCD_CATEGORY_Cc;
}

int ucd_isdigit(codepoint_t c)
{
	return (c >= 0x30 && c <= 0x39); // [0-9]
}

int ucd_isgraph(codepoint_t c)
{
	switch (ucd_lookup_category(c))
	{
	case UCD_CATEGORY_Cc:
	case UCD_CATEGORY_Cf:
	case UCD_CATEGORY_Cn:
	case UCD_CATEGORY_Co:
	case UCD_CATEGORY_Cs:
	case UCD_CATEGORY_Zl:
	case UCD_CATEGORY_Zp:
	case UCD_CATEGORY_Zs:
	case UCD_CATEGORY_Ii:
		return 0;
	default:
		return 1;
	}
}

int ucd_islower(codepoint_t c)
{
	switch (ucd_lookup_category(c))
	{
	case UCD_CATEGORY_Ll:
		return 1;
	case UCD_CATEGORY_Lt:
		return ucd_toupper(c) != c;
	case UCD_CATEGORY_Lo:
		return c == 0xAA  // Other_Lowercase : FEMININE ORDINAL INDICATOR
		    || c == 0xBA; // Other_Lowercase : MASCULINE ORDINAL INDICATOR
	case UCD_CATEGORY_Lm:
		return (c >= 0x02B0 && c <= 0x02B8)  // Other_Lowercase
		    || (c >= 0x02C0 && c <= 0x02C1)  // Other_Lowercase
		    || (c >= 0x02E0 && c <= 0x02E4)  // Other_Lowercase
		    ||  c == 0x037A                  // Other_Lowercase
		    || (c >= 0x1D2C && c <= 0x1D6A)  // Other_Lowercase
		    ||  c == 0x1D78                  // Other_Lowercase
		    || (c >= 0x1D9B && c <= 0x1DBF)  // Other_Lowercase
		    ||  c == 0x2071                  // Other_Lowercase
		    ||  c == 0x207F                  // Other_Lowercase
		    || (c >= 0x2090 && c <= 0x209C)  // Other_Lowercase
		    || (c >= 0x2C7C && c <= 0x2C7D)  // Other_Lowercase
		    || (c >= 0xA69C && c <= 0xA69D)  // Other_Lowercase
		    ||  c == 0xA770                  // Other_Lowercase
		    || (c >= 0xA7F8 && c <= 0xA7F9)  // Other_Lowercase
		    || (c >= 0xAB5C && c <= 0xAB5F); // Other_Lowercase
	case UCD_CATEGORY_Mn:
		return c == 0x0345; // Other_Lowercase : COMBINING GREEK YPOGEGRAMMENI
	case UCD_CATEGORY_Nl:
		return (c >= 0x2170 && c <= 0x217F); // Other_Lowercase
	case UCD_CATEGORY_So:
		return (c >= 0x24D0 && c <= 0x24E9); // Other_Lowercase
	default:
		return 0;
	}
}

int ucd_isprint(codepoint_t c)
{
	switch (ucd_lookup_category(c))
	{
	case UCD_CATEGORY_Cc:
	case UCD_CATEGORY_Cf:
	case UCD_CATEGORY_Cn:
	case UCD_CATEGORY_Co:
	case UCD_CATEGORY_Cs:
	case UCD_CATEGORY_Ii:
		return 0;
	default:
		return 1;
	}
}

int ucd_ispunct(codepoint_t c)
{
	switch (ucd_lookup_category(c))
	{
	case UCD_CATEGORY_Pc:
	case UCD_CATEGORY_Pd:
	case UCD_CATEGORY_Pe:
	case UCD_CATEGORY_Pf:
	case UCD_CATEGORY_Pi:
	case UCD_CATEGORY_Po:
	case UCD_CATEGORY_Ps:
		return 1;
	default:
		return 0;
	}
}

int ucd_isspace(codepoint_t c)
{
	switch (ucd_lookup_category(c))
	{
	case UCD_CATEGORY_Zl:
	case UCD_CATEGORY_Zp:
		return 1;
	case UCD_CATEGORY_Zs:
		switch (c) // Exclude characters with the <noBreak> DispositionType
		{
		case 0x00A0: // U+00A0 : NO-BREAK SPACE
		case 0x2007: // U+2007 : FIGURE SPACE
		case 0x202F: // U+202F : NARROW NO-BREAK SPACE
			return 0;
		}
		return 1;
	case UCD_CATEGORY_Cc:
		switch (c) // Include control characters marked as White_Space
		{
		case 0x09: // U+0009 : CHARACTER TABULATION
		case 0x0A: // U+000A : LINE FEED
		case 0x0B: // U+000B : LINE TABULATION
		case 0x0C: // U+000C : FORM FEED
		case 0x0D: // U+000D : CARRIAGE RETURN
		case 0x85: // U+0085 : NEXT LINE
			return 1;
		}
	default:
		return 0;
	}
}

int ucd_isupper(codepoint_t c)
{
	switch (ucd_lookup_category(c))
	{
	case UCD_CATEGORY_Lu:
		return 1;
	case UCD_CATEGORY_Lt:
		return ucd_tolower(c) != c;
	case UCD_CATEGORY_Nl:
		return (c >= 0x002160 && c <= 0x00216F); // Other_Uppercase
	case UCD_CATEGORY_So:
		return (c >= 0x0024B6 && c <= 0x0024CF)  // Other_Uppercase
		    || (c >= 0x01F130 && c <= 0x01F149)  // Other_Uppercase
		    || (c >= 0x01F150 && c <= 0x01F169)  // Other_Uppercase
		    || (c >= 0x01F170 && c <= 0x01F189); // Other_Uppercase
	default:
		return 0;
	}
}

int ucd_isxdigit(codepoint_t c)
{
	return (c >= 0x30 && c <= 0x39)  // [0-9]
	    || (c >= 0x41 && c <= 0x46)  // [A-Z]
	    || (c >= 0x61 && c <= 0x66); // [a-z]
}
