#include "notes/SideBar.h"
#include "notes/FileTree.h"

#include <QComboBox>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QVBoxLayout>

static int findDirInCombo(QComboBox *combo, const QString &dir) {
  for (int i = 0; i < combo->count(); ++i) {
    if (combo->itemData(i, Qt::UserRole).toString() == dir)
      return i;
  }
  return -1;
}

SideBar::SideBar(QWidget *parent) : QWidget(parent) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  m_fileTree = new FileTree(this);
  layout->addWidget(m_fileTree, 1);

  auto *bottomBar = new QHBoxLayout();
  bottomBar->setContentsMargins(4, 4, 4, 4);

  m_folderCombo = new QComboBox(this);
  m_folderCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_folderCombo->setToolTip(tr("Switch Folder"));
  connect(m_folderCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &SideBar::onFolderSelected);
  bottomBar->addWidget(m_folderCombo);

  auto *openBtn = new QPushButton(this);
  openBtn->setIcon(QIcon::fromTheme("document-open"));
  openBtn->setToolTip(tr("Open Folder"));
  openBtn->setFlat(true);
  connect(openBtn, &QPushButton::clicked, this, &SideBar::onOpenFolder);
  bottomBar->addWidget(openBtn);

  layout->addLayout(bottomBar);
}

FileTree *SideBar::fileTree() const { return m_fileTree; }

void SideBar::setSelectedFolder(const QString &dir) {
  int idx = findDirInCombo(m_folderCombo, dir);
  if (idx == -1) {
    idx = m_folderCombo->count();
    QSignalBlocker blocker(m_folderCombo);
    m_folderCombo->addItem(QDir(dir).dirName());
    m_folderCombo->setItemData(idx, dir, Qt::UserRole);
  }
  m_folderCombo->setCurrentIndex(idx);
}

void SideBar::onOpenFolder() {
  QString dir = QFileDialog::getExistingDirectory(this, tr("Open Folder"));
  if (dir.isEmpty())
    return;

  setSelectedFolder(dir);
}

void SideBar::onFolderSelected(int index) {
  if (index < 0)
    return;
  m_fileTree->populate(m_folderCombo->itemData(index, Qt::UserRole).toString());
}
