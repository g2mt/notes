#include "notes/EditorDocument.h"
#include "notes/Editor.h"
#include "notes/blocks/EditorAdmonitionBlock.h"
#include "notes/blocks/EditorCodeBlock.h"
#include "notes/blocks/EditorHeadingBlock.h"
#include "notes/blocks/EditorHrBlock.h"
#include "notes/blocks/EditorListBlock.h"
#include "notes/blocks/EditorListItemBlock.h"
#include "notes/blocks/EditorTableBlock.h"
#include "notes/blocks/EditorTableCellBlock.h"
#include "notes/blocks/EditorTableRowBlock.h"
#include "notes/fragments/EditorBrFragment.h"
#include "notes/fragments/EditorTextFragment.h"

#include <QMouseEvent>
#include <QResizeEvent>

// Uncomment to enable md4c traversal debug output
#define MD_TRACE_ENABLED

#ifdef MD_TRACE_ENABLED
#define MD_TRACE qDebug()
#else
#define MD_TRACE QNoDebug()
#endif

EditorDocument::EditorDocument(Editor *parent)
    : EditorBlock(parent), m_cursor(new EditorCursor(this)) {}

EditorDocument::~EditorDocument() { delete m_cursor; }

EditorCursor *EditorDocument::cursor() const { return m_cursor; }

//
// Document State
//

bool EditorDocument::isModified() const { return m_modified; }

void EditorDocument::setModified(bool modified) {
  if (m_modified != modified) {
    m_modified = modified;
    emit modificationChanged(modified);
  }
}

//
// Markdown Parsing
//

int EditorDocument::enterBlock(MD_BLOCKTYPE type, void *detail,
                               void *userdata) {
  auto *doc = static_cast<EditorDocument *>(userdata);

  if (type == MD_BLOCK_DOC) {
    doc->m_blockStack.push(doc);
    return 0;
  }

  auto *parentBlock = doc->m_blockStack.top();
  EditorBlock *block = nullptr;

  switch (type) {
  case MD_BLOCK_QUOTE:
    block = new EditorBlock(doc);
    block->setMargins(QMargins(24, 4, 8, 4));
    break;

  case MD_BLOCK_UL: {
    auto *ulDetail = static_cast<MD_BLOCK_UL_DETAIL *>(detail);
    block = new EditorListBlock(EditorListBlock::Unordered, doc);
    if (ulDetail->is_tight)
      block->setMargins(QMargins(16, 0, 8, 0));
    break;
  }

  case MD_BLOCK_OL: {
    auto *olDetail = static_cast<MD_BLOCK_OL_DETAIL *>(detail);
    block = new EditorListBlock(EditorListBlock::Ordered, doc);
    if (olDetail->is_tight)
      block->setMargins(QMargins(16, 0, 8, 0));
    break;
  }

  case MD_BLOCK_LI:
    block = new EditorListItemBlock(doc);
    break;

  case MD_BLOCK_HR:
    block = new EditorHrBlock(doc);
    parentBlock->addElement(block);
    doc->m_blockStack.push(block);
    return 0;

  case MD_BLOCK_H: {
    auto *hDetail = static_cast<MD_BLOCK_H_DETAIL *>(detail);
    block = new EditorHeadingBlock(hDetail->level, doc);
    break;
  }

  case MD_BLOCK_CODE:
    block = new EditorCodeBlock(doc);
    break;

  case MD_BLOCK_HTML:
    block = new EditorBlock(doc);
    break;

  case MD_BLOCK_P:
    block = new EditorBlock(doc);
    break;

  case MD_BLOCK_TABLE: {
    auto *tableDetail = static_cast<MD_BLOCK_TABLE_DETAIL *>(detail);
    block = new EditorTableBlock(
        tableDetail->col_count,
        tableDetail->head_row_count + tableDetail->body_row_count, doc);
    break;
  }

  case MD_BLOCK_THEAD:
    block = new EditorTableRowBlock(doc);
    break;

  case MD_BLOCK_TBODY:
    block = new EditorTableRowBlock(doc);
    break;

  case MD_BLOCK_TR:
    block = new EditorTableRowBlock(doc);
    break;

  case MD_BLOCK_TH:
    block = new EditorTableCellBlock(true, doc);
    break;

  case MD_BLOCK_TD:
    block = new EditorTableCellBlock(false, doc);
    break;

  case MD_BLOCK_FOOTNOTE_DEF_SECTION:
    block = new EditorBlock(doc);
    block->setMargins(QMargins(8, 12, 8, 4));
    break;

  case MD_BLOCK_FOOTNOTE_DEF:
    block = new EditorBlock(doc);
    block->setMargins(QMargins(16, 2, 8, 2));
    break;

  case MD_BLOCK_ADMONITION: {
    auto *admDetail = static_cast<MD_BLOCK_ADMONITION_DETAIL *>(detail);
    QString type =
        QString::fromUtf8(admDetail->type.text, admDetail->type.size);
    block = new EditorAdmonitionBlock(type, doc);
    break;
  }
  }

  parentBlock->addElement(block);
  doc->m_blockStack.push(block);
  return 0;
}

int EditorDocument::leaveBlock(MD_BLOCKTYPE type, void *detail,
                               void *userdata) {
  auto *doc = static_cast<EditorDocument *>(userdata);
  auto *block = doc->m_blockStack.top();
  doc->m_blockStack.pop();

  if (type != MD_BLOCK_DOC && type != MD_BLOCK_HR)
    static_cast<EditorBlock *>(block)->relayout();

  return 0;
}

int EditorDocument::enterSpan(MD_SPANTYPE type, void *detail, void *userdata) {
  auto *doc = static_cast<EditorDocument *>(userdata);
  QTextCharFormat fmt;

  if (!doc->m_formatStack.isEmpty())
    fmt = doc->m_formatStack.top();

  switch (type) {
  case MD_SPAN_STRONG:
    fmt.setFontWeight(QFont::Bold);
    break;
  case MD_SPAN_EM:
    fmt.setFontItalic(true);
    break;
  case MD_SPAN_DEL:
    fmt.setFontStrikeOut(true);
    break;
  case MD_SPAN_CODE:
    fmt.setFontFamilies({"monospace"});
    break;
  case MD_SPAN_U:
    fmt.setFontUnderline(true);
    break;
  case MD_SPAN_SUPERSCRIPT:
    fmt.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
    break;
  case MD_SPAN_SUBSCRIPT:
    fmt.setVerticalAlignment(QTextCharFormat::AlignSubScript);
    break;
  case MD_SPAN_A: {
    auto *linkDetail = static_cast<MD_SPAN_A_DETAIL *>(detail);
    fmt.setAnchorHref(
        QString::fromUtf8(linkDetail->href.text, linkDetail->href.size));
    fmt.setForeground(Qt::blue);
    fmt.setFontUnderline(true);
    break;
  }
  default:
    break;
  }

  doc->m_formatStack.push(fmt);
  return 0;
}

int EditorDocument::leaveSpan(MD_SPANTYPE type, void *detail, void *userdata) {
  auto *doc = static_cast<EditorDocument *>(userdata);
  doc->m_formatStack.pop();
  return 0;
}

int EditorDocument::textCallback(MD_TEXTTYPE type, const MD_CHAR *text,
                                 MD_SIZE size, void *userdata) {
  auto *doc = static_cast<EditorDocument *>(userdata);
  auto *block = static_cast<EditorBlock *>(doc->m_blockStack.top());

  switch (type) {
  case MD_TEXT_BR:
  case MD_TEXT_SOFTBR: {
    auto *frag = new EditorBrFragment(block);
    block->addElement(frag);
    break;
  }

  case MD_TEXT_CODE: {
    bool inCodeBlock = qobject_cast<EditorCodeBlock *>(block);

    QTextCharFormat fmt = doc->m_formatStack.isEmpty()
                              ? QTextCharFormat()
                              : doc->m_formatStack.top();

    if (inCodeBlock) {
      QString str = QString::fromUtf8(text, size);
      QStringList lines = str.split('\n');
      for (int i = 0; i < lines.size(); ++i) {
        if (i > 0) {
          auto *br = new EditorBrFragment(block);
          block->addElement(br);
        }
        auto *frag = new EditorTextFragment(block);
        frag->setText(lines[i]);
        frag->setCharFormat(fmt);
        block->addElement(frag);
      }
    } else {
      auto *frag = new EditorTextFragment(block);
      QString str = QString::fromUtf8(text, size);
      str = str.replace("\n", "");
      frag->setText(str);
      fmt.setFontFamilies({"monospace"});
      frag->setCharFormat(fmt);
      block->addElement(frag);
    }
    break;
  }

  case MD_TEXT_NORMAL:
  case MD_TEXT_NULLCHAR:
  case MD_TEXT_ENTITY:
  case MD_TEXT_HTML:
  case MD_TEXT_LATEXMATH: {
    auto *frag = new EditorTextFragment(block);
    QString str = QString::fromUtf8(text, size);
    frag->setText(str);

    QTextCharFormat fmt = doc->m_formatStack.isEmpty()
                              ? QTextCharFormat()
                              : doc->m_formatStack.top();

    auto *headingBlock = qobject_cast<EditorHeadingBlock *>(block);
    if (headingBlock) {
      fmt.setFont(headingBlock->headingFont());
    } else if (qobject_cast<EditorCodeBlock *>(block)) {
      fmt.setFontFamilies({"monospace"});
    }

    frag->setCharFormat(fmt);
    block->addElement(frag);
    break;
  }
  }

  return 0;
}

static const char *kBlockTypeNames[] = {
    [MD_BLOCK_DOC] = "MD_BLOCK_DOC",
    [MD_BLOCK_QUOTE] = "MD_BLOCK_QUOTE",
    [MD_BLOCK_UL] = "MD_BLOCK_UL",
    [MD_BLOCK_OL] = "MD_BLOCK_OL",
    [MD_BLOCK_LI] = "MD_BLOCK_LI",
    [MD_BLOCK_HR] = "MD_BLOCK_HR",
    [MD_BLOCK_H] = "MD_BLOCK_H",
    [MD_BLOCK_CODE] = "MD_BLOCK_CODE",
    [MD_BLOCK_HTML] = "MD_BLOCK_HTML",
    [MD_BLOCK_P] = "MD_BLOCK_P",
    [MD_BLOCK_TABLE] = "MD_BLOCK_TABLE",
    [MD_BLOCK_THEAD] = "MD_BLOCK_THEAD",
    [MD_BLOCK_TBODY] = "MD_BLOCK_TBODY",
    [MD_BLOCK_TR] = "MD_BLOCK_TR",
    [MD_BLOCK_TH] = "MD_BLOCK_TH",
    [MD_BLOCK_TD] = "MD_BLOCK_TD",
    [MD_BLOCK_FOOTNOTE_DEF_SECTION] = "MD_BLOCK_FOOTNOTE_DEF_SECTION",
    [MD_BLOCK_FOOTNOTE_DEF] = "MD_BLOCK_FOOTNOTE_DEF",
    [MD_BLOCK_ADMONITION] = "MD_BLOCK_ADMONITION",
};

static const char *kSpanTypeNames[] = {
    [MD_SPAN_EM] = "MD_SPAN_EM",
    [MD_SPAN_STRONG] = "MD_SPAN_STRONG",
    [MD_SPAN_A] = "MD_SPAN_A",
    [MD_SPAN_IMG] = "MD_SPAN_IMG",
    [MD_SPAN_CODE] = "MD_SPAN_CODE",
    [MD_SPAN_DEL] = "MD_SPAN_DEL",
    [MD_SPAN_LATEXMATH] = "MD_SPAN_LATEXMATH",
    [MD_SPAN_LATEXMATH_DISPLAY] = "MD_SPAN_LATEXMATH_DISPLAY",
    [MD_SPAN_WIKILINK] = "MD_SPAN_WIKILINK",
    [MD_SPAN_U] = "MD_SPAN_U",
    [MD_SPAN_SPOILER] = "MD_SPAN_SPOILER",
    [MD_SPAN_SUPERSCRIPT] = "MD_SPAN_SUPERSCRIPT",
    [MD_SPAN_SUBSCRIPT] = "MD_SPAN_SUBSCRIPT",
    [MD_SPAN_FOOTNOTE_REF] = "MD_SPAN_FOOTNOTE_REF",
};

static const char *kTextTypeNames[] = {
    [MD_TEXT_NORMAL] = "MD_TEXT_NORMAL",
    [MD_TEXT_NULLCHAR] = "MD_TEXT_NULLCHAR",
    [MD_TEXT_BR] = "MD_TEXT_BR",
    [MD_TEXT_SOFTBR] = "MD_TEXT_SOFTBR",
    [MD_TEXT_ENTITY] = "MD_TEXT_ENTITY",
    [MD_TEXT_CODE] = "MD_TEXT_CODE",
    [MD_TEXT_HTML] = "MD_TEXT_HTML",
    [MD_TEXT_LATEXMATH] = "MD_TEXT_LATEXMATH",
};

void EditorDocument::setMarkdown(const QString &markdown) {
  const auto &blocks = findChildren<EditorBlock *>();
  for (auto *block : blocks) {
    block->deleteLater();
  }
  m_elements.clear();
  m_blockStack.clear();
  m_formatStack.clear();

  MD_PARSER parser = {};
  parser.abi_version = 0;
  parser.flags = MD_FLAG_COLLAPSEWHITESPACE | MD_FLAG_STRIKETHROUGH |
                 MD_FLAG_TABLES | MD_FLAG_UNDERLINE | MD_FLAG_SUPERSCRIPTS |
                 MD_FLAG_SUBSCRIPTS;
  parser.enter_block = [](MD_BLOCKTYPE type, void *detail, void *userdata) {
    MD_TRACE << "enter_block" << kBlockTypeNames[type];
    return EditorDocument::enterBlock(type, detail, userdata);
  };
  parser.leave_block = [](MD_BLOCKTYPE type, void *detail, void *userdata) {
    MD_TRACE << "leave_block" << kBlockTypeNames[type];
    return EditorDocument::leaveBlock(type, detail, userdata);
  };
  parser.enter_span = [](MD_SPANTYPE type, void *detail, void *userdata) {
    MD_TRACE << "enter_span" << kSpanTypeNames[type];
    return EditorDocument::enterSpan(type, detail, userdata);
  };
  parser.leave_span = [](MD_SPANTYPE type, void *detail, void *userdata) {
    MD_TRACE << "leave_span" << kSpanTypeNames[type];
    return EditorDocument::leaveSpan(type, detail, userdata);
  };
  parser.text = [](MD_TEXTTYPE type, const MD_CHAR *text, MD_SIZE size,
                   void *userdata) {
    MD_TRACE << "text" << kTextTypeNames[type];
    return EditorDocument::textCallback(type, text, size, userdata);
  };
  parser.debug_log = nullptr;
  parser.syntax = nullptr;

  QByteArray utf8 = markdown.toUtf8();
  md_parse(utf8.constData(), utf8.size(), &parser, this);

  relayout();
}

//
// Events
//

void EditorDocument::mousePressEvent(QMouseEvent *event) {
  if (childAt(event->pos()) == nullptr && m_cursor)
    m_cursor->clearSelection();
  EditorBlock::mousePressEvent(event);
}
