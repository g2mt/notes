#include "notes/MainWindow.h"
#include "notes/Editor.h"
#include "notes/EditorTabs.h"

#include <QAction>
#include <QApplication>
#include <QIcon>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>

#include <QCoreApplication>
#include <QPlainTextEdit>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  m_editorTabs = new EditorTabs(this);
  setCentralWidget(m_editorTabs);

  setupActions();
  setupMenuBar();
  setupToolBar();
  setupStatusBar();

  setWindowTitle(tr("Wordpad"));
  resize(800, 600);
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
  connect(m_saveAction, &QAction::triggered, this,
          [this]() { m_editorTabs->saveDocument(QString()); });

  m_quitAction =
      new QAction(QIcon::fromTheme("application-exit"), tr("&Quit"), this);
  m_quitAction->setShortcut(QKeySequence::Quit);
  connect(m_quitAction, &QAction::triggered, qApp, &QApplication::quit);

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
      editor->onBoldToggled(checked);
  });

  m_italicAction =
      new QAction(QIcon::fromTheme("format-text-italic"), tr("&Italic"), this);
  m_italicAction->setCheckable(true);
  m_italicAction->setShortcut(QKeySequence::Italic);
  connect(m_italicAction, &QAction::toggled, this, [this](bool checked) {
    if (auto *editor = m_editorTabs->currentEditor())
      editor->onItalicToggled(checked);
  });

  m_underlineAction = new QAction(QIcon::fromTheme("format-text-underline"),
                                  tr("&Underline"), this);
  m_underlineAction->setCheckable(true);
  m_underlineAction->setShortcut(QKeySequence::Underline);
  connect(m_underlineAction, &QAction::toggled, this, [this](bool checked) {
    if (auto *editor = m_editorTabs->currentEditor())
      editor->onUnderlineToggled(checked);
  });

  connectEditorSignals(m_editorTabs->currentEditor());
  connect(m_editorTabs, &EditorTabs::currentEditorChanged, this,
          &MainWindow::connectEditorSignals);
}

void MainWindow::setupMenuBar() {
  m_fileMenu = menuBar()->addMenu(tr("&File"));
  m_fileMenu->addAction(m_newAction);
  m_fileMenu->addAction(m_openAction);
  m_fileMenu->addAction(m_saveAction);
  m_fileMenu->addSeparator();
  m_fileMenu->addAction(m_quitAction);

  m_editMenu = menuBar()->addMenu(tr("&Edit"));
  m_editMenu->addAction(m_undoAction);
  m_editMenu->addAction(m_redoAction);
  m_editMenu->addSeparator();
  m_editMenu->addAction(m_boldAction);
  m_editMenu->addAction(m_italicAction);
  m_editMenu->addAction(m_underlineAction);

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
}

void MainWindow::setupStatusBar() { statusBar()->showMessage(tr("Ready")); }

void MainWindow::connectEditorSignals(Editor *editor) {
  if (m_previousEditor) {
    QObject::disconnect(m_undoAction, nullptr, m_previousEditor, nullptr);
    QObject::disconnect(m_redoAction, nullptr, m_previousEditor, nullptr);
    QObject::disconnect(m_previousEditor, nullptr, m_undoAction, nullptr);
    QObject::disconnect(m_previousEditor, nullptr, m_redoAction, nullptr);
  }

  m_previousEditor = editor;

  if (editor) {
    connect(editor, &QPlainTextEdit::undoAvailable, m_undoAction,
            &QAction::setEnabled);
    connect(editor, &QPlainTextEdit::redoAvailable, m_redoAction,
            &QAction::setEnabled);
    connect(m_undoAction, &QAction::triggered, editor,
            &QPlainTextEdit::undo);
    connect(m_redoAction, &QAction::triggered, editor,
            &QPlainTextEdit::redo);
    m_undoAction->setEnabled(editor->document()->isUndoAvailable());
    m_redoAction->setEnabled(editor->document()->isRedoAvailable());
  } else {
    m_undoAction->setEnabled(false);
    m_redoAction->setEnabled(false);
  }
}
