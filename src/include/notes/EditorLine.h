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
  int xPlacement() const;
  void addWidget(QWidget *child);

private:
  QList<QWidget *> m_children;
  int m_xPlacement = 0;
};

#endif // EDITOR_LINE_H
