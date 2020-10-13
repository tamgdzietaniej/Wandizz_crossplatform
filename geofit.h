#ifndef GEOFIT_H
#define GEOFIT_H
#include "globals.h"

class geofit : public QObject
{
    Q_OBJECT
public:
    explicit geofit(QObject* parent=nullptr);
    ~geofit();

    QWidget* widget;
    int wx,wy,ww,wh;
    int x,y,w,h;
    qreal nx,ny,nw,nh;
    qreal sW,sH,sX,sY;
    qreal VH;
    qreal xw,xh;
    QRect new_w_rect;
    qreal sFX,sFY,v_marg;

    void update_geo(bool);
    bool fixed;
    bool resize_container_height;
    qreal set_container_height;
    qreal perc_container_height;

    QList<QWidget *> widgets;
    QList<QWidget *> rescale_list;

    QSize m_size;
    QRect m_geo;
    QRect mv_geo;
    qreal marg;
public slots:
    void resizer(QWidget*,bool mode=false);
    void init();
signals:
    void finish();
};

#endif // GEOFIT_H
