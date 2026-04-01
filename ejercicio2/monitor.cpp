#include "monitor.h"
#include <QDateTime>
#include <QDebug>

Monitor::Monitor(QObject *parent) : QObject(parent), process(new QProcess(this)), timer(new QTimer(this)), checkInterval(60) {
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Monitor::onProcessFinished);
    connect(timer, &QTimer::timeout, this, &Monitor::onTimerTimeout);
}

void Monitor::setCheckInterval(int seconds) {
    checkInterval = seconds;
    timer->setInterval(checkInterval * 1000);
    timer->start();
}

void Monitor::manualRefresh() {
    collectData();
}

void Monitor::collectData() {
    // Ejecutar comandos para recolectar datos
    // Primero uptime
    currentCommand = "uptime";
    process->start("powershell.exe", QStringList() << "-Command" << "$uptime = (Get-Date) - (Get-CimInstance Win32_OperatingSystem).LastBootUpTime; \"$($uptime.Days) days, $($uptime.Hours) hours\"");
}

void Monitor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        QString output = process->readAllStandardOutput().trimmed();
        QString error = process->readAllStandardError().trimmed();

        if (!error.isEmpty()) {
            emit errorOccurred("Command error: " + error);
            return;
        }

        static QString uptime, cpuLoad, memoryUsed, diskFree;

        if (currentCommand == "uptime") {
            uptime = output;
            // Siguiente: CPU
            currentCommand = "cpu";
            process->start("powershell.exe", QStringList() << "-Command" << "Get-CimInstance Win32_Processor | Measure-Object -Property LoadPercentage -Average | Select-Object -ExpandProperty Average");
        } else if (currentCommand == "cpu") {
            cpuLoad = output + "%";
            // Siguiente: Memoria usada
            currentCommand = "memory";
            process->start("powershell.exe", QStringList() << "-Command" << "Get-CimInstance Win32_OperatingSystem | ForEach-Object { [math]::Round(($_.TotalVisibleMemorySize - $_.FreePhysicalMemory) / 1MB, 1) }");
        } else if (currentCommand == "memory") {
            memoryUsed = output + " GB";
            // Siguiente: Disco libre
            currentCommand = "disk";
            process->start("powershell.exe", QStringList() << "-Command" << "Get-CimInstance Win32_LogicalDisk -Filter \"DeviceID='C:'\" | ForEach-Object { [math]::Round($_.FreeSpace / 1GB, 1) }");
        } else if (currentCommand == "disk") {
            diskFree = output + " GB";
            // Determinar status
            QString status = "OK";
            double cpuPercent = cpuLoad.remove('%').toDouble();
            if (cpuPercent > 95) {
                status = "Caído";
            } else if (cpuPercent > 80) {
                status = "Alerta";
            }
            QString lastCheck = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            emit dataReceived(status, uptime, cpuLoad, memoryUsed, diskFree, lastCheck);
        }
    } else {
        emit errorOccurred("Process failed: " + process->errorString());
    }
}

void Monitor::onTimerTimeout() {
    manualRefresh();
}