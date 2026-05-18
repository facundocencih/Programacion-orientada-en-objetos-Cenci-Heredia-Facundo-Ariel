#include "trex.h"

#include <QPainter>

TRex::TRex(QWidget *parent)
    : QWidget(parent),
      estado(Corriendo),
      frameActual(0),
      sueloY(420),
      velocidadY(0.0),
      enSalto(false),
      estaAgachado(false),
      estaAdelantando(false),
      estaFrenando(false)
{
    setFixedSize(92, 88);
    setFocusPolicy(Qt::NoFocus);
    crearSprites();
    timerAnimacion.setTimerType(Qt::CoarseTimer);
    connect(&timerAnimacion, SIGNAL(timeout()), this, SLOT(animar()));
    timerAnimacion.start(90);
}

void TRex::reiniciar(int suelo)
{
    sueloY = suelo;
    velocidadY = 0.0;
    enSalto = false;
    estaAgachado = false;
    estaAdelantando = false;
    estaFrenando = false;
    estado = Corriendo;
    frameActual = 0;
    setFixedSize(92, 88);
    move(90, sueloY - height());
    show();
    timerAnimacion.start(75);
    update();
}

void TRex::actualizarFisica()
{
    if (!enSalto) {
        return;
    }

    const double gravedad = 1.05;
    move(x(), y() + static_cast<int>(velocidadY));
    velocidadY += gravedad;

    if (y() + height() >= sueloY) {
        move(x(), sueloY - height());
        velocidadY = 0.0;
        enSalto = false;
        actualizarEstadoVisual();
    }
}

void TRex::saltar()
{
    if (estado == GameOver || enSalto) {
        return;
    }

    estaAgachado = false;
    enSalto = true;
    velocidadY = -18.0;
    estado = Saltando;
    setFixedSize(92, 88);
    frameActual = 0;
    update();
}

void TRex::agacharse(bool activo)
{
    if (estado == GameOver || enSalto) {
        return;
    }

    estaAgachado = activo;
    actualizarEstadoVisual();
}

void TRex::adelantar(bool activo)
{
    if (estado == GameOver) {
        return;
    }

    estaAdelantando = activo;
    actualizarEstadoVisual();
}

void TRex::frenar(bool activo)
{
    if (estado == GameOver) {
        return;
    }

    estaFrenando = activo;
    actualizarEstadoVisual();
}

void TRex::morir()
{
    estado = GameOver;
    frameActual = 0;
    timerAnimacion.stop();
    update();
}

QRect TRex::areaColision() const
{
    if (estado == Agachado) {
        return QRect(x() + 12, y() + 35, width() - 22, height() - 42);
    }

    return QRect(x() + 16, y() + 10, width() - 30, height() - 18);
}

int TRex::suelo() const
{
    return sueloY;
}

void TRex::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    const QVector<QPixmap> *sprites = &spritesCorriendo;
    if (estado == Saltando) {
        sprites = &spritesSaltando;
    } else if (estado == Agachado) {
        sprites = &spritesAgachado;
    } else if (estado == Adelantando) {
        sprites = &spritesAdelantando;
    } else if (estado == Frenando) {
        sprites = &spritesFrenando;
    } else if (estado == GameOver) {
        sprites = &spritesGameOver;
    }

    const QPixmap &sprite = sprites->at(frameActual % sprites->size());
    painter.drawPixmap(rect(), sprite);
}

void TRex::animar()
{
    frameActual++;
    update();
}

void TRex::crearSprites()
{
    for (int i = 0; i < 4; ++i) {
        spritesCorriendo.append(crearSprite(Corriendo, i));
        spritesSaltando.append(crearSprite(Saltando, i));
        spritesAgachado.append(crearSprite(Agachado, i));
        spritesAdelantando.append(crearSprite(Adelantando, i));
        spritesFrenando.append(crearSprite(Frenando, i));
    }
    spritesGameOver.append(crearSprite(GameOver, 0));
}

QPixmap TRex::crearSprite(Estado estadoSprite, int frame) const
{
    QPixmap pixmap(92, 88);
    pixmap.fill(Qt::transparent);

    QPainter p(&pixmap);

    QColor cuerpo(61, 72, 82);
    QColor sombra(96, 107, 115);
    QColor acento(48, 170, 128);
    if (estadoSprite == Adelantando) {
        cuerpo = QColor(35, 93, 152);
        acento = QColor(57, 191, 170);
    } else if (estadoSprite == Frenando) {
        cuerpo = QColor(90, 84, 78);
        acento = QColor(231, 116, 72);
    } else if (estadoSprite == GameOver) {
        cuerpo = QColor(92, 92, 92);
        acento = QColor(190, 64, 64);
    }

    const bool agachado = estadoSprite == Agachado;
    const int cuerpoY = agachado ? 44 : 31;
    const int cabezaX = agachado ? 48 : 50;
    const int cabezaY = agachado ? 27 : 12;
    const int piernaOffset = frame % 2 == 0 ? 0 : 8;

    p.setPen(Qt::NoPen);
    p.setBrush(cuerpo);
    p.drawRect(QRect(18, cuerpoY, agachado ? 56 : 42, agachado ? 24 : 36));
    p.drawRect(QRect(cabezaX, cabezaY, 28, 28));
    p.drawRect(QRect(cabezaX + 26, cabezaY + 12, 12, 8));
    p.drawRect(QRect(8, cuerpoY + 10, 16, 8));

    p.setBrush(sombra);
    p.drawRect(QRect(30, cuerpoY + 10, 18, agachado ? 8 : 16));

    p.setBrush(Qt::white);
    p.drawRect(QRect(cabezaX + 18, cabezaY + 7, 6, 6));
    p.setBrush(Qt::black);
    if (estadoSprite == GameOver) {
        p.drawRect(QRect(cabezaX + 18, cabezaY + 7, 3, 3));
        p.drawRect(QRect(cabezaX + 23, cabezaY + 12, 3, 3));
        p.setPen(Qt::NoPen);
    } else {
        p.drawRect(QRect(cabezaX + 22, cabezaY + 9, 3, 3));
    }

    p.setBrush(acento);
    p.drawRect(QRect(39, cuerpoY - 7 + (frame % 2), 8, 8));
    p.drawRect(QRect(29, cuerpoY - 5 - (frame % 2), 7, 7));

    p.setBrush(cuerpo.darker(115));
    if (estadoSprite == Saltando) {
        p.drawRect(QRect(27, 70, 8, 14));
        p.drawRect(QRect(54, 67, 8, 17));
    } else if (agachado) {
        p.drawRect(QRect(27 + piernaOffset, 64, 18, 10));
        p.drawRect(QRect(55 - piernaOffset / 2, 64, 18, 10));
    } else {
        p.drawRect(QRect(26, 66 + piernaOffset / 2, 9, 18));
        p.drawRect(QRect(54, 70 - piernaOffset / 2, 9, 14));
    }

    p.setBrush(cuerpo.darker(125));
    const int brazoY = estadoSprite == Adelantando ? cuerpoY + 11 - frame % 2 : cuerpoY + 14 + frame % 3;
    p.drawRect(QRect(54, brazoY, 18, 6));

    return pixmap;
}

void TRex::actualizarEstadoVisual()
{
    if (estado == GameOver || enSalto) {
        return;
    }

    const int pie = y() + height();
    if (estaAgachado) {
        estado = Agachado;
        setFixedSize(92, 70);
    } else {
        setFixedSize(92, 88);
        if (estaAdelantando) {
            estado = Adelantando;
        } else if (estaFrenando) {
            estado = Frenando;
        } else {
            estado = Corriendo;
        }
    }

    move(x(), pie - height());
    frameActual = 0;
    update();
}
