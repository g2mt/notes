#ifndef EDITOR_HR_BLOCK_H
#define EDITOR_HR_BLOCK_H

#include "notes/blocks/EditorBlock.h"

class EditorHrBlock : public EditorBlock {
public:
  EditorHrBlock(QWidget *parent = nullptr);

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // EDITOR_HR_BLOCK_H
