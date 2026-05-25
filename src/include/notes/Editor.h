#ifndef EDITOR_H
#define EDITOR_H

#include <QDateTime>
#include <QFocusEvent>
#include <QTextEdit>

class QMimeData;

enum class EditorCloseRequest { Normal, Exit };

class Editor : public QTextEdit {
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

  void wrapHeading(int level);
  void clearHeading();

  void insertOrderedList();
  void insertUnorderedList();

  void insertTable(int rows, int cols);

  void insertFromMimeData(const QMimeData *source) override;
  void focusInEvent(QFocusEvent *event) override;

  const QString &filePath() const;
  void setFilePath(const QString &path);

  bool save(const QString &path = QString());
  void close(EditorCloseRequest req = EditorCloseRequest::Normal);

signals:
  void formattingChanged();
  void closed(EditorCloseRequest req);

private:
  bool hasFileChangedExternally() const;

  QString m_filePath;
  QDateTime m_fileLastModified;
  qint64 m_fileSize = 0;
};

#endif // EDITOR_H
