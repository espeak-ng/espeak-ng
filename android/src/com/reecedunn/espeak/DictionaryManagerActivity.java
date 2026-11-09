/*
 * Copyright (C) 2026 eSpeak NG contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 */

package com.reecedunn.espeak;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.Stack;

/** Adds lightweight, language-scoped text replacements without compiling dictionaries. */
public class DictionaryManagerActivity extends Activity {
    private static final class LanguageOption {
        final String code;
        final String label;

        LanguageOption(String code, String name) {
            this.code = code;
            this.label = code + " — " + name;
        }

        @Override
        public String toString() {
            return label;
        }
    }

    private EditText mTarget;
    private EditText mReplacement;
    private Spinner mLanguage;
    private TextView mStatus;
    private Context mStorageContext;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setTitle(R.string.dictionary_manager_title);
        setContentView(R.layout.dictionary_manager);

        mStorageContext = EspeakApp.getStorageContext();
        mTarget = findViewById(R.id.lexicon_target);
        mReplacement = findViewById(R.id.lexicon_replacement);
        mLanguage = findViewById(R.id.lexicon_language);
        mStatus = findViewById(R.id.lexicon_status);

        List<LanguageOption> languages = loadLanguages();
        ArrayAdapter<LanguageOption> adapter = new ArrayAdapter<LanguageOption>(
                this, android.R.layout.simple_spinner_item, languages);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mLanguage.setAdapter(adapter);

        findViewById(R.id.lexicon_save).setOnClickListener(view -> saveMapping());
        updateCount();
    }

    private void saveMapping() {
        Object selected = mLanguage.getSelectedItem();
        if (!(selected instanceof LanguageOption)) {
            showStatus(getString(R.string.lexicon_no_languages));
            return;
        }
        LanguageOption language = (LanguageOption) selected;
        try {
            CustomLexicon.saveMapping(mStorageContext, language.code,
                    mTarget.getText().toString(), mReplacement.getText().toString());
            mTarget.setText("");
            mReplacement.setText("");
            int count = CustomLexicon.entryCount(mStorageContext);
            showStatus(getString(R.string.lexicon_saved, count));
            mTarget.requestFocus();
        } catch (IOException e) {
            showStatus(getString(R.string.lexicon_save_failed, e.getMessage()));
        }
    }

    private void updateCount() {
        try {
            int count = CustomLexicon.entryCount(mStorageContext);
            mStatus.setText(getString(R.string.lexicon_entry_count, count));
        } catch (IOException e) {
            mStatus.setText(R.string.lexicon_help);
        }
    }

    private void showStatus(String message) {
        mStatus.setText(message);
        Toast.makeText(this, message, Toast.LENGTH_LONG).show();
    }

    private List<LanguageOption> loadLanguages() {
        List<LanguageOption> result = new ArrayList<LanguageOption>();
        Set<String> seen = new HashSet<String>();
        File root = new File(CheckVoiceData.getDataPath(mStorageContext), "lang");
        Stack<File> pending = new Stack<File>();
        if (root.isDirectory()) pending.push(root);
        while (!pending.empty()) {
            File[] files = pending.pop().listFiles();
            if (files == null) continue;
            for (File file : files) {
                if (file.isDirectory()) {
                    pending.push(file);
                } else {
                    LanguageOption option = parseLanguage(file);
                    if (option != null && seen.add(option.code)) result.add(option);
                }
            }
        }
        if (result.isEmpty()) {
            result.add(new LanguageOption("ar", "Arabic"));
            result.add(new LanguageOption("en", "English"));
        }
        Collections.sort(result, new Comparator<LanguageOption>() {
            @Override
            public int compare(LanguageOption left, LanguageOption right) {
                return left.label.compareToIgnoreCase(right.label);
            }
        });
        return result;
    }

    private static LanguageOption parseLanguage(File file) {
        String code = null;
        String name = null;
        try (BufferedReader reader = new BufferedReader(new InputStreamReader(
                new FileInputStream(file), "UTF-8"))) {
            String line;
            while ((line = reader.readLine()) != null) {
                line = line.trim();
                if (line.startsWith("language ")) {
                    String value = line.substring("language ".length()).trim();
                    int separator = value.indexOf(' ');
                    code = separator < 0 ? value : value.substring(0, separator);
                } else if (line.startsWith("name ")) {
                    name = line.substring("name ".length()).trim();
                }
                if (code != null && name != null) break;
            }
        } catch (IOException ignored) {
            return null;
        }
        if (code == null || code.isEmpty()) return null;
        return new LanguageOption(code, name == null || name.isEmpty() ? file.getName() : name);
    }
}
