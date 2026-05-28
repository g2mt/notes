#include "notes/EditorDocument.h"

EditorTableBlock::EditorTableBlock(QWidget *parent)
    : EditorBlock(parent) {
  setMargins(QMargins(0, 4, 0, 4));
}
