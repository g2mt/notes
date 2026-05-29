#ifndef EDITOR_LINE_H
#define EDITOR_LINE_H

#include <QWidget>

class QHBoxLayout;

class EditorLine : public QWidget {
  Q_OBJECT

public:
  explicit EditorLine(QWidget *parent = nullptr);

  void addWidget(QWidget *child);

private:
  QHBoxLayout *m_layout;
};

#endif // EDITOR_LINE_H
