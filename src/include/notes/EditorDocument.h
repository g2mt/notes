#ifndef EDITOR_DOCUMENT_H
#define EDITOR_DOCUMENT_H

#include <QStack>
#include <QTextCharFormat>

#include <md4c.h>

#include "notes/blocks/EditorBlock.h"

class Editor;
class QMouseEvent;

class EditorCursor : public QObject {
  friend class EditorDocument;
  Q_OBJECT

public:
  explicit EditorCursor(EditorDocument *doc);

  void select(EditorElement *element);
  void extendTo(EditorElement *element);
  void clearSelection();

  bool isSelected(EditorElement *element) const;
  bool hasSelection() const;
  bool isDragging() const;
  void setDragging(bool dragging);

signals:
  void selectionChanged();

private:
  void applySelection();
  QList<EditorElement *> leafElements() const;

  EditorDocument *m_document;
  QPoint m_selectionStart;
  QPoint m_selectionEnd;
  bool m_dragging = false;
};

class EditorDocument : public EditorBlock {
  friend class EditorCursor;
  Q_OBJECT

public:
  explicit EditorDocument(Editor *parent = nullptr);
  ~EditorDocument() override;

  EditorCursor *cursor() const;

  bool isModified() const;
  void setModified(bool);
  void setMarkdown(const QString &);

  QSize sizeHint() const override;

signals:
  void modificationChanged(bool);

protected:
  void mousePressEvent(QMouseEvent *event) override;

private:
  void relayout() override;

  static int enterBlock(MD_BLOCKTYPE type, void *detail, void *userdata);
  static int leaveBlock(MD_BLOCKTYPE type, void *detail, void *userdata);
  static int enterSpan(MD_SPANTYPE type, void *detail, void *userdata);
  static int leaveSpan(MD_SPANTYPE type, void *detail, void *userdata);
  static int textCallback(MD_TEXTTYPE type, const MD_CHAR *text, MD_SIZE size,
                          void *userdata);

  bool m_modified = false;
  EditorCursor *m_cursor = nullptr;
  QStack<EditorBlock *> m_blockStack;
  QStack<QTextCharFormat> m_formatStack;
};

#endif // EDITOR_DOCUMENT_H
