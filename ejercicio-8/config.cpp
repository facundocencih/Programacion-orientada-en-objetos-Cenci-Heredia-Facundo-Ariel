#include "config.h"

#include <QSettings>

Config Config::cargar(const QString &ruta)
{
    Config config;
    QSettings settings(ruta, QSettings::IniFormat);

    config.usuario = settings.value("auth/user", config.usuario).toString();
    config.password = settings.value("auth/password", config.password).toString();
    config.segundosBloqueo = settings.value("auth/lockSeconds", config.segundosBloqueo).toInt();
    config.lenguajeDefault = settings.value("editor/defaultLanguage", config.lenguajeDefault).toString();
    config.rutaExportacion = settings.value("editor/exportPath", config.rutaExportacion).toString();
    config.rutaLog = settings.value("log/path", config.rutaLog).toString();

    return config;
}

