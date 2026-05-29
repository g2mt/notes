#include "notes/EditorLine.h"

#include <QHBoxLayout>

EditorLine::EditorLine(QWidget *parent) : QWidget(parent) {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}

bool EditorLine::isEmpty() const { return m_children.isEmpty(); }

int EditorLine::xPlacement() const { return m_xPlacement; }

void EditorLine::addWidget(QWidget *child) {
  child->setParent(this);
  m_children.append(child);
  child->move(m_xPlacement, 0);
  m_xPlacement += child->width();
  if (child->height() > height())
    setFixedHeight(child->height());
}
