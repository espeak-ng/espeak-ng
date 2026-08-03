/*
 * Copyright (C) 2026
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

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Build;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.speech.tts.TextToSpeech;
import android.speech.tts.UtteranceProgressListener;
import android.util.Log;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import com.reecedunn.espeak.LanguageSettings;
import com.reecedunn.espeak.SpeechSynthesis;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.File;
import java.util.HashSet;
import java.util.List;
import java.util.Locale;
import java.util.Set;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicReference;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.is;
import static org.hamcrest.Matchers.notNullValue;

/**
 * Drives the engine through the real {@link TextToSpeech} client API rather
 * than calling TtsService methods directly, so that the framework's
 * start/done/error contract is exercised the way a screen reader exercises it.
 *
 * Reproduces https://github.com/espeak-ng/espeak-ng/pull/2480#issuecomment-5148567724
 * "if I pressed deselect all langs button, and only checked persian, and then
 * opened my telegram which I have set it to english, reading the content was
 * stopping".
 */
@RunWith(AndroidJUnit4.class)
public class LanguageFilterSynthesisTest {
    private static final String TAG = "LangFilterTest";
    private static final String ENGINE = "com.reecedunn.espeak";

    private static final long INIT_TIMEOUT_S = 60;
    private static final long UTTERANCE_TIMEOUT_S = 20;

    private static final String DONE = "DONE";
    private static final String TIMEOUT = "TIMEOUT (no onDone, no onError)";

    private Context mPrefContext;
    private File mOutDir;
    private TextToSpeech mTts;

    @Before
    public void setUp() throws Exception {
        final Context ctx = InstrumentationRegistry.getInstrumentation().getTargetContext();
        mOutDir = ctx.getCacheDir();
        mPrefContext = (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N)
                ? ctx.createDeviceProtectedStorageContext()
                : ctx;
        clearFilter();

        final CountDownLatch ready = new CountDownLatch(1);
        final AtomicReference<Integer> status = new AtomicReference<Integer>();
        mTts = new TextToSpeech(ctx, new TextToSpeech.OnInitListener() {
            @Override
            public void onInit(int s) {
                status.set(s);
                ready.countDown();
            }
        }, ENGINE);

        assertThat("TTS init timed out",
                ready.await(INIT_TIMEOUT_S, TimeUnit.SECONDS), is(true));
        assertThat("TTS init failed", status.get(), is(TextToSpeech.SUCCESS));
    }

    @After
    public void tearDown() {
        if (mTts != null) {
            mTts.shutdown();
            mTts = null;
        }
        clearFilter();
    }

    private void clearFilter() {
        PreferenceManager.getDefaultSharedPreferences(mPrefContext)
                .edit()
                .remove(LanguageSettings.PREF_SUPPORTED_LANGUAGES)
                .commit();
    }

    /** The id LanguageSettings filters on is Voice.toString(), e.g. "fas". */
    private String persianFilterId() {
        final SpeechSynthesis engine = new SpeechSynthesis(mPrefContext, null);
        final List<com.reecedunn.espeak.Voice> voices = engine.getAvailableVoices();
        for (com.reecedunn.espeak.Voice voice : voices) {
            if (voice.locale.getLanguage().equals(new Locale("fa").getLanguage())) {
                Log.i(TAG, "Persian voice: name=" + voice.name + " id=" + voice.toString());
                return voice.toString();
            }
        }
        return null;
    }

    private void filterTo(String voiceId) {
        final Set<String> selected = new HashSet<String>();
        selected.add(voiceId);
        final SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(mPrefContext);
        prefs.edit().putStringSet(LanguageSettings.PREF_SUPPORTED_LANGUAGES, selected).commit();

        // onGetVoices() calls rebuildAvailableVoices() before returning, so this
        // forces the service to apply the new filter synchronously instead of
        // waiting on the SharedPreferences listener hop to the main thread.
        final int exposed = mTts.getVoices().size();
        Log.i(TAG, "filtered to " + voiceId + "; engine now exposes " + exposed + " voices");
    }

    private android.speech.tts.Voice findVoiceNamed(String name) {
        for (android.speech.tts.Voice voice : mTts.getVoices()) {
            if (voice.getName().equals(name)) {
                return voice;
            }
        }
        return null;
    }

    /**
     * Synthesizes to a file (rather than speak()) so the result does not depend
     * on the emulator having a working audio device. Both paths land in
     * TtsService.onSynthesizeText.
     */
    private String synthesize(String utteranceId) throws Exception {
        final CountDownLatch finished = new CountDownLatch(1);
        final AtomicReference<String> outcome = new AtomicReference<String>(TIMEOUT);

        mTts.setOnUtteranceProgressListener(new UtteranceProgressListener() {
            @Override
            public void onStart(String id) {
                Log.i(TAG, "onStart " + id);
            }

            @Override
            public void onDone(String id) {
                outcome.set(DONE);
                finished.countDown();
            }

            @Override
            public void onError(String id) {
                outcome.set("ERROR");
                finished.countDown();
            }

            @Override
            public void onError(String id, int errorCode) {
                outcome.set("ERROR(" + errorCode + ")");
                finished.countDown();
            }
        });

        final File out = new File(mOutDir, utteranceId + ".wav");
        final int queued = mTts.synthesizeToFile("hello world", new Bundle(), out, utteranceId);
        if (queued != TextToSpeech.SUCCESS) {
            return "QUEUE_REJECTED(" + queued + ")";
        }

        finished.await(UTTERANCE_TIMEOUT_S, TimeUnit.SECONDS);
        Log.i(TAG, "utterance " + utteranceId + " -> " + outcome.get());
        return outcome.get();
    }

    /**
     * Path A: the client selected a language (what TextToSpeech.setLanguage does,
     * and what the f46429c7 fallback covers).
     */
    @Test
    public void testEnglishLanguageRequestAfterFilteringToPersian() throws Exception {
        assertThat(mTts.setLanguage(Locale.US) >= TextToSpeech.LANG_AVAILABLE, is(true));
        assertThat("baseline English utterance failed", synthesize("lang-before"), is(DONE));

        final String fa = persianFilterId();
        assertThat("no Persian voice found", fa, is(notNullValue()));
        filterTo(fa);

        assertThat("English utterance never completed after filtering to Persian",
                synthesize("lang-after"), is(DONE));
    }

    /**
     * Path C: the client never called setLanguage() or setVoice(). Determines
     * whether the framework attaches a voice name by default, i.e. whether
     * selectLanguageWithFallback() is reachable at all on API 21+.
     */
    @Test
    public void testDefaultRequestAfterFilteringToPersian() throws Exception {
        assertThat("baseline default utterance failed", synthesize("default-before"), is(DONE));

        final String fa = persianFilterId();
        assertThat("no Persian voice found", fa, is(notNullValue()));
        filterTo(fa);

        assertThat("default utterance never completed after filtering to Persian",
                synthesize("default-after"), is(DONE));
    }

    /**
     * Path B: the client selected a concrete voice, then the user filtered that
     * voice away. TtsService.selectVoice() sends this to onLoadVoice(), which
     * has no fallback.
     */
    @Test
    public void testEnglishVoiceRequestAfterFilteringToPersian() throws Exception {
        final android.speech.tts.Voice english = findVoiceNamed("en-us");
        assertThat("no en-us voice exposed", english, is(notNullValue()));
        assertThat(mTts.setVoice(english), is(TextToSpeech.SUCCESS));
        assertThat("baseline English utterance failed", synthesize("voice-before"), is(DONE));

        final String fa = persianFilterId();
        assertThat("no Persian voice found", fa, is(notNullValue()));
        filterTo(fa);

        assertThat("English voice utterance never completed after filtering to Persian",
                synthesize("voice-after"), is(DONE));
    }
}
