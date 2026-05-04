#include "mainwindow.h"

#include "carddialog.h"
#include "columndialog.h"
#include "columnwidget.h"

#include <QApplication>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_api(new ApiClient(this))
    , m_stack(new QStackedWidget(this))
    , m_boardContainer(new QWidget(this))
    , m_boardLayout(new QHBoxLayout())
    , m_pollTimer(new QTimer(this))
{
    buildUi();
    applyStyles();

    setWindowTitle(QStringLiteral("Kanban Colaborativo"));
    resize(1480, 860);

    m_pollTimer->setInterval(4000);

    connect(m_pollTimer, &QTimer::timeout, m_api, &ApiClient::fetchBoard);
    connect(m_api, &ApiClient::loginSucceeded, this, [this]() {
        m_stack->setCurrentIndex(1);
        m_pollTimer->start();
        m_api->fetchBoard();
    });
    connect(m_api, &ApiClient::loginFailed, this, [this](const QString &message) {
        m_loginMessage->setText(message);
        m_loginButton->setEnabled(true);
    });
    connect(m_api, &ApiClient::boardReady, this, &MainWindow::renderBoard);
    connect(m_api, &ApiClient::statusChanged, this, &MainWindow::showStatusMessage);
    connect(m_api, &ApiClient::operationSucceeded, this, &MainWindow::showStatusMessage);
    connect(m_api, &ApiClient::operationFailed, this, &MainWindow::showErrorMessage);
}

void MainWindow::attemptLogin()
{
    const QString username = m_userEdit->text().trimmed();
    const QString password = m_passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        m_loginMessage->setText(QStringLiteral("Completa usuario y clave para continuar"));
        return;
    }

    m_loginButton->setEnabled(false);
    m_loginMessage->setText(QStringLiteral("Conectando con el VPS en el puerto 5003..."));
    m_api->setCredentials(username, password);
    m_api->validateCredentials();
}

void MainWindow::renderBoard(const BoardData &board)
{
    m_board = board;
    m_boardTitle->setText(QStringLiteral("Tablero activo - %1 columnas").arg(board.columns.size()));

    QLayoutItem *item = nullptr;
    while ((item = m_boardLayout->takeAt(0)) != nullptr) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    for (const ColumnData &column : board.columns) {
        auto *columnWidget = new ColumnWidget(column, m_boardContainer);
        connect(columnWidget, &ColumnWidget::addCardRequested, this, &MainWindow::createCard);
        connect(columnWidget, &ColumnWidget::editColumnRequested, this, &MainWindow::editColumn);
        connect(columnWidget, &ColumnWidget::deleteColumnRequested, this, &MainWindow::deleteColumn);
        connect(columnWidget, &ColumnWidget::editCardRequested, this, &MainWindow::editCard);
        connect(columnWidget, &ColumnWidget::deleteCardRequested, this, &MainWindow::deleteCard);
        connect(columnWidget, &ColumnWidget::moveCardLeftRequested, this, &MainWindow::moveCardLeft);
        connect(columnWidget, &ColumnWidget::moveCardRightRequested, this, &MainWindow::moveCardRight);
        connect(columnWidget, &ColumnWidget::moveCardUpRequested, this, &MainWindow::moveCardUp);
        connect(columnWidget, &ColumnWidget::moveCardDownRequested, this, &MainWindow::moveCardDown);
        m_boardLayout->addWidget(columnWidget);
    }

    m_boardLayout->addStretch();
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_headerStatus->setText(message);
    statusBar()->showMessage(message, 4000);
    m_loginButton->setEnabled(true);
}

void MainWindow::showErrorMessage(const QString &message)
{
    m_headerStatus->setText(message);
    statusBar()->showMessage(message, 6000);
    QMessageBox::warning(this, QStringLiteral("Kanban"), message);
}

void MainWindow::createColumn()
{
    ColumnDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("Nueva columna"));
    if (dialog.exec() != QDialog::Accepted || dialog.titleText().isEmpty()) {
        return;
    }

    m_api->createColumn(dialog.titleText());
}

void MainWindow::editColumn(int columnId)
{
    const ColumnData *column = findColumn(columnId);
    if (!column) {
        return;
    }

    ColumnDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("Editar columna"));
    dialog.setTitleText(column->title);
    if (dialog.exec() != QDialog::Accepted || dialog.titleText().isEmpty()) {
        return;
    }

    m_api->updateColumn(columnId, dialog.titleText());
}

void MainWindow::deleteColumn(int columnId)
{
    const ColumnData *column = findColumn(columnId);
    if (!column) {
        return;
    }

    if (QMessageBox::question(this,
                              QStringLiteral("Eliminar columna"),
                              QStringLiteral("Se eliminara la columna \"%1\" y sus tarjetas asociadas.").arg(column->title))
        != QMessageBox::Yes) {
        return;
    }

    m_api->deleteColumn(columnId);
}

void MainWindow::createCard(int columnId)
{
    CardDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("Nueva tarjeta"));
    if (dialog.exec() != QDialog::Accepted || dialog.title().isEmpty()) {
        return;
    }

    m_api->createCard(columnId, dialog.title(), dialog.description());
}

void MainWindow::editCard(int cardId)
{
    const CardData *card = findCard(cardId);
    if (!card) {
        return;
    }

    CardDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("Editar tarjeta"));
    dialog.setValues(card->title, card->description);
    if (dialog.exec() != QDialog::Accepted || dialog.title().isEmpty()) {
        return;
    }

    m_api->updateCard(card->id, card->columnId, dialog.title(), dialog.description());
}

void MainWindow::deleteCard(int cardId)
{
    const CardData *card = findCard(cardId);
    if (!card) {
        return;
    }

    if (QMessageBox::question(this,
                              QStringLiteral("Eliminar tarjeta"),
                              QStringLiteral("Se eliminara la tarjeta \"%1\".").arg(card->title))
        != QMessageBox::Yes) {
        return;
    }

    m_api->deleteCard(cardId);
}

void MainWindow::moveCardLeft(int cardId)
{
    moveCardAcrossColumns(cardId, -1);
}

void MainWindow::moveCardRight(int cardId)
{
    moveCardAcrossColumns(cardId, 1);
}

void MainWindow::moveCardUp(int cardId)
{
    moveCardInsideColumn(cardId, -1);
}

void MainWindow::moveCardDown(int cardId)
{
    moveCardInsideColumn(cardId, 1);
}

void MainWindow::buildUi()
{
    setCentralWidget(m_stack);
    m_stack->addWidget(buildLoginPage());
    m_stack->addWidget(buildBoardPage());
    m_stack->setCurrentIndex(0);
}

QWidget *MainWindow::buildLoginPage()
{
    auto *page = new QWidget(this);
    page->setObjectName(QStringLiteral("LoginPage"));

    auto *outerLayout = new QVBoxLayout(page);
    outerLayout->setContentsMargins(72, 48, 72, 48);

    auto *heroRow = new QHBoxLayout();
    heroRow->setSpacing(28);

    auto *leftPanel = new QFrame(page);
    leftPanel->setObjectName(QStringLiteral("LoginHero"));
    auto *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(32, 32, 32, 32);
    leftLayout->setSpacing(16);

    auto *heroBadge = new QLabel(QStringLiteral("KANBAN COLABORATIVO"), leftPanel);
    heroBadge->setObjectName(QStringLiteral("HeroBadge"));
    auto *heroTitle = new QLabel(QStringLiteral("Organiza tareas con una vista inspirada en Trello"), leftPanel);
    heroTitle->setWordWrap(true);
    heroTitle->setObjectName(QStringLiteral("HeroTitle"));
    auto *heroBody = new QLabel(QStringLiteral("Columnas horizontales, tarjetas limpias, cambios persistentes y sincronizacion automatica cada 4 segundos contra el VPS."), leftPanel);
    heroBody->setWordWrap(true);
    heroBody->setObjectName(QStringLiteral("HeroBody"));

    leftLayout->addWidget(heroBadge);
    leftLayout->addWidget(heroTitle);
    leftLayout->addWidget(heroBody);
    leftLayout->addStretch();

    auto *rightPanel = new QFrame(page);
    rightPanel->setObjectName(QStringLiteral("LoginCard"));
    auto *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(32, 32, 32, 32);
    rightLayout->setSpacing(14);

    auto *title = new QLabel(QStringLiteral("Entrar al tablero"), rightPanel);
    title->setObjectName(QStringLiteral("LoginTitle"));
    auto *subtitle = new QLabel(QStringLiteral("Backend FastAPI en http://173.212.209.61:5003"), rightPanel);
    subtitle->setObjectName(QStringLiteral("LoginSubtitle"));

    m_userEdit = new QLineEdit(rightPanel);
    m_passwordEdit = new QLineEdit(rightPanel);
    m_userEdit->setPlaceholderText(QStringLiteral("Usuario"));
    m_passwordEdit->setPlaceholderText(QStringLiteral("Clave"));
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    m_loginButton = new QPushButton(QStringLiteral("Conectar"), rightPanel);
    m_loginButton->setCursor(Qt::PointingHandCursor);
    m_loginButton->setObjectName(QStringLiteral("PrimaryButton"));

    m_loginMessage = new QLabel(QStringLiteral("Usa las credenciales del VPS para acceder al tablero."), rightPanel);
    m_loginMessage->setWordWrap(true);
    m_loginMessage->setObjectName(QStringLiteral("LoginMessage"));

    rightLayout->addWidget(title);
    rightLayout->addWidget(subtitle);
    rightLayout->addSpacing(8);
    rightLayout->addWidget(m_userEdit);
    rightLayout->addWidget(m_passwordEdit);
    rightLayout->addWidget(m_loginButton);
    rightLayout->addWidget(m_loginMessage);
    rightLayout->addStretch();

    heroRow->addWidget(leftPanel, 3);
    heroRow->addWidget(rightPanel, 2);
    outerLayout->addStretch();
    outerLayout->addLayout(heroRow);
    outerLayout->addStretch();

    connect(m_loginButton, &QPushButton::clicked, this, &MainWindow::attemptLogin);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &MainWindow::attemptLogin);

    return page;
}

QWidget *MainWindow::buildBoardPage()
{
    auto *page = new QWidget(this);
    page->setObjectName(QStringLiteral("BoardPage"));

    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 20, 24, 24);
    layout->setSpacing(18);

    auto *topBar = new QFrame(page);
    topBar->setObjectName(QStringLiteral("TopBar"));
    auto *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(18, 18, 18, 18);

    auto *titleColumn = new QVBoxLayout();
    auto *title = new QLabel(QStringLiteral("Mi tablero Kanban"), topBar);
    title->setObjectName(QStringLiteral("BoardHeaderTitle"));
    m_boardTitle = new QLabel(QStringLiteral("Esperando sincronizacion"), topBar);
    m_boardTitle->setObjectName(QStringLiteral("BoardHeaderSubtitle"));
    titleColumn->addWidget(title);
    titleColumn->addWidget(m_boardTitle);

    m_headerStatus = new QLabel(QStringLiteral("Listo para conectar"), topBar);
    m_headerStatus->setObjectName(QStringLiteral("StatusPill"));

    auto *refreshButton = new QPushButton(QStringLiteral("Actualizar"), topBar);
    refreshButton->setObjectName(QStringLiteral("GhostButton"));
    refreshButton->setCursor(Qt::PointingHandCursor);
    connect(refreshButton, &QPushButton::clicked, m_api, &ApiClient::fetchBoard);

    auto *addColumnButton = new QPushButton(QStringLiteral("+ Nueva columna"), topBar);
    addColumnButton->setObjectName(QStringLiteral("PrimaryButton"));
    addColumnButton->setCursor(Qt::PointingHandCursor);
    connect(addColumnButton, &QPushButton::clicked, this, &MainWindow::createColumn);

    topLayout->addLayout(titleColumn);
    topLayout->addStretch();
    topLayout->addWidget(m_headerStatus);
    topLayout->addWidget(refreshButton);
    topLayout->addWidget(addColumnButton);
    layout->addWidget(topBar);

    auto *scrollArea = new QScrollArea(page);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setFrameShape(QFrame::NoFrame);

    m_boardContainer->setObjectName(QStringLiteral("BoardCanvas"));
    m_boardLayout->setContentsMargins(0, 0, 0, 0);
    m_boardLayout->setSpacing(16);
    m_boardContainer->setLayout(m_boardLayout);
    scrollArea->setWidget(m_boardContainer);

    layout->addWidget(scrollArea, 1);
    return page;
}

void MainWindow::applyStyles()
{
    qApp->setStyleSheet(QStringLiteral(R"(
        QWidget {
            font-family: "Segoe UI";
            color: #102a43;
        }
        QMainWindow, #BoardPage, #LoginPage {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #0f4c81, stop:0.5 #1768ac, stop:1 #2b90d9);
        }
        #LoginHero, #LoginCard, #TopBar, #ColumnWidget, #CardWidget {
            border-radius: 18px;
        }
        #LoginHero {
            background: rgba(7, 38, 76, 0.74);
            border: 1px solid rgba(255, 255, 255, 0.16);
        }
        #HeroBadge {
            color: #d9eeff;
            font-size: 13px;
            font-weight: 700;
            letter-spacing: 2px;
        }
        #HeroTitle {
            color: white;
            font-size: 34px;
            font-weight: 800;
        }
        #HeroBody {
            color: #d6e8ff;
            font-size: 16px;
            line-height: 1.5;
        }
        #LoginCard {
            background: #f7fbff;
        }
        #LoginTitle {
            font-size: 28px;
            font-weight: 800;
        }
        #LoginSubtitle, #LoginMessage, #BoardHeaderSubtitle {
            color: #486581;
        }
        QLineEdit, QTextEdit {
            background: white;
            border: 1px solid #cbd2d9;
            border-radius: 12px;
            padding: 12px;
            font-size: 14px;
        }
        #PrimaryButton {
            background: #ffb703;
            color: #143b5d;
            border: none;
            border-radius: 12px;
            padding: 12px 18px;
            font-size: 14px;
            font-weight: 700;
        }
        #PrimaryButton:hover {
            background: #ffc533;
        }
        #GhostButton, #DangerGhostButton, #AddCardButton {
            background: rgba(255, 255, 255, 0.75);
            border: 1px solid #d9e2ec;
            border-radius: 10px;
            padding: 8px 12px;
            font-weight: 600;
        }
        #DangerGhostButton {
            color: #9b2226;
        }
        #AddCardButton {
            background: rgba(43, 144, 217, 0.12);
            color: #1768ac;
            text-align: left;
        }
        #TopBar {
            background: rgba(239, 248, 255, 0.92);
            border: 1px solid rgba(255, 255, 255, 0.4);
        }
        #BoardHeaderTitle {
            font-size: 26px;
            font-weight: 800;
            color: #102a43;
        }
        #StatusPill {
            background: #d9eeff;
            color: #0b3c5d;
            border-radius: 14px;
            padding: 8px 14px;
            font-weight: 700;
        }
        #BoardCanvas {
            background: transparent;
        }
        #ColumnWidget {
            background: #dcebf7;
            border: 1px solid rgba(255, 255, 255, 0.55);
        }
        #ColumnTitle {
            font-size: 18px;
            font-weight: 800;
        }
        #CountBadge {
            background: rgba(16, 42, 67, 0.12);
            border-radius: 10px;
            padding: 4px 10px;
            font-weight: 700;
        }
        #CardWidget {
            background: white;
            border: 1px solid #e4ecf3;
        }
        #CardTitle {
            font-size: 16px;
            font-weight: 700;
            color: #102a43;
        }
        #CardDescription {
            color: #486581;
            line-height: 1.4;
        }
        QScrollArea {
            background: transparent;
        }
        QStatusBar {
            background: rgba(255, 255, 255, 0.92);
        }
    )"));
}

const ColumnData *MainWindow::findColumn(int columnId) const
{
    for (const ColumnData &column : m_board.columns) {
        if (column.id == columnId) {
            return &column;
        }
    }
    return nullptr;
}

const CardData *MainWindow::findCard(int cardId, const ColumnData **column) const
{
    for (const ColumnData &currentColumn : m_board.columns) {
        for (const CardData &card : currentColumn.cards) {
            if (card.id == cardId) {
                if (column) {
                    *column = &currentColumn;
                }
                return &card;
            }
        }
    }
    return nullptr;
}

void MainWindow::moveCardAcrossColumns(int cardId, int direction)
{
    const ColumnData *column = nullptr;
    const CardData *card = findCard(cardId, &column);
    if (!card || !column) {
        return;
    }

    int currentColumnIndex = -1;
    for (int index = 0; index < m_board.columns.size(); ++index) {
        if (m_board.columns.at(index).id == column->id) {
            currentColumnIndex = index;
            break;
        }
    }

    const int targetIndex = currentColumnIndex + direction;
    if (currentColumnIndex < 0 || targetIndex < 0 || targetIndex >= m_board.columns.size()) {
        return;
    }

    const ColumnData &targetColumn = m_board.columns.at(targetIndex);
    m_api->moveCard(card->id, column->id, targetColumn.id, targetColumn.cards.size());
}

void MainWindow::moveCardInsideColumn(int cardId, int direction)
{
    const ColumnData *column = nullptr;
    const CardData *card = findCard(cardId, &column);
    if (!card || !column) {
        return;
    }

    QVector<int> orderedIds;
    orderedIds.reserve(column->cards.size());
    for (const CardData &currentCard : column->cards) {
        orderedIds.append(currentCard.id);
    }

    int currentIndex = orderedIds.indexOf(cardId);
    const int targetIndex = currentIndex + direction;
    if (currentIndex < 0 || targetIndex < 0 || targetIndex >= orderedIds.size()) {
        return;
    }

    orderedIds.move(currentIndex, targetIndex);
    m_api->reorderColumn(column->id, orderedIds);
}
