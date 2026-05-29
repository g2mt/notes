#ifndef EDITOR_BLOCK_H
#define EDITOR_BLOCK_H

#include <QList>
#include <QMargins>

#include "notes/EditorElement.h"

class QVBoxLayout;

class EditorBlock : public EditorElement {
  Q_OBJECT

public:
  EditorBlock(QWidget *parent = nullptr);

  bool isSelected() const;
  void setSelected(bool selected) override;

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
  QList<EditorElement *> m_elements;
  QVBoxLayout *m_layout = nullptr;
};

#endif // EDITOR_BLOCK_H
