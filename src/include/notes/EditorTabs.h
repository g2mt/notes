#ifndef EDITORTABS_H
#define EDITORTABS_H

#include <QTabWidget>
#include <QString>

class Editor;

class EditorTabs : public QTabWidget
{
    Q_OBJECT

public:
    explicit EditorTabs(QWidget *parent = nullptr);
    ~EditorTabs();

    Editor *currentEditor() const;

public slots:
    void newDocument();
    void openDocument(const QString &filePath);
    bool saveDocument(const QString &filePath);

private:
    int addEditorTab(const QString &title = QString());
};

#endif // EDITORTABS_H
