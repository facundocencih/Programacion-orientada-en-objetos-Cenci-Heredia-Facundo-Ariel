#include "monitor.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QDebug>

Monitor::Monitor(QObject *parent) : QObject(parent), manager(new QNetworkAccessManager(this)), timer(new QTimer(this)), checkInterval(60), alertThreshold(80) {
    connect(manager, &QNetworkAccessManager::finished, this, &Monitor::onReplyFinished);
    connect(timer, &QTimer::timeout, this, &Monitor::onTimerTimeout);
}

void Monitor::setCheckInterval(int seconds) {
    checkInterval = seconds;
    timer->setInterval(checkInterval * 1000);
    timer->start();
}

void Monitor::setServerUrl(const QString &url) {
    serverUrl = QUrl(url);
}

void Monitor::setAlertThreshold(int threshold) {
    alertThreshold = threshold;
}

void Monitor::manualRefresh() {
    collectData();
}

void Monitor::collectData() {
    if (serverUrl.isValid()) {
        QNetworkRequest request(serverUrl);
        manager->get(request);
    } else {
        emit errorOccurred("URL del servidor no válida");
    }
}

void Monitor::onReplyFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            QString uptime = obj.value("uptime").toString("N/A");
            QString load = obj.value("load").toString("N/A");
            QString memoryUsed = obj.value("memory_used").toString("N/A");
            QString diskFree = obj.value("disk_free").toString("N/A");
            QString status = obj.value("status").toString("Desconocido");

            // Determinar status basado en load si no viene
            if (status == "Desconocido" && !load.isEmpty()) {
                double cpuPercent = load.remove('%').toDouble();
                if (cpuPercent > 95) {
                    status = "Caído";
                } else if (cpuPercent > alertThreshold) {
                    status = "Alerta";
                } else {
                    status = "OK";
                }
            }

            QString lastCheck = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            emit dataReceived(status, uptime, load, memoryUsed, diskFree, lastCheck);
        } else {
            emit errorOccurred("Respuesta JSON inválida");
        }
    } else {
        emit errorOccurred("Error de red: " + reply->errorString());
    }
    reply->deleteLater();
}

void Monitor::onTimerTimeout() {
    manualRefresh();
}