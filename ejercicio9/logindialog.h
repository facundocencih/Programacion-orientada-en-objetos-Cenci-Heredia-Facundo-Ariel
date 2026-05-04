#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "databasemanager.h"

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(DatabaseManager *database, QWidget *parent = nullptr);
    ~LoginDialog() override;

    int authenticatedUserId() const;

private slots:
    void attemptLogin();

private:
    Ui::LoginDialog *ui;
    DatabaseManager *database = nullptr;
    int userId = -1;
};

#endif // LOGINDIALOG_H
