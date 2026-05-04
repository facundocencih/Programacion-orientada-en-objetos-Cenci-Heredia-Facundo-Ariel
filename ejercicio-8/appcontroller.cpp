#include "appcontroller.h"

#include "editorprincipal.h"
#include "logger.h"
#include "login.h"
#include "modobloqueado.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QWidget>

static QString rutaConfig()
{
    const QString juntoAlEjecutable = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("config.ini");
    if (QFile::exists(juntoAlEjecutable)) {
        return juntoAlEjecutable;
    }
    const QString directorioTrabajo = QDir::current().absoluteFilePath("config.ini");
    if (QFile::exists(directorioTrabajo)) {
        return directorioTrabajo;
    }
    return juntoAlEjecutable;
}

AppController::AppController(QObject *parent)
    : QObject(parent),
      m_config(Config::cargar(rutaConfig())),
      m_logger(new Logger(m_config.rutaLog, this)),
      m_login(new Login(m_config, m_logger)),
      m_editor(new EditorPrincipal(m_config, m_logger)),
      m_bloqueado(new ModoBloqueado(m_logger))
{
    connect(m_login, &Login::loginCorrecto, this, &AppController::mostrarEditor);
    connect(m_login, &Login::bloqueoSolicitado, this, &AppController::mostrarBloqueo);
    connect(m_bloqueado, &ModoBloqueado::bloqueoFinalizado, this, &AppController::volverAlLogin);
}

void AppController::iniciar()
{
    cambiarPantalla(m_login, m_login);
}

void AppController::mostrarEditor()
{
    cambiarPantalla(m_editor, m_editor, true);
}

void AppController::mostrarBloqueo(int segundos)
{
    m_bloqueado->iniciar(segundos);
    cambiarPantalla(m_bloqueado, m_bloqueado);
}

void AppController::volverAlLogin()
{
    cambiarPantalla(m_login, m_login);
}

void AppController::cambiarPantalla(Pantalla *pantalla, QWidget *widget, bool fullScreen)
{
    if (m_widgetActual) {
        m_widgetActual->hide();
    }

    m_actual = pantalla;
    m_widgetActual = widget;
    m_actual->registrarEvento("Pantalla activa mediante puntero base Pantalla");

    if (fullScreen) {
        widget->showFullScreen();
    } else {
        widget->resize(520, 360);
        widget->show();
    }
}
