#include "notes/fragments/EditorTextFragment.h"

#include <QFontMetrics>
#include <QPaintEvent>
#include <QPainter>
#include <QSize>

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
  QString chunk =
      m_tf->m_text.mid(m_textOffsetStart, m_textOffsetEnd - m_textOffsetStart);
  // qDebug() << chunk;
  QFontMetrics fm(m_tf->m_charFormat.font());
  setFixedSize(QSize(fm.horizontalAdvance(chunk), fm.height()));
}

void EditorTextFragmentSub::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.setFont(m_tf->m_charFormat.font());
  painter.setPen(m_tf->m_charFormat.foreground().color());

  QString chunk =
      m_tf->m_text.mid(m_textOffsetStart, m_textOffsetEnd - m_textOffsetStart);
  painter.drawText(QPoint(0, height() - painter.fontMetrics().descent()),
                   chunk);
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
  QFontMetrics fm(m_charFormat.font());
  setFixedSize(QSize(fm.horizontalAdvance(m_text), fm.height()));
  update();
}

QTextCharFormat EditorTextFragment::charFormat() const { return m_charFormat; }

void EditorTextFragment::setCharFormat(QTextCharFormat fmt) {
  m_charFormat = fmt;
  QFontMetrics fm(m_charFormat.font());
  setFixedSize(QSize(fm.horizontalAdvance(m_text), fm.height()));
  update();
}

int EditorTextFragment::selectionStart() const { return m_selectionStart; }

int EditorTextFragment::selectionEnd() const { return m_selectionEnd; }

void EditorTextFragment::setSelected(bool selected) {
  if (selected) {
    m_selectionStart = 0;
    m_selectionEnd = m_text.length();
  } else {
    m_selectionStart = -1;
    m_selectionEnd = -1;
  }
  update();
}

void EditorTextFragment::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.setFont(m_charFormat.font());
  painter.setPen(m_charFormat.foreground().color());
  painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, m_text);
}
