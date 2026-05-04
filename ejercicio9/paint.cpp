#include "paint.h"
#include "pintura.h"
#include "ui_paint.h"

#include <QColor>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QVBoxLayout>

Paint::Paint(DatabaseManager *database, int userId, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Paint)
{
    ui->setupUi(this);
    setWindowTitle("Ejercicio 09 - Coordenadas en base de datos");
    resize(1080, 720);
    setStyleSheet(QStringLiteral("QWidget { font-family: Segoe UI, Arial; background: #EEF5FC; color: #23303D; }"));

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(12);
    setLayout(layout);

    buildTopPanel(database, userId);
    canvas = new Pintura(database, userId, this);
    canvas->setStyleSheet(QStringLiteral(
        "Pintura { background: #FFFFFF; border: 1px solid #B6CEE6; border-radius: 8px; }"));
    canvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(canvas);

    connect(canvas, &Pintura::brushChanged, this, &Paint::updateBrushInfo);
    updateBrushInfo(canvas->currentColor(), canvas->currentWidth(), "Negro");
}

Paint::~Paint()
{
    delete ui;
}

void Paint::buildTopPanel(DatabaseManager *database, int userId)
{
    Q_UNUSED(database);
    Q_UNUSED(userId);

    auto *panel = new QFrame(this);
    panel->setStyleSheet(QStringLiteral(
        "QFrame#topPanel { background: #DCEBFA; border: 1px solid #B6CEE6; border-radius: 8px; }"
        "QLabel { background: transparent; }"));
    panel->setObjectName("topPanel");

    auto *layout = new QHBoxLayout(panel);
    layout->setContentsMargins(12, 10, 12, 10);
    layout->setSpacing(12);

    auto *brandCard = new QFrame(panel);
    brandCard->setStyleSheet(QStringLiteral("QFrame { background: #F7FBFF; border: 1px solid #B6CEE6; border-radius: 8px; }"));
    auto *brandLayout = new QVBoxLayout(brandCard);
    brandLayout->setContentsMargins(12, 9, 12, 9);
    brandLayout->setSpacing(3);
    auto *title = new QLabel("Ejercicio 09 - Paint SQLite", brandCard);
    title->setStyleSheet(QStringLiteral("font-size: 17px; font-weight: 700; color: #12324A;"));
    auto *subtitle = new QLabel("R rojo, G verde, B azul | rueda cambia grosor | Ctrl+Z deshace", brandCard);
    subtitle->setStyleSheet(QStringLiteral("font-size: 12px; color: #4F667A;"));
    brandLayout->addWidget(title);
    brandLayout->addWidget(subtitle);
    layout->addWidget(brandCard, 2);

    auto *colorsCard = new QFrame(panel);
    colorsCard->setStyleSheet(QStringLiteral("QFrame { background: #FDFEFE; border: 1px solid #B6CEE6; border-radius: 8px; }"));
    auto *colorsLayout = new QVBoxLayout(colorsCard);
    colorsLayout->setContentsMargins(12, 9, 12, 9);
    colorsLayout->setSpacing(7);
    auto *colorsTitle = new QLabel("COLORES", colorsCard);
    colorsTitle->setStyleSheet(QStringLiteral("font-size: 11px; font-weight: 700; color: #567086;"));
    auto *keysLayout = new QHBoxLayout();
    keysLayout->setSpacing(8);
    redCard = createColorCard("R", "Rojo", Qt::red);
    greenCard = createColorCard("G", "Verde", Qt::green);
    blueCard = createColorCard("B", "Azul", Qt::blue);
    keysLayout->addWidget(redCard);
    keysLayout->addWidget(greenCard);
    keysLayout->addWidget(blueCard);
    colorsLayout->addWidget(colorsTitle);
    colorsLayout->addLayout(keysLayout);
    layout->addWidget(colorsCard);

    auto *brushCard = new QFrame(panel);
    brushCard->setStyleSheet(QStringLiteral("QFrame { background: #FDFEFE; border: 1px solid #B6CEE6; border-radius: 8px; }"));
    auto *brushLayout = new QHBoxLayout(brushCard);
    brushLayout->setContentsMargins(12, 9, 12, 9);
    brushLayout->setSpacing(10);
    brushPreview = new QFrame(brushCard);
    brushPreview->setFixedSize(50, 50);
    selectedColorLabel = new QLabel("Negro", brushCard);
    selectedColorLabel->setStyleSheet(QStringLiteral("font-size: 15px; font-weight: 700; color: #12324A;"));
    brushLabel = new QLabel("4 px", brushCard);
    brushLabel->setStyleSheet(QStringLiteral("font-size: 12px; color: #4F667A;"));
    auto *textLayout = new QVBoxLayout();
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(2);
    textLayout->addWidget(selectedColorLabel);
    textLayout->addWidget(brushLabel);
    brushLayout->addWidget(brushPreview);
    brushLayout->addLayout(textLayout);
    layout->addWidget(brushCard);

    QWidget::layout()->addWidget(panel);
}

QFrame *Paint::createColorCard(const QString &key, const QString &name, const QColor &color)
{
    auto *card = new QFrame(this);
    card->setProperty("color", color.name(QColor::HexRgb));
    card->setFixedSize(78, 48);
    auto *layout = new QHBoxLayout(card);
    layout->setContentsMargins(8, 6, 8, 6);
    layout->setSpacing(7);

    auto *swatch = new QFrame(card);
    swatch->setFixedSize(20, 20);
    swatch->setStyleSheet(QStringLiteral("QFrame { background: %1; border: 1px solid #4A5568; border-radius: 10px; }").arg(color.name()));

    auto *text = new QLabel(QStringLiteral("<b>%1</b><br>%2").arg(key, name), card);
    text->setStyleSheet(QStringLiteral("font-size: 12px; color: #23303D;"));

    layout->addWidget(swatch);
    layout->addWidget(text);
    return card;
}

void Paint::updateBrushInfo(const QColor &color, int width, const QString &colorName)
{
    if (selectedColorLabel) {
        selectedColorLabel->setText(colorName);
    }
    if (brushLabel) {
        brushLabel->setText(QStringLiteral("%1 px").arg(width));
    }
    if (brushPreview) {
        brushPreview->setStyleSheet(QStringLiteral(
            "QFrame { background: white; border: %1px solid %2; border-radius: 25px; }")
                                        .arg(qMax(2, width / 3))
                                        .arg(color.name(QColor::HexRgb)));
    }
    markActiveColor(color);
}

void Paint::markActiveColor(const QColor &color)
{
    const QList<QFrame *> cards = {redCard, greenCard, blueCard};
    for (QFrame *card : cards) {
        if (!card) {
            continue;
        }
        const bool active = card->property("color").toString().compare(color.name(QColor::HexRgb), Qt::CaseInsensitive) == 0;
        card->setStyleSheet(QStringLiteral(
            "QFrame { background: %1; border: %2px solid %3; border-radius: 7px; }")
                                .arg(active ? QStringLiteral("#EAF4FF") : QStringLiteral("#FFFFFF"))
                                .arg(active ? 3 : 1)
                                .arg(active ? QStringLiteral("#111827") : QStringLiteral("#B6CEE6")));
    }
}
