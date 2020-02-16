/*
 * Copyright (C) 2014-2017 Eitan Isaacson
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see: <http://www.gnu.org/licenses/>.
 */

eSpeakNGWorker.prototype.list_voices = function() {
  var voices = [];
  var i;
  for (var voice = this.get_voices(i = 0); voice.ptr != 0; voice = this.get_voices(++i)) {
    var v = {
      name: voice.get_name(),
      identifier: voice.get_identifier(),
      languages: [],
    }

    var ii = 0;
    var byte = voice.get_languages(ii);

    function nullTerminatedString(offset) {
      var str = '';
      var index = offset;
      var b = voice.get_languages(index++);
      while (b != 0) {
        str += String.fromCharCode(b);
        b = voice.get_languages(index++);
      }

      return str;
    }

    while (byte != 0) {
      var lang = { priority: byte, name: nullTerminatedString(++ii) }
      v.languages.push(lang);
      ii += lang.name.length + 1;
      byte = voice.get_languages(ii);
    }

    voices.push(v);

  }
  return voices;
};

var eventTypes = [
  'list_terminated',
  'word',
  'sentence',
  'mark',
  'play',
  'end',
  'msg_terminated',
  'phoneme',
  'samplerate'
]

eSpeakNGWorker.prototype.synthesize = function (aText, aCallback) {
  var eventStructSize = this.getSizeOfEventStruct_();
  function cb(ptr, length, events_pointer) {
    var data = new Float32Array(length*2);
    for (var i = 0; i < length; i++) {
      data[i*2] = Math.max(-1, Math.min(1, getValue(ptr + i*2, 'i16') / 32768));
      data[i*2+1] = data[i*2];
    }
    var events = [];
    var ptr = events_pointer;
    for (ev = wrapPointer(ptr, espeak_EVENT);
         ev.get_type() != Module.espeakEVENT_LIST_TERMINATED;
         ev = wrapPointer((ptr += eventStructSize), espeak_EVENT)) {
      events.push({
        type: eventTypes[ev.get_type()],
        text_position: ev.get_text_position(),
        word_length: ev.get_length(),
        audio_position: ev.get_audio_position()
      });
    }
    return aCallback(data, events) ? 1 : 0;
  }

  var fp = addFunction(cb);
  this.synth_(aText, fp);
  removeFunction(fp);
};

eSpeakNGWorker.prototype.synthesize_ipa = function (aText, aCallback) {
  
  // Use a unique temp file for the worker. Avoid collisions, just in case.
  var ipaVirtualFileName = "espeak-ng-ipa-tmp-"  + Math.random().toString().substring(2);
  
  var res = "";
  var code = this.synth_ipa_(aText, ipaVirtualFileName);

  if(code == 0)
    res = FS.readFile(ipaVirtualFileName, { encoding: 'utf8' })
    
  // Clean up the tmp file
  FS.unlink(ipaVirtualFileName);
    
  var ret = {
    code: code,
    ipa: res
  }
  
  return ret;
};

// Make this a worker

if (typeof WorkerGlobalScope !== 'undefined') {
  var worker;
  
  Module.postRun = Module.postRun || [];

  Module.postRun.push(function () {
    worker = new eSpeakNGWorker();
    postMessage('ready');
  });

  onmessage = function(e) {
    
    if (!worker) {
      throw 'eSpeakNGWorker worker not initialized';
    }
    var args = e.data.args;
    var message = { callback: e.data.callback, done: true };
    if (e.data.method == 'synthesize') {
      args.push(function(samples, events) {
        postMessage(
          { callback: e.data.callback,
            result: [samples.buffer, events] }, [samples.buffer]);
      });
    }
    message.result = [worker[e.data.method].apply(worker, args)];
    if (e.data.callback)
      postMessage(message);
  }
}
