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
    : EditorElement(parent), m_selected(false) {}

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

void EditorBlock::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  // relayout();
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

void EditorBlock::relayout() {
  const int availableWidth = width();
  int totalHeight = 0;
  setFixedHeight(0);

  // Detach "real" elements from previous lines
  for (auto *elem : m_elements) {
    elem->setParent(this);
  }

  // Delete existing EditorLine widgets (and their sub-fragments)
  const auto lines = findChildren<EditorLine *>(Qt::FindDirectChildrenOnly);
  for (auto *line : lines) {
    delete line;
  }

  // Build new lines
  EditorLine *currentLine = nullptr;
  int lineWidth = 0;

  auto emptyLine = [&]() {
    if (currentLine == nullptr) {
      currentLine = new EditorLine(this);
      assert(lineWidth == 0);
      return currentLine;
    }
    if (currentLine->isEmpty()) {
      assert(lineWidth == 0);
      return currentLine;
    }
    currentLine->relayout();
    currentLine->move(0, totalHeight);
    currentLine->show();
    totalHeight += currentLine->height();

    currentLine = new EditorLine(this);
    currentLine->setFixedWidth(availableWidth);
    lineWidth = 0;
    return currentLine;
  };

  for (auto *child : m_elements) {
    if (qobject_cast<EditorBrFragment *>(child)) {
      emptyLine()->addWidget(child);
      child->setVisible(true);
    } else if (auto *block = qobject_cast<EditorBlock *>(child)) {
      emptyLine()->addWidget(block);
      block->setVisible(true);
      block->setFixedWidth(availableWidth);
      block->relayout();
    } else if (auto *tf = qobject_cast<EditorTextFragment *>(child)) {
      // Fits entirely within one line
      int tfWidth = tf->width();
      if (lineWidth + tfWidth < availableWidth) {
        tf->setVisible(true);
        if (currentLine != nullptr) {
          currentLine->addWidget(child);
          lineWidth += tfWidth;
        } else {
          emptyLine()->addWidget(child);
        }
        continue;
      }

      // Word wrapping is required
      tf->setVisible(false); // only subs are visible
      if (currentLine == nullptr)
        emptyLine();
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
          emptyLine()->addWidget(sub);
          chunkStart = offset;
          continue;
        }

        offset = wordEnd;
      }

      // Create a sub for any remaining text on the final line
      if (chunkStart < text.length()) {
        auto *sub = new EditorTextFragmentSub(chunkStart, text.length(), tf);
        subs.append(sub);
        emptyLine()->addWidget(sub);
      }

      tf->setSubs(subs);
    } else {
      child->setVisible(true);
      int fragW = child->width();
      qDebug() << child << "width" << fragW;

      if (lineWidth + fragW < availableWidth && currentLine != nullptr) {
        currentLine->addWidget(child);
        lineWidth += fragW;
      } else {
        emptyLine()->addWidget(child);
      }
    }
  }

  if (currentLine != nullptr) {
    currentLine->relayout();
    currentLine->move(0, totalHeight);
    currentLine->show();
    totalHeight += currentLine->height();
  }
  setFixedHeight(totalHeight);
}
