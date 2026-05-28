#include "notes/fragments/EditorTextFragment.h"

#include <QFontMetrics>
#include <QPaintEvent>
#include <QPainter>

EditorTextFragmentSub::EditorTextFragmentSub(int textOffsetStart,
                                             int textOffsetEnd,
                                             EditorTextFragment *parent)
    : EditorFragmentSub(parent), m_textOffsetStart(textOffsetStart),
      m_textOffsetEnd(textOffsetEnd) {}

void EditorTextFragmentSub::paintEvent(QPaintEvent *event) {
  auto *fragment = qobject_cast<EditorTextFragment *>(parentWidget());
  assert(fragment != nullptr);

  QPainter painter(this);
  painter.setFont(fragment->m_charFormat.font());
  painter.setPen(fragment->m_charFormat.foreground().color());

  QString chunk = fragment->m_text.mid(m_textOffsetStart,
                                       m_textOffsetEnd - m_textOffsetStart);
  int lineH = fragment->lineHeight();
  painter.drawText(QPoint(0, lineH - painter.fontMetrics().descent()), chunk);
}

EditorTextFragment::EditorTextFragment(QWidget *parent)
    : EditorFragment(parent) {}

EditorTextFragment::~EditorTextFragment() {}

const QString &EditorTextFragment::text() const { return m_text; }

void EditorTextFragment::setText(QString &text) {
  m_text = text;
  updateGeometry();
  update();
}

QTextCharFormat EditorTextFragment::charFormat() const { return m_charFormat; }

void EditorTextFragment::setCharFormat(QTextCharFormat fmt) {
  m_charFormat = fmt;
  updateGeometry();
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

int EditorTextFragment::preferredWidth() const {
  QFontMetrics fm(m_charFormat.font());
  return fm.horizontalAdvance(m_text);
}

int EditorTextFragment::lineHeight() const {
  QFontMetrics fm(m_charFormat.font());
  return fm.height();
}

void EditorTextFragment::paintEvent(QPaintEvent *event) {
  if (!subs().isEmpty())
    return;

  QPainter painter(this);
  painter.setFont(m_charFormat.font());
  painter.setPen(m_charFormat.foreground().color());
  painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, m_text);
}
