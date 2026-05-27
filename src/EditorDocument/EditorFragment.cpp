#include "notes/EditorDocument.h"

#include <QFontMetrics>
#include <QPainter>
#include <QPaintEvent>

EditorFragment::EditorFragment(QWidget *parent) : EditorElement(parent) {}

const QString &EditorFragment::text() const { return m_text; }

void EditorFragment::setText(QString &text) {
  m_text = text;
  updateGeometry();
  update();
}

QTextCharFormat EditorFragment::charFormat() const { return m_charFormat; }

void EditorFragment::setCharFormat(QTextCharFormat fmt) {
  m_charFormat = fmt;
  updateGeometry();
  update();
}

int EditorFragment::selectionStart() const { return m_selectionStart; }

int EditorFragment::selectionEnd() const { return m_selectionEnd; }

int EditorFragment::widthForText(const QString &text) const {
  QFontMetrics fm(m_charFormat.font());
  return fm.horizontalAdvance(text);
}

int EditorFragment::preferredWidth() const {
  return widthForText(m_text);
}

int EditorFragment::lineHeight() const {
  QFontMetrics fm(m_charFormat.font());
  return fm.height();
}

QList<EditorFragmentSub> EditorFragment::subs() const { return m_subs; }

void EditorFragment::setSubs(QList<EditorFragmentSub> subs) {
  m_subs = subs;
}

void EditorFragment::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.setFont(m_charFormat.font());
  painter.setPen(m_charFormat.foreground().color());

  if (!m_subs.isEmpty()) {
    int lineH = lineHeight();
    for (const auto &sub : m_subs) {
      QString chunk = m_text.mid(sub.textOffsetStart,
                                 sub.textOffsetEnd - sub.textOffsetStart);
      painter.drawText(
          QPoint(sub.pixelOffset.x(),
                 sub.pixelOffset.y() + lineH - painter.fontMetrics().descent()),
          chunk);
    }
  } else {
    painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, m_text);
  }
}
