#include "logger.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

Logger::Logger(const QString &ruta, QObject *parent)
    : QObject(parent), m_ruta(ruta)
{
    QDir().mkpath(QFileInfo(m_ruta).absolutePath());
}

void Logger::registrar(const QString &descripcion)
{
    QFile archivo(m_ruta);
    if (!archivo.open(QIODevice::Append | QIODevice::Text)) {
        return;
    }

    QTextStream salida(&archivo);
    salida << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
           << " - " << descripcion << "\n";
}

