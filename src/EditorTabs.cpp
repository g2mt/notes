#include "notes/EditorTabs.h"
#include "notes/Editor.h"

#include <QIcon>
#include <QTabBar>
#include <QToolButton>

EditorTabs::EditorTabs(QWidget *parent) : QTabWidget(parent) {
  setTabsClosable(true);
  setMovable(true);
  setDocumentMode(true);
  setupNewTabButton();
  newDocument();
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
  setCurrentIndex(index);
}

bool EditorTabs::saveDocument(const QString &filePath) {
  Q_UNUSED(filePath);
  return true;
}

int EditorTabs::addEditorTab(const QString &title) {
  auto *editor = new Editor(this);
  return addTab(editor, title.isEmpty() ? tr("Untitled") : title);
}
