#include "notes/EditorElement.h"
#include "notes/EditorDocument.h"

#include <QMouseEvent>

EditorElement::EditorElement(QWidget *parent) : QWidget(parent) {}

EditorDocument *EditorElement::document() const {
  for (QWidget *w = parentWidget(); w; w = w->parentWidget()) {
    if (auto *doc = qobject_cast<EditorDocument *>(w))
      return doc;
  }
  return nullptr;
}

QString EditorElement::toMarkdown() const { return QString(); }

void EditorElement::setSelected(bool selected) {}

void EditorElement::mousePressEvent(QMouseEvent *event) {
  QWidget::mousePressEvent(event);

  auto *doc = document();
  if (!doc || !doc->cursor())
    return;

  qDebug() << "select" << this;
  doc->cursor()->setDragging(true);
  doc->cursor()->select(this);
}

void EditorElement::mouseMoveEvent(QMouseEvent *event) {
  QWidget::mouseMoveEvent(event);

  auto *doc = document();
  if (!doc || !doc->cursor())
    return;

  if (!doc->cursor()->isDragging())
    return;

  QPoint docPos = mapTo(doc, event->pos());
  auto *child = qobject_cast<EditorElement *>(doc->childAt(docPos));
  if (child) {
    qDebug() << "extendTo" << child;
    doc->cursor()->extendTo(child);
  }
}

void EditorElement::mouseReleaseEvent(QMouseEvent *event) {
  QWidget::mouseReleaseEvent(event);
  qDebug() << "release" << this;

  auto *doc = document();
  if (doc && doc->cursor())
    doc->cursor()->setDragging(false);
}
