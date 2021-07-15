onload = async () => {
  chrome.runtime.sendNativeMessage(
    'native_messaging_espeakng',
    {},
    async (nativeMessage) => {
      parent.postMessage(nativeMessage, name);
      await new Promise((resolve) => setTimeout(resolve, 100));
      const controller = new AbortController();
      const { signal } = controller;
      parent.postMessage('Ready.', name);
      onmessage = async (e) => {
        if (e.data instanceof ReadableStream) {
          try {
            const { value: file, done } = await e.data.getReader().read();
            const fd = new FormData();
            const stdin = await file.text();
            fd.append(file.name, stdin);
            const { body } = await fetch('http://localhost:8000', {
              method: 'post',
              cache: 'no-store',
              credentials: 'omit',
              body: fd,
              signal,
            });
            parent.postMessage(body, name, [body]);
          } catch (err) {
            parent.postMessage(err, name);
          }
        } else {
          if (e.data === 'Done writing input stream.') {
            chrome.runtime.sendNativeMessage(
              'native_messaging_espeakng',
              {},
              (nativeMessage) => {
                parent.postMessage(nativeMessage, name);
              }
            );
          }
          if (e.data === 'Abort.') {
            controller.abort();
          }
        }
      };
    }
  );
};
