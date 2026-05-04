#ifndef PANTALLA_H
#define PANTALLA_H

#include <QPixmap>
#include <QString>

class Pantalla
{
public:
    virtual ~Pantalla() = default;

    virtual QString nombrePantalla() const = 0;
    virtual void inicializar() = 0;
    virtual void aplicarTema(const QPixmap &imagen) = 0;
};

#endif // PANTALLA_H

