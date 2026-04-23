#include "canvasview.h"

#include <QDateTime>
#include <QKeyEvent>
#include <QLineF>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QRandomGenerator>
#include <QWheelEvent>

#include <algorithm>
#include <cmath>

namespace {
constexpr qreal kMinBrushWidth = 2.0;
constexpr qreal kMaxBrushWidth = 48.0;

QPointF eventPoint(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}
}

CanvasView::CanvasView(DrawingModel *model, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
    , m_currentColor(colorForIndex(1))
{
    setAttribute(Qt::WA_StaticContents);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setAutoFillBackground(true);
    setPalette(QPalette(Qt::white));

    connect(m_model, &DrawingModel::changed, this, qOverload<>(&CanvasView::update));
}

QColor CanvasView::currentColor() const
{
    return m_currentColor;
}

qreal CanvasView::currentWidth() const
{
    return m_currentWidth;
}

QList<QColor> CanvasView::paletteColors() const
{
    QList<QColor> colors;
    for (int i = 1; i <= 9; ++i) {
        colors.append(colorForIndex(i));
    }
    return colors;
}

void CanvasView::setCurrentColorIndex(int index)
{
    if (index < 1 || index > 9) {
        return;
    }

    m_currentColorIndex = index;
    m_currentColor = colorForIndex(index);
    emit brushChanged(m_currentColor, m_currentWidth, false, m_currentColorIndex);
    update();
}

void CanvasView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), Qt::white);

    for (const Stroke &stroke : m_model->strokes()) {
        drawStroke(painter, stroke);
    }

    if (m_drawing) {
        drawStroke(painter, m_activeStroke);
    }
}

void CanvasView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
        beginStroke(eventPoint(event), event->button() == Qt::RightButton);
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void CanvasView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_drawing) {
        extendStroke(eventPoint(event));
        event->accept();
        return;
    }

    QWidget::mouseMoveEvent(event);
}

void CanvasView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_drawing && (event->button() == Qt::LeftButton || event->button() == Qt::RightButton)) {
        extendStroke(eventPoint(event));
        finishStroke();
        event->accept();
        return;
    }

    QWidget::mouseReleaseEvent(event);
}

void CanvasView::wheelEvent(QWheelEvent *event)
{
    const qreal deltaSteps = event->angleDelta().y() / 120.0;
    m_currentWidth = std::clamp(m_currentWidth + deltaSteps, kMinBrushWidth, kMaxBrushWidth);
    emit brushChanged(m_currentColor, m_currentWidth, false, m_currentColorIndex);
    update();
    event->accept();
}

void CanvasView::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();
    if (key >= Qt::Key_1 && key <= Qt::Key_9) {
        setCurrentColorIndex(key - Qt::Key_0);
        event->accept();
        return;
    }

    QWidget::keyPressEvent(event);
}

void CanvasView::beginStroke(const QPointF &point, bool eraser)
{
    m_drawing = true;
    m_activeStroke = Stroke();
    m_activeStroke.id = nextStrokeId();
    m_activeStroke.width = m_currentWidth;
    m_activeStroke.eraser = eraser;
    m_activeStroke.color = eraser ? QColor(Qt::white) : m_currentColor;
    m_activeStroke.createdAtMs = QDateTime::currentMSecsSinceEpoch();
    m_activeStroke.points.append(point);
    emit brushChanged(m_currentColor, m_currentWidth, eraser, m_currentColorIndex);
    update();
}

void CanvasView::extendStroke(const QPointF &point)
{
    if (!m_drawing || m_activeStroke.points.isEmpty()) {
        return;
    }

    const QVector<QPointF> segment = interpolateSegment(m_activeStroke.points.constLast(), point);
    for (const QPointF &interpolated : segment) {
        if (m_activeStroke.points.isEmpty() || QLineF(m_activeStroke.points.constLast(), interpolated).length() > 0.35) {
            m_activeStroke.points.append(interpolated);
        }
    }

    if (QLineF(m_activeStroke.points.constLast(), point).length() > 0.35) {
        m_activeStroke.points.append(point);
    }

    update();
}

void CanvasView::finishStroke()
{
    m_drawing = false;
    if (m_activeStroke.points.size() == 1) {
        m_activeStroke.points.append(m_activeStroke.points.constFirst() + QPointF(0.1, 0.1));
    }

    m_model->addStroke(m_activeStroke, true);
    m_activeStroke = Stroke();
    emit brushChanged(m_currentColor, m_currentWidth, false, m_currentColorIndex);
    update();
}

void CanvasView::drawStroke(QPainter &painter, const Stroke &stroke) const
{
    if (stroke.points.size() < 2) {
        return;
    }

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QColor effectiveColor = (stroke.eraser || stroke.color.alpha() == 0) ? QColor(Qt::white) : stroke.color;
    QPen pen(effectiveColor, stroke.width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    QPainterPath path(stroke.points.constFirst());
    if (stroke.points.size() == 2) {
        path.lineTo(stroke.points.constLast());
    } else {
        for (int i = 1; i < stroke.points.size() - 1; ++i) {
            const QPointF midPoint = (stroke.points.at(i) + stroke.points.at(i + 1)) * 0.5;
            path.quadTo(stroke.points.at(i), midPoint);
        }
        path.lineTo(stroke.points.constLast());
    }

    painter.drawPath(path);
    painter.restore();
}

QVector<QPointF> CanvasView::interpolateSegment(const QPointF &from, const QPointF &to) const
{
    QVector<QPointF> points;
    const qreal distance = QLineF(from, to).length();
    if (distance < 0.8) {
        return points;
    }

    const qreal step = std::max<qreal>(0.9, m_currentWidth * 0.18);
    const int steps = std::max(1, static_cast<int>(std::ceil(distance / step)));
    points.reserve(steps);

    for (int i = 1; i < steps; ++i) {
        const qreal t = static_cast<qreal>(i) / static_cast<qreal>(steps);
        const QPointF interpolated = from + (to - from) * t;
        points.append(interpolated);
    }

    return points;
}

QColor CanvasView::colorForIndex(int index) const
{
    const QColor start(192, 19, 76);
    const QColor end(24, 233, 199);
    const qreal t = static_cast<qreal>(index - 1) / 8.0;

    const int red = start.red() + static_cast<int>((end.red() - start.red()) * t);
    const int green = start.green() + static_cast<int>((end.green() - start.green()) * t);
    const int blue = start.blue() + static_cast<int>((end.blue() - start.blue()) * t);
    return QColor(red, green, blue);
}

QString CanvasView::nextStrokeId() const
{
    return QStringLiteral("%1-%2")
        .arg(QDateTime::currentMSecsSinceEpoch())
        .arg(QRandomGenerator::global()->generate64());
}
