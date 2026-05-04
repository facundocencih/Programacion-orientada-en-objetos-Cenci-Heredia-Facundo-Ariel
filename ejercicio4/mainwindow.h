#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "apiclient.h"
#include "kanbanmodels.h"

#include <QMainWindow>

class QLabel;
class QLineEdit;
class QHBoxLayout;
class QPushButton;
class QScrollArea;
class QStackedWidget;
class QTimer;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void attemptLogin();
    void renderBoard(const BoardData &board);
    void showStatusMessage(const QString &message);
    void showErrorMessage(const QString &message);
    void createColumn();
    void editColumn(int columnId);
    void deleteColumn(int columnId);
    void createCard(int columnId);
    void editCard(int cardId);
    void deleteCard(int cardId);
    void moveCardLeft(int cardId);
    void moveCardRight(int cardId);
    void moveCardUp(int cardId);
    void moveCardDown(int cardId);

private:
    void buildUi();
    QWidget *buildLoginPage();
    QWidget *buildBoardPage();
    void applyStyles();
    const ColumnData *findColumn(int columnId) const;
    const CardData *findCard(int cardId, const ColumnData **column = nullptr) const;
    void moveCardAcrossColumns(int cardId, int direction);
    void moveCardInsideColumn(int cardId, int direction);

    ApiClient *m_api = nullptr;
    QStackedWidget *m_stack = nullptr;
    QWidget *m_boardContainer = nullptr;
    QHBoxLayout *m_boardLayout = nullptr;
    QLineEdit *m_userEdit = nullptr;
    QLineEdit *m_passwordEdit = nullptr;
    QLabel *m_loginMessage = nullptr;
    QLabel *m_headerStatus = nullptr;
    QLabel *m_boardTitle = nullptr;
    QPushButton *m_loginButton = nullptr;
    QTimer *m_pollTimer = nullptr;
    BoardData m_board;
};

#endif
