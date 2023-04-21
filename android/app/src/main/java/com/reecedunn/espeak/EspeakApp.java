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
import android.content.Context;
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
    }

    public static Context getStorageContext() {
        return EspeakApp.storageContext;
    }
}
