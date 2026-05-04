#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include "validadorsintaxis.h"

#include <QMap>
#include <QTextEdit>
#include <QTextBlock>
#include <memory>

class CodeEditor : public QTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);
    void setValidador(std::unique_ptr<ValidadorSintaxis> validador);
    void validarLineaActual();
    void validarLinea(int numeroLinea);

signals:
    void lineaValidada(int numeroLinea, bool valida, const QString &diagnostico);
    void accionEditor(const QString &descripcion);

protected:
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void detectarCambioDeLinea();

private:
    int lineaActual() const;
    QTextBlock bloquePorNumero(int numeroLinea) const;
    void actualizarResaltado();

    std::unique_ptr<ValidadorSintaxis> m_validador;
    int m_ultimaLinea = 0;
    QMap<int, QString> m_errores;
};

#endif // CODEEDITOR_H
