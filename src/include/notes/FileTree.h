#ifndef FILETREE_H
#define FILETREE_H

#include <QDir>
#include <QFileSystemModel>
#include <QTreeView>

class FileTree : public QTreeView {
  Q_OBJECT

public:
  explicit FileTree(QWidget *parent = nullptr);

  void populate(const QDir &dir);

signals:
  void fileActivated(const QString &filePath);

private:
  QFileSystemModel *m_model;
};

#endif // FILETREE_H
