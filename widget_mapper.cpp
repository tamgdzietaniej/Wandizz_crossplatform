#include "widget_mapper.h"
widget_mapper::widget_mapper(QWidget *parent,int dopacity) : QOpenGLWidget(parent){
    setAttribute(Qt::WA_AlwaysStackOnTop,true);
  //  setAttribute(Qt::WA_TransparentForMouseEvents);
    setMouseTracking(false);
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
   // setAttribute( Qt::WA_TransparentForMouseEvents, false );
    setObjectName("container");
    dpi=devicePixelRatio();
    ready_to_paint=false;
}
void widget_mapper::initializeGL(){
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void widget_mapper::showSelector(bool s){
    if(selector_visible!=s){
        selector_visible=s;
        update();
    }
}
void widget_mapper::setFrames(QWidget* top_frame,QWidget *top_switch_frame){
    topFrame=static_cast<QWidget*>(top_frame);
    topFrame->hide();
    topSwFrame=static_cast<QWidget*>(top_switch_frame);
    topSwFrame->hide();
    ready_to_paint=true;
}
int widget_mapper::distance(QPoint p){
    return sqrt(p.x()*p.x()+p.y()*p.y());
}
/*
void widget_mapper::mousePressEvent(QMouseEvent *e){
    qDebug()<<"PRESS  MAPPER:";
    diff=e->pos();
    e->setAccepted(false);
}
void widget_mapper::mouseReleaseEvent(QMouseEvent *e){
     qDebug()<<"release  MAPPER:";
    if(abs(distance(e->pos()-diff))<10*dpi)
        get_hover(e->globalPos());
    else qDebug()<<"MAPPER DIFF:"<<abs(distance(e->pos()-diff))<<10*dpi;
    e->setAccepted(true);
}
*/
bool widget_mapper::get_hover(QPoint pos){
    if(!ready_to_paint){
        return false;
    }
    QList<QPushButton*> wl=topFrame->findChildren<QPushButton *>();
    wl.append(topSwFrame->findChildren<QPushButton *>());
    if(wl.count()>0){
        for(int i=0;i<wl.count();i++){
            if(wl.at(i)->rect().contains(wl.at(i)->mapFromGlobal(pos)) && wl.at(i)->isEnabled()){
                wl.at(i)->click();

            }
        }
    }
    return false;
}
void widget_mapper::resizeEvent(QResizeEvent *e){
    resizeGL(e->size().width(),e->size().height());
}
void widget_mapper::paintGL(){
    if(ready_to_paint){
        QPainter painter(this);
        painter.beginNativePainting();
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        painter.endNativePainting();
        topFrame->render(&painter);
        if(selector_visible)
            topSwFrame->render(&painter);
        painter.end();
    }
}
widget_mapper::~widget_mapper(){
}
