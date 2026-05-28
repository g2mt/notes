#include "notes/EditorDocument.h"

EditorListBlock::EditorListBlock(Type type, QWidget *parent)
    : EditorMultiLineBlock(parent), m_type(type) {
  setMargins(QMargins(16, 4, 8, 4));
}

EditorListBlock::Type EditorListBlock::listType() const { return m_type; }
