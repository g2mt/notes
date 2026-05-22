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
  bool saveDocument(const QString &filePath);
  void closeCurrentTab();
  bool closeAll();

private:
  void setupNewTabButton();
  int addEditorTab(const QString &title = QString());

  QToolButton *m_addTabButton = nullptr;
  bool m_closingAll = false;
};

#endif // EDITORTABS_H
