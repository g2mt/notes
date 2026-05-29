#include "notes/blocks/EditorTableRowBlock.h"

#include "notes/blocks/EditorTableCellBlock.h"

EditorTableRowBlock::EditorTableRowBlock(QWidget *parent)
    : EditorBlock(parent) {}

EditorTableBlock *EditorTableRowBlock::table() const {
  const QWidget *w = this;
  while (w) {
    w = w->parentWidget();
    if (auto *tbl = qobject_cast<const EditorTableBlock *>(w))
      return const_cast<EditorTableBlock *>(tbl);
  }
  return nullptr;
}

void EditorTableRowBlock::relayout() {
  auto *tbl = table();
  if (!tbl)
    return;

  int colCount = tbl->colCount();
  if (colCount == 0) {
    EditorBlock::relayout();
    return;
  }

  bool hasCells = false;
  for (auto *child : m_elements) {
    if (qobject_cast<EditorTableCellBlock *>(child)) {
      hasCells = true;
      break;
    }
  }

  if (!hasCells) {
    EditorBlock::relayout();
    return;
  }

  int availableWidth = width();
  int cellWidth = availableWidth / colCount;
  int x = 0;
  int maxHeight = 0;

  for (auto *child : m_elements) {
    auto *cell = qobject_cast<EditorTableCellBlock *>(child);
    if (!cell)
      continue;

    cell->setFixedWidth(cellWidth); // set width for word wrapping
    cell->relayout();
    int cellH = cell->sizeHint().height();
    cell->setGeometry(x, 0, cellWidth, cellH);
    maxHeight = qMax(maxHeight, cellH);
    x += cellWidth;
  }

  for (auto *child : m_elements) {
    child->setFixedHeight(maxHeight);
  }

  setFixedHeight(maxHeight);
}
