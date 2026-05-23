#ifndef EDITOR_H
#define EDITOR_H

#include <QTextEdit>

class QMimeData;

class Editor : public QTextEdit {
  Q_OBJECT

public:
  explicit Editor(QWidget *parent = nullptr);
  ~Editor();

  void setBold(bool bold);
  void setItalic(bool italic);
  void setUnderline(bool underline);

  bool isBold() const;
  bool isItalic() const;
  bool isUnderline() const;

  void wrapHeading(int level);

  void insertOrderedList();
  void insertUnorderedList();

  void insertTable(int rows, int cols);

  void insertFromMimeData(const QMimeData *source) override;

  const QString &filePath() const;
  void setFilePath(const QString &path);

  bool save(const QString *path = nullptr);
  void close(bool canCancel = true);

signals:
  void formattingChanged();
  void closed();

private:
  QString m_filePath;
};

#endif // EDITOR_H
