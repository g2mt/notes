#ifndef EDITOR_ELEMENT_H
#define EDITOR_ELEMENT_H

#include <QPoint>
#include <QString>
#include <QWidget>

class EditorDocument;
class QMouseEvent;

class EditorElement : public QWidget {
  friend class EditorDocument;
  Q_OBJECT

public:
  explicit EditorElement(QWidget *parent = nullptr);

  EditorDocument *document() const;
  QString toMarkdown() const;
};

#endif // EDITOR_ELEMENT_H
