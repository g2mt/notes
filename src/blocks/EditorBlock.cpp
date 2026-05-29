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
    : EditorElement(parent), m_selected(false) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  m_layout = new QVBoxLayout(this);
  m_layout->setAlignment(Qt::AlignTop);
  m_layout->setContentsMargins(0, 0, 0, 0);
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

bool EditorBlock::isEmpty() const { return m_elements.isEmpty(); }

const QList<EditorElement *> &EditorBlock::elements() const {
  return m_elements;
}

//
// Event Handlers
//

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

void EditorBlock::relayout() {
  const int availableWidth = width();

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
      // Fits entirely within one line
      if (tf->sizeHint().width() < availableWidth) {
        flushLine();
        currentLine->addWidget(tf);
        continue;
      }

      // Word wrapping is required
      currentLine = currentLine == nullptr ? new EditorLine(this) : currentLine;
      const QString &text = tf->text();
      QFontMetrics fm(tf->charFormat().font());
      int chunkStart = 0;
      int offset = 0;
      QList<EditorFragmentSub *> subs;

      while (offset < text.length()) {
        // Find the next word boundary (space or punctuation)
        int wordEnd = offset;
        while (wordEnd < text.length()) {
          QChar ch = text[wordEnd];
          if (ch.isSpace() || ch.isPunct())
            break;
          wordEnd++;
        }

        // Include following spaces and punctuation with the word
        while (wordEnd < text.length() &&
               (text[wordEnd].isSpace() || text[wordEnd].isPunct())) {
          wordEnd++;
        }

        // Cumulative width from chunkStart to the current word boundary
        int chunkWidth =
            fm.horizontalAdvance(text.mid(chunkStart, wordEnd - chunkStart));

        if (chunkWidth > availableWidth) {
          // Even a single word overflows — let it break the line
          if (offset == chunkStart) {
            offset = wordEnd;
            continue;
          }

          // Split before the current word: sub from chunkStart to offset
          auto *sub = new EditorTextFragmentSub(chunkStart, offset, tf);
          subs.append(sub);
          currentLine->addWidget(sub);
          flushLine();
          chunkStart = offset;
          continue;
        }

        offset = wordEnd;
      }

      // Create a sub for any remaining text on the final line
      if (chunkStart < text.length()) {
        auto *sub = new EditorTextFragmentSub(chunkStart, text.length(), tf);
        subs.append(sub);
        currentLine->addWidget(sub);
        lineWidth += fm.horizontalAdvance(
            text.mid(chunkStart, text.length() - chunkStart));
      }

      tf->setSubs(subs);
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
