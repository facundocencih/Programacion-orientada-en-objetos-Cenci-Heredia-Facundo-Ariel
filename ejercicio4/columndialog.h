#ifndef COLUMNDIALOG_H
#define COLUMNDIALOG_H

#include <QDialog>

class QLineEdit;

class ColumnDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColumnDialog(QWidget *parent = nullptr);

    void setTitleText(const QString &title);
    QString titleText() const;

private:
    QLineEdit *m_titleEdit = nullptr;
};

#endif
