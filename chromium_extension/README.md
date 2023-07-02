<h5>Motivation</h5>

Web Speech API does not support SSML input to the speech synthesis engine https://github.com/WICG/speech-api/issues/10, or the ability to capture the output of `speechSynthesis.speak()` as a`MedaiStreamTrack` or raw audio https://lists.w3.org/Archives/Public/public-speech-api/2017Jun/0000.html.

See [Issue 1115640: [FUGU] NativeTransferableStream](https://bugs.chromium.org/p/chromium/issues/detail?id=1115640).

<h5>Synopsis</h5>

[`"externally_connectable"`](https://developer.chrome.com/docs/extensions/mv3/manifest/externally_connectable/) => Native Messaging => eSpeak NG => `MediaStreamTrack`.

Use local `espeak-ng` with `-m` option set in the browser. 

Output speech sythesis audio as a live `MediaStreamTrack`.

Use [Native Messaging](https://developer.chrome.com/extensions/nativeMessaging), Bash with GNU Core Utiltities to input text and [Speech Synthesis Markup Language](https://www.w3.org/TR/speech-synthesis11/) as STDIN to [`espeak-ng`](https://github.com/espeak-ng/espeak-ng), stream STDOUT in "real-time" as live `MediaStreamTrack`. 

<h5>Install<h5>

<h6>Dependencies</h6>

eSpeak NG [Building eSpeak NG](https://github.com/espeak-ng/espeak-ng/blob/master/docs/building.md#building-espeak-ng).
 

```
git clone https://github.com/guest271314/native-messaging-espeak-ng.git
cd native-messaging-espeak-ng
chmod u+x nm_espeak_ng.sh
 ```

Navigate to `chrome://extensions`, set `Developer mode` to on, click `Load unpacked`, select downloaded git directory.

Note the generated extension ID, substitute that value for `<id>` in `nm_epseakng.json` and `AudioStream.js`.

Substitute full local path to `nm_espeakng.sh` for `/path/to` in `nm_espeakng.json`.
  
```
"allowed_origins": [ "chrome-extension://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx/" ]
```

Copy `nm_espeakng.json` to `NativeMessagingHosts` directory in Chromium or Chrome configuration folder, on Linux, i.e., `~/.config/chromium`; `~/.config/google-chrome-unstable`.

`cp nm_espeakng.json ~/.config/chromium/NativeMessagingHosts`

Reload extension.

<h5>Usage</h5>

On origins listed in `"matches"` array in [`"web_accessible_resources"`](https://developer.chrome.com/docs/extensions/mv3/manifest/web_accessible_resources/) and `"externally_connectable"` object in `manifest.json`, e.g., at `console`

```
let text = `So we need people to have weird new
ideas ... we need more ideas to break it
and make it better ...

Use it. Break it. File bugs. Request features.

- Soledad Penadés, Real time front-end alchemy, or: capturing, playing,
  altering and encoding video and audio streams, without
  servers or plugins!
<br>  
von Braun believed in testing. I cannot
emphasize that term enough – test, test,
test. Test to the point it breaks.

- Ed Buckbee, NASA Public Affairs Officer, Chasing the Moon
<br>
Now watch. ..., this how science works.
One researcher comes up with a result.
And that is not the truth. No, no.
A scientific emergent truth is not the
result of one experiment. What has to
happen is somebody else has to verify
it. Preferably a competitor. Preferably
someone who doesn't want you to be correct.

- Neil deGrasse Tyson, May 3, 2017 at 92nd Street Y
<br>
It’s like they say, if the system fails you, you create your own system.

- Michael K. Williams, Black Market
<br>
1. If a (logical or axiomatic formal) system is consistent, it cannot be complete.
2. The consistency of axioms cannot be proved within their own system.

- Kurt Gödel, Incompleteness Theorem, On Formally Undecidable Propositions of Principia Mathematica and Related Systems`;
let {AudioStream} = await import('chrome-extension://<id>/AudioStream.js')
let audioStream = new AudioStream({stdin: `espeak-ng -m --stdout "${text}"`});
await audioStream.start();
```
  
To record `MediaStreamTrack` pass `recorder: true` (set to `false` by default) to second parameter
  
```
let audioStream = var audioStream = new AudioStream({
   stdin: `espeak-ng -m --stdout '<voice name="Storm">Hello world.<br></voice>'`, 
   recorder: true
});
let ab = await audioStream.start(); // ArrayBuffer
let blobURL = URL.createObjectURL(new Blob([ab], {type: 'audio/wav'}));
```
  
Abort the request and audio output.
  
```
await audioStream.abort();
```
  
<h5>References</h5>

- [Include test for setting an SSML document at SpeechSynthesisUtterance .text property within speech-api](https://github.com/web-platform-tests/wpt/issues/8712)
- [This is again recording from microphone, not from audiooutput device](https://github.com/guest271314/SpeechSynthesisRecorder/issues/14)
- [Support SpeechSynthesis *to* a MediaStreamTrack](https://github.com/WICG/speech-api/issues/69)
- [Clarify getUserMedia({audio:{deviceId:{exact:<audiooutput_device>}}}) in this specification mandates capability to capture of audio output device - not exclusively microphone input device](https://github.com/w3c/mediacapture-main/issues/650)
- [How to modify existing code or build with -m option set for default SSML parsing?](https://github.com/pettarin/espeakng.js-cdn/issues/1)
- [Issue 795371: Implement SSML parsing at SpeechSynthesisUtterance](https://bugs.chromium.org/p/chromium/issues/detail?id=795371)
- [Implement SSML parsing at SpeechSynthesisUtterance](https://bugzilla.mozilla.org/show_bug.cgi?id=1425523)
- [How is a complete SSML document expected to be parsed when set once at .text property of SpeechSynthesisUtterance instance?](https://github.com/WICG/speech-api/issues/10)
- [How to programmatically send a unix socket command to a system server autospawned by browser or convert JavaScript to C++ souce code for Chromium?](https://stackoverflow.com/questions/48219981/how-to-programmatically-send-a-unix-socket-command-to-a-system-server-autospawne)
- [<script type="shell"> to execute arbitrary shell commands, and import stdout or result written to local file as a JavaScript module](https://github.com/whatwg/html/issues/3443)
- [Add execute() to FileSystemDirectoryHandle](https://github.com/WICG/native-file-system/issues/97)
- [Issue 795371: Implement SSML parsing at SpeechSynthesisUtterance](https://bugs.chromium.org/p/chromium/issues/detail?id=795371)
- [Implement SSML parsing at SpeechSynthesisUtterance](https://bugzilla.mozilla.org/show_bug.cgi?id=1425523)
- [How is a complete SSML document expected to be parsed when set once at .text property of SpeechSynthesisUtterance instance?](https://github.com/WICG/speech-api/issues/10)
- [How to programmatically send a unix socket command to a system server autospawned by browser or convert JavaScript to C++ souce code for Chromium?](https://stackoverflow.com/questions/48219981/how-to-programmatically-send-a-unix-socket-command-to-a-system-server-autospawne)
- [<script type="shell"> to execute arbitrary shell commands, and import stdout or result written to local file as a JavaScript module](https://github.com/whatwg/html/issues/3443)
- [Add execute() to FileSystemDirectoryHandle](https://github.com/WICG/native-file-system/issues/97)
- [SpeechSynthesis *to* a MediaStreamTrack or: How to execute arbitrary shell commands using inotify-tools and DevTools Snippets](https://gist.github.com/guest271314/59406ad47a622d19b26f8a8c1e1bdfd5)
