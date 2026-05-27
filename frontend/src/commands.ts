import {setBlockType, toggleMark, wrapIn} from 'prosemirror-commands';
import {redo, undo} from 'prosemirror-history';
import {defaultMarkdownParser} from 'prosemirror-markdown';
import {wrapInList} from 'prosemirror-schema-list';
import {EditorView} from 'prosemirror-view';

import {schema} from './schema';
import {markdownSerializer} from './serializer';

export function buildCommands(view: () => EditorView) {
  return {
    toggleBold() {
      toggleMark(schema.marks.strong)(view().state, view().dispatch);
      view().focus();
    },
    toggleItalic() {
      toggleMark(schema.marks.em)(view().state, view().dispatch);
      view().focus();
    },
    toggleUnderline() {
      toggleMark(schema.marks.underline)(view().state, view().dispatch);
      view().focus();
    },
    toggleStrikethrough() {
      toggleMark(schema.marks.strikethrough)(view().state, view().dispatch);
      view().focus();
    },
    toggleSuperscript() {
      toggleMark(schema.marks.superscript)(view().state, view().dispatch);
      view().focus();
    },
    toggleSubscript() {
      toggleMark(schema.marks.subscript)(view().state, view().dispatch);
      view().focus();
    },

    setMarkdown(md: string) {
      const doc = defaultMarkdownParser.parse(md);
      if (doc) {
        const v = view();
        v.dispatch(
            v.state.tr.replaceWith(0, v.state.doc.content.size, doc.content));
      }
    },
    getMarkdown(): string {
      return markdownSerializer.serialize(view().state.doc);
    },

    wrapHeading(level: number) {
      const v = view();
      const {$from} = v.state.selection;
      const nodeType = schema.nodes.heading;
      if (nodeType) {
        setBlockType(nodeType, {level})(v.state, v.dispatch);
      }
      v.focus();
    },
    clearHeading() {
      const v = view();
      setBlockType(schema.nodes.paragraph)(v.state, v.dispatch);
      v.focus();
    },

    insertOrderedList() {
      const v = view();
      wrapInList(schema.nodes.ordered_list)(v.state, v.dispatch);
      v.focus();
    },
    insertUnorderedList() {
      const v = view();
      wrapInList(schema.nodes.bullet_list)(v.state, v.dispatch);
      v.focus();
    },

    insertTable(rows: number, cols: number) {
      const v = view();
      const {state} = v;
      const cell = schema.nodes.table_cell;
      const header = schema.nodes.table_header;
      const row = schema.nodes.table_row;
      const table = schema.nodes.table;

      if (!cell || !row || !table) return;

      const headerCells = [];
      for (let c = 0; c < cols; c++) {
        headerCells.push((header || cell).createAndFill()!);
      }

      const rows_: any[] = [row.create(null, headerCells)];

      for (let r = 1; r < rows; r++) {
        const cells = [];
        for (let c = 0; c < cols; c++) {
          cells.push(cell.createAndFill()!);
        }
        rows_.push(row.create(null, cells));
      }

      const tableNode = table.create(null, rows_);
      const tr = state.tr.replaceSelectionWith(tableNode);
      if (tr) {
        v.dispatch(tr);
        v.focus();
      }
    },

    undo() {
      undo(view().state, view().dispatch);
      view().focus();
    },
    redo() {
      redo(view().state, view().dispatch);
      view().focus();
    },

    isEmpty(): boolean {
      const doc = view().state.doc;
      return doc.childCount === 1 &&
          doc.firstChild?.type.name === 'paragraph' &&
          doc.firstChild.content.size === 0;
    },

    isUndoAvailable(): boolean {
      return undo(view().state);
    },
    isRedoAvailable(): boolean {
      return redo(view().state);
    },

    getActiveMarks(): {
      bold: boolean; italic: boolean; underline: boolean;
      strikethrough: boolean;
      superscript: boolean;
      subscript: boolean
    } {
      const marks = view().state.selection.$from.marks();
      const has = (name: string) => marks.some(m => m.type.name === name);
      return {
        bold: has('strong'),
        italic: has('em'),
        underline: has('underline'),
        strikethrough: has('strikethrough'),
        superscript: has('superscript'),
        subscript: has('subscript'),
      };
    },
  };
}
