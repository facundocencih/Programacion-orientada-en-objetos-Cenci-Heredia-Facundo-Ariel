#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include "appconfig.h"

#include <QObject>

class Clima;
class Logger;
class Login;
class QNetworkAccessManager;
class QPixmap;
class Ventana;
class WallpaperManager;

class AppController : public QObject
{
    Q_OBJECT

public:
    explicit AppController(QObject *parent = nullptr);
    ~AppController() override;

    void start();

private slots:
    void onLoginRequested(const QString &usuario, const QString &clave);
    void onWallpaperReady(const QPixmap &pixmap);
    void onWallpaperError(const QString &message);
    void onClimaActualizado(const QString &temperatura, const QString &horaLocal, const QString &estado);
    void onErrorAmigable(const QString &message);

private:
    void configureProxy();
    void openMainWindowIfReady();

    AppConfigData m_config;
    Logger *m_logger;
    QNetworkAccessManager *m_networkManager;
    Login *m_login;
    Ventana *m_ventana;
    Clima *m_clima;
    WallpaperManager *m_wallpaperManager;
    QPixmap *m_pendingWallpaper;
    int m_failedAttempts;
    bool m_authenticated;
};

#endif // APPCONTROLLER_H

