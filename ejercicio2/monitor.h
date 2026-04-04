#ifndef MONITOR_H
#define MONITOR_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QString>

class Monitor : public QObject {
    Q_OBJECT

public:
    explicit Monitor(QObject *parent = nullptr);
    void setCheckInterval(int seconds);
    void setServerUrl(const QString &url); // Now it's IP or hostname
    void setAlertThreshold(int threshold);
    void setSshCredentials(const QString &user, const QString &password);
    void manualRefresh();

signals:
    void dataReceived(const QString &status, const QString &uptime, const QString &load, const QString &memoryUsed, const QString &diskFree, const QString &lastCheck);
    void errorOccurred(const QString &error);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onTimerTimeout();

private:
    void collectData();
    QProcess *process;
    QTimer *timer;
    int checkInterval;
    QString serverHost;
    int alertThreshold;
    QString sshUser;
    QString sshPassword;
};

#endif // MONITOR_H