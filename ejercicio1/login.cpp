#include "login.h"
Login::Login(){
    lUsuario = new QLabel("Usuario:");
    lClave = new QLabel("Clave:");
    leUsuario = new QLineEdit();
    leClave = new QLineEdit();
    leClave->setEchoMode(QLineEdit::Password);
    pb = new QPushButton("Ingresar");
    layout->addWidget(lUsuario,1,1,1,1);
    layout->addWidget(leUsuario,1,2,1,1);
    layout->addWidget(lClave,2,1,1,1);
    layout->addWidget(leClave,2,2,1,1);
    layout->addWidget(pb,3,1,1,2);
    setLayout(layout);
    connect(pb,SIGNAL(clicked()),this,SLOT(slot_ingresar()));
}