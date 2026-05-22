#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QToolBar;
class TextEditor;
class DocumentManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupActions();
    void setupToolBar();
    void setupStatusBar();

    TextEditor *m_editor = nullptr;
    DocumentManager *m_documentManager = nullptr;
    QToolBar *m_toolbar = nullptr;
    QAction *m_newAction = nullptr;
    QAction *m_openAction = nullptr;
    QAction *m_saveAction = nullptr;
    QAction *m_boldAction = nullptr;
    QAction *m_italicAction = nullptr;
    QAction *m_underlineAction = nullptr;
};

#endif // MAINWINDOW_H
