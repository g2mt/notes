#ifndef EDITOR_TEXT_FRAGMENT_H
#define EDITOR_TEXT_FRAGMENT_H

#include <QList>
#include <QRect>
#include <QTextCharFormat>

#include "notes/fragments/EditorFragment.h"

struct EditorFragmentSub {
  int textOffsetStart;
  int textOffsetEnd;
  QRect rect;
};

class EditorTextFragment : public EditorFragment {
  Q_OBJECT
  Q_PROPERTY(QString text READ text WRITE setText)
  Q_PROPERTY(QTextCharFormat charFormat READ charFormat WRITE setCharFormat)

public:
  EditorTextFragment(QWidget *parent = nullptr);
  ~EditorTextFragment() override;

  const QString &text() const;

  QTextCharFormat charFormat() const;
  void setCharFormat(QTextCharFormat);

  int selectionStart() const;
  int selectionEnd() const;

  int preferredWidth() const;
  int lineHeight() const;

  const QList<EditorFragmentSub *> &subs() const;
  void setSubs(QList<EditorFragmentSub *>);

public slots:
  void setText(QString &);

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  QString m_text;
  QTextCharFormat m_charFormat;
  int m_selectionStart = -1;
  int m_selectionEnd = -1;
  QList<EditorFragmentSub *> m_subs;
};

#endif // EDITOR_TEXT_FRAGMENT_H
