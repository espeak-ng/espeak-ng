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

import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Build;
import android.preference.DialogPreference;
import android.preference.PreferenceManager;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.TextView;

import com.reecedunn.espeak.R;
import com.reecedunn.espeak.ResourceIdListAdapter;
import com.reecedunn.espeak.VoiceSettings;
import com.reecedunn.espeak.VoiceVariant;

public class VoiceVariantPreference extends DialogPreference {
    private Spinner mCategory;
    private Spinner mVariant;

    private int mCategoryIndex = 0;
    private int mVariantIndex = 0;

    static class ViewHolder
    {
        public TextView text;
    }

    private class VariantData {
        private final int name;
        private final Object arg;
        private final VoiceVariant variant;

        protected VariantData(int name, String variant) {
            this(name, null, variant);
        }

        protected VariantData(int name, Object arg, String variant) {
            this.name = name;
            this.arg = arg;
            this.variant = VoiceVariant.parseVoiceVariant(variant);
        }

        public String getDisplayName(Context context) {
            String text = context.getText(name).toString();
            if (arg == null) {
                return text;
            }
            return String.format(text, arg);
        }

        public VoiceVariant getVariant() {
            return variant;
        }
    }

    public class VariantDataListAdapter extends ArrayAdapter<VariantData>
    {
        private final LayoutInflater mInflater;

        public VariantDataListAdapter(Activity context, VariantData[] resources)
        {
            super(context, android.R.layout.simple_list_item_1, resources);
            mInflater = context.getLayoutInflater();
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent)
        {
            ViewHolder holder;
            if (convertView == null)
            {
                convertView = mInflater.inflate(android.R.layout.simple_list_item_1, parent, false);
                holder = new ViewHolder();
                holder.text = (TextView)convertView.findViewById(android.R.id.text1);
                convertView.setTag(holder);
            }
            else
            {
                holder = (ViewHolder)convertView.getTag();
            }

            holder.text.setText(getItem(position).getDisplayName(getContext()));
            return convertView;
        }

        @Override
        public View getDropDownView(int position, View convertView, ViewGroup parent)
        {
            return getView(position, convertView, parent);
        }
    }

    private Integer[] categories = {
        R.string.variant_male,
        R.string.variant_female,
        R.string.variant_klatt,
        R.string.variant_nvda,
        R.string.variant_young,
        R.string.variant_old,
        R.string.variant_croak,
        R.string.variant_whisper,
    };

    private VariantData[][] variants = {
        {   // Male
            new VariantData(R.string.variant_default, "male"),
            new VariantData(R.string.variant_n, 1, "m1"),
            new VariantData(R.string.variant_n, 2, "m2"),
            new VariantData(R.string.variant_n, 3, "m3"),
            new VariantData(R.string.variant_n, 4, "m4"),
            new VariantData(R.string.variant_n, 5, "m5"),
            new VariantData(R.string.variant_n, 6, "m6"),
            new VariantData(R.string.variant_n, 7, "m7"),
            new VariantData(R.string.variant_n, 8, "m8"),
        },{ // Female
            new VariantData(R.string.variant_default, "female"),
            new VariantData(R.string.variant_n, 1, "f1"),
            new VariantData(R.string.variant_n, 2, "f2"),
            new VariantData(R.string.variant_n, 3, "f3"),
            new VariantData(R.string.variant_n, 4, "f4"),
            new VariantData(R.string.variant_n, 5, "f5"),
        },{ // Klatt
            new VariantData(R.string.variant_n, 1, "klatt"),
            new VariantData(R.string.variant_n, 2, "klatt2"),
            new VariantData(R.string.variant_n, 3, "klatt3"),
            new VariantData(R.string.variant_n, 4, "klatt4"),
            new VariantData(R.string.variant_n, 5, "klatt5"),
            new VariantData(R.string.variant_n, 6, "klatt6"),
        },{ // NVDA
            new VariantData(R.string.variant_adam, "adam"),
new VariantData(R.string.variant_adam2, "adam2"),
            new VariantData(R.string.variant_alex, "Alex"),
new VariantData(R.string.variant_alex2, "Alex2"),
new VariantData(R.string.variant_alexanderia, "Alexanderia"),
new VariantData(R.string.variant_alian, "alian"),
            new VariantData(R.string.variant_alicia, "Alicia"),
new VariantData(R.string.variant_alicia2, "Alicia2"),
new VariantData(R.string.variant_alien, "alien"),
            new VariantData(R.string.variant_andrea, "Andrea"),
new VariantData(R.string.variant_andrea2, "Andrea2"),
new VariantData(R.string.variant_andres, "andres"),
            new VariantData(R.string.variant_andy, "Andy"),
new VariantData(R.string.variant_andy2, "Andy2"),
            new VariantData(R.string.variant_anika, "anika"),
            new VariantData(R.string.variant_anika_robot, "anikaRobot"),
            new VariantData(R.string.variant_annie, "Annie"),
            new VariantData(R.string.variant_anouncer, "anouncer"),
            new VariantData(R.string.variant_antonio, "antonio"),
new VariantData(R.string.variant_antonio2, "antonio2"),
            new VariantData(R.string.variant_anxious_andy, "AnxiousAndy"),
new VariantData(R.string.variant_anxiousandy2, "AnxiousAndy2"),
            new VariantData(R.string.variant_aunty, "aunty"),
new VariantData(R.string.variant_aunty2, "aunty2"),
new VariantData(R.string.variant_baby, "baby"),
            new VariantData(R.string.variant_belinda, "belinda"),
new VariantData(R.string.variant_belinda2, "belinda2"),
            new VariantData(R.string.variant_benjamin, "benjamin"),
new VariantData(R.string.variant_benjamin2, "benjamin2"),
new VariantData(R.string.variant_benjamin3, "benjamin3"),
new VariantData(R.string.variant_betty, "betty"),
new VariantData(R.string.variant_betty2, "betty2"),
new VariantData(R.string.variant_betty3, "betty3"),
new VariantData(R.string.variant_betty4, "betty4"),
new VariantData(R.string.variant_bill, "Bill"),
new VariantData(R.string.variant_bns, "BNS"),
new VariantData(R.string.variant_bob, "bob"),
new VariantData(R.string.variant_bobby, "bobby"),
new VariantData(R.string.variant_bobby2, "bobby2"),
new VariantData(R.string.variant_bobby3, "bobby3"),
            new VariantData(R.string.variant_boris, "boris"),
new VariantData(R.string.variant_bug, "bug"),
new VariantData(R.string.variant_bugcrushed, "bugCrushed"),
            new VariantData(R.string.variant_caleb, "caleb"),
new VariantData(R.string.variant_caleb2, "caleb2"),
new VariantData(R.string.variant_calebf, "calebf"),
new VariantData(R.string.variant_caveman, "caveman"),
new VariantData(R.string.variant_caveman2, "caveman2"),
new VariantData(R.string.variant_chad, "chad"),
new VariantData(R.string.variant_chad2, "chad2"),
new VariantData(R.string.variant_chad3, "chad3"),
new VariantData(R.string.variant_chipmunk, "chipmunk"),
new VariantData(R.string.variant_croak2, "croak2"),
new VariantData(R.string.variant_daniel, "daniel"),
new VariantData(R.string.variant_daniel2, "daniel2"),
new VariantData(R.string.variant_dave, "dave"),
new VariantData(R.string.variant_dave2, "dave2"),
new VariantData(R.string.variant_dave3, "dave3"),
new VariantData(R.string.variant_dave4, "dave4"),
new VariantData(R.string.variant_dave5, "dave5"),
new VariantData(R.string.variant_dave6, "dave6"),
            new VariantData(R.string.variant_david, "david"),
new VariantData(R.string.variant_david2, "david2"),
new VariantData(R.string.variant_deepmale, "deepmale"),
new VariantData(R.string.variant_delta, "DELTA"),
            new VariantData(R.string.variant_demonic, "Demonic"),
new VariantData(R.string.variant_demonic2, "Demonic2"),
            new VariantData(R.string.variant_denis, "Denis"),
new VariantData(R.string.variant_dennis2, "Dennis2"),
new VariantData(R.string.variant_dennis3, "Dennis3"),
new VariantData(R.string.variant_dennis4, "Dennis4"),
new VariantData(R.string.variant_denture, "Denture"),
            new VariantData(R.string.variant_diogo, "Diogo"),
new VariantData(R.string.variant_doctor_dennis, "doctor dennis"),
new VariantData(R.string.variant_doctor_dennis2, "doctor dennis2"),
new VariantData(R.string.variant_doctor_dennis3, "doctor dennis3"),
new VariantData(R.string.variant_doctor_dennis4, "doctor dennis4"),
            new VariantData(R.string.variant_ed, "ed"),
new VariantData(R.string.variant_ed2, "ed2"),
new VariantData(R.string.variant_eddy, "eddy"),
new VariantData(R.string.variant_edith, "edith"),
            new VariantData(R.string.variant_edward, "edward"),
            new VariantData(R.string.variant_edward2, "edward2"),
new VariantData(R.string.variant_edward3, "edward3"),
new VariantData(R.string.variant_elif, "elif"),
new VariantData(R.string.variant_elif2, "elif2"),
new VariantData(R.string.variant_evil, "evil"),
new VariantData(R.string.variant_f6, "f6"),
new VariantData(R.string.variant_f7, "f7"),
new VariantData(R.string.variant_f8, "f8"),
new VariantData(R.string.variant_f9, "f9"),
new VariantData(R.string.variant_fast2, "fast2"),
new VariantData(R.string.variant_fatih, "fatih"),
new VariantData(R.string.variant_fatih2, "fatih2"),
new VariantData(R.string.variant_franco, "franco"),
new VariantData(R.string.variant_frank, "frank"),
new VariantData(R.string.variant_frank2, "frank2"),
            new VariantData(R.string.variant_gene, "Gene"),
            new VariantData(R.string.variant_gene2, "Gene2"),
new VariantData(R.string.variant_gene3, "Gene3"),
new VariantData(R.string.variant_gene4, "Gene4"),
new VariantData(R.string.variant_george, "George"),
new VariantData(R.string.variant_giantmonster, "giantmonster"),
new VariantData(R.string.variant_giantrobot, "giantrobot"),
new VariantData(R.string.variant_givinginstructions, "givingInstructions"),
new VariantData(R.string.variant_givinginstructions2, "givingInstructions2"),
new VariantData(R.string.variant_glen, "glen"),
            new VariantData(R.string.variant_grandma, "grandma"),
            new VariantData(R.string.variant_grandpa, "grandpa"),
            new VariantData(R.string.variant_gustave, "gustave"),
new VariantData(R.string.variant_gustave2, "gustave2"),
new VariantData(R.string.variant_harry, "harry"),
new VariantData(R.string.variant_harry2, "harry2"),
new VariantData(R.string.variant_harryrobot, "HarryRobot"),
            new VariantData(R.string.variant_henrique, "Henrique"),
            new VariantData(R.string.variant_hugo, "Hugo"),
new VariantData(R.string.variant_ian2, "ian2"),
            new VariantData(R.string.variant_iven, "iven"),
            new VariantData(R.string.variant_iven2, "iven2"),
            new VariantData(R.string.variant_iven3, "iven3"),
            new VariantData(R.string.variant_iven4, "iven4"),
new VariantData(R.string.variant_iven5, "iven5"),
            new VariantData(R.string.variant_jacky, "Jacky"),
new VariantData(R.string.variant_jason, "Jason"),
new VariantData(R.string.variant_jason2, "Jason2"),
            new VariantData(R.string.variant_john, "john"),
new VariantData(R.string.variant_john2, "john2"),
new VariantData(R.string.variant_jose, "jose"),
new VariantData(R.string.variant_josh, "josh"),
new VariantData(R.string.variant_josh2, "josh2"),
new VariantData(R.string.variant_julie, "julie"),
new VariantData(R.string.variant_julie2, "julie2"),
            new VariantData(R.string.variant_kaukovalta, "kaukovalta"),
new VariantData(R.string.variant_kevin, "kevin"),
new VariantData(R.string.variant_kit, "kit"),
new VariantData(R.string.variant_kit2, "kit2"),
new VariantData(R.string.variant_kit3, "kit3"),
new VariantData(R.string.variant_kit4, "kit4"),
new VariantData(R.string.variant_kit5, "kit5"),
new VariantData(R.string.variant_larry, "Larry"),
new VariantData(R.string.variant_larry2, "Larry2"),
new VariantData(R.string.variant_leann, "leann"),
            new VariantData(R.string.variant_lee, "Lee"),
new VariantData(R.string.variant_lee2, "Lee2"),
            new VariantData(R.string.variant_linda, "linda"),
new VariantData(R.string.variant_m9, "m9"),
            new VariantData(R.string.variant_marcelo, "Marcelo"),
new VariantData(R.string.variant_marcelo2, "marcelo2"),
new VariantData(R.string.variant_marcelo3, "marcelo3"),
            new VariantData(R.string.variant_marco, "Marco"),
            new VariantData(R.string.variant_mario, "Mario"),
            new VariantData(R.string.variant_max, "max"),
new VariantData(R.string.variant_max2, "max2"),
new VariantData(R.string.variant_max3, "max3"),
new VariantData(R.string.variant_max4, "max4"),
            new VariantData(R.string.variant_michael, "Michael"),
new VariantData(R.string.variant_michael2, "Michael2"),
            new VariantData(R.string.variant_michel, "michel"),
new VariantData(R.string.variant_michel2, "michel2"),
            new VariantData(R.string.variant_miguel, "miguel"),
new VariantData(R.string.variant_miguel2, "miguel2"),
            new VariantData(R.string.variant_mike, "Mike"),
new VariantData(R.string.variant_mike3, "Mike3"),
            new VariantData(R.string.variant_mr_serious, "Mr serious"),
new VariantData(R.string.variant_muhammed, "muhammed"),
new VariantData(R.string.variant_muhammed2, "muhammed2"),
            new VariantData(R.string.variant_nguyen, "Nguyen"),
            new VariantData(R.string.variant_norbert, "norbert"),
new VariantData(R.string.variant_osman, "osman"),
new VariantData(R.string.variant_osman2, "osman2"),
            new VariantData(R.string.variant_pablo, "pablo"),
            new VariantData(R.string.variant_paul, "paul"),
new VariantData(R.string.variant_paul2, "paul2"),
new VariantData(R.string.variant_paul3, "paul3"),
new VariantData(R.string.variant_paul3young, "Paul3young"),
            new VariantData(R.string.variant_pedro, "pedro"),
            new VariantData(R.string.variant_quincy, "quincy"),
new VariantData(R.string.variant_quincy2, "quincy2"),
new VariantData(R.string.variant_read, "read"),
new VariantData(R.string.variant_read2, "read2"),
new VariantData(R.string.variant_read3, "read3"),
new VariantData(R.string.variant_recep, "recep"),
new VariantData(R.string.variant_recep2, "recep2"),
new VariantData(R.string.variant_reed, "Reed"),
new VariantData(R.string.variant_reed2, "Reed2"),
            new VariantData(R.string.variant_ricishay_max, "RicishayMax"),
            new VariantData(R.string.variant_ricishay_max2, "RicishayMax2"),
            new VariantData(R.string.variant_ricishay_max3, "RicishayMax3"),
            new VariantData(R.string.variant_rob, "rob"),
new VariantData(R.string.variant_rob2, "rob2"),
            new VariantData(R.string.variant_robert, "robert"),
new VariantData(R.string.variant_robert2, "robert2"),
new VariantData(R.string.variant_robert3, "robert3"),
            new VariantData(R.string.variant_robosoft, "robosoft"),
            new VariantData(R.string.variant_robosoft2, "robosoft2"),
            new VariantData(R.string.variant_robosoft3, "robosoft3"),
            new VariantData(R.string.variant_robosoft3, "robosoft3"),
            new VariantData(R.string.variant_robosoft4, "robosoft4"),
            new VariantData(R.string.variant_robosoft4, "robosoft4"),
            new VariantData(R.string.variant_robosoft5, "robosoft5"),
            new VariantData(R.string.variant_robosoft6, "robosoft6"),
            new VariantData(R.string.variant_robosoft7, "robosoft7"),
            new VariantData(R.string.variant_robosoft8, "robosoft8"),
new VariantData(R.string.variant_rodger, "Rodger"),
new VariantData(R.string.variant_rodger2, "Rodger2"),
            new VariantData(R.string.variant_sandro, "sandro"),
new VariantData(R.string.variant_sandro2, "sandro2"),
new VariantData(R.string.variant_sandy, "sandy"),
new VariantData(R.string.variant_sandy2, "sandy2"),
new VariantData(R.string.variant_sarah, "sarah"),
            new VariantData(R.string.variant_shelby, "shelby"),
new VariantData(R.string.variant_shelby2, "shelby2"),
new VariantData(R.string.variant_shelley, "shelley"),
new VariantData(R.string.variant_sonya, "sonya"),
new VariantData(R.string.variant_sonya2, "sonya2"),
            new VariantData(R.string.variant_steph, "steph"),
            new VariantData(R.string.variant_steph2, "steph2"),
            new VariantData(R.string.variant_steph3, "steph3"),
new VariantData(R.string.variant_steph4, "steph4"),
            new VariantData(R.string.variant_storm, "Storm"),
new VariantData(R.string.variant_susan, "susan"),
new VariantData(R.string.variant_susan2, "Susan2"),
new VariantData(R.string.variant_susan3, "Susan3"),
new VariantData(R.string.variant_susan4, "susan4"),
new VariantData(R.string.variant_susan5, "Susan5"),
new VariantData(R.string.variant_susan6, "Susan6"),
new VariantData(R.string.variant_tom, "tom"),
new VariantData(R.string.variant_tom2, "tom2"),
new VariantData(R.string.variant_tom3, "tom3"),
            new VariantData(R.string.variant_travis, "travis"),
            new VariantData(R.string.variant_tweaky, "Tweaky"),
            new VariantData(R.string.variant_unirobot, "UniRobot"),
new VariantData(R.string.variant_ursula, "ursula"),
new VariantData(R.string.variant_ursula2, "ursula2"),
new VariantData(R.string.variant_ursula3, "ursula3"),
            new VariantData(R.string.variant_victor, "victor"),
new VariantData(R.string.variant_voiceoverartist, "voiceoverartist"),
new VariantData(R.string.variant_wave, "wave"),
new VariantData(R.string.variant_wave2, "wave2"),
new VariantData(R.string.variant_wave3, "wave3"),
new VariantData(R.string.variant_weirdvariant, "weirdvariant"),
new VariantData(R.string.variant_whisper2, "whisper2"),
new VariantData(R.string.variant_yelling, "yelling"),
new VariantData(R.string.variant_yelling2, "yelling2"),
            new VariantData(R.string.variant_zac, "zac"),
new VariantData(R.string.variant_zbonddenture, "ZBondDenture"),
new VariantData(R.string.variant_zira, "zira"),
        },{ // Young
            new VariantData(R.string.variant_male, "male-young"),
            new VariantData(R.string.variant_female, "female-young"),
        },{ // Old
            new VariantData(R.string.variant_male, "male-old"),
            new VariantData(R.string.variant_female, "female-old"),
        },{ // Croak
            new VariantData(R.string.variant_male, "croak"),
        },{ // Whisper
            new VariantData(R.string.variant_male, "whisper"),
            new VariantData(R.string.variant_female, "whisperf"),
        },
    };

    public VoiceVariantPreference(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        setDialogLayoutResource(R.layout.voice_variant_preference);
        setLayoutResource(R.layout.information_view);
        setPositiveButtonText(android.R.string.ok);
        setNegativeButtonText(android.R.string.cancel);
    }

    public VoiceVariantPreference(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public VoiceVariantPreference(Context context) {
        this(context, null);
    }

    public void setVoiceVariant(VoiceVariant variant) {
        for (int i = 0; i < variants.length; ++i) {
            VariantData[] items = variants[i];
            for (int j = 0; j < items.length; ++j) {
                if (items[j].getVariant().equals(variant)) {
                    mCategoryIndex = i;
                    mVariantIndex  = j;
                    onDataChanged();
                    return;
                }
            }
        }
        onDataChanged();
    }

    @Override
    protected View onCreateDialogView() {
        View root = super.onCreateDialogView();
        mCategory = (Spinner)root.findViewById(R.id.category);
        mVariant = (Spinner)root.findViewById(R.id.variant);
        return root;
    }

    @Override
    protected void onBindDialogView(View view) {
        super.onBindDialogView(view);

        // Cache the indices so they don't get overwritten by the OnItemSelectedListener handlers.
        final int category = mCategoryIndex;
        final int variant  = mVariantIndex;

        mCategory.setAdapter(new ResourceIdListAdapter((Activity)getContext(), categories));
        mCategory.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            private boolean mInitializing = true;

            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int position, long id) {
                mVariant.setAdapter(new VariantDataListAdapter((Activity) getContext(), variants[position]));
                if (mInitializing) {
                    mVariant.setSelection(variant);
                    mInitializing = false;
                }
                mCategoryIndex = position;
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
            }
        });
        mVariant.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int position, long id) {
                mVariantIndex = position;
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
            }
        });

        mCategory.setSelection(category);
    }

    @Override
    public void onClick(DialogInterface dialog, int which) {
        switch (which) {
            case DialogInterface.BUTTON_POSITIVE:
                onDataChanged();
                if (shouldCommit()) {
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N)
                    {
                        PreferenceManager preferenceManager = getPreferenceManager();
                        preferenceManager.setStorageDeviceProtected ();
                    }
                    SharedPreferences.Editor editor = getEditor();
                    if (editor != null) {
                        VoiceVariant variant = variants[mCategoryIndex][mVariantIndex].getVariant();
                        editor.putString(VoiceSettings.PREF_VARIANT, variant.toString());
                        editor.commit();
                    }
                }
                break;
        }
        super.onClick(dialog, which);
    }

    private void onDataChanged() {
        Context context = getContext();
        CharSequence category = context.getText(categories[mCategoryIndex]);
        CharSequence variant  = variants[mCategoryIndex][mVariantIndex].getDisplayName(context);
        callChangeListener(String.format("%s (%s)", category, variant));
    }
}
