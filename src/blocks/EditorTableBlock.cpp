#include "notes/blocks/EditorTableBlock.h"

EditorTableBlock::EditorTableBlock(int col, int row, QWidget *parent)
    : EditorBlock(parent), m_colCount(col), m_rowCount(row) {
  setMargins(QMargins(0, 4, 0, 4));
  m_cells.resize(m_colCount * m_rowCount);
  m_cells.fill(nullptr);
}

void EditorTableBlock::insertElement(int col, int row, EditorElement *child) {
  int idx = col + row * m_colCount;
  m_cells[idx] = child;
  EditorBlock::addElement(child);
}

void EditorTableBlock::relayout() {
  if (m_cells.isEmpty())
    return;

  int availableWidth = width() - m_margins.left() - m_margins.right();
  if (availableWidth < 0)
    availableWidth = 0;

  int colWidth = availableWidth / m_colCount;

  // First pass: compute row heights
  QList<int> rowHeights(m_rowCount, 0);
  for (int r = 0; r < m_rowCount; ++r) {
    for (int c = 0; c < m_colCount; ++c) {
      auto *cell = qobject_cast<EditorBlock *>(m_cells[c + r * m_colCount]);
      if (cell) {
        cell->relayout();
        int cellH = cell->sizeHint().height();
        if (cellH > rowHeights[r])
          rowHeights[r] = cellH;
      }
    }
  }

  // Second pass: position cells
  int y = m_margins.top();
  for (int r = 0; r < m_rowCount; ++r) {
    int x = m_margins.left();
    for (int c = 0; c < m_colCount; ++c) {
      auto *cell = m_cells[c + r * m_colCount];
      if (cell)
        cell->setGeometry(x, y, colWidth, rowHeights[r]);
      x += colWidth;
    }
    y += rowHeights[r];
  }

  setFixedHeight(y + m_margins.bottom());
}
