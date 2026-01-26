#include "AnimationManager.h"

AnimationManager* AnimationManager::s_instance = nullptr;

AnimationManager::AnimationManager(QObject* parent)
    : QObject(parent)
{
}

AnimationManager::~AnimationManager()
{
}

AnimationManager* AnimationManager::instance()
{
    if (!s_instance) {
        s_instance = new AnimationManager();
    }
    return s_instance;
}

void AnimationManager::fadeIn(QWidget* widget, int duration)
{
    if (!widget) {
        return;
    }

    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(opacityEffect);

    QPropertyAnimation* animation = new QPropertyAnimation(opacityEffect, "opacity", widget);
    animation->setDuration(duration);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation, &QPropertyAnimation::finished, [opacityEffect]() {
        delete opacityEffect;
    });
}

void AnimationManager::fadeOut(QWidget* widget, int duration)
{
    if (!widget) {
        return;
    }

    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(opacityEffect);

    QPropertyAnimation* animation = new QPropertyAnimation(opacityEffect, "opacity", widget);
    animation->setDuration(duration);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    animation->setEasingCurve(QEasingCurve::InCubic);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation, &QPropertyAnimation::finished, [opacityEffect]() {
        delete opacityEffect;
    });
}

void AnimationManager::slideInFromRight(QWidget* widget, int duration)
{
    if (!widget) {
        return;
    }

    QRect endGeometry = widget->geometry();
    QRect startGeometry = endGeometry;
    startGeometry.moveLeft(endGeometry.right());

    widget->setGeometry(startGeometry);

    QPropertyAnimation* animation = new QPropertyAnimation(widget, "geometry", widget);
    animation->setDuration(duration);
    animation->setStartValue(startGeometry);
    animation->setEndValue(endGeometry);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void AnimationManager::slideInFromLeft(QWidget* widget, int duration)
{
    if (!widget) {
        return;
    }

    QRect endGeometry = widget->geometry();
    QRect startGeometry = endGeometry;
    startGeometry.moveRight(endGeometry.left());

    widget->setGeometry(startGeometry);

    QPropertyAnimation* animation = new QPropertyAnimation(widget, "geometry", widget);
    animation->setDuration(duration);
    animation->setStartValue(startGeometry);
    animation->setEndValue(endGeometry);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void AnimationManager::slideInFromTop(QWidget* widget, int duration)
{
    if (!widget) {
        return;
    }

    QRect endGeometry = widget->geometry();
    QRect startGeometry = endGeometry;
    startGeometry.moveBottom(endGeometry.top());

    widget->setGeometry(startGeometry);

    QPropertyAnimation* animation = new QPropertyAnimation(widget, "geometry", widget);
    animation->setDuration(duration);
    animation->setStartValue(startGeometry);
    animation->setEndValue(endGeometry);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void AnimationManager::slideInFromBottom(QWidget* widget, int duration)
{
    if (!widget) {
        return;
    }

    QRect endGeometry = widget->geometry();
    QRect startGeometry = endGeometry;
    startGeometry.moveTop(endGeometry.bottom());

    widget->setGeometry(startGeometry);

    QPropertyAnimation* animation = new QPropertyAnimation(widget, "geometry", widget);
    animation->setDuration(duration);
    animation->setStartValue(startGeometry);
    animation->setEndValue(endGeometry);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void AnimationManager::bounceIn(QWidget* widget, int duration)
{
    if (!widget) {
        return;
    }

    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(opacityEffect);

    QPropertyAnimation* opacityAnimation = new QPropertyAnimation(opacityEffect, "opacity", widget);
    opacityAnimation->setDuration(duration);
    opacityAnimation->setStartValue(0.0);
    opacityAnimation->setEndValue(1.0);
    opacityAnimation->setEasingCurve(QEasingCurve::OutElastic);
    opacityAnimation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(opacityAnimation, &QPropertyAnimation::finished, [opacityEffect]() {
        delete opacityEffect;
    });
}

void AnimationManager::pulse(QWidget* widget, int duration)
{
    if (!widget) {
        return;
    }

    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(opacityEffect);

    QPropertyAnimation* animation = new QPropertyAnimation(opacityEffect, "opacity", widget);
    animation->setDuration(duration);
    animation->setStartValue(1.0);
    animation->setKeyValueAt(0.5, 0.5);
    animation->setEndValue(1.0);
    animation->setLoopCount(1);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation, &QPropertyAnimation::finished, [opacityEffect]() {
        delete opacityEffect;
    });
}

void AnimationManager::shake(QWidget* widget, int duration)
{
    if (!widget) {
        return;
    }

    QRect originalGeometry = widget->geometry();
    int shakeDistance = 10;
    int steps = 6;

    QPropertyAnimation* animation = new QPropertyAnimation(widget, "geometry", widget);
    animation->setDuration(duration);
    animation->setEasingCurve(QEasingCurve::Linear);

    for (int i = 0; i < steps; i++) {
        QRect geometry = originalGeometry;
        int offset = (i % 2 == 0) ? shakeDistance : -shakeDistance;
        geometry.translate(offset, 0);
        animation->setKeyValueAt(static_cast<qreal>(i) / steps, geometry);
    }

    animation->setEndValue(originalGeometry);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void AnimationManager::resizeWidth(QWidget* widget, int targetWidth, int duration)
{
    if (!widget) {
        return;
    }

    QSize startSize = widget->size();
    QSize endSize(targetWidth, startSize.height());

    QPropertyAnimation* animation = new QPropertyAnimation(widget, "size", widget);
    animation->setDuration(duration);
    animation->setStartValue(startSize);
    animation->setEndValue(endSize);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void AnimationManager::resizeHeight(QWidget* widget, int targetHeight, int duration)
{
    if (!widget) {
        return;
    }

    QSize startSize = widget->size();
    QSize endSize(startSize.width(), targetHeight);

    QPropertyAnimation* animation = new QPropertyAnimation(widget, "size", widget);
    animation->setDuration(duration);
    animation->setStartValue(startSize);
    animation->setEndValue(endSize);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void AnimationManager::setupOpacityAnimation(QWidget* widget, qreal startValue, qreal endValue, int duration)
{
    if (!widget) {
        return;
    }

    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(opacityEffect);

    QPropertyAnimation* animation = new QPropertyAnimation(opacityEffect, "opacity", widget);
    animation->setDuration(duration);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation, &QPropertyAnimation::finished, [opacityEffect]() {
        delete opacityEffect;
    });
}

void AnimationManager::setupGeometryAnimation(QWidget* widget, const QRect& startValue, const QRect& endValue, int duration)
{
    if (!widget) {
        return;
    }

    widget->setGeometry(startValue);

    QPropertyAnimation* animation = new QPropertyAnimation(widget, "geometry", widget);
    animation->setDuration(duration);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void AnimationManager::setupSizeAnimation(QWidget* widget, const QSize& startValue, const QSize& endValue, int duration)
{
    if (!widget) {
        return;
    }

    QPropertyAnimation* animation = new QPropertyAnimation(widget, "size", widget);
    animation->setDuration(duration);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}
