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
import android.view.HapticFeedbackConstants;
import android.view.InputDevice;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.TextView;

import com.reecedunn.espeak.R;
import com.reecedunn.espeak.VoiceSettings;

public class SeekBarPreference extends DialogPreference
{
    // -------- UI --------
    private SeekBar mRate;
    private SeekBar mPitch;
    private SeekBar mRange;
    private SeekBar mVol;

    private TextView mRateText;
    private TextView mPitchText;
    private TextView mRangeText;
    private TextView mVolText;

    private CheckBox mRateBoost;

    private Button mResetRate;
    private Button mResetPitch;
    private Button mResetRange;
    private Button mResetVol;

    // -------- Rate --------
    private String mRateTitle;
    private int mRateMin = 0;
    private int mRateMax = 100;
    private int mRateDefault = 50;
    private int mRateCurrent = 50;
    private int mOldRate = 50;
    private boolean mRateBoostCurrent = false;
    private boolean mOldRateBoost = false;
    private String mRateFormatter = "%s";

    // -------- Pitch --------
    private String mPitchTitle;
    private int mPitchMin = 0;
    private int mPitchMax = 100;
    private int mPitchDefault = 50;
    private int mPitchCurrent = 50;
    private int mOldPitch = 50;
    private String mPitchFormatter = "%s";

    // -------- Range --------
    private String mRangeTitle;
    private int mRangeMin = 0;
    private int mRangeMax = 100;
    private int mRangeDefault = 50;
    private int mRangeCurrent = 50;
    private int mOldRange = 50;
    private String mRangeFormatter = "%s";

    // -------- Volume --------
    private String mVolTitle;
    private int mVolMin = 0;
    private int mVolMax = 100;
    private int mVolDefault = 50;
    private int mVolCurrent = 50;
    private int mOldVol = 50;
    private String mVolFormatter = "%s";

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

    // -------- Config setters --------

    public void setRateConfig(String title, int min, int max, int defaultValue, int current, boolean boostCurrent, String formatter)
    {
        mRateTitle = title;
        mRateMin = min;
        mRateMax = max;
        mRateDefault = defaultValue;
        mRateCurrent = current;
        mOldRate = current;
        mRateBoostCurrent = boostCurrent;
        mOldRateBoost = boostCurrent;
        mRateFormatter = formatter;
    }

    public void setPitchConfig(String title, int min, int max, int defaultValue, int current, String formatter)
    {
        mPitchTitle = title;
        mPitchMin = min;
        mPitchMax = max;
        mPitchDefault = defaultValue;
        mPitchCurrent = current;
        mOldPitch = current;
        mPitchFormatter = formatter;
    }

    public void setRangeConfig(String title, int min, int max, int defaultValue, int current, String formatter)
    {
        mRangeTitle = title;
        mRangeMin = min;
        mRangeMax = max;
        mRangeDefault = defaultValue;
        mRangeCurrent = current;
        mOldRange = current;
        mRangeFormatter = formatter;
    }

    public void setVolumeConfig(String title, int min, int max, int defaultValue, int current, String formatter)
    {
        mVolTitle = title;
        mVolMin = min;
        mVolMax = max;
        mVolDefault = defaultValue;
        mVolCurrent = current;
        mOldVol = current;
        mVolFormatter = formatter;
    }

    @Override
    protected View onCreateDialogView()
    {
        View root = super.onCreateDialogView();

        mRate = (SeekBar) root.findViewById(R.id.rate);
        mPitch = (SeekBar) root.findViewById(R.id.pitch);
        mRange = (SeekBar) root.findViewById(R.id.range);
        mVol = (SeekBar) root.findViewById(R.id.volume);

        mRateText = (TextView) root.findViewById(R.id.rateText);
        mPitchText = (TextView) root.findViewById(R.id.pitchText);
        mRangeText = (TextView) root.findViewById(R.id.rangeText);
        mVolText = (TextView) root.findViewById(R.id.volumeText);

        mRateBoost = (CheckBox) root.findViewById(R.id.rateBoost);

        mResetRate = (Button) root.findViewById(R.id.resetRateToDefault);
        mResetPitch = (Button) root.findViewById(R.id.resetPitchToDefault);
        mResetRange = (Button) root.findViewById(R.id.resetRangeToDefault);
        mResetVol = (Button) root.findViewById(R.id.resetVolumeToDefault);

        mRate.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
            {
                // After the user has changed the slider, the new value is
                // persisted to ensure that eSpeak is using the new value the
                // next time e.g. TalkBack reads part of the UI.

                updateRateText();
                persistSettings(mRate.getProgress() + mRateMin, mPitch.getProgress() + mPitchMin, mRange.getProgress() + mRangeMin, mVol.getProgress() + mVolMin, mRateBoost.isChecked());
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar)
            {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar)
            {
            }
        });

        mPitch.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
            {
                // After the user has changed the slider, the new value is
                // persisted to ensure that eSpeak is using the new value the
                // next time e.g. TalkBack reads part of the UI.

                updatePitchText();
                persistSettings(mRate.getProgress() + mRateMin, mPitch.getProgress() + mPitchMin, mRange.getProgress() + mRangeMin, mVol.getProgress() + mVolMin, mRateBoost.isChecked());
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar)
            {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar)
            {
            }
        });

        mRange.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
            {
                // After the user has changed the slider, the new value is
                // persisted to ensure that eSpeak is using the new value the
                // next time e.g. TalkBack reads part of the UI.

                updateRangeText();
                persistSettings(mRate.getProgress() + mRateMin, mPitch.getProgress() + mPitchMin, mRange.getProgress() + mRangeMin, mVol.getProgress() + mVolMin, mRateBoost.isChecked());
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar)
            {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar)
            {
            }
        });

        mVol.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
            {
                // After the user has changed the slider, the new value is
                // persisted to ensure that eSpeak is using the new value the
                // next time e.g. TalkBack reads part of the UI.

                updateVolText();
                persistSettings(mRate.getProgress() + mRateMin, mPitch.getProgress() + mPitchMin, mRange.getProgress() + mRangeMin, mVol.getProgress() + mVolMin, mRateBoost.isChecked());
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar)
            {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar)
            {
            }
        });

        mRateBoost.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
            {
                updateRateText();
                persistSettings(mRate.getProgress() + mRateMin, mPitch.getProgress() + mPitchMin, mRange.getProgress() + mRangeMin, mVol.getProgress() + mVolMin, isChecked);
            }
        });

        mResetRate.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v)
            {
                mRate.setProgress(mRateDefault - mRateMin);
                mRateBoost.setChecked(false);
                updateRateText();
                persistSettings(mRateDefault, mPitch.getProgress() + mPitchMin, mRange.getProgress() + mRangeMin, mVol.getProgress() + mVolMin, false);
            }
        });

        mResetPitch.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v)
            {
                mPitch.setProgress(mPitchDefault - mPitchMin);
                updatePitchText();
                persistSettings(mRate.getProgress() + mRateMin, mPitchDefault, mRange.getProgress() + mRangeMin, mVol.getProgress() + mVolMin, mRateBoost.isChecked());
            }
        });

        mResetRange.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v)
            {
                mRange.setProgress(mRangeDefault - mRangeMin);
                updateRangeText();
                persistSettings(mRate.getProgress() + mRateMin, mPitch.getProgress() + mPitchMin, mRangeDefault, mVol.getProgress() + mVolMin, mRateBoost.isChecked());
            }
        });

        mResetVol.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v)
            {
                mVol.setProgress(mVolDefault - mVolMin);
                updateVolText();
                persistSettings(mRate.getProgress() + mRateMin, mPitch.getProgress() + mPitchMin, mRange.getProgress() + mRangeMin, mVolDefault, mRateBoost.isChecked());
            }
        });

        return root;
    }

    @Override
    protected void onBindDialogView(View view)
    {
        super.onBindDialogView(view);

        mRate.setMax(mRateMax - mRateMin);
        mPitch.setMax(mPitchMax - mPitchMin);
        mRange.setMax(mRangeMax - mRangeMin);
        mVol.setMax(mVolMax - mVolMin);

        mRate.setProgress(mRateCurrent - mRateMin);
        mPitch.setProgress(mPitchCurrent - mPitchMin);
        mRange.setProgress(mRangeCurrent - mRangeMin);
        mVol.setProgress(mVolCurrent - mVolMin);

        mRateBoost.setChecked(mRateBoostCurrent);

        mOldRate = mRateCurrent;
        mOldRateBoost = mRateBoostCurrent;
        mOldPitch = mPitchCurrent;
        mOldRange = mRangeCurrent;
        mOldVol = mVolCurrent;

        attachRotaryEncoder(mRate, mRateMin, mRateMax);
        attachRotaryEncoder(mPitch, mPitchMin, mPitchMax);
        attachRotaryEncoder(mRange, mRangeMin, mRangeMax);
        attachRotaryEncoder(mVol, mVolMin, mVolMax);

        mRate.requestFocus();

        updateRateText();
        updatePitchText();
        updateRangeText();
        updateVolText();
    }

    @Override
    public void onClick(DialogInterface dialog, int which)
    {
        switch (which) {
            case DialogInterface.BUTTON_POSITIVE:
                // Update the last saved values so this will be persisted when
                // the dialog is dismissed.

                mOldRate = mRate.getProgress() + mRateMin;
                mOldRateBoost = mRateBoost.isChecked();
                mOldPitch = mPitch.getProgress() + mPitchMin;
                mOldRange = mRange.getProgress() + mRangeMin;
                mOldVol = mVol.getProgress() + mVolMin;
                break;
        }
        super.onClick(dialog, which);
    }

    @Override
    public void onDismiss(DialogInterface dialog)
    {
        // There are 3 ways to dismiss a dialog:
        //   1.  Pressing the OK (positive) button.
        //   2.  Pressing the Cancel (negative) button.
        //   3.  Pressing the Back button.
        //
        // For [1], the new values needs to be persisted. For [2] and [3], the
        // old values needs to be persisted (so the last saved values are
        // restored). As there is no easy way to override the Dialog's back
        // button pressed handler, the following approach is used:
        //
        // 1.  If the user presses the OK button, the last saved values are
        //     updated to be the new values (see the onClick handler).
        //
        // 2.  In all cases, the last saved values are persisted when the dialog
        //     is closed (in this onDismiss handler).

        persistSettings(mOldRate, mOldPitch, mOldRange, mOldVol, mOldRateBoost);

        mRateCurrent = mOldRate;
        mPitchCurrent = mOldPitch;
        mRangeCurrent = mOldRange;
        mVolCurrent = mOldVol;
        mRateBoostCurrent = mOldRateBoost;

        String summary = buildSummary();
        callChangeListener(summary);
        setSummary(summary);
    }

    // -------- Per-bar text updates --------

    private void updateRateText()
    {
        int display = mRate.getProgress() + mRateMin;
        if (mRateBoost.isChecked()) {
            display = display * VoiceSettings.RATE_BOOST_MULTIPLIER;
            int boostedMax = mRateMax * VoiceSettings.RATE_BOOST_MULTIPLIER;
            if (display > boostedMax) {
                display = boostedMax;
            }
        }
        String text = String.format(mRateFormatter, Integer.toString(display));
        mRateText.setText(text);
        mRate.setContentDescription(text);
    }

    private void updatePitchText()
    {
        String text = String.format(mPitchFormatter, Integer.toString(mPitch.getProgress() + mPitchMin));
        mPitchText.setText(text);
        mPitch.setContentDescription(text);
    }

    private void updateRangeText()
    {
        String text = String.format(mRangeFormatter, Integer.toString(mRange.getProgress() + mRangeMin));
        mRangeText.setText(text);
        mRange.setContentDescription(text);
    }

    private void updateVolText()
    {
        String text = String.format(mVolFormatter, Integer.toString(mVol.getProgress() + mVolMin));
        mVolText.setText(text);
        mVol.setContentDescription(text);
    }

    // -------- Persistence --------

    private void persistSettings(int rate, int pitch, int range, int vol, boolean boost)
    {
        if (shouldCommit()) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                PreferenceManager preferenceManager = getPreferenceManager();
                preferenceManager.setStorageDeviceProtected();
            }
            SharedPreferences.Editor editor = getDeviceProtectedPreferences().edit();
            editor.putString(VoiceSettings.PREF_RATE, String.valueOf(rate));
            editor.putString(VoiceSettings.PREF_PITCH, String.valueOf(pitch));
            editor.putString(VoiceSettings.PREF_PITCH_RANGE, String.valueOf(range));
            editor.putString(VoiceSettings.PREF_VOLUME, String.valueOf(vol));
            editor.putBoolean(VoiceSettings.PREF_RATE_BOOST, boost);
            editor.commit();
        }
    }

    // -------- Wear OS rotary encoder --------

    private void attachRotaryEncoder(final SeekBar seekBar, final int min, final int max)
    {
        final int range = max - min;
        final int step = Math.max(1, range / 40);
        seekBar.setOnGenericMotionListener(new View.OnGenericMotionListener() {
            @Override
            public boolean onGenericMotion(View v, MotionEvent ev)
            {
                if (ev.getAction() != MotionEvent.ACTION_SCROLL
                        || !ev.isFromSource(InputDevice.SOURCE_ROTARY_ENCODER)) {
                    return false;
                }
                float scroll = ev.getAxisValue(MotionEvent.AXIS_SCROLL);
                if (scroll == 0f) {
                    return false;
                }
                int delta = (scroll > 0f ? -1 : 1) * step;
                int updated = Math.max(0, Math.min(range, seekBar.getProgress() + delta));
                if (updated != seekBar.getProgress()) {
                    seekBar.setProgress(updated);
                    v.performHapticFeedback(HapticFeedbackConstants.VIRTUAL_KEY);
                }
                return true;
            }
        });
        seekBar.setFocusable(true);
        seekBar.setFocusableInTouchMode(true);
    }

    // -------- Summary --------

    public String buildSummary()
    {
        int rateDisplay = mRateCurrent;
        if (mRateBoostCurrent) {
            rateDisplay = rateDisplay * VoiceSettings.RATE_BOOST_MULTIPLIER;
            int boostedMax = mRateMax * VoiceSettings.RATE_BOOST_MULTIPLIER;
            if (rateDisplay > boostedMax) {
                rateDisplay = boostedMax;
            }
        }

        return mRateTitle + ": " + String.format(mRateFormatter, Integer.toString(rateDisplay)) + ", "
             + mPitchTitle + ": " + String.format(mPitchFormatter, Integer.toString(mPitchCurrent)) + ", "
             + mRangeTitle + ": " + String.format(mRangeFormatter, Integer.toString(mRangeCurrent)) + ", "
             + mVolTitle + ": " + String.format(mVolFormatter, Integer.toString(mVolCurrent));
    }

    // -------- Helpers --------

    private SharedPreferences getDeviceProtectedPreferences()
    {
        Context context = getContext();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            context = context.createDeviceProtectedStorageContext();
        }
        return PreferenceManager.getDefaultSharedPreferences(context);
    }
}