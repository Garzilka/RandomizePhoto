#ifndef ITEMMANAGER_H
#define ITEMMANAGER_H

#include <QAbstractListModel>
#include "item.h"


class QItemManager : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int AllNumImg READ getNumImg WRITE setNumImg NOTIFY AllNumImgChanged)
    Q_PROPERTY(QString OutPath READ getOutPath WRITE setOutPath NOTIFY OutPathChanged)
    Q_PROPERTY(int targeNumImg READ getTargeNumImg WRITE setTargeNumImg NOTIFY TargeNumImgChanged)
    Q_PROPERTY(int startNumImg READ getstartNumImg WRITE setstartNumImg NOTIFY startNumImgChanged)

public:
    enum ItemRoles {
        ItemObjectRole = Qt::UserRole + 1
    };


    int getNumImg() { return AllNumImg; }

    QItemManager(QObject *parent = nullptr);
    ~QItemManager();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addItem(const QString& path);
    //Q_INVOKABLE void removeItem(const QString& path);
    Q_INVOKABLE void removeItem(int index);

    int GetItemId(const QString& path);
    QItem* GetItemById(const int& Id);
    QItem* GetItem(const QString& path);

    int GetNumItems();

    Q_INVOKABLE void setstartNumImg(int NewNum)
    {
        if (m_StartNumImg != NewNum)
        {
            qDebug() << "Стартовый номер:" << NewNum;
            m_StartNumImg = NewNum;
            emit startNumImgChanged();
        }
    }

    int getstartNumImg()
    {
        return m_StartNumImg;
    }

    Q_INVOKABLE void setTargeNumImg(int NewNum)
    {
        if (m_TargeNumImg != NewNum)
        {
            qDebug() << "Требуемое количество:" << NewNum;
            m_TargeNumImg = NewNum;
            emit TargeNumImgChanged();
        }
    }

    int getTargeNumImg()
    {
        return m_TargeNumImg;
    }

    Q_INVOKABLE void setOutPath(QString NewPath)
    {
        if (m_OutPath != NewPath)
        {
            m_OutPath = NewPath;
            emit OutPathChanged();
        }
    }
    QString getOutPath()
    {
        if(m_OutPath.isEmpty())
        {
            return "Путь для копирования файлов...";
        }
        return m_OutPath;
    }

    Q_INVOKABLE void startprocess();

    QStringList GetPhoto();
signals:
    void AllNumImgChanged();
    void OutPathChanged();
    void TargeNumImgChanged();
    void startNumImgChanged();
private slots:
    void AutoUpdateNum() {UpdateNum();};

private:
    QList<QItem*> m_itemList;
    QString m_OutPath = "";
    int AllNumImg = 0;
    int m_TargeNumImg = 100;
    int m_StartNumImg = 1;
    bool m_AllNumImg;
    void UpdateNum();
    void setNumImg(int newNum)
    {
        if (AllNumImg != newNum)
        {
            AllNumImg = newNum;
            emit AllNumImgChanged();
        }
    };
};

#endif // ITEMMANAGER_H
