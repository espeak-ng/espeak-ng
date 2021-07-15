chrome.action.onClicked.addListener(() => 
  chrome.runtime.sendNativeMessage('native_messaging_espeakng'
  , {}, (nativeMessage) => console.log({nativeMessage}))
);
