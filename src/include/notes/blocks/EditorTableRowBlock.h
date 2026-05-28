#ifndef EDITOR_TABLE_ROW_BLOCK_H
#define EDITOR_TABLE_ROW_BLOCK_H

#include "notes/blocks/EditorBlock.h"

#include "notes/blocks/EditorTableBlock.h"

class EditorTableRowBlock : public EditorBlock {
  Q_OBJECT

public:
  EditorTableRowBlock(QWidget *parent = nullptr);

  EditorTableBlock *table() const;
  void relayout() override;
};

#endif // EDITOR_TABLE_ROW_BLOCK_H
