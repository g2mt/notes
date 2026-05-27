#ifndef EDITORBRIDGE_P_H
#define EDITORBRIDGE_P_H

#include <QObject>

class EditorBridge : public QObject {
  Q_OBJECT

public:
  explicit EditorBridge(QObject *parent = nullptr) : QObject(parent) {}

  bool isModified() const { return m_modified; }

  void setModified(bool modified) {
    if (m_modified != modified) {
      m_modified = modified;
      emit modificationChanged(modified);
    }
  }

signals:
  void formattingChanged(bool bold, bool italic, bool underline,
                         bool strikethrough, bool superscript, bool subscript);
  void markdownChanged(const QString &markdown);
  void insertPlainText(const QString &text);
  void modificationChanged(bool modified);
  void undoAvailable(bool available);
  void redoAvailable(bool available);
  void loaded();

public slots:
  void emitLoaded() { emit loaded(); }
  void emitFormattingChanged(bool bold, bool italic, bool underline,
                             bool strikethrough, bool superscript,
                             bool subscript) {
    emit formattingChanged(bold, italic, underline, strikethrough, superscript,
                           subscript);
  }
  void emitModificationChanged(bool modified) {
    if (m_modified != modified) {
      m_modified = modified;
      emit modificationChanged(modified);
    }
  }
  void emitUndoAvailable(bool available) { emit undoAvailable(available); }
  void emitRedoAvailable(bool available) { emit redoAvailable(available); }

private:
  bool m_modified = false;
};

#endif // EDITORBRIDGE_P_H
