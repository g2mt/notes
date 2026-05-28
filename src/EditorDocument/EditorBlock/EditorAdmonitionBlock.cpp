#include "notes/EditorDocument.h"

#include <QPainter>

EditorAdmonitionBlock::EditorAdmonitionBlock(const QString &type,
                                             QWidget *parent)
    : EditorMultiLineBlock(parent), m_type(type) {
  setMargins(QMargins(16, 4, 8, 4));
}

QString EditorAdmonitionBlock::admonitionType() const { return m_type; }

void EditorAdmonitionBlock::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  QColor color = Qt::gray;
  if (m_type == "note")
    color = QColor(9, 105, 218);
  else if (m_type == "tip")
    color = QColor(26, 127, 55);
  else if (m_type == "important")
    color = QColor(130, 80, 223);
  else if (m_type == "warning")
    color = QColor(191, 135, 0);
  else if (m_type == "caution")
    color = QColor(207, 34, 46);

  painter.fillRect(0, 0, 4, height(), color);
  EditorBlock::paintEvent(event);
}
