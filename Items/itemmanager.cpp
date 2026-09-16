#include "itemmanager.h"
#include <algorithm>
#include <random>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QThread>
#include "Threads/fileworker.h"
#include "Data/HashTable/tmap.h"


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



QStringList QItemManager::GetSourcePhoto()
{
    QStringList Result;

    for(QItem* Item : m_itemList)
    {
        if(!Item->getenable()) continue;

        Result.append(Item->getPaths());
    }
    return Result;
}

void QItemManager::startprocess()
{
    // 1. Проверяем, выбран ли финальный путь
    if (m_OutPath.isEmpty())
    {
        qWarning() << "Ошибка: Финальный путь для копирования не выбран!";
        return;
    }

    // 2. Проверяем, target amount != 0
    if (m_TargeNumImg == 0)
    {
        qWarning() << "Ошибка: финальное количество равно нулю!";
        return;
    }

    QStringList allSourcePhotos = GetSourcePhoto();


    // 2. Создаем поток и рабочего
    QThread* thread = new QThread();
    QFileWorker* worker = new QFileWorker(allSourcePhotos, m_OutPath, m_TargeNumImg, m_StartNumImg);

    // Перемещаем объект рабочего в созданный поток.
    // ТЕПЕРЬ ВСЕ СЛОТЫ WORKER БУДУТ ВЫПОЛНЯТЬСЯ В THREAD!
    worker->moveToThread(thread);

    // 3. СВЯЗЫВАЕМ СИГНАЛЫ И СЛОТЫ (Самая важная часть!)

    // Как только поток запустится — рабочий начнет копирование
    connect(thread, &QThread::started, worker, &QFileWorker::process);

    connect(worker, &QFileWorker::finished, thread, &QThread::quit);

    connect(worker, &QFileWorker::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    connect(worker, &QFileWorker::progressChanged, this, &QItemManager::UpdateThreadStatus);

    thread->start();

    qDebug() << "Главный поток свободен! Копирование пошло в фоне.";

}
void QItemManager::UpdateThreadStatus(float Rand, float Write)
{
    setprogressValue((Rand + Write) / 2.f);
    qDebug() << "progressValue:" << (m_progressValue);
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
