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
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.TextView;

import com.reecedunn.espeak.R;
import com.reecedunn.espeak.VoiceSettings;

import java.util.ArrayList;
import java.util.List;

/**
 * A dialog preference that edits one or more of eSpeak's numeric voice
 * parameters. One section -- name, value, slider, reset button -- is added to
 * the dialog per parameter.
 *
 * A phone hosts all four parameters in a single dialog. Wear gives each
 * parameter its own preference, and therefore its own dialog, because the
 * rotating crown only ever delivers its scroll events to the focused view and
 * a watch has no D-pad to move focus with. A second slider in the same dialog
 * would be unreachable by the crown, so the split is a requirement of the
 * input model rather than a matter of screen size. See TtsSettingsActivity.
 */
public class SeekBarPreference extends DialogPreference
{
    /**
     * One editable parameter: where its value is stored, how it is presented,
     * and -- while the dialog is open -- the views bound to it.
     */
    public static class Parameter
    {
        private final String key;
        private final String title;
        private final int min;
        private final int max;
        private final int defaultValue;
        private final String formatter;

        /** The value shown by the slider right now. */
        private int current;
        /** The value to fall back to if the dialog is cancelled. */
        private int saved;

        /** Only the speech rate carries the rate-boost checkbox. */
        private boolean hasRateBoost;
        private boolean boost;
        private boolean savedBoost;

        private SeekBar mSeekBar;
        private TextView mValueText;
        private CheckBox mRateBoost;

        /**
         * True between onStartTrackingTouch and onStopTrackingTouch. A drag
         * emits a progress change per pixel, so persisting is deferred to the
         * end of the gesture; see {@link #persist}.
         */
        private boolean dragging;

        public Parameter(String key, String title, int min, int max, int defaultValue, int current, String formatter)
        {
            this.key = key;
            this.title = title;
            this.min = min;
            this.max = max;
            this.defaultValue = defaultValue;
            this.current = current;
            this.saved = current;
            this.formatter = formatter;
        }

        public void enableRateBoost(boolean enabled)
        {
            hasRateBoost = true;
            boost = enabled;
            savedBoost = enabled;
        }
    }

    private final List<Parameter> mParameters = new ArrayList<Parameter>();

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

    public void addParameter(Parameter parameter)
    {
        mParameters.add(parameter);
    }

    @Override
    protected View onCreateDialogView()
    {
        final View root = super.onCreateDialogView();
        final ViewGroup container = (ViewGroup) root.findViewById(R.id.parameters);
        final LayoutInflater inflater = LayoutInflater.from(getContext());

        for (final Parameter parameter : mParameters) {
            final View section = inflater.inflate(R.layout.seekbar_preference_section, container, false);
            bindSection(parameter, section);
            container.addView(section);
        }

        return root;
    }

    private void bindSection(final Parameter parameter, View section)
    {
        parameter.mSeekBar = (SeekBar) section.findViewById(R.id.seekBar);
        parameter.mValueText = (TextView) section.findViewById(R.id.valueText);
        parameter.mRateBoost = (CheckBox) section.findViewById(R.id.rateBoost);

        ((TextView) section.findViewById(R.id.parameterTitle)).setText(parameter.title);

        final Button reset = (Button) section.findViewById(R.id.resetToDefault);
        // Every section carries an identically labelled button, so name the
        // parameter for anyone who reaches it with a screen reader.
        reset.setContentDescription(parameter.title + ", " + reset.getText());
        reset.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v)
            {
                parameter.mSeekBar.setProgress(parameter.defaultValue - parameter.min);
                // setProgress() reports fromUser == false, so persist here
                // rather than leaving it to the progress listener.
                persist(parameter);
            }
        });

        parameter.mSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
            {
                parameter.current = progress + parameter.min;
                updateValueText(parameter);

                // TalkBack gestures and D-pad presses arrive here with
                // fromUser set and no surrounding touch gesture, and they are
                // the only notification we get -- onStopTrackingTouch never
                // fires for them. Persisting now is what makes the change
                // audible on the next thing eSpeak speaks.
                if (fromUser && !parameter.dragging) {
                    persist(parameter);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar)
            {
                parameter.dragging = true;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar)
            {
                parameter.dragging = false;
                persist(parameter);
            }
        });

        if (parameter.hasRateBoost) {
            // Checked before the listener is attached, so restoring the stored
            // state does not count as toggling it and write it straight back.
            parameter.mRateBoost.setChecked(parameter.boost);
            parameter.mRateBoost.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
                {
                    parameter.boost = isChecked;
                    updateValueText(parameter);
                    persist(parameter);
                }
            });
        } else {
            parameter.mRateBoost.setVisibility(View.GONE);
        }

        attachRotaryEncoder(parameter);
    }

    @Override
    protected void onBindDialogView(View view)
    {
        super.onBindDialogView(view);

        for (Parameter parameter : mParameters) {
            // Read before touching the SeekBar: setMax() notifies the progress
            // listener, which writes the slider's position -- still zero at
            // that point -- back over parameter.current.
            final int value = parameter.current;

            parameter.saved = value;
            parameter.savedBoost = parameter.boost;

            // Neither call persists anything, because neither reports itself
            // as a change made by the user. That is what keeps opening the
            // dialog from publishing a half-initialised set of values; an
            // earlier revision announced rate = 80 WPM and volume = 0 here.
            parameter.mSeekBar.setMax(parameter.max - parameter.min);
            parameter.mSeekBar.setProgress(value - parameter.min);
            parameter.current = value;

            updateValueText(parameter);
        }

        // Only meaningful for the one-parameter (Wear) dialog, where it hands
        // the crown to the single slider. With four sliders there is nothing
        // sensible to focus, and taking focus would just misdirect the crown.
        if (mParameters.size() == 1) {
            mParameters.get(0).mSeekBar.requestFocus();
        }
    }

    @Override
    public void onClick(DialogInterface dialog, int which)
    {
        switch (which) {
            case DialogInterface.BUTTON_POSITIVE:
                // Update the last saved values so these will be persisted when
                // the dialog is dismissed.

                for (Parameter parameter : mParameters) {
                    parameter.saved = parameter.current;
                    parameter.savedBoost = parameter.boost;
                }
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
        // For [1], the new values need to be persisted. For [2] and [3], the
        // old values need to be persisted (so the last saved values are
        // restored). As there is no easy way to override the Dialog's back
        // button pressed handler, the following approach is used:
        //
        // 1.  If the user presses the OK button, the last saved values are
        //     updated to be the new values (see the onClick handler).
        //
        // 2.  In all cases, the last saved values are persisted when the dialog
        //     is closed (in this onDismiss handler).

        for (Parameter parameter : mParameters) {
            parameter.current = parameter.saved;
            parameter.boost = parameter.savedBoost;
            persist(parameter);
        }

        String summary = buildSummary();
        callChangeListener(summary);
        setSummary(summary);
    }

    /**
     * The value to show for a parameter, which is not the stored value when
     * the rate boost multiplies it.
     */
    private int getDisplayValue(Parameter parameter)
    {
        int value = parameter.current;
        if (parameter.hasRateBoost && parameter.boost) {
            value = value * VoiceSettings.RATE_BOOST_MULTIPLIER;
            int boostedMax = parameter.max * VoiceSettings.RATE_BOOST_MULTIPLIER;
            if (value > boostedMax) {
                value = boostedMax;
            }
        }
        return value;
    }

    private void updateValueText(Parameter parameter)
    {
        String text = String.format(parameter.formatter, Integer.toString(getDisplayValue(parameter)));
        parameter.mValueText.setText(text);
        // A bare "50%" is ambiguous once pitch and pitch variation share a
        // dialog, so the announcement carries the parameter name too. This
        // replaces the percentage of the slider's range that a SeekBar would
        // otherwise announce, which means nothing for these values.
        parameter.mSeekBar.setContentDescription(parameter.title + ", " + text);
    }

    /**
     * Writes one parameter through to the settings eSpeak actually reads, so a
     * change is audible immediately rather than at the next dialog dismissal.
     *
     * apply() rather than commit(): the in-memory value is updated before this
     * returns, which is all TtsService needs since it reads the same
     * SharedPreferences instance in this process, and the disk write stays off
     * the main thread. Rotary detents and TalkBack steps arrive one after
     * another, so a synchronous write here would be felt.
     */
    private void persist(Parameter parameter)
    {
        if (!shouldCommit()) {
            return;
        }

        SharedPreferences.Editor editor = getDeviceProtectedPreferences().edit();
        editor.putString(parameter.key, Integer.toString(parameter.current));
        if (parameter.hasRateBoost) {
            editor.putBoolean(VoiceSettings.PREF_RATE_BOOST, parameter.boost);
        }
        editor.apply();
    }

    /**
     * Wire the Wear OS rotating crown to a slider. Rotary input is delivered
     * as ACTION_SCROLL events on SOURCE_ROTARY_ENCODER and only reaches the
     * focused view, so the slider has to be focusable and, in the
     * one-parameter dialog, take focus when it opens. The listener is a no-op
     * on phones (no rotary device ever fires it), so this code path stays
     * harmless on non-watch builds.
     */
    private void attachRotaryEncoder(final Parameter parameter)
    {
        final SeekBar seekBar = parameter.mSeekBar;
        final int range = parameter.max - parameter.min;
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
                    // As with the reset button: setProgress() is not a user
                    // change as far as the listener is concerned, but this is.
                    persist(parameter);
                    v.performHapticFeedback(HapticFeedbackConstants.VIRTUAL_KEY);
                }
                return true;
            }
        });
        seekBar.setFocusable(true);
        seekBar.setFocusableInTouchMode(true);
    }

    /**
     * The preference row's summary: the value on its own when the preference
     * owns a single parameter, otherwise each parameter named and listed.
     */
    public String buildSummary()
    {
        StringBuilder summary = new StringBuilder();
        for (Parameter parameter : mParameters) {
            if (summary.length() > 0) {
                summary.append(", ");
            }
            if (mParameters.size() > 1) {
                summary.append(parameter.title).append(": ");
            }
            summary.append(String.format(parameter.formatter, Integer.toString(getDisplayValue(parameter))));
        }
        return summary.toString();
    }

    private SharedPreferences getDeviceProtectedPreferences()
    {
        Context context = getContext();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            context = context.createDeviceProtectedStorageContext();
        }
        return PreferenceManager.getDefaultSharedPreferences(context);
    }
}
