#ifndef FILEWORKER_H
#define FILEWORKER_H
#include <QObject>
#include <QStringList>

class QFileWorker : public QObject
{
    Q_OBJECT
public:
    explicit QFileWorker(const QStringList& sourcePhotos, const QString& outPath, int targetCount, int startNumImg, QObject *parent = nullptr);

public slots:
    // Этот метод выполнится в отдельном потоке
    void process();

signals:
    // Сигналы для связи с главным потоком
    void progressChanged(float procentRandomize, float procentWriteFile);
    void finished();
    void errorOccurred(const QString& message);

private:
    QStringList GetPhoto();
    bool isPositionSafe(const QStringList& list, const QString& item, int index, int minDistance);
    void RunCopyFile(QStringList& ListPhoto);

    QStringList m_sourcePhotos;
    QString m_outPath;
    int m_TargeNumImg = 100;
    int m_StartNumImg = 1;

    float m_procentRandomize = 0.f;
    float m_procentWriteFile = 0.f;
};

#endif // FILEWORKER_H
