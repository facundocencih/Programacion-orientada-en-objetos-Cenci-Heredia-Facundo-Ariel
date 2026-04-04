#include "monitor.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QDebug>

Monitor::Monitor(QObject *parent) : QObject(parent), process(new QProcess(this)), timer(new QTimer(this)), checkInterval(60), alertThreshold(80) {
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Monitor::onProcessFinished);
    connect(timer, &QTimer::timeout, this, &Monitor::onTimerTimeout);
}

void Monitor::setCheckInterval(int seconds) {
    checkInterval = seconds;
    timer->setInterval(checkInterval * 1000);
    timer->start();
}

void Monitor::setServerUrl(const QString &url) {
    serverHost = url;
}

void Monitor::setAlertThreshold(int threshold) {
    alertThreshold = threshold;
}

void Monitor::setSshCredentials(const QString &user, const QString &password) {
    sshUser = user;
    sshPassword = password;
}

void Monitor::manualRefresh() {
    collectData();
}

void Monitor::collectData() {
    if (serverHost.isEmpty() || sshUser.isEmpty()) {
        emit errorOccurred("Host o credenciales SSH no configuradas");
        return;
    }
    // Run ssh command to get all data at once
    QString command = QString("sshpass -p '%1' ssh -o StrictHostKeyChecking=no %2@%3 \"uptime -p && uptime | awk -F'load average:' '{ print \$2 }' | cut -d, -f1 && free -h | grep Mem | awk '{print \$3}' && df -h / | tail -1 | awk '{print \$4}'\"").arg(sshPassword, sshUser, serverHost);
    process->start(command);
}

void Monitor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        QByteArray output = process->readAllStandardOutput();
        QString data = QString::fromUtf8(output).trimmed();
        QStringList lines = data.split('\n');
        if (lines.size() >= 4) {
            QString uptime = lines[0];
            QString load = lines[1] + '%';
            QString memoryUsed = lines[2];
            QString diskFree = lines[3];
            QString status = "OK";

            // Determine status based on load
            double cpuPercent = load.remove('%').toDouble();
            if (cpuPercent > 95) {
                status = "Caído";
            } else if (cpuPercent > alertThreshold) {
                status = "Alerta";
            }

            QString lastCheck = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            emit dataReceived(status, uptime, load, memoryUsed, diskFree, lastCheck);
        } else {
            emit errorOccurred("Salida SSH inválida");
        }
    } else {
        QByteArray error = process->readAllStandardError();
        emit errorOccurred("Error SSH: " + QString::fromUtf8(error));
    }
}

void Monitor::onTimerTimeout() {
    manualRefresh();
}