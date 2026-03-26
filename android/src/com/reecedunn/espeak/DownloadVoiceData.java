/*
 * Copyright (C) 2022 Beka Gozalishvili
 * Copyright (C) 2012-2013 Reece H. Dunn
 * Copyright (C) 2009 The Android Open Source Project
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

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.accessibility.AccessibilityEvent;

public class DownloadVoiceData extends Activity {
    public static final String BROADCAST_LANGUAGES_UPDATED = "com.reecedunn.espeak.LANGUAGES_UPDATED";

    private AsyncExtract mAsyncExtract;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.download_voice_data);
        final Context storageContext = EspeakApp.getStorageContext();

        mAsyncExtract = new AsyncExtract(storageContext) {
            @Override
            protected void onPostExecute(Integer result) {
                switch (result) {
                    case RESULT_OK:
                        final Intent intent = new Intent(BROADCAST_LANGUAGES_UPDATED);
                        sendBroadcast(intent);
                        break;
                    case RESULT_CANCELED:
                        break;
                }

                setResult(result);
                finish();
            }
        };

        mAsyncExtract.execute();

        findViewById(R.id.installing_voice_data)
                .sendAccessibilityEvent(AccessibilityEvent.TYPE_VIEW_FOCUSED);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        mAsyncExtract.cancel(true);
    }

    private static class AsyncExtract extends AsyncTask<Void, Void, Integer> {
        private final Context mContext;

        public AsyncExtract(Context context) {
            mContext = context;
        }

        @Override
        protected Integer doInBackground(Void... params) {
            if (CheckVoiceData.extractVoiceData(mContext)) {
                return RESULT_OK;
            }
            return RESULT_CANCELED;
        }
    }
}
