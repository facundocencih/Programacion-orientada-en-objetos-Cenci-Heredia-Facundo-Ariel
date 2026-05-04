#include "login.h"
#include "ui_login.h"

#include <QLabel>
#include <QPalette>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QTimer>

Login::Login(const AppConfigData &config, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::Login),
      m_config(config),
      m_lockTimer(new QTimer(this)),
      m_remainingSeconds(0)
{
    ui->setupUi(this);
    connect(m_lockTimer, SIGNAL(timeout()), this, SLOT(actualizarCuentaRegresiva()));
}

Login::~Login()
{
    delete ui;
}

QString Login::nombrePantalla() const
{
    return "Login";
}

void Login::inicializar()
{
    setWindowTitle("Ejercicio 07 - Login con clima");
    ui->lblEstado->setText("Esperando validacion");
    ui->lblInfoClima->setText("Temperatura: --");
    ui->lblHora->setText("Hora local: --");
    ui->statusbar->showMessage("Listo");
}

void Login::aplicarTema(const QPixmap &imagen)
{
    m_background = imagen;
    refreshBackground();
}

void Login::setEstadoClima(const QString &temperatura, const QString &horaLocal, const QString &estado)
{
    ui->lblInfoClima->setText("Temperatura: " + temperatura);
    ui->lblHora->setText("Hora local: " + horaLocal);
    ui->lblEstado->setText(estado);
}

void Login::setMensajeEstado(const QString &mensaje, bool error)
{
    ui->statusbar->showMessage(mensaje, 5000);
    ui->lblMensaje->setText(mensaje);
    ui->lblMensaje->setStyleSheet(error
        ? "color: #ffccd2; background-color: rgba(120, 16, 33, 170); border-radius: 10px; padding: 10px;"
        : "color: #f5f7fa; background-color: rgba(18, 96, 70, 150); border-radius: 10px; padding: 10px;");
}

void Login::on_btnIngresar_clicked()
{
    emit loginSolicitado(ui->txtUsuario->text().trimmed(), ui->txtClave->text());
}

void Login::actualizarCuentaRegresiva()
{
    --m_remainingSeconds;
    if (m_remainingSeconds <= 0) {
        m_lockTimer->stop();
        ui->btnIngresar->setEnabled(true);
        ui->txtUsuario->setEnabled(true);
        ui->txtClave->setEnabled(true);
        setMensajeEstado("Ya podes volver a intentar.", false);
        return;
    }

    setMensajeEstado(QString("Demasiados intentos. Espera %1 segundos.").arg(m_remainingSeconds), true);
}

void Login::bloquearTemporalmente(int segundos)
{
    m_remainingSeconds = segundos;
    ui->btnIngresar->setEnabled(false);
    ui->txtUsuario->setEnabled(false);
    ui->txtClave->setEnabled(false);
    setMensajeEstado(QString("Demasiados intentos. Espera %1 segundos.").arg(m_remainingSeconds), true);
    m_lockTimer->start(1000);
}

void Login::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    refreshBackground();
}

void Login::refreshBackground()
{
    if (m_background.isNull()) {
        return;
    }

    const QPixmap scaled = m_background.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window, scaled);
    setAutoFillBackground(true);
    setPalette(palette);
}
