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
 * Checks that stopping actually ends a synthesis in progress.
 *
 * <p>{@code espeak_Cancel()} cannot do this in the Android build -- its body is
 * entirely {@code #if USE_ASYNC}, which the build disables -- so the JNI layer
 * has to return SYNTH_ABORT from the synthesis callback instead. Without that,
 * a stop waits for the whole utterance to be synthesized, which is silence for
 * as long as the text the user just moved away from.
 */
@RunWith(AndroidJUnit4.class)
public class SynthesisStopTest extends TextToSpeechTestCase
{
    private static final Locale ENGLISH = new Locale("en");

    /** Long enough that a run to completion takes many buffers, not one or two. */
    private static final String LONG_TEXT =
            "This is a long sentence used to check that stopping works. "
            + "It has to run for long enough that a synthesis carried through "
            + "to the end is clearly distinguishable from one that was cut off "
            + "after the very first buffer of audio was handed over. "
            + "Otherwise the test would pass whether or not the abort happened.";

    private int mDataCallbacks;
    private int mCompleteCallbacks;
    private SpeechSynthesis mSynth;
    private boolean mStopOnFirstBuffer;

    private final SpeechSynthesis.SynthReadyCallback mCallback =
            new SpeechSynthesis.SynthReadyCallback()
    {
        @Override
        public void onSynthDataReady(byte[] audioData)
        {
            mDataCallbacks++;
            if (mStopOnFirstBuffer && mDataCallbacks == 1)
            {
                // Stopping from inside the callback is what TtsService does when
                // the framework stops accepting audio, and it makes the test
                // deterministic: the flag is set before the next buffer is asked
                // for, so no timing window is involved.
                mSynth.stop();
            }
        }

        @Override
        public void onSynthDataComplete()
        {
            mCompleteCallbacks++;
        }

        @Override
        public void onSynthWordBoundary(int textPosition, int textLength, int markerInFrames)
        {
        }
    };

    /** Synthesizes {@code text} with the en voice, optionally stopping partway. */
    private void synthesize(String text, boolean stopOnFirstBuffer)
    {
        mDataCallbacks = 0;
        mCompleteCallbacks = 0;
        mStopOnFirstBuffer = stopOnFirstBuffer;

        if (mSynth == null)
        {
            mSynth = new SpeechSynthesis(getContext(), mCallback);
            // Match on locale rather than voice name, which varies between
            // eSpeak releases.
            Voice en = null;
            for (Voice voice : mSynth.getAvailableVoices())
            {
                if (voice.match(ENGLISH) != TextToSpeech.LANG_NOT_SUPPORTED)
                {
                    en = voice;
                    break;
                }
            }
            assertThat(en, is(notNullValue()));
            mSynth.setVoice(en, VoiceVariant.parseVoiceVariant(VoiceVariant.MALE));
        }

        // synthesize() blocks until eSpeak has finished with this request.
        mSynth.synthesize(text, false);
    }

    @Test
    public void runToCompletionTakesManyBuffers()
    {
        // Establishes what "not stopped" looks like, so the stopped case below
        // is measured against something rather than an arbitrary constant.
        synthesize(LONG_TEXT, false);

        assertThat(mDataCallbacks, is(greaterThan(5)));
        assertThat(mCompleteCallbacks, is(1));
    }

    @Test
    public void stoppingEndsSynthesisImmediately()
    {
        synthesize(LONG_TEXT, true);

        // The buffer that triggered the stop is the only one delivered; the next
        // callback finds the flag set and aborts.
        assertThat(mDataCallbacks, is(1));
    }

    @Test
    public void stoppingStillReportsCompletion()
    {
        synthesize(LONG_TEXT, true);

        // eSpeak returns ENS_SPEECH_STOPPED without a final NULL-buffer
        // callback, so the JNI layer has to report completion itself. Without
        // it TtsService never calls done() and the framework treats the request
        // as still running.
        assertThat(mCompleteCallbacks, is(1));
    }

    @Test
    public void stoppingDoesNotAffectTheNextRequest()
    {
        synthesize(LONG_TEXT, true);
        assertThat(mDataCallbacks, is(1));

        // The stop flag is cleared per espeak_Synth call, so a later request
        // must run to completion again.
        synthesize(LONG_TEXT, false);

        assertThat(mDataCallbacks, is(greaterThan(5)));
        assertThat(mCompleteCallbacks, is(1));
    }
}
