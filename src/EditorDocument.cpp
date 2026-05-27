#include "notes/EditorDocument.h"
#include "notes/Editor.h"

#include <QLayout>
#include <QResizeEvent>
#include <QVBoxLayout>

// Uncomment to enable md4c traversal debug output
//#define MD_TRACE_ENABLED

#ifdef MD_TRACE_ENABLED
#  define MD_TRACE qDebug()
#else
#  define MD_TRACE QNoDebug()
#endif

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

EditorDocument::EditorDocument(Editor *parent) : QWidget(parent) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addStretch();
  setLayout(layout);
}

bool EditorDocument::isEmpty() const {
  return findChildren<EditorBlock *>().isEmpty();
}

bool EditorDocument::isModified() const { return m_modified; }

void EditorDocument::setModified(bool modified) {
  if (m_modified != modified) {
    m_modified = modified;
    emit modificationChanged(modified);
  }
}

void EditorDocument::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  const auto &blocks = findChildren<EditorBlock *>();
  for (auto *block : blocks)
    block->relayoutFragments();
}

int EditorDocument::enterBlock(MD_BLOCKTYPE type, void *detail,
                               void *userdata) {
  auto *doc = static_cast<EditorDocument *>(userdata);
  if (type == MD_BLOCK_P) {
    auto *block = new EditorBlock(doc);
    doc->m_blockStack.push(block);
    qobject_cast<QVBoxLayout *>(doc->layout())
        ->insertWidget(doc->layout()->count() - 1, block);
    return 0;
  }
  doc->m_blockStack.push(nullptr);
  return 0;
}

int EditorDocument::leaveBlock(MD_BLOCKTYPE type, void *detail,
                               void *userdata) {
  auto *doc = static_cast<EditorDocument *>(userdata);
  doc->m_blockStack.pop();
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
  if (doc->m_blockStack.isEmpty() || !doc->m_blockStack.top())
    return 0;

  auto *block = doc->m_blockStack.top();

  if (type == MD_TEXT_BR || type == MD_TEXT_SOFTBR) {
    new EditorBrFragment(block);
    return 0;
  }

  if (type != MD_TEXT_NORMAL)
    return 0;

  auto *frag = new EditorTextFragment(block);
  QString str = QString::fromUtf8(text, size);
  frag->setText(str);
  frag->setCharFormat(doc->m_formatStack.isEmpty() ? QTextCharFormat()
                                                   : doc->m_formatStack.top());
  frag->show();

  return 0;
}

void EditorDocument::setMarkdown(const QString &markdown) {
  const auto &blocks = findChildren<EditorBlock *>();
  for (auto *block : blocks) {
    block->deleteLater();
  }
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
    return EditorDocument::textCallback(type, text, size, userdata);
  };
  parser.debug_log = nullptr;
  parser.syntax = nullptr;

  QByteArray utf8 = markdown.toUtf8();
  md_parse(utf8.constData(), utf8.size(), &parser, this);

  updateGeometry();
  const auto &remainingBlocks = findChildren<EditorBlock *>();
  for (auto *block : remainingBlocks)
    block->relayoutFragments();
}
