#ifndef EDITOR_LIST_ITEM_BLOCK_H
#define EDITOR_LIST_ITEM_BLOCK_H

#include "notes/blocks/EditorBlock.h"

class EditorListItemBlock : public EditorBlock {
  Q_OBJECT

public:
  EditorListItemBlock(QWidget *parent = nullptr);
};

#endif // EDITOR_LIST_ITEM_BLOCK_H
