#include "notes/Editor.h"
#include "notes/EditorBridge_p.h"

#include <QEventLoop>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QWebChannel>
#include <QWebEnginePage>
#include <QWebEngineView>

void EditorPage::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level,
                                          const QString &message,
                                          int lineNumber,
                                          const QString &sourceID) {
  const char *prefix = "js: ";
  if (level == ErrorMessageLevel)
    prefix = "js error: ";
  else if (level == WarningMessageLevel)
    prefix = "js warn: ";
  qDebug().noquote() << prefix << message << "\n    at" << sourceID << ":"
                     << lineNumber;
}

Editor::Editor(QWidget *parent) : QWidget(parent) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  m_webView = new QWebEngineView(this);
  m_webView->setPage(new EditorPage(m_webView));
  m_channel = new QWebChannel(this);
  m_bridge = new EditorBridge(this);

  m_channel->registerObject(QStringLiteral("bridge"), m_bridge);
  m_webView->page()->setWebChannel(m_channel);

  // Block until the ProseMirror page finishes loading so that
  // runJs calls made immediately after construction are safe.
  QEventLoop loop;
  connect(m_bridge, &EditorBridge::loaded, &loop, &QEventLoop::quit);
  m_webView->setUrl(
      QUrl(QStringLiteral("qrc:/frontend/static/prose-editor.html")));
  loop.exec();

  layout->addWidget(m_webView);

  connect(m_bridge, &EditorBridge::formattingChanged, this,
          &Editor::formattingChanged);
  connect(m_bridge, &EditorBridge::modificationChanged, this,
          &Editor::modificationChanged);
  connect(m_bridge, &EditorBridge::undoAvailable, this, &Editor::undoAvailable);
  connect(m_bridge, &EditorBridge::redoAvailable, this, &Editor::redoAvailable);

  connect(m_bridge, &EditorBridge::modificationChanged, this,
          &QWidget::setWindowModified);
}

Editor::~Editor() = default;

void Editor::runJs(const QString &js) { m_webView->page()->runJavaScript(js); }

QString Editor::getMarkdownSync() {
  QString result;
  QEventLoop loop;
  m_webView->page()->runJavaScript(
      QStringLiteral("editorCommands.getMarkdown()"), [&](const QVariant &v) {
        result = v.toString();
        loop.quit();
      });
  loop.exec();
  return result;
}

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

  QString markdown = getMarkdownSync();

  QFile file(p);
  if (file.open(QIODevice::WriteOnly)) {
    file.write(markdown.toUtf8());
    file.close();
    m_filePath = p;
    QFileInfo newFi(p);
    m_fileLastModified = newFi.lastModified();
    m_fileSize = newFi.size();
    m_bridge->setModified(false);
    return true;
  }
  return false;
}

void Editor::close(EditorCloseRequest req) {
  bool canCancel = (req == EditorCloseRequest::Normal);

  if (!m_bridge->isModified()) {
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
  runJs(QStringLiteral("editorCommands.toggleBold()"));
}

void Editor::setItalic(bool italic) {
  runJs(QStringLiteral("editorCommands.toggleItalic()"));
}

void Editor::setUnderline(bool underline) {
  runJs(QStringLiteral("editorCommands.toggleUnderline()"));
}

void Editor::setStrikethrough(bool strike) {
  runJs(QStringLiteral("editorCommands.toggleStrikethrough()"));
}

void Editor::setSuperscript(bool super) {
  runJs(QStringLiteral("editorCommands.toggleSuperscript()"));
}

void Editor::setSubscript(bool sub) {
  runJs(QStringLiteral("editorCommands.toggleSubscript()"));
}

void Editor::setMarkdown(const QString &markdown) {
  emit m_bridge->markdownChanged(markdown);
}

void Editor::wrapHeading(int level) {
  level = qBound(1, level, 6);
  runJs(QStringLiteral("editorCommands.wrapHeading(%1)").arg(level));
}

void Editor::clearHeading() {
  runJs(QStringLiteral("editorCommands.clearHeading()"));
}

void Editor::insertOrderedList() {
  runJs(QStringLiteral("editorCommands.insertOrderedList()"));
}

void Editor::insertUnorderedList() {
  runJs(QStringLiteral("editorCommands.insertUnorderedList()"));
}

void Editor::insertTable(int rows, int cols) {
  runJs(QStringLiteral("editorCommands.insertTable(%1,%2)").arg(rows).arg(cols));
}

bool Editor::isModified() const { return m_bridge->isModified(); }

void Editor::setModified(bool modified) { m_bridge->setModified(modified); }

bool Editor::isUndoAvailable() const { return false; }

bool Editor::isRedoAvailable() const { return false; }

bool Editor::isEmpty() const {
  QString result;
  QEventLoop loop;
  m_webView->page()->runJavaScript(QStringLiteral("editorCommands.isEmpty()"),
                                   [&](const QVariant &v) {
                                     result = v.toString();
                                     loop.quit();
                                   });
  loop.exec();
  return result == QStringLiteral("true");
}

void Editor::undo() { runJs(QStringLiteral("editorCommands.undo()")); }

void Editor::redo() { runJs(QStringLiteral("editorCommands.redo()")); }

void Editor::cut() { m_webView->triggerPageAction(QWebEnginePage::Cut); }

void Editor::copy() { m_webView->triggerPageAction(QWebEnginePage::Copy); }

void Editor::paste() { m_webView->triggerPageAction(QWebEnginePage::Paste); }

void Editor::insertPlainText(const QString &text) {
  emit m_bridge->insertPlainText(text);
}

void Editor::deleteSelection() {
  runJs(QStringLiteral("document.execCommand('delete')"));
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
      m_bridge->setModified(false);
    } else {
      m_fileLastModified = fi.lastModified();
      m_fileSize = fi.size();
    }
  }

  QWidget::focusInEvent(event);
}
