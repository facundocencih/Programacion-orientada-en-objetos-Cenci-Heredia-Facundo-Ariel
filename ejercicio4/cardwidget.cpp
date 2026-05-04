#include "cardwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

CardWidget::CardWidget(const CardData &card, QWidget *parent)
    : QFrame(parent)
    , m_card(card)
{
    setObjectName(QStringLiteral("CardWidget"));
    setFrameShape(QFrame::NoFrame);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(10);

    auto *titleLabel = new QLabel(card.title, this);
    titleLabel->setWordWrap(true);
    titleLabel->setObjectName(QStringLiteral("CardTitle"));
    layout->addWidget(titleLabel);

    if (!card.description.isEmpty()) {
        auto *descriptionLabel = new QLabel(card.description, this);
        descriptionLabel->setWordWrap(true);
        descriptionLabel->setObjectName(QStringLiteral("CardDescription"));
        layout->addWidget(descriptionLabel);
    }

    auto *actionsRow = new QHBoxLayout();
    actionsRow->setSpacing(6);

    auto *moveLeftButton = new QPushButton(QStringLiteral("<"), this);
    auto *moveRightButton = new QPushButton(QStringLiteral(">"), this);
    auto *moveUpButton = new QPushButton(QStringLiteral("Subir"), this);
    auto *moveDownButton = new QPushButton(QStringLiteral("Bajar"), this);
    auto *editButton = new QPushButton(QStringLiteral("Editar"), this);
    auto *deleteButton = new QPushButton(QStringLiteral("Eliminar"), this);

    const QList<QPushButton *> smallButtons = {moveLeftButton, moveRightButton, moveUpButton, moveDownButton};
    for (QPushButton *button : smallButtons) {
        button->setCursor(Qt::PointingHandCursor);
        button->setObjectName(QStringLiteral("GhostButton"));
    }

    editButton->setCursor(Qt::PointingHandCursor);
    deleteButton->setCursor(Qt::PointingHandCursor);
    editButton->setObjectName(QStringLiteral("GhostButton"));
    deleteButton->setObjectName(QStringLiteral("DangerGhostButton"));

    actionsRow->addWidget(moveLeftButton);
    actionsRow->addWidget(moveRightButton);
    actionsRow->addWidget(moveUpButton);
    actionsRow->addWidget(moveDownButton);
    actionsRow->addStretch();
    actionsRow->addWidget(editButton);
    actionsRow->addWidget(deleteButton);
    layout->addLayout(actionsRow);

    connect(editButton, &QPushButton::clicked, this, [this]() { emit editRequested(m_card.id); });
    connect(deleteButton, &QPushButton::clicked, this, [this]() { emit deleteRequested(m_card.id); });
    connect(moveLeftButton, &QPushButton::clicked, this, [this]() { emit moveLeftRequested(m_card.id); });
    connect(moveRightButton, &QPushButton::clicked, this, [this]() { emit moveRightRequested(m_card.id); });
    connect(moveUpButton, &QPushButton::clicked, this, [this]() { emit moveUpRequested(m_card.id); });
    connect(moveDownButton, &QPushButton::clicked, this, [this]() { emit moveDownRequested(m_card.id); });
}
