/*
 * Copyright (C) 2012-2013 Reece H. Dunn
 * Copyright (C) 2009 The Android Open Source Project
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

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class FileUtils {
    public static String read(InputStream stream) throws IOException {
        ByteArrayOutputStream content = new ByteArrayOutputStream();
        int c = stream.read();
        while (c != -1)
        {
            content.write((byte)c);
            c = stream.read();
        }
        return content.toString();
    }

    public static void write(File outputFile, String contents) throws IOException {
        FileOutputStream outputStream = new FileOutputStream(outputFile);
        try {
            outputStream.write(contents.getBytes(), 0, contents.length());
        } finally {
            outputStream.close();
        }
        chmod(outputFile);
    }

    public static void chmod(File file) {
        try {
            Runtime.getRuntime().exec("/system/bin/chmod 755 " + file.getAbsolutePath());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void rmdir(File directory) {
        if (!directory.exists() || !directory.isDirectory()) {
            return;
        }

        for (File child : directory.listFiles()) {
            if (child.isDirectory()) {
                rmdir(child);
            }

            child.delete();
        }
    }
}
