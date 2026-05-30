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
