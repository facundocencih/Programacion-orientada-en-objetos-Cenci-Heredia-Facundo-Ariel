#include "columndialog.h"

#include <QDialogButtonBox>
#include <QLineEdit>
#include <QVBoxLayout>

ColumnDialog::ColumnDialog(QWidget *parent)
    : QDialog(parent)
    , m_titleEdit(new QLineEdit(this))
{
    setWindowTitle(QStringLiteral("Columna"));
    resize(360, 120);

    auto *layout = new QVBoxLayout(this);
    m_titleEdit->setPlaceholderText(QStringLiteral("Ej: En progreso"));
    layout->addWidget(m_titleEdit);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

void ColumnDialog::setTitleText(const QString &title)
{
    m_titleEdit->setText(title);
}

QString ColumnDialog::titleText() const
{
    return m_titleEdit->text().trimmed();
}
