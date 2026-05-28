#include "notes/EditorElement.h"

EditorElement::EditorElement(QWidget *parent) : QWidget(parent) {}

EditorDocument *EditorElement::document() const { return m_document; }

QString EditorElement::toMarkdown() const { return QString(); }
