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

package com.reecedunn.espeak;

import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.preference.DialogPreference;
import android.util.AttributeSet;
import android.view.View;
import android.widget.SeekBar;

public class SeekBarPreference extends DialogPreference
{
    private SeekBar mSeekBar;

    private int mProgress = 0;
    private int mMin = 0;
    private int mMax = 100;

    public void setProgress(int progress) {
        mProgress = progress + mMin;
        String text = Integer.toString(mProgress);
        callChangeListener(text);
    }

    public int getProgress() {
        return mProgress;
    }

    public void setMin(int min) {
        mProgress -= mMin;
        mMin =  min;
        mProgress += mMin;
        String text = Integer.toString(mProgress);
        callChangeListener(text);
    }

    public int getMin() {
        return mMin;
    }

    public void setMax(int max) {
        mMax =  max;
    }

    public int getMax() {
        return mMax;
    }

    public SeekBarPreference(Context context, AttributeSet attrs, int defStyle)
    {
        super(context, attrs, defStyle);
        setDialogLayoutResource(R.layout.seekbar_preference);
        setPositiveButtonText(android.R.string.ok);
        setNegativeButtonText(android.R.string.cancel);
    }

    public SeekBarPreference(Context context, AttributeSet attrs)
    {
        this(context, attrs, 0);
    }

    public SeekBarPreference(Context context)
    {
        this(context, null);
    }

    @Override
    protected View onCreateDialogView() {
        View root = super.onCreateDialogView();
        mSeekBar = (SeekBar)root.findViewById(R.id.seekBar);
        return root;
    }

    @Override
    protected void onBindDialogView(View view) {
        mSeekBar.setMax(mMax - mMin);
        mSeekBar.setProgress(mProgress + mMin);
    }

    @Override
    public void onClick(DialogInterface dialog, int which) {
        switch (which) {
            case DialogInterface.BUTTON_POSITIVE:
                mProgress = mSeekBar.getProgress() + mMin;
                String text = Integer.toString(mProgress);
                callChangeListener(text);
                if (shouldCommit()) {
                    SharedPreferences.Editor editor = getEditor();
                    editor.putString(getKey(), text);
                    editor.commit();
                }
                break;
        }
        super.onClick(dialog, which);
    }
}
