#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include "drawingmodel.h"

#include <QWidget>

class CanvasView : public QWidget
{
    Q_OBJECT

public:
    explicit CanvasView(DrawingModel *model, QWidget *parent = nullptr);

    QColor currentColor() const;
    qreal currentWidth() const;
    QList<QColor> paletteColors() const;

public slots:
    void setCurrentColorIndex(int index);

signals:
    void brushChanged(const QColor &color, qreal width, bool eraser, int colorIndex);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void beginStroke(const QPointF &point, bool eraser);
    void extendStroke(const QPointF &point);
    void finishStroke();
    void drawStroke(QPainter &painter, const Stroke &stroke) const;
    QVector<QPointF> interpolateSegment(const QPointF &from, const QPointF &to) const;
    QColor colorForIndex(int index) const;
    QString nextStrokeId() const;

    DrawingModel *m_model = nullptr;
    Stroke m_activeStroke;
    bool m_drawing = false;
    QColor m_currentColor;
    qreal m_currentWidth = 6.0;
    int m_currentColorIndex = 1;
};

#endif
