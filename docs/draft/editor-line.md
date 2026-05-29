# EditorLine

> **Warning:** This file was generated with AI assistance.

Introduce an `EditorLine` widget that represents a single visual line of
elements within the editor. Lines are ephemeral — destroyed and recreated on
every `relayout()` — and are an internal implementation detail of
`EditorBlock`. They are never exposed through public API.

## Current State

- `EditorElement` is the base widget for all content (`src/include/notes/EditorElement.h`).
- `EditorBlock` extends `EditorElement` and holds a `QList<EditorElement *> m_elements`
  containing both child blocks and inline fragments (`src/include/notes/blocks/EditorBlock.h:33`).
- `EditorDocument` extends `EditorBlock` and is the root widget
  (`src/include/notes/EditorDocument.h:42`).
- All layout happens in `EditorBlock::relayout()` (`src/blocks/EditorBlock.cpp:81`).
  It manually positions children with `setGeometry()` using a local `pending`
  list and `flushLine()` lambda for line-based word wrapping.
- `EditorFragmentSub` widgets represent visual sub-rectangles of a
  wrapped `EditorTextFragment` across multiple lines.
- No explicit line abstraction exists. Lines are purely algorithmic.

## Design

### `EditorLine` class (ephemeral, internal)

`EditorLine` is a simple `QWidget` with a `QHBoxLayout` (alignLeft) for its
inline children. It is NOT an `EditorElement` — it is a layout helper, never
selectable, never exposed in public API.

```cpp
// src/include/notes/EditorLine.h
#ifndef EDITOR_LINE_H
#define EDITOR_LINE_H

#include <QWidget>

class QHBoxLayout;
class EditorElement;

class EditorLine : public QWidget {
  Q_OBJECT

public:
  explicit EditorLine(QWidget *parent = nullptr);

  void addElement(EditorElement *child);

private:
  QHBoxLayout *m_layout;
};

#endif // EDITOR_LINE_H
```

No `elements()`, no `removeElement()`, no `isEmpty()`. Just `addElement` for
use during `relayout()`. The line owns its layout and children are
parented into it via `addElement`.

### `EditorBlock` changes

#### Layout strategy

`EditorBlock` uses a `QVBoxLayout` (alignTop, no spacing, no stretch) to
stack its lines vertically. This replaces the manual `setGeometry()` and
`setFixedHeight()` calls currently in `relayout()`.

In the constructor, create the layout once:

```cpp
EditorBlock::EditorBlock(QWidget *parent)
    : EditorElement(parent), m_selected(false), m_margins(8, 4, 8, 4) {
  m_layout = new QVBoxLayout(this);
  m_layout->setAlignment(Qt::AlignTop);
  m_layout->setContentsMargins(m_margins);
  m_layout->setSpacing(0);
}
```

No `setFixedHeight()` needed — the layout's `sizeHint()` naturally computes
the block height from its line children.

#### `relayout()` rewrite

`relayout()` is now a destructive rebuild:

1. **Reparent elements from `m_elements`**: iterate `m_elements` and call
   `setParent(nullptr)` on each element that is NOT an
   `EditorFragmentSub`. This detaches non-sub-fragment elements that were placed in lines during the
   previous `relayout()`.
2. **Delete all lines**: delete every `EditorLine` widget. This removes
   them from `m_layout` and destroys their remaining children — the
   `EditorTextFragmentSub` instances — via Qt parent-child.
3. **Compute new lines**: walk `m_elements`, determine line breaks, create
   `EditorLine` widgets and distribute children:

   ```
   for each child in m_elements:
     if child is EditorBrFragment:
       finalize current line, start new line
     else if child is EditorBlock (nested block):
       finalize current line
       create new EditorLine, add block to it, add line to layout
       start new line
     else if child is EditorTextFragment:
       measure text, word-wrap if needed
       for each wrapped piece:
         create EditorTextFragmentSub, add to current line
         if line overflowed: finalize current line, start new line, add sub to new line
       hide the parent EditorTextFragment (setVisible(false))
     else:
       if doesn't fit on current line: finalize, start new line
       add to current line
   ```

   The parent `EditorTextFragment` is hidden (not deleted) — its lifetime
   is tied to `m_elements` on the `EditorBlock`. Only the
   `EditorTextFragmentSub` instances are placed into lines.
   `EditorTextFragmentSub` widgets are owned by the parent block (their Qt
   parent is the `EditorLine` they live in, which is destroyed on the next
   `relayout()`). The `EditorTextFragment` references them via its
   `m_subs` list, which stores them as the base type `EditorFragmentSub *`.

4. **Add each line to `m_layout`**.

#### `m_elements` remains canonical

`QList<EditorElement *> m_elements` on `EditorBlock` stays — it is the
source of truth for what content the block owns. No `m_lines` or
`m_currentLine` members. No `addLine()`, no `lines()` accessor. Lines are
computed from `m_elements` during `relayout()` and discarded afterward.

#### `sizeHint()` simplification

```cpp
QSize EditorBlock::sizeHint() const {
  return m_layout->sizeHint();
}
```

The layout computes the height automatically.

#### `addElement()` unchanged

`addElement(EditorElement *child)` still appends to `m_elements` and calls
`child->setParent(this)`. The child is temporarily parented to the block;
during the next `relayout()`, it (or its subs) gets reparented into
the appropriate `EditorLine`.

### Parsing (`EditorDocument`) — no changes

The markdown callbacks (`enterBlock`, `textCallback`, etc.) still call
`block->addElement(frag)` which appends to `m_elements`. No changes needed
to the parsing pipeline.

### `EditorFragmentSub` / `EditorTextFragmentSub` retained

`EditorFragmentSub` (base class, `src/include/notes/fragments/EditorFragment.h:8`)
and `EditorTextFragmentSub` (subclass, `src/include/notes/fragments/EditorTextFragment.h:9`)
are kept. During `relayout()`:

- The parent `EditorTextFragment` is hidden (`setVisible(false)`).
- `EditorTextFragmentSub` instances are created, parented to their
  `EditorLine`, and distributed across lines.
- Subs are **owned** by the parent block — their Qt parent is the
  `EditorLine` they are placed in, which is destroyed on the next
  `relayout()`. The `EditorTextFragment` only **references** them via its
  `m_subs` list (stored as `EditorFragmentSub *`).
- The parent `EditorTextFragment` stays parented to the block (not
  deleted), so its lifetime matches the block's.
- When the fragment's text changes, a `relayout()` call regenerates subs.

### Selection (`EditorCursor`) — minimal impact

`EditorCursor::applySelection()` uses `findChildren<EditorElement *>()`.
Since `EditorLine` is a `QWidget` (not an `EditorElement`), and subs are
`EditorElement` children of lines, `findChildren` still finds them.
QPoint-based selection (`src/EditorCursor.cpp:18`) continues to work
because widget geometry is set by the layouts.

No `EditorCursor` changes needed.

### Widget tree after `relayout()`

```
EditorDocument (EditorBlock, QVBoxLayout)
├── EditorLine (QHBoxLayout)
│   ├── EditorTextFragmentSub "Hello"
│   ├── EditorTextFragmentSub " "
│   └── EditorTextFragmentSub "world"
├── EditorLine (QHBoxLayout)
│   └── EditorBrFragment
├── EditorLine (QHBoxLayout)
│   └── EditorTextFragmentSub "Foo"
└── EditorLine (QHBoxLayout)
    └── EditorHeadingBlock (QVBoxLayout)
        ├── EditorLine (QHBoxLayout)
        │   └── EditorTextFragmentSub "Title"

[hidden, parented to EditorBlock, not in any layout]
  EditorTextFragment "Hello world"  → subs above
  EditorTextFragment "Foo"          → sub above
  EditorTextFragment "Title"         → sub above
```

### Files affected

| File | Change |
|------|--------|
| `src/include/notes/EditorLine.h` | **New** — minimal class declaration |
| `src/EditorLine.cpp` | **New** — `addElement` adds to internal `QHBoxLayout` |
| `src/include/notes/blocks/EditorBlock.h` | Add `QVBoxLayout *m_layout`, remove `setFixedHeight` calls |
| `src/blocks/EditorBlock.cpp` | Rewrite `relayout()`, update constructor, simplify `sizeHint()`, update `setMargins()` to update layout margins |
| `CMakeLists.txt` | Add `src/EditorLine.cpp` and `src/include/notes/EditorLine.h` |

### Implementation steps

1. **Create `EditorLine`** — header and source with `QHBoxLayout` and
   `addElement`.
2. **Refactor `EditorBlock` constructor** — create `QVBoxLayout`,
   configure margins/alignment.
3. **Rewrite `EditorBlock::relayout()`** — destroy lines/subs, recompute
   from `m_elements`, reparent into new lines, add lines to layout.
4. **Simplify `EditorBlock::sizeHint()`** — delegate to layout.
5. **Update `setMargins()`** — propagate to `m_layout->setContentsMargins()`.
6. **Build and test** — verify layout, selection, markdown parsing.
