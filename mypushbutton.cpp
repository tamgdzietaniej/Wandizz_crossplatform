#include "mypushbutton.h"
#include <QRgb>
myPushButton::myPushButton(QWidget* parent):QPushButton(parent)
{
     QSize m_size=QApplication::primaryScreen()->availableSize();
    wdth=m_size.width()/50;
    font_s_3=m_size.height()/37;
    font_s_2=(font_s_3*80)/100;
    font_s_1=(font_s_2*80)/100;
    startAngle = 90 * 16;

    title="SYNC";
    progress=0;
    progress_perc=0;
    progress_req=0;

    setProgress(0);
    resetColors();
    resetStates();


}
void myPushButton::resetColors(){
    color_progress=QColor(255,54,159,255);
    color_error=QColor(255,0,0,255);
    color_idle=QColor(72,50,100,186);
 //   shad_col=QColor(233,44,129,30);
    shad2_col=QColor(255,54,159,150);
    color_noerror=color_idle;
    color_success=QColor(0,255,0,255);
    color_title=QColor(200,200,200,255);
    show_perc=true;

    QRectF prorect(x,subh+suby,w,h-suby-subh);
}
void myPushButton::resetStates(){
    state_idle=color_idle;
    state_success=color_success;
    state_progress=color_progress;
    state_noerror=color_noerror;
    state_error=color_error;
    state_title=color_title;
    pen_title.setColor(state_title);

    pen_pro.setWidth(wdth);
    pen_pro.setColor(state_progress);

    pen_idle.setWidth(wdth);
    pen_idle.setColor(state_idle);

 //   pen_shad.setWidth(wdth*7);
 //   pen_shad.setColor(shad_col);

    pen_shad2.setWidth(wdth);
    pen_shad2.setColor(shad2_col);
    update();
}
void myPushButton::setTitle(QString t){
    title=t;
    update();
}
void myPushButton::set_geo(){
    x=wdth;
    y=wdth;
    w=width()-2*wdth;
    h=height()-2*wdth;
    main_geo.setRect(x,y,w,h);

    suby=h/25;
    subh=h/1.5;
    sub_geo.setRect(x+wdth,y+wdth,w-2*wdth,h-2*wdth);
    titlerect.setRect(x,height()*.35,width()-2*x,height()*.4);
}
void myPushButton::showPerc(bool s){
    show_perc=s;
    update();

}

void myPushButton::resetProgress(){
    double change=0;
    if(progress_req>progress_perc)change=1;
    if(progress_req<progress_perc)change=-1;
    progress_perc+=change;
    progress = (double)progress_perc/100;
    update();
    //   qDebug()<<"PROGRESS:"<<progress<<","<<progress_perc<<","<<progress_req;
    setProgress(progress_req);
    //    qInfo()<<"UPDATING PROGRESS:"<<progress_perc<<",req:"<<progress_req<<",progress:"<<progress;

}
void myPushButton::setProgress(int val){
    if(val!=progress_perc){
        //   qInfo()<<"PROGRESS CHANGE:"<<val<<", req:"<<progress_req<<",perc:"<<progress_perc;
        progress_req=val;
        reset_timer.singleShot(20,this,SLOT(resetProgress()));
    }   else{
        progress_perc=val;
    }
    update();
}
void myPushButton::setState(QColor state,int timeout){
    if(progress_timer.isActive())return;
    state_title=state;
    if(timeout>0)progress_timer.singleShot(timeout,this,SLOT(resetStates()));
    update();
}
void myPushButton::setIdleColor(QColor c){
    state_idle=c;
    update();
}
void myPushButton::setButton(QString txt,int w){
    if(w>-1) wdth=w;
    title=txt;
    update();
}
void myPushButton::paintEvent(QPaintEvent *e)
{
    spanAngle = -progress * 360 * 16;


    QPainter p(this);


    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    p.setPen(pen_idle);
    p.drawArc(sub_geo, startAngle, 360*16);

    //p.setPen(pen_shad);
  //  p.drawArc(ssub_geo, startAngle, 360*16);

    p.setPen(pen_shad2);
    p.drawArc(sub_geo, startAngle, 360*16);

    p.setPen(pen_pro);
    p.drawArc(sub_geo, startAngle, spanAngle);

    p.setPen(pen_title);
    p.setFont(QFont("Ubuntu",font_s_3));
    p.drawText(titlerect,Qt::AlignHCenter | Qt::AlignTop ,title);
    if(show_perc){

        //    qDebug()<<"DEVICE PIXEL RATIO:"<<devicePixelRatio();
        p.setFont(QFont("Ubuntu",font_s_2));
        p.drawText(titlerect,Qt::AlignHCenter | Qt::AlignBottom,QString::number(progress*100)+"%");
    }
    //  myPushButton::paintEvent(e);

}
myPushButton::~myPushButton(){
    qDebug()<<"~myPushButton()";
}
