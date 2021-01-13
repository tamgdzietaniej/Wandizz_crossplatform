#include "geofit.h"

geofit::geofit(QObject* parent):QObject(parent){
    init();
}
void geofit::resizer(QWidget* m_widget,bool mode){
    QStringList flist={"verticalframe","shadow"};
    update_geo(mode);
    m_widget->geometry().getRect(&x,&y,&w,&h);
    if(set_container_height>0){
        sH=set_container_height;
    }
    sH=sH*perc_container_height/100;
    sFX=qreal(sW) / qreal(w);
    sFY=qreal(sH) / qreal(h);
    if(resize_container_height){
        m_widget->setGeometry(m_geo);
        if(fixed){
            m_widget->setFixedSize(sW,sH);
        }
        //      qDebug()<<"RESIZE CONT:"<<m_widget->geometry();
    }
    widgets = m_widget->findChildren<QWidget *>();
    for(int j = 0; j < widgets.count(); ++j){
        widget=widgets.at(j);
        if(widget!=nullptr){
            if(widget->objectName()=="centralwidget"){
                widget->setFixedSize(sW,sH);
                widget->resize(sW,sH);
                widget->setGeometry(m_geo);
                continue;
            }
            widget->geometry().getRect(&wx,&wy,&ww,&wh);
            x=wx*sFX;          // left - top corner X
            y=wy*sFY;          // Y
            w=ww*sFX;     // width
            h=wh*sFY;
            xw=ww*sFY;
            xh=wh*sFY;
            if(flist.contains(widget->objectName())){
                h=xh;
            }
            if(fixed){
                widget->setMinimumSize(w,h);
                widget->setMaximumSize(w,h);
            }
            widget->setGeometry(x,y,w,h);
        }
    }
    //   qDebug()<<"Geofit:"<<m_widget->objectName()<<m_widget->geometry();
    init();
}

void geofit::init(){
   // qDebug()<<"GEOFIT:"<<QApplication::primaryScreen()->availableGeometry()<<QApplication::primaryScreen()->availableVirtualGeometry()<<QApplication::primaryScreen()->geometry()<<QApplication::primaryScreen()->virtualGeometry();
    fixed=true;  // if you want to have MinMax sizes setted
    resize_container_height=true;
    set_container_height=0;
    perc_container_height=100;
    marg=0;
}
QRect geofit::update_geo(bool mode){
#if defined(Q_OS_IOS)
    m_geo=QApplication::primaryScreen()->geometry();
#elif defined(Q_OS_ANDROID)
    m_geo=QApplication::primaryScreen()->virtualGeometry();
#elif defined (Q_OS_MACX)
m_geo=QApplication::primaryScreen()->geometry();
#endif
    v_marg=m_geo.y();
    sX=m_geo.x();
    sY=m_geo.y();
    sW=m_geo.width();
    sH=m_geo.height();
    if(sW>sH){
        sH=0.75*sH;
        marg=(sW-sH*0.6)/2;
        sW=sH*0.6;
    }
}
geofit::~geofit(){
    //  qDebug()<<"~geofit()";
}



