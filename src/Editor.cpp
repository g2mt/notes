#include "notes/Editor.h"

#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextListFormat>
#include <QtGlobal>

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

bool Editor::isItalic() const { return currentCharFormat().fontItalic(); }

bool Editor::isUnderline() const { return currentCharFormat().fontUnderline(); }

void Editor::wrapHeading(int level) {
  level = qBound(1, level, 6);

  QTextCursor cursor = textCursor();
  cursor.movePosition(QTextCursor::StartOfBlock);
  cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
  QString tag = QStringLiteral("h%1").arg(level);
  cursor.insertHtml(QStringLiteral("<%1>%2</%1>")
                        .arg(tag, cursor.selectedText().toHtmlEscaped()));
}

void Editor::insertOrderedList() {
  QTextCursor cursor = textCursor();
  QTextListFormat fmt;
  fmt.setStyle(QTextListFormat::ListDecimal);
  cursor.insertList(fmt);
}

void Editor::insertUnorderedList() {
  QTextCursor cursor = textCursor();
  QTextListFormat fmt;
  fmt.setStyle(QTextListFormat::ListDisc);
  cursor.insertList(fmt);
}
