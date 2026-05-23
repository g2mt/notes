#include "notes/FileTree.h"

FileTree::FileTree(QWidget *parent) : QTreeView(parent) {
  m_model = new QFileSystemModel(this);
  m_model->setRootPath(QDir::currentPath());

  setModel(m_model);
  setRootIndex(m_model->index(QDir::currentPath()));

  connect(this, &QTreeView::doubleClicked, this,
          [this](const QModelIndex &index) {
            if (!index.isValid())
              return;
            const auto path = m_model->filePath(index);
            if (!path.isEmpty())
              emit fileActivated(path);
          });
}

void FileTree::populate(const QString &path) {
  m_model->setRootPath(path);
  setRootIndex(m_model->index(path));
}
