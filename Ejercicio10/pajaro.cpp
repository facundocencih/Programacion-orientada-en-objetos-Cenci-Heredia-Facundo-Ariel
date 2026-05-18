#include "pajaro.h"

#include <QPainter>

Pajaro::Pajaro(int velocidadInicial, int tipoInicial, QWidget *parent)
    : QWidget(parent),
      velocidad(velocidadInicial),
      tipo(tipoInicial),
      frameActual(0)
{
    setFixedSize(tipo == 2 ? QSize(82, 54) : QSize(72, 48));
    setFocusPolicy(Qt::NoFocus);
    crearSprites();
    timerMovimiento.setTimerType(Qt::PreciseTimer);
    timerAnimacion.setTimerType(Qt::CoarseTimer);
    connect(&timerMovimiento, SIGNAL(timeout()), this, SLOT(avanzar()));
    connect(&timerAnimacion, SIGNAL(timeout()), this, SLOT(animarAlas()));
    timerMovimiento.start(25);
    timerAnimacion.start(125);
}

void Pajaro::setVelocidad(int nuevaVelocidad)
{
    velocidad = nuevaVelocidad;
}

void Pajaro::detener()
{
    timerMovimiento.stop();
    timerAnimacion.stop();
}

QRect Pajaro::areaColision() const
{
    return QRect(x() + 8, y() + 10, width() - 16, height() - 18);
}

void Pajaro::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawPixmap(rect(), sprites.at(frameActual % sprites.size()));
}

void Pajaro::avanzar()
{
    move(x() - velocidad, y());
    if (x() + width() < 0) {
        emit salioDePantalla(this);
    }
}

void Pajaro::animarAlas()
{
    frameActual++;
    update();
}

void Pajaro::crearSprites()
{
    for (int i = 0; i < 4; ++i) {
        sprites.append(crearSprite(i));
    }
}

QPixmap Pajaro::crearSprite(int frame) const
{
    QPixmap pixmap(size());
    pixmap.fill(Qt::transparent);

    QPainter p(&pixmap);
    p.setPen(Qt::NoPen);

    const int ala = frame % 2 == 0 ? 6 : -7;
    const QColor cuerpo = tipo == 0 ? QColor(70, 88, 138) : (tipo == 1 ? QColor(132, 85, 145) : QColor(53, 132, 139));
    const QColor alaColor = tipo == 0 ? QColor(93, 130, 184) : (tipo == 1 ? QColor(172, 111, 177) : QColor(83, 174, 164));
    const int extra = tipo == 2 ? 8 : 0;

    p.setBrush(alaColor);
    p.drawPolygon(QPolygon() << QPoint(42 + extra, 21) << QPoint(62 + extra, 13 + ala) << QPoint(47 + extra, 28));
    p.drawPolygon(QPolygon() << QPoint(37 + extra, 22) << QPoint(17, 14 - ala) << QPoint(29 + extra, 30));

    p.setBrush(cuerpo);
    p.drawRect(QRect(18, 18, 38 + extra, 16));
    p.drawRect(QRect(9, 13, 16, 16));

    p.setBrush(QColor(246, 184, 75));
    p.drawPolygon(QPolygon() << QPoint(9, 20) << QPoint(0, 24) << QPoint(9, 28));

    p.setBrush(Qt::white);
    p.drawRect(QRect(13, 16, 5, 5));
    p.setBrush(Qt::black);
    p.drawRect(QRect(13, 18, 2, 2));

    p.setBrush(cuerpo.darker(130));
    p.drawPolygon(QPolygon() << QPoint(56 + extra, 25) << QPoint(71 + extra, 18) << QPoint(64 + extra, 31));

    return pixmap;
}
