#include "notes/Editor.h"

#include <QFile>
#include <QMimeData>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextListFormat>
#include <QtGlobal>

Editor::Editor(QWidget *parent) : QTextEdit(parent) {
  QFile css(QStringLiteral(":/templates/document.css"));
  if (css.open(QIODevice::ReadOnly))
    document()->setDefaultStyleSheet(QString::fromUtf8(css.readAll()));
  connect(this, &QTextEdit::cursorPositionChanged, this,
          &Editor::formattingChanged);
  connect(document(), &QTextDocument::modificationChanged, this,
          &QWidget::setWindowModified);
}

Editor::~Editor() = default;

const QString &Editor::filePath() const { return m_filePath; }

void Editor::setFilePath(const QString &path) { m_filePath = path; }

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

void Editor::insertFromMimeData(const QMimeData *source) {
  if (!source) {
    QTextEdit::insertFromMimeData(nullptr);
    return;
  }

  QString markdown;

  if (source->hasHtml()) {
    QTextDocument doc;
    doc.setHtml(source->html());
    markdown = doc.toMarkdown();
  } else if (source->hasText()) {
    markdown = source->text();
  } else {
    QTextEdit::insertFromMimeData(source);
    return;
  }

  QTextDocument doc;
  doc.setMarkdown(markdown);
  textCursor().insertHtml(doc.toHtml());
}
