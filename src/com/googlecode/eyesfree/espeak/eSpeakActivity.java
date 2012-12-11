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
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.speech.tts.TextToSpeech;
import android.view.View;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.Locale;

public class eSpeakActivity extends Activity {
    private static final String ACTION_TTS_SETTINGS = "com.android.settings.TTS_SETTINGS";

    /** Handler code for TTS initialization hand-off. */
    private static final int TTS_INITIALIZED = 1;

    private static final int REQUEST_CHECK = 1;
    private static final int REQUEST_DOWNLOAD = 2;
    private static final int REQUEST_DEFAULT = 3;

    private static final int DIALOG_SET_DEFAULT = 1;
    private static final int DIALOG_DOWNLOAD_FAILED = 2;
    private static final int DIALOG_ERROR = 3;

    private enum State {
        LOADING,
        FAILURE,
        SUCCESS
    }

    private boolean mDownloadedVoiceData;
    private ArrayList<String> mVoices;
    private TextToSpeech mTts;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.main);

        setState(State.LOADING);
        manageSettingVisibility();
        checkVoiceData();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();

        if (mTts != null) {
            mTts.shutdown();
        }
    }

    /**
     * Sets the UI state.
     *
     * @param state The current state.
     */
    private void setState(State state) {
        findViewById(R.id.loading).setVisibility((state == State.LOADING) ? View.VISIBLE
                : View.GONE);
        findViewById(R.id.success).setVisibility((state == State.SUCCESS) ? View.VISIBLE
                : View.GONE);
        findViewById(R.id.failure).setVisibility((state == State.FAILURE) ? View.VISIBLE
                : View.GONE);
    }

    /**
     * Launcher the voice data verifier.
     */
    private void checkVoiceData() {
        final Intent checkIntent = new Intent(this, CheckVoiceData.class);

        startActivityForResult(checkIntent, REQUEST_CHECK);
    }

    /**
     * Launches the voice data installer.
     */
    private void downloadVoiceData() {
        final Intent checkIntent = new Intent(this, DownloadVoiceData.class);

        startActivityForResult(checkIntent, REQUEST_DOWNLOAD);
    }

    /**
     * Initializes the TTS engine.
     */
    private void initializeEngine() {
        mTts = new TextToSpeech(this, mInitListener);
    }

    /**
     * Hides preferences according to SDK level.
     */
    private void manageSettingVisibility() {
        if (Build.VERSION.SDK_INT < 14) {
            // Hide the eSpeak setting button on pre-ICS.
            findViewById(R.id.engineSettings).setVisibility(View.GONE);
        }
    }

    /**
     * Handles the result of voice data verification. If verification fails
     * following a successful installation, displays an error dialog. Otherwise,
     * either launches the installer or attempts to initialize the TTS engine.
     *
     * @param resultCode The result of voice data verification.
     * @param data The intent containing available voices.
     */
    private void onDataChecked(int resultCode, Intent data) {
        if (resultCode != TextToSpeech.Engine.CHECK_VOICE_DATA_PASS) {
            if (mDownloadedVoiceData) {
                setState(State.FAILURE);
                showDialog(DIALOG_ERROR);
            } else {
                downloadVoiceData();
            }
            return;
        }

        mVoices = data.getStringArrayListExtra(TextToSpeech.Engine.EXTRA_AVAILABLE_VOICES);

        initializeEngine();
    }

    /**
     * Handles the result of voice data installation. Either shows a failure
     * dialog or launches the voice data verifier.
     *
     * @param resultCode
     */
    private void onDataDownloaded(int resultCode) {
        if (resultCode != RESULT_OK) {
            setState(State.FAILURE);
            showDialog(DIALOG_DOWNLOAD_FAILED);
            return;
        }

        mDownloadedVoiceData = true;

        checkVoiceData();
    }

    /**
     * Handles the result of TTS engine initialization. Either displays an error
     * dialog or populates the activity's UI.
     *
     * @param status The TTS engine initialization status.
     */
    private void onInitialized(int status) {
        if (!getPackageName().equals(mTts.getDefaultEngine())) {
            showDialog(DIALOG_SET_DEFAULT);
            return;
        }

        if (status == TextToSpeech.ERROR || mVoices == null) {
            setState(State.FAILURE);
            showDialog(DIALOG_ERROR);
            return;
        }

        final Locale ttsLocale = mTts.getLanguage();
        final String localeText = getString(
                R.string.current_tts_locale, ttsLocale.getDisplayName());
        final TextView currentLocale = (TextView) findViewById(R.id.currentLocale);
        currentLocale.setText(localeText);

        final String voicesText = getString(R.string.available_voices, mVoices.size());
        final TextView availableVoices = (TextView) findViewById(R.id.availableVoices);
        availableVoices.setText(voicesText);

        findViewById(R.id.ttsSettings).setOnClickListener(mOnClickListener);
        findViewById(R.id.engineSettings).setOnClickListener(mOnClickListener);

        setState(State.SUCCESS);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case REQUEST_CHECK:
                onDataChecked(resultCode, data);
                break;
            case REQUEST_DOWNLOAD:
                onDataDownloaded(resultCode);
                break;
            case REQUEST_DEFAULT:
                initializeEngine();
                break;
        }
    }

    @Override
    protected Dialog onCreateDialog(int id) {
        switch (id) {
            case DIALOG_SET_DEFAULT:
                return new AlertDialog.Builder(this).setTitle(R.string.app_name)
                        .setMessage(R.string.set_default_message)
                        .setNegativeButton(android.R.string.no, mFinishClickListener)
                        .setPositiveButton(android.R.string.ok, mDialogClickListener).create();
            case DIALOG_DOWNLOAD_FAILED:
                return new AlertDialog.Builder(this).setTitle(R.string.app_name)
                        .setMessage(R.string.voice_data_failed_message)
                        .setNegativeButton(android.R.string.ok, mFinishClickListener)
                        .setOnCancelListener(mFinishCancelListener).create();
            case DIALOG_ERROR:
                return new AlertDialog.Builder(this).setTitle(R.string.app_name)
                        .setMessage(R.string.error_message)
                        .setNegativeButton(android.R.string.no, mFinishClickListener)
                        .setNegativeButton(android.R.string.ok, mReportClickListener)
                        .setOnCancelListener(mFinishCancelListener).create();
        }

        return super.onCreateDialog(id);
    }

    private final DialogInterface.OnClickListener mDialogClickListener = new DialogInterface.OnClickListener() {
        @Override
        public void onClick(DialogInterface dialog, int which) {
            switch (which) {
                case DialogInterface.BUTTON_POSITIVE:
                    startActivityForResult(
                            new Intent(ACTION_TTS_SETTINGS), REQUEST_DEFAULT);
                    break;
            }
        }
    };

    private final DialogInterface.OnClickListener mReportClickListener = new DialogInterface.OnClickListener() {
        @Override
        public void onClick(DialogInterface dialog, int which) {
            // TODO: Send a crash report.
            finish();
        }
    };

    private final DialogInterface.OnClickListener mFinishClickListener = new DialogInterface.OnClickListener() {
        @Override
        public void onClick(DialogInterface dialog, int which) {
            finish();
        }
    };

    private final DialogInterface.OnCancelListener mFinishCancelListener = new DialogInterface.OnCancelListener() {
        @Override
        public void onCancel(DialogInterface dialog) {
            finish();
        }
    };

    private final TextToSpeech.OnInitListener mInitListener = new TextToSpeech.OnInitListener() {
        @Override
        public void onInit(int status) {
            mHandler.obtainMessage(TTS_INITIALIZED, status, 0).sendToTarget();
        }
    };

    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case TTS_INITIALIZED:
                    onInitialized(msg.arg1);
                    break;
            }
        }
    };

    private final View.OnClickListener mOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            switch (v.getId()) {
                case R.id.engineSettings:
                    startActivityForResult(
                            new Intent(eSpeakActivity.this, TtsSettingsActivity.class),
                            REQUEST_DEFAULT);
                    break;
                case R.id.ttsSettings:
                    startActivityForResult(new Intent(ACTION_TTS_SETTINGS), REQUEST_DEFAULT);
                    break;
            }
        }
    };
}
