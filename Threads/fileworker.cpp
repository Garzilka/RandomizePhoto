#include "fileworker.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QThread>
#include <algorithm>
#include <random>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

QFileWorker::QFileWorker(const QStringList& sourcePhotos, const QString& outPath, int targetCount, int startNumImg, QObject *parent)
    : QObject(parent), m_sourcePhotos(sourcePhotos), m_outPath(outPath), m_TargeNumImg(targetCount), m_StartNumImg(startNumImg) {}



bool QFileWorker::isPositionSafe(const QStringList &list, const QString &item, int index, int minDistance)
{
    int start = std::max(0, index - minDistance);

    for (int i = start; i < index; ++i)
    {
        if (list[i] == item)
        {
            return false;
        }
    }
    return true;
}

QStringList QFileWorker::GetPhoto()
{
    QStringList sourceList = m_sourcePhotos;
    int minDistance = sourceList.size() / 4;

    std::random_device rd;
    std::mt19937 g(rd());


    QStringList rawList = sourceList;
    std::shuffle(rawList.begin(), rawList.end(), g);

    if (rawList.size() < m_TargeNumImg)
    {
        int itemsNeeded = m_TargeNumImg - rawList.size();
        QStringList duplicatePool = sourceList;
        std::shuffle(duplicatePool.begin(), duplicatePool.end(), g);

        for (int i = 0; i < itemsNeeded; ++i)
        {
            if (i >= duplicatePool.size())
            {
                std::shuffle(duplicatePool.size() ? duplicatePool.begin() : duplicatePool.begin(), duplicatePool.end(), g);
            }
            rawList.append(duplicatePool[i % duplicatePool.size()]);
        }
    }
    else if (rawList.size() > m_TargeNumImg)
    {
        while (rawList.size() > m_TargeNumImg) rawList.removeLast();

        m_procentRandomize = 100.f;
        emit progressChanged(m_procentRandomize, m_procentWriteFile);
        return rawList;
    }

    // Финально перемешиваем перед распределением дистанции
    std::shuffle(rawList.begin(), rawList.end(), g);

    // 2. УМНОЕ РАСПРЕДЕЛЕНИЕ: Раздвигаем дубликаты
    QStringList result;
    result.reserve(m_TargeNumImg);

    // Пока в исходном сыром списке есть элементы
    while (!rawList.isEmpty())
    {
        bool placed = false;

        // Ищем первый элемент, который можно безопасно вставить на текущую позицию
        for (int i = 0; i < rawList.size(); ++i)
        {
            QString currentItem = rawList[i];

            if (isPositionSafe(result, currentItem, result.size(), minDistance))
            {
                result.append(currentItem);
                rawList.removeAt(i);

                m_procentRandomize = (float)result.size() / m_TargeNumImg;
                emit progressChanged(m_procentRandomize, m_procentWriteFile);

                placed = true;
                break;
            }
        }

        // КРИТИЧЕСКИЙ СЛУЧАЙ: Если мы дошли до конца списка, и ни один оставшийся элемент
        // не удовлетворяет условию дистанции (такое бывает в самом конце, когда остались одни дубликаты)
        if (!placed)
        {
            // Принудительно вставляем первый попавшийся элемент из остатка,
            // так как математически идеальный зазор уже невозможен
            result.append(rawList.takeFirst());
            m_procentRandomize = (float)result.size() / m_TargeNumImg;
            emit progressChanged(m_procentRandomize, m_procentWriteFile);
        }
    }

    return result;
}


void QFileWorker::RunCopyFile(QStringList &ListPhoto)
{
    // Проверяем, существует ли целевая папка, если нет — создаем
    QDir outDir(m_outPath);

    if (!outDir.exists())
    {
        outDir.mkpath(".");
    }

    // 4. Цикл копирования и переименования
    int counter = m_StartNumImg;
    int writedFile = 0;
    for (const QString& srcPath : ListPhoto)
    {
        QFileInfo fileInfo(srcPath);
        QString suffix = fileInfo.suffix(); // Сохраняем расширение (jpg, png etc.)

        // Формируем новое имя по шаблону: IMG_1.jpg, IMG_2.jpg...
        QString newFileName = QString("IMG_%1.%2")
                                  .arg(counter, 4, 10, QChar('0'))
                                  .arg(suffix);
        QString destPath = outDir.absoluteFilePath(newFileName);

        // Копируем файл
        // Если вдруг файл с таким именем уже существует (например, от прошлого запуска),
        // QFile::copy вернет false, поэтому лучше сначала удалить старый файл, если он там есть
        if (QFile::exists(destPath))
        {
            QFile::remove(destPath);
        }

        if (QFile::copy(srcPath, destPath))
        {
            qDebug() << "Успешно скопировано:" << srcPath << "->" << newFileName;
        } else
        {
            qWarning() << "Не удалось скопировать файл:" << srcPath;
        }
        writedFile++;
        m_procentWriteFile = (float)writedFile / m_TargeNumImg;
        emit progressChanged(m_procentRandomize, m_procentWriteFile);

        counter++;
    }

    qDebug() << "Обработка завершена! Скопировано файлов:" << (counter - 1);
}

void QFileWorker::process()
{
    if (m_outPath.isEmpty() || m_sourcePhotos.isEmpty() || m_TargeNumImg < 1)
    {
        emit errorOccurred("Неверные входные данные");
        emit finished();
        return;
    }
    m_procentRandomize = 0.f;
    m_procentWriteFile = 0.f;

    QStringList Result = GetPhoto();
    RunCopyFile(Result);

    // Сообщаем, что поток закончил работу
    emit finished();
}
