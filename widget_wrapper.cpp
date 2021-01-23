#include "widget_wrapper.h"
#define G_N_ELEMENTS(arr) ((sizeof(arr))/(sizeof(arr[0])))
widget_wrapper::widget_wrapper(QWidget *parent,QString cname) : QOpenGLWidget(parent),selectors_visible(false),
    ready_to_paint(false),type(cname),selectors_visible_prev(false){
    setAttribute(Qt::WA_AlwaysStackOnTop,true);
    setAttribute(Qt::WA_DeleteOnClose,true);
    setMouseTracking(true);
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    dpi = devicePixelRatio();
    debg=false;
#if defined (Q_OS_IOS)
    dpi*=2;
#endif
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    marker_head=new QOpenGLWidget(this);
    marker_tail=new QOpenGLWidget(this);
    marker_head->setFixedSize(0,0);
    marker_tail->setFixedSize(0,0);
    marker_head->move(0,0);
    marker_tail->move(0,height());
}
void widget_wrapper::run(){
    show();
    update();
}
void widget_wrapper::initializeGL(){
    glClearColor(0,0,0,0);
}
bool widget_wrapper::get_hover(QPoint pos){
    mpos=pos;
    if(!ready_to_paint){
        qDebug()<<objectName()<<"not ready";
        return false;
    }
    for(int j=0;j<wlist.count();j++){
        for(int i=0;i<wlist.at(j).count();i++){
            QList<QPushButton*> bl = wlist.at(j).at(i)->findChildren<QPushButton *>(QString(),Qt::FindChildOptions(Qt::FindChildrenRecursively));
            qDebug()<<"BUTTONS:"<<bl;
            if(bl.count()>0){
                for(int k=0;k<bl.count();k++){
                    if(clicked(bl.at(k))){
                        return false;
                    }
                }
            }
        }
        for(int i=0;i<wlist.at(j).count();i++){
            QList<QCheckBox*> bl = wlist.at(j).at(i)->findChildren<QCheckBox*>(QString(),Qt::FindChildOptions(Qt::FindChildrenRecursively));
              qDebug()<<"CHKBUTTONS:"<<bl;
            if(bl.count()>0){
                for(int k=0;k<bl.count();k++){
                    if(clicked(bl.at(k))){
                        return false;
                    }
                }
            }
        }
    }
//    if(clicked(edit)){
   //     if(!input->isVisible()){
    //        input->setVisible(true);
     //   }
    //    return false;
  //  }
}
bool widget_wrapper::clicked(QWidget* w){
    if(!w)return false;
    if(w->rect().contains(w->mapFromGlobal(mpos))){
        if(QString::fromLatin1(w->metaObject()->className())=="QPushButton"){
            if(w->isEnabled()){
                static_cast<QPushButton*>(w)->click();
            }
        }
        if(QString::fromLatin1(w->metaObject()->className())=="QLineEdit"){
            if(w->isEnabled()){
                static_cast<QLineEdit*>(w)->setFocus();
            }
        }
        if(QString::fromLatin1(w->metaObject()->className())=="QCheckBox"){
            if(w->isEnabled()){
                static_cast<QCheckBox*>(w)->click();
            }
        }
        return true;
    }
    return false;
}


void widget_wrapper::showSelectors(bool s){
    if(selectors_visible!=s){
        selectors_visible=s;
        update();
    }
}
int widget_wrapper::mrh(){
    if(oldh!=marker_head->y())
        qDebug()<<"MRH:"<<marker_head->y()<<marker_head->pos();
    oldh=marker_head->y();
    return marker_head->y()+vcorr;
}
int widget_wrapper::mrt(){
    if(oldt!=marker_tail->y())
        qDebug()<<"MRT:"<<marker_tail->y()<<marker_tail->pos();
    oldt=marker_tail->y();
    return marker_tail->y();
}
void widget_wrapper::setupWrapper(QList<QList<QWidget*>> wdl){
    /* mapper */
    /* groups:
     * 0 - static, rendered when vP::UNDER
     * 1 - dynamic, rendered always
     * 2- dynamic, rendered when vP::UNDER
     * 3- static, rendered when vP::OVER
     */
    marker_head->move(0,0);
    marker_tail->move(0,0);
    wlist.clear();
    wlist.append(wdl);
    for(int j=0;j<wdl.count();j++){
            for(int i=0;i<wlist.at(j).count();i++){
                QPoint tpos(wlist.at(j).at(i)->mapToGlobal(QPoint(0,0)));
                tpos.setY(tpos.y());
                if(j==1 || j==2){
                    wlist.at(j).at(i)->setParent(this);
                     wlist.at(j).at(i)->hide();
            }
                wlist.at(j).at(i)->move(mapFromGlobal(tpos));
            }
    }
    ready_to_paint=true;
    if(!isVisible()){
        update();
    QApplication::processEvents();
    }
}
void widget_wrapper::resizeEvent(QResizeEvent *e){
    resizeGL(e->size().width()*dpi,e->size().height()*dpi);
}
void widget_wrapper::paintGL(){
    if(ready_to_paint){
        QPainter painter(this);

        /* flush gpu */
        painter.beginNativePainting();
        glClear(GL_COLOR_BUFFER_BIT);
        painter.endNativePainting();

        /* paint event */
        /* main render loop */
        for(int j=0;j<wlist.count();j++){

            /* switch between alternative widgets */
            /* vP::UNDER - initial position */
            if( ( selectors_visible == vP::OVER && ( j==0 || j == 2 ) ) || (selectors_visible == vP::UNDER && j == 3 ) ){
                continue;
            }

            /* inner loop - cluster */
            for(int i=0;i<wlist.at(j).count();i++){
                QWidget* w=wlist.at(j).at(i);
     //           qDebug()<<"pint"<<w->objectName()<<w->isEnabled();
                if(w)
                    if(w->isEnabled()){
         //               qDebug()<<"Rend:"<<w->objectName()<<w->pos()<<w->isEnabled()<<w->size();
                        if(rect().intersects(QRect(w->pos(),w->size()))){

                            w->render(&painter,w->mapToGlobal(QPoint(0,0)));
                 //           if(w->objectName()=="switch_b_frame")
           //                 qDebug()<<"RENDERING SWITCH:"<<w->pos();
                        }
                    }
            }
        }
        if(nightmode)
            painter.fillRect(rect(),QColor(0,0,0,50));
        painter.end();
    } else
        qDebug()<<"Container not ready";
}
void widget_wrapper::forceUpdate(){
    //  qDebug()<<"FORCEUPDATE GLCONT";
    update();
}
widget_wrapper::~widget_wrapper(){
    //    qDebug()<<"DESTROYER:WWRAP";
}
