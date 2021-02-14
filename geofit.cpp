#include "geofit.h"

geofit::geofit(QObject* parent):QObject(parent){
    init();
}
void geofit::resizer(QWidget* m_widget,bool mode){
    QStringList flist={"verticalframe","shadow"};
    QRect mwr(m_widget->geometry());
    update_geo(mode);
    m_widget->geometry().getRect(&x,&y,&w,&h);
    if(set_container_height>0){
        sH=set_container_height;
    }
    sH=sH*perc_container_height/100;
    sFX=double(sW) / double(w);
    sFY=double(sH) / double(h);
    scpan=round((sFX/sFY)*100)/100;
    qDebug()<<"SWSH:"<<sFX<<sFY<<scpan;
    if(scpan==1)m_geo=a_geo;
    if(resize_container_height){
        m_widget->setGeometry(m_geo);
        if(fixed){
            m_widget->setFixedSize(sW,sH);
        }
    }
    int aa=m_widget->height();
    upsect=QRect(m_widget->rect().adjusted(0,0,0,-aa/3*2));
    dnsect=QRect(m_widget->rect().adjusted(0,aa/3*2,0,0));
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
            QPoint wpos=widget->mapToGlobal(QPoint(0,0));
            QRect wrct(wpos,widget->size());
            if(wrct.intersects(upsect) && wrct.intersects(dnsect)){
              // qDebug()<<"GEO:UPDOWN:"<<y<<h<<"/"<<y/scpan<<h*scpan<<widget->objectName()<<widget->geometry();
                y*=scpan;
                h*=scpan;
            } else if(wrct.intersects(upsect)){
              // qDebug()<<"GEO:UP:"<<y<<h<<"/"<<y+((h/scpan)-h)/2<<h*scpan<<widget->objectName()<<widget->geometry();
                h*=scpan;
                y+=((h/scpan)-h)/2;
            } else {
              // qDebug()<<"GEO:DN:"<<y<<h<<"/"<<y-((h/scpan)-h)/2<<h*scpan<<widget->objectName()<<widget->geometry();
                int ty=widget->mapToGlobal(QPoint(0,widget->size().height())).y();
                int noy=QApplication::primaryScreen()->geometry().height()-(ty*sFY)*scpan;
                int moy=noy-(mwr.height()-ty);
              //  qDebug()<<ty<<QApplication::primaryScreen()->geometry().height();
                y=y*scpan;//+((sH-y/scpan);
                if(widget->y()==widget->mapToGlobal(QPoint(0,0)).y()){
                    y+=moy;
//                    qDebug()<<"GEO:W:"<<widget->objectName();
               }// else {
                 //   qDebug()<<"GEO:W:PAREMT"<<widget->objectName();
               // }
                h*=scpan;

               // qDebug()<<"GEO:DN:COR:y+oy:"<<y+moy<<"oy"<<moy<<"m_w"<<w<<h<<"wpos"<<wpos.y()<<"wh"<<widget->geometry()<<widget->objectName();
            }
            if(fixed){
                widget->setMinimumSize(w,h);
                widget->setMaximumSize(w,h);
            }
            widget->setGeometry(x,y,w,h);
        }
    }
    init();
}

void geofit::init(){
    fixed=true;  // if you want to have MinMax sizes setted
    resize_container_height=true;
    set_container_height=0;
    perc_container_height=100;
    marg=0;
    update_geo(0);
}
QRect geofit::update_geo(bool mode){
#if defined(Q_OS_IOS)
    m_geo=QApplication::primaryScreen()->availableGeometry();
    a_geo=QApplication::primaryScreen()->geometry();
    gmarg=m_geo.y();
    gdiff=a_geo.height()-m_geo.height();
    qDebug()<<"GEOS:"<<QApplication::primaryScreen()->geometry()<<QApplication::primaryScreen()->availableGeometry()<<QApplication::primaryScreen()->virtualGeometry()<<QApplication::primaryScreen()->availableVirtualGeometry()<<gmarg<<gdiff<<"("<<gdiff/2<<")";
   if(mode){

   }
#elif defined(Q_OS_ANDROID)
    m_geo=QApplication::primaryScreen()->virtualGeometry();
    a_geo=m_geo;
    gdiff=0;
    gmarg=0;
    qDebug()<<"GEOS:"<<QApplication::primaryScreen()->geometry()<<QApplication::primaryScreen()->availableGeometry()<<QApplication::primaryScreen()->virtualGeometry()<<QApplication::primaryScreen()->availableVirtualGeometry();

#elif defined (Q_OS_MACX)
    m_geo=QApplication::primaryScreen()->geometry();
#endif

    sX=m_geo.x();
    sY=m_geo.y();
    sW=m_geo.width();
    sH=m_geo.height();

    if(sW>sH){
        sH=0.75*sH;
        marg=(sW-sH*0.6)/2;
        sW=sH*0.6;
    }
    return m_geo;
}
geofit::~geofit(){
    //  qDebug()<<"~geofit()";
}



