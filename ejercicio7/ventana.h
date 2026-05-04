#ifndef VENTANA_H
#define VENTANA_H

#include "pantalla.h"

#include <QWidget>

class QLabel;
class QFrame;
class QPixmap;
class QResizeEvent;

class Ventana : public QWidget, public Pantalla
{
    Q_OBJECT

public:
    explicit Ventana(QWidget *parent = nullptr);

    QString nombrePantalla() const override;
    void inicializar() override;
    void aplicarTema(const QPixmap &imagen) override;

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateAvatar();
    QPixmap loadProfilePhoto() const;
    void refreshPixmap();

    QLabel *m_backgroundLabel;
    QFrame *m_overlayCard;
    QLabel *m_titleLabel;
    QLabel *m_subtitleLabel;
    QLabel *m_avatarLabel;
    QLabel *m_descriptionLabel;
    QLabel *m_contactLabel;
    QLabel *m_detailsLabel;
    QPixmap m_pixmap;
    QPixmap m_avatarPixmap;
};

#endif // VENTANA_H
