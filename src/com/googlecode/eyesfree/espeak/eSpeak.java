/*
 * Copyright (C) 2011 The Android Open Source Project
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

package com.googlecode.eyesfree.espeak;

import android.app.Activity;
import android.os.Bundle;

/*
 * The Java portion of this TTS plugin engine app does nothing.
 * This activity is only here so that the native code can be
 * wrapped up inside an apk file.
 *
 * The file path structure convention is that the native library
 * implementing TTS must be a file placed here:
 * /data/data/<PACKAGE_NAME>/lib/libtts<ACTIVITY_NAME_LOWERCASED>.so
 * Example:
 * /data/data/com.googlecode.eyesfree.espeak/lib/libttsespeak.so
 */

public class eSpeak extends Activity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // The Java portion of this does nothing.
        // This activity is only here so that everything
        // can be wrapped up inside an apk file.
        finish();
    }
}
