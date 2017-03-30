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
#ifndef ESPEAK_NG_ENCODING_H
#define ESPEAK_NG_ENCODING_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct espeak_ng_TEXT_DECODER_ espeak_ng_TEXT_DECODER;

espeak_ng_TEXT_DECODER *
create_text_decoder(void);

void
destroy_text_decoder(espeak_ng_TEXT_DECODER *decoder);

espeak_ng_STATUS
text_decoder_decode_string(espeak_ng_TEXT_DECODER *decoder,
                           const char *string,
                           int length,
                           espeak_ng_ENCODING encoding);

espeak_ng_STATUS
text_decoder_decode_wstring(espeak_ng_TEXT_DECODER *decoder,
                            const wchar_t *string,
                            int length);

int
text_decoder_eof(espeak_ng_TEXT_DECODER *decoder);

uint32_t
text_decoder_getc(espeak_ng_TEXT_DECODER *decoder);

#ifdef __cplusplus
}
#endif

#endif
