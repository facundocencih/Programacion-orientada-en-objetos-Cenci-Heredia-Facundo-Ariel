#include "appcontroller.h"

#include "appconfig.h"
#include "clima.h"
#include "logger.h"
#include "login.h"
#include "ventana.h"
#include "wallpapermanager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QPixmap>

namespace {
QString findConfigPath(const QString &startDir)
{
    QDir dir(startDir);

    for (int depth = 0; depth < 5; ++depth) {
        const QString candidate = dir.filePath("config.ini");
        if (QFile::exists(candidate)) {
            return candidate;
        }

        if (!dir.cdUp()) {
            break;
        }
    }

    return QDir(startDir).filePath("config.ini");
}
}

AppController::AppController(QObject *parent)
    : QObject(parent),
      m_logger(nullptr),
      m_networkManager(nullptr),
      m_login(nullptr),
      m_ventana(nullptr),
      m_clima(nullptr),
      m_wallpaperManager(nullptr),
      m_pendingWallpaper(new QPixmap),
      m_failedAttempts(0),
      m_authenticated(false)
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QString configPath = findConfigPath(appDir);

    AppConfig configReader(configPath);
    m_config = configReader.load();

    m_logger = new Logger(QDir(appDir).filePath("logs/actividad.log"));
    m_networkManager = new QNetworkAccessManager(this);

    configureProxy();

    m_login = new Login(m_config);
    m_ventana = new Ventana;
    m_clima = new Clima(m_config, m_networkManager, m_logger, this);
    m_wallpaperManager = new WallpaperManager(m_config,
                                              QDir(appDir).filePath("cache/fondo.jpg"),
                                              m_networkManager,
                                              m_logger,
                                              this);

    connect(m_login, SIGNAL(loginSolicitado(QString,QString)), this, SLOT(onLoginRequested(QString,QString)));
    connect(m_wallpaperManager, SIGNAL(wallpaperReady(QPixmap)), this, SLOT(onWallpaperReady(QPixmap)));
    connect(m_wallpaperManager, SIGNAL(wallpaperError(QString)), this, SLOT(onWallpaperError(QString)));
    connect(m_clima, SIGNAL(climaActualizado(QString,QString,QString)), this, SLOT(onClimaActualizado(QString,QString,QString)));
    connect(m_clima, SIGNAL(errorAmigable(QString)), this, SLOT(onErrorAmigable(QString)));
}

AppController::~AppController()
{
    delete m_login;
    delete m_ventana;
    delete m_pendingWallpaper;
    delete m_logger;
}

void AppController::start()
{
    m_login->inicializar();
    m_login->show();
    m_logger->write("Aplicacion iniciada.");

    m_clima->inicializar();
    m_wallpaperManager->ensureWallpaper();
}

void AppController::onLoginRequested(const QString &usuario, const QString &clave)
{
    if (usuario.trimmed().isEmpty() || clave.isEmpty()) {
        m_logger->write("Validacion rechazada: campos vacios en login.");
        m_login->setMensajeEstado("Completa usuario y clave antes de continuar.", true);
        return;
    }

    if (usuario == m_config.usuario && clave == m_config.clave) {
        m_authenticated = true;
        m_failedAttempts = 0;
        m_logger->write(QString("Login exitoso para usuario %1").arg(usuario));
        m_login->setMensajeEstado("Credenciales correctas. Cargando perfil...", false);
        openMainWindowIfReady();
        return;
    }

    ++m_failedAttempts;
    m_logger->write(QString("Intento fallido de login. Total acumulado: %1").arg(m_failedAttempts));
    m_login->setMensajeEstado("Usuario o clave incorrectos. Proba nuevamente.", true);

    if (m_failedAttempts >= 3) {
        m_failedAttempts = 0;
        m_login->bloquearTemporalmente(m_config.bloqueoSegundos);
        m_logger->write("Se aplico bloqueo temporal por 3 intentos fallidos.");
    }
}

void AppController::onWallpaperReady(const QPixmap &pixmap)
{
    *m_pendingWallpaper = pixmap;
    m_login->aplicarTema(pixmap);
    openMainWindowIfReady();
}

void AppController::onWallpaperError(const QString &message)
{
    m_login->setMensajeEstado(message, true);
}

void AppController::onClimaActualizado(const QString &temperatura, const QString &horaLocal, const QString &estado)
{
    m_login->setEstadoClima(temperatura, horaLocal, estado);
}

void AppController::onErrorAmigable(const QString &message)
{
    m_login->setMensajeEstado(message, true);
}

void AppController::configureProxy()
{
    if (!m_config.proxy.enabled || m_config.proxy.host.trimmed().isEmpty()) {
        m_logger->write("Proxy deshabilitado.");
        return;
    }

    QNetworkProxy proxy(QNetworkProxy::HttpProxy,
                        m_config.proxy.host,
                        m_config.proxy.port,
                        m_config.proxy.user,
                        m_config.proxy.password);
    m_networkManager->setProxy(proxy);
    m_logger->write(QString("Proxy habilitado en %1:%2").arg(m_config.proxy.host).arg(m_config.proxy.port));
}

void AppController::openMainWindowIfReady()
{
    if (!m_authenticated || m_pendingWallpaper->isNull() || m_ventana->isVisible()) {
        return;
    }

    m_ventana->aplicarTema(*m_pendingWallpaper);
    m_ventana->showFullScreen();
    m_login->hide();
    m_logger->write("Ventana principal abierta en pantalla completa.");
}
