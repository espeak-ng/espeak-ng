/*
 * Copyright (C) 2026 eSpeak NG contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */

package com.reecedunn.espeak.test;

import android.content.Context;
import android.os.Build;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import com.reecedunn.espeak.CustomLexicon;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.is;

@RunWith(AndroidJUnit4.class)
public class CustomLexiconTest {
    private Context mContext;

    @Before
    public void setUp() {
        Context target = InstrumentationRegistry.getInstrumentation().getTargetContext();
        mContext = Build.VERSION.SDK_INT >= Build.VERSION_CODES.N
                ? target.createDeviceProtectedStorageContext() : target;
        CustomLexicon.clear(mContext);
    }

    @After
    public void tearDown() {
        CustomLexicon.clear(mContext);
    }

    @Test
    public void replacesWholeArabicWordOnly() throws Exception {
        CustomLexicon.saveMapping(mContext, "ar", "نشر", "نَشَرَ");
        CustomLexicon.ReplacementResult result = CustomLexicon.apply(
                mContext, "ar", "منشور نشر النشر");
        assertThat(result.text, is("منشور نَشَرَ النشر"));
        assertThat(CustomLexicon.entryCount(mContext), is(1));
    }

    @Test
    public void mappingsAreLanguageScopedWithVariantFallback() throws Exception {
        CustomLexicon.saveMapping(mContext, "en", "read", "reed");
        assertThat(CustomLexicon.apply(mContext, "en-us", "read this").text,
                is("reed this"));
        assertThat(CustomLexicon.apply(mContext, "ar", "read this").text,
                is("read this"));
    }

    @Test
    public void replacementIsAppliedOnlyOnce() throws Exception {
        CustomLexicon.saveMapping(mContext, "en", "alpha", "beta");
        CustomLexicon.saveMapping(mContext, "en", "beta", "gamma");
        assertThat(CustomLexicon.apply(mContext, "en", "alpha beta").text,
                is("beta gamma"));
    }

    @Test
    public void longestPhraseWins() throws Exception {
        CustomLexicon.saveMapping(mContext, "en", "new", "old");
        CustomLexicon.saveMapping(mContext, "en", "new york", "Newcastle");
        assertThat(CustomLexicon.apply(mContext, "en", "new york, new day").text,
                is("Newcastle, old day"));
    }

    @Test
    public void transformedOffsetsMapBackToOriginalWord() throws Exception {
        CustomLexicon.saveMapping(mContext, "en", "cat", "hippopotamus");
        CustomLexicon.ReplacementResult result = CustomLexicon.apply(
                mContext, "en", "say cat now");
        assertThat(result.text, is("say hippopotamus now"));
        assertThat(result.originalOffsetForCodePoint(4), is(4));
        assertThat(result.originalOffsetForCodePoint(16), is(7));
        assertThat(result.originalOffsetForCodePoint(17), is(8));
    }

    @Test
    public void explicitSsmlIsNotModified() throws Exception {
        CustomLexicon.saveMapping(mContext, "en", "speak", "broken");
        String ssml = "<speak>please speak</speak>";
        assertThat(CustomLexicon.apply(mContext, "en", ssml).text, is(ssml));
    }
}
