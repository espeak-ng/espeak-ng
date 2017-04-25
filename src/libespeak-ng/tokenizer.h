/*
 * Copyright (C) 2017 Reece H. Dunn
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see: <http://www.gnu.org/licenses/>.
 */
#ifndef ESPEAK_NG_TOKENIZER_H
#define ESPEAK_NG_TOKENIZER_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct espeak_ng_TOKENIZER_ espeak_ng_TOKENIZER;

espeak_ng_TOKENIZER *
create_tokenizer(void);

void
destroy_tokenizer(espeak_ng_TOKENIZER *tokenizer);

int
tokenizer_reset(espeak_ng_TOKENIZER *tokenizer,
                espeak_ng_TEXT_DECODER *decoder);

typedef enum
{
	ESPEAKNG_TOKEN_END_OF_BUFFER,
	ESPEAKNG_TOKEN_UNKNOWN,
} espeak_ng_TOKEN_TYPE;

espeak_ng_TOKEN_TYPE
tokenizer_read_next_token(espeak_ng_TOKENIZER *tokenizer);

const char *
tokenizer_get_token_text(espeak_ng_TOKENIZER *tokenizer);

#ifdef __cplusplus
}
#endif

#endif
