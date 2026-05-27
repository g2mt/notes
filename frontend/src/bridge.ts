interface Signal<T> {
  connect(callback: (arg: T) => void): void;
}

interface NativeBridge {
  emitFormattingChanged(
      bold: boolean, italic: boolean, underline: boolean,
      strikethrough: boolean, superscript: boolean, subscript: boolean): void;
  emitModificationChanged(modified: boolean): void;
  emitUndoAvailable(available: boolean): void;
  emitRedoAvailable(available: boolean): void;
  emitLoaded(): void;

  markdownChanged: Signal<string>;
  insertPlainText: Signal<string>;
}

declare global {
  interface Window {
    qt: any;
    QWebChannel: any;
  }
}

let _bridge: NativeBridge|null = null;

export function bridge(): NativeBridge {
  if (!_bridge) throw new Error('_bridge is null');
  return _bridge;
}

export function initBridge(): Promise<NativeBridge> {
  return new Promise((resolve) => {
    new window.QWebChannel(window.qt.webChannelTransport, (channel: any) => {
      _bridge = channel.objects.bridge as NativeBridge;
      resolve(_bridge);
    });
  });
}
