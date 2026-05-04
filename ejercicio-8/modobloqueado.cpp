#include "modobloqueado.h"

#include "logger.h"

#include <QCloseEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QProgressBar>
#include <QTimer>
#include <QVBoxLayout>

ModoBloqueado::ModoBloqueado(Logger *logger, QWidget *parent)
    : QWidget(parent), m_logger(logger), m_timer(new QTimer(this))
{
    inicializarUI();
    cargarDatos();
    conectarEventos();
}

void ModoBloqueado::iniciar(int segundos)
{
    m_total = qMax(1, segundos);
    m_restante = m_total;
    m_barra->setRange(0, m_total);
    m_barra->setValue(0);
    actualizarCuenta();
    m_timer->start(1000);
    registrarEvento(QString("Bloqueo iniciado por %1 segundos").arg(segundos));
}

QString ModoBloqueado::nombrePantalla() const
{
    return "ModoBloqueado";
}

void ModoBloqueado::inicializarUI()
{
    setWindowTitle("Acceso bloqueado");
    setFocusPolicy(Qt::StrongFocus);
    setStyleSheet("QWidget { background: #fff5f5; color: #4d1010; font-family: Segoe UI; }"
                  "QLabel#titulo { font-size: 26px; font-weight: 700; }"
                  "QProgressBar { border: 1px solid #d99; border-radius: 6px; height: 18px; text-align: center; }"
                  "QProgressBar::chunk { background: #d64545; border-radius: 6px; }");
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(56, 56, 56, 56);
    layout->setSpacing(18);

    auto *titulo = new QLabel("Bloqueo temporal", this);
    titulo->setObjectName("titulo");
    m_mensaje = new QLabel(this);
    m_mensaje->setWordWrap(true);
    m_barra = new QProgressBar(this);

    layout->addStretch();
    layout->addWidget(titulo);
    layout->addWidget(m_mensaje);
    layout->addWidget(m_barra);
    layout->addStretch();
}

void ModoBloqueado::conectarEventos()
{
    connect(m_timer, &QTimer::timeout, this, &ModoBloqueado::actualizarCuenta);
    connect(this, &ModoBloqueado::eventoRegistrado, m_logger, &Logger::registrar);
}

void ModoBloqueado::cargarDatos()
{
    m_mensaje->setText("Demasiados intentos fallidos. Esperá unos segundos antes de volver a intentar.");
}

bool ModoBloqueado::validarEstado() const
{
    return m_restante > 0;
}

void ModoBloqueado::registrarEvento(const QString &descripcion)
{
    emit eventoRegistrado(nombrePantalla() + ": " + descripcion);
}

void ModoBloqueado::actualizarCuenta()
{
    m_mensaje->setText(QString("Podrás volver al login en %1 segundo(s).").arg(m_restante));
    m_barra->setValue(m_total - m_restante);
    --m_restante;
    if (m_restante < 0) {
        m_timer->stop();
        registrarEvento("Bloqueo finalizado");
        emit bloqueoFinalizado();
    }
}

void ModoBloqueado::keyPressEvent(QKeyEvent *event)
{
    registrarEvento(QString("Tecla ignorada durante bloqueo: %1").arg(event->key()));
    event->accept();
}

void ModoBloqueado::mousePressEvent(QMouseEvent *event)
{
    registrarEvento(QString("Click informado durante bloqueo x=%1 y=%2").arg(event->position().x()).arg(event->position().y()));
    QWidget::mousePressEvent(event);
}

void ModoBloqueado::resizeEvent(QResizeEvent *event)
{
    registrarEvento(QString("Pantalla de bloqueo redimensionada a %1x%2").arg(width()).arg(height()));
    QWidget::resizeEvent(event);
}

void ModoBloqueado::closeEvent(QCloseEvent *event)
{
    registrarEvento("Intento de cierre durante bloqueo");
    QWidget::closeEvent(event);
}

void ModoBloqueado::focusInEvent(QFocusEvent *event)
{
    registrarEvento("Bloqueo recibio foco");
    QWidget::focusInEvent(event);
}

void ModoBloqueado::focusOutEvent(QFocusEvent *event)
{
    registrarEvento("Bloqueo perdio foco");
    QWidget::focusOutEvent(event);
}

