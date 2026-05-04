#ifndef KANBANMODELS_H
#define KANBANMODELS_H

#include <QString>
#include <QVector>

struct CardData
{
    int id = -1;
    int columnId = -1;
    int position = -1;
    QString title;
    QString description;
};

struct ColumnData
{
    int id = -1;
    int position = -1;
    QString title;
    QVector<CardData> cards;
};

struct BoardData
{
    QVector<ColumnData> columns;
};

#endif
