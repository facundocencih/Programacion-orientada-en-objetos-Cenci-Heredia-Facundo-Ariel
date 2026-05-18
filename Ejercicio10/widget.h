#ifndef WIDGET_H
#define WIDGET_H

#include "pajaro.h"
#include "trex.h"

#include <QElapsedTimer>
#include <QImage>
#include <QTimer>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void actualizarJuego();
    void crearPajaro();
    void eliminarPajaro(Pajaro *pajaro);
    void crearObstaculoSorpresa();

private:
    struct Cactus {
        QRect rect;
        QImage sprite;
    };

    void iniciarJuego();
    void terminarJuego();
    void crearSprites();
    QImage crearSpriteCactus(int tipo) const;
    QImage crearImagenGameOver() const;
    void crearCactus();
    void actualizarCactus();
    void actualizarPajaros();
    void actualizarNube();
    void detectarColisiones();
    void limpiarPajaros();
    void dibujarFondo(QPainter &painter);
    int ultimaPosicionCactus() const;
    int ultimaPosicionPajaro() const;
    int distanciaCactus() const;
    int distanciaAntesDePajaro() const;
    int distanciaEntrePajaros() const;
    int intervaloPajaros() const;
    int maximoPajarosActivos() const;
    int nivelDificultad() const;
    int velocidadActual() const;

    Ui::Widget *ui;
    TRex *trex;
    QTimer timerPrincipal;
    QTimer timerPajaros;
    QElapsedTimer reloj;
    QVector<Cactus> cactus;
    QVector<Pajaro *> pajaros;
    QVector<QImage> spritesCactus;
    QImage imagenGameOver;
    int sueloY;
    int velocidadBase;
    int distancia;
    int puntaje;
    int proximoCactusX;
    int nubeX;
    int nubeY;
    bool avanzando;
    bool frenando;
    bool juegoTerminado;
};
#endif // WIDGET_H
