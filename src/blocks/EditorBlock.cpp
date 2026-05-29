#include "notes/blocks/EditorBlock.h"
#include "notes/EditorLine.h"
#include "notes/fragments/EditorBrFragment.h"
#include "notes/fragments/EditorTextFragment.h"

#include <QFontMetrics>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <qlabel.h>
#include <qnamespace.h>

//
// Constructor / Destructor
//

EditorBlock::EditorBlock(QWidget *parent)
    : EditorElement(parent), m_selected(false), m_margins(8, 4, 8, 4) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  m_layout = new QVBoxLayout(this);
  m_layout->setAlignment(Qt::AlignTop);
  m_layout->setContentsMargins(m_margins);
  m_layout->setSpacing(0);
}

//
// Getters / Setters
//

bool EditorBlock::isSelected() const { return m_selected; }

void EditorBlock::setSelected(bool selected) {
  m_selected = selected;
  update();
}

void EditorBlock::setMargins(const QMargins &margins) {
  m_margins = margins;
  m_layout->setContentsMargins(margins);
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
  int availableWidth = width() - m_margins.left() - m_margins.right();

  // Detach "real" elements from previous lines
  for (auto *elem : m_elements) {
    elem->setParent(this);
  }

  // Delete existing EditorLine widgets (and their sub children)
  const auto lines = findChildren<EditorLine *>(Qt::FindDirectChildrenOnly);
  for (auto *line : lines) {
    line->deleteLater();
  }

  // Build new lines
  EditorLine *currentLine = nullptr;
  int lineWidth = 0;

  auto flushLine = [&]() {
    if (currentLine != nullptr) {
      m_layout->addWidget(currentLine);
    }
    currentLine = new EditorLine(this);
    lineWidth = 0;
  };

  for (auto *child : m_elements) {
    if (qobject_cast<EditorBrFragment *>(child)) {
      flushLine();
      currentLine->addWidget(child);
    } else if (auto *block = qobject_cast<EditorBlock *>(child)) {
      flushLine();
      currentLine->addWidget(block);
    } else if (auto *tf = qobject_cast<EditorTextFragment *>(child)) {
      currentLine = currentLine == nullptr ? new EditorLine(this) : currentLine;

      int fragLineH = tf->sizeHint().height();
      const QString &text = tf->text();
      QFontMetrics fm(tf->charFormat().font());

      int textOffset = 0;
      int subStart = 0;
      int subWidth = 0;

      while (textOffset < text.length()) {
        int boundary = nextWordBoundary(text, textOffset);

        if (boundary == textOffset) {
          QChar delim = text[textOffset];
          int delimWidth = fm.horizontalAdvance(delim);

          if (lineWidth + subWidth + delimWidth > availableWidth &&
              lineWidth > 0) {
            if (subWidth > 0) {
              auto *sub = new EditorTextFragmentSub(subStart, textOffset, tf);
              sub->setFixedSize(subWidth, fragLineH);
              currentLine->addWidget(sub);
            }
            flushLine();
            subStart = textOffset;
            subWidth = 0;
          }

          subWidth += delimWidth;
          textOffset++;
        } else {
          QString word = text.mid(textOffset, boundary - textOffset);
          int wordWidth = fm.horizontalAdvance(word);

          if (lineWidth + subWidth + wordWidth > availableWidth &&
              lineWidth > 0) {
            if (subWidth > 0) {
              auto *sub = new EditorTextFragmentSub(subStart, textOffset, tf);
              sub->setFixedSize(subWidth, fragLineH);
              currentLine->addWidget(sub);
            }
            flushLine();
            subStart = textOffset;
            subWidth = 0;
          }

          subWidth += wordWidth;
          textOffset = boundary;
        }
      }

      if (textOffset > subStart) {
        auto *sub = new EditorTextFragmentSub(subStart, textOffset, tf);
        sub->setFixedSize(subWidth, fragLineH);
        currentLine->addWidget(sub);
        lineWidth += subWidth;
      }

      tf->hide();
    } else {
      int fragW = child->sizeHint().width();

      if (lineWidth + fragW > availableWidth && lineWidth > 0)
        flushLine();

      currentLine->addWidget(child);
      lineWidth += fragW;
    }
  }

  flushLine();
}
