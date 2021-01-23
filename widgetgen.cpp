#include "widgetgen.h"
widgetGen::widgetGen(QObject *parent) : QObject(parent),

    back_mutex(false),
    br_deleted(-1),
    over_brick(-1),
    over_brick_ind(-1),
    antime(300),
    filtered(""),
    animating(false),
    lastvis(-1),
    filter_chars(0),
    vcatapultscrollspeed(0),
    menu_opened(false),
    locked(true),
    is_expanded(false),
    gen_busy(false),
    fav_mode(false),
    debg(0),
    scrr(0),
    scr(0),
    tgs(0),
    naviDis(true),
    pos(QPoint()),
    prev_pos(QPoint()),
    sh_titles(true),
    fooLab(new gLabel)

{
    glcontainer=new widget_wrapper();
    top_menu_switcher=new topMenuSwitcher();
    group = new QParallelAnimationGroup( this );
    connect(group,SIGNAL(finished()),this,SLOT(finish_deletion()));
    connect(this,SIGNAL(send_item(const int)),this,SLOT(put_poster(const int)),Qt::QueuedConnection);
    connect(this,SIGNAL(do_poster()),this,SLOT(make_poster()),Qt::QueuedConnection);
    splash_img.load(":/gui/BANNERS/wandizz_trnsp.png");
    star->addPixmap(QPixmap(":/gui/icons/green_dot_no_dot.png") );
    star->addPixmap(QPixmap( ":/gui/icons/green_dot.png"));
    timer.setInterval( probe_interval );
    connect( &timer, SIGNAL( timeout() ), this, SLOT( timerEv() ));
    tmp_list.clear();
    marker_middle.setFixedSize(0,0);
    marker_last.setFixedSize(0,0);
}
void widgetGen::shTitles(bool sh){
    sh_titles=sh;
    filter();
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
    qDebug()<<"tll:"<<top_menu_switcher->geometry();
    if(f){
        if(!glcontainer->isVisible())
            glcontainer->show();
        if(!top_menu_switcher->isVisible())
            top_menu_switcher->show();

    }
    glcontainer->showSelectors(s);
    top_menu_switcher->showSelectors(s);
    qDebug()<<"tll:"<<top_menu_switcher->geometry();
}
void widgetGen::timerEv(bool force){
    if(isLocked() && !force)
        return;
    int scr=scrollPerform();
    if(lastscr!=scr){
        lastscr=scr;
    }
    // if(tgs!=0)scr=0;
    tgs*=2;
    // if(tgs!=0 || scr!=0)
    glcontainer->scroll( tgs, scr );
    if(glcontainer->marker_head->x()>rct.width()){
        //   lock();
        toggleBricks(false);
    }
    if(br_deleted==-1){
        if(!pressed && scr==0 && scrr==0 && !isLocked()){
            timer.stop();
        }
    }
    if(scr!=0)setSelectors();
}
void widgetGen::setupTopMenuSwitcher(QRect r){
    top_menu_switcher->setGeometry(r);
    top_menu_switcher->setFixedSize(r.size());
    top_menu_switcher->updateGeometry();
}
void widgetGen::setFrames(QWidget *f1, QWidget *f2){
    top_menu_switcher->setFrames(f1,f2);
}
bool widgetGen::setContext(const QString& c){
    qDebug()<<"GEOO"<<mrh()<<glcontainer->marker_head->geometry()<<glcontainer->marker_head->mapToGlobal(QPoint(0,0))<<marker_middle.geometry()<<marker_last.geometry();
    /* Set proper context */
    /* first disable navigation for a moment */
    //  if(mrh()!=0){
    //   qDebug()<<"CORRECTING mrh:"<<mrh();
      glcontainer->marker_head->move(glcontainer->mapFromGlobal(QPoint(0,0)));
      qDebug()<<"GEOO"<<mrh()<<glcontainer->marker_head->geometry()<<glcontainer->marker_head->mapToGlobal(QPoint(0,0))<<marker_middle.geometry()<<marker_last.geometry();
    // }
    toggleNavi(true);
    if(debg)qDebug()<<"OLD CONTEXT:"<<context<<"new:"<<c;
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
    if(debg)qDebug()<<"WGEN CONTEXT:"<<context<<nc;
    qDebug()<<"tll:"<<top_menu_switcher->geometry();
    return nc;
}
int widgetGen::mrh(){
    if(glcontainer->mrh()!=0)
        //    qDebug()<<"RH:"<<glcontainer->marker_head->pos();
        return glcontainer->mrh();
}
int widgetGen::mrt(){
    qDebug()<<"RH:"<<glcontainer->marker_tail->pos();
    return corr_y+glcontainer->mrt();
}
int widgetGen::msy(){
    //   return top_menu_switcher->mapToGlobal(QPoint(0,top_menu_switcher->height())).y();
    return top_menu_switcher->y();
}
void widgetGen::lock(){
    if(locked)return;
    locked=true;
    stop_bg_proc();
    toggleBricks(false);
}
void widgetGen::unlock(){
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
void widgetGen::brick_remove( ){
    // stop_bg_proc();
    br_deleted = last_over_brick;
    bool animated = false;
    if( context != "videos" || context !="prospect" ){
        int acnt = 0;
        if(debg)qDebug()<<"LOOP DELETE";
        for( int j = br_deleted+1;j < lastBrick()+1;j++ ){
            animating=true;
            if(debg)qDebug()<<"ADING ANIM DEL:"<<item(j)->geometry()<<brick_geo((j));
            del_anim.append( new QPropertyAnimation( item( j ), "geometry", this ) );
            del_anim.last()->setDuration( antime );
            del_anim.last()->setStartValue( item( j )->geometry() );
            del_anim.last()->setEndValue( brick_geo(j-1) );
            del_anim.last()->setDirection( QPropertyAnimation::Direction::Forward );
            del_anim.last()->setEasingCurve( QEasingCurve::Linear );
            group->addAnimation( del_anim.last() );
            acnt++;
            animated = true;
            group->start();
        }
        if(debg)qDebug()<<"DELETING"<<br_deleted<<lastBrick();
        if( !animated  )finish_deletion();
    }

    if(filtered!="")filter(filtered);
}
int widgetGen::getItemsCount(){
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
    if(top_menu_switcher->isEditEn){
        filter(top_menu_switcher->edit->text());
        return;
    } else {
        toggleBricks(true);
    }
}
void widgetGen::filter(const QString& f){
    qDebug()<<"FILTER::"<<context;
    filtered=f;
    int scnt=0;
    lastvis=-1;
    QList<int> protect_list({});
    for(int i=0;i<getItemsCount();i++){
        if(debg)qDebug()<<"--------------------------------"<<item(i)->txt_text<<filtered<<par->prospect_initial_view<<par->prospect_filter_chars;
        if(((item(i)->tim_text.contains(filtered,Qt::CaseInsensitive) || item(i)->txt_text.contains(filtered,Qt::CaseInsensitive))
            && filtered.length()>=par->prospect_filter_chars) || !isProspect() || (par->prospect_initial_view && f=="")){
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
            if(!protect_list.contains(item(i)->db_index) && sh_titles){
                toggle(item(i),true,scnt);
                scnt++;
                if(debg)qDebug()<<"BRICK:"<<mrh()<<brick_geo(scnt)<<scnt;
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
    is_expanded=true;
    glcontainer->update();
}
void widgetGen::give_event(){
    currItem()->catch_event();
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
            }
            return false;
        }
    return true;
}
bool widgetGen::isTakingToCarousel(){
    return (currItem()->isVideos() || isFScenes());
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
        tmp_list.append(gl);
        ft=true;
    }
    // toggle(gl,par->prospect_initial_view || context!="prospect");
    if(debg)qDebug()<<"Brick geo:"<<index<<gl->geometry();
    gl->setObjectName(mod_name);
    gl->setGeometry(brick_geo(index));
    gl->event_to_share=event;
    if(ft){
        gl->set_prospect_params(*par);
        gl->setDPI(dpi);
        gl->set_params( index, di, itcnt,tid,imgfile, imgurl, up, dn,splash_img, titlemedia,od);
        connectSignals(gl);
    } else{
        gl->setSilent(index,mod_name,context);
    }
    if(isVideos(mod_name)){
        gl->fav_toggled=fav_videos_list.contains( tid );
    }

}
void widgetGen::setupWrapper(QList<QList<QWidget *>> wl){
    if(glcontainer)
        glcontainer->setupWrapper(wl);
    else {
        qDebug()<<"Critical:no glcontainer";
        QApplication::quit();
    }
}
void widgetGen::hideRastered(QWidget *cw){
    QList<QWidget*> uilist=cw->findChildren<QWidget*>();
    for(int i=0;i<uilist.count();i++){
        if(debg)qDebug()<<"Hiding:"<<uilist.at(i)->objectName();
        uilist.at(i)->setVisible(false);
    }
    cw->parentWidget()->update();
}
void widgetGen::connectSignals(gLabel* gl){
    connect( gl, SIGNAL( download( const QString&, const QString&,const int,bool) ), this,
             SIGNAL( download( const QString&, const QString&,const int,bool ) ) );
    connect( gl, SIGNAL( hover( int, int ) ), this,  SLOT( handleEnter( int, int ) ));
    connect( gl, SIGNAL( leaved() ), this, SLOT( handleLeave() ) );
    connect( gl, SIGNAL( toggle_fav_video (int, bool)), this, SIGNAL(toggle_fav_video(int,bool)));
    connect( gl, SIGNAL( del_fav( const QString&,int) ), this, SLOT( brick_remove( )) );
    connect( gl, SIGNAL( hided() ), this,  SIGNAL( hided()) );
    connect( gl, SIGNAL( get_poster(int,const QString&,const QString&,const QString&,const QString&,const QString&,int)),
             this, SLOT( poster_generator(int, const QString&, const QString&,const QString&,const QString&,const QString&,int)),Qt::QueuedConnection);
    connect(gl, SIGNAL( settled(int)),this,SLOT(removeTempData(int)),Qt::QueuedConnection);

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
    over_brick=-1;
}
void widgetGen::handleEnter( int e, int f ){
    over_brick = e;
    over_brick_ind = f;
    last_over_brick=e;
    if(QApplication::inputMethod()->isVisible()){
        top_menu_switcher->edit->releaseKeyboard();
        QApplication::inputMethod()->hide();
    }
}
void widgetGen::toggleBricks(bool w){
    if(!w && !is_expanded)return;
    toggleNavi(false);
    if(!w){
        stop_bg_proc();
        top_menu_switcher->setParent(0);
        glcontainer->setParent(0);   }
    is_expanded=w;
    lastvis=-1;
    qDebug()<<"Toggle bricks.Context:"<<context<<w<<getItemsCount();
    for(int i=0;i<getItemsCount();i++){
        QPointer<gLabel> gl(item(i));
        toggle(gl,w,i);
        if(w)lastvis=i;
    }
    //if(tgs!=0){
    //  tgs=0;
    //  emit go( "back",{} );
    //}
}
void widgetGen::toggle(gLabel* g,bool s,int i){
    if(s){
        if(i>-1){
            g->setGeometry(brick_geo(i));
            g->setFixedSize(brick_geo().size());
        }
   //     qDebug()<<"Unlock Brick:"<<i<<g->index;
        g->unlock();
        g->show();
        glcontainer->marker_tail->move(0,g->geometry().bottom());
    } else {
   //     qDebug()<<"Lock Brick:"<<i<<g->index;
        g->lock();
        g->hide();
    }
    g->forceUpdate();
    //  qDebug()<<"Brick:"<<g->index<<g->isVisible();
    glcontainer->forceUpdate();
    timerEv(true);
}
void widgetGen::activate(bool sh){
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
    toggleNavi(false);
    top_menu_switcher->setSearchMode(sh);
    manageSelectors();
}
void widgetGen::setDirector(QMainWindow *w){
    if(debg)qDebug()<<"SET DIRECTOR:"<<w->objectName();
    glcontainer->setGeometry(brct);
    glcontainer->setParent(w);
    top_menu_switcher->setParent(w);
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
    return QRect(0, mrh() + brcorr +i* brct.height(),brct.width(), brct.height());
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
        if( context  == "fav_videos")return &fvb;
        else
            if( context  == "fav_scenes"  )return &fsb;
            else
                if( context  == "fav_items"  )return &fib;
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
void widgetGen::setCoords(QRect rt,int d){
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
    // must be called before the widget or its parent window gets shown
    if(debg)qDebug()<<"RCT:"<<rct<<glcontainer->geometry();
    glcontainer->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    glcontainer->setFixedSize( (rct.size()+QSize(0,dpi)));
    connect(top_menu_switcher,SIGNAL(textChanged(const QString&)),this,SLOT(filter(const   QString&)));
    glcontainer->setGeometry(rct);
}
void widgetGen::setMFCorr(int m,int n){
    mfcorr=m;
    brcorr=n;
    glcontainer->vcorr=n;

}
bool widgetGen::manageSelectors(){
    return msy()-mrh() <= mfcorr;
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
    if(currItem()!=nullptr)
        return currItem()->type;
    else return "";
}
QPointer<gLabel> widgetGen::currItem(){
    if(over_brick!=-1){
        //     qDebug()<<"OVER BRICK::"<<over_brick;
        return item( over_brick );
    }
    else
    {
        return nullptr;
    }
}

void widgetGen::finish_deletion(){
    lastvis=-1;
    QString tp=item(br_deleted)->type;
    itemList()->removeAt(br_deleted);
    animating=false;
    if(debg)qDebug()<<"afer delete positioning "<<lastBrick()+1;
    int j;
    for( j = 0;j<getItemsCount();j++ ){
        if(debg)qDebug()<<"afer delete positioning brick nr"<<j<<item(j)->index;
        item( j )->index=j;
        item( j )->setGeometry(brick_geo( j ));
        if(!item(j)->isLocked())lastvis=j;
    }
    br_deleted=-1;
    emit del_fav(tp,over_brick_ind);

}
int widgetGen::lastBrick(){
    return itemList()->count()-1;
}


const QString& widgetGen::getContext(){
    return context;
}


/* BRICK & POSTER FACTORY */
void widgetGen::poster_generator(int ind, const QString& type, const QString& fn,const QString& up,const QString& dn,const QString& cn,int itcnt){
    poster_data.append({ind,type,fn,up,dn,cn,itcnt});
    //   if(debg)qDebug()<<"POSTERS WAITING:"<<poster_data.count()<<"ready:"<<poster_data_ready.count();
    if(!gen_busy){
        gen_busy=true;
        emit do_poster();
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
void widgetGen::make_poster(){
    future = QtConcurrent::run([=]() {
        QMutexLocker lock{&_mutex};
        while(!poster_data.isEmpty()){
            if(poster_data.count()==0){
                if(debg)qDebug()<<"NO DATA POSTER";
                break;
            }
            /* load poster image */
            PosterData pdata=poster_data.takeFirst();
            const QImage* _p=new QImage(pdata.file);
            if(debg)qDebug()<<"LOADED:"<<_p->sizeInBytes();
            if(_p->isNull()){
                /* remove from external mem if faulty */
                qDebug()<<"P is null:"<<pdata.type<<pdata.context<<pdata.file;
                QFile::remove(pdata.file);
                emit reval (item(pdata.index,pdata.context));
                break;
            }
            ppt=new QImage(lw,lh,QImage::Format_ARGB6666_Premultiplied);
            ppi=new QImage(lw,lh,QImage::Format_ARGB6666_Premultiplied);
            /* layout */
            if(pdata.type=="videos"){
                tim_align=Qt::AlignRight;
            } else {
                tim_align=Qt::AlignCenter;
            }


            /* aspectratios */
            if(pdata.type=="fav_items" || pdata.type=="prospect")
                _prop=1;
            else
                _prop=1.75;
            _lw=lh*_prop;
            _lw2=lw-_lw-(6*htm);


            /* look & feel */
            double fct=1;
            if(pdata.type=="prospect"){
                txt_font.setPixelSize(25.0*dpi);
                txt_font.setBold(true);
                txt_rect.setRect(_lw+htm,vtm,lw-_lw-2*htm,lh*.3);
                ppt->fill(QColor(0,0,0,255));
            } else {
                txt_font.setPixelSize(18.0*dpi);
                txt_font.setBold(false);
                txt_rect.setRect((_lw+3*htm),vtm,_lw2,lh*.5);
                ppt->fill(QColor(0,0,0,150));
                fct=.8;
            }
            tim_rect.setRect(txt_rect.left(),txt_rect.bottom(),txt_rect.width()*fct,lh-txt_rect.height());
            tim_font.setPixelSize(17*dpi);


            /* factory */
            QPainter painter;
            painter.begin(ppt);
            painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
            painter.fillRect(ppt->rect(),QBrush(QColor(0,0,0,0)));
            if(pdata.type=="fav_items" || pdata.type=="prospect"){
                painter.setBrush(QColor(255,255,255,255));
                painter.drawRect(QRect(0,0,_lw,lh));
                painter.setBrush(QBrush());
                double scale=double(_p->height())/double(lh);
                QSize ss(_p->size()/scale);

                painter.drawImage(QRect((_lw-ss.width())/2,(lh-ss.height())/2,ss.width(),ss.height()),*_p,QRectF(),Qt::ImageConversionFlag::AvoidDither);
            } else
                painter.drawImage(QRect(0,0,_lw,lh),*_p);
            painter.end();

            ppi->fill(QColor(0,0,0,0));
            painter.begin(ppi);
            painter.setBrush(*ppt);
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
                QFont cfont(tim_font);
                cfont.setPixelSize(par->videos_coming_soon_text_size*dpi);;
                QBrush b(painter.brush());
                painter.setBrush(QBrush(QColor(0,0,0,25*par->videos_coming_soon_picture_dim)));
                painter.drawRoundedRect(0,0,lw,lh,10*dpi,5*dpi);
                painter.setBrush(b);
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
            poster_data_ready.append({pdata.index,pdata.type,ppi,pdata.context});
            delete _p;
            delete ppt;
            emit send_item(poster_data_ready.count()-1);
        }
    });
}
void widgetGen::forceUpdate(){
    glcontainer->forceUpdate();
    top_menu_switcher->forceUpdate();
}
void widgetGen::put_poster(int i){
    gen_busy=false;
    if(back_mutex)return;
    if(item(poster_data_ready.at(i).index,poster_data_ready.at(i).context)){
        item(poster_data_ready.at(i).index,poster_data_ready.at(i).context)->settle_poster(poster_data_ready.at(i).poster);
    }
}
/* MOUSE MANAGEMENT */
void widgetGen::removeTempData(int i){
    poster_data_ready.removeAt(i);
}
bool widgetGen::mouseOver(QWidget* w){
    return QRect(w->mapToGlobal(QPoint(0,0)),w->size()).contains(QCursor::pos());
}
void widgetGen::mousePress(QMouseEvent* e){
    if(isLocked())return;
    if(over_brick>=0){
        pressed=true;
        diff.setX( 0 );
        diff.setY( 0 );
        ptimer.start( 300 );
        pos = e->globalPos();
        prev_pos = pos;
        touch_pos = pos;
        if(scrr!=0){
            catapult_brakes=true;
            no_click=true;
            scrr=0;
        }
    }
    start_bg_proc();
}
bool widgetGen::mouseRelease(QMouseEvent *e){
    if(isLocked())return false;
    if(menu_opened){
        emit closeMenu();
        return false;
    }
    pressed=false;
    bool st=true;
    if(over_brick>=0){
        if(vcatapultscrollspeed!=0 || swipe_orient==vertical || scr!=0){
            vcatapultscrollspeed = get_vscroll();
            no_click=true;
            st=false;
        } else {
            if(ptimer.isActive() && !no_click && swipe_orient==not_set){
                ptimer.stop();
                if(debg)qDebug()<<"MCLICK:"<<e->globalPos().x()<<currItem()->width()*4/5;
                if( e->globalPos().x() <=currItem()->width()*4/5 ){
                    st=mclick();
                } else{
                    if(debg)qDebug()<<currItem()->type;
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
        }
    } else {
        if(mouseOver(top_menu_switcher))
            st=top_menu_switcher->get_hover(e->globalPos());
        else{
            if(top_menu_switcher->isEditEn){
                top_menu_switcher->edit->releaseKeyboard();
                st=glcontainer->get_hover(e->globalPos());
            }
        }
    }
    swipe_orient=not_set;
    no_click=false;

    return st;
}
void widgetGen::mouseMove(QMouseEvent *e){
    if(isLocked())return;
    if(over_brick==-1)
        return;
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
        else {
            swipe_orient=not_set;
        }
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
        if(debg)
            qDebug()<<item( lastVis() )->geometry().bottom()<<mrh()<<rct.height()<<item( lastVis() )->geometry().bottom()-mrh()<<rct.height()<< (item( lastVis() )->geometry().bottom()-mrh()>rct.height() );
        if( item( lastVis() )->geometry().bottom()-mrh()>rct.height() ){
            swipe_orient = vertical;
            return;
        }
    } else
        if( abs( move.x() )>abs( move.y() ) && !offline && abs( diff.x() )>3*dpi ){
            swipe_orient = horizontal;
            if(currType()  == "videos" )return;
            give_event();
            no_click = true;
        }
}
/* SCROLLER MANAGEMENT */
int widgetGen::scrollPerform(){
    if(isLocked())return 0;
    if( !pressed ){
        rubberbandscrollspeed = 0;
        //  int tail = marker_last.y();
        //qDebug()<<"head2s:"<<glcontainer->mapToGlobal(glcontainer->marker_head->pos())<<glcontainer->mapToGlobal(glcontainer->marker_tail->pos());


        int tail=glcontainer->marker_tail->y();
        int head=glcontainer->marker_head->y();
        if(head!=0)
            // qDebug()<<head<<tail<<rct.height();
            // too high
            if( head>0 ){
                //  qDebug()<<"HEAD:"<<head<<tail;
                rubberbandscrollspeed = -head / 2;
                vcatapultscrollspeed /= 2;
                if( abs( rubberbandscrollspeed )<1 ){
                    rubberbandscrollspeed = -head;
                }
            }
        // too low
        if( tail<rct.height() && head<0 ){
            //      qDebug()<<"tail:"<<head;
            vcatapultscrollspeed /= 2;
            rubberbandscrollspeed = ( rct.height()-tail )/2;
            if( abs( rubberbandscrollspeed )<1 ){
                rubberbandscrollspeed = rct.height()-tail;
            }
        }
        if(rubberbandscrollspeed!=0){
            //        qDebug()<<"Rubber:"<<rubberbandscrollspeed<<mrh();
        }
        scrr += rubberbandscrollspeed;
        rubberbandscrollspeed = 0.0;
        if( fabs( vcatapultscrollspeed ) < 1 ){
            vcatapultscrollspeed = 0.0;
        } else {
            vcatapultscrollspeed *= 0.96;//+boost;
        }
        scrr += vcatapultscrollspeed;
        //    if(vcatapultscrollspeed!=0)
        //      qDebug()<<"vcatapult:"<<vcatapultscrollspeed<<mrh();
    }
    scr=scrr;
    scrr=0;
    return scr;
}
gLabel* widgetGen::lastVisibleItem(){
    if(lastVis()==-1 || getItemsCount()==0){
        return nullptr;
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
    future.waitForFinished();
    while(back_mutex){
        qDebug()<<"BACK MUTEX";
        QApplication::processEvents();
    }
    qDebug()<<"DESTROYER:WGEN";
    delete ppt;
}
