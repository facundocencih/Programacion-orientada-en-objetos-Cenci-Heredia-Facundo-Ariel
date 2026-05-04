#include "pintura.h"

#include <QKeyEvent>
#include <QKeySequence>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QtGlobal>
#include <QWheelEvent>

#include <utility>

Pintura::Pintura(DatabaseManager *database, int userId, QWidget *parent)
    : QWidget(parent),
      database(database),
      userId(userId),
      strokes(database ? database->loadStrokes(userId) : QVector<StrokeData>())
{
    setAttribute(Qt::WA_StaticContents);
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(760, 520);
    setAutoFillBackground(true);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
}

QColor Pintura::currentColor() const
{
    return brushColor;
}

int Pintura::currentWidth() const
{
    return brushWidth;
}

void Pintura::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), Qt::white);

    for (const StrokeData &stroke : std::as_const(strokes)) {
        drawStroke(painter, stroke);
    }

    if (drawing) {
        drawStroke(painter, currentStroke);
    }
}

void Pintura::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    setFocus();
    drawing = true;
    currentStroke = StrokeData();
    currentStroke.color = brushColor;
    currentStroke.width = brushWidth;
    currentStroke.points.append(event->pos());
}

void Pintura::mouseMoveEvent(QMouseEvent *event)
{
    if (!drawing || !(event->buttons() & Qt::LeftButton)) {
        return;
    }

    currentStroke.points.append(event->pos());
    update();
}

void Pintura::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !drawing) {
        return;
    }

    currentStroke.points.append(event->pos());
    finishCurrentStroke();
}

void Pintura::wheelEvent(QWheelEvent *event)
{
    const int step = event->angleDelta().y() > 0 ? 1 : -1;
    brushWidth = qBound(1, brushWidth + step, 40);
    emit brushChanged(brushColor, brushWidth, brushColorName);
    event->accept();
}

void Pintura::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Undo)) {
        undoLastStroke();
        return;
    }

    switch (event->key()) {
    case Qt::Key_R:
        setBrushColor(Qt::red, "Rojo");
        break;
    case Qt::Key_G:
        setBrushColor(Qt::green, "Verde");
        break;
    case Qt::Key_B:
        setBrushColor(Qt::blue, "Azul");
        break;
    case Qt::Key_Escape:
        clearCanvas();
        break;
    default:
        QWidget::keyPressEvent(event);
        return;
    }
}

void Pintura::drawStroke(QPainter &painter, const StrokeData &stroke) const
{
    if (stroke.points.isEmpty()) {
        return;
    }

    painter.save();
    QPen pen(stroke.color, stroke.width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    QPainterPath path(stroke.points.constFirst());
    if (stroke.points.size() == 1) {
        painter.drawPoint(stroke.points.first());
    } else if (stroke.points.size() == 2) {
        path.lineTo(stroke.points.constLast());
        painter.drawPath(path);
    } else {
        for (int i = 1; i < stroke.points.size() - 1; ++i) {
            const QPointF midPoint = (stroke.points.at(i) + stroke.points.at(i + 1)) * 0.5;
            path.quadTo(stroke.points.at(i), midPoint);
        }
        path.lineTo(stroke.points.constLast());
        painter.drawPath(path);
    }
    painter.restore();
}

void Pintura::finishCurrentStroke()
{
    drawing = false;

    if (currentStroke.points.size() < 2) {
        update();
        return;
    }

    if (database) {
        currentStroke.id = database->saveStroke(userId, currentStroke.color, currentStroke.width, currentStroke.points);
    }

    strokes.append(currentStroke);
    undoableActions = qMin(undoableActions + 1, 10);
    update();
}

void Pintura::setBrushColor(const QColor &color, const QString &colorName)
{
    brushColor = color;
    brushColorName = colorName;
    emit brushChanged(brushColor, brushWidth, brushColorName);
    update();
}

void Pintura::undoLastStroke()
{
    if (undoableActions <= 0 || strokes.isEmpty()) {
        return;
    }

    const StrokeData stroke = strokes.takeLast();
    if (database && stroke.id > 0) {
        database->deleteStroke(stroke.id);
    }

    --undoableActions;
    update();
}

void Pintura::clearCanvas()
{
    strokes.clear();
    currentStroke = StrokeData();
    drawing = false;
    undoableActions = 0;

    if (database) {
        database->clearStrokes(userId);
    }

    update();
}
