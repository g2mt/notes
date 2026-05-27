#include "notes/EditorDocument.h"

#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>

EditorBlock::EditorBlock(QWidget *parent)
    : EditorElement(parent), m_selected(false), m_margins(8, 4, 8, 4) {}

bool EditorBlock::isSelected() const { return m_selected; }

QSize EditorBlock::sizeHint() const {
  return QSize(width(), childrenRect().bottom() + m_margins.bottom());
}

void EditorBlock::resizeEvent(QResizeEvent *event) {
  EditorElement::resizeEvent(event);
  relayoutFragments();
}

void EditorBlock::relayoutFragments() {
  int x = m_margins.left();
  int y = m_margins.top();
  m_lineHeight = 0;

  const auto &children = findChildren<EditorFragment *>(QString(),
                                                        Qt::FindDirectChildrenOnly);

  for (auto *frag : children) {
    if (auto *br = qobject_cast<EditorBrFragment *>(frag)) {
      x = m_margins.left();
      y += m_lineHeight;
      m_lineHeight = 0;
      continue;
    }

    auto *tf = qobject_cast<EditorTextFragment *>(frag);
    if (!tf)
      continue;

    int fragWidth = tf->preferredWidth();
    int fragLineH = tf->lineHeight();
    tf->setGeometry(x, y, fragWidth, fragLineH);
    x += fragWidth;
    m_lineHeight = qMax(m_lineHeight, fragLineH);
  }

  setFixedHeight(y + m_lineHeight + m_margins.bottom());
}

void EditorBlock::paintEvent(QPaintEvent *event) {
  QPainter painter(this);

  if (m_selected) {
    painter.fillRect(rect(), palette().highlight().color().lighter(180));
  }

  EditorElement::paintEvent(event);
}
