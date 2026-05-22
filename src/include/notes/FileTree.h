#ifndef FILETREE_H
#define FILETREE_H

#include <QDir>
#include <QTreeWidget>

class QTreeWidgetItem;

class FileTree : public QTreeWidget {
  Q_OBJECT

public:
  explicit FileTree(QWidget *parent = nullptr);

  void refresh();

signals:
  void fileActivated(const QString &filePath);

private:
  void populate();
  void addDirectory(const QDir &dir, QTreeWidgetItem *parent);
};

#endif // FILETREE_H
