#ifndef EDITOR_H
#define EDITOR_H

#include <QDateTime>
#include <QFocusEvent>
#include <QObject>
#include <QWebEnginePage>
#include <QWidget>

class QWebChannel;
class QWebEngineView;
class EditorBridge;

class EditorPage : public QWebEnginePage {
  Q_OBJECT
public:
  using QWebEnginePage::QWebEnginePage;

protected:
  void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level,
                                const QString &message, int lineNumber,
                                const QString &sourceID) override;
};

enum class EditorCloseRequest { Normal, Exit };

class Editor : public QWidget {
  Q_OBJECT

public:
  explicit Editor(QWidget *parent = nullptr);
  ~Editor();

  void setBold(bool bold);
  void setItalic(bool italic);
  void setUnderline(bool underline);
  void setStrikethrough(bool strike);
  void setSuperscript(bool super);
  void setSubscript(bool sub);

  void setMarkdown(const QString &markdown);

  void wrapHeading(int level);
  void clearHeading();

  void insertOrderedList();
  void insertUnorderedList();
  void insertTable(int rows, int cols);

  bool isModified() const;
  void setModified(bool modified);
  bool isEmpty() const;
  bool isUndoAvailable() const;
  bool isRedoAvailable() const;

  void undo();
  void redo();
  void cut();
  void copy();
  void paste();
  void insertPlainText(const QString &text);
  void deleteSelection();

  const QString &filePath() const;
  void setFilePath(const QString &path);

  bool save(const QString &path = QString());
  void close(EditorCloseRequest req = EditorCloseRequest::Normal);

signals:
  void formattingChanged(bool bold, bool italic, bool underline,
                         bool strikethrough, bool superscript, bool subscript);
  void closed(EditorCloseRequest req);
  void modificationChanged(bool modified);
  void undoAvailable(bool available);
  void redoAvailable(bool available);

protected:
  void focusInEvent(QFocusEvent *event) override;

private:
  bool hasFileChangedExternally() const;
  void runJs(const QString &js);
  QString getMarkdownSync();

  QWebEngineView *m_webView = nullptr;
  QWebChannel *m_channel = nullptr;
  EditorBridge *m_bridge = nullptr;

  QString m_filePath;
  QDateTime m_fileLastModified;
  qint64 m_fileSize = 0;
};

#endif // EDITOR_H
