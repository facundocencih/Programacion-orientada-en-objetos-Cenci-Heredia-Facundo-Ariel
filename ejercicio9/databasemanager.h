#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QColor>
#include <QPoint>
#include <QSqlDatabase>
#include <QString>
#include <QVector>

struct StrokeData
{
    int id = -1;
    QColor color = Qt::black;
    int width = 4;
    QVector<QPoint> points;
};

class DatabaseManager
{
public:
    bool open();
    bool validateUser(const QString &username, const QString &password, int *userId);
    void registerAccess(const QString &username, bool success);

    QVector<StrokeData> loadStrokes(int userId) const;
    int saveStroke(int userId, const QColor &color, int width, const QVector<QPoint> &points);
    bool deleteStroke(int strokeId);
    bool clearStrokes(int userId);

private:
    bool createSchema();
    bool ensureDefaultUser();
    static QString passwordHash(const QString &password);

    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
