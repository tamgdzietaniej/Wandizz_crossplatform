#ifndef TOPMENUSWITCHER_H
#define TOPMENUSWITCHER_H
#include "globals.h"
class topMenuSwitcher : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit topMenuSwitcher(QWidget *parent = nullptr,int dopacity=1);
    ~topMenuSwitcher() override;
    void setFrames(QList<QWidget*> f1,QList<QWidget*> f2);
    bool get_hover(QPoint);
    void showSelectors(bool);
    bool selector_visible;
private:
    int dpi;
    bool noanim;
    bool ready_to_paint;
    QList<QWidget*> frames[2], currFrameSet;
protected:
    void paintGL() override;
    void initializeGL() override;
    void resizeEvent(QResizeEvent *e) override;
};

#endif // TOPMENUSWITCHER_H
