# Custom Renderer Spec

> **Warning:** This file was generated with AI assistance.

Implement each class declared in `src/include/notes/EditorDocument.h` in its
own `.cpp` file under `src/EditorDocument/`. For example:
- `src/EditorDocument.cpp`: contains only EditorDocument
- `src/EditorDocument/EditorElement.cpp`
- `src/EditorDocument/EditorBlock.cpp`
- `src/EditorDocument/EditorFragment.cpp`
- `src/EditorDocument/EditorCursor.cpp`

## md4c Integration

### CMakeLists.txt

- Add `vendored/md4c/src/md4c.c` and `vendored/md4c/src/entity.c` to sources
- Add `vendored/md4c/src` to target include directories

### Parsing: `EditorDocument::setMarkdown()`

1. Clear existing child widgets (blocks) — delete all `EditorBlock` children.
2. Set up `MD_PARSER` callbacks with `this` as `userdata`.
3. Maintain two stacks on the document:
   - **Block stack**: each `enter_block` pushes a new `EditorBlock*`, each `leave_block` pops.
   - **Format stack**: each `enter_span` pushes a `QTextCharFormat` built by copying the current top and applying the span's formatting; each `leave_span` pops.
4. Only handle `MD_BLOCK_P` (paragraph) for now — other block types are ignored.
5. Callbacks:
   - **`enter_block`** (`MD_BLOCK_P`): create a new `EditorBlock`, push to block stack, add as child widget of `EditorDocument`.
   - **`leave_block`**: pop block stack.
   - **`text`**: create an `EditorFragment` from `QString::fromUtf8(text, size)`, apply the current `QTextCharFormat` from the top of the format stack, add as child of the top block.
   - **`enter_span`**: copy top format, apply span-specific changes (bold → `setBold(true)`, italic → `setItalic(true)`, strikethrough → `setFontStrikeOut(true)`, code → `setFontFamilies({"monospace"})` + optional background, underline → `setUnderline(true)`, superscript → `setVerticalAlignment(QTextCharFormat::AlignSuperScript)`, subscript → `setVerticalAlignment(QTextCharFormat::AlignSubScript)`), push.
   - **`leave_span`**: pop format stack.
6. Flags: `MD_FLAG_COLLAPSEWHITESPACE | MD_FLAG_STRIKETHROUGH | MD_FLAG_TABLES | MD_FLAG_UNDERLINE | MD_FLAG_SUPERSCRIPTS | MD_FLAG_SUBSCRIPTS`
7. After parsing, call `updateGeometry()` on the document and each block to trigger relayout.

## Word Wrapping

### EditorFragment

- Stores `m_text` (QString) and `m_charFormat` (QTextCharFormat).
- **`widthForText(const QString &text) const`**: returns pixel width of the given text using `QFontMetrics` of `m_charFormat.font()`.
- **`preferredWidth() const`**: returns `widthForText(m_text)` — the width the fragment wants to occupy.
- **`lineHeight() const`**: returns `QFontMetrics(m_charFormat.font()).height()`.
- **`paintEvent`**: draws `m_text` at `(0, 0)` with `m_charFormat` via `QPainter::drawText`.

### EditorBlock

- Children: list of `EditorFragment*` in document order.
- Has a layout-margin inset, e.g. `(8, 4, 8, 4)`.
- Responsive to its own width (set by the parent `EditorDocument`).

#### Layout: `EditorBlock::relayoutFragments()`

Called from `resizeEvent` and after fragments change.

```
availableWidth = width() - leftMargin - rightMargin
x = leftMargin
y = topMargin
lineHeight = 0

for each fragment in children:
  fragWidth = fragment->preferredWidth()

  if x + fragWidth > leftMargin + availableWidth and x > leftMargin:
    // wrap to next line
    x = leftMargin
    y += max(lineHeight, fragment->lineHeight())
    lineHeight = fragment->lineHeight()

  // Single-fragment overflow: split the text
  if fragWidth > availableWidth:
    split fragment->text() at word boundaries:
      measure cumulative width of words + spaces
      when cumulative > availableWidth, record an EditorFragmentSub
        with textOffset (char index into fragment->text()) and
        a QPoint (pixel offset within fragment for painting)
    The sub-fragments are stored as QList<EditorFragmentSub> on the fragment.
    The widget itself is geometrically as tall as the wrapped text.
  else:
    // normal placement
    fragment->setGeometry(x, y, fragWidth, fragment->lineHeight())
    x += fragWidth
    lineHeight = max(lineHeight, fragment->lineHeight())

// After iterating all fragments, set the block's total height:
setFixedHeight(y + lineHeight + bottomMargin)
```

#### `EditorBlock::sizeHint()`

Returns the computed height after layout; width is determined by the parent.

#### `EditorBlock::paintEvent(QPaintEvent*)`

- Draws optional block-level decorations: selection highlight (if `m_selected` is true), paragraph background.
- Fragments paint themselves (they are child QWidgets positioned by layout).

## Rendering Overview

```
Editor (QScrollArea)
  └── EditorDocument (QWidget)
       ├── QVBoxLayout or manual positioning
       ├── EditorBlock (QWidget) — paragraph
       │    ├── EditorFragment (QWidget) — "Hello "
       │    ├── EditorFragment (QWidget) — "**world**"  [bold]
       │    └── EditorFragment (QWidget) — "!"
       ├── EditorBlock — another paragraph
       │    └── ...
       └── ...
```

- **EditorDocument** stacks blocks vertically. It can use a simple `QVBoxLayout` with zero spacing and stretch-at-bottom, or manually call `setGeometry` on each block during its own `resizeEvent`.
- **EditorBlock** handles wrapping: positions its fragment children based on available width. Fragments are placed left-to-right within the block; when they overflow, they wrap to the next visual line.
- **EditorFragment** is a leaf widget that paints a single run of uniformly-formatted text at its origin `(0, 0)`.

### Cursor / Selection (future)

- `EditorCursor` stores references to the `EditorElement*` at the start and end of a selection.
- When cursor moves, selection is communicated to blocks/fragments so they can draw selection backgrounds in their `paintEvent`.
- `EditorFragment` stores `m_selectionStart` and `m_selectionEnd` as character offsets; during paint, it draws a highlight rectangle behind the selected character range.
