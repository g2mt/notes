# Refactor: Remove QVBoxLayout, manual-only layout

> **Warning:** This file was generated with AI assistance.

## Goal

Remove all `QVBoxLayout` usage from `EditorDocument` and its block classes. All child positioning is done entirely by `EditorBlock::relayout()` and `EditorDocument`'s own manual stacking logic.

## Motivation

The current code mixes two layout strategies:
1. Qt's `QVBoxLayout` (on `EditorMultiLineBlock`, `EditorListItemBlock`, `EditorDocument`)
2. Manual positioning via `EditorBlock::relayout()` (iterating children with `findChildren`, calling `setGeometry`)

These conflict. After `relayout()` manually positions children, a subsequent Qt layout pass (e.g. on `showEvent`) re-activates the `QVBoxLayout`, which repositions children according to its own rules, overwriting the manual positions. This causes the sublist bug where nested list items display inline.

Additionally, `EditorMultiLineBlock::sizeHint()` returns `layout()->sizeHint()`, which knows nothing about text fragments (they're not in the layout). So height calculations are wrong for blocks containing both text and nested blocks.

Also, `findChildren` does not guarantee insertion order. We need to track children in a `QList` to preserve the order they were added.

## High-level changes

1. **Add `QList<EditorElement *> m_children` to `EditorBlock`** — preserves insertion order.
2. **Add `EditorBlock::addElement(EditorElement *)`** — appends to `m_children` and calls `setParent` + `show`.
3. **Simplify `EditorBlock::addWidget`** — non-virtual, delegates to `addElement`.
4. **Delete `EditorMultiLineBlock`** — its only purpose was QVBoxLayout management.
5. **Simplify `EditorListItemBlock`** — remove QVBoxLayout, remove `addWidget` override, keep only margin setup.
6. **Simplify `EditorDocument`** — remove QVBoxLayout; stack top-level blocks manually.

## File-by-file changes

### `src/include/notes/EditorDocument.h`

**EditorBlock:**
- Make `addWidget(EditorBlock *)` non-virtual.
- Add `void addElement(EditorElement *child)` method (appends to `m_children`, `setParent`, `show`).
- Add `QList<EditorElement *> m_children` protected member.
- Remove unused `#include <QVBoxLayout>` if present.

**Remove `EditorMultiLineBlock` class entirely.**

**EditorListItemBlock:**
- Remove `addWidget` override.
- Constructor stays (just sets `m_margins`).

**EditorListBlock, EditorTableBlock, EditorAdmonitionBlock:**
- Inherit `EditorBlock` instead of `EditorMultiLineBlock`.

**EditorDocument:**
- Add `void relayout()` method (renamed from current `resizeEvent` logic).
- Add `QSize sizeHint() const override`.
- Remove QVBoxLayout-related includes.

### `src/EditorDocument/EditorBlock/EditorBlock.cpp`

**`EditorBlock::addElement(EditorElement *child)`:**
```cpp
void EditorBlock::addElement(EditorElement *child) {
  child->setParent(this);
  m_children.append(child);
  child->show();
}
```

**`EditorBlock::addWidget(EditorBlock *child)`:**
- Replace body with `addElement(child)`.

**`EditorBlock::relayout()`:**
- Replace `findChildren<EditorElement *>(QString(), Qt::FindDirectChildrenOnly)` with iteration over `m_children`.

### `src/EditorDocument/EditorBlock/EditorMultiLineBlock.cpp`

**Delete the file.**

### `src/EditorDocument/EditorBlock/EditorListItemBlock.cpp`

**Constructor:**
- Remove QVBoxLayout setup. Keep the `m_margins = QMargins(24, 2, 8, 2)` assignment.
- Remove `#include <QVBoxLayout>`.

**Remove `addWidget` override entirely.**

### `src/EditorDocument.cpp`

**Constructor:**
- Remove QVBoxLayout setup. Body becomes empty (or minimal).
- Remove `#include <QVBoxLayout>`.

**`addBlockToParent`:**
- Top-level branch: replace `layout->addWidget(block)` with `doc->addElement(block)`.
- The `EditorDocument` needs an `addElement` method too. Or just inline: `block->setParent(doc); block->show()`.

Actually, `EditorDocument` is not an `EditorBlock`. So we need a way to track top-level children on the document too. Add a `QList<EditorBlock *> m_topBlocks` to `EditorDocument`, and an `addBlock(EditorBlock *)` method.

**`resizeEvent`:**
- After calling `block->relayout()` on all blocks, iterate `m_topBlocks` and stack them vertically:
  ```cpp
  int y = 0;
  int w = width();
  for (auto *block : m_topBlocks) {
    block->relayout();
    int h = block->sizeHint().height();
    block->setGeometry(0, y, w, h);
    y += h;
  }
  setFixedHeight(y);
  ```
  (This replaces the separate `relayout()` + stacking currently done in `setMarkdown`.)

**`leaveBlock` / `enterBlock`:**
- Replace all `new EditorMultiLineBlock(doc)` with `new EditorBlock(doc)`.
  Affected cases: `MD_BLOCK_QUOTE`, `MD_BLOCK_THEAD`, `MD_BLOCK_TBODY`, `MD_BLOCK_TR`, `MD_BLOCK_FOOTNOTE_DEF_SECTION`, `MD_BLOCK_FOOTNOTE_DEF`.

**`textCallback`:**
- After `new EditorBrFragment(block)`, add `block->addElement(frag)`.
- After `new EditorTextFragment(block)` setup, add `block->addElement(frag)`.

**`setMarkdown`:**
- After parsing, call `relayout()` (which stacks top-level blocks). Remove the separate `findChildren` + `relayout()` loop at the end.

### `CMakeLists.txt`

- Remove `src/EditorDocument/EditorBlock/EditorMultiLineBlock.cpp` from the sources list.

## Summary of deletions

| File | Deleted |
|---|---|
| `src/EditorDocument/EditorBlock/EditorMultiLineBlock.cpp` | Entire file |
| `EditorMultiLineBlock` class declaration | From header |
| `EditorListItemBlock::addWidget` | Method |
| `EditorDocument` QVBoxLayout | Constructor + `#include` |
| `addBlockToParent` layout branch | Replaced with direct parenting |
