#include "notes/EditorDocument.h"

#include <QWidget>

EditorCursor::EditorCursor(EditorDocument *doc) : m_document(doc) {}

void EditorCursor::applySelection() {}

void EditorCursor::select(EditorElement *element) {}

void EditorCursor::extendTo(EditorElement *element) {}

void EditorCursor::clearSelection() {}

bool EditorCursor::isSelected(EditorElement *element) const {}

bool EditorCursor::hasSelection() const { return !m_selectionStart.isNull(); }

bool EditorCursor::isDragging() const { return m_dragging; }

void EditorCursor::setDragging(bool dragging) { m_dragging = dragging; }
