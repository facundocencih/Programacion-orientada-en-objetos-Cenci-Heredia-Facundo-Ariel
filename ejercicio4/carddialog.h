#ifndef CARDDIALOG_H
#define CARDDIALOG_H

#include <QDialog>

class QLineEdit;
class QTextEdit;

class CardDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CardDialog(QWidget *parent = nullptr);

    void setValues(const QString &title, const QString &description);
    QString title() const;
    QString description() const;

private:
    QLineEdit *m_titleEdit = nullptr;
    QTextEdit *m_descriptionEdit = nullptr;
};

#endif
