#include "notes/FileTree.h"

#include <QDir>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QTreeWidgetItem>

FileTree::FileTree(QWidget *parent) : QTreeWidget(parent) {
  setHeaderHidden(true);

  connect(this, &QTreeWidget::itemDoubleClicked, this,
          [this](QTreeWidgetItem *item, int) {
            if (!item)
              return;
            const auto path = item->data(0, Qt::UserRole).toString();
            if (!path.isEmpty())
              emit fileActivated(path);
          });

  populate(QDir("."));
}

void FileTree::populate(const QDir &dir) {
  clear();
  QFileIconProvider iconProvider;

  addDirectory(dir, nullptr);
}

void FileTree::addDirectory(const QDir &dir, QTreeWidgetItem *parent) {
  QFileIconProvider iconProvider;

  const auto entries =
      dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
                        QDir::DirsFirst | QDir::Name);

  for (const auto &entry : entries) {
    QTreeWidgetItem *item;
    if (parent)
      item = new QTreeWidgetItem(parent);
    else
      item = new QTreeWidgetItem(this);

    item->setIcon(0, iconProvider.icon(entry));
    item->setText(0, entry.fileName());
    item->setData(0, Qt::UserRole, entry.absoluteFilePath());

    if (entry.isDir())
      addDirectory(QDir(entry.absoluteFilePath()), item);
  }
}
