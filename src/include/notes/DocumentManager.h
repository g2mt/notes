#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <QObject>
#include <QString>

class DocumentManager : public QObject
{
    Q_OBJECT

public:
    explicit DocumentManager(QObject *parent = nullptr);
    ~DocumentManager();

    void newDocument();
    void openDocument(const QString &filePath);
    bool saveDocument(const QString &filePath);
    QString currentFilePath() const;
    bool isModified() const;

signals:
    void fileOpened(const QString &filePath);
    void fileSaved(const QString &filePath);
    void modificationChanged(bool modified);

private:
    QString m_currentFilePath;
    bool m_modified = false;
};

#endif // DOCUMENTMANAGER_H
