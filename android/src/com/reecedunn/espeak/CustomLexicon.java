/*
 * Copyright (C) 2026 eSpeak NG contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 */

package com.reecedunn.espeak;

import android.content.Context;
import android.util.AtomicFile;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/** A small, language-scoped word/phrase replacement lexicon. */
public final class CustomLexicon {
    private static final String FILE_NAME = "custom-lexicon.json";
    private static final int MAX_FILE_BYTES = 1024 * 1024;
    private static final int MAX_ENTRIES = 2048;
    private static final int MAX_TARGET_LENGTH = 128;
    private static final int MAX_REPLACEMENT_LENGTH = 512;
    private static final String WORD_CHARACTER = "[\\p{L}\\p{M}\\p{N}_]";

    private static LexiconData sCache;
    private static long sCacheTimestamp = Long.MIN_VALUE;

    private CustomLexicon() {
    }

    /** Result text plus a map from transformed code-point boundaries to original UTF-16 offsets. */
    public static final class ReplacementResult {
        public final String text;
        private final int[] mOriginalOffsets;

        ReplacementResult(String text, int[] originalOffsets) {
            this.text = text;
            mOriginalOffsets = originalOffsets;
        }

        public int originalOffsetForCodePoint(int codePointBoundary) {
            if (codePointBoundary <= 0) return 0;
            if (codePointBoundary >= mOriginalOffsets.length) {
                return mOriginalOffsets[mOriginalOffsets.length - 1];
            }
            return mOriginalOffsets[codePointBoundary];
        }
    }

    private static final class CompiledLanguage {
        final Map<String, String> replacements;
        final Pattern pattern;

        CompiledLanguage(Map<String, String> values) {
            replacements = values;
            List<String> targets = new ArrayList<String>(values.keySet());
            Collections.sort(targets, new Comparator<String>() {
                @Override
                public int compare(String left, String right) {
                    return Integer.compare(right.length(), left.length());
                }
            });
            StringBuilder expression = new StringBuilder();
            for (String target : targets) {
                if (expression.length() > 0) expression.append('|');
                expression.append(Pattern.quote(target));
            }
            pattern = targets.isEmpty() ? null : Pattern.compile(
                    "(?<!" + WORD_CHARACTER + ")(?:" + expression + ")(?!"
                            + WORD_CHARACTER + ")");
        }
    }

    private static final class LexiconData {
        final Map<String, Map<String, String>> values =
                new LinkedHashMap<String, Map<String, String>>();
        final Map<String, CompiledLanguage> compiled =
                new HashMap<String, CompiledLanguage>();

        CompiledLanguage forLanguage(String language) {
            CompiledLanguage result = compiled.get(language);
            if (result != null) return result;
            Map<String, String> languageValues = values.get(language);
            if (languageValues == null && language.contains("-")) {
                languageValues = values.get(language.substring(0, language.indexOf('-')));
            }
            result = new CompiledLanguage(languageValues == null
                    ? Collections.<String, String>emptyMap() : languageValues);
            compiled.put(language, result);
            return result;
        }
    }

    public static synchronized void saveMapping(Context context, String language,
                                                String target, String replacement)
            throws IOException {
        language = normalize(language, 32, "language");
        target = normalize(target, MAX_TARGET_LENGTH, "target");
        replacement = normalize(replacement, MAX_REPLACEMENT_LENGTH, "replacement");

        LexiconData data = readData(context);
        int count = entryCount(data);
        Map<String, String> values = data.values.get(language);
        if (values == null) {
            values = new LinkedHashMap<String, String>();
            data.values.put(language, values);
        }
        if (!values.containsKey(target) && count >= MAX_ENTRIES) {
            throw new IOException("The custom lexicon is full");
        }
        values.put(target, replacement);
        writeData(context, data);
        invalidateCache();
    }

    public static synchronized int entryCount(Context context) throws IOException {
        return entryCount(readData(context));
    }

    public static synchronized ReplacementResult apply(Context context, String language,
                                                       String source) {
        if (source == null || source.isEmpty() || language == null) {
            return identity(source == null ? "" : source);
        }
        // Replacing inside markup could corrupt element or attribute names.
        // Android screen-reader requests are plain text; leave explicit SSML intact.
        if (source.regionMatches(true, 0, "<speak", 0, 6)) {
            return identity(source);
        }
        try {
            CompiledLanguage lexicon = readData(context).forLanguage(language);
            if (lexicon.pattern == null) return identity(source);
            Matcher matcher = lexicon.pattern.matcher(source);
            if (!matcher.find()) return identity(source);

            StringBuilder output = new StringBuilder(source.length());
            List<Integer> offsets = new ArrayList<Integer>();
            offsets.add(0);
            int sourceOffset = 0;
            do {
                appendIdentity(source, sourceOffset, matcher.start(), output, offsets);
                String replacement = lexicon.replacements.get(matcher.group());
                appendReplacement(replacement, matcher.start(), matcher.end(), output, offsets);
                sourceOffset = matcher.end();
            } while (matcher.find());
            appendIdentity(source, sourceOffset, source.length(), output, offsets);
            int[] mapping = new int[offsets.size()];
            for (int i = 0; i < offsets.size(); i++) mapping[i] = offsets.get(i);
            return new ReplacementResult(output.toString(), mapping);
        } catch (IOException e) {
            // A damaged optional lexicon must never take down the TTS service.
            return identity(source);
        }
    }

    public static synchronized void clear(Context context) {
        getFile(context).delete();
        invalidateCache();
    }

    private static ReplacementResult identity(String text) {
        int count = text.codePointCount(0, text.length());
        int[] offsets = new int[count + 1];
        int offset = 0;
        for (int i = 0; i < count; i++) {
            offsets[i] = offset;
            offset = text.offsetByCodePoints(offset, 1);
        }
        offsets[count] = text.length();
        return new ReplacementResult(text, offsets);
    }

    private static void appendIdentity(String source, int start, int end,
                                       StringBuilder output, List<Integer> offsets) {
        int position = start;
        while (position < end) {
            int codePoint = source.codePointAt(position);
            output.appendCodePoint(codePoint);
            position += Character.charCount(codePoint);
            offsets.add(position);
        }
    }

    private static void appendReplacement(String replacement, int originalStart,
                                          int originalEnd, StringBuilder output,
                                          List<Integer> offsets) {
        int count = replacement.codePointCount(0, replacement.length());
        int position = 0;
        for (int i = 0; i < count; i++) {
            output.appendCodePoint(replacement.codePointAt(position));
            position = replacement.offsetByCodePoints(position, 1);
            offsets.add(i == count - 1 ? originalEnd : originalStart);
        }
    }

    private static String normalize(String value, int maximum, String field)
            throws IOException {
        if (value == null) throw new IOException("Missing " + field);
        value = value.trim();
        if (value.isEmpty()) throw new IOException("Empty " + field);
        if (value.length() > maximum) throw new IOException(field + " is too long");
        if (value.indexOf('\n') >= 0 || value.indexOf('\r') >= 0
                || value.indexOf('\t') >= 0) {
            throw new IOException(field + " contains a line separator");
        }
        return value;
    }

    private static int entryCount(LexiconData data) {
        int count = 0;
        for (Map<String, String> values : data.values.values()) count += values.size();
        return count;
    }

    private static File getFile(Context context) {
        return new File(context.getFilesDir(), FILE_NAME);
    }

    private static LexiconData readData(Context context) throws IOException {
        File file = getFile(context);
        long timestamp = file.exists() ? file.lastModified() : -1;
        if (sCache != null && sCacheTimestamp == timestamp) return sCache;

        LexiconData data = new LexiconData();
        if (file.exists()) {
            if (file.length() > MAX_FILE_BYTES) throw new IOException("Lexicon is too large");
            byte[] bytes;
            try (FileInputStream input = new FileInputStream(file);
                 ByteArrayOutputStream output = new ByteArrayOutputStream((int) file.length())) {
                byte[] buffer = new byte[8192];
                int count;
                while ((count = input.read(buffer)) != -1) output.write(buffer, 0, count);
                bytes = output.toByteArray();
            }
            try {
                JSONObject root = new JSONObject(new String(bytes, StandardCharsets.UTF_8));
                JSONArray mappings = root.optJSONArray("mappings");
                if (mappings != null) {
                    for (int i = 0; i < mappings.length() && entryCount(data) < MAX_ENTRIES; i++) {
                        JSONObject mapping = mappings.getJSONObject(i);
                        String language = mapping.getString("language");
                        String target = mapping.getString("target");
                        String replacement = mapping.getString("replacement");
                        Map<String, String> values = data.values.get(language);
                        if (values == null) {
                            values = new LinkedHashMap<String, String>();
                            data.values.put(language, values);
                        }
                        values.put(target, replacement);
                    }
                }
            } catch (JSONException e) {
                throw new IOException("Invalid custom lexicon", e);
            }
        }
        sCache = data;
        sCacheTimestamp = timestamp;
        return data;
    }

    private static void writeData(Context context, LexiconData data) throws IOException {
        JSONObject root = new JSONObject();
        JSONArray mappings = new JSONArray();
        try {
            root.put("version", 1);
            for (Map.Entry<String, Map<String, String>> language : data.values.entrySet()) {
                for (Map.Entry<String, String> value : language.getValue().entrySet()) {
                    JSONObject mapping = new JSONObject();
                    mapping.put("language", language.getKey());
                    mapping.put("target", value.getKey());
                    mapping.put("replacement", value.getValue());
                    mappings.put(mapping);
                }
            }
            root.put("mappings", mappings);
        } catch (JSONException e) {
            throw new IOException(e);
        }
        byte[] bytes = root.toString().getBytes(StandardCharsets.UTF_8);
        if (bytes.length > MAX_FILE_BYTES) throw new IOException("Lexicon is too large");

        AtomicFile atomic = new AtomicFile(getFile(context));
        FileOutputStream output = null;
        try {
            output = atomic.startWrite();
            output.write(bytes);
            output.getFD().sync();
            atomic.finishWrite(output);
        } catch (IOException e) {
            if (output != null) atomic.failWrite(output);
            throw e;
        }
    }

    private static void invalidateCache() {
        sCache = null;
        sCacheTimestamp = Long.MIN_VALUE;
    }
}
