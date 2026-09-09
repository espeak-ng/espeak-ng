/*
 * Copyright (C) 2026 Alexandr Epaneshnikov
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

import java.text.BreakIterator;
import java.text.Normalizer;

/**
 * NFKC normalization of synthesis input, so that stylized Unicode text (such
 * as the Mathematical Alphanumeric Symbols popular on social media, fullwidth
 * forms, and enclosed letters) is spoken as the words it spells rather than
 * one codepoint at a time.
 *
 * <p>This mirrors what NVDA and speech-dispatcher do before text reaches a
 * synthesizer. Like NVDA, a supplementary mapping handles the negative
 * circled and negative squared Latin letters, which are decorative but have
 * no compatibility decomposition in Unicode.
 *
 * <p>NFKC can change the length of the text, but word boundary events must be
 * reported against the text the caller supplied, so normalization also builds
 * an offset map from the normalized string back to the original one.
 */
public final class UnicodeNormalization {
    private UnicodeNormalization() {
    }

    /** A normalized string together with its offset map into the original. */
    public static final class Result {
        /** The normalized text, to be handed to the synthesizer. */
        public final String text;

        /**
         * Boundary map of size {@code text.length() + 1}: entry {@code i} is
         * the UTF-16 offset in the original string corresponding to UTF-16
         * offset {@code i} in the normalized string. Within a run the
         * normalization changed, interior positions map to the end of the
         * original run, so a word ending inside it covers the whole run and
         * mapped ranges never invert.
         */
        private final int[] mOffsets;

        Result(String text, int[] offsets) {
            this.text = text;
            mOffsets = offsets;
        }

        /** Maps a UTF-16 offset in {@link #text} to one in the original string. */
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
     * Normalizes {@code text} to NFKC, including the supplementary mapping.
     *
     * @return the normalized text with its offset map, or {@code null} when
     * the text is already normalized and can be used as-is (the common case,
     * where exact identity of offsets is guaranteed).
     */
    public static Result normalize(String text) {
        if (!hasSupplementaryMappings(text)
                && Normalizer.isNormalized(text, Normalizer.Form.NFKC)) {
            return null;
        }

        // Normalize one grapheme cluster at a time. Canonical composition
        // never crosses a cluster boundary (combining marks belong to their
        // base's cluster), so this matches whole-string NFKC for real text
        // while keeping an unambiguous offset for every cluster.
        final BreakIterator graphemes = BreakIterator.getCharacterInstance();
        graphemes.setText(text);

        final StringBuilder normalized = new StringBuilder(text.length());
        int[] offsets = new int[text.length() + 1];
        int offsetCount = 0;

        int start = graphemes.first();
        for (int end = graphemes.next(); end != BreakIterator.DONE;
                start = end, end = graphemes.next()) {
            final String part = text.substring(start, end);
            final String norm = Normalizer.normalize(
                    applySupplementaryMappings(part), Normalizer.Form.NFKC);
            normalized.append(norm);

            if (offsetCount + norm.length() >= offsets.length) {
                offsets = grow(offsets, offsetCount + norm.length() + 1);
            }
            if (norm.equals(part)) {
                for (int i = 0; i < norm.length(); ++i) {
                    offsets[offsetCount++] = start + i;
                }
            } else if (!norm.isEmpty()) {
                offsets[offsetCount++] = start;
                for (int i = 1; i < norm.length(); ++i) {
                    offsets[offsetCount++] = end;
                }
            }
        }
        offsets[offsetCount++] = text.length();

        if (offsetCount != offsets.length) {
            final int[] trimmed = new int[offsetCount];
            System.arraycopy(offsets, 0, trimmed, 0, offsetCount);
            offsets = trimmed;
        }
        return new Result(normalized.toString(), offsets);
    }

    private static int[] grow(int[] array, int minCapacity) {
        final int[] grown = new int[Math.max(array.length * 2, minCapacity)];
        System.arraycopy(array, 0, grown, 0, array.length);
        return grown;
    }

    /**
     * Maps one decorative code point that NFKC leaves alone, or returns it
     * unchanged. Same ranges and emoji exclusions as NVDA's supplementary
     * normalization table.
     */
    private static int mapSupplementaryCodePoint(int codePoint) {
        // Negative Circled Latin Capital Letters.
        if (codePoint >= 0x1F150 && codePoint <= 0x1F169) {
            return 'A' + (codePoint - 0x1F150);
        }
        // Negative Squared Latin Capital Letters, except the four that carry
        // emoji semantics: 🅰 🅱 🅾 🅿 (blood types and the parking sign).
        if (codePoint >= 0x1F170 && codePoint <= 0x1F189) {
            switch (codePoint) {
                case 0x1F170:
                case 0x1F171:
                case 0x1F17E:
                case 0x1F17F:
                    return codePoint;
            }
            return 'A' + (codePoint - 0x1F170);
        }
        return codePoint;
    }

    private static boolean hasSupplementaryMappings(String text) {
        for (int i = 0; i < text.length(); ) {
            final int codePoint = text.codePointAt(i);
            if (mapSupplementaryCodePoint(codePoint) != codePoint) {
                return true;
            }
            i += Character.charCount(codePoint);
        }
        return false;
    }

    private static String applySupplementaryMappings(String part) {
        StringBuilder mapped = null;
        for (int i = 0; i < part.length(); ) {
            final int codePoint = part.codePointAt(i);
            final int replacement = mapSupplementaryCodePoint(codePoint);
            if (replacement != codePoint && mapped == null) {
                mapped = new StringBuilder(part.length());
                mapped.append(part, 0, i);
            }
            if (mapped != null) {
                mapped.appendCodePoint(replacement);
            }
            i += Character.charCount(codePoint);
        }
        return (mapped == null) ? part : mapped.toString();
    }
}
