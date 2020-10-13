#include "circles.h"
circles::circles(QWidget *parent) : QOpenGLWidget(parent){
    setAttribute(Qt::WA_AlwaysStackOnTop,true);
    setAttribute(Qt::WA_OpaquePaintEvent,false);
    setAttribute(Qt::WA_TranslucentBackground,false);
    setObjectName("container");
    timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(check_timer()));
    opacity=0;
    des_opacity=0;
}
void circles::check_timer(){
     if(opacity<0){
        opacity=0;
        update();
    } else
        if(opacity>1){
        opacity=1;
        update();
    } else
        if(opacity!=des_opacity){
        if(opacity<des_opacity)opacity+=.25;
        if(opacity>des_opacity)opacity-=.2;
        update();
    }
}
void circles::initializeGL(){
    glClearColor(0,0,0,0);
}
void circles::setOpacity(qreal o){
        des_opacity=o;
        if(!timer->isActive())
            timer->start(17);
}

bool circles::underMouse(){
    return rect().contains(mapFromGlobal(QCursor::pos()));
}

void circles::resizeEvent(QResizeEvent *e){
    resizeGL(e->size().width(),e->size().height());
}
void circles::paintGL(){
    QRect crect1,crect2,crect3,crect4;
    int x,y,w,h,mw,mh;
    crect1=rect();
    crect1.getRect(&x,&y,&w,&h);
    mw=w;
    mh=h;
    int sc=6;
    crect1.setRect(x+(mw/sc)*2,y+(mh/sc)*2,w-(mw/sc)*4,h-(mh/sc)*4);

    crect2=crect1;
    crect2.getRect(&x,&y,&w,&h);
    crect2.setRect(x+(mw/sc)*2,y+(mh/sc)*2,w-(mw/sc)*4,h-(mh/sc)*4);

    crect3=crect2;
    crect3.getRect(&x,&y,&w,&h);
    crect3.setRect(x+(mw/sc)*2,y+(mh/sc)*2,w-(mw/sc)*4,h-(mh/sc)*4);


    crect4=crect3;
    crect4.getRect(&x,&y,&w,&h);
    crect4.setRect(x+(mw/sc)*2,y+(mh/sc)*2,w-(mw/sc)*4,h-(mh/sc)*4);


          QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
        QPen pen;
        pen.setColor(QColor(170,70,120,195));
        pen.setWidth(mw/sc);
        painter.setPen(pen);
            painter.drawArc(crect1, 0, 360*16);

        pen.setColor(QColor(170,70,120,215));
            painter.setPen(pen);
            painter.drawArc(crect2, 0, 360*16);

        pen.setColor(QColor(170,70,120,235));
            painter.setPen(pen);
            painter.drawArc(crect3, 0, 360*16);

              pen.setColor(QColor(170,70,120,255));
            painter.setPen(pen);
            painter.drawArc(crect4, 0, 360*16);

          painter.end();
}
circles::~circles(){
}
