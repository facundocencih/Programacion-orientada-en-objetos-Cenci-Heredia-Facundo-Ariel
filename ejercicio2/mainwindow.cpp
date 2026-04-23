#include "mainwindow.h"

#include <QDateTime>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Prometheus VPS Monitor");
    resize(1180, 760);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    central->setObjectName("centralRoot");

    auto *rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(30, 28, 30, 28);
    rootLayout->setSpacing(24);

    auto *heroCard = new QFrame(this);
    heroCard->setObjectName("heroCard");
    auto *heroLayout = new QVBoxLayout(heroCard);
    heroLayout->setContentsMargins(28, 26, 28, 26);
    heroLayout->setSpacing(18);

    auto *title = new QLabel("Cloud Server Health", this);
    title->setObjectName("pageTitle");
    auto *subtitle = new QLabel("Panel operativo inspirado en Prometheus para seguimiento de un VPS en tiempo real.", this);
    subtitle->setObjectName("pageSubtitle");

    auto *statusRow = new QHBoxLayout();
    statusRow->setSpacing(16);
    m_statusBadge = new QLabel("SIN DATOS", this);
    m_statusBadge->setObjectName("statusBadge");
    m_summaryLabel = new QLabel("Esperando primer chequeo del endpoint.", this);
    m_summaryLabel->setObjectName("summaryLabel");
    statusRow->addWidget(m_statusBadge, 0);
    statusRow->addWidget(m_summaryLabel, 1);

    heroLayout->addWidget(title);
    heroLayout->addWidget(subtitle);
    heroLayout->addLayout(statusRow);

    auto *controlsCard = new QFrame(this);
    controlsCard->setObjectName("panelCard");
    auto *controlsLayout = new QGridLayout(controlsCard);
    controlsLayout->setContentsMargins(24, 24, 24, 24);
    controlsLayout->setHorizontalSpacing(18);
    controlsLayout->setVerticalSpacing(16);

    auto *endpointTitle = new QLabel("Endpoint de salud", this);
    endpointTitle->setObjectName("fieldLabel");
    m_endpointEdit = new QLineEdit(this);
    m_endpointEdit->setText("http://173.212.209.61:5000/metrics");
    m_endpointEdit->setPlaceholderText("http://173.212.209.61:5000/metrics");

    auto *intervalTitle = new QLabel("Intervalo (segundos)", this);
    intervalTitle->setObjectName("fieldLabel");
    m_intervalSpin = new QSpinBox(this);
    m_intervalSpin->setRange(5, 3600);
    m_intervalSpin->setValue(m_monitorService.refreshIntervalSeconds());

    auto *thresholdTitle = new QLabel("Umbral de alerta (%)", this);
    thresholdTitle->setObjectName("fieldLabel");
    m_thresholdSpin = new QSpinBox(this);
    m_thresholdSpin->setRange(50, 100);
    m_thresholdSpin->setValue(m_monitorService.alertThresholdPercent());

    m_refreshButton = new QPushButton("Refrescar ahora", this);
    m_refreshButton->setObjectName("accentButton");

    controlsLayout->addWidget(endpointTitle, 0, 0);
    controlsLayout->addWidget(m_endpointEdit, 0, 1, 1, 3);
    controlsLayout->addWidget(intervalTitle, 1, 0);
    controlsLayout->addWidget(m_intervalSpin, 1, 1);
    controlsLayout->addWidget(thresholdTitle, 1, 2);
    controlsLayout->addWidget(m_thresholdSpin, 1, 3);
    controlsLayout->addWidget(m_refreshButton, 0, 4, 2, 1);

    auto *metricsLayout = new QGridLayout();
    metricsLayout->setHorizontalSpacing(20);
    metricsLayout->setVerticalSpacing(20);

    metricsLayout->addWidget(buildMetricCard("UPTIME", &m_uptimeValue, nullptr), 0, 0);
    metricsLayout->addWidget(buildMetricCard("CPU LOAD", &m_cpuValue, &m_cpuBar), 0, 1);
    metricsLayout->addWidget(buildMetricCard("MEMORY", &m_memoryValue, &m_memoryBar), 1, 0);
    metricsLayout->addWidget(buildMetricCard("DISK", &m_diskValue, &m_diskBar), 1, 1);

    auto *bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(20);

    auto *activityCard = new QFrame(this);
    activityCard->setObjectName("panelCard");
    auto *activityLayout = new QVBoxLayout(activityCard);
    activityLayout->setContentsMargins(24, 24, 24, 24);
    activityLayout->setSpacing(14);

    auto *activityTitle = new QLabel("Historial corto de eventos", this);
    activityTitle->setObjectName("sectionTitle");
    m_eventsList = new QListWidget(this);
    m_eventsList->addItem("Todavia no hay eventos registrados.");
    activityLayout->addWidget(activityTitle);
    activityLayout->addWidget(m_eventsList, 1);

    auto *metaCard = new QFrame(this);
    metaCard->setObjectName("panelCard");
    auto *metaLayout = new QVBoxLayout(metaCard);
    metaLayout->setContentsMargins(24, 24, 24, 24);
    metaLayout->setSpacing(12);

    auto *metaTitle = new QLabel("Ultimo chequeo", this);
    metaTitle->setObjectName("sectionTitle");
    m_lastCheckLabel = new QLabel("Sin ejecucion previa", this);
    m_lastCheckLabel->setObjectName("lastCheckValue");
    auto *hintLabel = new QLabel(
        "QLineEdit permite cambiar el endpoint, QPushButton fuerza un chequeo inmediato y los QSpinBox ajustan el intervalo y el umbral operativo.",
        this);
    hintLabel->setWordWrap(true);
    hintLabel->setObjectName("hintLabel");

    metaLayout->addWidget(metaTitle);
    metaLayout->addWidget(m_lastCheckLabel);
    metaLayout->addStretch();
    metaLayout->addWidget(hintLabel);

    bottomLayout->addWidget(activityCard, 2);
    bottomLayout->addWidget(metaCard, 1);

    rootLayout->addWidget(heroCard);
    rootLayout->addWidget(controlsCard);
    rootLayout->addLayout(metricsLayout);
    rootLayout->addLayout(bottomLayout, 1);

    setStyleSheet(R"(
        QMainWindow, QWidget {
            background: #0e141b;
            color: #e6edf3;
            font-family: "Segoe UI";
            font-size: 11pt;
        }
        QWidget#centralRoot {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #0c1218, stop:1 #121b24);
        }
        QFrame#heroCard {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #1b2530, stop:0.5 #232f3c, stop:1 #263444);
            border: 1px solid #41505f;
            border-radius: 24px;
        }
        QFrame#panelCard {
            background: #18212b;
            border: 1px solid #324252;
            border-radius: 20px;
        }
        QLabel#pageTitle {
            font-size: 28pt;
            font-weight: 700;
            color: #ffb357;
        }
        QLabel#pageSubtitle {
            color: #9fb0c0;
            font-size: 12pt;
        }
        QLabel#statusBadge {
            padding: 10px 18px;
            border-radius: 16px;
            background: #253241;
            font-weight: 700;
            min-width: 120px;
        }
        QLabel#summaryLabel {
            color: #d5dde6;
            font-size: 12pt;
        }
        QLabel#fieldLabel, QLabel#sectionTitle {
            color: #8ea1b3;
            font-size: 10pt;
            font-weight: 600;
        }
        QLabel#metricTitle {
            color: #92a5b7;
            font-size: 10pt;
            font-weight: 700;
        }
        QLabel#metricValue {
            color: #f8fafc;
            font-size: 24pt;
            font-weight: 700;
        }
        QLabel#lastCheckValue {
            color: #ffcf8a;
            font-size: 18pt;
            font-weight: 700;
        }
        QLabel#hintLabel {
            color: #97a9bb;
        }
        QLineEdit, QSpinBox, QListWidget {
            background: #0c1117;
            border: 1px solid #3b4b5b;
            border-radius: 12px;
            padding: 12px 14px;
            color: #eef4fb;
        }
        QLineEdit:focus, QSpinBox:focus {
            border: 1px solid #ff9d3d;
        }
        QPushButton#accentButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #ff9d3d, stop:1 #ffc06a);
            color: #10151b;
            border: none;
            border-radius: 14px;
            padding: 14px 22px;
            font-weight: 700;
            min-width: 170px;
        }
        QPushButton#accentButton:hover {
            background: #ffb357;
        }
        QPushButton#accentButton:disabled {
            background: #7d654b;
            color: #d5c2a7;
        }
        QProgressBar {
            background: #0b1015;
            border: 1px solid #334352;
            border-radius: 11px;
            text-align: center;
            min-height: 22px;
            color: #eff4f9;
            font-weight: 600;
            padding: 2px;
        }
        QProgressBar::chunk {
            border-radius: 8px;
            background: #39c172;
        }
        QListWidget {
            outline: 0;
        }
        QListWidget::item {
            padding: 12px 10px;
            border-bottom: 1px solid #24303a;
        }
    )");

    connect(m_refreshButton, &QPushButton::clicked, this, &MainWindow::applyEndpoint);
    connect(m_intervalSpin, qOverload<int>(&QSpinBox::valueChanged), &m_monitorService, &MonitorService::setRefreshIntervalSeconds);
    connect(m_thresholdSpin, qOverload<int>(&QSpinBox::valueChanged), &m_monitorService, &MonitorService::setAlertThresholdPercent);
    connect(&m_monitorService, &MonitorService::snapshotUpdated, this, &MainWindow::handleSnapshot);
    connect(&m_monitorService, &MonitorService::requestStateChanged, this, &MainWindow::handleBusyState);

    applyEndpoint();
}

QWidget *MainWindow::buildMetricCard(const QString &title, QLabel **valueLabel, AnimatedProgressBar **progressBar)
{
    auto *card = new QFrame(this);
    card->setObjectName("panelCard");

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(22, 20, 22, 20);
    layout->setSpacing(14);

    auto *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("metricTitle");
    auto *mainValue = new QLabel("--", card);
    mainValue->setObjectName("metricValue");

    layout->addWidget(titleLabel);
    layout->addWidget(mainValue);

    if (progressBar) {
        auto *bar = new AnimatedProgressBar(card);
        bar->setRange(0, 100);
        bar->setValue(0);
        bar->setFormat("--%");
        layout->addWidget(bar);
        *progressBar = bar;
    } else {
        layout->addStretch();
    }

    *valueLabel = mainValue;
    return card;
}

void MainWindow::applyEndpoint()
{
    QUrl endpoint = QUrl::fromUserInput(m_endpointEdit->text().trimmed());
    m_monitorService.setEndpoint(endpoint);
    m_monitorService.setRefreshIntervalSeconds(m_intervalSpin->value());
    m_monitorService.setAlertThresholdPercent(m_thresholdSpin->value());
    m_monitorService.refreshNow();
}

void MainWindow::handleSnapshot(const MetricSnapshot &snapshot)
{
    setStatusStyle(snapshot.overallStatus);
    m_statusBadge->setText(snapshot.overallStatus);
    m_summaryLabel->setText(snapshot.summary);
    m_lastCheckLabel->setText(snapshot.checkedAt.toString("dd/MM/yyyy HH:mm:ss"));

    m_uptimeValue->setText(snapshot.uptimeText);
    m_cpuValue->setText(QString("%1%").arg(qRound(snapshot.cpuLoadPercent)));
    m_memoryValue->setText(QString("%1%").arg(qRound(snapshot.memoryPercent)));
    m_diskValue->setText(QString("%1%").arg(qRound(snapshot.diskPercent)));

    m_cpuBar->animateToValue(qRound(snapshot.cpuLoadPercent));
    m_cpuBar->setFormat(QString("%1%").arg(qRound(snapshot.cpuLoadPercent)));
    setProgressStyle(m_cpuBar, snapshot.cpuLoadPercent);

    m_memoryBar->animateToValue(qRound(snapshot.memoryPercent));
    m_memoryBar->setFormat(QString("%1%").arg(qRound(snapshot.memoryPercent)));
    setProgressStyle(m_memoryBar, snapshot.memoryPercent);

    m_diskBar->animateToValue(qRound(snapshot.diskPercent));
    m_diskBar->setFormat(QString("%1%").arg(qRound(snapshot.diskPercent)));
    setProgressStyle(m_diskBar, snapshot.diskPercent);

    m_eventsList->clear();
    for (const QString &event : snapshot.events) {
        m_eventsList->addItem(event);
    }
}

void MainWindow::handleBusyState(bool busy)
{
    m_refreshButton->setDisabled(busy);
    m_refreshButton->setText(busy ? "Consultando..." : "Refrescar ahora");
}

void MainWindow::setProgressStyle(AnimatedProgressBar *bar, double value) const
{
    QString color = "#39c172";
    QString glow = "rgba(57, 193, 114, 0.28)";
    if (value >= 95.0) {
        color = "#ff5c5c";
        glow = "rgba(255, 92, 92, 0.26)";
    } else if (value >= m_thresholdSpin->value()) {
        color = "#ffb357";
        glow = "rgba(255, 179, 87, 0.24)";
    }

    bar->setStyleSheet(QString(
        "QProgressBar {"
        " background: #0b1015;"
        " border: 1px solid #334352;"
        " border-radius: 11px;"
        " text-align: center;"
        " min-height: 22px;"
        " color: #eff4f9;"
        " font-weight: 600;"
        " padding: 2px;"
        "}"
        "QProgressBar::chunk {"
        " border-radius: 8px;"
        " background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 %1, stop:1 %2);"
        " margin: 0px;"
        "}").arg(color, glow));
}

void MainWindow::setStatusStyle(const QString &status)
{
    QString background = "#214f3c";
    QString foreground = "#cbffdd";

    if (status == "ALERTA") {
        background = "#5a3d17";
        foreground = "#ffd6a1";
    } else if (status == "CAIDO") {
        background = "#642b2b";
        foreground = "#ffd0d0";
    }

    m_statusBadge->setStyleSheet(QString(
        "QLabel {"
        " padding: 8px 16px;"
        " border-radius: 14px;"
        " background: %1;"
        " color: %2;"
        " font-weight: 700;"
        " min-width: 110px;"
        "}").arg(background, foreground));
}
