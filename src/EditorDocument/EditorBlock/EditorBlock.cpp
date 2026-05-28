#include "notes/EditorDocument.h"

#include <QFontMetrics>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>
#include <qnamespace.h>

EditorBlock::EditorBlock(QWidget *parent)
    : EditorElement(parent), m_selected(false), m_margins(8, 4, 8, 4) {}

bool EditorBlock::isSelected() const { return m_selected; }

void EditorBlock::setMargins(const QMargins &margins) { m_margins = margins; }

QSize EditorBlock::sizeHint() const {
  int bottom = childrenRect().bottom() + m_margins.bottom();
  return QSize(width(), bottom);
}

void EditorBlock::addWidget(EditorElement *child) {
  child->setParent(this);
  m_children.append(child);
  child->show();
}

void EditorBlock::resizeEvent(QResizeEvent *event) {
  EditorElement::resizeEvent(event);
  relayout();
}

void EditorBlock::relayout() {
  int x = m_margins.left();
  int y = m_margins.top();
  m_lineHeight = 0;
  int availableWidth = width() - m_margins.left() - m_margins.right();

  auto flushLine = [&]() {
    x = m_margins.left();
    y += m_lineHeight;
    m_lineHeight = 0;
  };

  const auto &children = m_children;

  for (auto *child : children) {
    auto *block = qobject_cast<EditorBlock *>(child);
    if (block) {
      flushLine();

      block->relayout();
      int blockW = width() - m_margins.left() - m_margins.right();
      if (blockW < 0)
        blockW = 0;
      int blockH = block->sizeHint().height();
      block->setGeometry(m_margins.left(), y, blockW, blockH);

      x = m_margins.left();
      y += blockH;
      m_lineHeight = 0;
      continue;
    }

    auto *frag = qobject_cast<EditorFragment *>(child);
    assert(frag != nullptr);

    if (qobject_cast<EditorBrFragment *>(frag)) {
      flushLine();
      continue;
    }

    auto *tf = qobject_cast<EditorTextFragment *>(frag);
    if (!tf)
      continue;

    int fragWidth = tf->preferredWidth();
    int fragLineH = tf->lineHeight();

    if (x + fragWidth <= availableWidth) {
      tf->setGeometry(x, y, fragWidth, fragLineH);
      tf->setSubs({});
      x += fragWidth;
      m_lineHeight = qMax(m_lineHeight, fragLineH);
      continue;
    }

    if (x > m_margins.left())
      flushLine();

    const QString &text = tf->text();
    QFontMetrics fm(tf->charFormat().font());
    QList<EditorFragmentSub> subs;

    int widgetOriginX = x;
    int widgetOriginY = y;
    int maxX = x;
    int textOffset = 0;
    int lineSubStart = 0;
    int lineSubX = x;

    while (textOffset < text.length()) {
      int spaceIdx = text.indexOf(' ', textOffset);
      int wordEnd = (spaceIdx == -1) ? text.length() : spaceIdx;

      QString word = text.mid(textOffset, wordEnd - textOffset);
      int wordWidth = fm.horizontalAdvance(word);

      if (x + wordWidth > availableWidth && x > m_margins.left()) {
        subs.append({lineSubStart, textOffset,
                     QPoint(lineSubX - widgetOriginX, y - widgetOriginY)});
        flushLine();
        lineSubStart = textOffset;
        lineSubX = x;
      }

      x += wordWidth;

      if (spaceIdx != -1) {
        x += fm.horizontalAdvance(' ');
        textOffset = spaceIdx + 1;
      } else {
        textOffset = text.length();
      }

      maxX = qMax(maxX, x);
      m_lineHeight = qMax(m_lineHeight, fragLineH);
    }

    subs.append({lineSubStart, textOffset,
                 QPoint(lineSubX - widgetOriginX, y - widgetOriginY)});

    int widgetW = maxX - widgetOriginX;
    int widgetH = (y - widgetOriginY) + fragLineH;
    tf->setGeometry(widgetOriginX, widgetOriginY, widgetW, widgetH);
    tf->setSubs(subs);
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
