#ifndef EDITOR_TEXT_FRAGMENT_H
#define EDITOR_TEXT_FRAGMENT_H

#include <QSize>
#include <QTextCharFormat>

#include "notes/fragments/EditorFragment.h"

class EditorTextFragment;
class EditorTextFragmentSub : public EditorFragmentSub {
  Q_OBJECT

public:
  EditorTextFragmentSub(int textOffsetStart, int textOffsetEnd,
                        EditorTextFragment *tf, QWidget *parent = nullptr);

  QSize sizeHint() const override;

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  int m_textOffsetStart;
  int m_textOffsetEnd;
  EditorTextFragment *m_tf;
  QSize m_sizeHint;
};

class EditorTextFragment : public EditorFragment {
  friend class EditorTextFragmentSub;
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

  void setSelected(bool selected) override;

  QSize sizeHint() const override;

public slots:
  void setText(QString &);

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  QString m_text;
  QTextCharFormat m_charFormat;
  QSize m_sizeHint;
  int m_selectionStart = -1;
  int m_selectionEnd = -1;
};

#endif // EDITOR_TEXT_FRAGMENT_H
