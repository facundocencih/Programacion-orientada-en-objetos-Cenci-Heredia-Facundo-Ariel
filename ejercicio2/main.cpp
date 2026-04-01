#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QGroupBox>
#include <QDateTime>
#include "monitor.h"

int main(int argc, char** argv) {
    QApplication a(argc, argv);

    QWidget *ventana = new QWidget;
    ventana->setWindowTitle("Local System Monitor Panel");
    ventana->resize(600, 400);

    // Tema negro-azul
    ventana->setStyleSheet("QWidget { background-color: #1a1a2e; color: #e0e0e0; }"
                           "QLabel { color: #e0e0e0; }"
                           "QPushButton { background-color: #16213e; color: #e0e0e0; border: 1px solid #0f3460; }"
                           "QPushButton:hover { background-color: #0f3460; }"
                           "QLineEdit, QSpinBox { background-color: #16213e; color: #e0e0e0; border: 1px solid #0f3460; }"
                           "QProgressBar { background-color: #16213e; color: #e0e0e0; }"
                           "QTextEdit { background-color: #16213e; color: #e0e0e0; border: 1px solid #0f3460; }");

    Monitor *monitor = new Monitor(ventana);

    // Configuración
    QGroupBox *configGroup = new QGroupBox("Configuración");
    QVBoxLayout *configLayout = new QVBoxLayout;
    QHBoxLayout *intervalLayout = new QHBoxLayout;
    QLabel *intervalLabel = new QLabel("Intervalo (seg):");
    QSpinBox *intervalSpin = new QSpinBox;
    intervalSpin->setRange(10, 3600);
    intervalSpin->setValue(5);
    intervalLayout->addWidget(intervalLabel);
    intervalLayout->addWidget(intervalSpin);
    configLayout->addLayout(intervalLayout);

    QPushButton *refreshButton = new QPushButton("Refrescar Manual");
    configLayout->addWidget(refreshButton);
    configGroup->setLayout(configLayout);

    // Métricas
    QGroupBox *metricsGroup = new QGroupBox("Métricas");
    QVBoxLayout *metricsLayout = new QVBoxLayout;
    QLabel *statusLabel = new QLabel("Estado: Desconocido");
    QLabel *uptimeLabel = new QLabel("Uptime: N/A");
    QLabel *loadLabel = new QLabel("Carga: N/A");
    QLabel *memoryLabel = new QLabel("Memoria usada: N/A");
    QLabel *diskLabel = new QLabel("Espacio libre: N/A");
    QLabel *lastCheckLabel = new QLabel("Último Chequeo: N/A");
    metricsLayout->addWidget(statusLabel);
    metricsLayout->addWidget(uptimeLabel);
    metricsLayout->addWidget(loadLabel);
    metricsLayout->addWidget(memoryLabel);
    metricsLayout->addWidget(diskLabel);
    metricsLayout->addWidget(lastCheckLabel);
    metricsGroup->setLayout(metricsLayout);

    // Historial
    QGroupBox *historyGroup = new QGroupBox("Historial de Eventos");
    QTextEdit *historyEdit = new QTextEdit;
    historyEdit->setMaximumHeight(100);
    QVBoxLayout *historyLayout = new QVBoxLayout;
    historyLayout->addWidget(historyEdit);
    historyGroup->setLayout(historyLayout);

    // Layout principal
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(configGroup);
    mainLayout->addWidget(metricsGroup);
    mainLayout->addWidget(historyGroup);
    ventana->setLayout(mainLayout);

    // Conexiones
    QObject::connect(intervalSpin, QOverload<int>::of(&QSpinBox::valueChanged), monitor, &Monitor::setCheckInterval);
    QObject::connect(refreshButton, &QPushButton::clicked, monitor, &Monitor::manualRefresh);
    QObject::connect(monitor, &Monitor::dataReceived, [statusLabel, uptimeLabel, loadLabel, memoryLabel, diskLabel, lastCheckLabel, historyEdit](const QString &status, const QString &uptime, const QString &load, const QString &memoryUsed, const QString &diskFree, const QString &lastCheck) {
        statusLabel->setText("Estado: " + status);
        uptimeLabel->setText("Uptime: " + uptime);
        loadLabel->setText("Carga CPU: " + load);
        memoryLabel->setText("Memoria usada: " + memoryUsed);
        diskLabel->setText("Espacio libre: " + diskFree);
        lastCheckLabel->setText("Último Chequeo: " + lastCheck);
        historyEdit->append("[" + QDateTime::currentDateTime().toString() + "] Datos actualizados");
    });
    QObject::connect(monitor, &Monitor::errorOccurred, [historyEdit](const QString &error) {
        historyEdit->append("[" + QDateTime::currentDateTime().toString() + "] Error: " + error);
    });

    // Inicializar
    monitor->setCheckInterval(intervalSpin->value());

    ventana->show();
    return a.exec();
}