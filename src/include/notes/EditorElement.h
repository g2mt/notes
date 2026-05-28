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

  virtual void setSelected(bool selected);

protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

private:
  bool m_dragging = false;
};

#endif // EDITOR_ELEMENT_H
