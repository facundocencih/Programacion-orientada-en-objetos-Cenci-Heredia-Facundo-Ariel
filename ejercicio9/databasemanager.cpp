#include "databasemanager.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QDebug>

bool DatabaseManager::open()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    const QString path = QDir(QCoreApplication::applicationDirPath()).filePath("ejercicio9.sqlite");
    db.setDatabaseName(path);

    if (!db.open()) {
        qWarning() << "No se pudo abrir SQLite:" << db.lastError().text();
        return false;
    }

    return createSchema() && ensureDefaultUser();
}

bool DatabaseManager::createSchema()
{
    QSqlQuery query(db);

    const QStringList statements = {
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL UNIQUE,"
        "password_hash TEXT NOT NULL)",

        "CREATE TABLE IF NOT EXISTS access_logs ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL,"
        "success INTEGER NOT NULL,"
        "accessed_at TEXT NOT NULL)",

        "CREATE TABLE IF NOT EXISTS strokes ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "color TEXT NOT NULL,"
        "width INTEGER NOT NULL,"
        "created_at TEXT NOT NULL,"
        "FOREIGN KEY(user_id) REFERENCES users(id))",

        "CREATE TABLE IF NOT EXISTS stroke_points ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "stroke_id INTEGER NOT NULL,"
        "order_index INTEGER NOT NULL,"
        "x INTEGER NOT NULL,"
        "y INTEGER NOT NULL,"
        "FOREIGN KEY(stroke_id) REFERENCES strokes(id) ON DELETE CASCADE)"
    };

    for (const QString &statement : statements) {
        if (!query.exec(statement)) {
            qWarning() << "Error creando esquema:" << query.lastError().text();
            return false;
        }
    }

    query.exec("PRAGMA foreign_keys = ON");
    return true;
}

bool DatabaseManager::ensureDefaultUser()
{
    QSqlQuery exists(db);
    exists.prepare("SELECT id FROM users WHERE username = ?");
    exists.addBindValue("admin");
    if (!exists.exec()) {
        qWarning() << "Error consultando usuario admin:" << exists.lastError().text();
        return false;
    }

    if (exists.next()) {
        return true;
    }

    QSqlQuery insert(db);
    insert.prepare("INSERT INTO users (username, password_hash) VALUES (?, ?)");
    insert.addBindValue("admin");
    insert.addBindValue(passwordHash("admin"));
    if (!insert.exec()) {
        qWarning() << "Error creando usuario admin:" << insert.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::validateUser(const QString &username, const QString &password, int *userId)
{
    QSqlQuery query(db);
    query.prepare("SELECT id FROM users WHERE username = ? AND password_hash = ?");
    query.addBindValue(username.trimmed());
    query.addBindValue(passwordHash(password));

    if (!query.exec()) {
        qWarning() << "Error validando usuario:" << query.lastError().text();
        return false;
    }

    if (!query.next()) {
        return false;
    }

    if (userId) {
        *userId = query.value(0).toInt();
    }
    return true;
}

void DatabaseManager::registerAccess(const QString &username, bool success)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO access_logs (username, success, accessed_at) VALUES (?, ?, ?)");
    query.addBindValue(username.trimmed());
    query.addBindValue(success ? 1 : 0);
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    if (!query.exec()) {
        qWarning() << "Error registrando acceso:" << query.lastError().text();
    }
}

QVector<StrokeData> DatabaseManager::loadStrokes(int userId) const
{
    QVector<StrokeData> strokes;
    QSqlQuery strokeQuery(db);
    strokeQuery.prepare("SELECT id, color, width FROM strokes WHERE user_id = ? ORDER BY id");
    strokeQuery.addBindValue(userId);

    if (!strokeQuery.exec()) {
        qWarning() << "Error cargando trazos:" << strokeQuery.lastError().text();
        return strokes;
    }

    while (strokeQuery.next()) {
        StrokeData stroke;
        stroke.id = strokeQuery.value(0).toInt();
        stroke.color = QColor(strokeQuery.value(1).toString());
        stroke.width = strokeQuery.value(2).toInt();

        QSqlQuery pointQuery(db);
        pointQuery.prepare("SELECT x, y FROM stroke_points WHERE stroke_id = ? ORDER BY order_index");
        pointQuery.addBindValue(stroke.id);
        if (pointQuery.exec()) {
            while (pointQuery.next()) {
                stroke.points.append(QPoint(pointQuery.value(0).toInt(), pointQuery.value(1).toInt()));
            }
        }

        strokes.append(stroke);
    }

    return strokes;
}

int DatabaseManager::saveStroke(int userId, const QColor &color, int width, const QVector<QPoint> &points)
{
    if (points.size() < 2) {
        return -1;
    }

    if (!db.transaction()) {
        qWarning() << "No se pudo iniciar transaccion:" << db.lastError().text();
        return -1;
    }

    QSqlQuery strokeQuery(db);
    strokeQuery.prepare("INSERT INTO strokes (user_id, color, width, created_at) VALUES (?, ?, ?, ?)");
    strokeQuery.addBindValue(userId);
    strokeQuery.addBindValue(color.name());
    strokeQuery.addBindValue(width);
    strokeQuery.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!strokeQuery.exec()) {
        qWarning() << "Error guardando trazo:" << strokeQuery.lastError().text();
        db.rollback();
        return -1;
    }

    const int strokeId = strokeQuery.lastInsertId().toInt();
    QSqlQuery pointQuery(db);
    pointQuery.prepare("INSERT INTO stroke_points (stroke_id, order_index, x, y) VALUES (?, ?, ?, ?)");

    for (int i = 0; i < points.size(); ++i) {
        pointQuery.bindValue(0, strokeId);
        pointQuery.bindValue(1, i);
        pointQuery.bindValue(2, points.at(i).x());
        pointQuery.bindValue(3, points.at(i).y());
        if (!pointQuery.exec()) {
            qWarning() << "Error guardando punto:" << pointQuery.lastError().text();
            db.rollback();
            return -1;
        }
    }

    if (!db.commit()) {
        qWarning() << "No se pudo confirmar trazo:" << db.lastError().text();
        return -1;
    }

    return strokeId;
}

bool DatabaseManager::deleteStroke(int strokeId)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM strokes WHERE id = ?");
    query.addBindValue(strokeId);
    if (!query.exec()) {
        qWarning() << "Error borrando trazo:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::clearStrokes(int userId)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM strokes WHERE user_id = ?");
    query.addBindValue(userId);
    if (!query.exec()) {
        qWarning() << "Error borrando lienzo:" << query.lastError().text();
        return false;
    }
    return true;
}

QString DatabaseManager::passwordHash(const QString &password)
{
    const QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString::fromLatin1(hash.toHex());
}
