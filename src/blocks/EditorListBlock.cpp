#include "notes/blocks/EditorListBlock.h"

EditorListBlock::EditorListBlock(Type type, QWidget *parent)
    : EditorBlock(parent), m_type(type) {}

EditorListBlock::Type EditorListBlock::listType() const { return m_type; }
