/*
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

package com.googlecode.eyesfree.espeak;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.view.accessibility.AccessibilityEvent;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.LinkedList;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class DownloadVoiceData extends Activity {
    public static final String BROADCAST_LANGUAGES_UPDATED = "com.googlecode.eyesfree.espeak.LANGUAGES_UPDATED";

    private static final File LEGACY_DATA = new File(
            Environment.getExternalStorageDirectory(), "espeak-data");

    private AsyncExtract mAsyncExtract;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.download_voice_data);

        final File dataPath = CheckVoiceData.getDataPath(this).getParentFile();

        mAsyncExtract = new AsyncExtract(this, R.raw.espeakdata, dataPath) {
            @Override
            protected void onPostExecute(Integer result) {
                onLanguagesInstalled(result);
            }
        };

        mAsyncExtract.execute();

        // Send a fake accessibility event so the user knows what's going on.
        findViewById(R.id.installing_voice_data)
                .sendAccessibilityEvent(AccessibilityEvent.TYPE_VIEW_FOCUSED);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        mAsyncExtract.cancel(true);
    }

    private void onLanguagesInstalled(int resultCode) {
        switch (resultCode) {
            case RESULT_OK:
                final Intent intent = new Intent(BROADCAST_LANGUAGES_UPDATED);
                sendBroadcast(intent);
                new AsyncCleanup().execute(LEGACY_DATA);
                break;
            case RESULT_CANCELED:
                // Do nothing?
                break;
        }

        setResult(resultCode);
        finish();
    }

    private static void clearContents(File directory) {
        if (!directory.exists() || !directory.isDirectory()) {
            return;
        }

        final File[] children = directory.listFiles();

        for (File child : children) {
            if (child.isDirectory()) {
                clearContents(child);
            }

            child.delete();
        }
    }

    private static class AsyncCleanup extends AsyncTask<File, Void, Void> {
        @Override
        protected Void doInBackground(File... params) {
            for (File directory : params) {
                clearContents(directory);
            }

            return null;
        }
    }

    private static class AsyncExtract extends AsyncTask<Void, Void, Integer> {
        private final LinkedList<File> mExtractedFiles = new LinkedList<File>();

        private final Context mContext;
        private final int mRawResId;
        private final File mOutput;

        public AsyncExtract(Context context, int rawResId, File output) {
            mContext = context;
            mRawResId = rawResId;
            mOutput = output;
        }

        @Override
        protected Integer doInBackground(Void... params) {
            clearContents(CheckVoiceData.getDataPath(mContext));

            final InputStream stream = mContext.getResources().openRawResource(mRawResId);
            final ZipInputStream zipStream = new ZipInputStream(new BufferedInputStream(stream));

            boolean successful = false;

            try {
                extractEntries(zipStream);
                successful = true;
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                try {
                    zipStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            if (isCancelled() || !successful) {
                removeExtractedFiles();
                return RESULT_CANCELED;
            }

            return RESULT_OK;
        }

        private void extractEntries(ZipInputStream zipStream) throws IOException {
            final byte[] buffer = new byte[10240];

            int bytesRead;
            ZipEntry entry;

            while (!isCancelled() && ((entry = zipStream.getNextEntry()) != null)) {
                final File outputFile = new File(mOutput, entry.getName());

                mExtractedFiles.add(outputFile);

                if (entry.isDirectory()) {
                    outputFile.mkdirs();
                    doChmod(outputFile);
                    continue;
                }

                // Ensure the target path exists.
                outputFile.getParentFile().mkdirs();

                final FileOutputStream outputStream = new FileOutputStream(outputFile);

                while (!isCancelled() && ((bytesRead = zipStream.read(buffer)) != -1)) {
                    outputStream.write(buffer, 0, bytesRead);
                }

                outputStream.close();
                zipStream.closeEntry();

                // Make sure the output file is readable.
                doChmod(outputFile);
            }
        }

        private void removeExtractedFiles() {
            for (File extractedFile : mExtractedFiles) {
                if (!extractedFile.isDirectory()) {
                    extractedFile.delete();
                }
            }

            mExtractedFiles.clear();
        }

        private static void doChmod(File file) {
            try {
                Runtime.getRuntime().exec("/system/bin/chmod 755 " + file.getAbsolutePath());
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
