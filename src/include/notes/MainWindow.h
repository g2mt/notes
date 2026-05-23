#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QList>
#include <QMainWindow>
#include <QMetaObject>

class QAction;
class QMenu;
class QSplitter;
class QToolBar;
class Editor;
class EditorTabs;
class FileTree;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

protected:
  void closeEvent(QCloseEvent *event) override;

private:
  void setupActions();
  void setupMenuBar();
  void setupToolBar();
  void setupStatusBar();
  void setupSplitter();
  void connectEditorSignals(Editor *editor);

  QSplitter *m_splitter = nullptr;
  FileTree *m_fileTree = nullptr;
  EditorTabs *m_editorTabs = nullptr;
  struct {
    Editor *e = nullptr;
    QList<QMetaObject::Connection> connections;
  } m_editor;

  QMenu *m_fileMenu = nullptr;
  QMenu *m_editMenu = nullptr;
  QMenu *m_viewMenu = nullptr;

  QToolBar *m_toolbar = nullptr;
  QAction *m_newAction = nullptr;
  QAction *m_openAction = nullptr;
  QAction *m_saveAction = nullptr;
  QAction *m_saveAsAction = nullptr;
  QAction *m_quitAction = nullptr;
  QAction *m_undoAction = nullptr;
  QAction *m_redoAction = nullptr;
  QAction *m_cutAction = nullptr;
  QAction *m_copyAction = nullptr;
  QAction *m_pasteAction = nullptr;
  QAction *m_pastePlainAction = nullptr;
  QAction *m_deleteAction = nullptr;
  QAction *m_boldAction = nullptr;
  QAction *m_italicAction = nullptr;
  QAction *m_underlineAction = nullptr;

  QAction *m_headingAction = nullptr;
  QAction *m_headingActions[6] = {};
  QMenu *m_headingMenu = nullptr;

  QAction *m_orderedListAction = nullptr;
  QAction *m_unorderedListAction = nullptr;
  QAction *m_closeTabAction = nullptr;
};

#endif // MAINWINDOW_H
