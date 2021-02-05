#include "widgetgen.h"
widgetGen::widgetGen(QObject *parent) : QObject(parent),

    back_mutex(false),
    br_deleted(-1),
    over_brick(-1),
    over_brick_ind(-1),
    antime(130),
    filtered(""),
    scan(false),
    animating(false),
    filter_chars(0),
    vcatapultscrollspeed(0),
    menu_opened(false),
    scrr(0),
    delmutex(false),
    locked(true),
    is_expanded(false),
    old(1000),
    gen_busy(false),
    debg(0),
    fav_mode(false),
    scr(0),
    tgs(0),
    naviDis(true),
    pos(QPoint()),
    prev_pos(QPoint()),
    sh_titles(true),
    fooLab(new gLabel),
    fallDown(new gLabel)
{
    top_menu_switcher=new topMenuSwitcher();
    splash_img.load(":/gui/BANNERS/wandizz_trnsp.png");
    timer.setInterval( probe_interval );
    connect( &timer, SIGNAL( timeout() ), this, SLOT( timerEv() ));
    connect(this,SIGNAL(timershot(int)),this,SLOT(setNextShot(int)),Qt::QueuedConnection);
    tmp_list.clear();

    fallDown->type="fall";
    updtimer.setInterval(2000);
    updtimer.setSingleShot(true);
    connect(&updtimer,SIGNAL(timeout()),this,SLOT(make_poster()),Qt::QueuedConnection);
}
void widgetGen::shTitles(bool sh){
    sh_titles=sh;
    filter();
}
void widgetGen::setMRB(bool ctx,int m,int n,int o){
    glcontainer->setMrb(ctx,m);
    mfcorr=n;
    brcorr=o;
}
void widgetGen::revalidate(QPointer<gLabel> g){
    g->revalidate();
}
void widgetGen::start_bg_proc(){
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
        if(!glcontainer->isVisible())
            glcontainer->showFullScreen();
        if(!top_menu_switcher->isVisible())
            top_menu_switcher->setVisible(true);

    }
    glcontainer->showSelectors(s);
    top_menu_switcher->showSelectors(s);
}


void widgetGen::restoreCoords(){
    glcontainer->scroll(0,-glcontainer->marker_head->y());
}
void widgetGen::timerEv(bool force){
    if(isLocked() && !force)
        return;
    int scr=scrollPerform();
    if(lastscr!=scr){
        lastscr=scr;
    }
    tgs*=2;
    glcontainer->scroll( tgs, scr );
    int fh=mrh()/brct.height();
    int ft=mrh()+glcontainer->height()/brct.height();
    if(ft>lastvis)ft=lastvis;
    for(int i=fh;i<=ft;i++){
        if(item(i)->geometry()!=brick_geo(i)){
            qDebug()<<"NDEB:Correcting:"<<item(i)->geometry()<<brick_geo(i);
            item(i)->setGeometry(brick_geo(i));
        }
    }
    qDebug()<<"GL:"<<glcontainer->isVisible();
    if(!glcontainer->isVisible())glcontainer->repaint();
    if(br_deleted==-1){
        if(!pressed && scr==0 && scrr==0 && !isLocked() && glcontainer->isVisible()){
            timer.stop();
        }
    }
    if(scr!=0)setSelectors();
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

bool widgetGen::setContext(const QString& c){
  //  toggleNavi(true);
    glcontainer->setTypeID(c!="prospect");
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
    pressed=false;
    vcatapultscrollspeed = 0;
    swipe_orient = not_set;
    move = QVector2D( 0, 0 );
    br_deleted = -1;
    return nc;
}
int widgetGen::mrh(){
    return glcontainer->mrh();
}
int widgetGen::mrt(){
    return glcontainer->mrt();
}
int widgetGen::msy(){
    //   return top_menu_switcher->mapToGlobal(QPoint(0,top_menu_switcher->height())).y();
    return top_menu_switcher->y();
}
void widgetGen::lock(){
    if(locked && !isExpanded())return;
    locked=true;
    stop_bg_proc();
    toggleBricks(false);

}
void widgetGen::unlock(){
    qDebug()<<"unlocking";
    locked=false;
    start_bg_proc();
}
bool widgetGen::isLocked(){
    return locked;
}
void widgetGen::set_fav_mode(){
    fav_mode=true;
}
void widgetGen::set_prospect_params(Prospect &p){
    par=&p;
    top_menu_switcher->par=&p;
    top_menu_switcher->setupSearch();
}
void widgetGen::brick_remove( const QString t,int i){
    delmutex=true;
    br_deleted_ind=i;
    br_deleted=last_over_brick;
    stop_bg_proc();
    animating=false;
    bool animated = false;
    if( context != "videos" || context !="prospect" ){
        int acnt = 0;
        if(debg)qDebug()<<"LOOP DELETE";
        currItem()->lock();
        group = new QSequentialAnimationGroup( this );
        connect(group,SIGNAL(finished()),this,SLOT(finish_deletion()));
        for( int j = br_deleted+1;j < getItemsCount();j++ ){
            if(item(j)->isLocked())
                continue;
            animating=true;
            QPropertyAnimation* an= new QPropertyAnimation( item( j ), "geometry", this );
            an->setDuration( antime );
            an->setStartValue( item( j )->geometry() );
            an->setEndValue( item(j)->geometry().adjusted(0,-item(j)->height(),0,item(j)->height() ));
            an->setDirection( QPropertyAnimation::Direction::Forward );
            an->setEasingCurve( QEasingCurve::Linear );
            //  del_anim.append(an);
            group->addAnimation( an );
            acnt++;
            animated = true;
        }
    }
    if(debg)qDebug()<<"DELETING"<<br_deleted;
    if( animated  ) group->start(QAbstractAnimation::DeleteWhenStopped);
    else finish_deletion();
    // if(filtered!="")filter(filtered);
}
int widgetGen::getItemsCount(){
    if(itemList()==nullptr)return 0;
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
void widgetGen::filter(){
    if(context!="prospect") {
        glcontainer->prospectInfo->setEnabled(false);
        glcontainer->update();
    }
    if(top_menu_switcher->isEditEn){
        filter(top_menu_switcher->edit->text());
        return;
    } else {
        toggleBricks(true);
    }
}
void widgetGen::filter(const QString& f){
    while(delmutex){
        qDebug()<<"DELMUTEX";
        QApplication::processEvents();
    }
    qDebug()<<"FILTER::"<<context;
    filtered=f;
    int scnt=0;
    lastvis=-1;
    glcontainer->prospectInfo->setEnabled(f=="" && context=="prospect");
    glcontainer->update();
    QList<int> protect_list({});
    for(int i=0;i<getItemsCount();i++){
        qDebug()<<"--------------------------------"<<item(i)->txt_text<<filtered<<par->prospect_initial_view<<par->prospect_filter_chars;
        if((item(i)->tim_text.contains(filtered,Qt::CaseInsensitive) || item(i)->txt_text.contains(filtered,Qt::CaseInsensitive))  &&
            ((filtered.length()>=par->prospect_filter_chars || !isProspect()) || (par->prospect_initial_view && f==""))){
            qDebug()<<"Prospect filter:"<<item(i)->filename;
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
            qDebug()<<"SHTITLES:"<<(!protect_list.contains(item(i)->db_index))<<sh_titles;
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
    //   glcontainer->noResults->setEnabled(lastvis==-1);
    glcontainer->update();
}

QString widgetGen::actContext(){
    return context;
}
const QString& widgetGen::currEvent(){
    if(currItem())
        return currItem()->event_to_share;
    else return no_data_string;
}
bool widgetGen::mclick(){
    if( lastVis()<0 )return false;
    QString _oid=QString::number(currItem()->oid);
    QString _nf=currItem()->netfile;
    QString _tid = QString::number(currItem()->title_id);
    qDebug()<<"TID:"<<_tid;
    QString w_desc=currItem()->txt_text;
    QString _time;
    QString _context=currType();
    if( currType()  == "fav_items" ){
        emit go("web",{currEvent()});
        return false;;
    } else
        if( isTakingToCarousel() ){
        if( currItem()->items_cnt >0 ){
            emit go("carousel",{w_desc, _time,_nf, _oid,_tid});
        } else qDebug()<<"ITEMSCNT!";
        return false;
    }
    return true;
}
bool widgetGen::isTakingToCarousel(){
    return (currItem()->isVideos() || isFScenes() || isFVideos());
}
QPointer<gLabel> widgetGen::mglsearch(const QString lookfor){
    QList<QPointer<gLabel>> tg;
    tg.clear();
    tg.append(vb);
    tg.append(fvb);
    tg.append(fsb);
    tg.append(fib);
    tg.append(msb);
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
        gl = new gLabel( glcontainer, mod_name ,context );
        gl->poster_data_ready=&poster_data_ready;
        tmp_list.append(gl);
        gl->vprect=rct;
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
    qDebug()<<"MAKE:"<<gl->index<<gl->db_index;

    if(isVideos(mod_name)){
        gl->fav_toggled=fav_videos_list.contains( tid );
    }
    if(!glcontainer->isVisible()){
        qDebug()<<"GLCONT NOT VIS";
        glcontainer->raise();
        glcontainer->showFullScreen();
    }
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
void widgetGen::setupWrapper(QList<QList<QWidget *>> wl){
    if(glcontainer)
        glcontainer->setupWrapper(wl);
    else {
        qDebug()<<"Critical:no glcontainer";
        QApplication::quit();
    }
}

void widgetGen::connectSignals(gLabel* gl){
    connect( gl, SIGNAL( download( const QString&, const QString&,const int,bool) ), this,
            SIGNAL( download( const QString&, const QString&,const int,bool ) ) );
    connect( gl, SIGNAL( hover( int, int ) ), this,  SLOT( handleEnter( int, int ) ));
    connect( gl, SIGNAL( leaved() ), this, SLOT( handleLeave() ) );
    connect( gl, SIGNAL( toggle_fav_video (int, bool)), this, SIGNAL(toggle_fav_video(int,bool)));
    connect( gl, SIGNAL( del_fav( const QString&,int) ), this, SLOT( brick_remove( const QString&, int )) );
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
void widgetGen::handleLeave(){
    //  over_brick=-1;
}
void widgetGen::handleEnter( int e, int f ){
    qDebug()<<"WGEN HANDLEENTER:"<<e<<f;
    over_brick = e;
    over_brick_ind = f;
    last_over_brick=e;
    if(top_menu_switcher->isEditEn){
        if(QApplication::inputMethod()->isVisible()){
            top_menu_switcher->edit->releaseKeyboard();
            QApplication::inputMethod()->hide();
        }
    }
}
void widgetGen::toggleBricks(bool w){
    while(delmutex){
        qDebug()<<"DELMUTEX";
        QApplication::processEvents();
    }
    //  toggleNavi(false);
    is_expanded=w;
    lastvis=-1;
    qDebug()<<"Toggle bricks.Context:"<<context<<w<<getItemsCount();
    for(int i=0;i<getItemsCount();i++){
        QPointer<gLabel> gl(item(i));
        toggle(gl,w,i);
        if(w)lastvis=i;
    }
}
void widgetGen::toggle(gLabel* g,bool s,int i){
    //qDebug()<<"TOGGLE:"<<g->index<<g->filename;
    /* UNLOCK */

    if(s){
        if(i>-1){
            g->setGeometry(brick_geo(i));
            g->setFixedSize(brick_geo().size());
        }
        glcontainer->marker_tail->move(0,g->geometry().bottom());
        g->unlock();
        g->show();
        g->raise();
        qDebug()<<"Unlock brick:"<<g->index<<g->isVisible();
        QApplication::processEvents();
        qDebug()<<"Unlock aftebrick:"<<g->index<<g->isVisible();
        g->update();
        qDebug()<<"VISIBLE1:"<<i<<glcontainer->isVisible()<<g->isVisible();
        if(!glcontainer->isVisible())glcontainer->show();
        qDebug()<<"VISIBLE2:"<<i<<glcontainer->isVisible()<<g->isVisible();
        if(!glcontainer->isVisible())glcontainer->raise();
        qDebug()<<"VISIBLE3:"<<i<<glcontainer->isVisible()<<g->isVisible();
        if(!g->isVisible())g->show();
        qDebug()<<"VISIBLE4:"<<i<<glcontainer->isVisible()<<g->isVisible();
        if(!g->isVisible())g->raise();
        
        
    } else {
        /* LOCK */
        g->lock();
        g->setVisible(false);
        //       g->update();
    }
    //    g->forceUpdate();
    //    glcontainer->forceUpdate();
    //   timerEv(true);
    //    qDebug()<<"TOGGLE:"<<s<<glcontainer->childrenRect()<<glcontainer->childrenRegion()<<glcontainer->sizeHint()<<glcontainer->frameGeometry();
}
void widgetGen::activate(){
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
    unlock();
    filter();
  //  toggleNavi(false);
    setSelectors();
    top_menu_switcher->showKB();
}
void widgetGen::setDirector(QWidget *w){
    //  glcontainer->setGeometry(brct);

    top_menu_switcher->setParent(w);
    disconnect(top_menu_switcher,SIGNAL(textChanged(const QString&)),this,SLOT(filter(const   QString&)));
    connect(top_menu_switcher,SIGNAL(textChanged(const QString&)),this,SLOT(filter(const   QString&)));
}
void widgetGen::toggleBricks(){
    toggleBricks(true);
}
bool widgetGen::isExpanded(){
    return is_expanded;
}
void widgetGen::noticeShow(){
    animating=false;
}
QRect widgetGen::brick_geo(int i){
    return QRect(0, brcorr + glcontainer->mrb()+i* brct.height(),brct.width(), brct.height());
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
void widgetGen::share(){
    QString text = currItem()->txt_text;
    QUrl url( currEvent());
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
    if(debg)qDebug()<<"RCT:"<<rct<<glcontainer->geometry();
}
void widgetGen::setContainer(widget_wrapper* glc){
    glcontainer=static_cast<widget_wrapper*>(glc);
}
bool widgetGen::manageSelectors(){
    return glcontainer->mrb() >= mfcorr;
}
QPointer<gLabel> widgetGen::producerOf(int i){
    if(i==-1)
        i=over_brick;
    int oid=item(i)->oid;
    for(int i=0;i<getItemsCount();i++){
        if(item(i)->db_index==oid){
            return item(i);
        }
    }
    return QPointer<gLabel>();
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
    return QPointer<gLabel>();
}
QString widgetGen::currType(){
    qDebug()<<currItem()<<last_over_brick<<over_brick;
    if(currItem()!=nullptr)
        return currItem()->type;
    else return "";
}
QPointer<gLabel> widgetGen::currItem(){
    if(last_over_brick!=-1){
        //     qDebug()<<"OVER BRICK::"<<over_brick;
        return item( last_over_brick );
    }
    else
    {
        qDebug()<<"CURRITEM NULL";
        return fallDown;
    }
}

void widgetGen::finish_deletion(){
    lastvis=-1;
    emit set_cnt(getItemsCount()-1);
    QString tp=item(br_deleted)->type;
    tp.replace("fav_","fav");
    if(itemList()!=nullptr)
    itemList()->removeAt(br_deleted);
    else qDebug()<<"DELETE:CRITICAL!";
    animating=false;
    int j,cn=0;
    for( j = 0;j<getItemsCount();j++ ){
        if(!item(j)->isLocked()){
            if(debg)qDebug()<<"---- afer delete positioning brick nr"<<j<<item(j)->index<<" setting to:"<<cn;
            item( j )->setGeometry(brick_geo( cn ));
            item( j )->index=cn;
            lastvis=cn;
            cn++;
        }
        else qDebug()<<"---- locked item "<<j<<" omit.";
    }
    br_deleted=-1;

    emit del_fav(tp,br_deleted_ind);

    qDebug()<<"DELETE:"<<tp<<over_brick_ind;
    delmutex=false;
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
        qDebug()<<"genbusy";
        make_poster();
    }


}
void widgetGen::make_poster(){
    if(poster_data.count()==0){
        gen_busy=false;
        return;
    }
    gen_busy=true;
    future=(QtConcurrent::run([=]() {
        QMutexLocker lock{&_mutex};
        /* GRAB DATA STRUCTURE */
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
                emit download(pdata.Url,pdata.file,pdata.index,1);
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
        if(ppi->sizeInBytes()==0 || ppi->isNull())   emit reval (item(pdata.index,pdata.context));
        int pdl=poster_data_ready.count();
        poster_data_ready.append({pdata.index,pdata.type,pdata.file,ppi,pdata.context,true});
        if(pdl<=poster_data_ready.count()){
            //        qDebug()<<"POSTER:"<<pdata.index;
        }
        else qDebug()<<"POSTER:!!!!!!!!!!!!!!!!!!!!!!!!ERROR:";
        emit timershot(50);
    }));
}
void widgetGen::setNextShot(int t){
    QTimer::singleShot(t, this, SLOT(make_poster()));
}
void widgetGen::forceUpdate(){
    glcontainer->forceUpdate();
    top_menu_switcher->forceUpdate();
}
/* MOUSE MANAGEMENT */
bool widgetGen::mouseOver(QWidget* w){
    return QRect(w->mapToGlobal(QPoint(0,0)),w->size()).contains(QCursor::pos());
}
void widgetGen::mousePress(QMouseEvent* e){
    if(isLocked())return;
    pressed=true;
    ptimer.start( 300 );
    if(over_brick>=0){
        diff.setX( 0 );
        diff.setY( 0 );
        pos = e->globalPos();
        prev_pos = pos;
        touch_pos = pos;
        if(scrr!=0){
            catapult_brakes=true;
            no_click=true;
            scrr=0;
        }
    }
    last_over_brick=over_brick;
    start_bg_proc();
}
bool widgetGen::mouseRelease(QMouseEvent *e){
    if(isLocked())return false;
    if(menu_opened){
        emit closeMenu();
        glcontainer->activateWindow();
        glcontainer->raise();
        if(!glcontainer->isVisible())glcontainer->showFullScreen();
        return false;
    }
    pressed=false;
    bool st=true;
    if(over_brick>=0){
        if(vcatapultscrollspeed!=0 || swipe_orient==vertical || scr!=0){
            vcatapultscrollspeed = get_vscroll();
            no_click=true;
            st=false;
        } else if(ptimer.isActive() && !no_click && swipe_orient==not_set){
            ptimer.stop();
            if( e->globalPos().x() <=currItem()->width()*4/5 ){
                st=mclick();
            } else{
                if( context=="videos" ){
                    currItem()->toggle_fav(true);
                    st=false;
                } else {
                    if(context.contains("fav")){
                        share();
                        st=false;
                    }
                }
            }
        }
    } else {
        if(ptimer.isActive()){
            if(mouseOver(top_menu_switcher)){
                st=top_menu_switcher->get_hover(e->globalPos());
            } else if(top_menu_switcher->isEditEn){
                top_menu_switcher->edit->releaseKeyboard();
            }
            if(st)
                st=glcontainer->get_hover(e->globalPos());
        }
    }
    swipe_orient=not_set;
    no_click=false;
    over_brick=-1;
    return st;
}
void widgetGen::mouseMove(QMouseEvent *e){
    if(isLocked()){
        qDebug()<<"LOCKED";
        return;
    }
    if(over_brick==-1){
        return;
    }
    pos = e->globalPos();
    move = QVector2D( pos-prev_pos );
    diff = QVector2D( pos-touch_pos );
    prev_pos = pos;
    get_swipe_predicter();
    if( swipe_orient == vertical){
        if( lastVis()>-1 ){
            scrr += double( move.y() );
            append_vscroll( double( move.y() ) );
        }
        return;
    }
}
bool widgetGen::isMousePressed(){
    return pressed;
}
/* SWIPE PREDICTER */
void widgetGen::get_swipe_predicter(){
    if( lastVis()<0 )return;
    if( swipe_orient != not_set )return;
    no_click = false;
    if( abs( move.x() ) <= abs( move.y() ) && abs( diff.y() )>2*dpi ){
        no_click = true;
        if( item( lastVis() )->geometry().bottom()-mrh()>rct.height() ){
            swipe_orient = vertical;
            if(over_brick>-1)currItem()->is_gesture=false;
            return;
        }
    } else
        if( abs( move.x() )>abs( move.y() ) && !offline && abs( diff.x() )>3*dpi ){
        swipe_orient = horizontal;
        if(over_brick>-1)currItem()->is_gesture=true;
        if(currType()  == "videos" )return;
        no_click = true;
    }
}
/* SCROLLER MANAGEMENT */
int widgetGen::scrollPerform(){
    if(isLocked())return 0;
    if( !pressed ){
        rubberbandscrollspeed = 0;
        int tail=glcontainer->marker_tail->y();
        int head=glcontainer->marker_head->y();
        if(head!=0)
            // too high
            if( head>0 ){
                rubberbandscrollspeed = -head / 2;
                vcatapultscrollspeed /= 2;
                if( abs( rubberbandscrollspeed )<1 ){
                    rubberbandscrollspeed = -head;
                }
            }
        // too low
        if( tail<rct.height() && head<0 ){
            vcatapultscrollspeed /= 2;
            rubberbandscrollspeed = ( rct.height()-tail )/2;
            if( abs( rubberbandscrollspeed )<1 ){
                rubberbandscrollspeed = rct.height()-tail;
            }
        }
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
    return scr;
}
gLabel* widgetGen::lastVisibleItem(){
    if(lastVis()==-1 || getItemsCount()==0 || lastVis()>=getItemsCount()){
        return fallDown;
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
    qDebug()<<"CA:"<<(currItem()->isVideos() || (par->prospect_favs && currItem()->isProducerType() && currItem()->isVideosType()));
    return currItem()->isVideos() || (par->prospect_favs && currItem()->isProducerType() && currItem()->isVideosType());
}
bool widgetGen::canToggleShares(){
    return (!currItem()->isProducerType() && !currItem()->isVideosType()) ||
           (par->prospect_shares && currItem()->isProducerType() && currItem()->isProducer() &&
            currType()!="videos");
}

/* DESTRUCTOR */
widgetGen::~widgetGen(){
    future.cancel();
    future.waitForFinished();
    qDebug()<<"DESTROYER:WGEN";
    delete ppi;

}
