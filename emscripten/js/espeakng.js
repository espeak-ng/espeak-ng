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

function eSpeakNG(worker_path, ready_cb) {
  this.worker = new Worker(worker_path);
  this.ready = false;
  this.worker.onmessage = function(e) {
    if (e.data !== 'ready') {
      return;
    }
    this.worker.onmessage = null;
    this.worker.addEventListener('message', this);
    this.ready = true;
    if (ready_cb) {
      ready_cb();
    }
  }.bind(this);
}

eSpeakNG.prototype.handleEvent = function (evt) {
  var callback = evt.data.callback;
  if (callback && this[callback]) {
    this[callback].apply(this, evt.data.result);
    if (evt.data.done) {
      delete this[callback];
    }
    return;
  }
};

function _createAsyncMethod(method) {
  return function() {
    var lastArg = arguments[arguments.length - 1];
    var message = { method: method, args: Array.prototype.slice.call(arguments, 0) };
    if (typeof lastArg == 'function') {
      var callback = '_' + method + '_' + Math.random().toString().substring(2) +'_cb';
      this[callback] = lastArg;
      message.args.pop();
      message.callback = callback;
    }
    this.worker.postMessage(message);
  };
}

for (var method of [
    'list_voices',
    'get_rate',
    'get_pitch',
    'set_rate',
    'set_pitch',
    'set_voice',
    'synthesize',
    'synthesize_ipa'
    ]) {
  eSpeakNG.prototype[method] = _createAsyncMethod(method);
}
