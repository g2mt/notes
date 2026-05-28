#ifndef EDITOR_HEADING_BLOCK_H
#define EDITOR_HEADING_BLOCK_H

#include "notes/blocks/EditorBlock.h"

class EditorHeadingBlock : public EditorBlock {
  Q_OBJECT

public:
  EditorHeadingBlock(int level, QWidget *parent = nullptr);

  int headingLevel() const;
  QFont headingFont() const;

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  int m_level;
};

#endif // EDITOR_HEADING_BLOCK_H
