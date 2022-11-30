/*
 * Copyright (C) 2022 Beka Gozalishvili
 * Copyright (C) 2013 Reece H. Dunn
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

package com.reecedunn.espeak.preference;

import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Build;
import android.preference.DialogPreference;
import android.preference.PreferenceManager;
import android.text.Editable;
import android.util.AttributeSet;
import android.view.View;
import android.widget.EditText;
import android.widget.RadioButton;

import com.reecedunn.espeak.R;
import com.reecedunn.espeak.SpeechSynthesis;
import com.reecedunn.espeak.VoiceSettings;

public class SpeakPunctuationPreference extends DialogPreference {
    private RadioButton mAll;
    private RadioButton mCustom;
    private RadioButton mNone;
    private EditText mPunctuationCharacters;

    private VoiceSettings mSettings;

    public SpeakPunctuationPreference(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        setDialogLayoutResource(R.layout.speak_punctuation_preference);
        setLayoutResource(R.layout.information_view);
        setPositiveButtonText(android.R.string.ok);
        setNegativeButtonText(android.R.string.cancel);
    }

    public SpeakPunctuationPreference(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public SpeakPunctuationPreference(Context context) {
        this(context, null);
    }

    public void setVoiceSettings(VoiceSettings settings) {
        mSettings = settings;
        onDataChanged(mSettings.getPunctuationLevel(), mSettings.getPunctuationCharacters());
    }

    private void onDataChanged(int level, String characters) {
        switch (level) {
            case SpeechSynthesis.PUNCT_ALL:
                callChangeListener(getContext().getText(R.string.punctuation_all));
                break;
            case SpeechSynthesis.PUNCT_SOME:
                if (characters == null || characters.isEmpty()) {
                    callChangeListener(getContext().getText(R.string.punctuation_none));
                } else {
                    callChangeListener(String.format(getContext().getText(R.string.punctuation_custom_fmt).toString(), characters));
                }
                break;
            case SpeechSynthesis.PUNCT_NONE:
                callChangeListener(getContext().getText(R.string.punctuation_none));
                break;
        }
    }

    @Override
    protected View onCreateDialogView() {
        View root = super.onCreateDialogView();
        mAll = (RadioButton)root.findViewById(R.id.all);
        mCustom = (RadioButton)root.findViewById(R.id.custom);
        mNone = (RadioButton)root.findViewById(R.id.none);
        mPunctuationCharacters = (EditText)root.findViewById(R.id.punctuation_characters);
        return root;
    }

    @Override
    protected void onBindDialogView(View view) {
        super.onBindDialogView(view);

        switch (mSettings.getPunctuationLevel()) {
            case SpeechSynthesis.PUNCT_ALL:
                mAll.toggle();
                break;
            case SpeechSynthesis.PUNCT_SOME:
                mCustom.toggle();
                break;
            case SpeechSynthesis.PUNCT_NONE:
                mNone.toggle();
                break;
        }

        mPunctuationCharacters.setText(mSettings.getPunctuationCharacters());
    }

    @Override
    public void onClick(DialogInterface dialog, int which) {
        switch (which) {
            case DialogInterface.BUTTON_POSITIVE:
                Editable text = mPunctuationCharacters.getText();
                String characters = null;
                int level;
                if (text != null) {
                    characters = text.toString();
                }

                if (mNone.isChecked()) {
                    level = SpeechSynthesis.PUNCT_NONE;
                } else if (characters == null || characters.isEmpty()) {
                    level = mAll.isChecked() ? SpeechSynthesis.PUNCT_ALL : SpeechSynthesis.PUNCT_NONE;
                } else {
                    level = mAll.isChecked() ? SpeechSynthesis.PUNCT_ALL : SpeechSynthesis.PUNCT_SOME;
                }

                onDataChanged(level, characters);

                if (shouldCommit()) {
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N)
                    {
                        PreferenceManager preferenceManager = getPreferenceManager();
                        preferenceManager.setStorageDeviceProtected ();
                    }
                    SharedPreferences.Editor editor = getEditor();
                    if (editor != null) {
                        editor.putString(VoiceSettings.PREF_PUNCTUATION_CHARACTERS, characters);
                        editor.putString(VoiceSettings.PREF_PUNCTUATION_LEVEL, Integer.toString(level));
                        editor.commit();
                    }
                }
                break;
        }
        super.onClick(dialog, which);
    }
}
