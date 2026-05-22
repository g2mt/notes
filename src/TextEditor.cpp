#include "notes/TextEditor.h"

TextEditor::TextEditor(QWidget *parent)
    : QPlainTextEdit(parent)
{
}

TextEditor::~TextEditor() = default;

void TextEditor::setBold(bool bold)
{
    Q_UNUSED(bold);
}

void TextEditor::setItalic(bool italic)
{
    Q_UNUSED(italic);
}

void TextEditor::setUnderline(bool underline)
{
    Q_UNUSED(underline);
}

bool TextEditor::isBold() const
{
    return false;
}

bool TextEditor::isItalic() const
{
    return false;
}

bool TextEditor::isUnderline() const
{
    return false;
}

void TextEditor::onBoldToggled(bool checked)
{
    setBold(checked);
}

void TextEditor::onItalicToggled(bool checked)
{
    setItalic(checked);
}

void TextEditor::onUnderlineToggled(bool checked)
{
    setUnderline(checked);
}

void TextEditor::mergeFormatOnSelection(const QTextCharFormat &format)
{
    Q_UNUSED(format);
}
