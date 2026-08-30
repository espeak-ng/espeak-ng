#!/usr/bin/env python3
"""Automated output-pipeline quality checks for native 44.1 kHz synthesis."""

import io
import json
import math
import os
import struct
import subprocess
import sys
import time
import wave

EXPECTED_RATE = 44100
MAX_FIRST_AUDIO_MS = 2000.0
MAX_SAMPLE_PEAK = 0.9999
MAX_JUMP = 0.60
MIN_LUFS = -40.0
MAX_LUFS = -3.0

CORPUS = (
    ("en", "Sharp clicks, smooth speech, fast transitions, and strong peaks."),
    ("en+max", "Output quality remains stable during rapid screen reader use."),
    ("en+klatt", "A clean signal should have no clipping or boundary clicks."),
    ("ar", "هذا اختبار آلي لجودة الصوت واستقرار المخزن المؤقت."),
    ("cmn", "这是一个自动语音质量测试。"),
)


def biquad(samples, b0, b1, b2, a1, a2):
    out = []
    x1 = x2 = y1 = y2 = 0.0
    for x0 in samples:
        y0 = b0 * x0 + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2
        out.append(y0)
        x2, x1 = x1, x0
        y2, y1 = y1, y0
    return out


def high_shelf(fs, frequency=1681.974450955533, gain_db=3.999843853973347):
    # RBJ high-shelf approximation to the BS.1770 pre-filter.
    A = 10.0 ** (gain_db / 40.0)
    w0 = 2.0 * math.pi * frequency / fs
    c, s = math.cos(w0), math.sin(w0)
    alpha = s / 2.0 * math.sqrt(2.0)
    root = math.sqrt(A)
    b0 = A * ((A + 1) + (A - 1) * c + 2 * root * alpha)
    b1 = -2 * A * ((A - 1) + (A + 1) * c)
    b2 = A * ((A + 1) + (A - 1) * c - 2 * root * alpha)
    a0 = (A + 1) - (A - 1) * c + 2 * root * alpha
    a1 = 2 * ((A - 1) - (A + 1) * c)
    a2 = (A + 1) - (A - 1) * c - 2 * root * alpha
    return tuple(v / a0 for v in (b0, b1, b2, a1, a2))


def high_pass(fs, frequency=38.13547087602444, q=0.5003270373238773):
    w0 = 2.0 * math.pi * frequency / fs
    c, s = math.cos(w0), math.sin(w0)
    alpha = s / (2.0 * q)
    b0, b1, b2 = (1 + c) / 2, -(1 + c), (1 + c) / 2
    a0, a1, a2 = 1 + alpha, -2 * c, 1 - alpha
    return tuple(v / a0 for v in (b0, b1, b2, a1, a2))


def integrated_lufs(samples, fs):
    weighted = biquad(samples, *high_shelf(fs))
    weighted = biquad(weighted, *high_pass(fs))
    block = int(0.400 * fs)
    hop = int(0.100 * fs)
    powers = []
    for start in range(0, len(weighted) - block + 1, hop):
        frame = weighted[start:start + block]
        power = sum(x * x for x in frame) / block
        if power > 0:
            powers.append(power)
    if not powers:
        return float("-inf")
    loudness = [-0.691 + 10.0 * math.log10(p) for p in powers]
    absolute = [p for p, level in zip(powers, loudness) if level > -70.0]
    if not absolute:
        return float("-inf")
    preliminary = -0.691 + 10.0 * math.log10(sum(absolute) / len(absolute))
    relative_gate = preliminary - 10.0
    gated = [p for p, level in zip(powers, loudness)
             if level > -70.0 and level > relative_gate]
    return -0.691 + 10.0 * math.log10(sum(gated) / len(gated))


def synthesize(executable, voice, text):
    command = [executable, "--stdout", "-v", voice, text]
    started = time.monotonic()
    process = subprocess.Popen(command, stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
    first = process.stdout.read(4096)
    first_audio_ms = (time.monotonic() - started) * 1000.0
    output = first + process.stdout.read()
    errors = process.stderr.read().decode("utf-8", "replace")
    status = process.wait()
    if status != 0:
        raise RuntimeError(f"{voice}: synthesis failed ({status}): {errors}")
    with wave.open(io.BytesIO(output), "rb") as wav:
        if wav.getnchannels() != 1 or wav.getsampwidth() != 2:
            raise RuntimeError(f"{voice}: expected mono signed 16-bit PCM")
        rate = wav.getframerate()
        data = wav.readframes(wav.getnframes())
    values = struct.unpack("<%dh" % (len(data) // 2), data)
    normalized = [value / 32768.0 for value in values]
    peak = max(abs(value) for value in normalized)
    clipped = sum(abs(value) >= 32767 for value in values)
    jump = max(abs(values[i] - values[i - 1]) for i in range(1, len(values))) / 32768.0
    return {
        "voice": voice,
        "sample_rate": rate,
        "first_audio_ms": round(first_audio_ms, 2),
        "lufs": round(integrated_lufs(normalized, rate), 2),
        "sample_peak": round(peak, 6),
        "clipped_samples": clipped,
        "max_jump": round(jump, 6),
        "frames": len(values),
    }


def main():
    executable = os.environ.get("ESPEAK_BIN")
    if not executable:
        raise RuntimeError("ESPEAK_BIN is not set")
    failures = []
    for voice, text in CORPUS:
        result = synthesize(executable, voice, text)
        print(json.dumps(result, ensure_ascii=False, sort_keys=True))
        if result["sample_rate"] != EXPECTED_RATE:
            failures.append(f"{voice}: sample rate {result['sample_rate']}")
        if result["first_audio_ms"] > MAX_FIRST_AUDIO_MS:
            failures.append(f"{voice}: first audio {result['first_audio_ms']} ms")
        if not (MIN_LUFS <= result["lufs"] <= MAX_LUFS):
            failures.append(f"{voice}: loudness {result['lufs']} LUFS")
        if result["sample_peak"] > MAX_SAMPLE_PEAK or result["clipped_samples"]:
            failures.append(f"{voice}: clipping detected")
        if result["max_jump"] > MAX_JUMP:
            failures.append(f"{voice}: discontinuity {result['max_jump']}")
    if failures:
        print("Audio quality failures:", file=sys.stderr)
        for failure in failures:
            print(" - " + failure, file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
