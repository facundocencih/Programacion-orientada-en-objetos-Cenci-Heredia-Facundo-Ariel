#ifndef LOGIN_H
#define LOGIN_H

#include "config.h"
#include "pantalla.h"

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;
class Logger;

class Login : public QWidget, public Pantalla
{
    Q_OBJECT

public:
    Login(const Config &config, Logger *logger, QWidget *parent = nullptr);

    QString nombrePantalla() const override;
    void inicializarUI() override;
    void conectarEventos() override;
    void cargarDatos() override;
    bool validarEstado() const override;
    void registrarEvento(const QString &descripcion) override;

signals:
    void loginCorrecto();
    void bloqueoSolicitado(int segundos);
    void eventoRegistrado(const QString &descripcion);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private slots:
    void intentarIngresar();

private:
    Config m_config;
    Logger *m_logger;
    QLabel *m_estado;
    QLineEdit *m_usuario;
    QLineEdit *m_password;
    QPushButton *m_boton;
    int m_intentosFallidos = 0;
};

#endif // LOGIN_H
