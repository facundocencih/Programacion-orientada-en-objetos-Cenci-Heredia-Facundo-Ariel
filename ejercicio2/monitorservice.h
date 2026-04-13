#ifndef MONITORSERVICE_H
#define MONITORSERVICE_H

#include <QObject>
#include <QDateTime>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QTimer>

struct MetricSnapshot
{
    QString overallStatus;
    double cpuLoadPercent = 0.0;
    double memoryPercent = 0.0;
    double diskPercent = 0.0;
    QString uptimeText;
    QString summary;
    QString sourceEndpoint;
    QDateTime checkedAt;
    QStringList events;
    bool online = false;
};

class MonitorService : public QObject
{
    Q_OBJECT

public:
    explicit MonitorService(QObject *parent = nullptr);

    void setEndpoint(const QUrl &endpoint);
    void setRefreshIntervalSeconds(int seconds);
    void setAlertThresholdPercent(int thresholdPercent);

    QUrl endpoint() const;
    int refreshIntervalSeconds() const;
    int alertThresholdPercent() const;

public slots:
    void refreshNow();

signals:
    void snapshotUpdated(const MetricSnapshot &snapshot);
    void requestStateChanged(bool busy);

private slots:
    void performRequest();

private:
    MetricSnapshot buildSnapshotFromJson(const QJsonObject &root);
    MetricSnapshot buildErrorSnapshot(const QString &message) const;
    QString inferOverallStatus(const QJsonObject &root,
                               double cpuPercent,
                               double memoryPercent,
                               double diskPercent) const;
    QString formatUptime(qint64 totalSeconds) const;
    QString extractUptimeText(const QJsonObject &root,
                              const QJsonObject &system,
                              const QJsonObject &metrics) const;
    QStringList extractEvents(const QJsonObject &root, const QString &status);
    double extractPercent(const QJsonObject &root, const QStringList &keys) const;
    double extractNumber(const QJsonObject &root, const QStringList &keys) const;
    QString extractString(const QJsonObject &root, const QStringList &keys) const;
    qint64 extractInteger(const QJsonObject &root, const QStringList &keys) const;
    QDateTime extractDateTime(const QJsonObject &root, const QStringList &keys) const;
    static QJsonObject nestedObject(const QJsonObject &object, const QString &key);

    QNetworkAccessManager m_networkManager;
    QTimer m_refreshTimer;
    QUrl m_endpoint;
    int m_refreshIntervalSeconds = 15;
    int m_alertThresholdPercent = 80;
    bool m_busy = false;
    QStringList m_recentEvents;
};

#endif
