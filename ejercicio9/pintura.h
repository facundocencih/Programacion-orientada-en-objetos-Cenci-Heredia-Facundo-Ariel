#ifndef PINTURA_H
#define PINTURA_H

#include "databasemanager.h"

#include <QWidget>

class Pintura : public QWidget
{
    Q_OBJECT

public:
    explicit Pintura(DatabaseManager *database, int userId, QWidget *parent = nullptr);
    QColor currentColor() const;
    int currentWidth() const;

signals:
    void brushChanged(const QColor &color, int width, const QString &colorName);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void drawStroke(QPainter &painter, const StrokeData &stroke) const;
    void finishCurrentStroke();
    void undoLastStroke();
    void clearCanvas();
    void setBrushColor(const QColor &color, const QString &colorName);

    DatabaseManager *database = nullptr;
    int userId = -1;
    QVector<StrokeData> strokes;
    StrokeData currentStroke;
    bool drawing = false;
    QColor brushColor = Qt::black;
    QString brushColorName = "Negro";
    int brushWidth = 4;
    int undoableActions = 0;
};

#endif // PINTURA_H
