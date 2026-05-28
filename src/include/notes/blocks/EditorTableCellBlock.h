#ifndef EDITOR_TABLE_CELL_BLOCK_H
#define EDITOR_TABLE_CELL_BLOCK_H

#include "notes/blocks/EditorBlock.h"

class EditorTableCellBlock : public EditorBlock {
public:
  EditorTableCellBlock(bool isHeader, QWidget *parent = nullptr);

  bool isHeader() const;

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  bool m_isHeader;
};

#endif // EDITOR_TABLE_CELL_BLOCK_H
