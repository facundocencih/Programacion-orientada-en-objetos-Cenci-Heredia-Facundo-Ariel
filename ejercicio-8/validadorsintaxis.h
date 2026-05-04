#ifndef VALIDADORSINTAXIS_H
#define VALIDADORSINTAXIS_H

#include <QString>

struct ResultadoValidacion
{
    bool valido = true;
    QString diagnostico;
};

class ValidadorSintaxis
{
public:
    virtual ~ValidadorSintaxis() = default;
    virtual QString lenguaje() const = 0;
    virtual ResultadoValidacion validarLinea(const QString &linea, int numeroLinea) const = 0;
};

class ValidadorCpp : public ValidadorSintaxis
{
public:
    QString lenguaje() const override;
    ResultadoValidacion validarLinea(const QString &linea, int numeroLinea) const override;
};

class ValidadorPython : public ValidadorSintaxis
{
public:
    QString lenguaje() const override;
    ResultadoValidacion validarLinea(const QString &linea, int numeroLinea) const override;
};

class ValidadorJava : public ValidadorSintaxis
{
public:
    QString lenguaje() const override;
    ResultadoValidacion validarLinea(const QString &linea, int numeroLinea) const override;
};

#endif // VALIDADORSINTAXIS_H
