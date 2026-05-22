#include "notes/MainWindow.h"
#include "notes/Editor.h"
#include "notes/EditorTabs.h"

#include <QAction>
#include <QIcon>
#include <QStatusBar>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  m_editorTabs = new EditorTabs(this);
  setCentralWidget(m_editorTabs);

  setupActions();
  setupToolBar();
  setupStatusBar();

  setWindowTitle(tr("Wordpad"));
  resize(800, 600);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupActions() {
  m_newAction = new QAction(QIcon::fromTheme("document-new"), tr("New"), this);
  m_newAction->setShortcut(QKeySequence::New);

  m_openAction =
      new QAction(QIcon::fromTheme("document-open"), tr("Open"), this);
  m_openAction->setShortcut(QKeySequence::Open);

  m_saveAction =
      new QAction(QIcon::fromTheme("document-save"), tr("Save"), this);
  m_saveAction->setShortcut(QKeySequence::Save);

  m_boldAction =
      new QAction(QIcon::fromTheme("format-text-bold"), tr("Bold"), this);
  m_boldAction->setCheckable(true);
  m_boldAction->setShortcut(QKeySequence::Bold);

  m_italicAction =
      new QAction(QIcon::fromTheme("format-text-italic"), tr("Italic"), this);
  m_italicAction->setCheckable(true);
  m_italicAction->setShortcut(QKeySequence::Italic);

  m_underlineAction = new QAction(QIcon::fromTheme("format-text-underline"),
                                  tr("Underline"), this);
  m_underlineAction->setCheckable(true);
  m_underlineAction->setShortcut(QKeySequence::Underline);

  connect(m_newAction, &QAction::triggered, m_editorTabs,
          &EditorTabs::newDocument);
  connect(m_openAction, &QAction::triggered, this,
          [this]() { m_editorTabs->openDocument(QString()); });
  connect(m_saveAction, &QAction::triggered, this,
          [this]() { m_editorTabs->saveDocument(QString()); });
  connect(m_boldAction, &QAction::toggled, this, [this](bool checked) {
    if (auto *editor = m_editorTabs->currentEditor())
      editor->onBoldToggled(checked);
  });
  connect(m_italicAction, &QAction::toggled, this, [this](bool checked) {
    if (auto *editor = m_editorTabs->currentEditor())
      editor->onItalicToggled(checked);
  });
  connect(m_underlineAction, &QAction::toggled, this, [this](bool checked) {
    if (auto *editor = m_editorTabs->currentEditor())
      editor->onUnderlineToggled(checked);
  });
}

void MainWindow::setupToolBar() {
  m_toolbar = addToolBar(tr("Main"));

  m_toolbar->addAction(m_newAction);
  m_toolbar->addAction(m_openAction);
  m_toolbar->addAction(m_saveAction);

  m_toolbar->addSeparator();

  m_toolbar->addAction(m_boldAction);
  m_toolbar->addAction(m_italicAction);
  m_toolbar->addAction(m_underlineAction);
}

void MainWindow::setupStatusBar() { statusBar()->showMessage(tr("Ready")); }
