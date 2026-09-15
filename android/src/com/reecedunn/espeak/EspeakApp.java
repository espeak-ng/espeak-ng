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

import android.annotation.TargetApi;
import android.app.Application;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.UserManager;
import android.preference.PreferenceManager;
import android.util.Log;

public class EspeakApp extends Application {

    private static final String TAG = EspeakApp.class.getSimpleName();

    /**
     * Written to device-protected storage the first time
     * {@link #migrateLegacyPreferences} runs with the credential-encrypted file
     * readable, so that file is never empty again after the first unlocked
     * start and a stray credential-encrypted file can never be adopted.
     */
    public static final String PREF_PREFERENCES_MIGRATED = "espeak_preferences_migrated";

    private static Context storageContext;

    public void onCreate() {
        super.onCreate();
        final Context appContext = getApplicationContext();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            EspeakApp.storageContext = appContext.createDeviceProtectedStorageContext();
            migrateLegacyPreferences(appContext, EspeakApp.storageContext);
            if (!appContext.getSystemService(UserManager.class).isUserUnlocked()) {
                // Started at boot, for a direct-boot-aware screen reader. The
                // credential-encrypted file cannot be read yet, so run the
                // migration again when the user unlocks instead of leaving it
                // to the next process restart, which may be days away.
                // ACTION_USER_UNLOCKED is a protected system broadcast, exempt
                // from the exported flag that targetSdk 34 requires otherwise.
                appContext.registerReceiver(new BroadcastReceiver() {
                    @Override
                    public void onReceive(Context context, Intent intent) {
                        appContext.unregisterReceiver(this);
                        migrateLegacyPreferences(appContext, EspeakApp.storageContext);
                    }
                }, new IntentFilter(Intent.ACTION_USER_UNLOCKED));
            }
        }
        else {
            EspeakApp.storageContext = appContext;
        }
        syncWearLauncherState();
    }

    /**
     * Adopts the preferences of an install that predates device-protected
     * storage (f7f66429, 2022) and still keeps them in the credential-encrypted
     * file. Runs at process start, before any component reads the settings,
     * and again on unlock when the process started before the user unlocked.
     *
     * Everything in this app reads and writes the device-protected file, and
     * {@link #PREF_PREFERENCES_MIGRATED} lands there the first time the
     * credential-encrypted file is readable, so once the device-protected file
     * holds anything it is the source of truth and there is nothing left to
     * migrate. A credential-encrypted file that shows up after that point can
     * only be a stray write through a plain Context, and
     * moveSharedPreferencesFrom() would copy it over the user's settings: that
     * is how #2536 wiped every setting on each screen reader restart. Such a
     * file is discarded instead, which turns that class of bug into a setting
     * that does not take effect -- visible, and harmless.
     */
    @TargetApi(Build.VERSION_CODES.N)
    public static void migrateLegacyPreferences(Context appContext, Context storageContext) {
        final String name = PreferenceManager.getDefaultSharedPreferencesName(appContext);
        if (!storageContext.getSharedPreferences(name, Context.MODE_PRIVATE).getAll().isEmpty()) {
            appContext.deleteSharedPreferences(name);
            return;
        }
        if (!appContext.getSystemService(UserManager.class).isUserUnlocked()) {
            // Credential-encrypted storage is not readable before the first
            // unlock; onCreate() retries this on ACTION_USER_UNLOCKED.
            return;
        }
        storageContext.moveSharedPreferencesFrom(appContext, name);
        // The move evicts the cached instance, so fetch the file anew.
        storageContext.getSharedPreferences(name, Context.MODE_PRIVATE).edit()
                .putBoolean(PREF_PREFERENCES_MIGRATED, true).apply();
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
