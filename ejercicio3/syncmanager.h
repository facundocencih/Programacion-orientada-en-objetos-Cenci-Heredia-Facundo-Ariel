#ifndef SYNCMANAGER_H
#define SYNCMANAGER_H

#include "drawingmodel.h"

#include <QObject>
#include <QUrl>

class QNetworkAccessManager;
class QNetworkReply;
class QTimer;

class SyncManager : public QObject
{
    Q_OBJECT

public:
    explicit SyncManager(DrawingModel *model, QObject *parent = nullptr);

    void start();

public slots:
    void saveNow();

signals:
    void statusChanged(const QString &message);

private slots:
    void checkServerHealth();
    void fetchRemoteSnapshot();

private:
    void sendPendingStrokes();
    void handleHealthReply(QNetworkReply *reply);
    void handleSnapshotReply(QNetworkReply *reply);
    void handleMergeReply(QNetworkReply *reply, const QStringList &sentIds);

    DrawingModel *m_model = nullptr;
    QNetworkAccessManager *m_network = nullptr;
    QTimer *m_pollTimer = nullptr;
    QUrl m_baseUrl;
    bool m_syncInFlight = false;
};

#endif
