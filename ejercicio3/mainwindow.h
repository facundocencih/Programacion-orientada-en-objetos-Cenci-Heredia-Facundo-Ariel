#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QColor>
#include <QMainWindow>

class CanvasView;
class DrawingModel;
class QButtonGroup;
class QFrame;
class QLabel;
class QPushButton;
class SyncManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void buildTopPanel();
    QPushButton *createColorButton(const QColor &color, int index);
    void updateBrushInfo(const QColor &color, qreal width, bool eraser, int colorIndex);

    DrawingModel *m_model = nullptr;
    CanvasView *m_canvas = nullptr;
    SyncManager *m_syncManager = nullptr;
    QButtonGroup *m_paletteGroup = nullptr;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_statusLabel = nullptr;
    QLabel *m_brushLabel = nullptr;
    QLabel *m_toolLabel = nullptr;
    QFrame *m_brushPreview = nullptr;
    QPushButton *m_saveButton = nullptr;
};

#endif
