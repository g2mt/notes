#include "notes/MainWindow.h"
#include "notes/Editor.h"
#include "notes/EditorTabs.h"
#include "notes/FileTree.h"
#include "notes/SideBar.h"
#include "notes/TablePopup.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>
#include <QTextEdit>
#include <QToolBar>

MainWindow::MainWindow(const QString &dirPath, const QStringList &files,
                       QWidget *parent)
    : QMainWindow(parent) {
  m_workDir = dirPath.isEmpty() ? QDir::current() : QDir(dirPath);
  m_editorTabs = new EditorTabs(this);

  setupSplitter();
  loadRecentFolders();
  m_sidebar->setRecentFolders(m_recentFolders);
  setupActions();
  setupMenuBar();
  setupToolBar();
  setupStatusBar();

  for (const auto &file : files) {
    if (!file.isEmpty())
      m_editorTabs->openDocument(file);
  }

  setWindowTitle(tr("notes"));
  resize(800, 600);
  emit workDirChanged(m_workDir);
}

void MainWindow::setupActions() {
  m_newAction = new QAction(QIcon::fromTheme("document-new"), tr("&New"), this);
  m_newAction->setShortcut(QKeySequence::New);
  connect(m_newAction, &QAction::triggered, m_editorTabs,
          &EditorTabs::newDocument);

  m_openAction =
      new QAction(QIcon::fromTheme("document-open"), tr("&Open"), this);
  m_openAction->setShortcut(QKeySequence::Open);
  connect(m_openAction, &QAction::triggered, this,
          [this]() { m_editorTabs->openDocument(QString()); });

  m_saveAction =
      new QAction(QIcon::fromTheme("document-save"), tr("&Save"), this);
  m_saveAction->setShortcut(QKeySequence::Save);
  connect(m_saveAction, &QAction::triggered, this, [this]() {
    if (auto *editor = m_editorTabs->currentEditor())
      editor->save(editor->filePath());
  });

  m_saveAsAction =
      new QAction(QIcon::fromTheme("document-save-as"), tr("Save &as"), this);
  m_saveAsAction->setShortcut(QKeySequence::SaveAs);
  connect(m_saveAsAction, &QAction::triggered, this, [this]() {
    if (auto *editor = m_editorTabs->currentEditor())
      editor->save(nullptr);
  });

  m_closeTabAction = new QAction(tr("&Close Tab"), this);
  m_closeTabAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_W));
  connect(m_closeTabAction, &QAction::triggered, m_editorTabs,
          &EditorTabs::closeCurrentTab);

  m_closeAllTabsAction = new QAction(tr("Close &All Tabs"), this);
  m_closeAllTabsAction->setShortcut(
      QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_W));
  connect(m_closeAllTabsAction, &QAction::triggered, m_editorTabs,
          [this]() { m_editorTabs->closeAll(EditorCloseRequest::Normal); });

  m_quitAction =
      new QAction(QIcon::fromTheme("application-exit"), tr("&Quit"), this);
  m_quitAction->setShortcut(QKeySequence::Quit);
  connect(m_quitAction, &QAction::triggered, this, &MainWindow::close);

  m_undoAction = new QAction(QIcon::fromTheme("edit-undo"), tr("&Undo"), this);
  m_undoAction->setShortcut(QKeySequence::Undo);
  m_undoAction->setEnabled(false);

  m_redoAction = new QAction(QIcon::fromTheme("edit-redo"), tr("&Redo"), this);
  m_redoAction->setShortcut(QKeySequence::Redo);
  m_redoAction->setEnabled(false);

  m_boldAction =
      new QAction(QIcon::fromTheme("format-text-bold"), tr("&Bold"), this);
  m_boldAction->setCheckable(true);
  m_boldAction->setShortcut(QKeySequence::Bold);
  connect(m_boldAction, &QAction::toggled, this, [this](bool checked) {
    if (auto *editor = m_editorTabs->currentEditor())
      editor->setBold(checked);
  });

  m_italicAction =
      new QAction(QIcon::fromTheme("format-text-italic"), tr("&Italic"), this);
  m_italicAction->setCheckable(true);
  m_italicAction->setShortcut(QKeySequence::Italic);
  connect(m_italicAction, &QAction::toggled, this, [this](bool checked) {
    if (auto *editor = m_editorTabs->currentEditor())
      editor->setItalic(checked);
  });

  m_underlineAction = new QAction(QIcon::fromTheme("format-text-underline"),
                                  tr("&Underline"), this);
  m_underlineAction->setCheckable(true);
  m_underlineAction->setShortcut(QKeySequence::Underline);
  connect(m_underlineAction, &QAction::toggled, this, [this](bool checked) {
    if (auto *editor = m_editorTabs->currentEditor())
      editor->setUnderline(checked);
  });

  m_strikethroughAction =
      new QAction(QIcon::fromTheme("format-text-strikethrough"),
                  tr("&Strikethrough"), this);
  m_strikethroughAction->setCheckable(true);
  connect(m_strikethroughAction, &QAction::toggled, this, [this](bool checked) {
    if (auto *editor = m_editorTabs->currentEditor())
      editor->setStrikethrough(checked);
  });

  m_superscriptAction = new QAction(QIcon::fromTheme("format-text-superscript"),
                                    tr("&Superscript"), this);
  m_superscriptAction->setCheckable(true);
  connect(m_superscriptAction, &QAction::toggled, this, [this](bool checked) {
    if (auto *editor = m_editorTabs->currentEditor())
      editor->setSuperscript(checked);
  });

  m_subscriptAction = new QAction(QIcon::fromTheme("format-text-subscript"),
                                  tr("&Subscript"), this);
  m_subscriptAction->setCheckable(true);
  connect(m_subscriptAction, &QAction::toggled, this, [this](bool checked) {
    if (auto *editor = m_editorTabs->currentEditor())
      editor->setSubscript(checked);
  });

  m_headingMenu = new QMenu(tr("&Heading"), this);

  m_normalTextAction = new QAction(tr("&Normal Text"), this);
  m_normalTextAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
  connect(m_normalTextAction, &QAction::triggered, this, [this]() {
    if (auto *editor = m_editorTabs->currentEditor())
      editor->clearHeading();
  });
  m_headingMenu->addAction(m_normalTextAction);
  m_headingMenu->addSeparator();

  for (int i = 0; i < 6; ++i) {
    int level = i + 1;
    m_headingActions[i] =
        new QAction(QStringLiteral("Heading &%1").arg(level), this);
    m_headingActions[i]->setShortcut(
        QKeySequence(QStringLiteral("Ctrl+%1").arg(level)));
    connect(m_headingActions[i], &QAction::triggered, this, [this, level]() {
      if (auto *editor = m_editorTabs->currentEditor())
        editor->wrapHeading(level);
    });
    m_headingMenu->addAction(m_headingActions[i]);
  }

  m_headingAction = new QAction(QIcon::fromTheme("format-text-heading"),
                                tr("&Heading"), this);
  m_headingAction->setMenu(m_headingMenu);

  m_unorderedListAction = new QAction(QIcon::fromTheme("format-list-unordered"),
                                      tr("&Unordered List"), this);
  connect(m_unorderedListAction, &QAction::triggered, this, [this]() {
    if (auto *editor = m_editorTabs->currentEditor())
      editor->insertUnorderedList();
  });

  m_tableAction =
      new QAction(QIcon::fromTheme("insert-table"), tr("&Table"), this);
  connect(m_tableAction, &QAction::triggered, this, [this]() {
    auto *editor = m_editorTabs->currentEditor();
    if (!editor)
      return;

    auto *popup = new TablePopup(this);
    if (auto *btn = m_toolbar->widgetForAction(m_tableAction))
      popup->move(btn->mapToGlobal(QPoint(0, btn->height())));
    connect(popup, &TablePopup::accepted, editor, &Editor::insertTable);
    popup->show();
  });

  m_orderedListAction = new QAction(QIcon::fromTheme("format-list-ordered"),
                                    tr("&Ordered List"), this);
  connect(m_orderedListAction, &QAction::triggered, this, [this]() {
    if (auto *editor = m_editorTabs->currentEditor())
      editor->insertOrderedList();
  });

  m_cutAction = new QAction(QIcon::fromTheme("edit-cut"), tr("Cu&t"), this);
  m_cutAction->setShortcut(QKeySequence::Cut);

  m_copyAction = new QAction(QIcon::fromTheme("edit-copy"), tr("&Copy"), this);
  m_copyAction->setShortcut(QKeySequence::Copy);

  m_pasteAction =
      new QAction(QIcon::fromTheme("edit-paste"), tr("&Paste"), this);
  m_pasteAction->setShortcut(QKeySequence::Paste);

  m_pastePlainAction = new QAction(tr("Paste as Plain &Text"), this);
  m_pastePlainAction->setShortcut(
      QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_V));

  m_deleteAction =
      new QAction(QIcon::fromTheme("edit-delete"), tr("&Delete"), this);
  m_deleteAction->setShortcut(QKeySequence::Delete);

  connectEditorSignals(m_editorTabs->currentEditor());
  connect(m_editorTabs, &EditorTabs::currentEditorChanged, this,
          &MainWindow::connectEditorSignals);
}

void MainWindow::setupMenuBar() {
  m_fileMenu = menuBar()->addMenu(tr("&File"));
  m_fileMenu->addAction(m_newAction);
  m_fileMenu->addAction(m_openAction);
  m_fileMenu->addAction(m_saveAction);
  m_fileMenu->addAction(m_saveAsAction);
  m_fileMenu->addSeparator();
  m_fileMenu->addAction(m_closeTabAction);
  m_fileMenu->addAction(m_closeAllTabsAction);
  m_fileMenu->addAction(m_quitAction);

  m_editMenu = menuBar()->addMenu(tr("&Edit"));
  m_editMenu->addAction(m_undoAction);
  m_editMenu->addAction(m_redoAction);
  m_editMenu->addSeparator();
  m_editMenu->addAction(m_cutAction);
  m_editMenu->addAction(m_copyAction);
  m_editMenu->addAction(m_pasteAction);
  m_editMenu->addAction(m_pastePlainAction);
  m_editMenu->addAction(m_deleteAction);
  m_editMenu->addSeparator();
  m_editMenu->addAction(m_boldAction);
  m_editMenu->addAction(m_italicAction);
  m_editMenu->addAction(m_underlineAction);
  m_editMenu->addAction(m_strikethroughAction);
  m_editMenu->addAction(m_superscriptAction);
  m_editMenu->addAction(m_subscriptAction);
  m_editMenu->addSeparator();
  m_editMenu->addMenu(m_headingMenu);

  m_viewMenu = menuBar()->addMenu(tr("&View"));
}

void MainWindow::setupToolBar() {
  m_toolbar = addToolBar(tr("Main"));

  m_toolbar->addAction(m_newAction);
  m_toolbar->addAction(m_openAction);
  m_toolbar->addAction(m_saveAction);

  m_toolbar->addSeparator();

  m_toolbar->addAction(m_undoAction);
  m_toolbar->addAction(m_redoAction);

  m_toolbar->addSeparator();

  m_toolbar->addAction(m_boldAction);
  m_toolbar->addAction(m_italicAction);
  m_toolbar->addAction(m_underlineAction);
  m_toolbar->addAction(m_strikethroughAction);
  m_toolbar->addAction(m_superscriptAction);
  m_toolbar->addAction(m_subscriptAction);

  m_toolbar->addSeparator();

  m_toolbar->addAction(m_unorderedListAction);
  m_toolbar->addAction(m_orderedListAction);
  m_toolbar->addAction(m_headingAction);

  m_toolbar->addSeparator();

  m_toolbar->addAction(m_tableAction);
}

void MainWindow::setupStatusBar() { statusBar()->showMessage(tr("Ready")); }

void MainWindow::setupSplitter() {
  m_splitter = new QSplitter(Qt::Horizontal, this);
  m_sidebar = new SideBar(m_splitter);
  m_splitter->addWidget(m_sidebar);
  m_splitter->addWidget(m_editorTabs);
  m_splitter->setStretchFactor(0, 0);
  m_splitter->setStretchFactor(1, 1);
  setCentralWidget(m_splitter);

  connect(m_sidebar->fileTree(), &FileTree::fileActivated, this,
          [this](const QString &path) { m_editorTabs->openDocument(path); });
  connect(this, &MainWindow::workDirChanged, this, [this](const QDir &dir) {
    const QString path = dir.absolutePath();
    emit m_sidebar->folderOpened(path);
  });
  connect(m_sidebar, &SideBar::folderOpened, this, [this](const QString &path) {
    m_recentFolders.removeAll(path);
    m_recentFolders.prepend(path);
    if (m_recentFolders.size() > 10)
      m_recentFolders = m_recentFolders.mid(0, 10);
    m_sidebar->setRecentFolders(m_recentFolders);
    saveRecentFolders();
  });
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (m_editorTabs->closeAll(EditorCloseRequest::Exit))
    event->ignore();
  else
    event->accept();
}

void MainWindow::connectEditorSignals(Editor *editor) {
  foreach (auto var, m_editor.connections) {
    QObject::disconnect(var);
  }
  m_editor.connections.clear();

  m_editor.e = editor;

  if (editor) {
    // File menu
    m_editor.connections.append(
        connect(editor, &QObject::destroyed, this, [this]() {
          m_saveAction->setEnabled(false);
          m_saveAsAction->setEnabled(false);
        }));
    m_saveAction->setEnabled(true);
    m_saveAsAction->setEnabled(true);

    // Edit menu
    m_editor.connections = {
        connect(editor, &Editor::undoAvailable, m_undoAction,
                &QAction::setEnabled),
        connect(editor, &Editor::redoAvailable, m_redoAction,
                &QAction::setEnabled),

        connect(m_undoAction, &QAction::triggered, editor, &Editor::undo),
        connect(m_redoAction, &QAction::triggered, editor, &Editor::redo),
        connect(m_cutAction, &QAction::triggered, editor, &Editor::cut),
        connect(m_copyAction, &QAction::triggered, editor, &Editor::copy),
        connect(m_pasteAction, &QAction::triggered, editor, &Editor::paste),
        connect(m_pastePlainAction, &QAction::triggered, editor,
                [editor]() {
                  editor->insertPlainText(QApplication::clipboard()->text());
                }),

        connect(m_deleteAction, &QAction::triggered, editor,
                [editor]() { editor->removeSelectedText(); }),
    };
    m_undoAction->setEnabled(editor->isUndoAvailable());
    m_redoAction->setEnabled(editor->isRedoAvailable());
  } else {
    // File
    m_saveAction->setEnabled(false);
    m_saveAsAction->setEnabled(false);

    // Edit
    m_undoAction->setEnabled(false);
    m_redoAction->setEnabled(false);
  }
}

const QDir &MainWindow::workDir() const { return m_workDir; }

void MainWindow::setWorkDir(const QDir &dir) {
  if (m_workDir == dir)
    return;
  m_workDir = dir;
  emit workDirChanged(dir);
}

void MainWindow::loadRecentFolders() {
  QSettings settings;
  m_recentFolders = settings.value("recentFolders").toStringList();
}

void MainWindow::saveRecentFolders() {
  QSettings settings;
  settings.setValue("recentFolders", m_recentFolders);
}
