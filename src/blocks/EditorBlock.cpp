#include "notes/blocks/EditorBlock.h"
#include "notes/fragments/EditorBrFragment.h"
#include "notes/fragments/EditorTextFragment.h"

#include <QFontMetrics>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>
#include <qnamespace.h>

//
// Constructor / Destructor
//

EditorBlock::EditorBlock(QWidget *parent)
    : EditorElement(parent), m_selected(false), m_margins(8, 4, 8, 4) {}

//
// Getters / Setters
//

bool EditorBlock::isSelected() const { return m_selected; }

void EditorBlock::setSelected(bool selected) {
  m_selected = selected;
  update();
}

void EditorBlock::setMargins(const QMargins &margins) { m_margins = margins; }

QSize EditorBlock::sizeHint() const {
  int bottom = childrenRect().bottom() + m_margins.bottom();
  return QSize(width(), bottom);
}

bool EditorBlock::isEmpty() const { return m_elements.isEmpty(); }

const QList<EditorElement *> &EditorBlock::elements() const {
  return m_elements;
}

//
// Event Handlers
//

void EditorBlock::resizeEvent(QResizeEvent *event) {
  EditorElement::resizeEvent(event);
  relayout();
}

void EditorBlock::paintEvent(QPaintEvent *event) {
  QPainter painter(this);

  if (m_selected) {
    painter.fillRect(rect(), palette().highlight().color().lighter(180));
  }

  EditorElement::paintEvent(event);
}

//
// Rendering
//

void EditorBlock::addElement(EditorElement *child) {
  child->setParent(this);
  m_elements.append(child);
  child->show();
}

static int nextWordBoundary(const QString &text, int offset) {
  while (offset < text.length()) {
    QChar ch = text[offset];
    if (ch.isSpace() || ch.isPunct())
      return offset;
    offset++;
  }
  return text.length();
}

void EditorBlock::relayout() {
  int x = m_margins.left();
  int y = m_margins.top();
  int lineHeight = 0;
  int availableWidth = width() - m_margins.left() - m_margins.right();

  QList<QWidget *> pending;

  auto flushLine = [&]() {
    int maxH = 0;
    for (auto *w : pending)
      maxH = qMax(maxH, w->height());
    for (auto *w : pending)
      w->move(w->x(), y + maxH - w->height());
    pending.clear();
    x = m_margins.left();
    y += lineHeight;
    lineHeight = 0;
  };

  const auto &children = m_elements;

  for (auto *child : children) {
    // Nested blocks: flush any partial line, then lay out the block below
    if (auto *block = qobject_cast<EditorBlock *>(child)) {
      flushLine();

      block->relayout();
      int w = width() - m_margins.left() - m_margins.right();
      if (w < 0)
        w = 0;
      int h = block->sizeHint().height();
      block->setGeometry(m_margins.left(), y, w, h);

      x = m_margins.left();
      y += h;
      lineHeight = 0;
    }

    // Line break fragment: force a new line
    else if (qobject_cast<EditorBrFragment *>(child)) {
      flushLine();
    }

    // Text fragments: render with word wrapping
    else if (auto *tf = qobject_cast<EditorTextFragment *>(child)) {
      int fragWidth = tf->preferredWidth();
      int fragLineH = tf->lineHeight();

      // Simple case: the entire text fragment fits on the current line
      if (x + fragWidth <= availableWidth) {
        tf->setGeometry(x, y, fragWidth, fragLineH);
        tf->setSubs({});
        pending.append(tf);
        x += fragWidth;
        lineHeight = qMax(lineHeight, fragLineH);
        continue;
      }

      // Fragment doesn't fit: wrap at word boundaries
      if (x > m_margins.left())
        flushLine();

      const QString &text = tf->text();
      QFontMetrics fm(tf->charFormat().font());
      QList<EditorFragmentSub *> subs;

      int widgetOriginX = x;
      int widgetOriginY = y;
      int maxX = x;
      int textOffset = 0;
      int lineSubStart = 0;
      int lineSubX = x;

      while (textOffset < text.length()) {
        int boundary = nextWordBoundary(text, textOffset);

        if (boundary == textOffset) {
          // Token is either a run of word chars, or a single delimiter about
          // to be measured character by character
          QChar delim = text[textOffset];
          int delimWidth = fm.horizontalAdvance(delim);

          if (x + delimWidth > availableWidth && x > m_margins.left()) {
            auto *sub = new EditorTextFragmentSub(lineSubStart, textOffset, tf);
            sub->setGeometry(lineSubX - widgetOriginX, y - widgetOriginY, 0,
                             fragLineH);
            subs.append(sub);
            pending.append(sub);
            flushLine();
            lineSubStart = textOffset;
            lineSubX = x;
          }

          x += delimWidth;
          textOffset++;
        } else {
          QString word = text.mid(textOffset, boundary - textOffset);
          int wordWidth = fm.horizontalAdvance(word);

          if (x + wordWidth > availableWidth && x > m_margins.left()) {
            auto *sub = new EditorTextFragmentSub(lineSubStart, textOffset, tf);
            sub->setGeometry(lineSubX - widgetOriginX, y - widgetOriginY, 0,
                             fragLineH);
            subs.append(sub);
            pending.append(sub);
            flushLine();
            lineSubStart = textOffset;
            lineSubX = x;
          }

          x += wordWidth;
          textOffset = boundary;
        }

        maxX = qMax(maxX, x);
        lineHeight = qMax(lineHeight, fragLineH);
      }

      auto *sub = new EditorTextFragmentSub(lineSubStart, textOffset, tf);
      sub->setGeometry(lineSubX - widgetOriginX, y - widgetOriginY, 0,
                       fragLineH);
      subs.append(sub);
      pending.append(sub);

      int widgetW = maxX - widgetOriginX;
      int widgetH = (y - widgetOriginY) + fragLineH;
      tf->setGeometry(widgetOriginX, widgetOriginY, widgetW, widgetH);
      tf->setSubs(subs);
    }

    // Other inline fragments: display as inline with ongoing line,
    // or in a new block if out of width
    else {
      int fragW = child->sizeHint().width();
      int fragH = child->sizeHint().height();

      if (x + fragW > availableWidth && x > m_margins.left())
        flushLine();

      child->setGeometry(x, y, fragW, fragH);
      pending.append(child);
      x += fragW;
      lineHeight = qMax(lineHeight, fragH);
    }
  }

  flushLine();

  // Account for last line's height and bottom margin
  setFixedHeight(y + m_margins.bottom());
}
