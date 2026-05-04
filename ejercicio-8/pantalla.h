#ifndef PANTALLA_H
#define PANTALLA_H

#include <QString>

class Pantalla
{
public:
    virtual ~Pantalla() = default;

    virtual QString nombrePantalla() const = 0;
    virtual void inicializarUI() = 0;
    virtual void conectarEventos() = 0;
    virtual void cargarDatos() = 0;
    virtual bool validarEstado() const = 0;
    virtual void registrarEvento(const QString &descripcion) = 0;
};

#endif // PANTALLA_H
