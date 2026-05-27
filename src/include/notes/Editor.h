#ifndef EDITOR_H
#define EDITOR_H

#include <QDateTime>
#include <QFocusEvent>
#include <QTextEdit>

class QMimeData;

class Editor;
enum class EditorCloseRequest { Normal, Exit };

class EditorDocument : public QWidget {
  Q_OBJECT

public:
  explicit EditorDocument(Editor *parent = nullptr);

  bool isEmpty() const;
  bool isModified() const;

signals:
  void modificationChanged(bool);

public slots:
  void setModified(bool);

private:
  bool m_modified = false;
};

class Editor : public QWidget {
  Q_OBJECT

public:
  explicit Editor(QWidget *parent = nullptr);
  ~Editor();

  EditorDocument *document() const;

  bool isBold() const;
  bool isItalic() const;
  bool isUnderline() const;
  bool isStrikethrough() const;
  bool isSuperscript() const;
  bool isSubscript() const;

  bool isUndoAvailable() const;
  bool isRedoAvailable() const;

  const QString &filePath() const;
  void setFilePath(const QString &path);

protected:
  void focusInEvent(QFocusEvent *event) override;

signals:
  void formattingChanged();
  void closed(EditorCloseRequest req);
  void undoAvailable(bool);
  void redoAvailable(bool);

public slots:
  void clear();
  void copy();
  void cut();
  void paste();
  void undo();
  void redo();

  void setBold(bool bold);
  void setItalic(bool italic);
  void setUnderline(bool underline);
  void setStrikethrough(bool strike);
  void setSuperscript(bool super);
  void setSubscript(bool sub);
  void wrapHeading(int level);
  void clearHeading();

  void insertOrderedList();
  void insertUnorderedList();
  void insertTable(int rows, int cols);
  void insertPlainText(const QString &text);
  void insertFromMimeData(const QMimeData *source);

  void removeSelectedText();

  void setMarkdown(const QString &text);

  bool save(const QString &path = QString());
  void close(EditorCloseRequest req = EditorCloseRequest::Normal);

private:
  bool hasFileChangedExternally() const;

  QString m_filePath;
  QDateTime m_fileLastModified;
  qint64 m_fileSize = 0;
};

#endif // EDITOR_H
