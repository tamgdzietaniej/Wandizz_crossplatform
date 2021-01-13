#include "gllabel.h"

gLabel::gLabel(QWidget* parent,QString o, QString t):QOpenGLWidget(parent),
    fresh_run(true),
    loaded(false),
    offs(0),
    is_poster(false),
    debg(false),
    type(o),
    need_switch(false),
    need_hide(true),
    need_update(false),
    des_opacity(1),
    fav_toggled(0),
    locked(true),
    ctype(t),
    sent1(false),
    sent2(false),
    anim_active(false),
    stim(200)
{
    setStyleSheet("border:none;");
    fav[0]=nullptr;
    fav[1]=nullptr;
    setMouseTracking(false);
    setTabletTracking(true);
    setAttribute(Qt::WA_AlwaysStackOnTop,true);
    setAttribute(Qt::WA_DeleteOnClose,true);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    connect(&timer,SIGNAL(timeout()),this,SIGNAL(chk_timer()),Qt::QueuedConnection);
    connect(this,SIGNAL(chk_timer()),this,SLOT(check_timer()),Qt::QueuedConnection);
}

/* EVENTS */
void gLabel::leaveEvent(QEvent*){
    emit leaved();
}
void gLabel::mouseMoveEvent(QMouseEvent *e){
    if(locked)return;
    if(type=="videos" && type=="prospect")event_owner=false;
    e->setAccepted(event_owner);
    if(!event_owner )return;
    diff=prev_pos-e->pos().x();
    int old_offs=offs;
    offs=press_pos-e->pos().x();
    prev_pos=e->pos().x();
    if(offs<0)offs=0;
    if(old_offs != offs){
        update();
    }
}
void gLabel::hideEvent(QHideEvent *e){
    qDebug()<<"Hide brick:"<<index;
    if(!isLocked())lock();
    e->accept();
    emit brick_hidden_done(index);
}
void gLabel::mousePressEvent(QMouseEvent *event){
    event->setAccepted(false);
    if(locked)return;
    press_pos=event->pos().x();
    prev_pos=press_pos;
    pressed=true;
    emit hover(index,db_index);
}
void gLabel::catch_event(){
    //   if(debg)qDebug()<<"GLA:catchevent:"<<index<<type<<"locked:"<<locked;
    event_owner=true;
}
void gLabel::mouseReleaseEvent(QMouseEvent *event){
    event->setAccepted(event_owner);
    if(locked || !event_owner){
        return;
    }
    event_owner=false;
    pressed=false;
    if(op_sc<.3){
        hcatapult=15;
    } else {
        hcatapult=diff;
    }
    if(hcatapult!=0 || offs!=0)update();
}
void gLabel::resizeEvent(QResizeEvent*e){
    resizeGL(e->size().width()*dpi,e->size().height()*dpi);
    update();
}
void gLabel::revalidate(){
    qDebug()<<"GLAB:"<<index<<"REVAL";
    if(!timer.isActive())timer.start();
}

/* MAIN TIMER EVENTLOOP */
void gLabel::check_timer(){
    if(debg)qDebug()<<"Brick:"<<index<<" check timer loaded locally:"<<sent2<<",("<<QFileInfo::exists(filename)<<"), filename:"<<filename<<" requested:"<<sent1<<QUrl(filename).fileName();
    if(!sent2){
        if(QFileInfo::exists(filename) )
        {
            if(debg)qDebug()<<"Brick:"<<index<<"Loading from cache"<<index<<type<<QFileInfo(filename).lastModified();
            //  timer.stop();
            sent2=true;
            //  emit download( fileurl, filename, index, true );
            emit get_poster(index,1,type,filename,txt_text,tim_text,ctype,items_cnt);
            return;
        }
        else
            if(!sent1)
            {
                if(debg)qDebug()<<"Brick:"<<index<<"Loading from network:"<<fileurl;
                //       timer.stop();
                sent1=true;
                if(QUrl(filename).fileName()!=".png"){
                    qDebug()<<"EMIT DOWNLOAD:"<<fileurl<<filename;
                    emit download( fileurl, filename, index ,false);
                }
                return;
            }
    }
    if ( need_switch || need_update || chkOffset()){
        if(!timer.isActive())
            timer.start(17);
        if(!isVisible())show();
        update();
    } else
        if(loaded)
        {
            //emit download( fileurl, filename,index );
            timer.stop();
        }

}

/* PAINTER */
void gLabel::paintGL(){
    if(debg)qDebug()<<"BRICK"<<index<<mapToGlobal(QPoint(0,0));
    rcol=0;
    if(type=="videos" || type=="prospect")offs=0;
    if(!is_poster || isLocked()){
        return;
    }
    if(need_switch){
        loaded=final_poster;
        need_switch=false;
    }
    if(offs>0){
        op_sc=(qreal(width())-qreal(abs(offs)))/qreal(width());
        if(abs(offs)>width()*0.5)    // brick moved enough distance left
        {
            if(pressed){
                rcol=1-(qreal(width()-offs)/qreal(width()/2));
            } else {
                hide();
                emit click_delete(index);
                return;
            }
        }
    }
    QPainter painter;
    painter.begin(this);
    if(op_sc<.8)
        painter.setOpacity(op_sc+.2);
    else {
        if(painter.opacity()<.5){
            painter.setOpacity(0.5);
        }
    }
    if(limage!=nullptr){
        qDebug()<<"type:"<<type;
        painter.drawPixmap(-offs,0,*limage);
    }
    bool shv=(ctype!="prospect");
    if(!shv){
        if(ctype == "prospect"){
            if((par->prospect_osd && fav_toggled) || (par->prospect_favs)){
                shv=true;
            }
        }
    }
    bool shs=(ctype!="prospect" && ctype!="videos");
    if(!shs){
        if(ctype == "prospect"){
            if(par->prospect_shares){
                shs=true;
            }
        }
    }
    if(shv){
        if(fav[fav_toggled]!=nullptr){
            painter.drawPixmap(add_fav_rect,*fav[fav_toggled]);
        }
    }
    if(shs){
        painter.drawPixmap(add_fav_rect,*sha_pix);
        if(debg)qDebug()<<"WWRAP";
    }
    painter.end();
    if(offs && !pressed){
        if(!timer.isActive())check_timer();
    }
    need_update=false;
}

/* STATES CHECK */
bool gLabel::isProducer(){
    return type=="prospect";
}
bool gLabel::chkOffset(){
    if(hcatapult!=0 && offs!=0)
    {
        if(op_sc<.3)
        {
            hcatapult*=.99;
            if(abs(hcatapult)<2)
                hcatapult=0;
            else
                hcatapult*=2;
            offs+=hcatapult;
        }
        else
        {
            offs=offs*0.7;
            if(offs<1)
                offs=0;
        }
    }
    return offs!=0;
}
bool gLabel::isLocked(){
    return locked;
}

/* STATES SET */
void gLabel::toggle(bool t){
    if(t)unlock();
    else lock(true);
}
void gLabel::unlock(){
    if(locked || !isVisible() || !isEnabled()){
        qDebug()<<"Brick unlock:"<<type<<ctype<<geometry();
        need_hide=false;
        setDisabled(false);
        locked=false;
        need_update=true;
        if(!isVisible())show();
        if(!timer.isActive())
            check_timer();
    }
}
void gLabel::lock(bool h){
    if(isVisible())setVisible(!h);
    // setUpdatesEnabled(!h);
    need_hide=h;
    locked=true;
    timer.stop();
}
bool gLabel::toggle_fav(){
    if(fav_toggled){
        unmark_fav_videos(true);
        emit del_fav_videos(db_index,type);
    } else {
        mark_fav_videos(true);
        emit add_fav_videos(db_index);
    }
    return fav_toggled;
}
void gLabel::mark_fav_videos(bool){
    fav_toggled=true;;
    update();
}
void gLabel::unmark_fav_videos(bool){
    fav_toggled=false;
    update();
}

/* PARAMS SET / INPUT */
void gLabel::settle_poster(QPixmap* im,bool tp)
{
    if(debg)qDebug()<<"SETTLE:loaded:"<<loaded<<tp;
    if(loaded && !tp){
        need_switch=false;
        return;
    }
    final_poster=tp;
    limage=im;
    is_poster=true;
    need_switch=true;
    emit chk_timer();
}
void gLabel::setOpacity(qreal o){
    des_opacity=o;
    if(!timer.isActive())
        check_timer();
}
void gLabel::setDPI(int d){
    dpi=d;
}
void gLabel::setContext(const QString &cont){
    ctype=cont;
}
void gLabel::set_params(int i, int db_ind,int itcnt,int tid,QString fn,QString url,QString up,
                        QString dn, QString nf,int od){
    sent1=false;
    sent2=false;
    loaded=false;
    oid=od;
    title_id=tid;
    items_cnt=itcnt;
    offs =0;
    netfile=nf;
    txt_text=up;
    tim_text=dn;
    offs=0;
    fileurl=url;
    op_sc=1;
    index=i;
    hcatapult=0;
    db_index=db_ind;
    filename=fn;
    set_items();
}
void gLabel::set_prospect_params(Prospect &p){
    par=&p;
}
void gLabel::set_items(){
    double fs=.2,mrg=height()*fs*20/100;
    add_fav_rect.setRect(width()-height()*fs-mrg,height()-height()*fs-mrg,height()*fs,height()*fs);
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
    if(debg)qDebug()<<"BRICK DESTR:"<<index<<type;
}
