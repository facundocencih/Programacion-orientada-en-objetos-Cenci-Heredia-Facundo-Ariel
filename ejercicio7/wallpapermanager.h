#ifndef WALLPAPERMANAGER_H
#define WALLPAPERMANAGER_H

#include "appconfig.h"

#include <QObject>

class Logger;
class QNetworkAccessManager;
class QNetworkReply;
class QPixmap;

class WallpaperManager : public QObject
{
    Q_OBJECT

public:
    explicit WallpaperManager(const AppConfigData &config,
                              const QString &cachePath,
                              QNetworkAccessManager *manager,
                              Logger *logger,
                              QObject *parent = nullptr);

    void ensureWallpaper();

signals:
    void wallpaperReady(const QPixmap &pixmap);
    void wallpaperError(const QString &message);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    void startWallpaperRequest(const QUrl &url, bool insecureFallback);
    void emitFallback(const QString &reason);

    AppConfigData m_config;
    QString m_cachePath;
    QNetworkAccessManager *m_manager;
    Logger *m_logger;
    bool m_requestPending;
};

#endif // WALLPAPERMANAGER_H
