#ifndef CLIMA_H
#define CLIMA_H

#include "appconfig.h"
#include "pantalla.h"

#include <QObject>

class Logger;
class QNetworkAccessManager;
class QNetworkReply;

class Clima : public QObject, public Pantalla
{
    Q_OBJECT

public:
    explicit Clima(const AppConfigData &config,
                   QNetworkAccessManager *manager,
                   Logger *logger,
                   QObject *parent = nullptr);

    QString nombrePantalla() const override;
    void inicializar() override;
    void aplicarTema(const QPixmap &imagen) override;

signals:
    void climaActualizado(const QString &temperatura, const QString &horaLocal, const QString &estado);
    void errorAmigable(const QString &mensaje);

public slots:
    void consultarClima();

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    void startWeatherRequest(const QUrl &url, bool insecureFallback);
    void emitirOffline(const QString &motivo);

    AppConfigData m_config;
    QNetworkAccessManager *m_manager;
    Logger *m_logger;
    bool m_requestPending;
};

#endif // CLIMA_H
