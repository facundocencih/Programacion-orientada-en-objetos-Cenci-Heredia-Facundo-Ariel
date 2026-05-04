#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>

class Logger : public QObject
{
    Q_OBJECT

public:
    explicit Logger(const QString &ruta, QObject *parent = nullptr);

public slots:
    void registrar(const QString &descripcion);

private:
    QString m_ruta;
};

#endif // LOGGER_H
