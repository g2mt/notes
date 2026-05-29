#include "notes/EditorLine.h"

#include <QHBoxLayout>

EditorLine::EditorLine(QWidget *parent) : QWidget(parent) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  m_layout = new QHBoxLayout(this);
  m_layout->setAlignment(Qt::AlignLeft);
  m_layout->setContentsMargins(0, 0, 0, 0);
  m_layout->setSpacing(0);
}

void EditorLine::addWidget(QWidget *child) {
  child->setParent(this);
  m_layout->addWidget(child);
}
