/*
 * Copyright (C) 2022 Beka Gozalishvili
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

import android.app.Application;
import android.content.ComponentName;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.util.Log;

public class EspeakApp extends Application {

    private static final String TAG = EspeakApp.class.getSimpleName();

    private static Context storageContext;

    public void onCreate() {
        super.onCreate();
        Context appContext = getApplicationContext();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            EspeakApp.storageContext = appContext.createDeviceProtectedStorageContext();
        }
        else {
            EspeakApp.storageContext = appContext;
        }
        syncWearLauncherState();
    }

    /**
     * The launcher icon should appear only on Wear, where the system
     * Text-to-speech settings has no per-engine config affordance. On phones
     * the user reaches CONFIGURE_ENGINE through the gear in TTS settings.
     *
     * The alias ships android:enabled="false", so this only ever *enables* it,
     * and only on a watch. Never disable it: disabling a launcher alias closes
     * the activity that was started through it -- silently, with no exception,
     * and DONT_KILL_APP does not prevent it. A phone tapping the icon that the
     * old always-enabled alias put on the home screen therefore watched the
     * settings screen open and immediately vanish, which reads as a crash.
     * Shipping the alias disabled means a phone is already in the desired
     * state and needs no PackageManager write at all.
     *
     * A @bool/-watch resource on android:enabled cannot replace this:
     * PackageManager parses that attribute against a configuration that does
     * not include the device's UI mode, so a values-watch override resolves to
     * its default at install time (verified on a Pixel Watch 3,
     * mCurUiMode=0x16, where the alias stayed disabled despite the bool's
     * watch qualifier). setComponentEnabledSetting bypasses that resolution.
     *
     * Use the FQCN built from the Java package, not getPackageName(): the
     * latter returns the runtime applicationId, which may differ from the
     * manifest namespace that the activity-alias's relative ".WearLauncher"
     * was resolved against at build time.
     *
     * TtsService shares this process, so onCreate() also runs whenever the TTS
     * framework binds the engine -- at boot, or the first time a screen reader
     * speaks. The launcher alias is irrelevant to that path, so the work is
     * pushed off the startup thread rather than charging binder round trips to
     * every cold start. It is also fully guarded: changing component state can
     * be refused by device policy on managed profiles and some vendor ROMs, and
     * an escaping exception here would take down the engine before it ever
     * reaches onSynthesizeText().
     */
    private void syncWearLauncherState() {
        final PackageManager pm = getPackageManager();
        final ComponentName alias = new ComponentName(
            this, EspeakApp.class.getPackage().getName() + ".WearLauncher");

        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    if (!pm.hasSystemFeature(PackageManager.FEATURE_WATCH)) {
                        return; // already disabled in the manifest
                    }
                    if (pm.getComponentEnabledSetting(alias)
                            != PackageManager.COMPONENT_ENABLED_STATE_ENABLED) {
                        pm.setComponentEnabledSetting(alias,
                            PackageManager.COMPONENT_ENABLED_STATE_ENABLED,
                            PackageManager.DONT_KILL_APP);
                    }
                } catch (RuntimeException e) {
                    // SecurityException (device policy), IllegalArgumentException
                    // (alias missing from a repackaged build), or anything a
                    // vendor PackageManager throws. Losing the launcher icon is
                    // a cosmetic failure; taking the process down is not.
                    Log.w(TAG, "Could not enable the Wear launcher alias", e);
                }
            }
        }, "wear-launcher-sync").start();
    }

    public static Context getStorageContext() {
        return EspeakApp.storageContext;
    }
}
