#ifndef EDITOR_CODE_BLOCK_H
#define EDITOR_CODE_BLOCK_H

#include "notes/blocks/EditorBlock.h"

class EditorCodeBlock : public EditorBlock {
  Q_OBJECT

public:
  EditorCodeBlock(QWidget *parent = nullptr);

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // EDITOR_CODE_BLOCK_H
