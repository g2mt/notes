#include "notes/fragments/EditorFragment.h"

EditorFragmentSub::EditorFragmentSub(QWidget *parent) : QWidget(parent) {}

QSize EditorFragmentSub::sizeHint() const { return size(); }

EditorFragment::EditorFragment(QWidget *parent) : EditorElement(parent) {}

const QList<EditorFragmentSub *> &EditorFragment::subs() const {
  return m_subs;
}

void EditorFragment::setSubs(QList<EditorFragmentSub *> subs) {
  qDeleteAll(m_subs);
  m_subs = subs;
}
