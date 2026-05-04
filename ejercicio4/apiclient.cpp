#include "apiclient.h"

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <algorithm>

ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager(this))
    , m_baseUrl(QStringLiteral("http://173.212.209.61:5003"))
{
}

void ApiClient::setCredentials(const QString &username, const QString &password)
{
    m_username = username.trimmed();
    m_password = password;
}

bool ApiClient::hasCredentials() const
{
    return !m_username.isEmpty() && !m_password.isEmpty();
}

QString ApiClient::baseUrl() const
{
    return m_baseUrl.toString();
}

void ApiClient::validateCredentials()
{
    emit statusChanged(QStringLiteral("Validando acceso al tablero..."));
    sendJsonRequest(QStringLiteral("/columns"), QStringLiteral("GET"), QJsonDocument(),
                    [this](const QJsonDocument &) {
                        emit loginSucceeded();
                        emit statusChanged(QStringLiteral("Sesion iniciada"));
                    },
                    [this](QNetworkReply *reply, const QByteArray &) {
                        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                        if (statusCode == 401 || statusCode == 403) {
                            emit loginFailed(QStringLiteral("Usuario o clave invalidos"));
                        } else {
                            emit loginFailed(QStringLiteral("No se pudo conectar al VPS en el puerto 5003"));
                        }
                    });
}

void ApiClient::fetchBoard()
{
    emit statusChanged(QStringLiteral("Actualizando tablero..."));
    sendJsonRequest(QStringLiteral("/board"), QStringLiteral("GET"), QJsonDocument(),
                    [this](const QJsonDocument &document) {
                        handleBoardPayload(document);
                    },
                    [this](QNetworkReply *reply, const QByteArray &) {
                        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                        if (statusCode == 404) {
                            fetchColumns();
                            return;
                        }
                        emit operationFailed(QStringLiteral("No se pudo actualizar el tablero"));
                    });
}

void ApiClient::createColumn(const QString &title)
{
    QJsonObject payload;
    payload["title"] = title;
    payload["name"] = title;

    sendJsonRequest(QStringLiteral("/columns"), QStringLiteral("POST"), QJsonDocument(payload),
                    [this](const QJsonDocument &) {
                        emit operationSucceeded(QStringLiteral("Columna creada"));
                        fetchBoard();
                    });
}

void ApiClient::updateColumn(int columnId, const QString &title)
{
    QJsonObject payload;
    payload["title"] = title;
    payload["name"] = title;

    sendJsonRequest(QStringLiteral("/columns/%1").arg(columnId), QStringLiteral("PUT"), QJsonDocument(payload),
                    [this](const QJsonDocument &) {
                        emit operationSucceeded(QStringLiteral("Columna actualizada"));
                        fetchBoard();
                    });
}

void ApiClient::deleteColumn(int columnId)
{
    sendJsonRequest(QStringLiteral("/columns/%1").arg(columnId), QStringLiteral("DELETE"), QJsonDocument(),
                    [this](const QJsonDocument &) {
                        emit operationSucceeded(QStringLiteral("Columna eliminada"));
                        fetchBoard();
                    });
}

void ApiClient::createCard(int columnId, const QString &title, const QString &description)
{
    QJsonObject payload;
    payload["column_id"] = columnId;
    payload["title"] = title;
    payload["name"] = title;
    payload["description"] = description;
    payload["details"] = description;

    sendJsonRequest(QStringLiteral("/cards"), QStringLiteral("POST"), QJsonDocument(payload),
                    [this](const QJsonDocument &) {
                        emit operationSucceeded(QStringLiteral("Tarjeta creada"));
                        fetchBoard();
                    });
}

void ApiClient::updateCard(int cardId, int columnId, const QString &title, const QString &description)
{
    QJsonObject payload;
    payload["column_id"] = columnId;
    payload["title"] = title;
    payload["name"] = title;
    payload["description"] = description;
    payload["details"] = description;

    sendJsonRequest(QStringLiteral("/cards/%1").arg(cardId), QStringLiteral("PUT"), QJsonDocument(payload),
                    [this](const QJsonDocument &) {
                        emit operationSucceeded(QStringLiteral("Tarjeta actualizada"));
                        fetchBoard();
                    });
}

void ApiClient::deleteCard(int cardId)
{
    sendJsonRequest(QStringLiteral("/cards/%1").arg(cardId), QStringLiteral("DELETE"), QJsonDocument(),
                    [this](const QJsonDocument &) {
                        emit operationSucceeded(QStringLiteral("Tarjeta eliminada"));
                        fetchBoard();
                    });
}

void ApiClient::moveCard(int cardId, int sourceColumnId, int targetColumnId, int targetPosition)
{
    QJsonObject payload;
    payload["card_id"] = cardId;
    payload["source_column_id"] = sourceColumnId;
    payload["column_id"] = targetColumnId;
    payload["target_column_id"] = targetColumnId;
    payload["position"] = targetPosition;
    payload["new_position"] = targetPosition;

    sendJsonRequest(QStringLiteral("/cards/%1/move").arg(cardId), QStringLiteral("POST"), QJsonDocument(payload),
                    [this](const QJsonDocument &) {
                        emit operationSucceeded(QStringLiteral("Tarjeta movida"));
                        fetchBoard();
                    },
                    [this, payload](QNetworkReply *reply, const QByteArray &) {
                        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                        if (statusCode == 404) {
                            sendJsonRequest(QStringLiteral("/cards/move"), QStringLiteral("POST"), QJsonDocument(payload),
                                            [this](const QJsonDocument &) {
                                                emit operationSucceeded(QStringLiteral("Tarjeta movida"));
                                                fetchBoard();
                                            });
                            return;
                        }
                        emit operationFailed(QStringLiteral("No se pudo mover la tarjeta"));
                    });
}

void ApiClient::reorderColumn(int columnId, const QVector<int> &cardIds)
{
    QJsonArray cardIdsArray;
    QJsonArray orderArray;
    for (int index = 0; index < cardIds.size(); ++index) {
        cardIdsArray.append(cardIds.at(index));

        QJsonObject item;
        item["card_id"] = cardIds.at(index);
        item["position"] = index;
        orderArray.append(item);
    }

    QJsonObject payload;
    payload["column_id"] = columnId;
    payload["card_ids"] = cardIdsArray;
    payload["order"] = orderArray;

    sendJsonRequest(QStringLiteral("/columns/%1/reorder").arg(columnId), QStringLiteral("POST"), QJsonDocument(payload),
                    [this](const QJsonDocument &) {
                        emit operationSucceeded(QStringLiteral("Orden actualizado"));
                        fetchBoard();
                    },
                    [this, payload](QNetworkReply *reply, const QByteArray &) {
                        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                        if (statusCode == 404) {
                            sendJsonRequest(QStringLiteral("/cards/reorder"), QStringLiteral("POST"), QJsonDocument(payload),
                                            [this](const QJsonDocument &) {
                                                emit operationSucceeded(QStringLiteral("Orden actualizado"));
                                                fetchBoard();
                                            });
                            return;
                        }
                        emit operationFailed(QStringLiteral("No se pudo reordenar la columna"));
                    });
}

void ApiClient::sendJsonRequest(const QString &path,
                                const QString &method,
                                const QJsonDocument &payload,
                                const JsonHandler &onSuccess,
                                const std::function<void(QNetworkReply *, const QByteArray &)> &onFailure)
{
    QNetworkRequest request = buildRequest(path);
    QNetworkReply *reply = nullptr;

    const QByteArray body = payload.isNull() ? QByteArray() : payload.toJson(QJsonDocument::Compact);

    if (method == QStringLiteral("GET")) {
        reply = m_network->get(request);
    } else if (method == QStringLiteral("POST")) {
        reply = m_network->post(request, body);
    } else if (method == QStringLiteral("PUT")) {
        reply = m_network->put(request, body);
    } else if (method == QStringLiteral("DELETE")) {
        reply = m_network->sendCustomRequest(request, "DELETE", body);
    } else {
        emit operationFailed(QStringLiteral("Metodo HTTP no soportado"));
        return;
    }

    connect(reply, &QNetworkReply::finished, this, [this, reply, onSuccess, onFailure]() {
        const QByteArray data = reply->readAll();
        const bool ok = reply->error() == QNetworkReply::NoError;

        if (ok) {
            const QJsonDocument document = data.isEmpty() ? QJsonDocument(QJsonObject()) : QJsonDocument::fromJson(data);
            if (onSuccess) {
                onSuccess(document);
            }
        } else if (onFailure) {
            onFailure(reply, data);
        } else {
            emit operationFailed(QStringLiteral("Error de red: %1").arg(reply->errorString()));
        }

        reply->deleteLater();
    });
}

QNetworkRequest ApiClient::buildRequest(const QString &path) const
{
    QNetworkRequest request(m_baseUrl.resolved(QUrl(path)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setRawHeader("Accept", "application/json");

    if (hasCredentials()) {
        request.setRawHeader("Authorization", authorizationHeader().toUtf8());
    }

    return request;
}

QString ApiClient::authorizationHeader() const
{
    const QByteArray credentials = QStringLiteral("%1:%2").arg(m_username, m_password).toUtf8().toBase64();
    return QStringLiteral("Basic %1").arg(QString::fromUtf8(credentials));
}

void ApiClient::fetchColumns()
{
    sendJsonRequest(QStringLiteral("/columns"), QStringLiteral("GET"), QJsonDocument(),
                    [this](const QJsonDocument &document) {
                        QVector<ColumnData> columns;
                        if (document.isArray()) {
                            columns = parseColumns(document.array());
                        } else if (document.isObject()) {
                            const QJsonObject object = document.object();
                            if (object.contains("columns")) {
                                columns = parseColumns(object.value("columns"));
                            } else {
                                columns = parseColumns(object);
                            }
                        }

                        bool hasEmbeddedCards = false;
                        for (const ColumnData &column : std::as_const(columns)) {
                            if (!column.cards.isEmpty()) {
                                hasEmbeddedCards = true;
                                break;
                            }
                        }

                        if (hasEmbeddedCards) {
                            BoardData board;
                            board.columns = columns;
                            sortBoard(board);
                            emit boardReady(board);
                            emit statusChanged(QStringLiteral("Tablero sincronizado"));
                        } else {
                            fetchCardsForColumns(columns);
                        }
                    });
}

void ApiClient::fetchCardsForColumns(QVector<ColumnData> columns)
{
    sendJsonRequest(QStringLiteral("/cards"), QStringLiteral("GET"), QJsonDocument(),
                    [this, columns](const QJsonDocument &document) mutable {
                        QVector<CardData> cards;
                        if (document.isArray()) {
                            cards = parseCards(document.array());
                        } else if (document.isObject()) {
                            const QJsonObject object = document.object();
                            if (object.contains("cards")) {
                                cards = parseCards(object.value("cards"));
                            } else {
                                cards = parseCards(object);
                            }
                        }

                        for (const CardData &card : std::as_const(cards)) {
                            auto it = std::find_if(columns.begin(), columns.end(), [card](const ColumnData &column) {
                                return column.id == card.columnId;
                            });
                            if (it != columns.end()) {
                                it->cards.append(card);
                            }
                        }

                        BoardData board;
                        board.columns = columns;
                        sortBoard(board);
                        emit boardReady(board);
                        emit statusChanged(QStringLiteral("Tablero sincronizado"));
                    },
                    [this, columns](QNetworkReply *reply, const QByteArray &) {
                        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                        if (statusCode == 404) {
                            BoardData board;
                            board.columns = columns;
                            sortBoard(board);
                            emit boardReady(board);
                            emit statusChanged(QStringLiteral("Tablero sincronizado"));
                            return;
                        }
                        emit operationFailed(QStringLiteral("No se pudo cargar el listado de tarjetas"));
                    });
}

void ApiClient::handleBoardPayload(const QJsonDocument &document)
{
    BoardData board;
    if (document.isArray()) {
        board.columns = parseColumns(document.array());
    } else if (document.isObject()) {
        const QJsonObject object = document.object();
        if (object.contains("columns")) {
            board.columns = parseColumns(object.value("columns"));
        } else if (object.contains("board")) {
            board.columns = parseColumns(object.value("board"));
        } else {
            board.columns = parseColumns(object);
        }
    }

    sortBoard(board);
    emit boardReady(board);
    emit statusChanged(QStringLiteral("Tablero sincronizado"));
}

QVector<ColumnData> ApiClient::parseColumns(const QJsonValue &value) const
{
    QVector<ColumnData> columns;

    if (value.isArray()) {
        const QJsonArray array = value.toArray();
        columns.reserve(array.size());
        for (const QJsonValue &item : array) {
            if (item.isObject()) {
                columns.append(parseColumn(item.toObject()));
            }
        }
    } else if (value.isObject()) {
        columns.append(parseColumn(value.toObject()));
    }

    return columns;
}

QVector<CardData> ApiClient::parseCards(const QJsonValue &value, int fallbackColumnId) const
{
    QVector<CardData> cards;

    if (value.isArray()) {
        const QJsonArray array = value.toArray();
        cards.reserve(array.size());
        for (const QJsonValue &item : array) {
            if (item.isObject()) {
                cards.append(parseCard(item.toObject(), fallbackColumnId));
            }
        }
    } else if (value.isObject()) {
        cards.append(parseCard(value.toObject(), fallbackColumnId));
    }

    return cards;
}

ColumnData ApiClient::parseColumn(const QJsonObject &object) const
{
    ColumnData column;
    column.id = readInt(object, {"id", "column_id"});
    column.position = readInt(object, {"position", "order", "index"}, column.id);
    column.title = readString(object, {"title", "name"});
    column.cards = parseCards(object.value("cards"), column.id);
    return column;
}

CardData ApiClient::parseCard(const QJsonObject &object, int fallbackColumnId) const
{
    CardData card;
    card.id = readInt(object, {"id", "card_id"});
    card.columnId = readInt(object, {"column_id", "columnId"}, fallbackColumnId);
    card.position = readInt(object, {"position", "order", "index"}, card.id);
    card.title = readString(object, {"title", "name"});
    card.description = readString(object, {"description", "details", "content"});
    return card;
}

QString ApiClient::readString(const QJsonObject &object, std::initializer_list<const char *> keys) const
{
    for (const char *key : keys) {
        const QJsonValue value = object.value(QString::fromUtf8(key));
        if (value.isString()) {
            return value.toString();
        }
    }
    return {};
}

int ApiClient::readInt(const QJsonObject &object, std::initializer_list<const char *> keys, int defaultValue) const
{
    for (const char *key : keys) {
        const QJsonValue value = object.value(QString::fromUtf8(key));
        if (value.isDouble()) {
            return value.toInt();
        }
        if (value.isString()) {
            bool ok = false;
            const int parsed = value.toString().toInt(&ok);
            if (ok) {
                return parsed;
            }
        }
    }
    return defaultValue;
}

void ApiClient::sortBoard(BoardData &board) const
{
    std::sort(board.columns.begin(), board.columns.end(), [](const ColumnData &left, const ColumnData &right) {
        return left.position < right.position;
    });

    for (ColumnData &column : board.columns) {
        sortColumn(column);
    }
}

void ApiClient::sortColumn(ColumnData &column) const
{
    std::sort(column.cards.begin(), column.cards.end(), [](const CardData &left, const CardData &right) {
        return left.position < right.position;
    });
}
