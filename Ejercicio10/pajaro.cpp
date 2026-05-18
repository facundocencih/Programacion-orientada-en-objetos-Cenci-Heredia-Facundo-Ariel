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
    timerMovimiento.start(33);
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
    p.drawPolygon(QPolygon() << QPoint(30, 21) << QPoint(10, 13 + ala) << QPoint(25, 28));
    p.drawPolygon(QPolygon() << QPoint(35, 22) << QPoint(55 + extra, 14 - ala) << QPoint(43, 30));

    p.setBrush(cuerpo);
    p.drawRect(QRect(18, 18, 38 + extra, 16));
    p.drawRect(QRect(47 + extra, 13, 16, 16));

    p.setBrush(QColor(246, 184, 75));
    p.drawPolygon(QPolygon() << QPoint(61 + extra, 20) << QPoint(70 + extra, 24) << QPoint(61 + extra, 28));

    p.setBrush(Qt::white);
    p.drawRect(QRect(54 + extra, 16, 5, 5));
    p.setBrush(Qt::black);
    p.drawRect(QRect(56 + extra, 18, 2, 2));

    p.setBrush(cuerpo.darker(130));
    p.drawPolygon(QPolygon() << QPoint(20, 25) << QPoint(5, 18) << QPoint(12, 31));

    return pixmap;
}
