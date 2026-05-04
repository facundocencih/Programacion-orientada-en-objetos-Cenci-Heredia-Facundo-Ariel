#ifndef MODOBLOQUEADO_H
#define MODOBLOQUEADO_H

#include "pantalla.h"

#include <QWidget>

class QLabel;
class QProgressBar;
class QTimer;
class Logger;

class ModoBloqueado : public QWidget, public Pantalla
{
    Q_OBJECT

public:
    explicit ModoBloqueado(Logger *logger, QWidget *parent = nullptr);
    void iniciar(int segundos);

    QString nombrePantalla() const override;
    void inicializarUI() override;
    void conectarEventos() override;
    void cargarDatos() override;
    bool validarEstado() const override;
    void registrarEvento(const QString &descripcion) override;

signals:
    void bloqueoFinalizado();
    void eventoRegistrado(const QString &descripcion);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private slots:
    void actualizarCuenta();

private:
    Logger *m_logger;
    QLabel *m_mensaje;
    QProgressBar *m_barra;
    QTimer *m_timer;
    int m_restante = 0;
    int m_total = 1;
};

#endif // MODOBLOQUEADO_H
