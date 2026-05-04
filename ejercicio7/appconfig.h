#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>

struct ProxySettings
{
    bool enabled = false;
    QString host;
    int port = 0;
    QString user;
    QString password;
};

struct AppConfigData
{
    QString apiBaseUrl;
    QString apiKey;
    QString ciudad;
    QString unidades;
    QString fondoUrl;
    QString usuario;
    QString clave;
    int bloqueoSegundos = 15;
    ProxySettings proxy;
};

class AppConfig
{
public:
    explicit AppConfig(const QString &configPath);

    AppConfigData load();
    QString configPath() const;

private:
    void ensureDefaultFile() const;

    QString m_configPath;
};

#endif // APPCONFIG_H

