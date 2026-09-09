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

package com.reecedunn.espeak.test;

import com.reecedunn.espeak.UnicodeNormalization;

import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.Test;
import org.junit.runner.RunWith;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.*;

/**
 * Checks the NFKC normalization applied to synthesis input and the offset map
 * used to keep {@code SynthesisCallback.rangeStart} anchored to the text the
 * caller supplied.
 */
@RunWith(AndroidJUnit4.class)
public class UnicodeNormalizationTest
{
    @Test
    public void plainTextIsAlreadyNormalized()
    {
        assertThat(UnicodeNormalization.normalize(""), is(nullValue()));
        assertThat(UnicodeNormalization.normalize("Hello, world!"), is(nullValue()));
        assertThat(UnicodeNormalization.normalize("héllo wörld"), is(nullValue()));
        // Composed text with an astral emoji is normalized too; the fast path
        // must not be confused by surrogate pairs.
        assertThat(UnicodeNormalization.normalize("ab 😀 cd"), is(nullValue()));
    }

    @Test
    public void mathematicalAlphanumericSymbols()
    {
        // The examples from issue #2485.
        assertThat(UnicodeNormalization.normalize("𝖁𝕰𝕹𝖀𝕾").text, is("VENUS"));
        assertThat(UnicodeNormalization.normalize("𝘛𝘢𝘳𝘢").text, is("Tara"));
        assertThat(UnicodeNormalization.normalize("𝐂𝐨𝐝𝐞").text, is("Code"));
    }

    @Test
    public void fullwidthForms()
    {
        assertThat(UnicodeNormalization.normalize("Ｈｅｌｌｏ").text, is("Hello"));
    }

    @Test
    public void negativeCircledLettersUseSupplementaryMapping()
    {
        // U+1F150.. has no compatibility decomposition, so plain NFKC would
        // leave it alone; the supplementary mapping must kick in.
        assertThat(UnicodeNormalization.normalize("🅐🅑🅒").text, is("ABC"));
    }

    @Test
    public void negativeSquaredLettersPreserveEmoji()
    {
        // 🅾 has emoji semantics (blood type O) and must survive, while the
        // surrounding squared letters normalize.
        assertThat(UnicodeNormalization.normalize("🅲🅾🅳🅴").text, is("C🅾DE"));
        // Text consisting only of the excluded emoji needs no normalization.
        assertThat(UnicodeNormalization.normalize("🅰🅱🅾🅿"), is(nullValue()));
    }

    @Test
    public void combiningMarksCompose()
    {
        // The input is decomposed ("e" + combining acute); NFKC composes it
        // to a single "é", so the two literals differ despite looking alike.
        assertThat(UnicodeNormalization.normalize("café").text, is("café"));
    }

    @Test
    public void offsetsForStyledWord()
    {
        // "ab 𝖁𝕰𝕹𝖀𝕾 cd": each styled letter is one code point but two
        // UTF-16 units, so the original is 16 units long and the normalized
        // form ("ab VENUS cd") is 11.
        final String text = "ab 𝖁𝕰𝕹𝖀𝕾 cd";
        final UnicodeNormalization.Result result = UnicodeNormalization.normalize(text);
        assertThat(result, is(notNullValue()));
        assertThat(result.text, is("ab VENUS cd"));

        // "ab " is untouched.
        assertThat(result.toOriginalOffset(0), is(0));
        assertThat(result.toOriginalOffset(3), is(3));
        // The word "VENUS" spans [3, 8) in the normalized text and must map
        // back to the full styled span [3, 13) in the original.
        assertThat(result.toOriginalOffset(8), is(13));
        // "cd" after the styled word.
        assertThat(result.toOriginalOffset(9), is(14));
        assertThat(result.toOriginalOffset(11), is(16));
    }

    @Test
    public void offsetsForExpandingLigature()
    {
        // "ĳ" normalizes to "ij": the text gets longer, and offsets inside
        // the expansion must not run past the original character.
        final UnicodeNormalization.Result result = UnicodeNormalization.normalize("ĳs");
        assertThat(result, is(notNullValue()));
        assertThat(result.text, is("ijs"));

        assertThat(result.toOriginalOffset(0), is(0));
        // Both the interior of the expansion and its end map to the end of
        // the ligature, so a word ending there covers it exactly.
        assertThat(result.toOriginalOffset(1), is(1));
        assertThat(result.toOriginalOffset(2), is(1));
        assertThat(result.toOriginalOffset(3), is(2));
    }

    @Test
    public void offsetsAreMonotonic()
    {
        final String text = "a Ｗｉｄｅ ĳ 𝖁 b";
        final UnicodeNormalization.Result result = UnicodeNormalization.normalize(text);
        assertThat(result, is(notNullValue()));

        int previous = 0;
        for (int i = 0; i <= result.text.length(); ++i) {
            final int offset = result.toOriginalOffset(i);
            assertThat(offset, is(greaterThanOrEqualTo(previous)));
            assertThat(offset, is(lessThanOrEqualTo(text.length())));
            previous = offset;
        }
        assertThat(result.toOriginalOffset(result.text.length()), is(text.length()));
    }

    @Test
    public void outOfRangeOffsetsAreClamped()
    {
        final UnicodeNormalization.Result result = UnicodeNormalization.normalize("𝖁");
        assertThat(result, is(notNullValue()));
        assertThat(result.text, is("V"));
        assertThat(result.toOriginalOffset(-1), is(0));
        assertThat(result.toOriginalOffset(100), is(2));
    }
}
