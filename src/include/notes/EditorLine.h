#ifndef EDITOR_LINE_H
#define EDITOR_LINE_H

#include <QList>
#include <QWidget>

class QHBoxLayout;

class EditorLine : public QWidget {
  Q_OBJECT

public:
  explicit EditorLine(QWidget *parent = nullptr);

  bool isEmpty() const;
  void addWidget(QWidget *child);
  int preferredHeight() const;
  void relayout();

private:
  QList<QWidget *> m_children;
};

#endif // EDITOR_LINE_H
