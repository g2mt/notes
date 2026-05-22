#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QMenu;
class QToolBar;
class EditorTabs;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

private:
  void setupActions();
  void setupMenuBar();
  void setupToolBar();
  void setupStatusBar();

  EditorTabs *m_editorTabs = nullptr;

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
};

#endif // MAINWINDOW_H
