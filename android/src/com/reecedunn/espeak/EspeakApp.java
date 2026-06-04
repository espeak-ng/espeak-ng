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

public class EspeakApp extends Application {

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
     * Done at runtime rather than via @bool/-watch resource on the
     * activity-alias's android:enabled: PackageManager parses that attribute
     * against a configuration that does not include the device's UI mode,
     * so a values-watch override resolves to its default at install time
     * (verified on a Pixel Watch 3, mCurUiMode=0x16, where the alias was
     * registered as disabled despite the bool's watch qualifier).
     * setComponentEnabledSetting bypasses the manifest-time resolution.
     * Use the FQCN built from the Java package, not getPackageName(): the
     * latter returns the runtime applicationId, which may differ from the
     * manifest namespace that the activity-alias's relative ".WearLauncher"
     * was resolved against at build time.
     */
    private void syncWearLauncherState() {
        PackageManager pm = getPackageManager();
        boolean isWatch = pm.hasSystemFeature(PackageManager.FEATURE_WATCH);
        ComponentName alias = new ComponentName(this, EspeakApp.class.getPackage().getName() + ".WearLauncher");
        int desired = isWatch
            ? PackageManager.COMPONENT_ENABLED_STATE_ENABLED
            : PackageManager.COMPONENT_ENABLED_STATE_DISABLED;
        if (pm.getComponentEnabledSetting(alias) != desired) {
            pm.setComponentEnabledSetting(alias, desired, PackageManager.DONT_KILL_APP);
        }
    }

    public static Context getStorageContext() {
        return EspeakApp.storageContext;
    }
}
