#include "widget.h"
#include "ui_widget.h"

#include <QKeyEvent>
#include <QPainter>
#include <QRandomGenerator>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget),
      trex(new TRex(this)),
      sueloY(430),
      velocidadBase(7),
      distancia(0),
      puntaje(0),
      proximoCactusX(0),
      nubeX(940),
      nubeY(72),
      avanzando(false),
      frenando(false),
      juegoTerminado(false)
{
    ui->setupUi(this);
    setFixedSize(900, 520);
    setWindowTitle("Ejercicio 10 - T-Rex Extremo");
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_OpaquePaintEvent);

    crearSprites();

    timerPrincipal.setTimerType(Qt::PreciseTimer);
    timerPajaros.setTimerType(Qt::CoarseTimer);
    connect(&timerPrincipal, SIGNAL(timeout()), this, SLOT(actualizarJuego()));
    connect(&timerPajaros, SIGNAL(timeout()), this, SLOT(crearPajaro()));

    iniciarJuego();
}

Widget::~Widget()
{
    limpiarPajaros();
    delete ui;
}

void Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    dibujarFondo(painter);

    for (const Cactus &c : cactus) {
        painter.drawImage(c.rect, c.sprite);
    }

    painter.setPen(QColor(37, 47, 56));
    painter.setFont(QFont("Consolas", 18, QFont::Bold));
    painter.drawText(QRect(width() - 260, 24, 220, 34), Qt::AlignRight, QString("Puntos: %1").arg(puntaje));

    painter.setFont(QFont("Consolas", 10, QFont::Bold));
    painter.setPen(QColor(89, 103, 113));
    painter.drawText(24, 34, "ESPACIO salto   ABAJO agacharse   DERECHA adelantarse   IZQUIERDA frenarse");

    if (juegoTerminado) {
        painter.fillRect(rect(), QColor(255, 255, 255, 160));
        const QRect destino(width() / 2 - 210, height() / 2 - 115, 420, 160);
        painter.drawImage(destino, imagenGameOver);
        painter.setPen(QColor(50, 59, 67));
        painter.setFont(QFont("Consolas", 16, QFont::Bold));
        painter.drawText(QRect(0, height() / 2 + 70, width(), 40),
                         Qt::AlignCenter,
                         "Presiona R o Espacio para reiniciar");
    }
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        return;
    }

    if (juegoTerminado) {
        if (event->key() == Qt::Key_R || event->key() == Qt::Key_Space) {
            iniciarJuego();
        }
        return;
    }

    if (event->key() == Qt::Key_Space) {
        trex->saltar();
    } else if (event->key() == Qt::Key_Down) {
        trex->agacharse(true);
    } else if (event->key() == Qt::Key_Right) {
        avanzando = true;
        trex->adelantar(true);
    } else if (event->key() == Qt::Key_Left) {
        frenando = true;
        trex->frenar(true);
    }
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat() || juegoTerminado) {
        return;
    }

    if (event->key() == Qt::Key_Down) {
        trex->agacharse(false);
    } else if (event->key() == Qt::Key_Right) {
        avanzando = false;
        trex->adelantar(false);
    } else if (event->key() == Qt::Key_Left) {
        frenando = false;
        trex->frenar(false);
    }
}

void Widget::actualizarJuego()
{
    if (avanzando) {
        trex->move(qMin(trex->x() + 4, 180), trex->y());
    }
    if (frenando) {
        trex->move(qMax(trex->x() - 4, 40), trex->y());
    }

    trex->actualizarFisica();
    actualizarCactus();
    actualizarPajaros();
    actualizarNube();

    distancia += velocidadActual();
    puntaje = distancia / 10;
    velocidadBase = velocidadActual();
    if (timerPajaros.interval() != intervaloPajaros()) {
        timerPajaros.setInterval(intervaloPajaros());
    }

    detectarColisiones();
    update();
}

void Widget::crearPajaro()
{
    if (juegoTerminado) {
        return;
    }

    if (pajaros.size() >= maximoPajarosActivos()) {
        return;
    }

    const int tipo = QRandomGenerator::global()->bounded(3);
    Pajaro *pajaro = new Pajaro(velocidadActual(), tipo, this);
    const int altura = QRandomGenerator::global()->bounded(sueloY - 112, sueloY - 84);
    int x = width() + 170;
    x = qMax(x, ultimaPosicionCactus() + distanciaAntesDePajaro());
    x = qMax(x, ultimaPosicionPajaro() + distanciaEntrePajaros());
    pajaro->move(x, altura);
    pajaro->show();
    connect(pajaro, SIGNAL(salioDePantalla(Pajaro*)), this, SLOT(eliminarPajaro(Pajaro*)));
    pajaros.append(pajaro);
}

void Widget::eliminarPajaro(Pajaro *pajaro)
{
    pajaros.removeAll(pajaro);
    pajaro->deleteLater();
}

void Widget::crearObstaculoSorpresa()
{
    if (!juegoTerminado) {
        crearCactus();
    }
}

void Widget::iniciarJuego()
{
    juegoTerminado = false;
    distancia = 0;
    puntaje = 0;
    velocidadBase = 7;
    nubeX = width() + 80;
    nubeY = QRandomGenerator::global()->bounded(58, 105);
    avanzando = false;
    frenando = false;
    cactus.clear();
    limpiarPajaros();
    trex->reiniciar(sueloY);
    proximoCactusX = width() + 360;
    crearCactus();

    reloj.restart();
    timerPrincipal.start(25);
    timerPajaros.start(intervaloPajaros());
    QTimer::singleShot(4500, this, SLOT(crearObstaculoSorpresa()));
    setFocus(Qt::OtherFocusReason);
    update();
}

void Widget::terminarJuego()
{
    juegoTerminado = true;
    timerPrincipal.stop();
    timerPajaros.stop();
    trex->morir();

    for (Pajaro *pajaro : std::as_const(pajaros)) {
        pajaro->detener();
    }

    update();
}

void Widget::crearSprites()
{
    spritesCactus.clear();
    for (int i = 0; i < 3; ++i) {
        spritesCactus.append(crearSpriteCactus(i));
    }
    imagenGameOver = crearImagenGameOver();
}

QImage Widget::crearSpriteCactus(int tipo) const
{
    QImage image(62, 92, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter p(&image);
    p.setPen(Qt::NoPen);

    const QColor cuerpo = tipo == 0 ? QColor(57, 151, 112) : (tipo == 1 ? QColor(44, 128, 91) : QColor(98, 173, 138));
    p.setBrush(cuerpo);

    const int x = tipo == 2 ? 24 : 27;
    const int alto = tipo == 1 ? 82 : 72;
    p.drawRect(QRect(x, 92 - alto, 14, alto));
    p.drawRect(QRect(x - 18, 34, 12, 32));
    p.drawRect(QRect(x - 18, 34, 24, 10));
    p.drawRect(QRect(x + 14, 43, 12, 27));
    p.drawRect(QRect(x + 2, 43, 24, 10));

    if (tipo > 0) {
        p.drawRect(QRect(7, 52, 11, 40));
        p.drawRect(QRect(42, 58, 11, 34));
    }

    p.setBrush(QColor(185, 225, 204));
    p.drawRect(QRect(x + 4, 92 - alto + 8, 3, alto - 14));

    return image;
}

QImage Widget::crearImagenGameOver() const
{
    QImage image(420, 160, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter p(&image);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(37, 47, 56));
    p.drawRoundedRect(QRect(10, 14, 400, 122), 10, 10);
    p.setBrush(QColor(239, 73, 73));
    p.drawRoundedRect(QRect(22, 26, 376, 98), 8, 8);
    p.setPen(Qt::white);
    p.setFont(QFont("Consolas", 42, QFont::Black));
    p.drawText(QRect(0, 32, 420, 60), Qt::AlignCenter, "GAME OVER");
    p.setFont(QFont("Consolas", 15, QFont::Bold));
    p.drawText(QRect(0, 92, 420, 32), Qt::AlignCenter, "T-Rex Extremo");

    return image;
}

void Widget::crearCactus()
{
    const int tipo = QRandomGenerator::global()->bounded(spritesCactus.size());
    const QSize tamano(tipo == 1 ? 70 : 58, tipo == 1 ? 96 : 84);
    Cactus nuevo;
    nuevo.sprite = spritesCactus.at(tipo);

    proximoCactusX = qMax(proximoCactusX, ultimaPosicionCactus() + distanciaCactus());
    proximoCactusX = qMax(proximoCactusX, ultimaPosicionPajaro() + distanciaAntesDePajaro());
    nuevo.rect = QRect(proximoCactusX, sueloY - tamano.height(), tamano.width(), tamano.height());
    cactus.append(nuevo);
    proximoCactusX = nuevo.rect.right() + distanciaCactus();
}

void Widget::actualizarCactus()
{
    const int velocidad = velocidadActual();
    for (Cactus &c : cactus) {
        c.rect.translate(-velocidad, 0);
    }

    while (!cactus.isEmpty() && cactus.first().rect.right() < 0) {
        cactus.removeFirst();
    }

    const int ventanaAdelantada = 260 + nivelDificultad() * 26;
    if (cactus.isEmpty() || cactus.last().rect.x() < width() + ventanaAdelantada) {
        proximoCactusX = qMax(width() + QRandomGenerator::global()->bounded(80, 170),
                              ultimaPosicionCactus() + distanciaCactus());
        crearCactus();
    }
}

void Widget::actualizarPajaros()
{
    const int velocidad = velocidadActual();
    for (Pajaro *pajaro : std::as_const(pajaros)) {
        pajaro->setVelocidad(velocidad);
    }
}

void Widget::actualizarNube()
{
    nubeX -= 1;
    if (nubeX + 125 < 0) {
        nubeX = width() + QRandomGenerator::global()->bounded(80, 220);
        nubeY = QRandomGenerator::global()->bounded(58, 105);
    }
}

void Widget::detectarColisiones()
{
    const QRect areaTrex = trex->areaColision();
    for (const Cactus &c : std::as_const(cactus)) {
        const QRect areaCactus = c.rect.adjusted(8, 8, -8, -4);
        if (areaTrex.intersects(areaCactus)) {
            terminarJuego();
            return;
        }
    }

    for (Pajaro *pajaro : std::as_const(pajaros)) {
        if (areaTrex.intersects(pajaro->areaColision())) {
            terminarJuego();
            return;
        }
    }
}

void Widget::limpiarPajaros()
{
    for (Pajaro *pajaro : std::as_const(pajaros)) {
        pajaro->deleteLater();
    }
    pajaros.clear();
}

void Widget::dibujarFondo(QPainter &painter)
{
    painter.fillRect(rect(), QColor(249, 251, 248));

    painter.setPen(QPen(QColor(188, 202, 199), 2));
    painter.drawLine(0, sueloY, width(), sueloY);

    painter.setPen(QPen(QColor(213, 224, 221), 1));
    const int desplazamiento = (distancia / 3) % 140;
    for (int x = -desplazamiento; x < width(); x += 140) {
        painter.drawLine(x, sueloY + 22, x + 52, sueloY + 22);
        painter.drawLine(x + 84, sueloY + 36, x + 126, sueloY + 36);
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(229, 236, 233));
    painter.drawRect(QRect(nubeX, nubeY + 12, 118, 18));
    painter.drawRect(QRect(nubeX + 24, nubeY + 4, 36, 16));
    painter.drawRect(QRect(nubeX + 58, nubeY, 44, 20));
}

int Widget::ultimaPosicionCactus() const
{
    int ultima = width() + 80;
    for (const Cactus &c : cactus) {
        ultima = qMax(ultima, c.rect.right());
    }
    return ultima;
}

int Widget::ultimaPosicionPajaro() const
{
    int ultima = width() + 80;
    for (const Pajaro *pajaro : pajaros) {
        ultima = qMax(ultima, pajaro->geometry().right());
    }
    return ultima;
}

int Widget::distanciaCactus() const
{
    const int nivel = nivelDificultad();
    const int minima = qMax(340, 340 + velocidadActual() * 8 - nivel * 10);
    const int maxima = qMax(minima + 70, 520 + velocidadActual() * 10 - nivel * 14);
    return QRandomGenerator::global()->bounded(minima, maxima + 1);
}

int Widget::distanciaAntesDePajaro() const
{
    return qMax(500, 650 + velocidadActual() * 4 - nivelDificultad() * 18);
}

int Widget::distanciaEntrePajaros() const
{
    return qMax(560, 760 + velocidadActual() * 4 - nivelDificultad() * 20);
}

int Widget::intervaloPajaros() const
{
    return qMax(2600, 5200 - nivelDificultad() * 220);
}

int Widget::maximoPajarosActivos() const
{
    return nivelDificultad() >= 7 ? 3 : 2;
}

int Widget::nivelDificultad() const
{
    return qMin(12, puntaje / 220);
}

int Widget::velocidadActual() const
{
    return qMin(25, 7 + puntaje / 160);
}
