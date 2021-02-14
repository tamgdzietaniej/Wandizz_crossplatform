#include "gllabel.h"

gLabel::gLabel(QWidget* parent,QString o, QString t):QOpenGLWidget(parent),
    fresh_run(true),
    loaded(false),
    offs(0),
    is_poster(false),
    debg(false),
    is_gesture(0),
    reindexing(false),
    need_hide(false),
    type(o),
    need_update(false),
    need_show(false),
    scale(1),
    accept(true),
    fav_toggled(0),
    nde(-1),
    locked(true),
    ctype(t),
    rt(90),
    sent1(false),
    sent2(false),
    anim_active(false)
{
    limage=new QImage();
    //  opacity = new QGraphicsOpacityEffect(this);
    // opacity->setOpacity(.5);
    // setGraphicsEffect(opacity);
    // anim = new QPropertyAnimation(opacity, "opacity",this);
    // anim->setDuration(500);
    // anim->setStartValue(0.5);
    //  anim->setEndValue(1.0);
    setStyleSheet("border:none;");
    fav[0]=nullptr;
    fav[1]=nullptr;
    del=false;
    setTabletTracking(true);
    setUpdatesEnabled(false);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_AlwaysStackOnTop,true);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_DeleteOnClose,true);
    //    setAttribute(Qt::WA_Hover);
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    // connect(&timer,SIGNAL(timeout()),this,SIGNAL(chk_timer()),Qt::QueuedConnection);
    connect(&timer,SIGNAL(timeout()),this,SLOT(check_timer()),Qt::QueuedConnection);
    connect(this,SIGNAL(chk_timer()),this,SLOT(check_timer()),Qt::QueuedConnection);

    // connect(anim,SIGNAL(finished()),this,SLOT(endAnim()));

}
void gLabel::endAnim(){
    // if(need_lock)hide();
    //  locked=need_lock;
    // need_lock=false;
    // need_unlock=false;
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
    /*
    qDebug()<<"unLOCK:"<<index<<rt<<type<<isVisible();
    opacity->setOpacity(0.5);
    anim->setDirection(QPropertyAnimation::Forward);
    des_opacity=0;
    need_lock=false;
    need_unlock=true;
    need_show=true;
    locked=false;
    show();
    if(isVisible())update();
    else qDebug()<<"Brick"<<index<<type<<"not visible";
    op_sc=1;
    des_opacity=0;
    offs=0;
*/
    qDebug()<<"UNLOCK:"<<index;
    setUpdatesEnabled(true);
    setVisible(true);
    locked=false;
    need_hide=false;
    need_show=true;
    need_update=true;
    start_bg_proc(true);
}
void gLabel::setWatcher(QString& ct,QString& cct,QString& ce,QString& nf,int& od,int& c_title_id,int &c,int &c_items_cnt,bool &u){
    over_me_clicked=&c;
    click_update=&u;
    act_type=&ct;
    act_ctype=&cct;
    act_event_to_share=&ce;
    act_oid=&od;
    act_netfile=&nf;
    act_title_id=&c_title_id;
    act_items_cnt=&c_items_cnt;

}
void gLabel::lock(){
    if(locked && !isVisible())return;
    /*
    opacity->setOpacity(1);
    anim->setDirection(QPropertyAnimation::Backward);
    qDebug()<<"NEED LOCK:"<<index<<rt;
    need_lock=true;
    need_unlock=false;
*/
    qDebug()<<"LOCK:"<<index;
    hide();
    locked=true;
    need_hide=true;
    need_show=false;
    setUpdatesEnabled(false);
    //  start_bg_proc();
}
void gLabel::mousePressEvent(QMouseEvent *event){
    if(locked){
        return;
    }
    is_gesture=false;
    press_position=event->pos().x();
    prev_position=0;
    pressed=true;
    qDebug()<<"HOVER:"<<index<<db_index<<txt_text;
    *over_me_clicked=index;
    *click_update=true;
    *act_ctype=ctype;
    *act_type=type;
    *act_event_to_share=event_to_share;
    *act_items_cnt=items_cnt;
    *act_oid=oid;
    *act_netfile=netfile;
    *act_title_id=title_id;  
    *act_oid=oid;
    event->setAccepted(false);
    //  emit mpressed(this,event);
}
void gLabel::relEvent(QMouseEvent *event){
    pressed=false;
    if(locked){
        return;
    }
    hcatapult=1;
    if(offs>width()/3){
        del=true;
    }
    else del=false;
    update();
    event->setAccepted(true);
    is_gesture=false;
}
void gLabel::setGesture(bool g){
    //  setAttribute(Qt::WA_TransparentForMouseEvents,!g);
}

void gLabel::movEvent(QMouseEvent *e){
    if(locked)return;
    //  qDebug()<<"isgestL:<="<<!isVideos()<<!isProducer()<<is_gesture<<offs;
    if(!isVideos() && !isProducer()){
        offs=press_position-e->pos().x();
        if(offs<0)offs=0;
        if(prev_position!=offs) update();
        prev_position=offs;
    }
    //   qDebug()<<"IS:"<<is_gesture<<offs;
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
void gLabel::reindex(int i,QRect togo){
    qDebug()<<"Reindex:from"<<index<<"to"<<i;
    index=i;

   // vrect=togo;
   // if(togo!=QRect())need_update=true;
    update();
}
/* MAIN TIMER EVENTLOOP */
void gLabel::check_timer(){
    if(locked){
        timer.stop();
        qDebug()<<"Widget locked"<<index;
        return;
    }

    if(nde==-1){
        /* NODE HOOK FOR DATA - CHECK IN LIST */
        for(int i=0;i<poster_data_ready->count();i++){
            if(poster_data_ready->at(i).file==filename){
                nde=i;
            }
        }
    }

    if(!loaded && !is_poster && nde!=-1){
        /* HAS NODE AND NEED DATA */
        qDebug()<<"IMAGE:"<<index;
        limage=poster_data_ready->at(nde).poster;
        limage->setDevicePixelRatio(dpi);
        is_poster=true;
        if(!locked)need_show=true;
        else loaded=true;
        need_update=true;
    }
    if ( need_show ){
        /* UPDATE VIEW */
        if(!isVisible() && need_show){
            show();
            QApplication::processEvents();
        }
    }
    if(need_update){
        update();
    }  else
        if(need_hide && isVisible()){
        need_hide=isVisible();
        locked=!isVisible();
        if(locked)qDebug()<<"Itm:"<<index<<"locked";
        hide();
        QApplication::processEvents();
    } else need_hide=false;
    if(loaded && timer.isActive() && !pressed && offs==0 && !need_show && !need_update && !need_unlock && !need_lock && !need_hide){
        /* IDLE STATE */
        timer.stop();
    } else {
        if(!timer.isActive())timer.start(17);
    }

}
/* PAINTER */
void gLabel::paintGL(){
#if defined (Q_OS_ANDROID)
    qDebug()<<"Painting glabel:"<<index<<type<<vrect<<geometry()<<mrb;
#endif
    QPainter painter(this);
    painter.beginNativePainting();
    glClear(GL_COLOR_BUFFER_BIT);
    painter.endNativePainting();
    if(scale!=1){
        scale*=.9;
        if(scale<1)scale=1;
        int ofx=(1-scale)*width()/2;
        int ofy=(1-scale)*height()/2;
        painter.translate(ofx,ofy);
        painter.scale(scale,scale);
    }
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
                qDebug()<<"PAINTGLLOCK:"<<index;
                lock();
                emit del_fav(index,db_index,type);
                return;
            }
        }
    }
    /* H-SWIPE OPACITY */
    if(op_sc<.8)
        painter.setOpacity(op_sc+.2);

    /* SPLASH */
    //if(sw_opacity!=0){
    /* MAIN CONTENT */
    if(is_poster)
        loaded=true;
    if(!loaded){
        painter.save();
        painter.drawImage(width()/2-timage->width()/2,height()/2-timage->height()/2,*timage);
        painter.setBrush(QColor(0,0,0,100));
        painter.setPen(QColor(0,0,0,200));
        painter.drawRoundedRect(0,0,width(),height()*0.97,15,15);
        painter.restore();
    }
    /* FINAL CONTENT */
    if(loaded){
        painter.translate(-offs,0);
        painter.drawImage(0,0,*limage);
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
    }
    painter.end();
    need_update=false;
    need_show=false;
    /*
    if(vrect.y()!=y()){
        int m=0;
        if(vrect.y()-y()<10)
            m=vrect.y()-y();
        else
            m=(vrect.y()-y())/10;
        qDebug()<<"Moving:"<<index<<"by"<<m;
        move(0,y()+m);
        reindexing=true;
    } else reindexing=false;
*/
    if( chkOffset() || scale!=1 || reindexing)update();
    //  qDebug()<<"GLWID:PAINTED";
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
void gLabel::setScaled(bool s){
    if(s)
        scale=1.3;
    //   scale=1;
    else
        scale=1;
}
bool gLabel::chkOffset(){
    if(offs!=0 && !pressed){
        if(del)
        {
            offs*=1.3;
        }
        else
        {
            offs*=0.9;
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
    qDebug()<<"TOGGLE:"<<fav_toggled<<index<<txt_text<<locked;
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
void gLabel::setDPI(int d){
    dpi=d;
}
void gLabel::setSilent(int id,const QString& to1,const QString& to2){
    index=id;
    type=to1;
    ctype=to2;
}
void gLabel::set_params(int i, int db_ind,int itcnt,int tid,QString fn,QString url,QString up, QString dn,QImage* tim,QString nf,int od){
    reindex(i);
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
    hcatapult=0;
    db_index=db_ind;
    filename=fn;
    if(url!=fileurl) emit get_poster(index, type, url, fn, up, dn, ctype,itcnt);
    fileurl=url;
    set_items();
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
