#include "drawingmodel.h"

#include <QJsonArray>
#include <QJsonDocument>

DrawingModel::DrawingModel(QObject *parent)
    : QObject(parent)
{
}

const QList<Stroke> &DrawingModel::strokes() const
{
    return m_strokes;
}

QList<Stroke> DrawingModel::pendingStrokes() const
{
    QList<Stroke> pending;
    for (const Stroke &stroke : m_strokes) {
        if (m_pendingStrokeIds.contains(stroke.id)) {
            pending.append(stroke);
        }
    }
    return pending;
}

bool DrawingModel::addStroke(const Stroke &stroke, bool pending)
{
    if (stroke.id.isEmpty() || stroke.points.isEmpty() || m_strokeIds.contains(stroke.id)) {
        return false;
    }

    m_strokes.append(stroke);
    m_strokeIds.insert(stroke.id);
    if (pending) {
        m_pendingStrokeIds.insert(stroke.id);
    }

    emit changed();
    emitPendingCount();
    return true;
}

bool DrawingModel::mergeRemoteStrokes(const QList<Stroke> &remoteStrokes)
{
    bool merged = false;
    for (const Stroke &stroke : remoteStrokes) {
        if (stroke.id.isEmpty() || stroke.points.isEmpty() || m_strokeIds.contains(stroke.id)) {
            continue;
        }

        m_strokes.append(stroke);
        m_strokeIds.insert(stroke.id);
        merged = true;
    }

    if (merged) {
        emit changed();
    }

    return merged;
}

void DrawingModel::markStrokesSynced(const QStringList &strokeIds)
{
    bool changedPending = false;
    for (const QString &id : strokeIds) {
        changedPending = m_pendingStrokeIds.remove(id) || changedPending;
    }

    if (changedPending) {
        emitPendingCount();
    }
}

QJsonObject DrawingModel::toJson() const
{
    QJsonArray strokesArray;
    for (const Stroke &stroke : m_strokes) {
        strokesArray.append(strokeToJson(stroke));
    }

    QJsonObject json;
    json["strokes"] = strokesArray;
    return json;
}

bool DrawingModel::mergeFromJson(const QJsonDocument &document)
{
    if (!document.isObject()) {
        return false;
    }

    QList<Stroke> remoteStrokes;
    const QJsonArray strokesArray = document.object().value("strokes").toArray();
    remoteStrokes.reserve(strokesArray.size());

    for (const QJsonValue &value : strokesArray) {
        remoteStrokes.append(strokeFromJson(value.toObject()));
    }

    return mergeRemoteStrokes(remoteStrokes);
}

void DrawingModel::emitPendingCount()
{
    emit pendingSyncChanged(m_pendingStrokeIds.size());
}
