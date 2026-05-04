#ifndef LOGGER_H
#define LOGGER_H

#include <QString>

class Logger
{
public:
    explicit Logger(const QString &logPath);

    void write(const QString &message);
    QString logPath() const;

private:
    QString m_logPath;
};

#endif // LOGGER_H

