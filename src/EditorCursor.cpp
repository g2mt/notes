#include "notes/EditorDocument.h"

#include <algorithm>

EditorCursor::EditorCursor(EditorDocument *doc) : m_document(doc) {}

int EditorCursor::indexOf(EditorElement *element) const {
  if (!m_document)
    return -1;
  const auto &elements = m_document->m_orderedElements;
  for (int i = 0; i < elements.size(); ++i) {
    if (elements[i] == element)
      return i;
  }
  return -1;
}

void EditorCursor::applySelection() {
  if (!m_document)
    return;

  int curMin =
      std::min(m_selectionStart, m_selectionEnd);
  int curMax =
      std::max(m_selectionStart, m_selectionEnd);

  const auto &elements = m_document->m_orderedElements;

  for (int i = 0; i < elements.size(); ++i) {
    bool inPrev = i >= m_prevMin && i <= m_prevMax;
    bool inCur = i >= curMin && i <= curMax;

    if (inCur && !inPrev)
      elements[i]->setSelected(true);
    else if (!inCur && inPrev)
      elements[i]->setSelected(false);
  }

  m_prevMin = curMin;
  m_prevMax = curMax;
}

void EditorCursor::select(EditorElement *element) {
  clearSelection();

  int idx = indexOf(element);
  if (idx < 0)
    return;

  m_selectionStart = idx;
  m_selectionEnd = idx;
  applySelection();
  emit selectionChanged();
}

void EditorCursor::extendTo(EditorElement *element) {
  int idx = indexOf(element);
  if (idx < 0)
    return;

  m_selectionEnd = idx;
  applySelection();
  emit selectionChanged();
}

void EditorCursor::clearSelection() {
  if (!m_document)
    return;

  if (m_selectionStart >= 0) {
    int prevMin = std::min(m_selectionStart, m_selectionEnd);
    int prevMax = std::max(m_selectionStart, m_selectionEnd);

    const auto &elements = m_document->m_orderedElements;
    for (int i = prevMin; i <= prevMax && i < elements.size(); ++i)
      elements[i]->setSelected(false);
  }

  m_selectionStart = -1;
  m_selectionEnd = -1;
  m_prevMin = -1;
  m_prevMax = -1;
  emit selectionChanged();
}

bool EditorCursor::isSelected(EditorElement *element) const {
  int idx = indexOf(element);
  if (idx < 0 || m_selectionStart < 0)
    return false;

  int lo = std::min(m_selectionStart, m_selectionEnd);
  int hi = std::max(m_selectionStart, m_selectionEnd);
  return idx >= lo && idx <= hi;
}

bool EditorCursor::hasSelection() const { return m_selectionStart != -1; }

bool EditorCursor::isDragging() const { return m_dragging; }

void EditorCursor::setDragging(bool dragging) { m_dragging = dragging; }
