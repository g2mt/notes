#ifndef TABLEPOPUP_H
#define TABLEPOPUP_H

#include <QWidget>

class TablePopup : public QWidget {
  Q_OBJECT

public:
  explicit TablePopup(QWidget *parent = nullptr);

signals:
  void accepted(int rows, int cols);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void leaveEvent(QEvent *event) override;

private:
  QPair<int, int> cellAt(int x, int y) const;

  static constexpr int kGridSize = 10;
  static constexpr int kCellSize = 24;
  static constexpr int kLabelHeight = 24;

  int m_hoverRows = 0;
  int m_hoverCols = 0;
};

#endif // TABLEPOPUP_H
