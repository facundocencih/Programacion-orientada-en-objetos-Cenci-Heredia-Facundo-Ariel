#ifndef DRAWINGTYPES_H
#define DRAWINGTYPES_H

#include <QColor>
#include <QJsonArray>
#include <QJsonObject>
#include <QPointF>
#include <QString>
#include <QVector>

struct Stroke
{
    QString id;
    QColor color;
    qreal width = 4.0;
    bool eraser = false;
    qint64 createdAtMs = 0;
    QVector<QPointF> points;
};

inline QJsonObject strokeToJson(const Stroke &stroke)
{
    QJsonArray pointsArray;
    for (const QPointF &point : stroke.points) {
        QJsonObject pointObject;
        pointObject["x"] = point.x();
        pointObject["y"] = point.y();
        pointsArray.append(pointObject);
    }

    QJsonObject json;
    json["id"] = stroke.id;
    json["width"] = stroke.width;
    json["eraser"] = stroke.eraser;
    json["createdAtMs"] = QString::number(stroke.createdAtMs);
    json["color"] = stroke.color.name(QColor::HexArgb);
    json["points"] = pointsArray;
    return json;
}

inline Stroke strokeFromJson(const QJsonObject &json)
{
    Stroke stroke;
    stroke.id = json["id"].toString();
    stroke.width = json["width"].toDouble(4.0);
    stroke.eraser = json["eraser"].toBool(false);
    stroke.createdAtMs = json["createdAtMs"].toVariant().toLongLong();
    stroke.color = QColor(json["color"].toString("#FF000000"));

    const QJsonArray pointsArray = json["points"].toArray();
    stroke.points.reserve(pointsArray.size());
    for (const QJsonValue &value : pointsArray) {
        const QJsonObject pointObject = value.toObject();
        stroke.points.append(QPointF(pointObject["x"].toDouble(), pointObject["y"].toDouble()));
    }

    return stroke;
}

#endif
