#ifndef DRAWINGMODEL_H
#define DRAWINGMODEL_H

#include "drawingtypes.h"

#include <QJsonDocument>
#include <QObject>
#include <QSet>

class DrawingModel : public QObject
{
    Q_OBJECT

public:
    explicit DrawingModel(QObject *parent = nullptr);

    const QList<Stroke> &strokes() const;
    QList<Stroke> pendingStrokes() const;
    bool addStroke(const Stroke &stroke, bool pending);
    bool mergeRemoteStrokes(const QList<Stroke> &remoteStrokes);
    void markStrokesSynced(const QStringList &strokeIds);
    QJsonObject toJson() const;
    bool mergeFromJson(const QJsonDocument &document);

signals:
    void changed();
    void pendingSyncChanged(int pendingCount);

private:
    void emitPendingCount();

    QList<Stroke> m_strokes;
    QSet<QString> m_strokeIds;
    QSet<QString> m_pendingStrokeIds;
};

#endif
