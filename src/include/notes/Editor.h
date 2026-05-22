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

  void insertFromMimeData(const QMimeData *source) override;

signals:
  void formattingChanged();
};

#endif // EDITOR_H
