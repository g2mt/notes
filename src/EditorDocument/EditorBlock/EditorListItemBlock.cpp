#include "notes/EditorDocument.h"

EditorListItemBlock::EditorListItemBlock(QWidget *parent)
    : EditorBlock(parent) {
  m_margins = QMargins(24, 2, 8, 2);
}
