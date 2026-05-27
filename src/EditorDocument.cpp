#include "notes/EditorDocument.h"
#include "notes/Editor.h"

#include <QLayout>
#include <QResizeEvent>
#include <QVBoxLayout>

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
  if (type != MD_TEXT_NORMAL)
    return 0;

  auto *doc = static_cast<EditorDocument *>(userdata);
  if (doc->m_blockStack.isEmpty() || !doc->m_blockStack.top())
    return 0;

  auto *block = doc->m_blockStack.top();
  auto *frag = new EditorFragment(block);
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
    return EditorDocument::enterBlock(type, detail, userdata);
  };
  parser.leave_block = [](MD_BLOCKTYPE type, void *detail, void *userdata) {
    return EditorDocument::leaveBlock(type, detail, userdata);
  };
  parser.enter_span = [](MD_SPANTYPE type, void *detail, void *userdata) {
    return EditorDocument::enterSpan(type, detail, userdata);
  };
  parser.leave_span = [](MD_SPANTYPE type, void *detail, void *userdata) {
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
