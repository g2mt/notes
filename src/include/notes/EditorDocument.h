#ifndef EDITOR_DOCUMENT_H
#define EDITOR_DOCUMENT_H

#include <QLabel>
#include <QObject>
#include <QTextCharFormat>
#include <QWidget>

class EditorDocument;
class EditorElement : public QWidget {
  friend class EditorDocument;
  Q_OBJECT

public:
  EditorDocument *document() const;
  QString toMarkdown() const;

private:
  EditorDocument *m_document;
};

class EditorBlock : public EditorElement {
  Q_OBJECT

public:
  // Selection is set by the EditorCursor

  bool isSelected() const;

protected:
  bool m_selected;
};

class EditorListBlock : public EditorBlock {};
class EditorTableBlock : public EditorBlock {};

class EditorFragment : public EditorElement {
  Q_OBJECT

public:
  const QString &text() const;

  QTextCharFormat charFormat() const;
  void setCharFormat(QTextCharFormat);

  int selectionStart() const;
  int selectionEnd() const;

public slots:
  void setText(QString &);

private:
  QString m_text;
  QTextCharFormat m_charFormat;
  // May be set by EditorCursor.
  int m_selectionStart;
  // May be set by EditorCursor. Equals to m_selectionStart for single
  // selections.
  int m_selectionEnd;
};

class EditorCursor : public QObject {
  friend class EditorDocument;
  Q_OBJECT

protected:
  EditorElement *start;
  EditorElement *end;
};

class Editor;
class EditorDocument : public QWidget {
  Q_OBJECT

public:
  explicit EditorDocument(Editor *parent = nullptr);

  bool isEmpty() const;
  bool isModified() const;
  void setModified(bool);
  void setMarkdown(const QString &);

signals:
  void modificationChanged(bool);

private:
  bool m_modified = false;
  EditorCursor *cursor = nullptr;
};

#endif // EDITOR_DOCUMENT_H
