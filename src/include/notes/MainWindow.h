#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
  Editor *m_previousEditor = nullptr;

  QMenu *m_fileMenu = nullptr;
  QMenu *m_editMenu = nullptr;
  QMenu *m_viewMenu = nullptr;

  QToolBar *m_toolbar = nullptr;
  QAction *m_newAction = nullptr;
  QAction *m_openAction = nullptr;
  QAction *m_saveAction = nullptr;
  QAction *m_quitAction = nullptr;
  QAction *m_undoAction = nullptr;
  QAction *m_redoAction = nullptr;
  QAction *m_boldAction = nullptr;
  QAction *m_italicAction = nullptr;
  QAction *m_underlineAction = nullptr;

  QAction *m_headingAction = nullptr;
  QAction *m_headingActions[6] = {};
  QMenu *m_headingMenu = nullptr;

  QAction *m_orderedListAction = nullptr;
  QAction *m_unorderedListAction = nullptr;
};

#endif // MAINWINDOW_H
