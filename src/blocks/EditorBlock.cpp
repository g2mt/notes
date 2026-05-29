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

__attribute__((optimize("Ofast"))) void EditorBlock::relayout() {
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
    line->deleteLater();
  }

  // Build new lines
  EditorLine *currentLine = new EditorLine(this);

  auto newLine = [&](bool createLine = true) {
    currentLine->move(0, totalHeight);
    currentLine->show();
    totalHeight += currentLine->height();
    if (createLine)
      currentLine = new EditorLine(this);
  };

  for (auto *child : m_elements) {
    if (qobject_cast<EditorBrFragment *>(child)) {
      currentLine->addWidget(child);
      newLine();
    } else if (auto *block = qobject_cast<EditorBlock *>(child)) {
      block->setFixedWidth(availableWidth);
      block->relayout();
      if (currentLine->isEmpty()) {
        currentLine->addWidget(block);
        newLine();
      } else {
        newLine();
        currentLine->addWidget(block);
      }
    } else if (auto *tf = qobject_cast<EditorTextFragment *>(child)) {
      // Fits entirely within one line
      if (currentLine->xPlacement() + tf->width() < availableWidth) {
        tf->setVisible(true);
        tf->setSubs({});
        currentLine->addWidget(tf);
        continue;
      }

      // Word wrapping is required
      tf->setVisible(false); // only subs are visible
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

        if (currentLine->xPlacement() + chunkWidth > availableWidth) {
          // Split before the current word: sub from chunkStart to offset
          auto *sub = new EditorTextFragmentSub(chunkStart, offset, tf);
          subs.append(sub);
          currentLine->addWidget(sub);
          newLine();
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
      }

      tf->setSubs(subs);
    } else {
      int fragW = child->width();

      if (currentLine->xPlacement() + fragW < availableWidth) {
        currentLine->addWidget(child);
      } else {
        newLine();
        currentLine->addWidget(child);
      }
    }
  }

  newLine(false);
  setFixedHeight(totalHeight);
}
