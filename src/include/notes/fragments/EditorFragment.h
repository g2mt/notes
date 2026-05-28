#ifndef EDITOR_FRAGMENT_H
#define EDITOR_FRAGMENT_H

#include <QList>

#include "notes/EditorElement.h"

class EditorFragmentSub : public QWidget {
  Q_OBJECT

public:
  explicit EditorFragmentSub(QWidget *parent = nullptr);
};

class EditorFragment : public EditorElement {
  Q_OBJECT

public:
  EditorFragment(QWidget *parent = nullptr);

  const QList<EditorFragmentSub *> &subs() const;
  void setSubs(QList<EditorFragmentSub *>);

private:
  QList<EditorFragmentSub *> m_subs;
};

#endif // EDITOR_FRAGMENT_H
