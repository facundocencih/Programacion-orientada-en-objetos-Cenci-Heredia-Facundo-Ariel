#include "columnwidget.h"

#include "cardwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

ColumnWidget::ColumnWidget(const ColumnData &column, QWidget *parent)
    : QFrame(parent)
    , m_column(column)
{
    setObjectName(QStringLiteral("ColumnWidget"));
    setMinimumWidth(320);
    setMaximumWidth(360);
    setFrameShape(QFrame::NoFrame);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    auto *headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(8);

    auto *titleLabel = new QLabel(column.title, this);
    titleLabel->setObjectName(QStringLiteral("ColumnTitle"));

    auto *countLabel = new QLabel(QString::number(column.cards.size()), this);
    countLabel->setObjectName(QStringLiteral("CountBadge"));

    auto *editButton = new QPushButton(QStringLiteral("Editar"), this);
    auto *deleteButton = new QPushButton(QStringLiteral("Eliminar"), this);
    editButton->setObjectName(QStringLiteral("GhostButton"));
    deleteButton->setObjectName(QStringLiteral("DangerGhostButton"));
    editButton->setCursor(Qt::PointingHandCursor);
    deleteButton->setCursor(Qt::PointingHandCursor);

    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(countLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(editButton);
    headerLayout->addWidget(deleteButton);
    layout->addLayout(headerLayout);

    for (const CardData &card : column.cards) {
        auto *cardWidget = new CardWidget(card, this);
        connect(cardWidget, &CardWidget::editRequested, this, &ColumnWidget::editCardRequested);
        connect(cardWidget, &CardWidget::deleteRequested, this, &ColumnWidget::deleteCardRequested);
        connect(cardWidget, &CardWidget::moveLeftRequested, this, &ColumnWidget::moveCardLeftRequested);
        connect(cardWidget, &CardWidget::moveRightRequested, this, &ColumnWidget::moveCardRightRequested);
        connect(cardWidget, &CardWidget::moveUpRequested, this, &ColumnWidget::moveCardUpRequested);
        connect(cardWidget, &CardWidget::moveDownRequested, this, &ColumnWidget::moveCardDownRequested);
        layout->addWidget(cardWidget);
    }

    auto *addCardButton = new QPushButton(QStringLiteral("+ Agregar tarjeta"), this);
    addCardButton->setCursor(Qt::PointingHandCursor);
    addCardButton->setObjectName(QStringLiteral("AddCardButton"));
    layout->addWidget(addCardButton);
    layout->addStretch();

    connect(addCardButton, &QPushButton::clicked, this, [this]() { emit addCardRequested(m_column.id); });
    connect(editButton, &QPushButton::clicked, this, [this]() { emit editColumnRequested(m_column.id); });
    connect(deleteButton, &QPushButton::clicked, this, [this]() { emit deleteColumnRequested(m_column.id); });
}
