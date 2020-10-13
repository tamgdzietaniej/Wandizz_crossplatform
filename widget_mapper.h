#ifndef WIDGET_MAPPER_H
#define WIDGET_MAPPER_H
#include "globals.h"
class widget_mapper : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit widget_mapper(QWidget *parent = nullptr,int dopacity=1);
    ~widget_mapper() override;
    void setFrames(QWidget* f1,QWidget* f2);
    bool get_hover(QPoint);
    int dpi;
    bool noanim;
    bool ready_to_paint;
    bool underMouse();
    void showSelector(bool);
    bool selector_visible;
    QPoint diff;
    QWidget* topFrame,*topSwFrame;
   protected:
    void paintGL() override;
    void initializeGL() override;
   // void mousePressEvent(QMouseEvent*) override;
  //  void mouseReleaseEvent(QMouseEvent*) override;
    void resizeEvent(QResizeEvent *e) override;

private:
    int distance(QPoint);
};

#endif // WIDGET_MAPPER_H
