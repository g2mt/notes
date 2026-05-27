import {initBridge} from './bridge';
import {EditorCommands} from './EditorCommands';

declare global {
  interface Window {
    editorCommands: EditorCommands;
  }
}

export function createEditor(element: HTMLElement) {
  window.editorCommands = new EditorCommands(element);
}
