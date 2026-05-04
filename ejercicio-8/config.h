#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

struct Config
{
    QString usuario = "admin";
    QString password = "1234";
    int segundosBloqueo = 15;
    QString lenguajeDefault = "C++";
    QString rutaExportacion = "exports/codigo_exportado.jpg";
    QString rutaLog = "logs/eventos.log";

    static Config cargar(const QString &ruta);
};

#endif // CONFIG_H
