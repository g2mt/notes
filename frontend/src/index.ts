import {baseKeymap} from 'prosemirror-commands';
import {dropCursor} from 'prosemirror-dropcursor';
import {gapCursor} from 'prosemirror-gapcursor';
import {history} from 'prosemirror-history';
import {keymap} from 'prosemirror-keymap';
import {defaultMarkdownParser, defaultMarkdownSerializer, MarkdownParser, schema,} from 'prosemirror-markdown';
import {EditorState} from 'prosemirror-state';
import {EditorView} from 'prosemirror-view';

import {initBridge} from './bridge';

declare global {
  interface Window {
    proseCommands: any;
  }
}

const parser = new MarkdownParser(
    schema, defaultMarkdownParser.tokenizer, defaultMarkdownParser.tokens);

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

  window.proseCommands = {
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
    isEmpty(): boolean {
      return false;
    },
    insertPlainText(_text: string) {},
  };

  initBridge().then(bridge => {
    bridge.markdownChanged.connect((md: string) => {
      window.proseCommands.setMarkdown(md);
    });
    bridge.insertPlainText.connect((text: string) => {
      window.proseCommands.insertPlainText(text);
    });
    bridge.emitLoaded();
  });
}
