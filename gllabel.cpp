#include "gllabel.h"

gLabel::gLabel(QWidget* parent,QString o):QOpenGLWidget(parent),
    loaded(false),
    des_opacity(1),
    is_poster(false),
    need_switch(false),
    type(o),
    need_update(false),
    to_hide(false),
    stim(200),
    debg(true)
{
    mainh=main_h-(main_h/2-ms/2);
    setStyleSheet("border:none;");
#if defined (Q_OS_IOS)
    dpi = devicePixelRatio();
#else
    dpi=devicePixelRatio();
#endif
    anim_act=false;
    nv=0;
    under=false;
    anim_active=false;
    need_hide=false;
    sent1=false;
    sent2=false;
    timer_proc=false;
    emitted_once=false;
    setMouseTracking(false);
    setTabletTracking(true);
    setAttribute(Qt::WA_AlwaysStackOnTop,true);
    setAttribute(Qt::WA_DeleteOnClose,true);
    connect(&timer,SIGNAL(timeout()),this,SLOT(check_timer()));
    setUpdateBehavior(NoPartialUpdate);
    QPalette palette;
    lpix=new QLabel(this);
    lpix->setAttribute(Qt::WA_DontShowOnScreen);
    lpix->hide();
    lpix->setScaledContents(false);
    lpix->setStyleSheet("background:transparent;border:none;");
    set_style();
}
void gLabel::initializeGL(){
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void gLabel::leaveEvent(QEvent *e){
    emit leaved();
}
void gLabel::mouseMoveEvent(QMouseEvent *e){
    if(locked)return;
    if(type=="videos")event_owner=false;
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
void gLabel::showEvent(QShowEvent* e){
    if(need_hide){
        qDebug()<<index<<type<<" ignoring show";
        e->ignore();
        return;
    }
    if(!sent2 || !sent1){
        if(debg)qDebug()<<"Reactivating timer at brick:"<<index;
        startTimer(1000);
    }
    if(debg)qDebug()<<"Show brick:"<<index;
    e->accept();
}
void gLabel::hideEvent(QHideEvent *e){
    if(timer.isActive()){
        if(debg)qDebug()<<"Brick "<<index<<" pausing timer";
        timer.stop();
    }
    e->accept();
}
void gLabel::mousePressEvent(QMouseEvent *event){
    event->setAccepted(false);
    if(locked)return;
    emit hover(index,db_index);
    press_pos=event->pos().x();
    prev_pos=press_pos;
    pressed=true;
}
void gLabel::catch_event(){
    //   qDebug()<<"GLA:catchevent:"<<index<<type<<"locked:"<<locked;
    event_owner=true;
}
void gLabel::mouseReleaseEvent(QMouseEvent *event){
    event->setAccepted(event_owner);
    if(locked || !event_owner)return;
    event_owner=false;
    pressed=false;
    if(op_sc<.3){
        hcatapult=15;
    } else {
        hcatapult=diff;
    }
    if(hcatapult!=0 || offs!=0)update();
}
void gLabel::check_timer(){
    if(debg)qDebug()<<"Brick:"<<index<<" check timer loaded locally:"<<sent2<<",("<<QFileInfo::exists(filename)<<"), filename:"<<filename<<" requested:"<<sent1<<QUrl(filename).fileName();
    if(!sent2){
        if(QFileInfo::exists(filename) )
        {
            if(debg)qDebug()<<"Brick:"<<index<<"Loading from cache"<<index<<type;
            timer.stop();
            sent2=true;
            emit get_poster(index,1,type,filename,txt_text,tim_text);
            timer_proc=false;
            return;
        }
        else
            if(!sent1)
            {
                if(debg)qDebug()<<"Brick:"<<index<<"Loading from network:"<<fileurl;
                timer.stop();
                sent1=true;
                emit get_poster(index,0,type,"",txt_text,tim_text);
                if(QUrl(filename).fileName()!=".png"){
                    emit download( fileurl, filename );
                }
                timer_proc=false;
                return;
            }
    }
    if(loaded && !need_switch && !need_update)
    {
        timer.stop();
    }
    else
        if ( need_switch || need_update )
        {
            timer.start(500);
            //      qDebug()<<"NEED RE-SWITCH"<<index;
            if(!isVisible())show();
            update();
            //     QApplication::processEvents();
        }
    timer_proc=false;
}


void gLabel::set_params(int i, int db_ind,qreal mrg,int mh,QString fn,QString url,QString up,QString dn, QOpenGLWidget* m_first,QString nf){
    marker_first=m_first;
    mrgh=mrg;
    sent1=false;
    sent2=false;
    sharing=false;
    brick_act_shift=0;
    sh_del =false;
    sh_sha =false;
    divider =10;
    main_h=mh;
    offs =0;
    item ="";
    loaded=false;
    netfile=nf;
    //  main_s=qApp->primaryScreen()->availableVirtualSize();
    main_s=size();
    ms=(main_h/3)*2;
    txt_text=up;
    offs=0;
    tim_text=dn;
    fileurl=url;
    op_sc=1;
    index=i;
    hcatapult=0;
    db_index=db_ind;
    filename=fn;
    updGeo();
    set_items();
    if(type=="fav_items")aspect_ratio=Qt::KeepAspectRatio;
    else aspect_ratio=Qt::IgnoreAspectRatio;
    where_to_share_list.clear();
    if(type=="videos"){
        tim_align=Qt::AlignRight;
    } else {
        tim_align=Qt::AlignCenter;
    }
    lpix->setGeometry(this->geometry().adjusted(0,0,0,lh-main_h));
    //  lpix->resize(lpix->size()*dpi);
}
void gLabel::lock(){
    // if(!locked){
    emitted_once=false;
    timer.stop();
    need_hide=true;
    setVisible(false);
    //    setDisabled(true);
    //    setUpdatesEnabled(false);
    locked=true;
    //  }

}
void gLabel::unlock(){
    if(type=="videos"){
        if(fav_toggled)mark_fav_videos(false);
        else unmark_fav_videos(false);
    }
    // if(locked){
    need_hide=false;
    setDisabled(false);
    setUpdatesEnabled(true);
    locked=false;
    //     qDebug()<<"UNLOCK:"<<index<<type<<isVisible()<<is_poster<<need_hide<<geometry();
    need_hide=false;
    if(!isVisible())show();
    //  }
    if(!timer.isActive())
        check_timer();
}
bool gLabel::chkOffset(){
    //    qDebug()<<"chk:"<<index<<hcatapult<<offs;
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
void gLabel::resizeEvent(QResizeEvent*e){
    resizeGL(e->size().width()*dpi,e->size().height()*dpi);
    pdev.setSize(e->size()*dpi);
}
void gLabel::paintGL(){
    rcol=0;
    if(type=="videos")offs=0;
    if(!is_poster){
        return;
    }
    QPixmap* fav_pict=favpix;
    QPoint add_fav_pos((add_fav_rect.adjusted(-offs,0,0,0)).topLeft());
    QRect srct(rect());
    srct.setHeight(srct.height()/1.03);
    op_sc=(qreal(lw)-qreal(abs(offs)))/qreal(lw);
    if(need_switch){
        need_switch=false;
        loaded=final_poster;
    }
    if(abs(offs)>width()*0.5)    // brick moved enough distance left
    {
        if(pressed){
            rcol=1-(qreal(width()-offs)/qreal(width()/2));
        } else {
            hide();
            prep_remove=true;
            emit click_delete(index,db_index);
            return;
        }
    }
    QPainter painter;
    painter.begin(this);
    painter.beginNativePainting();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    painter.endNativePainting();
    if(op_sc<.8)
        painter.setOpacity(op_sc+.2);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::Qt4CompatiblePainting);
    if(rcol!=0){
        painter.beginNativePainting();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        painter.endNativePainting();
    }
    painter.drawPixmap(QRect(-offs,0,width()-offs,height()+lh-main_h),*lpix->pixmap(),lpix->pixmap()->rect());
    //     painter.drawPixmap(-offs,0,*lpixmap);
    if(type=="videos"){
        painter.drawPixmap(add_fav_rect,*fav_pict);
    } else {
        if(painter.opacity()>.5)
            painter.setOpacity(0.5);
        painter.drawPixmap(add_fav_pos,*sha_pix);
    }
    painter.end();
    if(op_sc!=1 && !pressed){
        chkOffset();
        update();
    }
    need_update=false;
}
void gLabel::setOpacity(qreal o){
    des_opacity=o;
    if(!timer.isActive())
        check_timer();
}
void gLabel::set_items(){
    fav_on=new QPixmap(QPixmap(":/gui/ICONS_FAVS/fav5.png").scaled(add_fav_rect.size()*devicePixelRatio(),Qt::IgnoreAspectRatio,Qt::FastTransformation));
    fav_off=new QPixmap(QPixmap(":/gui/ICONS_FAVS/fav5grays.png").scaled(add_fav_rect.size()*devicePixelRatio(),Qt::IgnoreAspectRatio,Qt::FastTransformation));
    sha_pix=new QPixmap(QPixmap(":/gui/SHARE ICONS/share.png").scaled(add_fav_rect.size()*devicePixelRatio()*.8,Qt::IgnoreAspectRatio,Qt::FastTransformation));
    fav_on->setDevicePixelRatio(devicePixelRatio());
    fav_off->setDevicePixelRatio(devicePixelRatioF());
    sha_pix->setDevicePixelRatio(devicePixelRatioF());
}
void gLabel::set_style(){
    txt_font.setBold(false);
    txt_font.setPixelSize(16.0);
    txt_font.setFamily("Apple Gothic");
    txt_font.setStyleStrategy(QFont::StyleStrategy::PreferQuality);
    tim_font.setStyleStrategy(QFont::StyleStrategy::PreferQuality);
    tim_font.setPointSizeF(15.0);
}
void gLabel::updGeo(){
    sdur=200;
    hdur=30;
    qreal w=main_s.width();
    qreal h=main_h/1.03;
    if(w!=0.0 && h!=0.0){
        lh=qreal(h);
        lw=(1-mrgh*2)*w;
        lm=w/1000;
    } else
        return;
    type=="fav_items" ? _prop=1 : _prop=1.75;
    _lw=lh*_prop;
    int ih=rect().height();
    int iw=rect().width();
    int vtm=0.05*ih;
    int htm=0.01*iw;
    int _lw2=lw-_lw-(2*htm);
    txt_rect.setRect((_lw+htm),vtm,_lw2,ih*.72);
    int fl=height()-txt_rect.height()-vtm;
    tim_rect.setRect(txt_rect.left(), txt_rect.bottom(), (txt_rect.width()-fl-htm),fl);
    add_fav_rect.setRect(width()-fl,txt_rect.bottom(),(fl-vtm),(fl-vtm));
    add_fav_anim_rect=QRect(add_fav_rect.center(),QSize(1,1));
    del_fav_rect=add_fav_rect;
    del_fav_rect.setLeft(del_fav_rect.left()-add_fav_rect.width());
    del_fav_rect.setTop(del_fav_rect.top()-add_fav_rect.height());
    picrect.setRect(0,0,_lw,lh);
}
QRect gLabel::actRect(int i,qreal scale){
    if(i==-1)i=index;
    int w=(main_s.width()-2*mrgh*main_s.width())*scale;
    int h=main_h*scale;
    int x=((main_s.width()-w)/2);
    int y=marker_first->y() + i * main_h +(main_h-h)/2;
    QRect at(x,y,w,h);
    //   qDebug()<<"ACTRECT:"<<at<<"("<<geometry()<<")";
    return  at;
}

void gLabel::setAnimRect(){
    qreal shrink=0.1;
    qreal bf=2;
    int anx,any,anw,anh;
    actRect().getRect(&anx,&any,&anw,&anh);
    qreal shrw=anw*shrink;
    qreal shrh=anh*shrink;
    anrect_smaller=QRect(anx+shrw, any+shrh, anw-2*shrw, anh-2*shrh);
    anrect_bigger=anrect.adjusted(-anw*bf,-anh*bf,2*anw*bf,2*anh*bf);
    anrect_hor=QRect(anrect.left(),anrect.center().y(),anrect.width(),2);
    anrect_ver=QRect(anrect.center().x(),anrect.y(),2,anrect.height());
}
bool gLabel::toggle_fav(){
    if(fav_toggled){
        unmark_fav_videos();
        emit del_fav_videos(db_index,type);
    } else {
        mark_fav_videos();
        emit add_fav_videos(db_index);
    }
    return fav_toggled;
}
void gLabel::set_fav_videos(bool fav){
    if(fav)mark_fav_videos();
    else unmark_fav_videos();

}
void gLabel::mark_fav_videos(bool a){
    fav_toggled=true;
    if(type=="videos"){
        favpix=fav_on;
    }
    update();
    check_timer();
}
void gLabel::unmark_fav_videos(bool a){
    fav_toggled=false;
    if(type=="videos"){
        favpix=fav_off;
    }
    update();
    check_timer();
}
void gLabel::settle_poster(const QImage& im,bool tp)
{
    need_switch=true;
    if(loaded && !tp){
        need_switch=false;
        return;
    }
    final_poster=tp;
    while (paintingActive()) {
        qDebug()<<"PAINTING ACTIVE:"<<index;
        QApplication::processEvents();

    }
    lpix->setPixmap(QPixmap::fromImage(im));
    //  lpixmap=new QPixmap(lpix->pixmap()->scaled(width()*dpi,height()*dpi+(lh-main_h)*dpi,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    is_poster=true;
    if(fav_toggled)mark_fav_videos(false);
    else unmark_fav_videos(false);
    check_timer();
}
gLabel::~gLabel(){
    if(fav_off!=nullptr)delete fav_off;
    if(fav_on!=nullptr) delete fav_on;
    if(sha_pix!=nullptr)delete sha_pix;
    if(del_pix!=nullptr)delete del_pix;
    qDebug()<<"BRICK DESTR:"<<index<<type;
}
