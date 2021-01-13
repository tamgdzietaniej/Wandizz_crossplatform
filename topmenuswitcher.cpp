#include "topmenuswitcher.h"
topMenuSwitcher::topMenuSwitcher(QWidget *parent,int dopacity) : QOpenGLWidget(parent){
    setAttribute(Qt::WA_AlwaysStackOnTop,true);
    setMouseTracking(false);
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    setObjectName("container");
    dpi = devicePixelRatio();
#if defined (Q_OS_IOS)
    dpi*=2;
#endif

    ready_to_paint=false;
}
void topMenuSwitcher::initializeGL(){
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0,0,0,0);
}
void topMenuSwitcher::showSelectors(bool s){
    if(selector_visible!=s){
        selector_visible=s;
        update();
    }
}
void topMenuSwitcher::setFrames(QList<QWidget*> lfrm_a,QList<QWidget*> lfrm_b){
    frames[0]=static_cast<QList<QWidget*>>(lfrm_a);
    frames[1]=static_cast<QList<QWidget*>>(lfrm_b);
    ready_to_paint=true;
}
bool topMenuSwitcher::get_hover(QPoint pos){
    if(!ready_to_paint){
        return false;
    }
    QList<QPushButton*> wl;
    if(!selector_visible){
        wl.clear();
        for(int j=0;j<2;j++){
            for(int i=0;i<frames[j].count();i++){
                wl.append(frames[j].at(i)->findChildren<QPushButton *>(QString(),Qt::FindChildOptions(Qt::FindChildrenRecursively)));
            }
        }
    }
    qDebug()<<"WL:"<<wl;
    if(wl.count()>0){
        for(int i=0;i<wl.count();i++){
            if(wl.at(i)->rect().contains(wl.at(i)->mapFromGlobal(pos)) && wl.at(i)->isEnabled()){
                qDebug()<<"CLICK:"<<wl.at(i)->objectName()<<" enabled";
                wl.at(i)->click();
                return false;
            } else qDebug()<<"CLICK:"<<wl.at(i)->objectName()<<" disabled";
        }
    }
    return true;
}
void topMenuSwitcher::resizeEvent(QResizeEvent *e){
    resizeGL(e->size().width(),e->size().height());
}
void topMenuSwitcher::paintGL(){
    if(ready_to_paint){
        QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
        f->glClear(GL_COLOR_BUFFER_BIT);
        QPainter painter(this);
        currFrameSet=frames[!selector_visible];
        for(int i=0;i<currFrameSet.count();i++){
            currFrameSet.at(i)->render(&painter,QPoint(0,0));
        }
        painter.end();
    }
}
topMenuSwitcher::~topMenuSwitcher(){
}
