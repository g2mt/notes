#include "notes/EditorDocument.h"

#include <QWidget>

EditorCursor::EditorCursor(EditorDocument *doc) : m_document(doc) {}

void EditorCursor::applySelection() {
  if (!m_document)
    return;

  int loX = std::min(m_selectionStart.x(), m_selectionEnd.x());
  int loY = std::min(m_selectionStart.y(), m_selectionEnd.y());
  int hiX = std::max(m_selectionStart.x(), m_selectionEnd.x());
  int hiY = std::max(m_selectionStart.y(), m_selectionEnd.y());

  auto elements = m_document->findChildren<EditorElement *>();
  for (auto *elem : elements) {
    QPoint pos = elem->mapTo(m_document, QPoint(0, 0));
    bool inside = pos.y() >= loY && pos.y() <= hiY &&
                  pos.x() >= loX && pos.x() <= hiX;
    elem->setSelected(inside);
  }
}

void EditorCursor::select(EditorElement *element) {
  clearSelection();

  if (!element || !m_document)
    return;

  m_selectionStart = element->mapTo(m_document, QPoint(0, 0));
  m_selectionEnd = m_selectionStart;
  applySelection();
  emit selectionChanged();
}

void EditorCursor::extendTo(EditorElement *element) {
  if (!element || !m_document)
    return;

  m_selectionEnd = element->mapTo(m_document, QPoint(0, 0));
  applySelection();
  emit selectionChanged();
}

void EditorCursor::clearSelection() {
  if (!m_document)
    return;

  auto elements = m_document->findChildren<EditorElement *>();
  for (auto *elem : elements)
    elem->setSelected(false);

  m_selectionStart = QPoint();
  m_selectionEnd = QPoint();
  emit selectionChanged();
}

bool EditorCursor::isSelected(EditorElement *element) const {
  if (!element || !m_document || m_selectionStart.isNull())
    return false;

  QPoint pos = element->mapTo(m_document, QPoint(0, 0));
  int loX = std::min(m_selectionStart.x(), m_selectionEnd.x());
  int loY = std::min(m_selectionStart.y(), m_selectionEnd.y());
  int hiX = std::max(m_selectionStart.x(), m_selectionEnd.x());
  int hiY = std::max(m_selectionStart.y(), m_selectionEnd.y());
  return pos.y() >= loY && pos.y() <= hiY &&
         pos.x() >= loX && pos.x() <= hiX;
}

bool EditorCursor::hasSelection() const { return !m_selectionStart.isNull(); }

bool EditorCursor::isDragging() const { return m_dragging; }

void EditorCursor::setDragging(bool dragging) { m_dragging = dragging; }
