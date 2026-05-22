#include "notes/EditorTabs.h"
#include "notes/Editor.h"

EditorTabs::EditorTabs(QWidget *parent)
    : QTabWidget(parent)
{
    setTabsClosable(true);
    setDocumentMode(true);
}

EditorTabs::~EditorTabs() = default;

Editor *EditorTabs::currentEditor() const
{
    return qobject_cast<Editor *>(currentWidget());
}

void EditorTabs::newDocument()
{
    int index = addEditorTab(tr("Untitled"));
    setCurrentIndex(index);
}

void EditorTabs::openDocument(const QString &filePath)
{
    int index = addEditorTab(filePath.section(QLatin1Char('/'), -1));
    setCurrentIndex(index);
}

bool EditorTabs::saveDocument(const QString &filePath)
{
    Q_UNUSED(filePath);
    return true;
}

int EditorTabs::addEditorTab(const QString &title)
{
    auto *editor = new Editor(this);
    return addTab(editor, title.isEmpty() ? tr("Untitled") : title);
}
