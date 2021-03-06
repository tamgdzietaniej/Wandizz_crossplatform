#ifndef GEOFIT_H
#define GEOFIT_H
#include <QObject>
#include <QSize>
#include <QRect>
#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QDesktopWidget>
#include <QOperatingSystemVersion>
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
    qreal scpan;
    QRect upsect,dnsect;
    QRect update_geo(bool);
    bool fixed;
    bool resize_container_height;
    qreal set_container_height;
    qreal perc_container_height;

    QList<QWidget *> widgets;
    QList<QWidget *> rescale_list;
    int gdiff,gmarg;
    QSize m_size;
    QRect m_geo;
    QRect a_geo;
    qreal marg;
public slots:
    void resizer(QWidget*,bool mode=false);
    void init();
signals:
    void finish();
};

#endif // GEOFIT_H
