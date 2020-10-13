#ifndef CIRCLES_H
#define CIRCLES_H
#include "globals.h"

class circles : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit circles(QWidget *parent = nullptr);
    ~circles() override;
    double opacity,des_opacity,step;
    bool underMouse();
    void setOpacity(qreal);
    QTimer *timer;

protected:
    void paintGL() override;
    void initializeGL() override;
    void resizeEvent(QResizeEvent *e) override;
public slots:
    void check_timer();
};

#endif // CIRCLES_H
