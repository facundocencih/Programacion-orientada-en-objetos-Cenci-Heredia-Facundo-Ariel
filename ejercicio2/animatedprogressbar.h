#ifndef ANIMATEDPROGRESSBAR_H
#define ANIMATEDPROGRESSBAR_H

#include <QProgressBar>

class QPropertyAnimation;

class AnimatedProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    explicit AnimatedProgressBar(QWidget *parent = nullptr);

    void animateToValue(int value);

private:
    QPropertyAnimation *m_animation = nullptr;
};

#endif
