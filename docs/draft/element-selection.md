# Element Selection

> **Warning:** This file was generated with AI assistance.

Implement mouse-based selection of editor elements. Clicking and dragging
across elements updates `EditorCursor` on the `EditorDocument`, and selected
elements visually highlight themselves.

## Current State

- `EditorCursor` (`src/include/notes/EditorDocument.h:13`) already exists with
  `start`/`end` `EditorElement*` fields but has no methods and is barely wired.
- `EditorBlock` (`src/include/notes/blocks/EditorBlock.h:16`) has
  `m_selected`/`isSelected()` and `paintEvent` draws a highlight rect when
  selected. The flag is never set anywhere.
- `EditorTextFragment` has `m_selectionStart`/`m_selectionEnd` for
  character-level selection but they are unused.
- `EditorElement` has `document()` returning the owning `EditorDocument`.
- No element currently handles mouse events.

## Design

### Document ordering

`EditorDocument` maintains a flat `QList<EditorElement *> m_orderedElements`
in visual (document) order. The list is rebuilt at the end of `relayout()`
after all child geometry is final. Elements are collected via
`findChildren<EditorElement *>()` and sorted by Y then X:

```cpp
void EditorDocument::relayout() {
  // … existing layout logic …

  m_orderedElements = findChildren<EditorElement *>();
  std::sort(m_orderedElements.begin(), m_orderedElements.end(),
            [](EditorElement *a, EditorElement *b) {
              if (a->y() != b->y())
                return a->y() < b->y();
              return a->x() < b->x();
            });
}
```

No new public methods — the list is a private member consumed by
`EditorCursor`.

### Selection model

Selection works like text selection in a web page:

1. **Mouse press** on an element: that element's index becomes both
   `m_selectionStart` and `m_selectionEnd` — a single-element selection.
2. **Mouse drag** (while button held down): as the cursor enters different
   elements, `m_selectionEnd` updates to that element's index.
   `m_selectionStart` stays fixed at the anchor.
   - Dragging **after** the anchor: `start ≤ end`, the range is `[start, end]`.
   - Dragging **before** the anchor: `end < start`, the range is `[end, start]`.
3. **Mouse release**: the selection is final.

Selected elements are `m_orderedElements[min(start, end) .. max(start, end)]`.

### Cursor API (`EditorCursor`)

Replace `start`/`end` with indices:

```cpp
class EditorCursor : public QObject {
  friend class EditorDocument;
  Q_OBJECT

public:
  explicit EditorCursor(EditorDocument *doc);

  void select(EditorElement *element);
  void extendTo(EditorElement *element);
  void clearSelection();

  bool isSelected(EditorElement *element) const;
  bool hasSelection() const;

signals:
  void selectionChanged();

private:
  int indexOf(EditorElement *element) const;
  void applySelection();

  EditorDocument *m_document;
  int m_selectionStart = -1;
  int m_selectionEnd = -1;
};
```

- Constructor: stores `m_document`, nothing else.
- `select(element)` — calls `clearSelection()`, sets
  `m_selectionStart = m_selectionEnd = indexOf(element)`, calls
  `applySelection()`, emits `selectionChanged()`.
- `extendTo(element)` — sets `m_selectionEnd = indexOf(element)`, calls
  `applySelection()`, emits `selectionChanged()`.
- `clearSelection()` — iterates `m_orderedElements` from `min` to `max` calling
  `setSelected(false)` on each, resets both indices to `-1`, emits
  `selectionChanged()`.
- `isSelected(element)` — checks if the element's index falls within
  `[min(start, end), max(start, end)]`.
- `hasSelection()` — returns `m_selectionStart != -1`.
- `indexOf(element)` — looks up the element's index in
  `m_document->m_orderedElements`.
- `applySelection()` — iterates `m_orderedElements` from `min` to `max` calling
  `setSelected(true)` on each, and iterates the previous range (if any) calling
  `setSelected(false)` on elements that fell out.

Optimization: instead of clearing and re-selecting the whole range each time,
track the previous `[min, max]` and only update the diff — elements that
entered the range get `setSelected(true)`, elements that left get
`setSelected(false)`.

### Element changes

#### `EditorElement`

Store a `m_dragging` flag. Override mouse handlers:

```cpp
void EditorElement::mousePressEvent(QMouseEvent *event) {
  QWidget::mousePressEvent(event);

  auto *doc = document();
  if (!doc || !doc->cursor())
    return;

  m_dragging = true;
  doc->cursor()->select(this);
}

void EditorElement::mouseMoveEvent(QMouseEvent *event) {
  QWidget::mouseMoveEvent(event);
  if (!m_dragging)
    return;

  auto *doc = document();
  if (!doc || !doc->cursor())
    return;

  doc->cursor()->extendTo(this);
}

void EditorElement::mouseReleaseEvent(QMouseEvent *event) {
  QWidget::mouseReleaseEvent(event);
  m_dragging = false;
}
```

#### `EditorBlock`

Override `setSelected(bool)`:

```cpp
void EditorBlock::setSelected(bool selected) {
  m_selected = selected;
  update();
}
```

#### `EditorTextFragment`

Override `setSelected(bool)`:

```cpp
void EditorTextFragment::setSelected(bool selected) {
  if (selected) {
    m_selectionStart = 0;
    m_selectionEnd = m_text.length();
  } else {
    m_selectionStart = -1;
    m_selectionEnd = -1;
  }
  update();
}
```

### `setSelected(bool)` virtual

Declared on `EditorElement` as a virtual no-op. Overridden in `EditorBlock`
and `EditorTextFragment` as above.

### Clearing selection on background click

Override `EditorDocument::mousePressEvent`:

```cpp
void EditorDocument::mousePressEvent(QMouseEvent *event) {
  if (childAt(event->pos()) == nullptr && m_cursor)
    m_cursor->clearSelection();
  EditorBlock::mousePressEvent(event);
}
```

### Cursor lifetime

`m_cursor` is created in `EditorDocument`'s constructor and destroyed in its
destructor:

```cpp
EditorDocument::EditorDocument(Editor *parent)
    : EditorBlock(parent), m_cursor(new EditorCursor(this)) {}

EditorDocument::~EditorDocument() { delete m_cursor; }
```

`EditorCursor`'s constructor just stores the document:

```cpp
EditorCursor::EditorCursor(EditorDocument *doc) : m_document(doc) {}
```

## Implementation Steps

### Step 1: Add `m_orderedElements` and rebuild at end of `relayout()`

File: `src/include/notes/EditorDocument.h` and `src/EditorDocument.cpp`

- Add `QList<EditorElement *> m_orderedElements` private member.
- At the end of `relayout()`, after geometry is set: use
  `findChildren<EditorElement *>()` and sort by `y()` then `x()`.

### Step 2: Rework `EditorCursor`

File: `src/include/notes/EditorDocument.h` and `src/EditorCursor.cpp`

- Replace `start`/`end` with `m_selectionStart`/`m_selectionEnd` (indices).
- Add constructor taking `EditorDocument*`.
- Implement `select()`, `extendTo()`, `clearSelection()`, `isSelected()`,
  `hasSelection()`, `indexOf()`, `applySelection()`.
- Add `selectionChanged()` signal.

### Step 3: Add `setSelected(bool)` virtual to `EditorElement`

File: `src/include/notes/EditorElement.h`

- Declare `virtual void setSelected(bool selected);` (default no-op).

### Step 4: Override `setSelected` in `EditorBlock` and `EditorTextFragment`

- `EditorBlock`: set/clear `m_selected`, call `update()`.
- `EditorTextFragment`: set/clear `m_selectionStart`/`m_selectionEnd`, call
  `update()`.

### Step 5: Wire mouse handlers on `EditorElement`

File: `src/EditorElement.cpp`

- Override `mousePressEvent`, `mouseMoveEvent`, `mouseReleaseEvent` with drag
  selection logic and a `m_dragging` flag.

### Step 6: Expose `cursor()` on `EditorDocument`

File: `src/include/notes/EditorDocument.h`

- Add `EditorCursor *cursor() const;` public getter.

### Step 7: Handle document background click

File: `src/EditorDocument.cpp`

- Override `mousePressEvent` to call `m_cursor->clearSelection()` when clicking
  empty space.

### Step 8: Create/Destroy cursor in EditorDocument constructor/destructor

File: `src/EditorDocument.cpp`

- `new EditorCursor(this)` in constructor, `delete m_cursor` in destructor.

## Future Extensions

- **Character-level selection in fragments**: use click X coordinate to compute
  character offset within a text fragment, setting `m_selectionStart`/
  `m_selectionEnd` precisely.
- **Keyboard navigation**: arrow keys to move cursor between elements.
- **Clipboard integration**: wire `removeSelectedText()`, `copy()`, `cut()` in
  `Editor` to use cursor selection.
