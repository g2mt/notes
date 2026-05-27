#if 0
#include "notes/Editor.h"

#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextDocumentFragment>
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

bool Editor::hasFileChangedExternally() const {
  if (m_filePath.isEmpty())
    return false;

  QFileInfo fi(m_filePath);
  return fi.lastModified() != m_fileLastModified || fi.size() != m_fileSize;
}

const QString &Editor::filePath() const { return m_filePath; }

void Editor::setFilePath(const QString &path) {
  m_filePath = path;
  QFileInfo fi(path);
  if (fi.exists()) {
    m_fileLastModified = fi.lastModified();
    m_fileSize = fi.size();
  } else {
    m_fileLastModified = QDateTime();
    m_fileSize = 0;
  }
}

bool Editor::save(const QString &path) {
  QString p;
  if (path.isEmpty()) {
    p = QFileDialog::getSaveFileName(this, tr("Save As"), m_filePath);
    if (p.isEmpty())
      return false;
  } else {
    p = path;
  }

  if (p == m_filePath && hasFileChangedExternally()) {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("File Changed"));
    msgBox.setText(tr("The file has been modified outside the editor."));
    msgBox.setInformativeText(
        tr("Do you want to overwrite it, save elsewhere, or discard?"));
    auto *overwriteBtn =
        msgBox.addButton(tr("&Overwrite"), QMessageBox::AcceptRole);
    auto *saveAsBtn = msgBox.addButton(tr("Save &as"), QMessageBox::AcceptRole);
    auto *discardBtn =
        msgBox.addButton(tr("&Discard"), QMessageBox::DestructiveRole);
    msgBox.setDefaultButton(overwriteBtn);
    msgBox.setIcon(QMessageBox::Warning);

    msgBox.exec();
    auto *clicked = msgBox.clickedButton();

    if (clicked == saveAsBtn) {
      p = QFileDialog::getSaveFileName(this, tr("Save File as"), m_filePath);
      if (p.isEmpty())
        return false;
    } else if (clicked == discardBtn) {
      return false;
    }
  }

  QFile file(p);
  if (file.open(QIODevice::WriteOnly)) {
    file.write(toMarkdown().toUtf8());
    file.close();
    m_filePath = p;
    QFileInfo newFi(p);
    m_fileLastModified = newFi.lastModified();
    m_fileSize = newFi.size();
    document()->setModified(false);
    return true;
  }
  return false;
}

void Editor::close(EditorCloseRequest req) {
  bool canCancel = (req == EditorCloseRequest::Normal);

  if (!document()->isModified()) {
    emit closed(req);
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
          [this, msgBox, saveBtn, saveAsBtn, discardBtn, canCancel, req](int) {
            auto *clicked = msgBox->clickedButton();

            if (clicked == saveBtn || clicked == saveAsBtn) {
              if (save(clicked == saveAsBtn ? "" : m_filePath))
                emit closed(req);
              else if (!canCancel)
                emit closed(req);
            } else if (clicked == discardBtn) {
              emit closed(req);
            }
          });

  msgBox->open();
}

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

static QString stripHeadingMarkdown(const QTextDocumentFragment &fragment) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
  QString md = fragment.toMarkdown().replace("\n", " ").trimmed();
#else
  QTextDocument doc;
  doc.setHtml(fragment.toHtml());
  QString md = doc.toMarkdown().trimmed();
#endif

  static const QRegularExpression headingRe(QStringLiteral("^#+\\s*"));
  md.remove(headingRe);
  return md;
}

void Editor::wrapHeading(int level) {
  level = qBound(1, level, 6);

  QTextCursor cursor = textCursor();
  cursor.movePosition(QTextCursor::StartOfBlock);
  cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
  if (cursor.currentTable() != nullptr || cursor.currentList() != nullptr)
    return;

  QString md = stripHeadingMarkdown(QTextDocumentFragment(cursor));
  md = QStringLiteral("#").repeated(level) + QStringLiteral(" ") + md;
  cursor.insertMarkdown(md);
}

void Editor::clearHeading() {
  QTextCursor cursor = textCursor();
  cursor.movePosition(QTextCursor::StartOfBlock);
  cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
  if (cursor.currentTable() != nullptr || cursor.currentList() != nullptr)
    return;

  QString md = stripHeadingMarkdown(QTextDocumentFragment(cursor));
  cursor.insertMarkdown(md);
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

void Editor::focusInEvent(QFocusEvent *event) {
  if (hasFileChangedExternally()) {
    QFileInfo fi(m_filePath);
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("File Changed"));
    msgBox.setText(tr("The file has been modified outside the editor."));
    msgBox.setInformativeText(
        tr("Do you want to reload it or discard the external changes?"));
    auto *reloadBtn = msgBox.addButton(tr("&Reload"), QMessageBox::AcceptRole);
    auto *discardBtn =
        msgBox.addButton(tr("&Discard"), QMessageBox::DestructiveRole);
    msgBox.setDefaultButton(reloadBtn);
    msgBox.setIcon(QMessageBox::Question);

    msgBox.exec();
    auto *clicked = msgBox.clickedButton();

    if (clicked == reloadBtn) {
      QFile file(m_filePath);
      if (file.open(QIODevice::ReadOnly))
        setMarkdown(QString::fromUtf8(file.readAll()));
      m_fileLastModified = fi.lastModified();
      m_fileSize = fi.size();
      document()->setModified(false);
    } else {
      m_fileLastModified = fi.lastModified();
      m_fileSize = fi.size();
    }
  }

  QTextEdit::focusInEvent(event);
}
#endif
