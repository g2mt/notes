import {EditorCommands} from './editorCommands';
import {initBridge} from './bridge';

declare global {
  interface Window {
    editorCommands: EditorCommands;
  }
}

export function createEditor(element: HTMLElement) {
  window.editorCommands = new EditorCommands(element);

  initBridge().then(bridge => {
    bridge.markdownChanged.connect((md: string) => {
      window.editorCommands.setMarkdown(md);
    });
    bridge.insertPlainText.connect((text: string) => {
      window.editorCommands.insertPlainText(text);
    });
    bridge.emitLoaded();
  });
}
