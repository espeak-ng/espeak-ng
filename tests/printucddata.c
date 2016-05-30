/*
 * Copyright (C) 2012-2016 Reece H. Dunn
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

void fput_utf8c(FILE *out, codepoint_t c)
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

int fget_utf8c(FILE *in, codepoint_t *c)
{
	int ch = EOF;
	if ((ch = fgetc(in)) == EOF) return 0;
	if ((uint8_t)ch < 0x80)
		*c = (uint8_t)ch;
	else switch ((uint8_t)ch & 0xF0)
	{
	default:
		*c = (uint8_t)ch & 0x1F;
		if ((ch = fgetc(in)) == EOF) return 0;
		*c = (*c << 6) + ((uint8_t)ch & 0x3F);
		break;
	case 0xE0:
		*c = (uint8_t)ch & 0x0F;
		if ((ch = fgetc(in)) == EOF) return 0;
		*c = (*c << 6) + ((uint8_t)ch & 0x3F);
		if ((ch = fgetc(in)) == EOF) return 0;
		*c = (*c << 6) + ((uint8_t)ch & 0x3F);
		break;
	case 0xF0:
		*c = (uint8_t)ch & 0x07;
		if ((ch = fgetc(in)) == EOF) return 0;
		*c = (*c << 6) + ((uint8_t)ch & 0x3F);
		if ((ch = fgetc(in)) == EOF) return 0;
		*c = (*c << 6) + ((uint8_t)ch & 0x3F);
		if ((ch = fgetc(in)) == EOF) return 0;
		*c = (*c << 6) + ((uint8_t)ch & 0x3F);
		break;
	}
	return 1;
}

void uprintf_codepoint(FILE *out, codepoint_t c, char mode)
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

void uprintf(FILE *out, codepoint_t c, const char *format)
{
	while (*format) switch (*format)
	{
	case '%':
		switch (*++format)
		{
		case 'c': // category
			fputs(ucd_get_category_string(ucd_lookup_category(c)), out);
			break;
		case 'C': // category group
			fputs(ucd_get_category_group_string(ucd_lookup_category_group(c)), out);
			break;
		case 'p': // codepoint
			uprintf_codepoint(out, c, *++format);
			break;
		case 'L': // lowercase
			uprintf_codepoint(out, ucd_tolower(c), *++format);
			break;
		case 's': // script
			fputs(ucd_get_script_string(ucd_lookup_script(c)), out);
			break;
		case 'T': // titlecase
			uprintf_codepoint(out, ucd_totitle(c), *++format);
			break;
		case 'U': // uppercase
			uprintf_codepoint(out, ucd_toupper(c), *++format);
			break;
		case 'W': // whitespace
			if (ucd_isspace(c))
				fputs("White_Space", out);
			break;
		}
		++format;
		break;
	default:
		fputc(*format, out);
		++format;
		break;
	}
}

void print_file(FILE *in)
{
	codepoint_t c = 0;
	while (fget_utf8c(in, &c))
		uprintf(stdout, c, "%pc\t%pH\t%s\t%c\t%Uc\t%Lc\t%Tc\t%W\n");
}

int main(int argc, char **argv)
{
	if (argc == 2)
	{
		if (!strcmp(argv[1], "--stdin") || !strcmp(argv[1], "-"))
			print_file(stdin);
		else
		{
			FILE *in = fopen(argv[1], "r");
			if (in)
			{
				print_file(in);
				fclose(in);
			}
			else
				fprintf(stdout, "cannot open `%s`\n", argv[1]);
		}
	}
	else
	{
		for (codepoint_t c = 0; c <= 0x10FFFF; ++c)
			uprintf(stdout, c, "%pH %s %C %c %UH %LH %TH %W\n");
	}
	return 0;
}
