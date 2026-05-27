#ifndef EDITOR_DOCUMENT_H
#define EDITOR_DOCUMENT_H

#include <QLabel>
#include <QMargins>
#include <QObject>
#include <QStack>
#include <QTextCharFormat>
#include <QWidget>

#include <md4c.h>

class EditorDocument;

struct EditorFragmentSub {
  int textOffsetStart;
  int textOffsetEnd;
  QPoint pixelOffset;
};

class EditorElement : public QWidget {
  friend class EditorDocument;
  Q_OBJECT

public:
  explicit EditorElement(QWidget *parent = nullptr);

  EditorDocument *document() const;
  QString toMarkdown() const;

private:
  EditorDocument *m_document;
};

class EditorBlock : public EditorElement {
  Q_OBJECT

public:
  EditorBlock(QWidget *parent = nullptr);

  QSize sizeHint() const override;
  bool isSelected() const;

  void relayoutFragments();

protected:
  void resizeEvent(QResizeEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

  bool m_selected;
  QMargins m_margins;
  int m_lineHeight = 0;
};

class EditorListBlock : public EditorBlock {};
class EditorTableBlock : public EditorBlock {};

class EditorFragment : public EditorElement {
  Q_OBJECT

public:
  EditorFragment(QWidget *parent = nullptr);
};

class EditorTextFragment : public EditorFragment {
  Q_OBJECT
  Q_PROPERTY(QString text READ text WRITE setText)
  Q_PROPERTY(QTextCharFormat charFormat READ charFormat WRITE setCharFormat)

public:
  EditorTextFragment(QWidget *parent = nullptr);

  const QString &text() const;

  QTextCharFormat charFormat() const;
  void setCharFormat(QTextCharFormat);

  int selectionStart() const;
  int selectionEnd() const;

  int preferredWidth() const;
  int lineHeight() const;
  int widthForText(const QString &text) const;

  QList<EditorFragmentSub> subs() const;
  void setSubs(QList<EditorFragmentSub>);

public slots:
  void setText(QString &);

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  QString m_text;
  QTextCharFormat m_charFormat;
  int m_selectionStart = -1;
  int m_selectionEnd = -1;
  QList<EditorFragmentSub> m_subs;
};

class EditorBrFragment : public EditorFragment {
  Q_OBJECT

public:
  EditorBrFragment(QWidget *parent = nullptr);
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

protected:
  void resizeEvent(QResizeEvent *event) override;

private:
  static int enterBlock(MD_BLOCKTYPE type, void *detail, void *userdata);
  static int leaveBlock(MD_BLOCKTYPE type, void *detail, void *userdata);
  static int enterSpan(MD_SPANTYPE type, void *detail, void *userdata);
  static int leaveSpan(MD_SPANTYPE type, void *detail, void *userdata);
  static int textCallback(MD_TEXTTYPE type, const MD_CHAR *text, MD_SIZE size,
                          void *userdata);

  bool m_modified = false;
  EditorCursor *cursor = nullptr;
  QStack<EditorBlock *> m_blockStack;
  QStack<QTextCharFormat> m_formatStack;
};

#endif // EDITOR_DOCUMENT_H
