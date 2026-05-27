import {keymap} from 'prosemirror-keymap';
import {baseKeymap} from 'prosemirror-commands';
import {history} from 'prosemirror-history';
import {dropCursor} from 'prosemirror-dropcursor';
import {gapCursor} from 'prosemirror-gapcursor';
import {EditorState} from 'prosemirror-state';
import {EditorView} from 'prosemirror-view';
import {
  schema,
  defaultMarkdownParser,
  defaultMarkdownSerializer,
  MarkdownParser,
} from 'prosemirror-markdown';

import {initBridge} from './bridge';

const parser = new MarkdownParser(schema, defaultMarkdownParser.tokenizer, defaultMarkdownParser.tokens);

let view: EditorView;

export function createEditor(element: HTMLElement) {
  const state = EditorState.create({
    schema,
    plugins: [
      keymap(baseKeymap),
      history(),
      dropCursor(),
      gapCursor(),
    ],
  });

  view = new EditorView(element, {state});

  (window as any).proseCommands = {
    setMarkdown(md: string) {
      const doc = parser.parse(md);
      view.dispatch(
        view.state.tr.replaceWith(0, view.state.doc.content.size, doc));
    },
    getMarkdown(): string {
      return defaultMarkdownSerializer.serialize(view.state.doc);
    },

    toggleBold() {},
    toggleItalic() {},
    toggleUnderline() {},
    toggleStrikethrough() {},
    toggleSuperscript() {},
    toggleSubscript() {},
    wrapHeading(_level: number) {},
    clearHeading() {},
    insertOrderedList() {},
    insertUnorderedList() {},
    insertTable(_rows: number, _cols: number) {},
    undo() {},
    redo() {},
    isEmpty(): boolean { return false; },
    isUndoAvailable(): boolean { return false; },
    isRedoAvailable(): boolean { return false; },
    getActiveMarks(): Record<string, boolean> {
      return {bold: false, italic: false, underline: false, strikethrough: false, superscript: false, subscript: false};
    },
  };

  initBridge().then(() => {});
}
