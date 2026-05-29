#include "notes/blocks/EditorHrBlock.h"

#include <QPainter>

EditorHrBlock::EditorHrBlock(QWidget *parent) : EditorBlock(parent) {
  setFixedHeight(18);
}

void EditorHrBlock::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  int y = height() / 2;
  painter.setPen(palette().mid().color());
  painter.drawLine(0, y, width(), y);
}
