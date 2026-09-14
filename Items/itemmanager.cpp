#include "itemmanager.h"
#include <algorithm>
#include <random>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>


QItemManager::QItemManager(QObject *parent) : QAbstractListModel(parent)
{}

QItemManager::~QItemManager()
{
    qDeleteAll(m_itemList);
    m_itemList.clear();
}

QHash<int, QByteArray> QItemManager::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ItemObjectRole] = "itemObject"; // Под этим именем объект будет доступен в QML делегате
    return roles;
}

int QItemManager::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_itemList.size();
}

QVariant QItemManager::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_itemList.size())
        return QVariant();

    if (role == ItemObjectRole)
    {
        return QVariant::fromValue(m_itemList[index.row()]);
    }
    return QVariant();
}

void QItemManager::addItem(const QString& path)
{
    if(GetItemId(path) > -1) return;

    beginInsertRows(QModelIndex(), m_itemList.size(), m_itemList.size());
    m_itemList.append(new QItem(path, this));
    connect(&*m_itemList.last(), &QItem::enableChanged, this, &QItemManager::AutoUpdateNum);
    endInsertRows();
    UpdateNum();
}

void QItemManager::UpdateNum()
{
    int NewNum = 0;
    for(QItem* Item : m_itemList)
    {
        if(!Item->getenable()) continue;

        NewNum += Item->GetNumImg();
    }
    setNumImg(NewNum);
}
/*
void QItemManager::removeItem(const QString& path)
{
    int Idx = GetItemId(path);
    if(Idx < 0) return;

    beginRemoveRows(QModelIndex(), Idx, Idx);
    ItemList[Idx]->deleteLater();
    ItemList.removeAt(Idx);
    endRemoveRows();
}*/
QStringList QItemManager::GetPhoto()
{
    QStringList sourceList;

    for(QItem* Item : m_itemList)
    {
        if(!Item->getenable()) continue;

        sourceList.append(Item->getPaths());
    }

    if (sourceList.isEmpty() || m_TargeNumImg <= 0) return sourceList;


    QStringList result;

    // Генератор случайных чисел
    std::random_device rd;
    std::mt19937 g(rd());

    // Перемешиваем первый слой, чтобы исходный порядок не влиял на результат
    result = sourceList;
    std::shuffle(result.begin(), result.end(), g);

    // Если запрошено меньше или ровно столько, сколько есть, просто обрезаем
    if (result.size() >= m_TargeNumImg)
    {
        while (result.size() > m_TargeNumImg)
        {
            result.removeLast();
        }
        return result;
    }

    // 2. Добираем оставшиеся элементы
    int itemsNeeded = m_TargeNumImg - result.size();

    // Создаем копию исходного списка для создания "пула дубликатов"
    QStringList duplicatePool = sourceList;

    // Перемешиваем пул, чтобы выборка дубликатов была случайной
    std::shuffle(duplicatePool.begin(), duplicatePool.end(), g);

    // Добавляем ровно столько дубликатов, сколько не хватает
    for (int i = 0; i < itemsNeeded; ++i)
    {
        // Если вдруг targetCount значительно больше (например, надо из 30 сделать 100),
        // и пул закончился, мы просто перемешиваем его заново и берем сначала
        if (i >= duplicatePool.size())
        {
            std::shuffle(duplicatePool.begin(), duplicatePool.end(), g);
        }

        // Добавляем элемент из перемешанного пула.
        // За счет взятия по индексу [i % size] ни один элемент не повторится трижды,
        // пока остальные не повторились хотя бы дважды.
        result.append(duplicatePool[i % duplicatePool.size()]);
    }

    // 3. Финальный штрих: перемешиваем весь итоговый список,
    // чтобы дубликаты не шли друг за другом в конце списка, а равномерно распределились.
    std::shuffle(result.begin(), result.end(), g);

    return result;
}


void QItemManager::startprocess()
{
    // 1. Проверяем, выбран ли финальный путь
    if (m_OutPath.isEmpty())
    {
        qWarning() << "Ошибка: Финальный путь для копирования не выбран!";
        return;
    }

    // Проверяем, существует ли целевая папка, если нет — создаем
    QDir outDir(m_OutPath);
    if (!outDir.exists())
    {
        outDir.mkpath(".");
    }

    // 3. Вызываем функцию балансировки файлов, чтобы получить ровно targetCount путей
    QStringList balancedList = GetPhoto();

    // 4. Цикл копирования и переименования
    int counter = m_StartNumImg;
    for (const QString& srcPath : balancedList)
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

        counter++;
    }

    qDebug() << "Обработка завершена! Скопировано файлов:" << (counter - 1);
}
void QItemManager::removeItem(int index)
{
    if(index < 0 || index > m_itemList.size()) return;

    beginRemoveRows(QModelIndex(), index, index);
    m_itemList[index]->deleteLater();
    m_itemList.removeAt(index);
    endRemoveRows();
    UpdateNum();
}

int QItemManager::GetItemId(const QString& path)
{
    for(int Idx = 0; Idx < m_itemList.size(); Idx++)
    {
        if(!m_itemList[Idx]) continue;
        if(m_itemList[Idx]->getPath() != path) continue;

        return Idx;
    }

    return -1;
}

QItem* QItemManager::GetItem(const QString& path)
{
    int Idx = GetItemId(path);
    if(Idx < 0) return nullptr;

    return m_itemList[Idx];
}

QItem* QItemManager::GetItemById(const int& Id)
{
    if(Id >= 0 && Id < m_itemList.size())
    {
        return m_itemList[Id];
    }
    return nullptr;
}

int QItemManager::GetNumItems()
{
    return m_itemList.size();
}
