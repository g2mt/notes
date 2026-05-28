#include "notes/blocks/EditorListBlock.h"

EditorListBlock::EditorListBlock(Type type, QWidget *parent)
    : EditorBlock(parent), m_type(type) {
  setMargins(QMargins(16, 4, 8, 4));
}

EditorListBlock::Type EditorListBlock::listType() const { return m_type; }
