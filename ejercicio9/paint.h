#ifndef PAINT_H
#define PAINT_H

#include <QWidget>

class DatabaseManager;
class QLabel;
class Pintura;
class QFrame;

QT_BEGIN_NAMESPACE
namespace Ui {
class Paint;
}
QT_END_NAMESPACE

class Paint : public QWidget
{
    Q_OBJECT

public:
    explicit Paint(DatabaseManager *database, int userId, QWidget *parent = nullptr);
    ~Paint() override;

private:
    void buildTopPanel(DatabaseManager *database, int userId);
    void updateBrushInfo(const QColor &color, int width, const QString &colorName);
    QFrame *createColorCard(const QString &key, const QString &name, const QColor &color);
    void markActiveColor(const QColor &color);

    Ui::Paint *ui;
    Pintura *canvas = nullptr;
    QLabel *brushLabel = nullptr;
    QLabel *selectedColorLabel = nullptr;
    QFrame *brushPreview = nullptr;
    QFrame *redCard = nullptr;
    QFrame *greenCard = nullptr;
    QFrame *blueCard = nullptr;
};
#endif // PAINT_H
