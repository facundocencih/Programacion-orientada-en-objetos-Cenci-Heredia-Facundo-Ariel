#ifndef TREX_H
#define TREX_H

#include <QPixmap>
#include <QTimer>
#include <QWidget>

class TRex : public QWidget
{
    Q_OBJECT

public:
    enum Estado {
        Corriendo,
        Saltando,
        Agachado,
        Adelantando,
        Frenando,
        GameOver
    };

    explicit TRex(QWidget *parent = nullptr);

    void reiniciar(int sueloY);
    void actualizarFisica();
    void saltar();
    void agacharse(bool activo);
    void adelantar(bool activo);
    void frenar(bool activo);
    void morir();

    QRect areaColision() const;
    int suelo() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void animar();

private:
    void crearSprites();
    QPixmap crearSprite(Estado estado, int frame) const;
    void actualizarEstadoVisual();

    QVector<QPixmap> spritesCorriendo;
    QVector<QPixmap> spritesSaltando;
    QVector<QPixmap> spritesAgachado;
    QVector<QPixmap> spritesAdelantando;
    QVector<QPixmap> spritesFrenando;
    QVector<QPixmap> spritesGameOver;

    QTimer timerAnimacion;
    Estado estado;
    int frameActual;
    int sueloY;
    double velocidadY;
    bool enSalto;
    bool estaAgachado;
    bool estaAdelantando;
    bool estaFrenando;
};

#endif // TREX_H
