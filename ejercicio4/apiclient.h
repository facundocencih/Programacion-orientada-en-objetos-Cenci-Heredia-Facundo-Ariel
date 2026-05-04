#ifndef APICLIENT_H
#define APICLIENT_H

#include "kanbanmodels.h"

#include <QObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QUrl>

#include <functional>

class QJsonArray;
class QJsonObject;
class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;

class ApiClient : public QObject
{
    Q_OBJECT

public:
    explicit ApiClient(QObject *parent = nullptr);

    void setCredentials(const QString &username, const QString &password);
    bool hasCredentials() const;
    QString baseUrl() const;

public slots:
    void validateCredentials();
    void fetchBoard();
    void createColumn(const QString &title);
    void updateColumn(int columnId, const QString &title);
    void deleteColumn(int columnId);
    void createCard(int columnId, const QString &title, const QString &description);
    void updateCard(int cardId, int columnId, const QString &title, const QString &description);
    void deleteCard(int cardId);
    void moveCard(int cardId, int sourceColumnId, int targetColumnId, int targetPosition);
    void reorderColumn(int columnId, const QVector<int> &cardIds);

signals:
    void loginSucceeded();
    void loginFailed(const QString &message);
    void boardReady(const BoardData &board);
    void operationSucceeded(const QString &message);
    void operationFailed(const QString &message);
    void statusChanged(const QString &message);

private:
    using JsonHandler = std::function<void(const QJsonDocument &)>;

    void sendJsonRequest(const QString &path,
                         const QString &method,
                         const QJsonDocument &payload,
                         const JsonHandler &onSuccess,
                         const std::function<void(QNetworkReply *, const QByteArray &)> &onFailure = {});
    QNetworkRequest buildRequest(const QString &path) const;
    QString authorizationHeader() const;
    void fetchColumns();
    void fetchCardsForColumns(QVector<ColumnData> columns);
    void handleBoardPayload(const QJsonDocument &document);
    QVector<ColumnData> parseColumns(const QJsonValue &value) const;
    QVector<CardData> parseCards(const QJsonValue &value, int fallbackColumnId = -1) const;
    ColumnData parseColumn(const QJsonObject &object) const;
    CardData parseCard(const QJsonObject &object, int fallbackColumnId = -1) const;
    QString readString(const QJsonObject &object, std::initializer_list<const char *> keys) const;
    int readInt(const QJsonObject &object, std::initializer_list<const char *> keys, int defaultValue = -1) const;
    void sortBoard(BoardData &board) const;
    void sortColumn(ColumnData &column) const;

    QNetworkAccessManager *m_network = nullptr;
    QUrl m_baseUrl;
    QString m_username;
    QString m_password;
};

#endif
