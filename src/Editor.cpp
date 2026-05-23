#include "notes/Editor.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
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

bool Editor::save(const QString *path) {
  QString p;
  if (!path || path->isEmpty()) {
    p = QFileDialog::getSaveFileName(this, tr("Save As"),
                                     path ? QString() : m_filePath);
    if (p.isEmpty())
      return false;
  } else {
    p = *path;
  }

  QFile file(p);
  if (file.open(QIODevice::WriteOnly)) {
    file.write(toMarkdown().toUtf8());
    file.close();
    m_filePath = p;
    document()->setModified(false);
    return true;
  }
  return false;
}

void Editor::close(bool canCancel) {
  if (!document()->isModified()) {
    emit closed();
    return;
  }

  auto *msgBox = new QMessageBox(this);
  msgBox->setWindowTitle(tr("Unsaved Changes"));
  msgBox->setText(tr("The document has been modified."));
  msgBox->setInformativeText(tr("Do you want to save your changes?"));
  auto *saveBtn = msgBox->addButton(tr("&Save"), QMessageBox::AcceptRole);
  auto *saveAsBtn = msgBox->addButton(tr("Save &as"), QMessageBox::AcceptRole);
  auto *discardBtn =
      msgBox->addButton(tr("&Discard"), QMessageBox::DestructiveRole);
  if (canCancel)
    msgBox->addButton(tr("&Cancel"), QMessageBox::RejectRole);
  msgBox->setDefaultButton(saveBtn);
  msgBox->setIcon(QMessageBox::Question);
  msgBox->setAttribute(Qt::WA_DeleteOnClose);

  connect(msgBox, &QMessageBox::finished, this,
          [this, msgBox, saveBtn, saveAsBtn, discardBtn, canCancel](int) {
            auto *clicked = msgBox->clickedButton();

            if (clicked == saveBtn || clicked == saveAsBtn) {
              if (save(clicked == saveAsBtn ? nullptr : &m_filePath))
                emit closed();
              else if (!canCancel)
                emit closed();
            } else if (clicked == discardBtn) {
              emit closed();
            }
          });

  msgBox->open();
}

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

void Editor::setStrikethrough(bool strike) {
  QTextCharFormat fmt;
  fmt.setFontStrikeOut(strike);
  mergeCurrentCharFormat(fmt);
}

void Editor::setSuperscript(bool super) {
  QTextCharFormat fmt;
  fmt.setVerticalAlignment(super ? QTextCharFormat::AlignSuperScript
                                 : QTextCharFormat::AlignNormal);
  mergeCurrentCharFormat(fmt);
}

void Editor::setSubscript(bool sub) {
  QTextCharFormat fmt;
  fmt.setVerticalAlignment(sub ? QTextCharFormat::AlignSubScript
                               : QTextCharFormat::AlignNormal);
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

void Editor::insertTable(int rows, int cols) {
  QTextCursor cursor = textCursor();
  QString html;
  html += QStringLiteral("<table>");

  // Header row
  html += QStringLiteral("<thead><tr>");
  for (int c = 0; c < cols; ++c)
    html += QStringLiteral("<th></th>");
  html += QStringLiteral("</tr></thead>");

  // Body rows
  if (rows > 1) {
    html += QStringLiteral("<tbody>");
    for (int r = 1; r < rows; ++r) {
      html += QStringLiteral("<tr>");
      for (int c = 0; c < cols; ++c)
        html += QStringLiteral("<td></td>");
      html += QStringLiteral("</tr>");
    }
    html += QStringLiteral("</tbody>");
  }

  html += QStringLiteral("</table>");

  cursor.insertHtml(html);
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
