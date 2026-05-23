#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>

class QComboBox;
class FileTree;

class SideBar : public QWidget {
  Q_OBJECT

public:
  explicit SideBar(QWidget *parent = nullptr);

  FileTree *fileTree() const;
  void setSelectedFolder(const QString &dir);

private:
  void onOpenFolder();
  void onFolderSelected(int index);

  FileTree *m_fileTree = nullptr;
  QComboBox *m_folderCombo = nullptr;
};

#endif // SIDEBAR_H
