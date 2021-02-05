#include "appinspector.h"
appinspector::appinspector(QWidget *parent) : QOpenGLWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents,false);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_AlwaysStackOnTop,true);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(false);
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    setObjectName("container");
    setAttribute(Qt::WA_TransparentForMouseEvents);
    connect(&t,SIGNAL(timeout()),this,SLOT(update()));
    t.start(500);
}
void appinspector::initializeGL(){
    glClearColor(0,0,0,0);
}
void appinspector::addData(QList<int*> i,QStringList n){
    integers.append(i);
    intNames.append(n);
}
void appinspector::addData(QList<QString*> i,QStringList n){
    strings.append(i);
    strNames.append(n);
}
void appinspector::addData(QList<QRect*> i,QStringList n){
    rects.append(i);
    rctNames.append(n);
}
void appinspector::addData(QList<QPoint*> i,QStringList n){
    points.append(i);
    pntNames.append(n);
}
void appinspector::addData(QList<QSize*> i,QStringList n){
    sizes.append(i);
    sizNames.append(n);
}
void appinspector::addData(QList<QWidget*> i,QStringList n){
    widgets.append(i);
    widNames.append(n);
}
QPoint appinspector::rcol(bool i){
    if(!i)return lcol;
    QPoint r(cts*width()/2,rts*25+50);
    if(i)cts++;
    if(cts==1){
        cts=0;
        rts++;
    }
    lcol=r;
    return r;
}
void appinspector::rres(){
    cts=0;
    rts=0;
}
void appinspector::paintGL(){
    painter.begin(this);
    painter.beginNativePainting();
    glClear(GL_COLOR_BUFFER_BIT);
    painter.endNativePainting();
    painter.save();
    painter.setBrush(QColor(255,255,255,200));
    painter.drawRoundedRect(0,0,width(),height(),25,25);
    painter.setPen(QColor(0,0,0,254));
    painter.setFont(QFont("Apple Gothic",10));
    rres();
    for(int i=0;i<integers.count();i++){
        int *k(const_cast<int*>(integers.at(i)));
        painter.drawText(rcol(),intNames.at(i)+":"+QString::number(*k));
    }
    for(int i=0;i<strings.count();i++){
        QString *k(const_cast<QString*>(strings.at(i)));
        painter.drawText(rcol(),strNames.at(i)+":"+*k);
    }
      for(int i=0;i<points.count();i++){
        QPoint *k(const_cast<QPoint*>(points.at(i)));
        painter.drawText(rcol(),pntNames.at(i)+":"+QString::number(k->x())+ " x " +QString::number(k->y()));
    }
    for(int i=0;i<rects.count();i++){
        QRect *k(const_cast<QRect*>(rects.at(i)));
        painter.drawText(rcol(),rctNames.at(i)+":"+QString::number(k->x())+  "x " +QString::number(k->y())+"/"+QString::number(k->width())+":"+QString::number(k->height()));;
    }
     for(int i=0;i<sizes.count();i++){
        QSize *k(const_cast<QSize*>(sizes.at(i)));
        painter.drawText(rcol(),sizNames.at(i)+":"+QString::number(k->width())+":"+QString::number(k->height()));;
    }
    for(int i=0;i<widgets.count();i++){
           QWidget *k(const_cast<QWidget*>(widgets.at(i)));
        QString msg=" * no data * ";
        QPoint gpos=k->mapToGlobal(QPoint(0,0));
        if(k){
            QString n;
            if(widNames.count()>i)n=widNames.at(i);
            n+="("+k->objectName()+")";

            msg=" name:"+n+" pos: "+QString::number(k->x())+" x "+QString::number(k->y())+"/"+QString::number(k->width())+":"+QString::number(k->height())+" GLOBAL pos: "+QString::number(gpos.x())+" x "+QString::number(gpos.y());
        }
        painter.drawText(rcol(),msg);
    //    qDebug()<<"Painter:"<<msg<<rcol(false);
    }
    painter.restore();
    painter.end();
}

appinspector::~appinspector(){
}
