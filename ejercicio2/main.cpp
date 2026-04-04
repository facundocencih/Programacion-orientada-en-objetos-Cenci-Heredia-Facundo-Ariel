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
    ventana->setWindowTitle("VPS Monitor Panel");
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
    QHBoxLayout *hostLayout = new QHBoxLayout;
    QLabel *hostLabel = new QLabel("IP/Host del VPS:");
    QLineEdit *hostEdit = new QLineEdit("173.212.209.61");
    hostLayout->addWidget(hostLabel);
    hostLayout->addWidget(hostEdit);
    configLayout->addLayout(hostLayout);

    QHBoxLayout *userLayout = new QHBoxLayout;
    QLabel *userLabel = new QLabel("Usuario SSH:");
    QLineEdit *userEdit = new QLineEdit("facundo");
    userLayout->addWidget(userLabel);
    userLayout->addWidget(userEdit);
    configLayout->addLayout(userLayout);

    QHBoxLayout *passLayout = new QHBoxLayout;
    QLabel *passLabel = new QLabel("Contraseña SSH:");
    QLineEdit *passEdit = new QLineEdit;
    passEdit->setPlaceholderText("Introduce contraseña");
    passEdit->setEchoMode(QLineEdit::Password);
    passLayout->addWidget(passLabel);
    passLayout->addWidget(passEdit);
    configLayout->addLayout(passLayout);

    QHBoxLayout *intervalLayout = new QHBoxLayout;
    QLabel *intervalLabel = new QLabel("Intervalo (seg):");
    QSpinBox *intervalSpin = new QSpinBox;
    intervalSpin->setRange(10, 3600);
    intervalSpin->setValue(60);
    intervalLayout->addWidget(intervalLabel);
    intervalLayout->addWidget(intervalSpin);
    configLayout->addLayout(intervalLayout);

    QHBoxLayout *thresholdLayout = new QHBoxLayout;
    QLabel *thresholdLabel = new QLabel("Umbral Alerta CPU (%):");
    QSpinBox *thresholdSpin = new QSpinBox;
    thresholdSpin->setRange(50, 100);
    thresholdSpin->setValue(80);
    thresholdLayout->addWidget(thresholdLabel);
    thresholdLayout->addWidget(thresholdSpin);
    configLayout->addLayout(thresholdLayout);

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
    QObject::connect(hostEdit, &QLineEdit::textChanged, monitor, &Monitor::setServerUrl);
    QObject::connect(userEdit, &QLineEdit::textChanged, [monitor, passEdit](const QString &user) {
        monitor->setSshCredentials(user, passEdit->text());
    });
    QObject::connect(passEdit, &QLineEdit::textChanged, [monitor, userEdit](const QString &pass) {
        monitor->setSshCredentials(userEdit->text(), pass);
    });
    QObject::connect(intervalSpin, QOverload<int>::of(&QSpinBox::valueChanged), monitor, &Monitor::setCheckInterval);
    QObject::connect(thresholdSpin, QOverload<int>::of(&QSpinBox::valueChanged), monitor, &Monitor::setAlertThreshold);
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
    monitor->setServerUrl(hostEdit->text());
    monitor->setSshCredentials(userEdit->text(), passEdit->text());
    monitor->setCheckInterval(intervalSpin->value());
    monitor->setAlertThreshold(thresholdSpin->value());

    ventana->show();
    return a.exec();
}