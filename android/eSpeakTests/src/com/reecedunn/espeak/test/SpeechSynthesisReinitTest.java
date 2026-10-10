/*
 * Copyright (C) 2026 Jesús Pavón Abián
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
 * Checks that creating another {@link SpeechSynthesis} does not disturb one
 * that is already in use.
 *
 * <p>eSpeak keeps its state in process-wide globals, and the TTS service, the
 * settings screens and CheckVoiceData each create their own SpeechSynthesis.
 * Initializing eSpeak again while the service had it loaded made later
 * requests return almost no audio, so a screen reader went silent as soon as
 * the user moved around the settings.
 *
 * <p>The text is Spanish on purpose: initializing again leaves eSpeak on its
 * default language, so a request in English happens to keep working and hides
 * the problem.
 */
@RunWith(AndroidJUnit4.class)
public class SpeechSynthesisReinitTest extends TextToSpeechTestCase
{
    private static final Locale SPANISH = new Locale("es");
    private static final String TEXT = "Esta frase es lo bastante larga para ocupar varios bloques de audio.";

    private int mBytes;

    private final SpeechSynthesis.SynthReadyCallback mCallback =
            new SpeechSynthesis.SynthReadyCallback()
    {
        @Override
        public void onSynthDataReady(byte[] audioData)
        {
            mBytes += audioData.length;
        }

        @Override
        public void onSynthDataComplete()
        {
        }

        @Override
        public void onSynthWordBoundary(int textPosition, int textLength, int markerInFrames)
        {
        }
    };

    private Voice mVoice;

    private SpeechSynthesis createSynth()
    {
        final SpeechSynthesis synth = new SpeechSynthesis(getContext(), mCallback);
        mVoice = null;
        for (Voice voice : synth.getAvailableVoices())
        {
            if (voice.match(SPANISH) != TextToSpeech.LANG_NOT_SUPPORTED)
            {
                mVoice = voice;
                break;
            }
        }
        assertThat(mVoice, is(notNullValue()));
        return synth;
    }

    /** Speaks the text the way TtsService does: voice and parameters are set on every request. */
    private int speak(SpeechSynthesis synth)
    {
        synth.setVoice(mVoice, VoiceVariant.parseVoiceVariant(VoiceVariant.MALE));
        synth.Rate.setValue(266);
        synth.Pitch.setValue(50);
        synth.PitchRange.setValue(50);
        synth.Volume.setValue(100);
        synth.Punctuation.setValue(0);

        mBytes = 0;
        synth.synthesize(TEXT, false);
        return mBytes;
    }

    @Test
    public void secondInstanceSharesTheSampleRate()
    {
        final SpeechSynthesis first = createSynth();
        final SpeechSynthesis second = createSynth();

        assertThat(first.getSampleRate(), is(greaterThan(0)));
        assertThat(second.getSampleRate(), is(first.getSampleRate()));
    }

    @Test
    public void creatingAnotherInstanceDoesNotSilenceTheFirst()
    {
        final SpeechSynthesis service = createSynth();
        // The first request after start-up differs slightly from the ones that
        // follow, so measure against the second.
        speak(service);
        final int before = speak(service);
        assertThat(before, is(greaterThan(0)));

        // What opening a settings screen does while the service is running.
        createSynth();

        // The same request must still produce about the same audio. Allow a
        // little slack for state that is not part of what is being tested; the
        // failure this guards against is audio that nearly disappears.
        final int after = speak(service);
        assertThat(after, is(greaterThan(before * 9 / 10)));
    }
}
