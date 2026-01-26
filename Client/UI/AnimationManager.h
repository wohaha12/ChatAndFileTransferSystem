#ifndef CLIENT_UI_ANIMATIONMANAGER_H
#define CLIENT_UI_ANIMATIONMANAGER_H

#include <QObject>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QWidget>

class AnimationManager : public QObject
{
    Q_OBJECT

public:
    static AnimationManager* instance();

    void fadeIn(QWidget* widget, int duration = 300);
    void fadeOut(QWidget* widget, int duration = 300);
    void slideInFromRight(QWidget* widget, int duration = 300);
    void slideInFromLeft(QWidget* widget, int duration = 300);
    void slideInFromTop(QWidget* widget, int duration = 300);
    void slideInFromBottom(QWidget* widget, int duration = 300);
    void bounceIn(QWidget* widget, int duration = 600);
    void pulse(QWidget* widget, int duration = 1000);
    void shake(QWidget* widget, int duration = 500);
    void resizeWidth(QWidget* widget, int targetWidth, int duration = 300);
    void resizeHeight(QWidget* widget, int targetHeight, int duration = 300);

private:
    explicit AnimationManager(QObject* parent = nullptr);
    ~AnimationManager();

    void setupOpacityAnimation(QWidget* widget, qreal startValue, qreal endValue, int duration);
    void setupGeometryAnimation(QWidget* widget, const QRect& startValue, const QRect& endValue, int duration);
    void setupSizeAnimation(QWidget* widget, const QSize& startValue, const QSize& endValue, int duration);

    static AnimationManager* s_instance;
};

#endif // CLIENT_UI_ANIMATIONMANAGER_H