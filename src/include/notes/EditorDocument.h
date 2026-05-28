#ifndef EDITOR_DOCUMENT_H
#define EDITOR_DOCUMENT_H

#include <QStack>
#include <QTextCharFormat>

#include <md4c.h>

#include "notes/blocks/EditorBlock.h"

class Editor;

class EditorCursor : public QObject {
  friend class EditorDocument;
  Q_OBJECT

protected:
  EditorElement *start;
  EditorElement *end;
};

class EditorDocument : public EditorBlock {
  Q_OBJECT

public:
  explicit EditorDocument(Editor *parent = nullptr);

  bool isModified() const;
  void setModified(bool);
  void setMarkdown(const QString &);

  QSize sizeHint() const override;

signals:
  void modificationChanged(bool);

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
