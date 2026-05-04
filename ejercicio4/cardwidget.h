#ifndef CARDWIDGET_H
#define CARDWIDGET_H

#include "kanbanmodels.h"

#include <QFrame>

class QLabel;
class QPushButton;

class CardWidget : public QFrame
{
    Q_OBJECT

public:
    explicit CardWidget(const CardData &card, QWidget *parent = nullptr);

signals:
    void editRequested(int cardId);
    void deleteRequested(int cardId);
    void moveLeftRequested(int cardId);
    void moveRightRequested(int cardId);
    void moveUpRequested(int cardId);
    void moveDownRequested(int cardId);

private:
    CardData m_card;
};

#endif
