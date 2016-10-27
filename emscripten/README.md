# espeakng.js

This port of
[eSpeak-ng](https://github.com/espeak-ng/espeak-ng)
to Javascript via
[emscripten](http://emscripten.org)
allows client-side text-to-speech synthesis in any browser
supporting Web workers and the Web Audio API.

* Version: 1.49.0
* Date: 2016-10-27
* License: the GNU General Public License, Version 3 (GPLv3)
* Size: 3.0 MB (including all the voices)


## Demo

For an online demo, visit [this page](https://www.readbeyond.it/espeakng/).

If you prefer a local demo, open the file `demo.html` in your browser.
(Depending on your browser, you might need to serve it via a Web server.
A simple way consists in calling `$ python -m SimpleHTTPServer 8000`
and pointing your browser to `http://localhost:8000/demo.html`)


## Usage

1. Include `js/espeakng.js` in your HTML file:
    
    ```html
    <script type="text/javascript" src="js/espeakng.js"></script>
    ```

2. Create a new `eSpeakNG` instance with the path of the worker (e.g., `espeakng.worker.js`) as the first argument.
   You can supply a callback function that will be called once the worker is ready:

    ```js
    <script>
      var espeakng = new eSpeakNG('js/espeakng.worker.js', function(){
        // now you can use espeakng
        ...
      });
    </script>
    ```

3. Functions exposed by the `eSpeakNG` object:

    * `list_voices(cb(voices))`: obtain the list of available voices, passing it (`voices`) to the callback function `cb`
    * `set_rate(value)`: set the rate of the synthesized speech, `value` must be an integer in `[80..450]`, default `175`
    * `set_pitch(value)`: set the pitch of the synthesized speech, `value` must be an integer in `[0..99]`, default `50`
    * `set_voice.apply(espeakng, voice)`: set the voice for synthesizing speech, `voice` must be the code string for the desired voice (e.g., `en`, `en-us`, `it`, etc.)
    * `synthesize(text, cb(samples, events))`: synthesize the given string `text` and call the callback function `cb` when done, passing the generated audio samples (`samples`) and events (`events`)

    See the file `demo.html` for details.


## Download

You can download pre-built JS files from the [Releases page](https://github.com/pettarin/espeak-ng/releases).

TODO: add the compiled JS files to a public free CDN, and list the URL here.


## Building

1. Make sure you have `emscripten` [installed and activated](http://kripken.github.io/emscripten-site/docs/getting_started/downloads.html).

2. Clone the repo and enter the `espeak-ng` directory:

    ```bash
    $ git clone https://github.com/espeak-ng/espeak-ng
    $ cd espeak-ng
    ```

3. Compile `espeak-ng`:

    ```bash
    $ ./autogen.sh
    $ ./configure --prefix=/usr --without-async --without-mbrola --without-sonic
    $ make
    ```

4. Recompile `espeak-ng` with `emconfigure` and `emmake`: 

    ```bash
    $ emconfigure ./configure --prefix=/usr --without-async --without-mbrola --without-sonic
    $ emmake make
    ```

    Note: the `emmake make` command currently exits
    with the error `recipe for target 'phsource/phonemes.stamp' failed`,
    but you can safely ignore it.
    (TODO: fix this issue.)

5. Enter the `emscripten` directory and compile the JS worker with `emmake`:

    ```bash
    $ cd emscripten
    $ emmake make
    ```

6. The `js/` directory should contain the output JS files:
    
    * `espeakng.js`,
    * `espeakng.worker.js`, and
    * `espeakng.worker.data`.

### Notes

* If you want to build only a single voice, for example the English one,
  run `$ make en` instead of `$ make` at step 3.
  You can also build a subset of all the available voices,
  by running (say) `$ make en && make it && make fr && make es`.
  Note that `list_voices()` will still list all available voices,
  but if you try using one that has not been compiled,
  you will get a runtime error.
  (TODO: fix this issue.)
* The `-O3` optimization flag is passed to `emscripten` by default,
  achieving a considerable reduction of the size of the generated JS worker.
  If you want to omit it, for example for debug purposes,
  just `$ export EM_DEBUG=1` before calling `emmake make` in step 5.
* To remove intermediate files compiled by emscripten,
  keeping only the JS worker files, run `$ emmake clean-intermediate`.
* To remove all the files compiled by emscripten, run `$ emmake clean`.


## Credits

[Eitan Isaacson](https://blog.monotonous.org/) wrote the [original glue code](https://github.com/eeejay/espeak/tree/emscripten) for compiling eSpeak to Javascript via emscripten.

[Alberto Pettarin](http://www.albertopettarin.it) adapted Eitan's files [to work with eSpeak-ng](https://github.com/pettarin/espeak-ng/tree/emscripten) and wrote this `README` file.
