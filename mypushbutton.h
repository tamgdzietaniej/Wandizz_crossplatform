#ifndef WIDGET_H
#define WIDGET_H
#include "globals.h"
class myPushButton : public QPushButton{
    Q_OBJECT
public:
    explicit myPushButton(QWidget *parent = nullptr);
    ~myPushButton();
    QColor color_idle,color_noerror,color_error,color_success,color_progress,color_title,shad_col;
    QColor state_idle,state_noerror,state_error,state_success,state_progress,state_title,shad2_col;
    bool mode=0;
    int font_s_1,font_s_2,font_s_3,prop_h;
    int x;
    int y;
    int w;
    int h;
    int suby;
    int subh;
    int startAngle;
    int spanAngle;
    QRectF main_geo,sub_geo,ssub_geo;
    QRectF titlerect;
    QRectF imgrect;
    QRectF prorect;
    QPen pen_pro,pen_idle,pen_title;
  //  QPen pen_shad,
    QPen pen_shad2;
    void set_geo();
signals:

public slots:
    void setProgress(int val);
    void setButton(QString,int w=-1) ;
    void setState(QColor,int timeout=5000);
    void resetColors();
    void resetProgress();
    void setTitle(QString);
    void showPerc(bool);
    void setIdleColor(QColor);
    void resetStates();
protected:
    void paintEvent(QPaintEvent *);

private:
    double progress;
    int wdth=0;
    QString title;
    QTimer progress_timer,reset_timer;
    int progress_perc,progress_req;
    bool show_perc;


};

#endif // WIDGET_H
