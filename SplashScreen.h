#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>

class SplashScreen : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal m_progress READ progress WRITE setProgress)

public:
    explicit SplashScreen(QWidget *parent = nullptr);
    ~SplashScreen();

    void startAnimation();
    void fadeOutAndClose();

signals:
    void finished(); // emitted when splash is done, main window should show

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    qreal progress() const { return m_progress; }
    void setProgress(qreal p);

    qreal m_progress = 0.0;
    QGraphicsOpacityEffect *m_opacityEffect;
    QPropertyAnimation  *m_fadeInAnim;
    QPropertyAnimation  *m_progressAnim;
    QPropertyAnimation  *m_fadeOutAnim;
};

#endif // SPLASHSCREEN_H
