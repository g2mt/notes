#ifndef EDITORTABS_H
#define EDITORTABS_H

#include <QString>
#include <QTabWidget>

class Editor;
class QToolButton;

class EditorTabs : public QTabWidget {
  Q_OBJECT

public:
  explicit EditorTabs(QWidget *parent = nullptr);

  Editor *currentEditor() const;

signals:
  void currentEditorChanged(Editor *editor);

public slots:
  void newDocument();
  void openDocument(const QString &filePath);
  void closeCurrentTab();
  bool closeAll();

private:
  void setupNewTabButton();
  Editor *addEditorTab(const QString &title, int &index);

  QToolButton *m_addTabButton = nullptr;
  bool m_closingAll = false;
};

#endif // EDITORTABS_H
