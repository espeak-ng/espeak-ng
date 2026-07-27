/*
 * Copyright (C) 2026 Reece H. Dunn
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

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import com.reecedunn.espeak.SpeechSynthesis;
import com.reecedunn.espeak.Voice;
import com.reecedunn.espeak.VoiceVariant;

import android.speech.tts.TextToSpeech;

import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.Test;
import org.junit.runner.RunWith;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.*;

/**
 * Checks the word boundary events that back {@code SynthesisCallback.rangeStart}.
 *
 * <p>eSpeak reports word positions as 1-based Unicode code point indices, while
 * the Android API expects 0-based UTF-16 indices, so the interesting cases are
 * text outside the Basic Multilingual Plane and text with multi-byte characters.
 */
@RunWith(AndroidJUnit4.class)
public class WordBoundaryTest extends TextToSpeechTestCase
{
    private static final Locale ENGLISH = new Locale("en");

    private static class Word
    {
        final int position;
        final int length;
        final int marker;

        Word(int position, int length, int marker)
        {
            this.position = position;
            this.length = length;
            this.marker = marker;
        }
    }

    private final List<Word> mWords = new ArrayList<Word>();

    private final SpeechSynthesis.SynthReadyCallback mCallback =
            new SpeechSynthesis.SynthReadyCallback()
    {
        @Override
        public void onSynthDataReady(byte[] audioData)
        {
        }

        @Override
        public void onSynthDataComplete()
        {
        }

        @Override
        public void onSynthWordBoundary(int textPosition, int textLength, int markerInFrames)
        {
            mWords.add(new Word(textPosition, textLength, markerInFrames));
        }
    };

    /** Synthesizes {@code text} with the en voice and returns the words reported. */
    private List<Word> wordsFor(String text)
    {
        mWords.clear();

        SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        // Match on locale rather than voice name, which varies between eSpeak
        // releases.
        Voice en = null;
        for (Voice voice : synth.getAvailableVoices())
        {
            if (voice.match(ENGLISH) != TextToSpeech.LANG_NOT_SUPPORTED)
            {
                en = voice;
                break;
            }
        }
        assertThat(en, is(notNullValue()));

        synth.setVoice(en, VoiceVariant.parseVoiceVariant(VoiceVariant.MALE));
        // synthesize() blocks until eSpeak has finished, so every event for this
        // request has been delivered by the time it returns.
        synth.synthesize(text, false);
        return mWords;
    }

    /**
     * Applies the same conversion as {@code TtsService}: eSpeak's 1-based code
     * point position and code point length to a UTF-16 substring.
     */
    private static String substringFor(String text, Word word)
    {
        int start = text.offsetByCodePoints(0, word.position - 1);
        int end = text.offsetByCodePoints(start, word.length);
        return text.substring(start, end);
    }

    @Test
    public void ascii()
    {
        String text = "Hello world again";
        List<Word> words = wordsFor(text);

        assertThat(words.size(), is(3));
        assertThat(substringFor(text, words.get(0)), is("Hello"));
        assertThat(substringFor(text, words.get(1)), is("world"));
        assertThat(substringFor(text, words.get(2)), is("again"));
    }

    @Test
    public void multiByteCharactersAreCountedAsOne()
    {
        // Each accented letter is two UTF-8 bytes but a single code point, so a
        // byte-based reading of text_position would drift on the later words.
        String text = "héllo wörld tëst";
        List<Word> words = wordsFor(text);

        assertThat(words.size(), is(3));
        assertThat(substringFor(text, words.get(0)), is("héllo"));
        assertThat(substringFor(text, words.get(1)), is("wörld"));
        assertThat(substringFor(text, words.get(2)), is("tëst"));
    }

    @Test
    public void surrogatePairsAreNotSplit()
    {
        // The emoji is one code point but two UTF-16 units. Treating eSpeak's
        // position as a UTF-16 index would yield half a surrogate pair.
        String text = "ab 😀 cd";
        List<Word> words = wordsFor(text);

        assertThat(words.size(), greaterThanOrEqualTo(2));
        assertThat(substringFor(text, words.get(0)), is("ab"));
        assertThat(substringFor(text, words.get(1)), is("😀"));
        // The last word must still land on "cd" despite the surrogate pair.
        assertThat(substringFor(text, words.get(words.size() - 1)), is("cd"));
    }

    @Test
    public void markersAreNonDecreasingAndStartAtZero()
    {
        List<Word> words = wordsFor("Hello world again");

        assertThat(words.isEmpty(), is(false));
        assertThat(words.get(0).marker, is(0));
        for (int i = 1; i < words.size(); ++i)
        {
            assertThat(words.get(i).marker, greaterThanOrEqualTo(words.get(i - 1).marker));
        }
    }

    @Test
    public void markersRestartForEachRequest()
    {
        wordsFor("Hello world again");
        // frames_delivered is reset per espeak_Synth call, so a second request
        // must not continue counting from the first.
        List<Word> words = wordsFor("Hello world again");

        assertThat(words.isEmpty(), is(false));
        assertThat(words.get(0).marker, is(0));
    }
}
