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

import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;

/**
 * Host-side (JVM) tests for {@link DigitSpacing}. The transform is plain Java,
 * so these run without a device: {@code ./gradlew testDebugUnitTest}.
 */
public class DigitSpacingTest {

    @Test
    public void plainDigitsAreSeparated() {
        assertEquals("1 2 3", DigitSpacing.separate("123").text);
    }

    @Test
    public void arabicIndicDigitsAreSeparated() {
        assertEquals("١ ٢ ٣", DigitSpacing.separate("١٢٣").text);
    }

    @Test
    public void decimalPointBecomesItsOwnToken() {
        // "3.1" must not reach the engine as a decimal number; each digit is
        // read individually and the point stands alone (a pause, or "point"
        // when punctuation is being read).
        assertEquals("3 . 1 4", DigitSpacing.separate("3.14").text);
    }

    @Test
    public void thousandsSeparatorBecomesItsOwnToken() {
        // "1,2" must not reach the engine as "one comma two".
        assertEquals("1 , 2 3 4", DigitSpacing.separate("1,234").text);
    }

    @Test
    public void phoneNumberIsSeparated() {
        assertEquals("+1 (5 5 5) 1 2 3 - 4 5 6 7",
                DigitSpacing.separate("+1 (555) 123-4567").text);
    }

    @Test
    public void surroundingTextIsUntouched() {
        assertEquals("Code 4 2!", DigitSpacing.separate("Code 42!").text);
    }

    @Test
    public void separatorOutsideADigitRunIsUntouched() {
        assertEquals("3 . 1 4.", DigitSpacing.separate("3.14.").text);
        // A separator at the end of a digit run stays attached punctuation;
        // the digits themselves are still separated: "1 2." not "12.".
        assertEquals("1 2.", DigitSpacing.separate("12.").text);
    }

    @Test
    public void offsetMapForPlainDigits() {
        final DigitSpacing.Result result = DigitSpacing.separate("123");
        // "1 2 3": boundaries 0->0, 1->1, 2->1, 3->2, 4->2, 5->3.
        assertEquals(0, result.toOriginalOffset(0));
        assertEquals(1, result.toOriginalOffset(1));
        assertEquals(1, result.toOriginalOffset(2));
        assertEquals(2, result.toOriginalOffset(3));
        assertEquals(2, result.toOriginalOffset(4));
        assertEquals(3, result.toOriginalOffset(5));
    }

    @Test
    public void offsetMapAroundDigitRun() {
        final DigitSpacing.Result result = DigitSpacing.separate("ab12cd");
        // "ab1 2cd": boundaries 0->0, 1->1, 2->2, 3->3, 4->3, 5->4, 6->5, 7->6.
        assertEquals(2, result.toOriginalOffset(2));
        assertEquals(4, result.toOriginalOffset(5));
        assertEquals(6, result.toOriginalOffset(7));
    }

    @Test
    public void offsetMapWithSeparatorToken() {
        final DigitSpacing.Result result = DigitSpacing.separate("3.14");
        // "3 . 1 4": the digit "1" starts at input offset 2 and the run ends
        // at input offset 4.
        assertEquals(2, result.toOriginalOffset(4));
        assertEquals(4, result.toOriginalOffset(7));
    }

    @Test
    public void offsetMapComposesWithUnicodeNormalization() {
        // NFKC folds the fullwidth digits first; the digit separator then
        // spaces the folded ASCII digits. Composing both maps must land on
        // the caller's text again (this mirrors TtsService.onSynthWordBoundary).
        final UnicodeNormalization.Result normalized = UnicodeNormalization.normalize("１２３");
        final DigitSpacing.Result digits = DigitSpacing.separate(normalized.text);
        assertEquals("1 2 3", digits.text);
        final int start = normalized.toOriginalOffset(digits.toOriginalOffset(2));
        final int end = normalized.toOriginalOffset(digits.toOriginalOffset(3));
        assertEquals(1, start);
        assertEquals(2, end);
    }

    @Test
    public void emptyAndNullInput() {
        assertEquals("", DigitSpacing.separate("").text);
        assertNull(DigitSpacing.separate(null).text);
    }
}
