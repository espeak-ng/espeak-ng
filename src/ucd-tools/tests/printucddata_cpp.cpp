/*
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

#include <string.h>
#include <stdio.h>

void fput_utf8c(FILE *out, ucd::codepoint_t c)
{
	if (c < 0x80)
		fputc((uint8_t)c, out);
	else if (c < 0x800)
	{
		fputc(0xC0 | (c >> 6), out);
		fputc(0x80 + (c & 0x3F), out);
	}
	else if (c < 0x10000)
	{
		fputc(0xE0 | (c >> 12), out);
		fputc(0x80 + ((c >> 6) & 0x3F), out);
		fputc(0x80 + (c & 0x3F), out);
	}
	else if (c < 0x200000)
	{
		fputc(0xF0 | (c >> 18), out);
		fputc(0x80 + ((c >> 12) & 0x3F), out);
		fputc(0x80 + ((c >>  6) & 0x3F), out);
		fputc(0x80 + (c & 0x3F), out);
	}
}

bool fget_utf8c(FILE *in, ucd::codepoint_t &c)
{
	int ch = EOF;
	if ((ch = fgetc(in)) == EOF) return false;
	if (uint8_t(ch) < 0x80)
		c = uint8_t(ch);
	else switch (uint8_t(ch) & 0xF0)
	{
	default:
		c = uint8_t(ch) & 0x1F;
		if ((ch = fgetc(in)) == EOF) return false;
		c = (c << 6) + (uint8_t(ch) & 0x3F);
		break;
	case 0xE0:
		c = uint8_t(ch) & 0x0F;
		if ((ch = fgetc(in)) == EOF) return false;
		c = (c << 6) + (uint8_t(ch) & 0x3F);
		if ((ch = fgetc(in)) == EOF) return false;
		c = (c << 6) + (uint8_t(ch) & 0x3F);
		break;
	case 0xF0:
		c = uint8_t(ch) & 0x07;
		if ((ch = fgetc(in)) == EOF) return false;
		c = (c << 6) + (uint8_t(ch) & 0x3F);
		if ((ch = fgetc(in)) == EOF) return false;
		c = (c << 6) + (uint8_t(ch) & 0x3F);
		if ((ch = fgetc(in)) == EOF) return false;
		c = (c << 6) + (uint8_t(ch) & 0x3F);
		break;
	}
	return true;
}

void uprintf_codepoint(FILE *out, ucd::codepoint_t c, char mode)
{
	switch (mode)
	{
	case 'c': // character
		switch (c)
		{
		case '\t': fputs("\\t", out);  break;
		case '\r': fputs("\\r", out);  break;
		case '\n': fputs("\\n", out);  break;
		default:   fput_utf8c(out, c); break;
		}
		break;
	case 'h': // hexadecimal (lower)
		fprintf(out, "%06x", c);
		break;
	case 'H': // hexadecimal (upper)
		fprintf(out, "%06X", c);
		break;
	}
}

void uprintf_is(FILE *out, ucd::codepoint_t c, char mode)
{
	switch (mode)
	{
	case 'A': // alpha-numeric
		fputc(ucd::isalnum(c) ? '1' : '0', out);
		break;
	case 'a': // alpha
		fputc(ucd::isalpha(c) ? '1' : '0', out);
		break;
	case 'b': // blank
		fputc(ucd::isblank(c) ? '1' : '0', out);
		break;
	case 'c': // control
		fputc(ucd::iscntrl(c) ? '1' : '0', out);
		break;
	case 'd': // numeric
		fputc(ucd::isdigit(c) ? '1' : '0', out);
		break;
	case 'g': // glyph
		fputc(ucd::isgraph(c) ? '1' : '0', out);
		break;
	case 'l': // lower case
		fputc(ucd::islower(c) ? '1' : '0', out);
		break;
	case 'P': // printable
		fputc(ucd::isprint(c) ? '1' : '0', out);
		break;
	case 'p': // punctuation
		fputc(ucd::ispunct(c) ? '1' : '0', out);
		break;
	case 's': // whitespace
		fputc(ucd::isspace(c) ? '1' : '0', out);
		break;
	case 'u': // upper case
		fputc(ucd::isupper(c) ? '1' : '0', out);
		break;
	case 'x': // xdigit
		fputc(ucd::isxdigit(c) ? '1' : '0', out);
		break;
	}
}

void uprintf(FILE *out, ucd::codepoint_t c, const char *format)
{
	while (*format) switch (*format)
	{
	case '%':
		switch (*++format)
		{
		case 'c': // category
			fputs(ucd::get_category_string(ucd::lookup_category(c)), out);
			break;
		case 'C': // category group
			fputs(ucd::get_category_group_string(ucd::lookup_category_group(c)), out);
			break;
		case 'p': // codepoint
			uprintf_codepoint(out, c, *++format);
			break;
		case 'P': // properties
			fprintf(out, "%016llx", ucd::properties(c, ucd::lookup_category(c)));
			break;
		case 'i': // is*
			uprintf_is(out, c, *++format);
			break;
		case 'L': // lowercase
			uprintf_codepoint(out, ucd::tolower(c), *++format);
			break;
		case 's': // script
			fputs(ucd::get_script_string(ucd::lookup_script(c)), out);
			break;
		case 'T': // titlecase
			uprintf_codepoint(out, ucd::totitle(c), *++format);
			break;
		case 'U': // uppercase
			uprintf_codepoint(out, ucd::toupper(c), *++format);
			break;
		}
		++format;
		break;
	case '\\':
		switch (*++format) {
		case 0:
			break;
		case 't':
			fputc('\t', out);
			++format;
			break;
		case 'r':
			fputc('\r', out);
			++format;
			break;
		case 'n':
			fputc('\n', out);
			++format;
			break;
		default:
			fputc(*format, out);
			++format;
			break;
		}
		break;
	default:
		fputc(*format, out);
		++format;
		break;
	}
}

void print_file(FILE *in, const char *format)
{
	ucd::codepoint_t c = 0;
	while (fget_utf8c(in, c))
		uprintf(stdout, c, format ? format : "%pc\t%pH\t%s\t%c\t%Uc\t%Lc\t%Tc\t%is\n");
}

int main(int argc, char **argv)
{
	FILE *in = NULL;
	const char *format = NULL;
	for (int argn = 1; argn != argc; ++argn)
	{
		const char *arg = argv[argn];
		if (!strcmp(arg, "--stdin") || !strcmp(arg, "-"))
			in = stdin;
		else if (!strncmp(arg, "--format=", 9))
			format = arg + 9;
		else if (in == NULL)
		{
			in = fopen(arg, "r");
			if (!in)
				fprintf(stdout, "cannot open `%s`\n", argv[1]);
		}
	}

	if (in == stdin)
		print_file(stdin, format);
	else if (in != NULL)
	{
		print_file(in, format);
		fclose(in);
	}
	else
	{
		for (ucd::codepoint_t c = 0; c <= 0x10FFFF; ++c)
			uprintf(stdout, c, format ? format :
			        "%pH %s %C %c %UH %LH %TH %id %ix %ic %is %ib %ip %iP %ig %iA %ia %iu %il %P\n");
	}
	return 0;
}
