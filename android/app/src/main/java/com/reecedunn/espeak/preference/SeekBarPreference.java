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
import android.util.AttributeSet;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

import com.reecedunn.espeak.R;

public class SeekBarPreference extends DialogPreference implements SeekBar.OnSeekBarChangeListener
{
    private SeekBar mSeekBar;
    private TextView mValueText;

    private int mOldProgress = 0;
    private int mProgress = 0;
    private int mDefaultValue = 0;
    private int mMin = 0;
    private int mMax = 100;
    private String mFormatter = "%s";

    public void setProgress(int progress) {
        mProgress = progress;
        String text = Integer.toString(mProgress);
        callChangeListener(text);

        // Update the last saved value to the so it can be restored later if
        // the user cancels the dialog. This needs to be done here as well
        // as the onProgressChanged handler as the SeekBar will not be
        // initialized at this point.

        mOldProgress = mProgress;
    }

    public int getProgress() {
        return mProgress;
    }

    public void setDefaultValue(int defaultValue) {
        mDefaultValue = defaultValue;
    }

    public int getDefaultValue() {
        return mDefaultValue;
    }

    public void setMin(int min) {
        mMin =  min;
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

    public void setFormatter(String formatter) {
        mFormatter = formatter;
    }

    public String getFormatter() {
        return mFormatter;
    }

    public SeekBarPreference(Context context, AttributeSet attrs, int defStyle)
    {
        super(context, attrs, defStyle);
        setDialogLayoutResource(R.layout.seekbar_preference);
        setLayoutResource(R.layout.information_view);
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

    private void persistSettings(int progress) {
        mProgress = progress;
        String text = Integer.toString(mProgress);
        callChangeListener(text);
        if (shouldCommit()) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N)
            {
                PreferenceManager preferenceManager = getPreferenceManager();
                preferenceManager.setStorageDeviceProtected ();
            }
            SharedPreferences.Editor editor = getEditor();
            editor.putString(getKey(), text);
            editor.commit();
        }
    }

    @Override
    protected View onCreateDialogView() {
        View root = super.onCreateDialogView();
        mSeekBar = (SeekBar)root.findViewById(R.id.seekBar);
        mValueText = (TextView)root.findViewById(R.id.valueText);

        Button reset = (Button)root.findViewById(R.id.resetToDefault);
        reset.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v)
            {
                int defaultValue = getDefaultValue();
                mSeekBar.setProgress(defaultValue - mMin);

                // Persist the value here to ensure that eSpeak is using the
                // new value the next time e.g. TalkBack reads part of the UI.

                persistSettings(defaultValue);
            }
        });
        return root;
    }

    @Override
    protected void onBindDialogView(View view) {
        super.onBindDialogView(view);
        mSeekBar.setOnSeekBarChangeListener(this);
        mSeekBar.setMax(mMax - mMin);
        mSeekBar.setProgress(mProgress - mMin);

        // Update the last saved value to the so it can be restored later if
        // the user cancels the dialog.

        mOldProgress = mProgress;
    }

    @Override
    public void onClick(DialogInterface dialog, int which) {
        switch (which) {
            case DialogInterface.BUTTON_POSITIVE:
                // Update the last saved value so this will be persisted when
                // the dialog is dismissed.

                mOldProgress = mSeekBar.getProgress() + mMin;
                break;
        }
        super.onClick(dialog, which);
    }

    @Override
    public void onDismiss(DialogInterface dialog) {
        // There are 3 ways to dismiss a dialog:
        //   1.  Pressing the OK (positive) button.
        //   2.  Pressing the Cancel (negative) button.
        //   3.  Pressing the Back button.
        //
        // For [1], the new value needs to be persisted. For [2] and [3], the
        // old value needs to be persisted (so the last saved value is
        // restored). As there is no easy way to override the Dialog's back
        // button pressed handler, the following approach is used:
        //
        // 1.  If the user presses the OK button, the last saved value is
        //     updated to be the new value (see the onClick handler).
        //
        // 2.  In all cases, the last saved value is persisted when the dialog
        //     is closed (in this onDismiss handler).

        persistSettings(mOldProgress);
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
    {
        // This callback gets called frequently when the user is moving the
        // slider, so constantly persisting the seeker value will be annoying.
        //
        // If the value is being set programatically, persisting the seeker
        // value here will cause the speech rate to be set to 80 WPM (via the
        // onBindDialogView handler).

        String text = String.format(getFormatter(), Integer.toString(progress + mMin));
        mValueText.setText(text);
        mSeekBar.setContentDescription(text);
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar)
    {
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar)
    {
        // After the user has let go of the slider, the new value is
        // persisted to ensure that eSpeak is using the new value the
        // next time e.g. TalkBack reads part of the UI.

        persistSettings(mSeekBar.getProgress() + mMin);
    }
}
