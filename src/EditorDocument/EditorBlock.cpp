#include "notes/EditorDocument.h"

#include <QFontMetrics>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <qnamespace.h>

EditorBlock::EditorBlock(QWidget *parent)
    : EditorElement(parent), m_selected(false), m_margins(8, 4, 8, 4) {}

bool EditorBlock::isSelected() const { return m_selected; }

void EditorBlock::setMargins(const QMargins &margins) { m_margins = margins; }

QSize EditorBlock::sizeHint() const {
  int bottom = childrenRect().bottom() + m_margins.bottom();
  return QSize(width(), bottom);
}

void EditorBlock::addWidget(EditorBlock *child) {
  child->setParent(this);
  child->show();
}

void EditorBlock::resizeEvent(QResizeEvent *event) {
  EditorElement::resizeEvent(event);
  relayout();
}

void EditorBlock::relayout() {
  int x = m_margins.left();
  int y = m_margins.top();
  m_lineHeight = 0;
  int availableWidth = width() - m_margins.left() - m_margins.right();

  auto flushLine = [&]() {
    x = m_margins.left();
    y += m_lineHeight;
    m_lineHeight = 0;
  };

  const auto &children =
      findChildren<EditorElement *>(QString(), Qt::FindDirectChildrenOnly);

  for (auto *child : children) {
    auto *block = qobject_cast<EditorBlock *>(child);
    if (block) {
      flushLine();

      block->relayout();
      int blockW = width() - m_margins.left() - m_margins.right();
      if (blockW < 0)
        blockW = 0;
      int blockH = block->sizeHint().height();
      block->setGeometry(m_margins.left(), y, blockW, blockH);

      x = m_margins.left();
      y += blockH;
      m_lineHeight = 0;
      continue;
    }

    auto *frag = qobject_cast<EditorFragment *>(child);
    assert(frag != nullptr);

    if (qobject_cast<EditorBrFragment *>(frag)) {
      flushLine();
      continue;
    }

    auto *tf = qobject_cast<EditorTextFragment *>(frag);
    if (!tf)
      continue;

    int fragWidth = tf->preferredWidth();
    int fragLineH = tf->lineHeight();

    if (x + fragWidth <= availableWidth) {
      tf->setGeometry(x, y, fragWidth, fragLineH);
      tf->setSubs({});
      x += fragWidth;
      m_lineHeight = qMax(m_lineHeight, fragLineH);
      continue;
    }

    if (x > m_margins.left())
      flushLine();

    const QString &text = tf->text();
    QFontMetrics fm(tf->charFormat().font());
    QList<EditorFragmentSub> subs;

    int widgetOriginX = x;
    int widgetOriginY = y;
    int maxX = x;
    int textOffset = 0;
    int lineSubStart = 0;
    int lineSubX = x;

    while (textOffset < text.length()) {
      int spaceIdx = text.indexOf(' ', textOffset);
      int wordEnd = (spaceIdx == -1) ? text.length() : spaceIdx;

      QString word = text.mid(textOffset, wordEnd - textOffset);
      int wordWidth = fm.horizontalAdvance(word);

      if (x + wordWidth > availableWidth && x > m_margins.left()) {
        subs.append({lineSubStart, textOffset,
                     QPoint(lineSubX - widgetOriginX, y - widgetOriginY)});
        flushLine();
        lineSubStart = textOffset;
        lineSubX = x;
      }

      x += wordWidth;

      if (spaceIdx != -1) {
        x += fm.horizontalAdvance(' ');
        textOffset = spaceIdx + 1;
      } else {
        textOffset = text.length();
      }

      maxX = qMax(maxX, x);
      m_lineHeight = qMax(m_lineHeight, fragLineH);
    }

    subs.append({lineSubStart, textOffset,
                 QPoint(lineSubX - widgetOriginX, y - widgetOriginY)});

    int widgetW = maxX - widgetOriginX;
    int widgetH = (y - widgetOriginY) + fragLineH;
    tf->setGeometry(widgetOriginX, widgetOriginY, widgetW, widgetH);
    tf->setSubs(subs);
  }

  setFixedHeight(y + m_lineHeight + m_margins.bottom());
}

void EditorBlock::paintEvent(QPaintEvent *event) {
  QPainter painter(this);

  if (m_selected) {
    painter.fillRect(rect(), palette().highlight().color().lighter(180));
  }

  EditorElement::paintEvent(event);
}

EditorMultiLineBlock::EditorMultiLineBlock(QWidget *parent)
    : EditorBlock(parent) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(m_margins.left(), m_margins.top(),
                             m_margins.right(), m_margins.bottom());
  layout->setSpacing(0);
}

QSize EditorMultiLineBlock::sizeHint() const { return layout()->sizeHint(); }

void EditorMultiLineBlock::addWidget(EditorBlock *child) {
  auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
  layout->addWidget(child);
}

void EditorMultiLineBlock::setMargins(const QMargins &margins) {
  EditorBlock::setMargins(margins);
  qobject_cast<QVBoxLayout *>(layout())->setContentsMargins(
      margins.left(), margins.top(), margins.right(), margins.bottom());
}

EditorListItemBlock::EditorListItemBlock(QWidget *parent)
    : EditorBlock(parent) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->setAlignment(Qt::AlignTop);
  m_margins = QMargins(24, 2, 8, 2);
}

void EditorListItemBlock::addWidget(EditorBlock *child) {
  auto *l = qobject_cast<QVBoxLayout *>(layout());
  l->addWidget(child);
}

EditorListBlock::EditorListBlock(Type type, QWidget *parent)
    : EditorMultiLineBlock(parent), m_type(type) {
  setMargins(QMargins(16, 4, 8, 4));
}

EditorListBlock::Type EditorListBlock::listType() const { return m_type; }

EditorTableCellBlock::EditorTableCellBlock(bool isHeader, QWidget *parent)
    : EditorBlock(parent), m_isHeader(isHeader) {
  m_margins = QMargins(4, 4, 4, 4);
}

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

EditorTableBlock::EditorTableBlock(QWidget *parent)
    : EditorMultiLineBlock(parent) {
  setMargins(QMargins(0, 4, 0, 4));
}

EditorHrBlock::EditorHrBlock(QWidget *parent) : EditorBlock(parent) {
  m_margins = QMargins(8, 8, 8, 8);
  setFixedHeight(18);
}

void EditorHrBlock::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  int y = height() / 2;
  painter.setPen(palette().mid().color());
  painter.drawLine(m_margins.left(), y, width() - m_margins.right(), y);
}

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

EditorCodeBlock::EditorCodeBlock(QWidget *parent) : EditorBlock(parent) {
  m_margins = QMargins(8, 4, 8, 4);
}

void EditorCodeBlock::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.fillRect(rect(), palette().base().color().darker(105));
  EditorBlock::paintEvent(event);
}

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
