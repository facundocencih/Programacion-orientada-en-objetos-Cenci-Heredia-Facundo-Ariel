#ifndef COLUMNWIDGET_H
#define COLUMNWIDGET_H

#include "kanbanmodels.h"

#include <QFrame>

class ColumnWidget : public QFrame
{
    Q_OBJECT

public:
    explicit ColumnWidget(const ColumnData &column, QWidget *parent = nullptr);

signals:
    void addCardRequested(int columnId);
    void editColumnRequested(int columnId);
    void deleteColumnRequested(int columnId);
    void editCardRequested(int cardId);
    void deleteCardRequested(int cardId);
    void moveCardLeftRequested(int cardId);
    void moveCardRightRequested(int cardId);
    void moveCardUpRequested(int cardId);
    void moveCardDownRequested(int cardId);

private:
    ColumnData m_column;
};

#endif
