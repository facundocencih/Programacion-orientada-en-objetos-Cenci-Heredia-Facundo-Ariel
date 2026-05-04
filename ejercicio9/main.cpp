#include "paint.h"
#include "databasemanager.h"
#include "logindialog.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DatabaseManager database;
    if (!database.open()) {
        QMessageBox::critical(nullptr, "SQLite", "No se pudo abrir o inicializar la base de datos.");
        return 1;
    }

    LoginDialog login(&database);
    if (login.exec() != QDialog::Accepted) {
        return 0;
    }

    Paint w(&database, login.authenticatedUserId());
    w.show();
    return QCoreApplication::exec();
}
