#include "notes/DocumentManager.h"

DocumentManager::DocumentManager(QObject *parent)
    : QObject(parent)
{
}

DocumentManager::~DocumentManager() = default;

void DocumentManager::newDocument()
{
    m_currentFilePath.clear();
    m_modified = false;
}

void DocumentManager::openDocument(const QString &filePath)
{
    m_currentFilePath = filePath;
    emit fileOpened(filePath);
}

bool DocumentManager::saveDocument(const QString &filePath)
{
    m_currentFilePath = filePath;
    emit fileSaved(filePath);
    return true;
}

QString DocumentManager::currentFilePath() const
{
    return m_currentFilePath;
}

bool DocumentManager::isModified() const
{
    return m_modified;
}
