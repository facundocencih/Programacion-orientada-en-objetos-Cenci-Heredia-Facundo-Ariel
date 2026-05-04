#include "clima.h"

#include "logger.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPixmap>
#include <QSslSocket>
#include <QUrl>
#include <QUrlQuery>

Clima::Clima(const AppConfigData &config,
             QNetworkAccessManager *manager,
             Logger *logger,
             QObject *parent)
    : QObject(parent),
      m_config(config),
      m_manager(manager),
      m_logger(logger),
      m_requestPending(false)
{
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
}

QString Clima::nombrePantalla() const
{
    return "Clima";
}

void Clima::inicializar()
{
    consultarClima();
}

void Clima::aplicarTema(const QPixmap &)
{
}

void Clima::consultarClima()
{
    if (m_requestPending) {
        return;
    }

    if (m_config.apiKey.trimmed().isEmpty() || m_config.apiKey == "COLOCA_TU_API_KEY") {
        emitirOffline("No se encontro una API key valida. Se usan datos simulados.");
        return;
    }

    QUrl url(m_config.apiBaseUrl);
    QUrlQuery query;
    query.addQueryItem("q", m_config.ciudad);
    query.addQueryItem("units", m_config.unidades);
    query.addQueryItem("appid", m_config.apiKey);
    url.setQuery(query);

    const bool sslUnavailable = (url.scheme() == "https" && !QSslSocket::supportsSsl());
    if (sslUnavailable) {
        const QUrl secureUrl = url;
        url.setScheme("http");
        m_logger->write(QString("SSL no disponible en Qt. Reintentando clima por HTTP: %1 -> %2")
                        .arg(secureUrl.toString(), url.toString()));
    }

    startWeatherRequest(url, sslUnavailable);
}

void Clima::onReplyFinished(QNetworkReply *reply)
{
    if (reply->property("requestType").toString() != "weather") {
        return;
    }

    m_requestPending = false;

    if (reply->error() != QNetworkReply::NoError) {
        const QUrl requestUrl = reply->request().url();
        const bool insecureFallback = reply->property("insecureFallback").toBool();

        if (!insecureFallback && requestUrl.scheme() == "https") {
            QUrl fallbackUrl = requestUrl;
            fallbackUrl.setScheme("http");
            m_logger->write(QString("Fallo HTTPS al consultar clima (%1). Reintentando por HTTP.")
                            .arg(reply->errorString()));
            reply->deleteLater();
            startWeatherRequest(fallbackUrl, true);
            return;
        }

        emitirOffline(QString("No hubo conexion con el servicio de clima (%1).").arg(reply->errorString()));
        reply->deleteLater();
        return;
    }

    const QByteArray payload = reply->readAll();
    const QJsonDocument doc = QJsonDocument::fromJson(payload);
    const QJsonObject root = doc.object();
    const QJsonObject mainObject = root.value("main").toObject();

    if (!doc.isObject() || mainObject.isEmpty() || !mainObject.contains("temp")) {
        m_logger->write("Respuesta invalida al consultar clima.");
        emit errorAmigable("La respuesta del clima fue invalida. Se muestran datos simulados.");
        emitirOffline("Respuesta JSON invalida.");
        reply->deleteLater();
        return;
    }

    const double temperatura = mainObject.value("temp").toDouble();
    const qint64 timezoneOffset = root.value("timezone").toInteger();
    const QDateTime localUtc = QDateTime::currentDateTimeUtc().addSecs(timezoneOffset);
    const QString horaLocal = localUtc.toString("hh:mm");
    const QString temperaturaTexto = QString::number(temperatura, 'f', 1) + " C";
    const QString estado = QString("Online - %1").arg(m_config.ciudad);

    m_logger->write(QString("Clima actualizado correctamente: %1").arg(temperaturaTexto));
    emit climaActualizado(temperaturaTexto, horaLocal, estado);
    reply->deleteLater();
}

void Clima::startWeatherRequest(const QUrl &url, bool insecureFallback)
{
    QNetworkRequest request(url);
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("User-Agent", "Ejercicio7Qt/1.0");

    QNetworkReply *reply = m_manager->get(request);
    reply->setProperty("requestType", "weather");
    reply->setProperty("insecureFallback", insecureFallback);
    m_requestPending = true;
    m_logger->write(QString("Consulta de clima iniciada para %1 usando %2")
                    .arg(m_config.ciudad, url.toString()));
}

void Clima::emitirOffline(const QString &motivo)
{
    const QString temperatura = "22.5 C";
    const QString hora = QDateTime::currentDateTime().toString("hh:mm");

    m_logger->write(QString("Modo offline activado: %1").arg(motivo));
    emit climaActualizado(temperatura, hora, "Offline - datos simulados");
    emit errorAmigable("No se pudo obtener el clima real. La app sigue funcionando con datos simulados.");
}
