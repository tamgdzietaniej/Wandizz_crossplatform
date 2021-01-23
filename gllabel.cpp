#include "gllabel.h"

gLabel::gLabel(QWidget* parent,QString o, QString t):QOpenGLWidget(parent),
    fresh_run(true),
    loaded(false),
    offs(0),
    is_poster(false),
    debg(false),
    type(o),
    need_update(false),
    des_opacity(1),
    fav_toggled(0),
    locked(true),
    ctype(t),
    sent1(false),
    sent2(false),
    anim_active(false)
{
    setStyleSheet("border:none;");
    fav[0]=nullptr;
    fav[1]=nullptr;
    setMouseTracking(false);
    setTabletTracking(true);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_AlwaysStackOnTop,true);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_DeleteOnClose,true);
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    connect(&timer,SIGNAL(timeout()),this,SIGNAL(chk_timer()),Qt::QueuedConnection);
    connect(this,SIGNAL(chk_timer()),this,SLOT(check_timer()),Qt::QueuedConnection);
}
void gLabel::initializeGL(){
    glClearColor(0,0,0,0);
}

/* EVENTS */
void gLabel::leaveEvent(QEvent*){
    emit leaved();
}
void gLabel::mouseMoveEvent(QMouseEvent *e){
    if(locked)return;
    if(isVideos() && isProducer())event_owner=false;
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
    if(debg)qDebug()<<"HIDE BRCK:"<<index<<txt_text<<tim_text<<type<<ctype;
    e->accept();
}
void gLabel::showEvent(QShowEvent *e){
    if(debg)qDebug()<<"SHOW BRCK:"<<index<<txt_text<<tim_text<<type<<ctype;
    if(isLocked()){
        e->setAccepted(false);
        return;
    }
    e->accept();
    check_timer();
}
void gLabel::mousePressEvent(QMouseEvent *event){
    if(locked){
        event->setAccepted(false);
        return;
    }
    event->setAccepted(false);
    press_pos=event->pos().x();
    prev_pos=press_pos;
    pressed=true;
    emit hover(index,db_index);
}
void gLabel::catch_event(){
    event_owner=true;
}
void
gLabel::unlock(){
    locked=false;
  need_update=true;
  op_sc=1;
  offs=0;
  check_timer();
}
void gLabel::lock(){
    locked=true;
    update();
    hide();
}
void gLabel::mouseReleaseEvent(QMouseEvent *event){
    if(locked){
        event->setAccepted(false);
        return;
    }
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
    if(debg)qDebug()<<"GLAB:"<<index<<"REVAL";
    sent1=false;
    sent2=false;
    loaded=false;
    is_poster=false;
    qDebug()<<"REVALIDATE:"<<index;
    if(!timer.isActive())timer.start();
}

/* MAIN TIMER EVENTLOOP */
void gLabel::check_timer(){
    if(locked)return;
    if(debg)qDebug()<<"Brick:"<<index<<ctype<<" check timer loaded locally:"<<sent2<<",("<<QFileInfo::exists(filename)<<"), filename:"<<filename<<" requested:"<<sent1<<QUrl(filename).fileName();
    if(!sent2){
        if(QFileInfo::exists(filename) )
        {
            sent2=true;
            //  emit download( fileurl, filename, index, true );
            emit get_poster(index,type,filename,txt_text,tim_text,ctype,items_cnt);
        } else if(!sent1){
            sent1=true;
            if(QUrl(filename).fileName()!=".png"){
                emit download( fileurl, filename, index ,false);
                timer.setInterval(1000);
            }
        }
    }
    if(need_update)if(debg)qDebug()<<"brick:"<<index<<"wait for forceupdate"<<isVisibleTo(parentWidget());
    if(!locked){
        if ( need_update || chkOffset()){
            locked=false;
            update();
        }
        if(!timer.isActive())
            timer.start(17);
    } else
        if(loaded)
        {
            timer.stop();
        }
}

/* PAINTER */
void gLabel::paintGL(){
    QPainter painter(this);
    painter.beginNativePainting();
    glClear(GL_COLOR_BUFFER_BIT);
    painter.endNativePainting();
    if(locked)return;
    bool shs=((!isProducerType() && !isVideosType()) || (par->prospect_shares && isProducerType() &&  !isProducer() && !isVideos()));
    bool shv=((isVideos() && isVideosType()) || /* videos widget */
              (isVideos() && isProducerType() && /* prospect video widget  AND ->*/
               (( par->prospect_osd && fav_toggled ) ||/*-> show ony marked as FAV */
                par->prospect_favs)) ); /*-> or show toggler too */
    //  qDebug()<<"Show fav:"<<index<<par->prospect_favs<<par->prospect_osd<<fav_toggled<<shs<<shv<<fav_toggled<<isVideos()<<isVideosType()<<type<<ctype;
    qDebug()<<"PAINT:"<<index<<type<<ctype<<locked<<fav_toggled;
    rcol=0;
    if(isVideos() || isProducer())offs=0;
    loaded=is_poster;
    need_update=false;
    if(offs>0){
        op_sc=(qreal(width())-qreal(abs(offs)))/qreal(width());
        if(abs(offs)>width()*0.4)    // brick moved enough distance left
        {
            if(pressed){
                rcol=1-(qreal(width()-offs)/qreal(width()/2));
            } else {
                hide();

                emit del_fav(type,index);
                return;
            }
        }
    }

    if(op_sc<.8)
        painter.setOpacity(op_sc+.2);
    else {
        if(painter.opacity()<.5){
            painter.setOpacity(0.5);
        }
    }
    if(limage!=nullptr){
        painter.drawImage(-offs,0,*limage);
    }
    if(shv){
        if(fav[fav_toggled]!=nullptr){
            painter.drawPixmap(add_fav_rect,*fav[fav_toggled]);
        } //else qDebug()<<par->prospect_favs<<par->prospect_osd<<fav_toggled<<isVideos()<<isVideosType();
    }
    if(shs){
        painter.drawPixmap(add_fav_rect,*sha_pix);
        if(debg)qDebug()<<"WWRAP";
    }
    painter.end();
    if(offs!=0 && !pressed){
        need_update=false;
        check_timer();
    } else need_update=false;
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
    if(hcatapult!=0 && offs!=0)
    {
        if(op_sc<.3)
        {
            hcatapult*=.995;
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
void gLabel::setIndex(int i){
    index=i;
}
bool gLabel::toggle_fav(bool sql){
    fav_toggled=!fav_toggled;
    update();
    if(sql)emit toggle_fav_video(title_id,fav_toggled);
    return fav_toggled;
}
void gLabel::forceUpdate(){
    need_update=true;
    check_timer();
}
/* PARAMS SET / INPUT */
void gLabel::settle_poster(const QImage* im){
    // qDebug()<<type<<ctype<<index;
    limage=const_cast<QImage*>(im);
    //  qDebug()<<"SETTLE:index"<<index<<"dpi"<<dpi<<"geo"<<geometry()<<"imgsize"<<limage->sizeInBytes();
    limage->setDevicePixelRatio(dpi);
    is_poster=true;
    need_update=true;
    if(!timer.isActive())
        check_timer();
    emit settled(index);
}
void gLabel::setOpacity(qreal o){
    des_opacity=o;
    if(!timer.isActive())
        check_timer();
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
void gLabel::set_params(int i, int db_ind,int itcnt,int tid,QString fn,QString url,QString up, QString dn,QImage& tim,QString nf,int od){
    sent1=false;
    sent2=false;
    loaded=false;
    oid=od;
    title_id=tid;
    items_cnt=itcnt;
    offs =0;
    netfile=nf;
    timage=&tim;
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
    add_fav_rect.setRect(width()-height()*fs-mrg,height()-height()*fs-mrg,height()*fs-mrg,height()*fs-mrg);
    fav[1]=new QPixmap(QPixmap(":/gui/ICONS_FAVS/fav5.png").scaled(add_fav_rect.size()*dpi*2,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    fav[0]=new QPixmap(QPixmap(":/gui/ICONS_FAVS/fav5grays.png").scaled(add_fav_rect.size()*dpi*2,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    sha_pix=new QPixmap(QPixmap(":/gui/SHARE ICONS/share.png").scaled(add_fav_rect.size()*dpi*1.6,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    fav[0]->setDevicePixelRatio(dpi);
    fav[1]->setDevicePixelRatio(dpi);
    sha_pix->setDevicePixelRatio(dpi);
    if(debg) qDebug()<<"ADDFAVRECT"<<add_fav_rect<<geometry();
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
