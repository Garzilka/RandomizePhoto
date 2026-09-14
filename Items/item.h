#ifndef ITEM_H
#define ITEM_H

#include <QObject>

class QItem  : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool enable READ getenable WRITE setenable NOTIFY enableChanged)
    Q_PROPERTY(QString path READ getPath WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(bool canLightChange READ getCanLightChange WRITE setCanLightChange NOTIFY canLightChangeChanged)
    Q_PROPERTY(bool canMirrorChange READ getCanMirrorChange WRITE setCanMirrorChange NOTIFY canMirrorChanged)

    int NumImg = 0;
public:
    explicit QItem(const QString& inPath, QObject *parent = nullptr);

    const int& GetNumImg() { return NumImg; }
    QString getPath() const
    {
        QString result = QString("%1 (%2)").arg(m_path).arg(NumImg);
        return result;
    }
    QStringList getPaths() const
    {
        return Paths;
    }
    bool getCanLightChange() const { return m_canLightChange; }
    bool getCanMirrorChange() const { return m_canMirrorChange; }
    bool getenable() const { return m_enable; }

    void setPath(const QString& newPath)
    {
        if (m_path != newPath)
        {
            m_path = newPath;
            emit pathChanged();
        }
    }

    void setCanLightChange(bool newValue)
    {
        if (m_canLightChange != newValue)
        {
            m_canLightChange = newValue;
            emit canLightChangeChanged();
        }
    }

    void setCanMirrorChange(bool newValue)
    {
        if (m_canMirrorChange != newValue)
        {
            m_canMirrorChange = newValue;
            emit canMirrorChanged();
        }
    }

    void setenable(bool newValue)
    {
        if (m_enable != newValue)
        {
            m_enable = newValue;
            emit enableChanged();
        }
    }

signals:
    void pathChanged();
    void canLightChangeChanged();
    void canMirrorChanged();
    void enableChanged();

private:
    QString m_path;
    QStringList Paths;
    bool m_canLightChange = false;
    bool m_canMirrorChange = false;
    bool m_enable = true;
};

#endif // ITEM_H
