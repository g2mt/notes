#include "notes/blocks/EditorHrBlock.h"

#include <QPainter>

EditorHrBlock::EditorHrBlock(QWidget *parent) : EditorBlock(parent) {
  setMargins(QMargins(8, 8, 8, 8));
  setFixedHeight(18);
}

void EditorHrBlock::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  int y = height() / 2;
  painter.setPen(palette().mid().color());
  painter.drawLine(m_margins.left(), y, width() - m_margins.right(), y);
}
