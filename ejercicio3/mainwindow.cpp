#include "mainwindow.h"

#include "canvasview.h"
#include "drawingmodel.h"
#include "syncmanager.h"

#include <QAbstractButton>
#include <QButtonGroup>
#include <QColor>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSize>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_model(new DrawingModel(this))
    , m_canvas(new CanvasView(m_model, this))
    , m_syncManager(new SyncManager(m_model, this))
{
    setWindowTitle(QStringLiteral("Lienzo Colaborativo Qt"));
    resize(1180, 760);
    setCentralWidget(m_canvas);
    buildTopPanel();

    statusBar()->showMessage(QStringLiteral("Teclas 1-9 cambian color, rueda cambia grosor, click derecho borra"));
    updateBrushInfo(m_canvas->currentColor(), m_canvas->currentWidth(), false, 1);

    connect(m_canvas, &CanvasView::brushChanged, this, &MainWindow::updateBrushInfo);
    connect(m_saveButton, &QPushButton::clicked, m_syncManager, &SyncManager::saveNow);
    connect(m_syncManager, &SyncManager::statusChanged, m_statusLabel, &QLabel::setText);

    m_syncManager->start();
}

void MainWindow::buildTopPanel()
{
    QToolBar *toolbar = new QToolBar(QStringLiteral("Paint"), this);
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(20, 20));
    toolbar->setStyleSheet(QStringLiteral(
        "QToolBar { background: #DCEBFA; border: none; padding: 10px 12px; spacing: 14px; }"
        "QLabel { color: #23303D; }"));
    addToolBar(Qt::TopToolBarArea, toolbar);

    QWidget *panel = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(panel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(14);

    QFrame *brandCard = new QFrame(panel);
    brandCard->setStyleSheet(QStringLiteral("QFrame { background: #F7FBFF; border: 1px solid #B6CEE6; border-radius: 8px; }"));
    QVBoxLayout *brandLayout = new QVBoxLayout(brandCard);
    brandLayout->setContentsMargins(12, 10, 12, 10);
    brandLayout->setSpacing(4);
    m_titleLabel = new QLabel(QStringLiteral("Qt Paint Colaborativo"), brandCard);
    m_titleLabel->setStyleSheet(QStringLiteral("font-size: 17px; font-weight: 700; color: #12324A;"));
    QLabel *subtitleLabel = new QLabel(QStringLiteral("Lapiz, goma y sincronizacion en VPS"), brandCard);
    subtitleLabel->setStyleSheet(QStringLiteral("font-size: 12px; color: #4F667A;"));
    brandLayout->addWidget(m_titleLabel);
    brandLayout->addWidget(subtitleLabel);
    layout->addWidget(brandCard);

    QFrame *toolsCard = new QFrame(panel);
    toolsCard->setStyleSheet(QStringLiteral("QFrame { background: #FDFEFE; border: 1px solid #B6CEE6; border-radius: 8px; }"));
    QVBoxLayout *toolsLayout = new QVBoxLayout(toolsCard);
    toolsLayout->setContentsMargins(12, 10, 12, 10);
    toolsLayout->setSpacing(6);
    QLabel *toolsTitle = new QLabel(QStringLiteral("Herramienta"), toolsCard);
    toolsTitle->setStyleSheet(QStringLiteral("font-size: 11px; font-weight: 700; text-transform: uppercase; color: #567086;"));
    m_toolLabel = new QLabel(QStringLiteral("Lapiz"), toolsCard);
    m_toolLabel->setAlignment(Qt::AlignCenter);
    m_toolLabel->setMinimumWidth(88);
    m_toolLabel->setStyleSheet(QStringLiteral(
        "QLabel { background: #FFFFFF; color: #184A6A; border: 1px solid #94B7D5; border-radius: 14px; padding: 6px 14px; font-weight: 700; }"));
    toolsLayout->addWidget(toolsTitle);
    toolsLayout->addWidget(m_toolLabel);
    layout->addWidget(toolsCard);

    QFrame *colorsCard = new QFrame(panel);
    colorsCard->setStyleSheet(QStringLiteral("QFrame { background: #FDFEFE; border: 1px solid #B6CEE6; border-radius: 8px; }"));
    QVBoxLayout *colorsLayout = new QVBoxLayout(colorsCard);
    colorsLayout->setContentsMargins(12, 10, 12, 10);
    colorsLayout->setSpacing(6);
    QLabel *colorsTitle = new QLabel(QStringLiteral("Colores"), colorsCard);
    colorsTitle->setStyleSheet(QStringLiteral("font-size: 11px; font-weight: 700; text-transform: uppercase; color: #567086;"));
    QGridLayout *paletteLayout = new QGridLayout();
    paletteLayout->setHorizontalSpacing(6);
    paletteLayout->setVerticalSpacing(6);
    m_paletteGroup = new QButtonGroup(this);
    m_paletteGroup->setExclusive(true);

    const QList<QColor> colors = m_canvas->paletteColors();
    for (int i = 0; i < colors.size(); ++i) {
        QPushButton *button = createColorButton(colors.at(i), i + 1);
        m_paletteGroup->addButton(button, i + 1);
        paletteLayout->addWidget(button, i / 5, i % 5);
    }

    const QList<QAbstractButton *> buttons = m_paletteGroup->buttons();
    for (QAbstractButton *button : buttons) {
        connect(button, &QAbstractButton::clicked, this, [this, button]() {
            m_canvas->setCurrentColorIndex(m_paletteGroup->id(button));
        });
    }
    colorsLayout->addWidget(colorsTitle);
    colorsLayout->addLayout(paletteLayout);
    layout->addWidget(colorsCard);

    QFrame *brushCard = new QFrame(panel);
    brushCard->setStyleSheet(QStringLiteral("QFrame { background: #FDFEFE; border: 1px solid #B6CEE6; border-radius: 8px; }"));
    QVBoxLayout *brushLayout = new QVBoxLayout(brushCard);
    brushLayout->setContentsMargins(12, 10, 12, 10);
    brushLayout->setSpacing(6);
    QLabel *brushTitle = new QLabel(QStringLiteral("Pincel"), brushCard);
    brushTitle->setStyleSheet(QStringLiteral("font-size: 11px; font-weight: 700; text-transform: uppercase; color: #567086;"));
    m_brushPreview = new QFrame(brushCard);
    m_brushPreview->setFixedSize(54, 54);
    m_brushPreview->setStyleSheet(QStringLiteral("QFrame { background: white; border: 1px solid #B7CDE1; border-radius: 27px; }"));
    m_brushLabel = new QLabel(brushCard);
    m_brushLabel->setStyleSheet(QStringLiteral("font-size: 12px; color: #314659;"));
    brushLayout->addWidget(brushTitle);
    brushLayout->addWidget(m_brushPreview, 0, Qt::AlignHCenter);
    brushLayout->addWidget(m_brushLabel);
    layout->addWidget(brushCard);

    QFrame *saveCard = new QFrame(panel);
    saveCard->setStyleSheet(QStringLiteral("QFrame { background: #F7FBFF; border: 1px solid #B6CEE6; border-radius: 8px; }"));
    QVBoxLayout *saveLayout = new QVBoxLayout(saveCard);
    saveLayout->setContentsMargins(12, 10, 12, 10);
    saveLayout->setSpacing(6);
    QLabel *saveTitle = new QLabel(QStringLiteral("Archivo"), saveCard);
    saveTitle->setStyleSheet(QStringLiteral("font-size: 11px; font-weight: 700; text-transform: uppercase; color: #567086;"));
    m_saveButton = new QPushButton(QStringLiteral("Guardar"), saveCard);
    m_saveButton->setCursor(Qt::PointingHandCursor);
    m_saveButton->setMinimumHeight(38);
    m_saveButton->setStyleSheet(QStringLiteral(
        "QPushButton { background: #0078D4; color: white; border: none; border-radius: 4px; padding: 8px 22px; font-size: 14px; font-weight: 700; }"
        "QPushButton:hover { background: #1384DB; }"
        "QPushButton:pressed { background: #005EA8; }"));
    saveLayout->addWidget(saveTitle);
    saveLayout->addWidget(m_saveButton);
    layout->addWidget(saveCard);

    QFrame *statusCard = new QFrame(panel);
    statusCard->setStyleSheet(QStringLiteral("QFrame { background: #FDFEFE; border: 1px solid #B6CEE6; border-radius: 8px; }"));
    QVBoxLayout *statusLayout = new QVBoxLayout(statusCard);
    statusLayout->setContentsMargins(12, 10, 12, 10);
    statusLayout->setSpacing(6);
    QLabel *statusTitle = new QLabel(QStringLiteral("Estado"), statusCard);
    statusTitle->setStyleSheet(QStringLiteral("font-size: 11px; font-weight: 700; text-transform: uppercase; color: #567086;"));
    m_statusLabel = new QLabel(QStringLiteral("Inicializando..."), statusCard);
    m_statusLabel->setMinimumWidth(230);
    m_statusLabel->setWordWrap(true);
    m_statusLabel->setStyleSheet(QStringLiteral("font-size: 12px; color: #314659;"));
    statusLayout->addWidget(statusTitle);
    statusLayout->addWidget(m_statusLabel);
    layout->addWidget(statusCard);
    layout->addStretch(1);

    toolbar->addWidget(panel);
}

QPushButton *MainWindow::createColorButton(const QColor &color, int index)
{
    QPushButton *button = new QPushButton(QString::number(index), this);
    button->setCheckable(true);
    button->setCursor(Qt::PointingHandCursor);
    button->setFixedSize(34, 34);
    button->setStyleSheet(QStringLiteral(
        "QPushButton { background: %1; color: white; border: 2px solid #D9E6F2; border-radius: 5px; font-weight: 700; }"
        "QPushButton:checked { border: 3px solid #111827; }"
        "QPushButton:hover { border: 2px solid #4A6D8C; }")
                              .arg(color.name(QColor::HexRgb)));
    if (index == 1) {
        button->setChecked(true);
    }
    return button;
}

void MainWindow::updateBrushInfo(const QColor &color, qreal width, bool eraser, int colorIndex)
{
    const QString modeText = eraser ? QStringLiteral("Goma blanca") : QStringLiteral("Lapiz");
    const QString previewColor = eraser ? QStringLiteral("#FFFFFF") : color.name(QColor::HexRgb);
    m_toolLabel->setText(modeText);
    m_toolLabel->setStyleSheet(QStringLiteral(
        "QLabel { background: %1; color: %2; border: 1px solid #94B7D5; border-radius: 14px; padding: 6px 14px; font-weight: 700; }")
                                   .arg(eraser ? QStringLiteral("#FFFDF2") : QStringLiteral("#FFFFFF"))
                                   .arg(eraser ? QStringLiteral("#8A5A00") : QStringLiteral("#184A6A")));
    m_brushPreview->setStyleSheet(QStringLiteral(
        "QFrame { background: white; border: %1px solid %2; border-radius: 27px; }")
                                      .arg(qMax(2, static_cast<int>(width / 4.0)))
                                      .arg(previewColor));
    m_brushLabel->setText(QStringLiteral("%1 | Color %2 | %3 px")
                              .arg(modeText)
                              .arg(colorIndex)
                              .arg(QString::number(width, 'f', 1)));

    if (m_paletteGroup && m_paletteGroup->button(colorIndex)) {
        m_paletteGroup->button(colorIndex)->setChecked(true);
    }
}
