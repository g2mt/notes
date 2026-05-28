#ifndef EDITOR_ADMONITION_BLOCK_H
#define EDITOR_ADMONITION_BLOCK_H

#include "notes/blocks/EditorBlock.h"

class EditorAdmonitionBlock : public EditorBlock {
  Q_OBJECT

public:
  EditorAdmonitionBlock(const QString &type, QWidget *parent = nullptr);

  QString admonitionType() const;

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  QString m_type;
};

#endif // EDITOR_ADMONITION_BLOCK_H
