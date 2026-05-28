#include "notes/fragments/EditorTextFragment.h"

#include <QFontMetrics>
#include <QPaintEvent>
#include <QPainter>

EditorTextFragment::EditorTextFragment(QWidget *parent)
    : EditorFragment(parent) {}

EditorTextFragment::~EditorTextFragment() { qDeleteAll(m_subs); }

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

int EditorTextFragment::preferredWidth() const {
  QFontMetrics fm(m_charFormat.font());
  return fm.horizontalAdvance(m_text);
}

int EditorTextFragment::lineHeight() const {
  QFontMetrics fm(m_charFormat.font());
  return fm.height();
}

const QList<EditorFragmentSub *> &EditorTextFragment::subs() const { return m_subs; }

void EditorTextFragment::setSubs(QList<EditorFragmentSub *> subs) {
  qDeleteAll(m_subs);
  m_subs = subs;
}

void EditorTextFragment::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.setFont(m_charFormat.font());
  painter.setPen(m_charFormat.foreground().color());

  if (!m_subs.isEmpty()) {
    int lineH = lineHeight();
    for (const auto *sub : m_subs) {
      QString chunk = m_text.mid(sub->textOffsetStart,
                                 sub->textOffsetEnd - sub->textOffsetStart);
      painter.drawText(
          QPoint(sub->rect.x(),
                 sub->rect.y() + lineH - painter.fontMetrics().descent()),
          chunk);
    }
  } else {
    painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, m_text);
  }
}
