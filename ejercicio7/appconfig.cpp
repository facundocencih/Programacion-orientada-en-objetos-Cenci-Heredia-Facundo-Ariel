#include "appconfig.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QTextStream>

AppConfig::AppConfig(const QString &configPath)
    : m_configPath(configPath)
{
}

AppConfigData AppConfig::load()
{
    ensureDefaultFile();

    QSettings settings(m_configPath, QSettings::IniFormat);
    AppConfigData data;

    data.apiBaseUrl = settings.value("api/baseUrl", "https://api.openweathermap.org/data/2.5/weather").toString();
    data.apiKey = settings.value("api/key", "COLOCA_TU_API_KEY").toString();
    data.ciudad = settings.value("api/city", "Cordoba,AR").toString();
    data.unidades = settings.value("api/units", "metric").toString();
    data.fondoUrl = settings.value("ui/backgroundUrl", "https://images.unsplash.com/photo-1500530855697-b586d89ba3ee?auto=format&fit=crop&w=1600&q=80").toString();
    data.usuario = settings.value("auth/user", "admin").toString();
    data.clave = settings.value("auth/password", "1234").toString();
    data.bloqueoSegundos = settings.value("auth/lockSeconds", 15).toInt();

    data.proxy.enabled = settings.value("proxy/enabled", false).toBool();
    data.proxy.host = settings.value("proxy/host", "").toString();
    data.proxy.port = settings.value("proxy/port", 8080).toInt();
    data.proxy.user = settings.value("proxy/user", "").toString();
    data.proxy.password = settings.value("proxy/password", "").toString();

    return data;
}

QString AppConfig::configPath() const
{
    return m_configPath;
}

void AppConfig::ensureDefaultFile() const
{
    if (QFile::exists(m_configPath)) {
        return;
    }

    QDir().mkpath(QFileInfo(m_configPath).absolutePath());

    QFile file(m_configPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    out << "[api]\n";
    out << "baseUrl=https://api.openweathermap.org/data/2.5/weather\n";
    out << "key=COLOCA_TU_API_KEY\n";
    out << "city=Cordoba,AR\n";
    out << "units=metric\n\n";
    out << "[ui]\n";
    out << "backgroundUrl=https://images.unsplash.com/photo-1500530855697-b586d89ba3ee?auto=format&fit=crop&w=1600&q=80\n\n";
    out << "[auth]\n";
    out << "user=admin\n";
    out << "password=1234\n";
    out << "lockSeconds=15\n\n";
    out << "[proxy]\n";
    out << "enabled=false\n";
    out << "host=\n";
    out << "port=8080\n";
    out << "user=\n";
    out << "password=\n";
}
