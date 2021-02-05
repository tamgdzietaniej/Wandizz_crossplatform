#include "gllabel.h"

gLabel::gLabel(QWidget* parent,QString o, QString t):QOpenGLWidget(parent),
    fresh_run(true),
    loaded(false),
    offs(0),
    is_poster(false),
    debg(false),
    is_gesture(0),
    type(o),
    need_update(false),
    need_show(false),
    des_opacity(1),
    fav_toggled(0),
    nde(-1),
    locked(true),
    ctype(t),
    sent1(false),
    sent2(false),
    anim_active(false)
{
    limage=new QImage();
    setStyleSheet("border:none;");
    fav[0]=nullptr;
    fav[1]=nullptr;
    setMouseTracking(false);
    setTabletTracking(true);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_AlwaysStackOnTop,true);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_DeleteOnClose,true);
    //    setAttribute(Qt::WA_Hover);
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    // connect(&timer,SIGNAL(timeout()),this,SIGNAL(chk_timer()),Qt::QueuedConnection);
    connect(&timer,SIGNAL(timeout()),this,SLOT(check_timer()),Qt::QueuedConnection);
    connect(this,SIGNAL(chk_timer()),this,SLOT(check_timer()),Qt::QueuedConnection);
    setMouseTracking(true);
}
void gLabel::initializeGL(){
    glClearColor(0,0,0,0);
}

/* EVENTS */

/*
void gLabel::hideEvent(QHideEvent *e){
    qDebug()<<"HIDE BRCK:"<<index<<txt_text<<tim_text<<type<<ctype;
    e->accept();
}
void gLabel::showEvent(QShowEvent *e){
    qDebug()<<"SHOW BRCK:"<<index<<txt_text<<tim_text<<type<<ctype;
    if(isLocked()){
        e->setAccepted(false);
        return;
    }
    e->accept();
    check_timer();
}
*/
void gLabel::unlock(){
    qDebug()<<"UNLOCK:"<<index<<" visible:"<<isVisible()<<" size:"<<size();
    locked=false;
    need_show=true;;
    op_sc=1;
    offs=0;
    check_timer();
}
void gLabel::lock(){
    need_update=false;
    need_show=false;
    locked=true;
    hide();
}
void gLabel::mousePressEvent(QMouseEvent *event){
    if(locked){
        return;
    }
    press_pos=event->pos().x();
    prev_pos=press_pos;
    pressed=true;
    qDebug()<<"HOVER:"<<index<<db_index<<txt_text;
    emit hover(index,db_index);
    qDebug()<<"GLABELHOVER:"<<index<<db_index;
    event->setAccepted(false);
}
void gLabel::mouseReleaseEvent(QMouseEvent *event){
    pressed=false;
    is_gesture=false;
    if(locked){
        return;
    }
    hcatapult=1;
    chkOffset();
    event->setAccepted(false);
}
void gLabel::mouseMoveEvent(QMouseEvent *e){
    if(locked)return;
    if(is_gesture && (!isVideos() && !isProducer())){
        e->setAccepted(true);
        diff=prev_pos-e->pos().x();
        int old_offs=offs;
        offs=press_pos-e->pos().x();
        prev_pos=e->pos().x();
        if(offs<0)offs=0;
        if(old_offs != offs){
            update();
        }
    } else e->setAccepted(false);
}
void gLabel::resizeEvent(QResizeEvent*e){
    resizeGL(e->size().width()*dpi,e->size().height()*dpi);
    update();
}
void gLabel::revalidate(){
    qDebug()<<"GLAB:"<<index<<"REVAL";
    sent1=false;
    sent2=false;
    loaded=false;
    is_poster=false;
    qDebug()<<"REVALIDATE:"<<index;
    if(!timer.isActive())check_timer();
}

/* MAIN TIMER EVENTLOOP */
void gLabel::check_timer(){
    if(locked && isVisible()){
        qDebug()<<"Widget locked"<<index;
        return;
    }

    if(nde==-1){
        /* NODE HOOK FOR DATA - CHECK IN LIST */
        //     qDebug()<<"WIDGET"<<index<<"not yet loaded";
        for(int i=0;i<poster_data_ready->count();i++){
            if(poster_data_ready->at(i).file==filename){
                nde=i;
            }
        }
    }
    if(!loaded && !is_poster && nde!=-1){
        /* HAS NODE AND NEED DATA */
        limage=poster_data_ready->at(nde).poster;
        limage->setDevicePixelRatio(dpi);
        is_poster=true;
        if(!locked)need_show=true;
        else loaded=true;
    }
    if ( need_show || need_update || chkOffset()){
        /* UPDATE VIEW */
        if(!isVisible() && need_show){
            qDebug()<<"Widget:"<<index<<isVisible()<<locked;
            setVisible(true);
            need_show=!isVisible();
        }
        if(need_show){
            qDebug()<<"Widget:"<<index<<"stage2";
            QApplication::processEvents();
            need_show=!isVisible();
        }
        if(need_show){
            qDebug()<<"Widget:"<<index<<"stage3"<<mapToGlobal(QPoint(0,0))<<updatesEnabled()<<isEnabled();
            show();
            raise();
            repaint();
            QApplication::processEvents();
            need_show=!isVisible();
        }

        update();
        if(need_update){
            update();
        }

    }  else {
        if(loaded && timer.isActive() && !pressed && offs==0 && !need_show && !need_update){
            /* IDLE STATE */
            timer.stop();
        } else {
            if(!timer.isActive())timer.start(17);
        }
    }
}

/* PAINTER */
void gLabel::paintGL(){
    QPainter painter(this);
    painter.beginNativePainting();
    glClear(GL_COLOR_BUFFER_BIT);
    painter.endNativePainting();
    if(locked)return;
    /* PAINT WHEN IN VIEVPORT ONLY */
    QPoint p=mapToGlobal(QPoint(0,0));
    /* OSD TOGGLES */
    bool shs=((!isProducerType() && !isVideosType()) || (par->prospect_shares && isProducerType() &&  !isProducer() && !isVideos()));
    bool shv=((isVideos() && isVideosType()) || /* videos widget */
                (isVideos() && isProducerType() && /* prospect video widget  AND ->*/
                 (( par->prospect_osd && fav_toggled ) ||/*-> show ony marked as FAV */
                  par->prospect_favs)) ); /*-> or show toggler too */
    rcol=0;
    if(isVideos() || isProducer())offs=0;

    /* H-SWIPE */
    if(offs>0){
        op_sc=(qreal(width())-qreal(abs(offs)))/qreal(width())*1.2;
        if(abs(offs)>width())    // brick moved enough distance left
        {
            if(pressed){
                rcol=1-(qreal(width()-offs)/qreal(width()/2));
            } else {
                hide();
                emit del_fav(type,db_index);
                fav_toggled=false;
                lock();
                offs=0;
                return;
            }
        }
    }
    /* H-SWIPE OPACITY */
    if(op_sc<.8)
        painter.setOpacity(op_sc+.2);
 //   else {
    //    if(painter.opacity()<.4){
     //       painter.setOpacity(0.4);
      //  }
  //  }
    /* MAIN CONTENT */
    if(is_poster)
        loaded=true;
    if(loaded){
        /* FINAL CONTENT */
        painter.translate(-offs,0);
        painter.drawImage(0,0,*limage);
    } else {
        /* SPLASH */
        painter.save();
        painter.drawImage(width()/2-timage->width()/2,height()/2-timage->height()/2,*timage);
        painter.setBrush(QColor(0,0,0,100));
        painter.setPen(QColor(0,0,0,200));
        painter.drawRoundedRect(0,0,width(),height()*0.97,15,15);
        painter.restore();
    }
    if(shv){
        /* SHOW FAV BUTTON */
        if(fav[fav_toggled]!=nullptr){
            painter.drawPixmap(add_fav_rect,*fav[fav_toggled]);
        }
    }
    if(shs){
        /* SHOW SHARE BUTTON */
        painter.drawPixmap(add_fav_rect,*sha_pix);
    }
    painter.end();
    if(chkOffset())update();
    else
        start_bg_proc(need_show || need_update);
    qDebug()<<"GLWID:PAINTED";
}

/* STATES CHECK */
bool gLabel::isProducerType(){
    return ctype=="prospect";
}
bool gLabel::isProducer(){
    return type=="prospect";
}
bool gLabel::isVideosType(){
    return ctype=="videos";
}
bool gLabel::isVideos(){
    return type=="videos";
}
bool gLabel::isFVideos(){
    return type=="fav_videos";
}
bool gLabel::isFScenes(){
    return type=="fav_scenes";
}
bool gLabel::isFItems(){
    return type=="fav_items";
}
bool gLabel::chkOffset(){
    if(offs!=0 && !pressed){
        qDebug()<<"CAT"<<op_sc<<offs;
        if(op_sc<.5)
        {
            offs*=1.05;
        }
        else
        {
           offs*=0.95;
        }
    }
    return offs!=0;
}
bool gLabel::isLocked(){
    return locked;
}

/* STATES SET */
void gLabel::setIndex(int i){
    index=i;
}
bool gLabel::toggle_fav(bool sql){
    fav_toggled=!fav_toggled;
    if(!locked)update();
    if(sql)emit toggle_fav_video(title_id,fav_toggled);
    return fav_toggled;
}
void gLabel::start_bg_proc(bool ns){
    if(ns)need_show=ns;
    if(!timer.isActive()){
        check_timer();
    }
}
void gLabel::forceUpdate(){
    need_update=true;
    start_bg_proc();
}
/* PARAMS SET / INPUT */
void gLabel::setOpacity(qreal o){
    des_opacity=o;
    need_update=true;
    start_bg_proc();
}
void gLabel::setDPI(int d){
    dpi=d;
}
void gLabel::setSilent(int id,const QString& to1,const QString& to2){
    index=id;
    type=to1;
    ctype=to2;
    //  qDebug()<<"SETTING BRICk:"<<index<<" ctxts:"<<type<<ctype;
}
void gLabel::set_params(int i, int db_ind,int itcnt,int tid,QString fn,QString url,QString up, QString dn,QImage* tim,QString nf,int od){
    sent1=false;
    sent2=false;
    loaded=false;
    is_poster=false;
    oid=od;
    title_id=tid;
    items_cnt=itcnt;
    nde=-1;
    offs =0;
    netfile=nf;
    timage=tim;
    txt_text=up;
    tim_text=dn;
    op_sc=1;
    index=i;
    hcatapult=0;
    db_index=db_ind;
    filename=fn;
    if(url!=fileurl) emit get_poster(index, type, url, fn, up, dn, ctype,itcnt);
    fileurl=url;
    set_items();


    PosterData _tmpdata;
}

void gLabel::set_prospect_params(Prospect &p){
    par=&p;
}
void gLabel::set_items(){
    double fs=.2,mrg=height()*fs*20/100;
    add_fav_rect.setRect(width()-height()*fs-mrg,height()-height()*fs-mrg,height()*fs-mrg,height()*fs-mrg);
    fav[1]=new QPixmap(QPixmap(":/gui/ICONS_FAVS/fav5.png").scaled(add_fav_rect.size()*dpi*2,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    fav[0]=new QPixmap(QPixmap(":/gui/ICONS_FAVS/fav5grays.png").scaled(add_fav_rect.size()*dpi*2,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    sha_pix=new QPixmap(QPixmap(":/gui/SHARE ICONS/share.png").scaled(add_fav_rect.size()*dpi*1.6,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    fav[0]->setDevicePixelRatio(dpi);
    fav[1]->setDevicePixelRatio(dpi);
    sha_pix->setDevicePixelRatio(dpi);
}

/* DESTRUCTOR */
gLabel::~gLabel(){
    timer.stop();
    future.waitForFinished();
    if(fav[0]!=nullptr)delete fav[0];
    if(fav[1]!=nullptr) delete fav[1];
    if(sha_pix!=nullptr)delete sha_pix;
    qDebug()<<"BRICK DESTR:"<<index<<type;
}
