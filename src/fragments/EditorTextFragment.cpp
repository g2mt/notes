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
  int lineH = fragment->m_sizeHint.height();
  painter.drawText(QPoint(0, lineH - painter.fontMetrics().descent()), chunk);
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
  m_sizeHint = QSize(fm.horizontalAdvance(m_text), fm.height());
  updateGeometry();
  update();
}

QTextCharFormat EditorTextFragment::charFormat() const { return m_charFormat; }

void EditorTextFragment::setCharFormat(QTextCharFormat fmt) {
  m_charFormat = fmt;
  QFontMetrics fm(m_charFormat.font());
  m_sizeHint = QSize(fm.horizontalAdvance(m_text), fm.height());
  updateGeometry();
  update();
}

QSize EditorTextFragment::sizeHint() const { return m_sizeHint; }

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
  if (!subs().isEmpty())
    return;

  QPainter painter(this);
  painter.setFont(m_charFormat.font());
  painter.setPen(m_charFormat.foreground().color());
  painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, m_text);
}
