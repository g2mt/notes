#include "notes/EditorDocument.h"

#include <QFontMetrics>
#include <QPainter>

EditorHeadingBlock::EditorHeadingBlock(int level, QWidget *parent)
    : EditorBlock(parent), m_level(level) {
  m_margins = QMargins(8, 8, 8, 4);
}

int EditorHeadingBlock::headingLevel() const { return m_level; }

QFont EditorHeadingBlock::headingFont() const {
  QFont f;
  f.setBold(true);
  double scale = 1.0;
  switch (m_level) {
  case 1:
    scale = 2.0;
    break;
  case 2:
    scale = 1.5;
    break;
  case 3:
    scale = 1.17;
    break;
  case 4:
    scale = 1.0;
    break;
  case 5:
    scale = 0.83;
    break;
  case 6:
    scale = 0.67;
    break;
  }
  if (f.pointSize() > 0)
    f.setPointSizeF(f.pointSizeF() * scale);
  else
    f.setPixelSize(qRound(f.pixelSize() * scale));
  return f;
}

void EditorHeadingBlock::paintEvent(QPaintEvent *event) {
  EditorBlock::paintEvent(event);
}
