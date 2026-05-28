#ifndef EDITOR_TABLE_BLOCK_H
#define EDITOR_TABLE_BLOCK_H

#include "notes/blocks/EditorBlock.h"

class EditorTableBlock : public EditorBlock {
public:
  EditorTableBlock(int col, int row, QWidget *parent = nullptr);

  void relayout() override;
  void addElement(EditorElement *child) = delete;
  void insertElement(int col, int row, EditorElement *child);

private:
  int m_colCount;
  int m_rowCount;
  QList<EditorElement *> m_cells; // col*row 2d array
};

#endif // EDITOR_TABLE_BLOCK_H
