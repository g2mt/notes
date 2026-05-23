#include "notes/EditorTabs.h"
#include "notes/Editor.h"

#include <QApplication>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QFile>
#include <QIcon>
#include <QTabBar>
#include <QToolButton>

EditorTabs::EditorTabs(QWidget *parent) : QTabWidget(parent) {
  setTabsClosable(true);
  setMovable(true);
  setDocumentMode(true);
  setupNewTabButton();
  newDocument();

  connect(this, &QTabWidget::tabCloseRequested, this, [this](int index) {
    auto *editor = qobject_cast<Editor *>(widget(index));
    if (editor)
      editor->close();
  });

  connect(this, &QTabWidget::currentChanged, this, [this](int index) {
    Q_UNUSED(index);
    emit currentEditorChanged(currentEditor());
  });
}

void EditorTabs::setupNewTabButton() {
  auto *button = new QToolButton(this);
  button->setIcon(QIcon::fromTheme("list-add"));
  button->setToolTip(tr("Add new tab"));
  button->setAutoRaise(true);

  setCornerWidget(button, Qt::TopLeftCorner);

  connect(button, &QToolButton::clicked, this, &EditorTabs::newDocument);

  m_addTabButton = button;
}

Editor *EditorTabs::currentEditor() const {
  return qobject_cast<Editor *>(currentWidget());
}

void EditorTabs::newDocument() {
  int index;
  addEditorTab(tr("Untitled"), index);
  setCurrentIndex(index);
}

void EditorTabs::openDocument(const QString &filePath) {
  int index;
  auto *editor = addEditorTab(filePath.section(QLatin1Char('/'), -1), index);
  if (editor) {
    editor->setFilePath(filePath);
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
      editor->setMarkdown(QString::fromUtf8(file.readAll()));
    editor->document()->setModified(false);
  }
  setCurrentIndex(index);
}

void EditorTabs::closeCurrentTab() {
  auto *editor = currentEditor();
  if (editor)
    editor->close();
}

bool EditorTabs::closeAll() {
  if (count() == 0)
    return false;
  if (m_closingAll)
    return false;
  m_closingAll = true;
  QList<Editor *> editors;
  for (int i = 0; i < count(); ++i)
    editors.append(qobject_cast<Editor *>(widget(i)));

  for (auto *editor : editors)
    editor->close(false);
  return true;
}

Editor *EditorTabs::addEditorTab(const QString &title, int &index) {
  auto *editor = new Editor(this);
  index = addTab(editor, title.isEmpty() ? tr("Untitled") : title);

  connect(editor, &Editor::closed, this, [this, editor]() {
    int idx = indexOf(editor);
    if (idx >= 0)
      removeTab(idx);
    if (m_closingAll && count() == 0)
      qApp->quit();
  });

  connect(editor->document(), &QTextDocument::modificationChanged, this,
          [this, editor](bool modified) {
            int idx = indexOf(editor);
            if (idx < 0)
              return;
            QString path = editor->filePath();
            auto base = path.isEmpty() ? tr("Untitled")
                                       : path.section(QLatin1Char('/'), -1);
            setTabText(idx, modified ? base + QStringLiteral("*") : base);
          });

  return editor;
}
