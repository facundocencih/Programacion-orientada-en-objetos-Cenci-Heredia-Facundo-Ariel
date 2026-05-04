#include "wallpapermanager.h"

#include "logger.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QColor>
#include <QFont>
#include <QLinearGradient>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPainter>
#include <QPixmap>
#include <QSslSocket>
#include <QUrl>

WallpaperManager::WallpaperManager(const AppConfigData &config,
                                   const QString &cachePath,
                                   QNetworkAccessManager *manager,
                                   Logger *logger,
                                   QObject *parent)
    : QObject(parent),
      m_config(config),
      m_cachePath(cachePath),
      m_manager(manager),
      m_logger(logger),
      m_requestPending(false)
{
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
}

void WallpaperManager::ensureWallpaper()
{
    if (m_requestPending) {
        return;
    }

    QFile cacheFile(m_cachePath);
    if (cacheFile.exists()) {
        QPixmap cached;
        if (cached.load(m_cachePath)) {
            m_logger->write("Imagen de fondo cargada desde cache local.");
            emit wallpaperReady(cached);
            return;
        }
    }

    QUrl url(m_config.fondoUrl);
    const bool sslUnavailable = (url.scheme() == "https" && !QSslSocket::supportsSsl());
    if (sslUnavailable) {
        const QUrl secureUrl = url;
        url.setScheme("http");
        m_logger->write(QString("SSL no disponible en Qt. Reintentando imagen por HTTP: %1 -> %2")
                        .arg(secureUrl.toString(), url.toString()));
    }

    startWallpaperRequest(url, sslUnavailable);
}

void WallpaperManager::onReplyFinished(QNetworkReply *reply)
{
    if (reply->property("requestType").toString() != "wallpaper") {
        return;
    }

    m_requestPending = false;

    if (reply->error() != QNetworkReply::NoError) {
        const QUrl requestUrl = reply->request().url();
        const bool insecureFallback = reply->property("insecureFallback").toBool();

        if (!insecureFallback && requestUrl.scheme() == "https") {
            QUrl fallbackUrl = requestUrl;
            fallbackUrl.setScheme("http");
            m_logger->write(QString("Fallo HTTPS al descargar imagen (%1). Reintentando por HTTP.")
                            .arg(reply->errorString()));
            reply->deleteLater();
            startWallpaperRequest(fallbackUrl, true);
            return;
        }

        emitFallback(QString("Fallo la descarga de la imagen (%1).").arg(reply->errorString()));
        reply->deleteLater();
        return;
    }

    const QByteArray data = reply->readAll();
    QPixmap pixmap;
    if (!pixmap.loadFromData(data)) {
        emitFallback("La imagen descargada no pudo interpretarse.");
        reply->deleteLater();
        return;
    }

    QDir().mkpath(QFileInfo(m_cachePath).absolutePath());
    pixmap.save(m_cachePath);
    m_logger->write("Imagen principal descargada y guardada en cache.");
    emit wallpaperReady(pixmap);
    reply->deleteLater();
}

void WallpaperManager::startWallpaperRequest(const QUrl &url, bool insecureFallback)
{
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "Ejercicio7Qt/1.0");

    QNetworkReply *reply = m_manager->get(request);
    reply->setProperty("requestType", "wallpaper");
    reply->setProperty("insecureFallback", insecureFallback);
    m_requestPending = true;
    m_logger->write(QString("Descarga de imagen principal iniciada usando %1").arg(url.toString()));
}

void WallpaperManager::emitFallback(const QString &reason)
{
    m_logger->write(QString("Se usara fondo alternativo: %1").arg(reason));

    QPixmap fallback(1600, 900);
    fallback.fill(QColor("#12344d"));

    QPainter painter(&fallback);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QLinearGradient gradient(0, 0, fallback.width(), fallback.height());
    gradient.setColorAt(0.0, QColor("#0f2027"));
    gradient.setColorAt(0.5, QColor("#203a43"));
    gradient.setColorAt(1.0, QColor("#2c5364"));
    painter.fillRect(fallback.rect(), gradient);
    painter.setPen(QColor(255, 255, 255, 180));
    QFont titleFont("Segoe UI", 28, QFont::Bold);
    painter.setFont(titleFont);
    painter.drawText(QRect(80, 140, 900, 80), "Portfolio profesional");
    QFont bodyFont("Segoe UI", 14);
    painter.setFont(bodyFont);
    painter.drawText(QRect(80, 240, 920, 160),
                     Qt::TextWordWrap,
                     "Sin conexion al recurso remoto. Se muestra un fondo local elegante para mantener la experiencia completa.");
    painter.end();

    emit wallpaperReady(fallback);
    emit wallpaperError("No se pudo descargar la imagen remota. Se uso un fondo local.");
}
