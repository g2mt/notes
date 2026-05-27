#ifndef EDITOR_H
#define EDITOR_H

#include <QDateTime>
#include <QFocusEvent>
#include <QObject>
#include <QWidget>

class QWebChannel;
class QWebEngineView;

class ProseBridge : public QObject {
  Q_OBJECT

public:
  explicit ProseBridge(QObject *parent = nullptr);

  bool isBold() const;
  bool isItalic() const;
  bool isUnderline() const;
  bool isStrikethrough() const;
  bool isSuperscript() const;
  bool isSubscript() const;
  bool isModified() const;
  bool isUndoAvail() const;
  bool isRedoAvail() const;
  void setModified(bool modified);

signals:
  void formattingChanged();
  void modificationChanged(bool modified);
  void undoAvailable(bool available);
  void redoAvailable(bool available);

public slots:
  void notifyFormattingChanged(bool bold, bool italic, bool underline,
                           bool strikethrough, bool superscript,
                           bool subscript);
  void notifyModificationChanged(bool modified);
  void notifyUndoAvailable(bool available);
  void notifyRedoAvailable(bool available);

private:
  bool m_bold = false;
  bool m_italic = false;
  bool m_underline = false;
  bool m_strikethrough = false;
  bool m_superscript = false;
  bool m_subscript = false;
  bool m_modified = false;
  bool m_undoAvail = false;
  bool m_redoAvail = false;
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

  bool isBold() const;
  bool isItalic() const;
  bool isUnderline() const;

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
  void formattingChanged();
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
  ProseBridge *m_bridge = nullptr;

  QString m_filePath;
  QDateTime m_fileLastModified;
  qint64 m_fileSize = 0;
};

#endif // EDITOR_H
