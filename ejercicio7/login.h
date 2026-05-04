#ifndef LOGIN_H
#define LOGIN_H

#include "appconfig.h"
#include "pantalla.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Login; }
QT_END_NAMESPACE

class QPixmap;
class QTimer;

class Login : public QMainWindow, public Pantalla
{
    Q_OBJECT

public:
    explicit Login(const AppConfigData &config, QWidget *parent = nullptr);
    ~Login() override;

    QString nombrePantalla() const override;
    void inicializar() override;
    void aplicarTema(const QPixmap &imagen) override;

    void setEstadoClima(const QString &temperatura, const QString &horaLocal, const QString &estado);
    void setMensajeEstado(const QString &mensaje, bool error = false);

signals:
    void loginSolicitado(const QString &usuario, const QString &clave);

private slots:
    void on_btnIngresar_clicked();
    void actualizarCuentaRegresiva();

public slots:
    void bloquearTemporalmente(int segundos);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void refreshBackground();

    Ui::Login *ui;
    AppConfigData m_config;
    QPixmap m_background;
    QTimer *m_lockTimer;
    int m_remainingSeconds;
};

#endif // LOGIN_H

