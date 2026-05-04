/********************************************************************************
** Form generated from reading UI file 'login.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGIN_H
#define UI_LOGIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QVBoxLayout *outerLayout;
    QFrame *loginCard;
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QFormLayout *formLayout;
    QLabel *usernameLabel;
    QLineEdit *usernameLineEdit;
    QLabel *passwordLabel;
    QLineEdit *passwordLineEdit;
    QPushButton *loginButton;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName("LoginDialog");
        LoginDialog->resize(420, 300);
        outerLayout = new QVBoxLayout(LoginDialog);
        outerLayout->setObjectName("outerLayout");
        outerLayout->setContentsMargins(24, 24, 24, 24);
        loginCard = new QFrame(LoginDialog);
        loginCard->setObjectName("loginCard");
        loginCard->setFrameShape(QFrame::StyledPanel);
        verticalLayout = new QVBoxLayout(loginCard);
        verticalLayout->setSpacing(12);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(24, 22, 24, 22);
        titleLabel = new QLabel(loginCard);
        titleLabel->setObjectName("titleLabel");
        titleLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(titleLabel);

        subtitleLabel = new QLabel(loginCard);
        subtitleLabel->setObjectName("subtitleLabel");
        subtitleLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(subtitleLabel);

        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        formLayout->setVerticalSpacing(10);
        usernameLabel = new QLabel(loginCard);
        usernameLabel->setObjectName("usernameLabel");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, usernameLabel);

        usernameLineEdit = new QLineEdit(loginCard);
        usernameLineEdit->setObjectName("usernameLineEdit");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, usernameLineEdit);

        passwordLabel = new QLabel(loginCard);
        passwordLabel->setObjectName("passwordLabel");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, passwordLabel);

        passwordLineEdit = new QLineEdit(loginCard);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setEchoMode(QLineEdit::Password);

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, passwordLineEdit);


        verticalLayout->addLayout(formLayout);

        loginButton = new QPushButton(loginCard);
        loginButton->setObjectName("loginButton");
        loginButton->setMinimumHeight(42);

        verticalLayout->addWidget(loginButton);


        outerLayout->addWidget(loginCard);


        retranslateUi(LoginDialog);

        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QCoreApplication::translate("LoginDialog", "Login", nullptr));
        titleLabel->setText(QCoreApplication::translate("LoginDialog", "Acceso al lienzo", nullptr));
        subtitleLabel->setText(QCoreApplication::translate("LoginDialog", "Ingres\303\241 con un usuario guardado en SQLite", nullptr));
        usernameLabel->setText(QCoreApplication::translate("LoginDialog", "Usuario", nullptr));
        passwordLabel->setText(QCoreApplication::translate("LoginDialog", "Clave", nullptr));
        loginButton->setText(QCoreApplication::translate("LoginDialog", "Ingresar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGIN_H
