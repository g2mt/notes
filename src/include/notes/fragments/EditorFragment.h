#ifndef EDITOR_FRAGMENT_H
#define EDITOR_FRAGMENT_H

#include "notes/EditorElement.h"

class EditorFragment : public EditorElement {
  Q_OBJECT

public:
  EditorFragment(QWidget *parent = nullptr);
};

#endif // EDITOR_FRAGMENT_H
