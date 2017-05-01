# espeakng.js

This port of
[eSpeak-ng](https://github.com/espeak-ng/espeak-ng)
to Javascript via
[emscripten](http://emscripten.org)
allows client-side text-to-speech synthesis in any browser
supporting Web workers and the Web Audio API.

* Version: 1.49.1
* Date: 2017-05-01
* License: the GNU General Public License, Version 3 (GPLv3)
* Size: 3.2 MB (including all the voices)


## Demo

For an online demo, visit [this page](https://www.readbeyond.it/espeakng/).

If you prefer a local demo, open the file [`demo.html`](demo.html) in your browser.
(Depending on your browser, you might need to serve it via a Web server.
A simple way consists in calling `$ python -m SimpleHTTPServer 8000`
and pointing your browser to `http://localhost:8000/demo.html`)


## Usage

1. Include `js/espeakng.js` in your HTML file:

    ```html
    <script type="text/javascript" src="js/espeakng.js"></script>
    ```

2. Create a new `eSpeakNG` instance with the path of the worker
   (e.g., `js/espeakng.worker.js`) as the first argument.
   You can supply a callback function that will be called once the worker is ready:

    ```js
    <script>
      var tts = new eSpeakNG('js/espeakng.worker.js', function(){
        // now tts is initialized and
        // you can call tts.function(...)
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

    See the file [`demo.html`](demo.html) for details.


## Download

You can download pre-built JS/data files from the
[jsdelivr CDN](http://www.jsdelivr.com/).
You need to get the following files:

```
# Latest version
https://cdn.jsdelivr.net/espeakng.js/latest/espeakng.min.js
https://cdn.jsdelivr.net/espeakng.js/latest/espeakng.worker.js
https://cdn.jsdelivr.net/espeakng.js/latest/espeakng.worker.data

# Specific version
https://cdn.jsdelivr.net/espeakng.js/1.49.0/espeakng.min.js
https://cdn.jsdelivr.net/espeakng.js/1.49.0/espeakng.worker.js
https://cdn.jsdelivr.net/espeakng.js/1.49.0/espeakng.worker.data
```

For details, see the
[espeakng.js-cdn](https://github.com/pettarin/espeakng.js-cdn)
repository.


## Building

1. Make sure you have `emscripten`
   [installed and activated](http://kripken.github.io/emscripten-site/docs/getting_started/downloads.html),
   and configured in your shell.

2. Clone the repository, enter the `espeak-ng` directory, and initialize it:

    ```bash
    $ git clone https://github.com/espeak-ng/espeak-ng
    $ cd espeak-ng
    $ ./autogen.sh
    ```

3. Compile `espeak-ng`:

    ```bash
    $ ./configure --prefix=/usr --without-async --without-mbrola --without-sonic
    $ make
    ```

    Instead, if you just want a subset of languages of your choice,
    run `make XX` for each of them, where `XX` is the language code.
    For example:

    ```bash
    $ # build English only
    $ make en
    $
    $ # build English, Italian, and German
    $ make en && make it && make de
    ```

4. Enter the `src/ucd-tools` directory, `make clean` it, and compile with `emconfigure` and `emmake`:

    ```bash
    $ cd src/ucd-tools
    $ ./autogen.sh
    $ make clean
    $ emconfigure ./configure
    $ emmake make
    $ cd ../..
    ```

5. Recompile the `espeak-ng` library with `emconfigure` and `emmake`:

    ```bash
    $ emconfigure ./configure --prefix=/usr --without-async --without-mbrola --without-sonic
    $ emmake make src/libespeak-ng.la
    ```

6. Enter the `emscripten` directory and compile the JS worker with `emmake`:

    ```bash
    $ cd emscripten
    $ emmake make
    ```

7. The `js/` directory should contain the output JS files:

    * `espeakng.js`,
    * `espeakng.worker.js`, and
    * `espeakng.worker.data`.

### Notes

* Even if you build only a subset of languages,
  the `list_voices()` function will still list all the available voices.
  However, if you try using language that has not been built,
  you will get a runtime error.
  (TODO: fix this issue.)
* The `-O3` optimization flag is passed to `emscripten` by default,
  achieving a considerable reduction of the size of the generated JS worker.
  If you want to omit it, for example for debug purposes,
  just export the `EM_DEBUG` environment variable with value `1`
  (i.e., `$ export EM_DEBUG=1`) before step 3.
* To remove intermediate files compiled by emscripten,
  run `$ emmake clean-intermediate`.
  This command will keep the output JS files, if any.
* To remove all the files compiled by emscripten,
  including the output JS files,
  run `$ emmake clean`.


## Credits

[Eitan Isaacson](https://blog.monotonous.org/) wrote the [original glue code](https://github.com/eeejay/espeak/tree/emscripten) for compiling eSpeak to Javascript via emscripten.

[Alberto Pettarin](http://www.albertopettarin.it) adapted Eitan's files [to work with eSpeak-ng](https://github.com/pettarin/espeak-ng/tree/emscripten) and wrote this `README` file.
