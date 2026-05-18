#ifndef PAJARO_H
#define PAJARO_H

#include <QPixmap>
#include <QTimer>
#include <QWidget>

class Pajaro : public QWidget
{
    Q_OBJECT

public:
    explicit Pajaro(int velocidadInicial, int tipoInicial, QWidget *parent = nullptr);

    void setVelocidad(int nuevaVelocidad);
    void detener();
    QRect areaColision() const;

signals:
    void salioDePantalla(Pajaro *pajaro);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void avanzar();
    void animarAlas();

private:
    void crearSprites();
    QPixmap crearSprite(int frame) const;

    QTimer timerMovimiento;
    QTimer timerAnimacion;
    QVector<QPixmap> sprites;
    int velocidad;
    int tipo;
    int frameActual;
};

#endif // PAJARO_H
