#include "notes/Editor.h"

#include <QTextCharFormat>

Editor::Editor(QWidget *parent) : QTextEdit(parent) {
  connect(this, &QTextEdit::cursorPositionChanged, this,
          &Editor::formattingChanged);
  connect(document(), &QTextDocument::modificationChanged, this,
          &QWidget::setWindowModified);
}

Editor::~Editor() = default;

void Editor::setBold(bool bold) {
  QTextCharFormat fmt;
  fmt.setFontWeight(bold ? QFont::Bold : QFont::Normal);
  mergeCurrentCharFormat(fmt);
}

void Editor::setItalic(bool italic) {
  QTextCharFormat fmt;
  fmt.setFontItalic(italic);
  mergeCurrentCharFormat(fmt);
}

void Editor::setUnderline(bool underline) {
  QTextCharFormat fmt;
  fmt.setFontUnderline(underline);
  mergeCurrentCharFormat(fmt);
}

bool Editor::isBold() const {
  return currentCharFormat().fontWeight() == QFont::Bold;
}

bool Editor::isItalic() const {
  return currentCharFormat().fontItalic();
}

bool Editor::isUnderline() const {
  return currentCharFormat().fontUnderline();
}


