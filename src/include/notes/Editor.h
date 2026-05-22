#ifndef EDITOR_H
#define EDITOR_H

#include <QPlainTextEdit>

class Editor : public QPlainTextEdit {
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

public slots:
  void onBoldToggled(bool checked);
  void onItalicToggled(bool checked);
  void onUnderlineToggled(bool checked);

signals:
  void formattingChanged();

private:
  void mergeFormatOnSelection(const QTextCharFormat &format);
};

#endif // EDITOR_H
