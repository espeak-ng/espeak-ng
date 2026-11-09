/*
 * Copyright (C) 2026 eSpeak NG contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.reecedunn.espeak;

import java.util.Arrays;

/**
 * Pure-Java digit spacing used by the "read numbers digit by digit" setting,
 * with an offset map back to the input text.
 *
 * <p>Every digit run is rewritten so that each digit and each separator that
 * sits between two digits becomes its own token, separated from its
 * neighbours by a single space:
 * <ul>
 *   <li>{@code "123" -> "1 2 3"}</li>
 *   <li>{@code "3.14" -> "3 . 1 4"}</li>
 *   <li>{@code "1,234" -> "1 , 2 3 4"}</li>
 * </ul>
 * Keeping the separator in its own token stops the engine from merging
 * digits back into a decimal ("3.1") or a grouped number ("1,2"), which it
 * would then read with the wrong prosody ("three point one", "one comma
 * two"). Separators are only recognised between two digits; punctuation at
 * the edge of a run or outside a run is left exactly as it was.
 *
 * <p>Because the transform only inserts spaces, it can build an exact
 * boundary map from the spaced text back to the input, which the Android TTS
 * service composes with the Unicode normalization map to report word
 * boundaries against the text the caller supplied.
 */
public final class DigitSpacing {
    private DigitSpacing() {
    }

    /** Characters that act as separators inside a run of digits. */
    static boolean isDigitSeparator(int codePoint) {
        switch (codePoint) {
            case '.':
            case ',':
            case '-':
            case '/':
            case ':':
            case '\u066B': // Arabic decimal separator
            case '\u066C': // Arabic thousands separator
                return true;
            default:
                return false;
        }
    }

    /** A digit-spaced string together with its offset map into the input. */
    public static final class Result {
        /** The spaced text, to be handed to the synthesizer. */
        public final String text;

        /**
         * Boundary map of size {@code text.length() + 1}: entry {@code i} is
         * the UTF-16 offset in the input string corresponding to UTF-16
         * offset {@code i} in {@link #text}. Inserted spaces map to the
         * boundary they were inserted at, so mapped ranges never invert.
         */
        private final int[] mOffsets;

        Result(String text, int[] offsets) {
            this.text = text;
            mOffsets = offsets;
        }

        /** Maps a UTF-16 offset in {@link #text} to one in the input string. */
        public int toOriginalOffset(int offset) {
            if (offset <= 0) {
                return 0;
            }
            if (offset >= mOffsets.length) {
                return mOffsets[mOffsets.length - 1];
            }
            return mOffsets[offset];
        }
    }

    /**
     * Spaces out every digit run of {@code text} and returns the result with
     * its offset map. Returns {@code null} text for a null input.
     */
    public static Result separate(String text) {
        if (text == null) {
            return new Result(null, new int[] { 0 });
        }
        final int len = text.length();
        if (len == 0) {
            return new Result("", new int[] { 0 });
        }

        final StringBuilder out = new StringBuilder(len * 2);
        // Every input UTF-16 unit maps to at most one output unit, and a space
        // is inserted between adjacent output tokens, so the output can never
        // exceed 2 * len UTF-16 units.
        final int[] map = new int[len * 2 + 1];
        int mapPos = 0;
        map[mapPos++] = 0;

        int i = 0;
        while (i < len) {
            final int codePoint = text.codePointAt(i);
            final int charCount = Character.charCount(codePoint);
            if (!Character.isDigit(codePoint)) {
                out.appendCodePoint(codePoint);
                for (int k = 0; k < charCount; k++) {
                    map[mapPos++] = i + k + 1;
                }
                i += charCount;
                continue;
            }

            // Digit run: emit every digit and every in-number separator as its
            // own token, single-spaced.
            int pos = i;
            boolean firstToken = true;
            while (true) {
                final int digit = text.codePointAt(pos);
                final int digitCount = Character.charCount(digit);
                if (!firstToken) {
                    out.append(' ');
                    map[mapPos++] = pos;
                }
                out.appendCodePoint(digit);
                for (int k = 0; k < digitCount; k++) {
                    map[mapPos++] = pos + k + 1;
                }
                firstToken = false;
                pos += digitCount;

                if (pos < len) {
                    final int separator = text.codePointAt(pos);
                    final int separatorCount = Character.charCount(separator);
                    if (isDigitSeparator(separator)
                            && pos + separatorCount < len
                            && Character.isDigit(text.codePointAt(pos + separatorCount))) {
                        out.append(' ');
                        map[mapPos++] = pos;
                        out.appendCodePoint(separator);
                        for (int k = 0; k < separatorCount; k++) {
                            map[mapPos++] = pos + k + 1;
                        }
                        pos += separatorCount;
                        continue; // the next element must be a digit
                    }
                }

                if (pos >= len || !Character.isDigit(text.codePointAt(pos))) {
                    break;
                }
            }
            i = pos;
        }

        final String spaced = out.toString();
        final int[] offsets = (mapPos == map.length) ? map : Arrays.copyOf(map, mapPos);
        return new Result(spaced, offsets);
    }
}
