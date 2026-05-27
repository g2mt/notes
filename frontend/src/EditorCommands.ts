import {baseKeymap} from 'prosemirror-commands';
import {dropCursor} from 'prosemirror-dropcursor';
import {gapCursor} from 'prosemirror-gapcursor';
import {history} from 'prosemirror-history';
import {keymap} from 'prosemirror-keymap';
import {defaultMarkdownParser, defaultMarkdownSerializer, MarkdownParser, schema} from 'prosemirror-markdown';
import {EditorState} from 'prosemirror-state';
import {EditorView} from 'prosemirror-view';

export class EditorCommands {
  private view: EditorView;
  private parser: MarkdownParser;

  constructor(element: HTMLElement) {
    this.parser = new MarkdownParser(
        schema, defaultMarkdownParser.tokenizer, defaultMarkdownParser.tokens);

    const state = EditorState.create({
      schema,
      plugins: [
        keymap(baseKeymap),
        history(),
        dropCursor(),
        gapCursor(),
      ],
    });

    this.view = new EditorView(element, {state});
  }

  setMarkdown(md: string) {
    const doc = this.parser.parse(md);
    this.view.dispatch(
        this.view.state.tr.replaceWith(0, this.view.state.doc.content.size, doc));
  }

  getMarkdown(): string {
    return defaultMarkdownSerializer.serialize(this.view.state.doc);
  }

  toggleBold() {}
  toggleItalic() {}
  toggleUnderline() {}
  toggleStrikethrough() {}
  toggleSuperscript() {}
  toggleSubscript() {}
  wrapHeading(_level: number) {}
  clearHeading() {}
  insertOrderedList() {}
  insertUnorderedList() {}
  insertTable(_rows: number, _cols: number) {}
  insertPlainText(_text: string) {}
  undo() {}
  redo() {}
  isEmpty(): boolean {
    return false;
  }
}
