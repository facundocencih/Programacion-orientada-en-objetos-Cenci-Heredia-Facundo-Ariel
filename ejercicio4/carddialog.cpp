#include "carddialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>

CardDialog::CardDialog(QWidget *parent)
    : QDialog(parent)
    , m_titleEdit(new QLineEdit(this))
    , m_descriptionEdit(new QTextEdit(this))
{
    setWindowTitle(QStringLiteral("Tarjeta"));
    resize(420, 280);

    auto *mainLayout = new QVBoxLayout(this);
    auto *formLayout = new QFormLayout();
    m_descriptionEdit->setMinimumHeight(120);

    formLayout->addRow(QStringLiteral("Titulo"), m_titleEdit);
    formLayout->addRow(QStringLiteral("Descripcion"), m_descriptionEdit);
    mainLayout->addLayout(formLayout);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);
}

void CardDialog::setValues(const QString &title, const QString &description)
{
    m_titleEdit->setText(title);
    m_descriptionEdit->setPlainText(description);
}

QString CardDialog::title() const
{
    return m_titleEdit->text().trimmed();
}

QString CardDialog::description() const
{
    return m_descriptionEdit->toPlainText().trimmed();
}
