/*
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
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Environment;
import android.preference.DialogPreference;
import android.util.AttributeSet;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.AppCompatSpinner;

import com.reecedunn.espeak.CheckVoiceData;
import com.reecedunn.espeak.DownloadVoiceData;
import com.reecedunn.espeak.FileListAdapter;
import com.reecedunn.espeak.FileUtils;
import com.reecedunn.espeak.R;

import java.io.File;
import java.io.FileFilter;
import java.io.IOException;
import java.util.Arrays;

public class ImportVoicePreference extends DialogPreference {
    private final File mRoot;
    private AppCompatSpinner mDictionaries;

    public ImportVoicePreference(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        setDialogLayoutResource(R.layout.import_voice_preference);
        setLayoutResource(R.layout.information_view);
        setPositiveButtonText(android.R.string.ok);
        setNegativeButtonText(android.R.string.cancel);

        mRoot = Environment.getExternalStorageDirectory();
    }

    public ImportVoicePreference(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public ImportVoicePreference(Context context) {
        this(context, null);
    }

    public void setDescription(int resId) {
        callChangeListener(getContext().getString(resId));
    }

    @Override
    protected View onCreateDialogView() {
        View root = super.onCreateDialogView();
        mDictionaries = root.findViewById(R.id.dictionaries);
        return root;
    }

    @Override
    protected void onBindDialogView(View view) {
        super.onBindDialogView(view);
        File[] dictionaries = mRoot.listFiles(new FileFilter() {
            @Override
            public boolean accept(File file) {
                return !file.isDirectory() && file.getName().endsWith("_dict");
            }
        });
        if (dictionaries != null) {
            Arrays.sort(dictionaries);
            mDictionaries.setAdapter(new FileListAdapter((AppCompatActivity) getContext(), dictionaries));
        }
    }

    @Override
    public void onClick(DialogInterface dialog, int which) {
        if (which == DialogInterface.BUTTON_POSITIVE) {
            new AsyncTask<Object, Object, File>() {
                @Override
                protected File doInBackground(Object... objects) {
                    File source = (File) mDictionaries.getSelectedItem();
                    if (source != null) {
                        File destination = new File(CheckVoiceData.getDataPath(getContext()), source.getName());
                        try {
                            byte[] data = FileUtils.readBinary(source);
                            FileUtils.write(destination, data);
                            return source;
                        } catch (IOException e) {
                        }
                    }
                    return null;
                }

                @Override
                protected void onPostExecute(File file) {
                    if (file != null) {
                        final Intent intent = new Intent(DownloadVoiceData.BROADCAST_LANGUAGES_UPDATED);
                        getContext().sendBroadcast(intent);
                    }
                }
            }.execute();
        }
        super.onClick(dialog, which);
    }
}
