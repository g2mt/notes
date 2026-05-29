#include "notes/blocks/EditorCodeBlock.h"

#include <QPainter>

EditorCodeBlock::EditorCodeBlock(QWidget *parent) : EditorBlock(parent) {
  setMargins(QMargins(8, 4, 8, 4));
}

void EditorCodeBlock::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.fillRect(rect(), palette().base().color().darker(105));
  EditorBlock::paintEvent(event);
}
