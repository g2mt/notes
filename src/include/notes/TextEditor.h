#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QPlainTextEdit>

class TextEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit TextEditor(QWidget *parent = nullptr);
    ~TextEditor();

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

#endif // TEXTEDITOR_H
