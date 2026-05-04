#include "logger.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

Logger::Logger(const QString &logPath)
    : m_logPath(logPath)
{
    QDir().mkpath(QFileInfo(m_logPath).absolutePath());
}

void Logger::write(const QString &message)
{
    QFile file(m_logPath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
        << " - " << message << "\n";
}

QString Logger::logPath() const
{
    return m_logPath;
}

