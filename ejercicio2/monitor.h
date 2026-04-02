#ifndef MONITOR_H
#define MONITOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QUrl>

class Monitor : public QObject {
    Q_OBJECT

public:
    explicit Monitor(QObject *parent = nullptr);
    void setCheckInterval(int seconds);
    void setServerUrl(const QString &url);
    void setAlertThreshold(int threshold);
    void manualRefresh();

signals:
    void dataReceived(const QString &status, const QString &uptime, const QString &load, const QString &memoryUsed, const QString &diskFree, const QString &lastCheck);
    void errorOccurred(const QString &error);

private slots:
    void onReplyFinished(QNetworkReply *reply);
    void onTimerTimeout();

private:
    void collectData();
    QNetworkAccessManager *manager;
    QTimer *timer;
    int checkInterval;
    QUrl serverUrl;
    int alertThreshold;
};

#endif // MONITOR_H