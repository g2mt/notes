#include "notes/Editor.h"
#include "notes/EditorDocument.h"

#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QtGlobal>

Editor::Editor(QWidget *parent) : QScrollArea(parent) {
  m_document = new EditorDocument(this);
  setWidget(m_document);
  setWidgetResizable(true);
}

Editor::~Editor() = default;

EditorDocument *Editor::document() const { return m_document; }

void Editor::setMarkdown(const QString &text) { m_document->setMarkdown(text); }

//
// File Saving / Loading
//

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
    qDebug() << "save: toMarkdown";
    file.write(QByteArray());
    file.close();
    m_filePath = p;
    QFileInfo newFi(p);
    m_fileLastModified = newFi.lastModified();
    m_fileSize = newFi.size();
    m_document->setModified(false);
    return true;
  }
  return false;
}

void Editor::close(EditorCloseRequest req) {
  bool canCancel = (req == EditorCloseRequest::Normal);

  if (!m_document->isModified()) {
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
      if (file.open(QIODevice::ReadOnly)) {
        qDebug() << "focusInEvent: reloading" << m_filePath;
        setMarkdown(QString::fromUtf8(file.readAll()));
      }
      m_fileLastModified = fi.lastModified();
      m_fileSize = fi.size();
      m_document->setModified(false);
    } else {
      m_fileLastModified = fi.lastModified();
      m_fileSize = fi.size();
    }
  }
}

//
// Formatting
//

void Editor::setBold(bool bold) { qDebug() << "setBold"; }

void Editor::setItalic(bool italic) { qDebug() << "setItalic"; }

void Editor::setUnderline(bool underline) { qDebug() << "setUnderline"; }

void Editor::setStrikethrough(bool strike) { qDebug() << "setStrikethrough"; }

void Editor::setSuperscript(bool super) { qDebug() << "setSuperscript"; }

void Editor::setSubscript(bool sub) { qDebug() << "setSubscript"; }

bool Editor::isBold() const { return false; }

bool Editor::isItalic() const { return false; }

bool Editor::isUnderline() const { return false; }

bool Editor::isStrikethrough() const { return false; }

bool Editor::isSuperscript() const { return false; }

bool Editor::isSubscript() const { return false; }

void Editor::wrapHeading(int level) { qDebug() << "wrapHeading"; }

void Editor::clearHeading() { qDebug() << "clearHeading"; }

void Editor::insertOrderedList() { qDebug() << "insertOrderedList"; }

void Editor::insertUnorderedList() { qDebug() << "insertUnorderedList"; }

void Editor::insertTable(int rows, int cols) { qDebug() << "insertTable"; }

void Editor::insertPlainText(const QString &text) {
  qDebug() << "insertPlainText";
}

void Editor::insertFromMimeData(const QMimeData *source) {
  qDebug() << "insertFromMimeData";
}

void Editor::removeSelectedText() { qDebug() << "removeSelectedText"; }

//
// Clipboard / Editing
//

void Editor::clear() { qDebug() << "clear"; }

void Editor::copy() { qDebug() << "copy"; }

void Editor::cut() { qDebug() << "cut"; }

void Editor::paste() { qDebug() << "paste"; }

void Editor::undo() { qDebug() << "undo"; }

void Editor::redo() { qDebug() << "redo"; }

bool Editor::isUndoAvailable() const { return false; }

bool Editor::isRedoAvailable() const { return false; }
