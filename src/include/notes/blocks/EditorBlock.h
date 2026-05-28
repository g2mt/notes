#ifndef EDITOR_BLOCK_H
#define EDITOR_BLOCK_H

#include <QList>
#include <QMargins>

#include "notes/EditorElement.h"

class EditorBlock : public EditorElement {
  Q_OBJECT

public:
  EditorBlock(QWidget *parent = nullptr);

  QSize sizeHint() const override;
  bool isSelected() const;

  virtual void relayout();
  virtual void setMargins(const QMargins &margins);

  bool isEmpty() const;

  const QList<EditorElement *> &elements() const;
  void addElement(EditorElement *child);

protected:
  void resizeEvent(QResizeEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

  bool m_selected;
  QMargins m_margins;
  int m_lineHeight = 0;
  QList<EditorElement *> m_elements;
};

#endif // EDITOR_BLOCK_H
