#include "notes/EditorDocument.h"

EditorTableBlock::EditorTableBlock(QWidget *parent)
    : EditorMultiLineBlock(parent) {
  setMargins(QMargins(0, 4, 0, 4));
}
