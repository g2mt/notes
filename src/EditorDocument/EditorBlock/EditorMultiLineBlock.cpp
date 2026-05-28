#include "notes/EditorDocument.h"

#include <QVBoxLayout>

EditorMultiLineBlock::EditorMultiLineBlock(QWidget *parent)
    : EditorBlock(parent) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(m_margins.left(), m_margins.top(),
                             m_margins.right(), m_margins.bottom());
  layout->setSpacing(0);
}

QSize EditorMultiLineBlock::sizeHint() const { return layout()->sizeHint(); }

void EditorMultiLineBlock::addWidget(EditorBlock *child) {
  auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
  layout->addWidget(child);
}

void EditorMultiLineBlock::setMargins(const QMargins &margins) {
  EditorBlock::setMargins(margins);
  qobject_cast<QVBoxLayout *>(layout())->setContentsMargins(
      margins.left(), margins.top(), margins.right(), margins.bottom());
}
