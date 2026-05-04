#include "appcontroller.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    AppController controller;
    controller.start();

    return app.exec();
}

