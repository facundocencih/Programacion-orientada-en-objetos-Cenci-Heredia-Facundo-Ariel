#include "logindialog.h"
#include "ui_login.h"

#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

LoginDialog::LoginDialog(DatabaseManager *database, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::LoginDialog),
      database(database)
{
    ui->setupUi(this);
    setFixedSize(420, 300);
    setStyleSheet(QStringLiteral(
        "QDialog { background: #EEF5FC; font-family: Segoe UI, Arial; color: #23303D; }"
        "QFrame#loginCard { background: #FDFEFE; border: 1px solid #B6CEE6; border-radius: 8px; }"
        "QLabel#titleLabel { font-size: 22px; font-weight: 700; color: #12324A; }"
        "QLabel#subtitleLabel { font-size: 12px; color: #4F667A; }"
        "QLabel { font-size: 13px; color: #314659; }"
        "QLineEdit { background: white; color: #111827; selection-background-color: #BBD7F0; selection-color: #111827; border: 1px solid #B6CEE6; border-radius: 5px; padding: 9px 10px; font-size: 14px; }"
        "QLineEdit::placeholder { color: #6B7280; }"
        "QLineEdit:focus { border: 2px solid #0078D4; }"
        "QPushButton { background: #0078D4; color: white; border: none; border-radius: 5px; padding: 10px 18px; font-size: 14px; font-weight: 700; }"
        "QPushButton:hover { background: #1384DB; }"
        "QPushButton:pressed { background: #005EA8; }"));
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    ui->usernameLineEdit->setPlaceholderText("Usuario");
    ui->passwordLineEdit->setPlaceholderText("Clave");
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginDialog::attemptLogin);
    connect(ui->passwordLineEdit, &QLineEdit::returnPressed, this, &LoginDialog::attemptLogin);
    connect(ui->usernameLineEdit, &QLineEdit::returnPressed, this, &LoginDialog::attemptLogin);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

int LoginDialog::authenticatedUserId() const
{
    return userId;
}

void LoginDialog::attemptLogin()
{
    const QString username = ui->usernameLineEdit->text().trimmed();
    const QString password = ui->passwordLineEdit->text();
    int foundUserId = -1;

    const bool success = database && database->validateUser(username, password, &foundUserId);
    if (database) {
        database->registerAccess(username, success);
    }

    if (!success) {
        QMessageBox::warning(this, "Login", "Usuario o clave incorrectos.");
        ui->passwordLineEdit->clear();
        ui->passwordLineEdit->setFocus();
        return;
    }

    userId = foundUserId;
    accept();
}
