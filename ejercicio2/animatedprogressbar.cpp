#include "animatedprogressbar.h"

#include <QEasingCurve>
#include <QPropertyAnimation>

AnimatedProgressBar::AnimatedProgressBar(QWidget *parent)
    : QProgressBar(parent)
{
    m_animation = new QPropertyAnimation(this, "value", this);
    m_animation->setDuration(650);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);
}

void AnimatedProgressBar::animateToValue(int value)
{
    m_animation->stop();
    m_animation->setStartValue(this->value());
    m_animation->setEndValue(value);
    m_animation->start();
}
