#include "fileworker.h"

FileWorker::FileWorker(const QString& folderPath)
{
    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.webp";

    Directory = new QDir(folderPath);

    if(!Directory) return;

    const QFileInfoList fileList = Directory->entryInfoList(filters, QDir::Files);

    for (const auto& fileInfo : fileList)
    {
        Paths.append(fileInfo.absoluteFilePath());
    }
}

FileWorker::~FileWorker()
{
    delete Directory;
}

int FileWorker::getNumImages()
{
    return Paths.size();
}
