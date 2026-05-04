#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include "config.h"
#include "pantalla.h"

#include <QObject>

class EditorPrincipal;
class Logger;
class Login;
class ModoBloqueado;
class QWidget;

class AppController : public QObject
{
    Q_OBJECT

public:
    explicit AppController(QObject *parent = nullptr);
    void iniciar();

private slots:
    void mostrarEditor();
    void mostrarBloqueo(int segundos);
    void volverAlLogin();

private:
    void cambiarPantalla(Pantalla *pantalla, QWidget *widget, bool fullScreen = false);

    Config m_config;
    Logger *m_logger;
    Login *m_login;
    EditorPrincipal *m_editor;
    ModoBloqueado *m_bloqueado;
    Pantalla *m_actual = nullptr;
    QWidget *m_widgetActual = nullptr;
};

#endif // APPCONTROLLER_H
