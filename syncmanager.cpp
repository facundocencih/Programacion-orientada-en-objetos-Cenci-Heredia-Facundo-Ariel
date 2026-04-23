#include "syncmanager.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>

SyncManager::SyncManager(DrawingModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_network(new QNetworkAccessManager(this))
    , m_pollTimer(new QTimer(this))
    , m_baseUrl(QStringLiteral("http://173.212.209.61:5002"))
{
    m_pollTimer->setInterval(2500);
    connect(m_pollTimer, &QTimer::timeout, this, &SyncManager::fetchRemoteSnapshot);
    connect(m_model, &DrawingModel::pendingSyncChanged, this, [this](int pendingCount) {
        if (pendingCount > 0 && !m_syncInFlight) {
            sendPendingStrokes();
        }
    });
}

void SyncManager::start()
{
    emit statusChanged(QStringLiteral("Conectando al VPS..."));
    checkServerHealth();
    m_pollTimer->start();
}

void SyncManager::saveNow()
{
    sendPendingStrokes();
}

void SyncManager::checkServerHealth()
{
    QNetworkRequest request(m_baseUrl.resolved(QUrl(QStringLiteral("/health"))));
    QNetworkReply *reply = m_network->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleHealthReply(reply);
    });
}

void SyncManager::fetchRemoteSnapshot()
{
    QNetworkRequest request(m_baseUrl.resolved(QUrl(QStringLiteral("/drawing"))));
    QNetworkReply *reply = m_network->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleSnapshotReply(reply);
    });
}

void SyncManager::sendPendingStrokes()
{
    if (m_syncInFlight) {
        return;
    }

    const QList<Stroke> pending = m_model->pendingStrokes();
    if (pending.isEmpty()) {
        emit statusChanged(QStringLiteral("Sin cambios pendientes"));
        fetchRemoteSnapshot();
        return;
    }

    m_syncInFlight = true;
    emit statusChanged(QStringLiteral("Guardando cambios en el VPS..."));

    QJsonArray strokesArray;
    QStringList sentIds;
    for (const Stroke &stroke : pending) {
        strokesArray.append(strokeToJson(stroke));
        sentIds.append(stroke.id);
    }

    QJsonObject payload;
    payload["strokes"] = strokesArray;

    QNetworkRequest request(m_baseUrl.resolved(QUrl(QStringLiteral("/drawing"))));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

    QNetworkReply *reply = m_network->post(request, QJsonDocument(payload).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, [this, reply, sentIds]() {
        handleMergeReply(reply, sentIds);
    });
}

void SyncManager::handleHealthReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit statusChanged(QStringLiteral("VPS no disponible, trabajando en modo local"));
        reply->deleteLater();
        return;
    }

    emit statusChanged(QStringLiteral("VPS en linea, sincronizando lienzo"));
    reply->deleteLater();
    fetchRemoteSnapshot();
}

void SyncManager::handleSnapshotReply(QNetworkReply *reply)
{
    const QByteArray data = reply->readAll();

    if (reply->error() != QNetworkReply::NoError) {
        emit statusChanged(QStringLiteral("VPS no disponible, trabajando en modo local"));
        reply->deleteLater();
        return;
    }

    const QJsonDocument document = QJsonDocument::fromJson(data);
    const bool merged = m_model->mergeFromJson(document);
    emit statusChanged(merged ? QStringLiteral("Lienzo remoto fusionado") : QStringLiteral("Lienzo remoto actualizado"));
    reply->deleteLater();
}

void SyncManager::handleMergeReply(QNetworkReply *reply, const QStringList &sentIds)
{
    m_syncInFlight = false;
    const QByteArray data = reply->readAll();

    if (reply->error() != QNetworkReply::NoError) {
        emit statusChanged(QStringLiteral("No se pudo guardar en el VPS"));
        reply->deleteLater();
        return;
    }

    m_model->markStrokesSynced(sentIds);
    const QJsonDocument document = QJsonDocument::fromJson(data);
    if (document.isObject()) {
        m_model->mergeFromJson(document);
    }

    emit statusChanged(QStringLiteral("Cambios guardados y fusionados"));
    reply->deleteLater();
    fetchRemoteSnapshot();
}
