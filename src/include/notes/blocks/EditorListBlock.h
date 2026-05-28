#ifndef EDITOR_LIST_BLOCK_H
#define EDITOR_LIST_BLOCK_H

#include "notes/blocks/EditorBlock.h"

class EditorListBlock : public EditorBlock {
public:
  enum Type { Unordered, Ordered };

  EditorListBlock(Type type, QWidget *parent = nullptr);

  Type listType() const;

private:
  Type m_type;
};

#endif // EDITOR_LIST_BLOCK_H
