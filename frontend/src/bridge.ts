interface CppBridge {
  notifyFormattingChanged(
    bold: boolean,
    italic: boolean,
    underline: boolean,
    strikethrough: boolean,
    superscript: boolean,
    subscript: boolean
  ): void;
  notifyModificationChanged(modified: boolean): void;
  notifyUndoAvailable(available: boolean): void;
  notifyRedoAvailable(available: boolean): void;
}

let bridge: CppBridge | null = null;

export function initBridge(): Promise<CppBridge> {
  return new Promise((resolve) => {
    new (window as any).QWebChannel(
      (window as any).qt.webChannelTransport,
      (channel: any) => {
        bridge = channel.objects.bridge as CppBridge;
        resolve(bridge);
      }
    );
  });
}

export function getBridge(): CppBridge | null {
  return bridge;
}
