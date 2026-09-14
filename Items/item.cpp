#include "item.h"
#include "FileWorker/fileworker.h"

QItem::QItem(const QString& inPath, QObject *parent)
    : QObject(parent), m_path(inPath), m_canLightChange(false)
{
    FileWorker l_fileWorker(m_path);
    NumImg = l_fileWorker.getNumImages();
    Paths = l_fileWorker.getPaths();
}
