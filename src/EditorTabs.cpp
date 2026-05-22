#include "notes/EditorTabs.h"
#include "notes/Editor.h"

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

  connect(this, &QTabWidget::tabCloseRequested, this,
          [this](int index) { removeTab(index); });

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
  int index = addEditorTab(tr("Untitled"));
  setCurrentIndex(index);
}

void EditorTabs::openDocument(const QString &filePath) {
  int index = addEditorTab(filePath.section(QLatin1Char('/'), -1));
  auto *editor = qobject_cast<Editor *>(widget(index));
  if (editor) {
    editor->setFilePath(filePath);
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
      editor->setMarkdown(QString::fromUtf8(file.readAll()));
    editor->document()->setModified(false);
  }
  setCurrentIndex(index);
}

bool EditorTabs::saveDocument(const QString &filePath) {
  Q_UNUSED(filePath);
  return true;
}

void EditorTabs::closeCurrentTab() {
  int index = currentIndex();
  if (index >= 0)
    removeTab(index);
}

int EditorTabs::addEditorTab(const QString &title) {
  auto *editor = new Editor(this);
  int index = addTab(editor, title.isEmpty() ? tr("Untitled") : title);

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

  return index;
}
