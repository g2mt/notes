#include "notes/blocks/EditorTableBlock.h"

EditorTableBlock::EditorTableBlock(int col, int row, QWidget *parent)
    : EditorBlock(parent), m_colCount(col), m_rowCount(row) {}

int EditorTableBlock::colCount() const { return m_colCount; }
