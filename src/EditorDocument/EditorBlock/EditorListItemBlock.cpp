#include "notes/EditorDocument.h"

#include <QVBoxLayout>

EditorListItemBlock::EditorListItemBlock(QWidget *parent)
    : EditorBlock(parent) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->setAlignment(Qt::AlignTop);
  m_margins = QMargins(24, 2, 8, 2);
}

void EditorListItemBlock::addWidget(EditorBlock *child) {
  auto *l = qobject_cast<QVBoxLayout *>(layout());
  l->addWidget(child);
}
