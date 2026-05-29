#include "notes/blocks/EditorTableCellBlock.h"

#include <QPainter>

EditorTableCellBlock::EditorTableCellBlock(bool isHeader, QWidget *parent)
    : EditorBlock(parent), m_isHeader(isHeader) {}

bool EditorTableCellBlock::isHeader() const { return m_isHeader; }

void EditorTableCellBlock::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  if (m_isHeader)
    painter.fillRect(rect(), palette().base().color().darker(110));
  else
    painter.fillRect(rect(), palette().base());
  painter.setPen(palette().text().color());
  painter.drawRect(rect().adjusted(0, 0, -1, -1));
  EditorBlock::paintEvent(event);
}
