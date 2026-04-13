#include "monitorservice.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QNetworkReply>
#include <QStringList>

namespace
{
QString normalizeStatus(const QString &value)
{
    const QString lowered = value.trimmed().toLower();
    if (lowered.contains("down") || lowered.contains("offline") || lowered.contains("caido")) {
        return "CAIDO";
    }

    if (lowered.contains("warn") || lowered.contains("alert") || lowered.contains("degrad")) {
        return "ALERTA";
    }

    return "OK";
}
}

MonitorService::MonitorService(QObject *parent)
    : QObject(parent)
{
    m_endpoint = QUrl("http://173.212.209.61:5000/metrics");

    m_refreshTimer.setInterval(m_refreshIntervalSeconds * 1000);
    connect(&m_refreshTimer, &QTimer::timeout, this, &MonitorService::performRequest);
    m_refreshTimer.start();
}

void MonitorService::setEndpoint(const QUrl &endpoint)
{
    m_endpoint = endpoint;
}

void MonitorService::setRefreshIntervalSeconds(int seconds)
{
    m_refreshIntervalSeconds = qMax(5, seconds);
    m_refreshTimer.setInterval(m_refreshIntervalSeconds * 1000);
}

void MonitorService::setAlertThresholdPercent(int thresholdPercent)
{
    m_alertThresholdPercent = qBound(50, thresholdPercent, 100);
}

QUrl MonitorService::endpoint() const
{
    return m_endpoint;
}

int MonitorService::refreshIntervalSeconds() const
{
    return m_refreshIntervalSeconds;
}

int MonitorService::alertThresholdPercent() const
{
    return m_alertThresholdPercent;
}

void MonitorService::refreshNow()
{
    performRequest();
}

void MonitorService::performRequest()
{
    if (m_busy) {
        return;
    }

    if (!m_endpoint.isValid() || m_endpoint.scheme().isEmpty()) {
        emit snapshotUpdated(buildErrorSnapshot("Endpoint invalido o incompleto."));
        return;
    }

    QNetworkRequest request(m_endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    m_busy = true;
    emit requestStateChanged(true);

    QNetworkReply *reply = m_networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        m_busy = false;
        emit requestStateChanged(false);

        if (reply->error() != QNetworkReply::NoError) {
            emit snapshotUpdated(buildErrorSnapshot(reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QByteArray body = reply->readAll();
        const QJsonDocument document = QJsonDocument::fromJson(body);

        if (!document.isObject()) {
            emit snapshotUpdated(buildErrorSnapshot("La respuesta no es un objeto JSON valido."));
            reply->deleteLater();
            return;
        }

        MetricSnapshot snapshot = buildSnapshotFromJson(document.object());
        snapshot.sourceEndpoint = m_endpoint.toString();
        snapshot.online = true;
        emit snapshotUpdated(snapshot);
        reply->deleteLater();
    });
}

MetricSnapshot MonitorService::buildSnapshotFromJson(const QJsonObject &root)
{
    MetricSnapshot snapshot;
    snapshot.checkedAt = extractDateTime(root, {"last_check", "checked_at", "timestamp", "time"});
    if (!snapshot.checkedAt.isValid()) {
        snapshot.checkedAt = QDateTime::currentDateTime();
    }

    const QJsonObject metrics = nestedObject(root, "metrics");
    const QJsonObject memory = nestedObject(root, "memory");
    const QJsonObject disk = nestedObject(root, "disk");
    const QJsonObject cpu = nestedObject(root, "cpu");
    const QJsonObject system = nestedObject(root, "system");

    const QJsonObject mergedRoot = root;

    snapshot.cpuLoadPercent = extractPercent(cpu, {"usage", "load", "percent"});
    if (snapshot.cpuLoadPercent <= 0.0) {
        snapshot.cpuLoadPercent = extractPercent(metrics, {"cpu", "cpu_usage", "cpu_percent", "load"});
    }
    if (snapshot.cpuLoadPercent <= 0.0) {
        snapshot.cpuLoadPercent = extractPercent(mergedRoot, {"cpu", "cpu_usage", "cpu_percent", "load"});
    }

    snapshot.memoryPercent = extractPercent(memory, {"usage", "used_percent", "percent"});
    if (snapshot.memoryPercent <= 0.0) {
        snapshot.memoryPercent = extractPercent(metrics, {"memory", "memory_usage", "memory_percent", "ram"});
    }
    if (snapshot.memoryPercent <= 0.0) {
        snapshot.memoryPercent = extractPercent(mergedRoot, {"memory", "memory_usage", "memory_percent", "ram"});
    }

    snapshot.diskPercent = extractPercent(disk, {"usage", "used_percent", "percent"});
    if (snapshot.diskPercent <= 0.0) {
        snapshot.diskPercent = extractPercent(metrics, {"disk", "disk_usage", "disk_percent", "storage"});
    }
    if (snapshot.diskPercent <= 0.0) {
        snapshot.diskPercent = extractPercent(mergedRoot, {"disk", "disk_usage", "disk_percent", "storage"});
    }

    snapshot.uptimeText = extractUptimeText(root, system, metrics);

    snapshot.overallStatus = inferOverallStatus(root,
                                                snapshot.cpuLoadPercent,
                                                snapshot.memoryPercent,
                                                snapshot.diskPercent);
    snapshot.summary = extractString(root, {"message", "summary", "status_message"});
    if (snapshot.summary.isEmpty()) {
        snapshot.summary = QString("CPU %1% | RAM %2% | DISCO %3%")
                               .arg(qRound(snapshot.cpuLoadPercent))
                               .arg(qRound(snapshot.memoryPercent))
                               .arg(qRound(snapshot.diskPercent));
    }

    snapshot.events = extractEvents(root, snapshot.overallStatus);
    snapshot.sourceEndpoint = m_endpoint.toString();
    return snapshot;
}

MetricSnapshot MonitorService::buildErrorSnapshot(const QString &message) const
{
    MetricSnapshot snapshot;
    snapshot.overallStatus = "CAIDO";
    snapshot.cpuLoadPercent = 0.0;
    snapshot.memoryPercent = 0.0;
    snapshot.diskPercent = 0.0;
    snapshot.uptimeText = "Sin conexion";
    snapshot.summary = message;
    snapshot.checkedAt = QDateTime::currentDateTime();
    snapshot.events = {
        "No se pudo consultar el servicio.",
        message,
        QString("Endpoint: %1").arg(m_endpoint.toString())
    };
    snapshot.sourceEndpoint = m_endpoint.toString();
    snapshot.online = false;
    return snapshot;
}

QString MonitorService::inferOverallStatus(const QJsonObject &root,
                                           double cpuPercent,
                                           double memoryPercent,
                                           double diskPercent) const
{
    const QString explicitStatus = extractString(root, {"status", "state", "overall_status", "health"});
    if (!explicitStatus.isEmpty()) {
        return normalizeStatus(explicitStatus);
    }

    if (cpuPercent >= 95.0 || memoryPercent >= 95.0 || diskPercent >= 95.0) {
        return "CAIDO";
    }

    if (cpuPercent >= m_alertThresholdPercent
        || memoryPercent >= m_alertThresholdPercent
        || diskPercent >= m_alertThresholdPercent) {
        return "ALERTA";
    }

    return "OK";
}

QString MonitorService::formatUptime(qint64 totalSeconds) const
{
    const qint64 days = totalSeconds / 86400;
    const qint64 hours = (totalSeconds % 86400) / 3600;
    const qint64 minutes = (totalSeconds % 3600) / 60;

    if (days > 0) {
        return QString("%1d %2h %3m").arg(days).arg(hours).arg(minutes);
    }

    if (hours > 0) {
        return QString("%1h %2m").arg(hours).arg(minutes);
    }

    return QString("%1m").arg(minutes);
}

QString MonitorService::extractUptimeText(const QJsonObject &root,
                                         const QJsonObject &system,
                                         const QJsonObject &metrics) const
{
    const QStringList textKeys = {
        "uptime_human", "uptime_text", "uptime", "server_uptime", "server_uptime_text"
    };

    const QString directText = extractString(root, textKeys);
    if (!directText.isEmpty()) {
        return directText;
    }

    const QString systemText = extractString(system, textKeys);
    if (!systemText.isEmpty()) {
        return systemText;
    }

    const QString metricsText = extractString(metrics, textKeys);
    if (!metricsText.isEmpty()) {
        return metricsText;
    }

    const QStringList numericKeys = {
        "uptime_seconds", "uptime", "uptime_sec", "server_uptime_seconds", "uptimeSeconds"
    };

    const qint64 directSeconds = extractInteger(root, numericKeys);
    if (directSeconds > 0) {
        return formatUptime(directSeconds);
    }

    const qint64 systemSeconds = extractInteger(system, numericKeys);
    if (systemSeconds > 0) {
        return formatUptime(systemSeconds);
    }

    const qint64 metricsSeconds = extractInteger(metrics, numericKeys);
    if (metricsSeconds > 0) {
        return formatUptime(metricsSeconds);
    }

    return "Sin dato";
}

QStringList MonitorService::extractEvents(const QJsonObject &root, const QString &status)
{
    QStringList events;
    const QJsonArray array = root.value("events").toArray();
    for (const QJsonValue &value : array) {
        if (value.isString()) {
            events << value.toString();
        } else if (value.isObject()) {
            const QJsonObject eventObject = value.toObject();
            const QString timestamp = extractString(eventObject, {"time", "timestamp"});
            const QString message = extractString(eventObject, {"message", "detail", "event"});
            if (!message.isEmpty()) {
                events << (timestamp.isEmpty() ? message : QString("%1 - %2").arg(timestamp, message));
            }
        }
    }

    if (events.isEmpty()) {
        events << QString("%1 | Estado %2").arg(QDateTime::currentDateTime().toString("HH:mm:ss"), status);
        events << QString("Chequeo automatico desde %1").arg(m_endpoint.host());
    }

    m_recentEvents = events + m_recentEvents;
    while (events.size() > 6) {
        events.removeLast();
    }
    while (m_recentEvents.size() > 6) {
        m_recentEvents.removeLast();
    }

    return m_recentEvents;
}

double MonitorService::extractPercent(const QJsonObject &root, const QStringList &keys) const
{
    double value = extractNumber(root, keys);
    if (value <= 1.0 && value > 0.0) {
        value *= 100.0;
    }

    return qBound(0.0, value, 100.0);
}

double MonitorService::extractNumber(const QJsonObject &root, const QStringList &keys) const
{
    for (const QString &key : keys) {
        const QJsonValue value = root.value(key);
        if (value.isDouble()) {
            return value.toDouble();
        }
        if (value.isString()) {
            bool ok = false;
            const double parsed = value.toString().remove('%').toDouble(&ok);
            if (ok) {
                return parsed;
            }
        }
    }
    return 0.0;
}

QString MonitorService::extractString(const QJsonObject &root, const QStringList &keys) const
{
    for (const QString &key : keys) {
        const QJsonValue value = root.value(key);
        if (value.isString()) {
            return value.toString();
        }
    }
    return {};
}

qint64 MonitorService::extractInteger(const QJsonObject &root, const QStringList &keys) const
{
    for (const QString &key : keys) {
        const QJsonValue value = root.value(key);
        if (value.isDouble()) {
            return static_cast<qint64>(value.toDouble());
        }
        if (value.isString()) {
            bool ok = false;
            const qint64 parsed = value.toString().toLongLong(&ok);
            if (ok) {
                return parsed;
            }
        }
    }
    return 0;
}

QDateTime MonitorService::extractDateTime(const QJsonObject &root, const QStringList &keys) const
{
    for (const QString &key : keys) {
        const QJsonValue value = root.value(key);
        if (!value.isString()) {
            continue;
        }

        const QString raw = value.toString();
        QDateTime parsed = QDateTime::fromString(raw, Qt::ISODate);
        if (!parsed.isValid()) {
            parsed = QDateTime::fromString(raw, "dd/MM/yyyy HH:mm:ss");
        }
        if (parsed.isValid()) {
            return parsed;
        }
    }

    return {};
}

QJsonObject MonitorService::nestedObject(const QJsonObject &object, const QString &key)
{
    return object.value(key).toObject();
}
