#include "appcontroller.h"

#include <QApplication>
#include <QCoreApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (!QFile::exists(QCoreApplication::applicationDirPath() + "/config.ini")) {
        QFile::copy("config.ini", QCoreApplication::applicationDirPath() + "/config.ini");
    }

    AppController controller;
    controller.iniciar();

    return app.exec();
}

