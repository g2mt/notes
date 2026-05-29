#ifndef EDITOR_BLOCK_H
#define EDITOR_BLOCK_H

#include <QList>

#include "notes/EditorElement.h"

class QVBoxLayout;

class EditorBlock : public EditorElement {
  Q_OBJECT

public:
  EditorBlock(QWidget *parent = nullptr);

  bool isSelected() const;
  void setSelected(bool selected) override;

  virtual void relayout();

  bool isEmpty() const;

  const QList<EditorElement *> &elements() const;
  void addElement(EditorElement *child);

protected:
  void paintEvent(QPaintEvent *event) override;

  bool m_selected;
  QList<EditorElement *> m_elements;
};

#endif // EDITOR_BLOCK_H
