#include "notes/Editor.h"

Editor::Editor(QWidget *parent)
    : QPlainTextEdit(parent)
{
}

Editor::~Editor() = default;

void Editor::setBold(bool bold)
{
    Q_UNUSED(bold);
}

void Editor::setItalic(bool italic)
{
    Q_UNUSED(italic);
}

void Editor::setUnderline(bool underline)
{
    Q_UNUSED(underline);
}

bool Editor::isBold() const
{
    return false;
}

bool Editor::isItalic() const
{
    return false;
}

bool Editor::isUnderline() const
{
    return false;
}

void Editor::onBoldToggled(bool checked)
{
    setBold(checked);
}

void Editor::onItalicToggled(bool checked)
{
    setItalic(checked);
}

void Editor::onUnderlineToggled(bool checked)
{
    setUnderline(checked);
}

void Editor::mergeFormatOnSelection(const QTextCharFormat &format)
{
    Q_UNUSED(format);
}
