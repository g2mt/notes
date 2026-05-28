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

  virtual void relayout();
  virtual void setMargins(const QMargins &margins);

  void addElement(EditorElement *child);

protected:
  void resizeEvent(QResizeEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

  bool m_selected;
  QMargins m_margins;
  int m_lineHeight = 0;
  QList<EditorElement *> m_children;
};

class EditorListItemBlock : public EditorBlock {
public:
  EditorListItemBlock(QWidget *parent = nullptr);
};

class EditorListBlock : public EditorBlock {
public:
  enum Type { Unordered, Ordered };

  EditorListBlock(Type type, QWidget *parent = nullptr);

  Type listType() const;

private:
  Type m_type;
};

class EditorTableCellBlock : public EditorBlock {
public:
  EditorTableCellBlock(bool isHeader, QWidget *parent = nullptr);

  bool isHeader() const;

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  bool m_isHeader;
};

class EditorTableBlock : public EditorBlock {
public:
  EditorTableBlock(QWidget *parent = nullptr);
};

class EditorHrBlock : public EditorBlock {
public:
  EditorHrBlock(QWidget *parent = nullptr);

protected:
  void paintEvent(QPaintEvent *event) override;
};

class EditorHeadingBlock : public EditorBlock {
  Q_OBJECT

public:
  EditorHeadingBlock(int level, QWidget *parent = nullptr);

  int headingLevel() const;
  QFont headingFont() const;

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  int m_level;
};

class EditorCodeBlock : public EditorBlock {
  Q_OBJECT

public:
  EditorCodeBlock(QWidget *parent = nullptr);

protected:
  void paintEvent(QPaintEvent *event) override;
};

class EditorAdmonitionBlock : public EditorBlock {
public:
  EditorAdmonitionBlock(const QString &type, QWidget *parent = nullptr);

  QString admonitionType() const;

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  QString m_type;
};

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

  void addElement(EditorElement *child);
  QSize sizeHint() const override;

signals:
  void modificationChanged(bool);

protected:
  void resizeEvent(QResizeEvent *event) override;

private:
  void relayout();

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
  QList<EditorElement *> m_children;
};

#endif // EDITOR_DOCUMENT_H
