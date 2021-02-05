#include "widget_wrapper.h"
#define G_N_ELEMENTS(arr) ((sizeof(arr))/(sizeof(arr[0])))
widget_wrapper::widget_wrapper(QWidget *parent,QString cname) : QOpenGLWidget(parent),selectors_visible(false),
    ready_to_paint(false),selectors_visible_prev(false){
    setAttribute(Qt::WA_AlwaysStackOnTop,true);
    setAttribute(Qt::WA_DeleteOnClose,true);
    setAttribute(Qt::WA_TransparentForMouseEvents,false);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(false);
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    dpi = devicePixelRatio();
    debg=false;
#if defined (Q_OS_IOS)
    dpi*=2;
#endif
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    gradient.setCoordinateMode(QLinearGradient::ObjectMode);
    gradient.setStart(0,0);
    gradient.setFinalStop(QPoint(1,0));// diagonal gradient from top-left to bottom-right
    //   gradient.setColorAt(0, QColor(59,34,100));
    gradient.setColorAt(0, QColor(50,25,90));
    gradient.setColorAt(0.5, QColor(150,2,170));
    gradient.setColorAt(1, QColor(59,34,100));
    gradient2.setCoordinateMode(QLinearGradient::ObjectMode);
    gradient2.setStart(0,0);
    gradient2.setFinalStop(QPoint(0,1));
    gradient2.setColorAt(0, QColor(0,0,0,200));
    gradient2.setColorAt(0.05, QColor(0,0,0,80));
    gradient2.setColorAt(0.5, QColor(0,0,0,60));
    gradient2.setColorAt(0.95, QColor(0,0,0,80));
    gradient2.setColorAt(1, QColor(0,0,0,200));
    int wdt=QApplication::desktop()->geometry().width();
    int hgh=QApplication::desktop()->geometry().height();
    int marg=15*wdt/400;
    qDebug()<<"COORDS:"<<width()<<height();
    shadow_geo.setRect(marg,marg,wdt-2*marg,hgh-2*marg);
    setMarkers();
}
void widget_wrapper::setMarkers(){
    marker_head=new QOpenGLWidget(this);
    marker_body[0]=new QOpenGLWidget(this);
    marker_body[1]=new QOpenGLWidget(this);
    marker_tail=new QOpenGLWidget(this);
    marker_head->setFixedSize(0,0);
    marker_body[0]->setFixedSize(0,0);
    marker_body[1]->setFixedSize(0,0);
    marker_tail->setFixedSize(0,0);
    marker_head->move(0,0);
    marker_body[0]->move(0,0);
    marker_body[1]->move(0,0);
    marker_tail->move(0,height());
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
            if(bl.count()>0){
                for(int k=0;k<bl.count();k++){
                    if(clicked(bl.at(k))){
                        return false;
                    }
                }
            }
        }
    }
    if(prospectInfo->isEnabled()){
        QList<QPushButton*> bl=prospectInfo->findChildren<QPushButton*>(QString(),Qt::FindChildrenRecursively);
        if(bl.count()>0){
            bl[0]->click();
        }
        else qDebug()<<"no button";
    }
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
void widget_wrapper::setTypeID(bool ctx){
    type_id=ctx;
}

void widget_wrapper::showSelectors(bool s){
    if(selectors_visible!=s){
        selectors_visible=s;
        update();
    }
}
int widget_wrapper::mrh(){
    return marker_head->y();
}
int widget_wrapper::mrt(){
    return marker_tail->y();
}
int widget_wrapper::mrb(){
    return marker_body[type_id]->y();
}
void widget_wrapper::setMrb(bool ctx,int m){
    marker_body[ctx]->move(0,m);
}
void widget_wrapper::setupInfo(QFrame *pi){
    prospectInfo=static_cast<QFrame*>(pi);
}
void widget_wrapper::setupWrapper(QList<QList<QWidget*>> wdl){
    /* mapper */
    /* groups:
     * 0 - static, rendered when vP::UNDER
     * 1 - dynamic, rendered always
     * 2- dynamic, rendered when vP::UNDER
     * 3- static, rendered when vP::OVER
     */
    wlist.clear();
    wlist.append(wdl);
    for(int j=0;j<wdl.count();j++){
        for(int i=0;i<wlist.at(j).count();i++){
            QPoint tpos(wlist.at(j).at(i)->mapToGlobal(QPoint(0,0)));
            tpos.setY(tpos.y()-mrh());
            if(j==1 || j==2){
                wlist.at(j).at(i)->setParent(this);
                wlist.at(j).at(i)->setVisible(false);
            }
            wlist.at(j).at(i)->move(mapFromGlobal(tpos));
        }
    }
    int marg=15*width()/400;
    shadow_geo.setRect(marg,marg,width()-2*marg,height()-2*marg);
    ready_to_paint=true;
}
void widget_wrapper::resizeEvent(QResizeEvent *e){
    resizeGL(e->size().width()*dpi,e->size().height()*dpi);
}
void widget_wrapper::paintGL(){
    QPainter painter(this);
    qDebug()<<"PAINT";
    /* flush gpu */
    painter.beginNativePainting();
    glClear(GL_COLOR_BUFFER_BIT);
    painter.endNativePainting();

    /* background */
    QPainter p(this);
    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColor(0,0,0,0));
    p.setRenderHints(QPainter::HighQualityAntialiasing);
    p.setPen(pen);
    p.setBrush(gradient);
    p.fillRect(rect(),p.brush());
    p.setPen(QColor(0,0,0,100));
    p.setBrush(QColor(0,0,0,70));
    p.drawRoundedRect(shadow_geo,5,5);
    if(ready_to_paint){
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
                if(w)
                    if(w->isEnabled()){
                        if(rect().intersects(QRect(w->pos(),w->size()))){
                            w->render(&painter,w->mapToGlobal(QPoint(0,0)));
                        }
                    }
            }
        }
        if(prospectInfo->isEnabled()){
            prospectInfo->render(&painter,prospectInfo->pos());
        }
        //    if(noResults->isEnabled()){
        //        noResults->render(&painter,noResults->pos());
        //    }
        if(nightmode)
            painter.fillRect(rect(),QColor(0,0,0,50));
        painter.end();
        qDebug()<<"PAINTER GLCONTAINER";
    } else
        qDebug()<<"Container not ready";
}
void widget_wrapper::forceUpdate(){
    //  qDebug()<<"FORCEUPDATE GLCONT";
    update();
}
widget_wrapper::~widget_wrapper(){
    qDebug()<<"DESTROYER:WWRAP";
}
