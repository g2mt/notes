#include "notes/TablePopup.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>

TablePopup::TablePopup(QWidget *parent) : QWidget(parent) {
  setWindowFlags(Qt::Popup);
  setMouseTracking(true);
  int w = kGridSize * kCellSize + 2 * 2;
  int h = kGridSize * kCellSize + kLabelHeight + 2 * 2;
  resize(w, h);
}

QPair<int, int> TablePopup::cellAt(int x, int y) const {
  int col = (x - 2) / kCellSize;
  int row = (y - kLabelHeight - 2) / kCellSize;
  return {qBound(0, row, kGridSize - 1), qBound(0, col, kGridSize - 1)};
}

void TablePopup::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, false);

  // Background
  p.fillRect(rect(), palette().window());

  // Label
  if (m_hoverRows > 0 && m_hoverCols > 0)
    p.drawText(QRect(0, 0, width(), kLabelHeight), Qt::AlignCenter,
               tr("%1 x %2 Table").arg(m_hoverRows).arg(m_hoverCols));

  // Grid cells
  for (int r = 0; r < kGridSize; ++r) {
    for (int c = 0; c < kGridSize; ++c) {
      int x = 2 + c * kCellSize;
      int y = kLabelHeight + 2 + r * kCellSize;
      QRect cellRect(x, y, kCellSize, kCellSize);

      bool highlighted = r < m_hoverRows && c < m_hoverCols;
      p.fillRect(cellRect,
                 highlighted ? palette().highlight() : palette().base());
      p.setPen(palette().mid().color());
      p.drawRect(cellRect);
    }
  }
}

void TablePopup::mouseMoveEvent(QMouseEvent *event) {
  auto [row, col] = cellAt(event->pos().x(), event->pos().y());
  row = qMin(row + 1, kGridSize);
  col = qMin(col + 1, kGridSize);

  if (m_hoverRows != row || m_hoverCols != col) {
    m_hoverRows = row;
    m_hoverCols = col;
    update();
  }
}

void TablePopup::mousePressEvent(QMouseEvent *event) {
  auto [row, col] = cellAt(event->pos().x(), event->pos().y());
  if (row >= 0 && col >= 0 && row < kGridSize && col < kGridSize) {
    emit accepted(row + 1, col + 1);
  }
  close();
}

void TablePopup::leaveEvent(QEvent *) {
  m_hoverRows = 0;
  m_hoverCols = 0;
  update();
}
