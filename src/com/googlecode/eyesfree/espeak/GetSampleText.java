/*
 * Copyright (C) 2011 The Android Open Source Project
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

package com.googlecode.eyesfree.espeak;

import android.app.Activity;
import android.content.Intent;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.content.res.Resources.NotFoundException;
import android.os.Bundle;
import android.speech.tts.TextToSpeech;
import android.util.DisplayMetrics;
import android.util.Log;

import java.util.Locale;

/*
 * Returns the sample text string for the language requested
 */
public class GetSampleText extends Activity {
    private static final String TAG = GetSampleText.class.getSimpleName();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        final Locale locale = getLocaleFromIntent(getIntent());
        final Resources res = getResourcesForLocale(this, locale);

        String text = null;

        try {
            text = res.getString(R.string.sample_text, locale.getDisplayName(locale));
        } catch (NotFoundException e) {
            e.printStackTrace();
        }

        final String language = (locale == null) ? "eng" : locale.getISO3Language();

        if (text != null) {
            // Do nothing.
        } else if (language.equals("afr")) {
            text = getString(R.string.afr);
        } else if (language.equals("bos")) {
            text = getString(R.string.bos);
        } else if (language.equals("zho")) {
            text = getString(R.string.zho);
        } else if (language.equals("hrv")) {
            text = getString(R.string.hrv);
        } else if (language.equals("ces")) {
            text = getString(R.string.ces);
        } else if (language.equals("nld")) {
            text = getString(R.string.nld);
        } else if (language.equals("eng")) {
            text = getString(R.string.eng);
        } else if (language.equals("epo")) {
            text = getString(R.string.epo);
        } else if (language.equals("fin")) {
            text = getString(R.string.fin);
        } else if (language.equals("fra")) {
            text = getString(R.string.fra);
        } else if (language.equals("deu")) {
            text = getString(R.string.deu);
        } else if (language.equals("ell")) {
            text = getString(R.string.ell);
        } else if (language.equals("hin")) {
            text = getString(R.string.hin);
        } else if (language.equals("hun")) {
            text = getString(R.string.hun);
        } else if (language.equals("isl")) {
            text = getString(R.string.isl);
        } else if (language.equals("ind")) {
            text = getString(R.string.ind);
        } else if (language.equals("ita")) {
            text = getString(R.string.ita);
        } else if (language.equals("kur")) {
            text = getString(R.string.kur);
        } else if (language.equals("lat")) {
            text = getString(R.string.lat);
        } else if (language.equals("mkd")) {
            text = getString(R.string.mkd);
        } else if (language.equals("nor")) {
            text = getString(R.string.nor);
        } else if (language.equals("pol")) {
            text = getString(R.string.pol);
        } else if (language.equals("por")) {
            text = getString(R.string.por);
        } else if (language.equals("ron")) {
            text = getString(R.string.ron);
        } else if (language.equals("rus")) {
            text = getString(R.string.rus);
        } else if (language.equals("srp")) {
            text = getString(R.string.srp);
        } else if (language.equals("slk")) {
            text = getString(R.string.slk);
        } else if (language.equals("spa")) {
            text = getString(R.string.spa);
        } else if (language.equals("swa")) {
            text = getString(R.string.swa);
        } else if (language.equals("swe")) {
            text = getString(R.string.swe);
        } else if (language.equals("tam")) {
            text = getString(R.string.tam);
        } else if (language.equals("tur")) {
            text = getString(R.string.tur);
        } else if (language.equals("vie")) {
            text = getString(R.string.vie);
        } else if (language.equals("cym")) {
            text = getString(R.string.cym);
        } else {
            Log.e(TAG, "Missing sample text for " + language);
            text = getString(R.string.eng);
        }

        final int result = TextToSpeech.LANG_AVAILABLE;
        final Intent returnData = new Intent();
        returnData.putExtra("sampleText", text);
        setResult(result, returnData);
        finish();
    }

    private static Locale getLocaleFromIntent(Intent intent) {
        if (intent != null) {
            final String language = intent.getStringExtra("language");

            if (language != null) {
                return new Locale(language);
            }
        }

        return Locale.getDefault();
    }

    private static Resources getResourcesForLocale(Activity activity, Locale locale) {
        final Configuration config = activity.getResources().getConfiguration();
        config.locale = locale;

        final DisplayMetrics metrics = new DisplayMetrics();
        activity.getWindowManager().getDefaultDisplay().getMetrics(metrics);

        return new Resources(activity.getAssets(), metrics, config);
    }
}
