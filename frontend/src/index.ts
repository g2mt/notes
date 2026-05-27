import { EditorState } from "prosemirror-state";
import { EditorView } from "prosemirror-view";
import { keymap } from "prosemirror-keymap";
import { baseKeymap } from "prosemirror-commands";
import { history } from "prosemirror-history";
import { dropCursor } from "prosemirror-dropcursor";
import { gapCursor } from "prosemirror-gapcursor";
import { tableEditing, columnResizing } from "prosemirror-tables";
import { schema } from "./schema";
import { buildCommands } from "./commands";
import { initBridge, getBridge } from "./bridge";

let view: EditorView;

function notifyState() {
  const b = getBridge();
  const cmds = (window as any).proseCommands;
  if (!b || !cmds) return;

  const marks = cmds.getActiveMarks();
  b.notifyFormattingChanged(
    marks.bold,
    marks.italic,
    marks.underline,
    marks.strikethrough,
    marks.superscript,
    marks.subscript
  );
  b.notifyModificationChanged(true);
  b.notifyUndoAvailable(cmds.isUndoAvailable());
  b.notifyRedoAvailable(cmds.isRedoAvailable());
}

export function createEditor(element: HTMLElement) {
  const state = EditorState.create({
    schema,
    plugins: [
      keymap(baseKeymap),
      history(),
      dropCursor(),
      gapCursor(),
      columnResizing(),
      tableEditing(),
    ],
  });

  view = new EditorView(element, {
    state,
    dispatchTransaction(tr) {
      const newState = view.state.apply(tr);
      view.updateState(newState);
      notifyState();
    },
  });

  (window as any).proseCommands = buildCommands(() => view);

  initBridge().then(() => {
    notifyState();
  });
}
