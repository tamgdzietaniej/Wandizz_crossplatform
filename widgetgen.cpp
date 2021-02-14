/*
 *
 * */
#include "widgetgen.h"

#define G_N_ELEMENTS(arr) ((sizeof(arr))/(sizeof(arr[0])))
widgetGen::widgetGen(QWidget *parent,QString cname) : QOpenGLWidget(parent),selectors_visible(false),
    ready_to_paint(false),selectors_visible_prev(false),
    back_mutex(false),
    br_deleted(-1),
    over_brick(-1),
    over_brick_ind(-1),
    antime(130),
    filtered(""),
    scan(false),
    animating(false),
    locker(0),
    filter_chars(0),
    vcatapultscrollspeed(0),
    over_brick_hold(0),
    over_brick_ind_hold(0),
    scrr(0),
    empty_queue(false),
    br_updated(false),
    delmutex(false),
    locked(false),
    freezed(true),
    is_expanded(false),
    old(1000),
    gen_busy(false),
    debg(0),
    fav_mode(false),
    scr(0),
    naviDis(true),
    pos(QPoint()),
    prev_pos(QPoint()),
    sh_titles(true),
    fooLab(new gLabel),
    fallDown(new gLabel)
{
    top_menu_switcher=new topMenuSwitcher();
    setAttribute(Qt::WA_AlwaysStackOnTop,true);
    setAttribute(Qt::WA_DeleteOnClose,true);
    setAttribute(Qt::WA_TransparentForMouseEvents,false);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    dpi = devicePixelRatio();
    debg=false;
    ptimer.setSingleShot(true);
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
    tmp_list.clear();
    fallDown->type="fall";
    shadow_geo.setRect(marg,marg,wdt-2*marg,hgh-2*marg);
    splash_img.load(":/gui/BANNERS/wandizz_trnsp.png");
    timer.setInterval( probe_interval );
    connect( &timer, SIGNAL( timeout() ), this, SLOT( timerEv() ));
    connect(this,SIGNAL(timershot(int)),this,SLOT(setNextShot(int)),Qt::QueuedConnection);
    connect(&updtimer,SIGNAL(timeout()),this,SLOT(make_poster()),Qt::QueuedConnection);
    updtimer.setInterval(2000);
    updtimer.start(2000);
}
void widgetGen::initializeGL(){
    glClearColor(0,0,0,0);
}
void widgetGen::shTitles(bool sh){
    sh_titles=sh;
    toggleBricks(true);
}
void widgetGen::setMRB(bool ctx,int m,int n,int o){
    mfcorr=n;
    brcorr=o;
    marker_body[ctx]->move(0,m);
}
void widgetGen::revalidate(QPointer<gLabel> g){
    g->revalidate();
}
void widgetGen::start_bg_proc(){
    if( !timer.isActive() )
        timer.start(probe_interval);
}
void widgetGen::stop_bg_proc(){
    if( !timer.isActive() )return;
    timer.stop();
}
void widgetGen::setSelectors(bool f){
    bool s=manageSelectors();
    if(s==prev_selector && !f)return;
    prev_selector=s;
    if(f){
        if(!top_menu_switcher->isVisible())
            top_menu_switcher->setVisible(true);
    }
    showSelectors(s);
    top_menu_switcher->showSelectors(s);
}
void widgetGen::restoreCoords(){
    int c=-marker_head->mapToGlobal(QPoint(0,0)).y();
    if(c!=0){
        qDebug()<<"RESTORE:"<<c<<marker_head->mapToGlobal(QPoint(0,0))<<mapToGlobal(QPoint(0,0));
        scroll(0,c);
    }
}
void widgetGen::corrGeo(){
    int c=0;

    for(int i=0;i<=lastvis;i++){
        if(item(i)){
            if(item(i)->isLocked()) continue;

            if(item(i)->geometry()!=brick_geo(c)){
                qDebug()<<"CORR GEO:"<<i;
                item(i)->setGeometry(brick_geo(c));
            }
            c++;
        }
    }
}
void widgetGen::timerEv(bool force){
    if(scrollPerform()!=0){
        setSelectors();
        corrGeo();
    } else {
        if((br_deleted==-1 && !delmutex && !force) || locked){
            if(!pressed ){
                timer.stop();
            }
        }
    }
}
void widgetGen::setupTopMenuSwitcher(QRect r){
    top_menu_switcher->setGeometry(r);
    top_menu_switcher->setFixedSize(r.size());
    top_menu_switcher->updateGeometry();
    top_menu_switcher->setVisible(true);
}
void widgetGen::setFrames(QWidget *f1, QWidget *f2){
    top_menu_switcher->setFrames(f1,f2);
}
void widgetGen::clean(){
    wait_to_lock=true;
    lock();

}
bool widgetGen::setContext(const QString& c){
    //  toggleNavi(true);
    freeze(true);
    setTypeID(c!="prospect");
    bool nc=false;
    if(c!=context){
        if(timer.isActive()){
            timer.stop();
            if(debg)qDebug()<<"TIMER INACTIVE:!!!:";
        }
        nc=true;
    }
    tmp_list.clear();
    context=c;
    prospectInfo->setEnabled(isProspect());
    pressed=false;
    vcatapultscrollspeed = 0;
    swipe_orient = not_set;
    move = QVector2D( 0, 0 );
    br_deleted = -1;
    return nc;
}
int widgetGen::msy(){
    //   return top_menu_switcher->mapToGlobal(QPoint(0,top_menu_switcher->height())).y();
    return top_menu_switcher->y();
}
void widgetGen::freeze(bool l){
    freezed=l;
    //  if(isEnabled()==l)setEnabled(!l);
    //   if(top_menu_switcher->isEnabled()==l)top_menu_switcher->setEnabled(!l);
}
void widgetGen::lock(){
    emit setLocked(true);
    lockToggle(true);
}
void widgetGen::unlock(){
    emit setLocked(false);
    lockToggle(false);
    freezed=false;
    need_corr=true;
    start_bg_proc();
}
void widgetGen::lockToggle(bool l){
    locked=l;
    toggleBricks(!l);
}

bool widgetGen::isLocked(){
    return locked || freezed;
}
void widgetGen::set_fav_mode(){
    fav_mode=true;
}
void widgetGen::set_prospect_params(Prospect &p){
    par=&p;
    top_menu_switcher->par=&p;
    top_menu_switcher->setupSearch();
}
void widgetGen::brick_remove( int i ,int dbi,const QString tp){
    if(!item(i))return;
    delmutex=true;
    //    freeze(true);
    qDebug()<<"delete:"<<over_brick<<i<<dbi;
    br_deleted_ind=dbi;
    br_deleted=i;
    br_deleted_type=tp;
    stop_bg_proc();
    int cnt=0;
    animating=false;
    bool animated = false;
    if( context != "videos" || context !="prospect" ){
        int acnt = 0;
        if(debg)qDebug()<<"LOOP DELETE";
        group = new QSequentialAnimationGroup( this );
        connect(group,SIGNAL(finished()),this,SLOT(finish_deletion()));
        for( int j = 0;j < getItemsCount();j++ ){
            if(item(j)->isLocked()){
                qDebug()<<"LOCKED:"<<j;
                continue;
            }
            if(cnt<br_deleted){
                cnt++;
                continue;
            }
            animating=true;
            QPropertyAnimation* an= new QPropertyAnimation( item( j ), "geometry", this );
            an->setDuration( antime );
            an->setStartValue( item( j )->geometry() );
            an->setEndValue( brick_geo(cnt));
            an->setDirection( QPropertyAnimation::Direction::Forward );
            an->setEasingCurve( QEasingCurve::Linear );
            //  del_anim.append(an);
            group->addAnimation( an );
            acnt++;
            animated = true;
            cnt++;
        }
    }
    qDebug()<<"DELETING"<<br_deleted;
    need_corr=true;
    if( animated  ) group->start(QAbstractAnimation::DeleteWhenStopped);
    else
        finish_deletion();

    // if(filtered!="")filter(filtered);
}
int widgetGen::getItemsCount(){
    if(itemList()==nullptr)
        return 0;
    return itemList()->count();
}
bool widgetGen::isVideos(QString t){
    if(t=="")t=context;
    return t=="videos";
}
bool widgetGen::isFVideos(QString t){
    if(t=="")t=context;
    return t=="fav_videos";
}
bool widgetGen::isFScenes(QString t){
    if(t=="")t=context;
    return t=="fav_scenes";
}
bool widgetGen::isFItems(QString t){
    if(t=="")t=context;
    return t=="fav_items";
}
bool widgetGen::isProspect(QString t){
    if(t=="")t=context;
    return t=="prospect";
}
void widgetGen::setProspectInfo(QFrame *p){
    prospectInfo=p;
}
void widgetGen::filter(const QString& f){
    while(delmutex){
        qDebug()<<"DELMUTEX";
        QApplication::processEvents();
    }
    // qDebug()<<"FILTER::"<<context;
    filtered=f;
    int scnt=0;
    lastvis=-1;
    prospectInfo->setEnabled(f=="" && context=="prospect");
    update();
    QList<int> protect_list({});
    for(int i=0;i<getItemsCount();i++){
        if(!item(i))continue;
        if(debg)qDebug()<<"--------------------------------"<<getItemsCount();
        if((getItemLowerText(i).contains(filtered,Qt::CaseInsensitive) || getItemUpperText(i).contains(filtered,Qt::CaseInsensitive))  &&
            ((filtered.length()>=par->prospect_filter_chars || !isProspect()) || (par->prospect_initial_view && f==""))){
            if(isProspect() && !item(i)->isProducer()){
                /*  Show producer according to title as list-section-head, if locked */
                if(!protect_list.contains(producerOf(i)->db_index)){
                    /* Avoiding multiple appear */
                    protect_list.append(producerOf(i)->db_index);
                    toggle(producerOf(i),true,scnt);
                    scnt++;
                }
            }
            /* Show proper widget */
            //       qDebug()<<"SHTITLES:"<<(!protect_list.contains(item(i)->db_index))<<sh_titles;
            if(!protect_list.contains(item(i)->db_index) && sh_titles){
                toggle(item(i),true,scnt);
                scnt++;
                if(item(i)->isProducer())
                    protect_list.append(item(i)->db_index);
                lastvis=i;
            }
        }
        else {
            /* Roll to invisible state not matching widget - if not at list of one of items's producers */
            if(!protect_list.contains(item(i)->db_index))
                toggle(item(i),false,scnt);
        }
    }
    emit filter_done();
    is_expanded=lastvis>-1;
    //   noResults->setEnabled(lastvis==-1);
    update();
}

QString widgetGen::actContext(){
    return context;
}
bool widgetGen::mclick(){
    if( lastVis()<0 )return false;
    QString _oid=QString::number(curr_oid);
    QString _nf=curr_netfile;
    QString _tid = QString::number(curr_title_id);
    QString w_desc=getItemUpperText();
    QString _time;
    QString _context=curr_type;
    if( curr_type  == "fav_items" ){
        emit go("web",{curr_event});
        return false;;
    } else
        if(curr_type=="prospect"){
        if(curr_item)
            emit go("userprof",{"context",w_desc,QString::number(curr_item->db_index)});
    } else
        if( isTakingToCarousel() ){
        if( curr_items_cnt >0 ){
            emit go("carousel",{w_desc, _time,_nf, _oid,_tid});
        } else qDebug()<<"ITEMSCNT!";
        return false;
    }
    return true;
}
bool widgetGen::isTakingToCarousel(){
    if(!curr_item)return false;
    return (curr_item->isVideos() || isFScenes() || isFVideos());
}
QList<QPointer<gLabel>> widgetGen::getTables(){
    QList<QPointer<gLabel>> tg;
    tg.clear();
    tg.append(vb);
    tg.append(fvb);
    tg.append(fsb);
    tg.append(fib);
    tg.append(msb);
    return tg;
}
QPointer<gLabel> widgetGen::mglsearch(const QString lookfor){
    QList<QPointer<gLabel>> tg=getTables();
    QPointer<gLabel> res;
    if(tg.count()==0)return nullptr;
    for(int i=0;i<tg.count();i++){
        gLabel* _t=tg.at(i);
        if(_t){
            if(_t->fileurl==lookfor){
                if(debg)qDebug()<<"Compare&found:"<<_t->fileurl<<lookfor;

                return _t;
            }
        }
    }
    return nullptr;
}
void widgetGen::toggleNavi(bool t){
    emit enableMenu();
    naviDis=t;
}
bool widgetGen::isNDis(){
    return naviDis;
}
void widgetGen::make_brick( int index, int di,int tid,int itcnt, const QString& up, const QString& dn, const QString& imgurl,
                           const QString& imgfile, const QString& titlemedia, const QString& mod_name,int od, QString event){
    //   qDebug()<<"MAKE BRICK:"<<index<<mod_name<<context<<imgurl;
    bool ft = false;
    if(index==0)lastvis=-1;
    /* SEARCH OR CREATION OF WIDGET */
    QPointer<gLabel> gl;
    gl=mglsearch(imgurl);
    if(gl!=nullptr)
        tmp_list.append(gl);
    else {
        if(debg)qDebug()<<"NEW BRICK:"<<up<<dn<<mod_name;
        gl = new gLabel( this, mod_name ,context );
        gl->poster_data_ready=&poster_data_ready;
        tmp_list.append(gl);
        gl->set_prospect_params(*par);
        gl->setDPI(dpi);
        connectSignals(gl);
        ft=true;
    }
    // toggle(gl,par->prospect_initial_view || context!="prospect");
    if(debg)qDebug()<<"Brick geo:"<<index<<gl->geometry();
    gl->setObjectName(mod_name);
    gl->setGeometry(brick_geo(index));
    gl->event_to_share=event;
    gl->set_params( index, di, itcnt,tid,imgfile, imgurl, up, dn,&splash_img, titlemedia,od);
    gl->setSilent(index,mod_name,context);
    if(debg)qDebug()<<"MAKE:"<<gl->index<<gl->db_index;
    if(isVideos(mod_name)){
        gl->fav_toggled=fav_videos_list.contains( tid );
    }
    gl->setWatcher(curr_type,curr_ctype,curr_event,curr_netfile,curr_oid,curr_title_id,over_brick,curr_items_cnt,br_updated);
    if(gl->y()+brct.height()>-mrh() && gl->y()<-mrh()+rct.height()>0)
        gl->setScaled(false);
    else gl->setScaled(false);
    //  if(!isVisible()){
    //     qDebug()<<"GLCONT NOT VIS";
    //     raise();
    //     showFullScreen();
    //  }
    timerEv();
}
QStringList widgetGen::getThumbRef(const QString& imPath,QString title){
    QFileInfo posterFile=QFileInfo( imPath );
    QString poster_basename=posterFile.baseName();
    if(poster_basename==""){
        poster_basename=title.replace(" ","_");
    }
    QString posterFullPath=posterFile.path()+"/"+f_poster_prefix+poster_basename+".png";
    posterFullPath=posterFullPath.replace("/media/","/frames/");
    QString posterUrl=serverRoot+posterFullPath;
    QString posterDir= d_posters+poster_basename+".png";
    return {posterUrl,posterDir,serverRoot+imPath};
}

void widgetGen::connectSignals(gLabel* gl){
    //  connect( gl, SIGNAL( download( const QString, const QString) ), this,
    //      SIGNAL( download( const QString, const QString ) ) );
    connect( gl, SIGNAL( mpressed( gLabel*,QMouseEvent* ) ), this,  SLOT( handlePressed( gLabel*,QMouseEvent* ) ),Qt::DirectConnection);
    connect( gl, SIGNAL( mreleased( gLabel*,QMouseEvent* ) ), this,  SLOT( handleReleased( gLabel*,QMouseEvent* ) ),Qt::DirectConnection);
    connect( gl, SIGNAL( toggle_fav_video (int, bool)), this, SIGNAL(toggle_fav_video(int,bool)));
    connect( gl, SIGNAL( del_fav( int,int,const QString) ), this, SLOT( brick_remove( int, int,const QString )) );
    connect( gl, SIGNAL( hided() ), this,  SIGNAL( hided()) );
    connect( gl, SIGNAL( get_poster(int,const QString&,const QString&,const QString&,const QString&,const QString&,const QString&,int)),
            this, SLOT( poster_generator(int, const QString&, const QString&,const QString&,const QString&,const QString&,const QString&,int)),Qt::QueuedConnection);
}
void widgetGen::set_fav_videos_list(const QJsonArray *arr){
    int _cnt = arr->count();
    fav_videos_list.clear();
    for( int i = 0;i<_cnt;i++ ){
        int in = arr->at( i ).toObject().value( "fav_id" ).toInt();
        fav_videos_list.append( in );
    }
}
void widgetGen::handlePressed( gLabel* g ,QMouseEvent* e){
    if(delmutex || isLocked())return;
    //   br_updated=true;
    //   over_brick = e;
    //   over_brick_ind = f;
    //  over_brick_hold=e;
    //  over_brick_ind_hold=f;

    mousePressEvent(e);
}
void widgetGen::handleReleased( gLabel* g,QMouseEvent*e ){
    if(delmutex || isLocked())return;
    //   br_updated=true;
    //   over_brick = e;
    //   over_brick_ind = f;
    //  over_brick_hold=e;
    //  over_brick_ind_hold=f;

}

void widgetGen::toggleBricks(bool w){
    while(delmutex){
        qDebug()<<"DELMUTEX";
        QApplication::processEvents();
    }
    if(w && top_menu_switcher->isEditEn){
        filter(top_menu_switcher->edit->text());
        return;
    }
    //  toggleNavi(false);
    is_expanded=w;
    lastvis=-1;
    freeze(true);
    for(int i=0;i<getItemsCount();i++){
        QPointer<gLabel> gl(item(i));
        toggle(gl,w,i);
        if(w)lastvis=i;
    }
    wait_to_lock=false;
    freeze(false);
}
void widgetGen::toggle(gLabel* g,bool s,int i){
    if(i==-1)i=g->index;
    if(i>-1){
        //    g->reindex(i,brick_geo(i));
        // if(debg)qDebug()<<"BRICK GEO:"<<brick_geo(i);
        marker_tail->move(0,brick_geo(i).bottom());
        g->setGeometry(brick_geo(i));
        qDebug()<<"TOGGLE:"<<i<<g->geometry();
    }
    if(s){
        g->unlock();
        g->activateWindow();
    } else {
        /* LOCK */
        if(debg)qDebug()<<"LOCK:"<<g->type<<g->index;
        g->lock();
        while(g->isVisible()){
            qDebug()<<"WAIT FOR OFF:"<<i;
            QApplication::processEvents();
        }
    }
    update();

    //    g->forceUpdate();
    //    forceUpdate();
    //   timerEv(true);
    //    qDebug()<<"TOGGLE:"<<s<<childrenRect()<<childrenRegion()<<sizeHint()<<frameGeometry();
}
void widgetGen::activate(){
    unlock();
    if(isProspect())
        msb=tmp_list;
    if(isVideos())
        vb=tmp_list;
    if(isFItems())
        fib=tmp_list;
    if(isFScenes())
        fsb=tmp_list;
    if(isFVideos())
        fvb=tmp_list;

    if(debg)qDebug()<<context<<tmp_list;
    if(debg) qDebug()<<"Activate:"<<leased<<" drived straight";
    leased=0;
    toggleBricks(true);
    while(!isVisible()){
        showFullScreen();
        qDebug()<<"RAISING";
        update();
        QApplication::processEvents();
    }
    setSelectors();
    top_menu_switcher->showKB();
    timerEv();
}
void widgetGen::setDirector(QWidget *w){
    top_menu_switcher->setParent(w);
    setParent(w);
    disconnect(top_menu_switcher,SIGNAL(textChanged(const QString&)),this,SLOT(filter(const   QString&)));
    connect(top_menu_switcher,SIGNAL(textChanged(const QString&)),this,SLOT(filter(const   QString&)));
}
bool widgetGen::isExpanded(){
    return is_expanded;
}
void widgetGen::noticeShow(){
    animating=false;
}
QRect widgetGen::brick_geo(int i){
    return QRect(0, brcorr + mrb()+i* brct.height(),brct.width(), brct.height());
}
QRect widgetGen::get_srect(int i){
    QRect srect=brick_geo(i);
    srect.setHeight(1);
    srect.setY(srect.bottom()-srect.height()+1);
    return srect;
}
QList<QPointer<gLabel> > *widgetGen::itemList(){
    if( context  == "videos")return &vb;
    else
        if(isFVideos())return &fvb;
    else
        if(isFScenes())return &fsb;
    else
        if(isFItems())return &fib;
    else
        if( isProspect() )return &msb;
    else
        return nullptr;
}

QString widgetGen::getItemUpperText(int i){
    if(i==-1)i=over_brick;
    if(i<0)return "";
    if(!item(i))return "";
    else return item(i)->txt_text;
}
QString widgetGen::getItemLowerText(int i){
    if(i==-1)i=over_brick;
    if(i<0)return "";
    if(!item(i))return "";
    else return item(i)->tim_text;
}
int widgetGen::getItemOid(int i){
    if(i==-1)i=over_brick;
    if(i<0)return -1;
    if(!item(i))return -1;
    else return item(i)->oid;
}
void widgetGen::share(){
    QString text = getItemUpperText();
    QUrl url( curr_event);
#if defined( Q_OS_ANDROID )
    share_cross* share = new share_cross( this );
    share->share( text, url );
#elif defined ( Q_OS_IOS )
    shareios* share = new shareios();
    share->share( text, url );
#endif
}
void widgetGen::setCoords(QRect rt, int d){
    qreal w;
    dpi=d;
    mrgh=.01;
    brct.setRect(0,0,rt.width(),rt.width()/par->widget_height_factor);
    w=brct.width()*dpi;
    vtm=brct.height()/20;
    htm=brct.width()/20;
    lh=(brct.height()/1.03)*dpi;
    mrgh=lw*0.01;
    lw=w;
    lm=w/100;
    lmm=lm*2;
    txt_font.setFamily("Apple Gothic");
    tim_font.setFamily("Apple Gothic");
    txt_font.setStyleStrategy(QFont::StyleStrategy::PreferQuality);
    tim_font.setStyleStrategy(QFont::StyleStrategy::PreferQuality);
    rct=rt;
    splash_img=splash_img.scaled(brct.size()*.8/dpi);
    // must be called before the widget or its parent window gets shown
    if(debg)qDebug()<<"RCT:"<<rct<<geometry();
}
bool widgetGen::manageSelectors(){
    return mrb() >= mfcorr;
}

QPointer<gLabel> widgetGen::producerOf(int i){
    if(i==-1)
        i=over_brick;
    int oid=getItemOid(i);
    for(int i=0;i<getItemsCount();i++){
        if(item(i)){
            if(item(i)->db_index==oid){
                return item(i);
            }
        }
    }
    return nullptr;
}
QPointer<gLabel> widgetGen::item( int i,QString t){
    QString cn=context;
    if(t!="")cn=t;
    if(isVideos(cn))
        if(vb.count()>i)
            return vb.at(i);

    if(isFVideos(cn))
        if(fvb.count()>i)
            return fvb.at(i);

    if(isFScenes(cn))
        if( fsb.count() > i )
            return fsb.at(i);

    if(isFItems(cn))
        if(fib.count() > i )
            return fib.at(i);

    if(isProspect(cn))
        if(msb.count() > i )
            return msb.at(i);
    return nullptr;
}
void widgetGen::finish_deletion(){
    lastvis=-1;
    emit set_cnt(itemList()->count()-1);
    qDebug()<<"br:"<<br_deleted;
    if(item(br_deleted))
        if(item(br_deleted)){
            br_deleted_type.replace("fav_","fav");
            if(!itemList()->isEmpty() && br_deleted<getItemsCount())
                itemList()->removeAt(br_deleted);
            else qDebug()<<"DELETE:CRITICAL!";
            animating=false;
            emit del_fav(br_deleted_type,br_deleted_ind);
        }
    for(int i=br_deleted;i<getItemsCount();i++){
        if(item(i)){
            item(i)->index=i;
            if(!item(i)->isLocked()){
                lastvis=i;
            }
        }
    }
    br_deleted=-1;
    br_deleted_ind=0;
    br_deleted_type="";
    curr_item=nullptr;
    delmutex=false;
    marker_tail->move(0,brick_geo(lastVis()).bottom());
    corrGeo();
    start_bg_proc();
}
const QString& widgetGen::getContext(){
    return context;
}
void widgetGen::poster_generator(int ind,QString tp,  QString url,  QString fn, QString up, QString dn, QString cn,int itcnt){
    for(int i=0;i<poster_data_ready.count();i++){
        if(poster_data_ready.at(i).type==tp && poster_data_ready.at(i).file==fn){
            qDebug()<<"Generatoe has file "<<fn<<" of "<<tp;
            return;
        }
    }
    PosterData _tmpdata;
    _tmpdata.index=ind;
    _tmpdata.type=tp;
    _tmpdata.Url=url;
    _tmpdata.file=fn;
    _tmpdata.up=up;
    _tmpdata.dn=dn;
    _tmpdata.context=cn;
    _tmpdata.itcnt=itcnt;
    poster_data.append(_tmpdata);
    /* BRICK & POSTER FACTORY */
    //qDebug()<<"POSTERS WAITING:"<<poster_data.count()<<"ready:"<<poster_data_ready.count();
    if(!gen_busy){
        make_poster();
    }


}
void widgetGen::make_poster(){
    if(poster_data.isEmpty()){
        return;
    }
    empty_queue=false;
    gen_busy=true;
    future=(QtConcurrent::run([=]() {
        QMutexLocker lock{&_mutex};
        /* GRAB DATA STRUCTURE */
        if(poster_data.isEmpty()){
            gen_busy=false;
            return;
        }
        PosterData pdata=poster_data.takeFirst();
        QImage _p;
        /* IS FILE AVAILABLE? */
        bool chk=QFileInfo::exists(pdata.file);
        if(chk){
            _p.load(pdata.file);
            // validation
            if(_p.isNull()){
                QFile::remove(pdata.file);
                chk=false;
            }
        }
        pdata.to_dload=!chk;
        if(pdata.to_dload){
            if(!pdata.in_dl_queue){
                pdata.in_dl_queue=true;
                //      qDebug()<<"POSTER:TO DOWNLOAD:"<<pdata.file;
                emit download(pdata.Url,pdata.file);
                _p=splash_img;
                poster_data.append(pdata);
                emit timershot(50);
                return;
            } else {
                //        qDebug()<<"POSTER:CONTINUE:"<<pdata.index;
                poster_data.append(pdata);
                emit timershot(150);
                return;
            }
        } else pdata.is_splash=false;
        QImage ppt(lw,lh,QImage::Format_ARGB6666_Premultiplied);
        ppi=new QImage(lw,lh,QImage::Format_ARGB6666_Premultiplied);
        /* layout */
        if(pdata.type=="videos"){
            tim_align=Qt::AlignCenter;
        } else {
            tim_align=Qt::AlignCenter;
        }


        /* aspectratios */
        if(pdata.type=="fav_items" || pdata.type=="prospect")
            _prop=1;
        else
            _prop=1.75;
        _lw=lh*_prop;
        _lw2=lw-_lw-2*htm;


        /* look & feel */
        double fct=1;
        if(pdata.type=="prospect"){
            txt_font.setPixelSize(25.0*dpi);
            txt_font.setBold(true);
            txt_rect.setRect(_lw+htm,vtm,lw-_lw-2*htm,lh*.3);
            ppt.fill(QColor(0,0,0,170));
        } else if(pdata.type=="fav_items"){
            txt_font.setPixelSize(17.0*dpi);
            txt_font.setBold(false);
            txt_rect.setRect(_lw+htm,vtm,lw-_lw-2*htm,lh*.3);
            ppt.fill(QColor(0,0,0,150));
        } else {
            txt_font.setPixelSize(17.0*dpi);
            txt_font.setBold(false);
            txt_rect.setRect(_lw+htm,vtm,_lw2,lh*.75);
            ppt.fill(QColor(0,0,0,150));
            fct=.8;
        }
        tim_rect.setRect(txt_rect.left(),txt_rect.bottom(),txt_rect.width()*fct,lh-txt_rect.height());
        tim_font.setPixelSize(16*dpi);
        /* CORRECTION GEO WATCHDOG */
        for(int i=0;i<=lastvis;i++){
            if(item(i)){
                if(item(i)->isLocked()){
                    qDebug()<<"Locked:"<<i;
                    continue;
                }
                corrGeo();
            }
        }

        /* factory */
        QPainter painter;
        painter.begin(&ppt);
        painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
        painter.fillRect(ppt.rect(),QBrush(QColor(0,0,0,0)));
        if(pdata.type=="fav_items" || pdata.type=="prospect"){
            painter.setBrush(QColor(255,255,255,255*(pdata.type=="fav_items")));
            painter.drawRect(QRect(0,0,_lw,lh));
            painter.setBrush(QBrush());

            _p=_p.scaled(_lw,lh,Qt::KeepAspectRatio,Qt::SmoothTransformation);

            painter.drawImage((_lw-_p.width())/2,(lh-_p.height())/2,_p);
        } else
            painter.drawImage(QRect(0,0,_lw,lh),_p);
        painter.end();

        ppi->fill(QColor(0,0,0,0));
        painter.begin(ppi);
        painter.setBrush(ppt);
        painter.setPen(QColor(0,0,0,0));
        painter.drawRoundedRect(1,1,lw-2,lh-2,10*dpi,5*dpi);
        if(pdata.type=="prospect"){
            painter.setPen(QColor("#aaf"));
        } else
            painter.setPen(QColor("#eef"));
        painter.setFont(txt_font);
        painter.drawText(txt_rect,Qt::TextFlag::TextWordWrap | Qt::AlignCenter,pdata.up);
        if(pdata.type=="prospect"){
            painter.setPen(QColor("#fff"));
        } else
            painter.setPen(QColor("#999"));
        painter.setFont(tim_font);
        painter.drawText(tim_rect,Qt::TextFlag::TextWordWrap | Qt::AlignVCenter | tim_align ,pdata.dn);
        if(pdata.itcnt==0 && pdata.type=="videos"){
            //      qDebug()<<"POSTER:COMING SOON:"<<pdata.file;
            QFont cfont(tim_font);
            cfont.setPixelSize(par->videos_coming_soon_text_size*dpi);;
            //  QBrush b(painter.brush());
            painter.save();
            painter.setPen(QColor(0,0,0,255));
            painter.setBrush(QBrush(QColor(0,0,0,25*par->videos_coming_soon_picture_dim)));
            painter.drawRoundedRect(0,0,lw,lh,10*dpi,5*dpi);
            //   painter.setBrush(b);
            painter.restore();
            painter.setBrush(QColor(235,186,26,150));
            QSize ts=painter.fontMetrics().boundingRect(par->videos_coming_soon_text).size()*1.2;
            painter.translate(lh*1.7-ts.width()*1.3,lh*.9);
            painter.rotate(-20);
            QRect csrect(QPoint(0,0),ts);
            painter.drawRoundedRect(csrect,15,15);
            painter.setPen(QColor(0,0,0));
            painter.drawText(csrect,Qt::AlignCenter,par->videos_coming_soon_text);
        }
        painter.end();
        if(item(pdata.index,pdata.context)){
            if(ppi->sizeInBytes()==0 || ppi->isNull())   emit reval (item(pdata.index,pdata.context));
            poster_data_ready.append({pdata.index,pdata.type,pdata.file,ppi,pdata.context,true});
        }
        int pdl=poster_data_ready.count();
        if(pdl<=poster_data_ready.count()){
            //        qDebug()<<"POSTER:"<<pdata.index;
        }

        else qDebug()<<"POSTER:!!!!!!!!!!!!!!!!!!!!!!!!ERROR:";
        gen_busy=false;
        emit timershot(50);
    }));
}
void widgetGen::setNextShot(int t){
    QTimer::singleShot(t, this, SLOT(make_poster()));
}
void widgetGen::forceUpdate(){
    update();
    top_menu_switcher->forceUpdate();
}
/* MOUSE MANAGEMENT */
bool widgetGen::mouseOver(QWidget* w){
    return QRect(w->mapToGlobal(QPoint(0,0)),w->size()).contains(QCursor::pos());
}
void widgetGen::mousePressEvent(QMouseEvent* e){
    qDebug()<<"BR UPDATED"<<br_updated<<over_brick;
    pressed=true;
    no_click=false;
    if(br_updated){
        if(over_brick>-1){
            qDebug()<<"hadle enter:"<<over_brick;
            curr_item=item(over_brick);
        }  else curr_item=nullptr;
    } else {
        curr_item=nullptr;
        over_brick=-1;
    }
    br_updated=false;

    if(top_menu_switcher->isEditEn){
        top_menu_switcher->showKB(false);
        }
    if(swipe_orient==not_set && abs(vcatapultscrollspeed)<5){
        ptimer.start( 200 );
        qDebug()<<"start ptimer"<<vcatapultscrollspeed<<scr;
    }
    else{
        q_vscroll.clear();
        ptimer.stop();
        q_vscroll.clear();
        vcatapultscrollspeed=0;
        ptimer.stop();
    }
    if(over_brick>=0){
        diff.setX( 0 );
        diff.setY( 0 );
        pos = e->globalPos();
        prev_pos = pos;
        touch_pos = pos;
    }
    if(abs(scrr)>5){
        catapult_brakes=true;
        scrr=0;
    }
    if(swipe_orient!=not_set)
        no_click=true;
    swipe_orient=not_set;
    start_bg_proc();
}
void widgetGen::mouseReleaseEvent(QMouseEvent *e){
    if(isLocked())return;
    pressed=false;
    bool st=true;
    vcatapultscrollspeed = get_vscroll();

    catapult_brakes=false;
    if(!no_click && swipe_orient==not_set){
        qDebug()<<"ptimer:"<<ptimer.isActive()<<(swipe_orient==not_set);
        if(ptimer.isActive()){
            st=get_hover();
            if( over_brick>-1){
                if(e->globalPos().x() <=brct.width()*4/5 ){
                    mclick();
                } else
                    if( context=="videos" ){
                    if(curr_item)
                        curr_item->toggle_fav(true);
                } else
                    if(context.contains("fav")){
                    share();
                }
            }
        }
    } else if(swipe_orient==horizontal){
        if(over_brick>-1 && curr_item)
            curr_item->relEvent(e);
    }
    swipe_orient=not_set;
}
void widgetGen::mouseMoveEvent(QMouseEvent *e){
    qDebug()<<"WGEN MOUSEMOE:"<<over_brick<<isLocked()<<delmutex<<swipe_orient<<lastvis;
    if(isLocked() || over_brick==-1 || delmutex){
        qDebug()<<"LOCKED("<<over_brick<<")";
        return;
    }
    prev_pos = pos;
    pos = e->globalPos();
    move = QVector2D( pos-prev_pos );
    diff = QVector2D( pos-touch_pos );

    get_swipe_predicter();
    if(swipe_orient==horizontal && over_brick>-1){
        if(curr_item)
            curr_item->movEvent(e);
        return;
    }
    if( swipe_orient == vertical){
        if( lastVis()>-1 ){
            scrr += double( move.y() );
            append_vscroll( double( move.y() ) );
        }
    }
}
/* SWIPE PREDICTER */
void widgetGen::get_swipe_predicter(){
    if( lastVis()<0 )return;
    if( swipe_orient != not_set )return;
    if( abs( move.x() ) <= abs( move.y() ) && abs( diff.y() )>2*dpi ){
        if( item( lastVis() )->geometry().bottom()-mrh()>rct.height() ){
            swipe_orient = vertical;
            if(curr_item)
                curr_item->setGesture(false);
            return;
        }
    } else
        if( abs( move.x() )>abs( move.y() ) && !offline && abs( diff.x() )>3*dpi ){
        swipe_orient = horizontal;
        if(curr_type  == "videos" )return;
        if(curr_item)curr_item->setGesture(true);
    }
}
/* SCROLLER MANAGEMENT */
int widgetGen::scrollPerform(){
    if( !pressed ){
        rubberbandscrollspeed = 0;
        int tail=marker_tail->y();
        int head=marker_head->geometry().bottom();
        // too low
        if(tail<rct.height() && head>0){
            rubberbandscrollspeed = -(head/2)*!reinit-head*reinit;
            if(reinit)
                qDebug()<<"REINIT";
            if( abs( rubberbandscrollspeed )<1 ){
                rubberbandscrollspeed = -head;
            }
        } else
            if( tail<rct.height() && head<0){
            qDebug()<<"LOW:"<<head<<tail;
            vcatapultscrollspeed /= 2;
            rubberbandscrollspeed = ( rct.height()-tail )/2;
            if( abs( rubberbandscrollspeed )<1 ){
                rubberbandscrollspeed = rct.height()-tail;
            }
        } else
            if( head>0 && tail>rct.height()){
            rubberbandscrollspeed = -head / 2;
            vcatapultscrollspeed /= 2;
            if( abs( rubberbandscrollspeed )<1 ){
                rubberbandscrollspeed = -head;
            }
        }
        //else qDebug()<<"HEAD:"<<head<<glcontainer->marker_head->y()<<glcontainer->marker_head->mapToGlobal(QPoint(0,0));
        scrr += rubberbandscrollspeed;
        rubberbandscrollspeed = 0.0;
        if( fabs( vcatapultscrollspeed ) < 1 ){
            vcatapultscrollspeed = 0.0;
        } else {
            vcatapultscrollspeed *= 0.96;//+boost;
        }
        scrr += vcatapultscrollspeed;
    }
    scr=scrr;
    scrr=0;
    scroll( 0, scr );
    if(reinit){
        reinit=false;
    }
    return scr;
}
gLabel* widgetGen::lastVisibleItem(){
    if(lastVis()==-1 || getItemsCount()==0 || lastVis()>=getItemsCount()){
        return nullptr;
    } else {
        if(!item(lastVis())->isLocked())
            return item(lastVis());
        else {
            qDebug()<<"!!!!!!!!!LASTVIS LOCKED!!!!!!!!";
            return fallDown;
        }
    }
}
int widgetGen::lastVis(){
    return lastvis;;
}
int widgetGen::get_vscroll(bool e){
    int i, vv = 0, zeros = 0;
    if( q_vscroll.count()<max_q )return 0;
    for( i = 0;i<q_vscroll.count();i++ ){
        if( q_vscroll.at( i )  == 0 )zeros++;
        vv += q_vscroll.at( i );
    }
    if(e)q_vscroll.clear();
    if(i==0)i=1;
    return ( ( vv / ( i ) ) / dpi ) * 4 ;
}
void widgetGen::append_vscroll( int s ){
    q_vscroll.append( s );
    if( q_vscroll.count()>max_q )q_vscroll.pop_front();
}
/* PERMISSIONS CHECK & INFO */
bool widgetGen::canToggleFavs(){
    if(!curr_item)return false;
    return isVideos() || (par->prospect_favs && curr_item->isProducerType() && curr_item->isVideos());
}
bool widgetGen::canToggleShares(){
    if(!curr_item)return false;
    return (!curr_item->isProducerType() && !curr_item->isVideosType()) ||
           (par->prospect_shares && curr_item->isProducerType() && curr_item->isProducer() &&
            !isVideos());
}
void widgetGen::setMarkers(){
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
bool widgetGen::get_hover(){
    mpos=QCursor::pos();
    nmpos=mapToGlobal(mpos);
    if(!ready_to_paint){
        qDebug()<<objectName()<<"not ready";
        return false;
    }
    for(int j=0;j<wlist.count();j++){
        for(int i=0;i<wlist.at(j).count();i++){
            QList<QPushButton*> bl = wlist.at(j).at(i)->findChildren<QPushButton *>();
            for(int k=0;k<bl.count();k++){
                if(clicked(bl.at(k))){
                    return false;
                }
            }
        }
        for(int i=0;i<wlist.at(j).count();i++){
            QList<QCheckBox*> bl = wlist.at(j).at(i)->findChildren<QCheckBox*>();
            if(bl.count()>0){
                for(int k=0;k<bl.count();k++){
                    if(clicked(bl.at(k))){
                        return false;
                    }
                }
            }
        }
    }
    //  if(prospectInfo->isEnabled()){
    //      QList<QPushButton*> bl=prospectInfo->findChildren<QPushButton*>();
    //      if(bl.count()>0){
    //         bl[0]->click();
    //     }
    //   else qDebug()<<"no button";
    //  }
}
bool widgetGen::clicked(QWidget* w){
    if(!w)return false;
    QRect rc=QRect(w->mapToGlobal(QPoint(0,0)),w->size());
    if(rc.contains(mpos)){
        if(QString::fromLatin1(w->metaObject()->className())=="QPushButton"){
            if(w->isEnabled()){

                int cx=w->mapToGlobal(QPoint(0,0)).x();
                int cy=w->mapToGlobal(QPoint(0,0)).y();
                qDebug()<<"-----------------------------------------------------------";
                qDebug()<<"ITEM NAME:"<<w->objectName();
                qDebug()<<"--- HOVER : WW ---- MOUSE:"<<nmpos<<" / global:"<<mpos;
                qDebug()<<"- item pos:"<<mpos<<", map to glob:"<<w->mapToGlobal(QPoint(0,0))<<w->size()<<"-";
                qDebug()<<"- item geo:"<<w->rect().adjusted(cx,cy,cx,cy);
                qDebug()<<"-----------------------------------------------------------";
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
void widgetGen::setTypeID(bool ctx){
    type_id=ctx;
}

void widgetGen::showSelectors(bool s){
    if(selectors_visible!=s){
        selectors_visible=s;
        update();
    }
}
int widgetGen::mrh(){
    return marker_head->y();
}
int widgetGen::mrt(){
    return marker_tail->y();
}
int widgetGen::mrb(){
    return marker_body[type_id]->y();
}
void widgetGen::setupInfo(QFrame *pi){
    prospectInfo=static_cast<QFrame*>(pi);

}
void widgetGen::setupWrapper(QList<QList<QWidget*>> wdl){
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
            QPoint tpos(mapFromGlobal(wlist.at(j).at(i)->mapToGlobal(QPoint(0,0))));
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
    timerEv();
}
void widgetGen::resizeEvent(QResizeEvent *e){
    resizeGL(e->size().width()*dpi,e->size().height()*dpi);
}
void widgetGen::paintGL(){
    QPainter painter(this);
#if defined (Q_OS_ANDROID)
    qDebug()<<"PAINT";
#endif
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
        //   if(prospectInfo->isEnabled()){
        //      prospectInfo->render(&painter,prospectInfo->pos());
        //  }
        //    if(noResults->isEnabled()){
        //        noResults->render(&painter,noResults->pos());
        //    }
        if(nightmode)
            painter.fillRect(rect(),QColor(0,0,0,50));
        painter.end();
    } else
        qDebug()<<"Container not ready";
}

/* DESTRUCTOR */
widgetGen::~widgetGen(){
    future.cancel();
    future.waitForFinished();
    qDebug()<<"DESTROYER:WGEN";
    delete ppi;

}
