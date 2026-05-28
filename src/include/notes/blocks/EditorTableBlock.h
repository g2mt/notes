#ifndef EDITOR_TABLE_BLOCK_H
#define EDITOR_TABLE_BLOCK_H

#include "notes/blocks/EditorBlock.h"

class EditorTableBlock : public EditorBlock {
  Q_OBJECT

public:
  EditorTableBlock(int col, int row, QWidget *parent = nullptr);

  void addElement(EditorElement *child);

  int colCount() const;

private:
  int m_colCount;
  int m_rowCount;
};

#endif // EDITOR_TABLE_BLOCK_H
