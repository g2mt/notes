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
  connect(this, &SideBar::folderOpened, m_fileTree, &FileTree::populate);
  layout->addWidget(m_fileTree, 1);

  auto *bottomBar = new QHBoxLayout();
  bottomBar->setContentsMargins(4, 4, 4, 4);

  m_folderCombo = new QComboBox(this);
  m_folderCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_folderCombo->setToolTip(tr("Switch Folder"));
  connect(
      m_folderCombo, &QComboBox::currentIndexChanged, this, [this](int index) {
        if (index < 0)
          return;
        QString path = m_folderCombo->itemData(index, Qt::UserRole).toString();
        emit folderOpened(path);
      });
  bottomBar->addWidget(m_folderCombo);

  auto *openBtn = new QPushButton(this);
  openBtn->setIcon(QIcon::fromTheme("document-open"));
  openBtn->setToolTip(tr("Open Folder"));
  openBtn->setFlat(true);
  connect(openBtn, &QPushButton::clicked, this, [this]() {
    QString path = QFileDialog::getExistingDirectory(this, tr("Open Folder"));
    if (path.isEmpty())
      return;
    emit folderOpened(path);
  });
  bottomBar->addWidget(openBtn);

  layout->addLayout(bottomBar);
}

FileTree *SideBar::fileTree() const { return m_fileTree; }

void SideBar::setRecentFolders(const QList<QString> &folders) {
  QSignalBlocker blocker(m_folderCombo); // prevents double emission of
                                         // QComboBox::currentIndexChanged
  m_folderCombo->clear();
  for (const auto &dir : folders) {
    m_folderCombo->addItem(QDir(dir).dirName());
    m_folderCombo->setItemData(m_folderCombo->count() - 1, dir, Qt::UserRole);
  }
  m_folderCombo->setCurrentIndex(0);
}
