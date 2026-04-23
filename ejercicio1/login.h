#ifndef LOGIN_H
#define LOGIN_H
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
class Login : public QWidget
{
    Q_OBJECT
private:
    QLabel * lUsuario,* lClave;
    QLineEdit * leUsuario, * leClave;
    QPushButton * pb;
    QGridLayout * layout;
public:
    Login();
private slots;
    void slot_ingresar(){
        this->close();
    }
};
#endif // LOGIN_H
