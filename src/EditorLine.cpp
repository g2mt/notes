#include "notes/EditorLine.h"

#include <QHBoxLayout>
#include <algorithm>

EditorLine::EditorLine(QWidget *parent) : QWidget(parent) {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}

bool EditorLine::isEmpty() const { return m_children.isEmpty(); }

void EditorLine::addWidget(QWidget *child) {
  child->setParent(this);
  m_children.append(child);
}

void EditorLine::relayout() {
  int width = 0;
  int height = 0;
  for (auto *child : m_children) {
    child->move(width, 0);
    width += child->width();
    height = std::max(child->height(), height);
  }
}
