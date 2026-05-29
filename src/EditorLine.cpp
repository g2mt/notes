#include "notes/EditorLine.h"

#include <QHBoxLayout>
#include <algorithm>

EditorLine::EditorLine(QWidget *parent) : QWidget(parent) {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  m_layout = new QHBoxLayout(this);
  m_layout->setAlignment(Qt::AlignLeft);
  m_layout->setContentsMargins(0, 0, 0, 0);
  m_layout->setSpacing(0);
}

bool EditorLine::isEmpty() const { return m_layout->isEmpty(); }

void EditorLine::addWidget(QWidget *child) {
  child->setParent(this);
  m_layout->addWidget(child);
}

// Implemented instead of sizeHint
int EditorLine::preferredHeight() const {
  int height = 0;
  for (auto *el :
       findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly)) {
    height = std::max(height, el->sizeHint().height());
  }
  return height;
}
