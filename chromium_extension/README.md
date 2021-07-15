<h5>Motivation</h5>

Web Speech API does not support SSML input to the speech synthesis engine https://github.com/WICG/speech-api/issues/10, or the ability to capture the output of `speechSynthesis.speak()` as a`MedaiStreamTrack` or raw audio https://lists.w3.org/Archives/Public/public-speech-api/2017Jun/0000.html.

See [Issue 1115640: [FUGU] NativeTransferableStream](https://bugs.chromium.org/p/chromium/issues/detail?id=1115640).

<h5>Synopsis</h5>

Native Messaging => eSpeak NG => PHP `passthru()` => `fetch()` => Transferable Streams => `MediaStreamTrack`.

Use local `espeak-ng` with `-m` option set in the browser. 

Output speech sythesis audio as a live `MediaStreamTrack`.

Use [Native Messaging](https://developer.chrome.com/extensions/nativeMessaging), PHP `passthru()` to input text and [Speech Synthesis Markup Language](https://www.w3.org/TR/speech-synthesis11/) as STDIN to [`espeak-ng`](https://github.com/espeak-ng/espeak-ng), stream STDOUT in "real-time" as live `MediaStreamTrack`. 

<h5>Install<h5>

<h6>Dependencies</h6>

eSpeak NG [Building eSpeak NG](https://github.com/espeak-ng/espeak-ng/blob/master/docs/building.md#building-espeak-ng).

PHP is used for `passthru()`. Substitute server language of choice.

```
git clone https://github.com/guest271314/native-messaging-espeak-ng.git
cd native-messaging-espeak-ng/
chmod +x local_server.sh index.php
```

Navigate to `chrome://extensions`, set `Developer mode` to on, click `Load unpacked`, select downloaded git directory.

Note the generated extension ID, substitute that value for `<id>` in `native_messaging_espeakng.json`, `AudioStream.js`, `index.php`; add the value to `"extensions"` array in `manifest.json`.

Substitute full local path to `local_server.sh` for `/path/to` in `native_messaging_espeakng.json`.

Copy `native_messaging_espeakng.json` to `NativeMessagingHosts` directory in Chromium or Chrome configuration folder, on Linux, i.e., `~/.config/chromium`; `~/.config/google-chrome-unstable`.

`cp native_messaging_espeakng ~/.config/chromium/NativeMessagingHosts`

Reload extension.

<h5>Usage</h5>

On origins listed in `"matches"` array in `"web_accessible_resources"` object in `manifest.json`, e.g., at `console`

```
var text = `Test`;
var stdin = `espeak-ng -m --stdout "${text}"`;
var espeakng = new AudioStream({ stdin, recorder: true });
// espeakng.mediaStream is a live MediaStreamTrack
var ab = await espeakng.start();
console.log(
  URL.createObjectURL(new Blob([ab], { type: 'audio/webm;codecs=opus' }))
);
```
  
Abort the request and audio output.
  
```
await espeakng.abort()
```
  
To turn local server on and off with user action pin and click the extension icon on Chromium or Chrome toolbar.
