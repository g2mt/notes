#include "notes/fragments/EditorTextFragment.h"

#include <QFontMetrics>
#include <QPaintEvent>
#include <QPainter>
#include <QSize>

static int renderedWidth(const QFontMetrics &fm, QStringView text) {
  int width = 0;
  for (int i = 0; i < text.size(); ++i) {
    QChar ch = text.at(i);
    width -= fm.leftBearing(ch);
    width += fm.horizontalAdvance(ch);
  }
  return width;
}

static void renderCharByChar(QPainter &painter, const QFontMetrics &fm,
                             QStringView text, int x, int y) {
  if (text.isEmpty())
    return;
  for (int i = 0; i < text.size(); ++i) {
    QChar ch = text.at(i);
    x -= fm.leftBearing(ch);
    painter.drawText(x, y, ch);
    x += fm.horizontalAdvance(ch);
  }
}

//
// EditorTextFragmentSub
//

EditorTextFragmentSub::EditorTextFragmentSub(int textOffsetStart,
                                             int textOffsetEnd,
                                             EditorTextFragment *tf,
                                             QWidget *parent)
    : EditorFragmentSub(parent), m_textOffsetStart(textOffsetStart),
      m_textOffsetEnd(textOffsetEnd), m_tf(tf) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  QStringView chunk =
      QStringView(m_tf->m_text)
          .sliced(m_textOffsetStart, m_textOffsetEnd - m_textOffsetStart);
  QFontMetrics fm = m_tf->fontMetrics();
  setFixedSize(renderedWidth(fm, chunk), fm.height());
}

void EditorTextFragmentSub::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setFont(m_tf->m_charFormat.font());
  painter.setPen(m_tf->m_charFormat.foreground().color());

  QFontMetrics fm = painter.fontMetrics();
  QStringView chunk =
      QStringView(m_tf->m_text)
          .sliced(m_textOffsetStart, m_textOffsetEnd - m_textOffsetStart);
  renderCharByChar(painter, fm, chunk, 0, fm.ascent());
}

//
// EditorTextFragment
//

EditorTextFragment::EditorTextFragment(QWidget *parent)
    : EditorFragment(parent) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

EditorTextFragment::~EditorTextFragment() {}

const QString &EditorTextFragment::text() const { return m_text; }

void EditorTextFragment::setText(QString &text) {
  m_text = text;
  QFontMetrics fm = fontMetrics();
  setFixedSize(renderedWidth(fm, m_text), fm.height());
  update();
}

QFontMetrics EditorTextFragment::fontMetrics() const {
  return QFontMetrics(m_charFormat.font());
}

QTextCharFormat EditorTextFragment::charFormat() const { return m_charFormat; }

void EditorTextFragment::setCharFormat(QTextCharFormat fmt) {
  m_charFormat = fmt;
  QFontMetrics fm = fontMetrics();
  setFixedSize(renderedWidth(fm, m_text), fm.height());
  update();
}

int EditorTextFragment::selectionStart() const { return m_selectionStart; }

int EditorTextFragment::selectionEnd() const { return m_selectionEnd; }

void EditorTextFragment::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.setFont(m_charFormat.font());
  painter.setPen(m_charFormat.foreground().color());
  QFontMetrics fm = fontMetrics();
  int y = rect().top() + (rect().height() - fm.height()) / 2 + fm.ascent();
  renderCharByChar(painter, fm, m_text, rect().left(), y);
}
