#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "animatedprogressbar.h"
#include "monitorservice.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QListWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void applyEndpoint();
    void handleSnapshot(const MetricSnapshot &snapshot);
    void handleBusyState(bool busy);

private:
    QWidget *buildMetricCard(const QString &title, QLabel **valueLabel, AnimatedProgressBar **progressBar);
    void setProgressStyle(AnimatedProgressBar *bar, double value) const;
    void setStatusStyle(const QString &status);

    MonitorService m_monitorService;

    QLabel *m_statusBadge = nullptr;
    QLabel *m_summaryLabel = nullptr;
    QLabel *m_lastCheckLabel = nullptr;
    QLabel *m_uptimeValue = nullptr;
    QLabel *m_cpuValue = nullptr;
    QLabel *m_memoryValue = nullptr;
    QLabel *m_diskValue = nullptr;

    AnimatedProgressBar *m_cpuBar = nullptr;
    AnimatedProgressBar *m_memoryBar = nullptr;
    AnimatedProgressBar *m_diskBar = nullptr;

    QLineEdit *m_endpointEdit = nullptr;
    QPushButton *m_refreshButton = nullptr;
    QSpinBox *m_intervalSpin = nullptr;
    QSpinBox *m_thresholdSpin = nullptr;
    QListWidget *m_eventsList = nullptr;
};

#endif
