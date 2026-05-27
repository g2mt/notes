import {baseKeymap, setBlockType, toggleMark, wrapIn} from 'prosemirror-commands';
import {dropCursor} from 'prosemirror-dropcursor';
import {gapCursor} from 'prosemirror-gapcursor';
import {history, redo, undo} from 'prosemirror-history';
import {keymap} from 'prosemirror-keymap';
import {defaultMarkdownParser, defaultMarkdownSerializer, MarkdownParser, schema} from 'prosemirror-markdown';
import {MarkType} from 'prosemirror-model';
import {EditorState} from 'prosemirror-state';
import {EditorView} from 'prosemirror-view';

import {bridge, initBridge, type NativeBridge} from './bridge';

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

    this.view = new EditorView(element, {
      state,
      dispatchTransaction: (tr) => {
        const newState = this.view.state.apply(tr);
        this.view.updateState(newState);
        this.emitFormattingChanged();
      },
    });

    initBridge().then(bridge => {
      bridge.markdownChanged.connect((md: string) => {
        this.setMarkdown(md);
      });
      bridge.insertPlainText.connect((text: string) => {
        this.insertPlainText(text);
      });
      bridge.emitLoaded();
    });
  }

  private isMarkActive(markType: MarkType): boolean {
    const {from, $from, to, empty} = this.view.state.selection;
    if (empty) {
      return !!markType.isInSet(this.view.state.storedMarks || $from.marks());
    }
    return this.view.state.doc.rangeHasMark(from, to, markType);
  }

  private emitFormattingChanged() {
    bridge().emitFormattingChanged(
        this.isMarkActive(schema.marks.strong),
        this.isMarkActive(schema.marks.em), false, false, false, false);
  }

  setMarkdown(md: string) {
    const doc = this.parser.parse(md);
    this.view.dispatch(this.view.state.tr.replaceWith(
        0, this.view.state.doc.content.size, doc));
  }

  getMarkdown(): string {
    return defaultMarkdownSerializer.serialize(this.view.state.doc);
  }

  toggleBold() {
    toggleMark(schema.marks.strong)(this.view.state, this.view.dispatch);
  }

  toggleItalic() {
    toggleMark(schema.marks.em)(this.view.state, this.view.dispatch);
  }

  toggleUnderline() {}
  toggleStrikethrough() {}
  toggleSuperscript() {}
  toggleSubscript() {}

  wrapHeading(level: number) {
    setBlockType(schema.nodes.heading, {level})(
        this.view.state, this.view.dispatch);
  }

  clearHeading() {
    setBlockType(schema.nodes.paragraph)(this.view.state, this.view.dispatch);
  }

  insertOrderedList() {
    wrapIn(schema.nodes.ordered_list)(this.view.state, this.view.dispatch);
  }

  insertUnorderedList() {
    wrapIn(schema.nodes.bullet_list)(this.view.state, this.view.dispatch);
  }

  insertTable(_rows: number, _cols: number) {}

  insertPlainText(text: string) {
    this.view.dispatch(this.view.state.tr.insertText(text));
  }

  undo() {
    undo(this.view.state, this.view.dispatch);
  }

  redo() {
    redo(this.view.state, this.view.dispatch);
  }

  isEmpty(): boolean {
    return this.view.state.doc.textContent.trim().length === 0;
  }
}
