#include "notes/blocks/EditorCodeBlock.h"

#include <QPainter>

EditorCodeBlock::EditorCodeBlock(QWidget *parent) : EditorBlock(parent) {}

void EditorCodeBlock::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.fillRect(rect(), palette().base().color().darker(105));
  EditorBlock::paintEvent(event);
}
