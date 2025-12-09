/*
 * Copyright (C) 2025
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

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.preference.MultiSelectListPreference;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;

import com.reecedunn.espeak.EspeakApp;
import com.reecedunn.espeak.LanguageSettings;
import com.reecedunn.espeak.R;

import java.util.HashSet;
import java.util.Set;

/**
 * Multi-select preference with a custom dialog that includes explicit
 * Select All / Deselect All buttons alongside OK/Cancel.
 */
public class SupportedLanguagesPreference extends MultiSelectListPreference {
    private CharSequence[] mDialogEntryValues;
    private final Set<String> mNewValues = new HashSet<String>();

    private View mDialogView;
    private ListView mListView;
    private Button mButtonSelectAll;
    private Button mButtonDeselectAll;
    private int mEntryCount = 0;

    public SupportedLanguagesPreference(Context context, AttributeSet attrs) {
        super(context, attrs);
        setKey(LanguageSettings.PREF_SUPPORTED_LANGUAGES);
        setPersistent(true);
    }

    public SupportedLanguagesPreference(Context context) {
        this(context, null);
    }

    @Override
    protected void onPrepareDialogBuilder(AlertDialog.Builder builder) {
        builder.setPositiveButton(android.R.string.ok, null);
        builder.setNegativeButton(android.R.string.cancel, null);

        if (getEntries() == null || getEntryValues() == null) {
            throw new IllegalStateException("SupportedLanguagesPreference requires entries and entryValues.");
        }

        mDialogEntryValues = getEntryValues();
        mEntryCount = mDialogEntryValues.length;

        final Set<String> values = getValues();

        LayoutInflater inflater = LayoutInflater.from(getContext());
        mDialogView = inflater.inflate(R.layout.supported_languages_dialog, null);

        mListView = mDialogView.findViewById(R.id.supported_languages_list);
        mListView.setChoiceMode(ListView.CHOICE_MODE_MULTIPLE);
        ArrayAdapter<CharSequence> adapter = new ArrayAdapter<CharSequence>(
                getContext(),
                android.R.layout.simple_list_item_multiple_choice,
                getEntries());
        mListView.setAdapter(adapter);

        for (int i = 0; i < mDialogEntryValues.length; i++) {
            mListView.setItemChecked(i, values.contains(mDialogEntryValues[i].toString()));
        }

        mButtonSelectAll = mDialogView.findViewById(R.id.button_select_all);
        mButtonDeselectAll = mDialogView.findViewById(R.id.button_deselect_all);

        mButtonSelectAll.setOnClickListener(v -> setAll(true));
        mButtonDeselectAll.setOnClickListener(v -> setAll(false));

        builder.setView(mDialogView);
        builder.setTitle(getDialogTitle());
        builder.setIcon(getDialogIcon());
        builder.setCancelable(true);
    }

    private void setAll(boolean checked) {
        for (int i = 0; i < mDialogEntryValues.length; i++) {
            mListView.setItemChecked(i, checked);
        }
    }

    private Set<String> collectSelections() {
        Set<String> selections = new HashSet<String>();
        if (mListView == null || mDialogEntryValues == null) {
            return selections;
        }
        for (int i = 0; i < mDialogEntryValues.length; i++) {
            if (mListView.isItemChecked(i)) {
                selections.add(mDialogEntryValues[i].toString());
            }
        }
        return selections;
    }

    @Override
    protected void showDialog(Bundle state) {
        super.showDialog(state);

        final AlertDialog dialog = (AlertDialog) getDialog();
        if (dialog == null) return;

        // Wire default dialog buttons.
        Button positive = dialog.getButton(AlertDialog.BUTTON_POSITIVE);
        Button negative = dialog.getButton(AlertDialog.BUTTON_NEGATIVE);

        if (positive != null) {
            positive.setOnClickListener(v -> {
                Set<String> selections = collectSelections();
                if (selections.isEmpty()) {
                    Toast.makeText(getContext(), R.string.espeak_supported_languages_guard, Toast.LENGTH_SHORT).show();
                    return;
                }
                if (callChangeListener(selections)) {
                    setValues(selections);
                }
                dialog.dismiss();
            });
        }

        if (negative != null) {
            negative.setOnClickListener(v -> dialog.cancel());
        }
    }

    // Persist using device-protected storage to stay in sync with TtsService.
    @Override
    public boolean persistStringSet(Set<String> values) {
        if (!shouldPersist()) return false;
        int total = mEntryCount > 0 ? mEntryCount :
                (getEntryValues() != null ? getEntryValues().length : 0);

        android.content.SharedPreferences.Editor editor = android.preference.PreferenceManager
                .getDefaultSharedPreferences(EspeakApp.getStorageContext())
                .edit();

        if (values == null || (total > 0 && values.size() >= total)) {
            // Treat as "all": remove the preference key so TtsService exposes everything.
            editor.remove(getKey());
            editor.apply();
            return true;
        }

        Set<String> copy = new HashSet<String>(values);
        editor.putStringSet(getKey(), copy);
        editor.apply();
        return true;
    }

    @Override
    public Set<String> getPersistedStringSet(Set<String> defaultReturnValue) {
        if (!shouldPersist()) return defaultReturnValue;
        Set<String> stored = android.preference.PreferenceManager
                .getDefaultSharedPreferences(EspeakApp.getStorageContext())
                .getStringSet(getKey(), defaultReturnValue);
        return (stored == null) ? defaultReturnValue : new HashSet<String>(stored);
    }
}
