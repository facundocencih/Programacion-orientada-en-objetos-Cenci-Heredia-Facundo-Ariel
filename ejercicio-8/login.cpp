#include "login.h"

#include "logger.h"

#include <QCloseEvent>
#include <QFocusEvent>
#include <QFormLayout>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>

Login::Login(const Config &config, Logger *logger, QWidget *parent)
    : QWidget(parent), m_config(config), m_logger(logger)
{
    inicializarUI();
    cargarDatos();
    conectarEventos();
}

QString Login::nombrePantalla() const
{
    return "Login";
}

void Login::inicializarUI()
{
    setWindowTitle("Editor multilenguaje - Login");
    setFocusPolicy(Qt::StrongFocus);
    setStyleSheet("QWidget { background: #f4f7fb; color: #18212f; font-family: Segoe UI; }"
                  "QLineEdit { padding: 10px; border: 1px solid #bac6d5; border-radius: 6px; background: white; }"
                  "QPushButton { padding: 10px 18px; background: #146c94; color: white; border: 0; border-radius: 6px; }"
                  "QLabel#titulo { font-size: 24px; font-weight: 700; }");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(64, 54, 64, 54);
    layout->setSpacing(18);

    auto *titulo = new QLabel("Editor multilenguaje", this);
    titulo->setObjectName("titulo");
    auto *subtitulo = new QLabel("Ingreso inicial requerido", this);

    m_usuario = new QLineEdit(this);
    m_password = new QLineEdit(this);
    m_password->setEchoMode(QLineEdit::Password);
    m_boton = new QPushButton("Ingresar", this);
    m_estado = new QLabel(this);
    m_estado->setWordWrap(true);

    auto *form = new QFormLayout;
    form->addRow("Usuario", m_usuario);
    form->addRow("Password", m_password);

    layout->addStretch();
    layout->addWidget(titulo);
    layout->addWidget(subtitulo);
    layout->addLayout(form);
    layout->addWidget(m_boton);
    layout->addWidget(m_estado);
    layout->addStretch();
}

void Login::conectarEventos()
{
    connect(m_boton, &QPushButton::clicked, this, &Login::intentarIngresar);
    connect(m_password, &QLineEdit::returnPressed, this, &Login::intentarIngresar);
    connect(this, &Login::eventoRegistrado, m_logger, &Logger::registrar);
}

void Login::cargarDatos()
{
    m_usuario->setText(m_config.usuario);
    m_estado->setText("Usuario de prueba: admin / 1234");
}

bool Login::validarEstado() const
{
    return !m_usuario->text().trimmed().isEmpty() && !m_password->text().isEmpty();
}

void Login::registrarEvento(const QString &descripcion)
{
    emit eventoRegistrado(nombrePantalla() + ": " + descripcion);
}

void Login::intentarIngresar()
{
    if (!validarEstado()) {
        m_estado->setText("Completa usuario y password para continuar.");
        registrarEvento("Intento incompleto");
        return;
    }

    if (m_usuario->text() == m_config.usuario && m_password->text() == m_config.password) {
        registrarEvento("Login valido");
        emit loginCorrecto();
        return;
    }

    ++m_intentosFallidos;
    m_estado->setText(QString("Credenciales incorrectas. Intento %1 de 3.").arg(m_intentosFallidos));
    registrarEvento(QString("Login fallido numero %1").arg(m_intentosFallidos));

    if (m_intentosFallidos >= 3) {
        m_intentosFallidos = 0;
        emit bloqueoSolicitado(m_config.segundosBloqueo);
    }
}

void Login::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        registrarEvento("Escape presionado en login");
        close();
        return;
    }
    QWidget::keyPressEvent(event);
}

void Login::mousePressEvent(QMouseEvent *event)
{
    registrarEvento(QString("Click en login x=%1 y=%2").arg(event->position().x()).arg(event->position().y()));
    QWidget::mousePressEvent(event);
}

void Login::resizeEvent(QResizeEvent *event)
{
    registrarEvento(QString("Login redimensionado a %1x%2").arg(width()).arg(height()));
    QWidget::resizeEvent(event);
}

void Login::closeEvent(QCloseEvent *event)
{
    registrarEvento("Cierre solicitado desde login");
    QWidget::closeEvent(event);
}

void Login::focusInEvent(QFocusEvent *event)
{
    registrarEvento("Login recibio foco");
    QWidget::focusInEvent(event);
}

void Login::focusOutEvent(QFocusEvent *event)
{
    registrarEvento("Login perdio foco");
    QWidget::focusOutEvent(event);
}
