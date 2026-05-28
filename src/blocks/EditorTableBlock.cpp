#include "notes/blocks/EditorTableBlock.h"

EditorTableBlock::EditorTableBlock(int col, int row, QWidget *parent)
    : EditorBlock(parent), m_colCount(col), m_rowCount(row) {
  setMargins(QMargins(0, 4, 0, 4));
}

int EditorTableBlock::colCount() const { return m_colCount; }
