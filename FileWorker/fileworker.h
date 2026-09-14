#ifndef FILEWORKER_H
#define FILEWORKER_H

#include <QDir>
#include <QStringList>
#include <QFileInfoList>

class FileWorker
{

private:
    QStringList filters;
    QStringList Paths;
    QDir* Directory = nullptr;
public:
    FileWorker(const QString& folderPath);
    ~FileWorker();

    QStringList getRandomImages(int count);
    int getNumImages();
    QStringList getPaths() {return Paths;}
};

#endif // FILEWORKER_H
