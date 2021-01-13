#include "widgetgen.h"
widgetGen::widgetGen(QObject *parent) : QObject(parent),

    back_mutex(false),
    br_deleted(-1),
    over_brick(-1),
    over_brick_ind(-1),
    antime(100),
    filtered(""),
    animating(false),
    lastVis(-1),
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
    pos(QPoint()),
    prev_pos(QPoint())
{
    group = new QParallelAnimationGroup( this );
    connect(group,SIGNAL(finished()),this,SLOT(finish_deletion()));
    connect(this,SIGNAL(send_item(const int)),this,SLOT(put_poster(const int)),Qt::QueuedConnection);
    connect(this,SIGNAL(do_poster()),this,SLOT(make_poster()),Qt::QueuedConnection);
    // splash_img.load(":/gui/BANNERS/wandizz_trnsp.png");
    star->addPixmap(QPixmap( ":/gui/icons/green_dot_no_dot.png" ));
    star->addPixmap(QPixmap( ":/gui/icons/green_dot.png"));
    timer.setInterval( probe_interval );
    connect( &timer, SIGNAL( timeout() ), this, SLOT( timerEv() ));
}
void widgetGen::start_bg_proc(){
    timer.start(probe_interval);
}
void widgetGen::stop_bg_proc(){
    if( !timer.isActive() )return;
    timer.stop();
}
void widgetGen::timerEv(bool force){
    if( locked && !force){
        qDebug()<<"LOCK:"<<isExpanded()<<lastVis;
        return;
    }
    int scr=scrollPerform();
    if(lastscr!=scr){
        lastscr=scr;
    }
    glcontainer->scroll( 0, scr );
    bool s=manageSelectors();
    if(lasttoggl!=s){
        glcontainer->showSelectors(s);
        top_menu_switcher->showSelectors(s);
        lasttoggl=s;
    }
    if(force){
        QApplication::processEvents();
        qDebug()<<"SCROL:"<<scr<<marker->y()<<marker_first->y()<<mfy();


    }
}
void widgetGen::setContainer(){
    glcontainer=new widget_wrapper();
    // must be called before the widget or its parent window gets shown
    glcontainer->setGeometry(rct);
    glcontainer->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    glcontainer->setFixedSize( (rct.size()+QSize(0,dpi)));
    connect(glcontainer,SIGNAL(textChanged(const QString&)),this,SLOT(filter(const QString&)));
    ppt=new QPixmap(lw,lh);
}
void widgetGen::setupTopMenuSwitcher(QRect r){
    if(top_menu_switcher==nullptr)
        top_menu_switcher=new topMenuSwitcher();
    top_menu_switcher->setGeometry(r);
    top_menu_switcher->setFixedSize(r.size());
}
void widgetGen::setFrames(QList<QWidget *> f1, QList<QWidget *> f2){
    top_menu_switcher->setFrames(f1,f2);
}
void widgetGen::showSelectors(){
    top_menu_switcher->showSelectors(manageSelectors());
}

bool widgetGen::setContext(const QString& c){
    /* Set proper context */
    qDebug()<<"OLD CONTEXT:"<<context<<"new:"<<c;
    bool nc=false;
    if(c!=context){
        nc=true;
        if(context!=""){
            toggleBricks(false);
        }
    }
    context=c;
    if(isProspect())mfcorr=30;
    else mfcorr=0;
    pressed=false;
    vcatapultscrollspeed = 0;
    swipe_orient = not_set;
    move = QVector2D( 0, 0 );
    br_deleted = -1;
    qDebug()<<"WGEN CONTEXT:"<<context<<nc;
    return nc;
}
void widgetGen::set_markers(QOpenGLWidget *mf, QOpenGLWidget *mm, QOpenGLWidget *ml,  QOpenGLWidget *me,  QOpenGLWidget *ms){
    marker_first=mf;
    marker_middle=mm;
    marker_last=ml;
    marker=me;
    marker_switch=ms;
    marker->setParent(glcontainer);
    marker_first->setParent(glcontainer);
}
int widgetGen::mfy(){
    if(marker_first)
        return marker_first->y()+mfcorr;
    else return mfcorr;
}
void widgetGen::lock(){
    locked=true;
    stop_bg_proc();
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
}
void widgetGen::brick_remove( int td ){
    // stop_bg_proc();
    br_deleted = td;
    bool animated = false;
    if( context != "videos" || context !="prospect"){
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
        if(lastBrick()==-1)br_deleted = -1;
        if( !animated  )finish_deletion();
    }

    if(filtered!="")filter(filtered);
}
int widgetGen::getItemsCount(){
    return itemList().count();
}
bool widgetGen::isProspect(){
    return context=="prospect";
}
void widgetGen::filter(){
    if(glcontainer->is_search()){
        filter(glcontainer->edit->text());
        return;
    } else {
        toggleBricks(true);
    }
}
void widgetGen::filter(const QString& f){
    qDebug()<<"FILTER::"<<context;
    filtered=f;
    int scnt=0;
    lastVis=-1;
    QList<int> protect_list({});
    for(int i=0;i<getItemsCount();i++){
        if(debg)qDebug()<<"--------------------------------"<<item(i)->txt_text<<filtered<<par->prospect_initial_view<<par->prospect_filter_chars;
        if(((item(i)->tim_text.contains(filtered,Qt::CaseInsensitive) || item(i)->txt_text.contains(filtered,Qt::CaseInsensitive))
            && filtered.length()>=par->prospect_filter_chars) || !isProspect() || (par->prospect_initial_view && f=="")){
            if(isProspect() && !item(i)->isProducer()){
                /*  Show producer according to title as list-section-head, if locked */
                if(!protect_list.contains(item(i)->oid)){
                    /* Avoiding multiple appear */
                    protect_list.append(item(i)->oid);
                    if(debg)qDebug()<<"ADDING PRODUCER:"<<producerOf(i)->txt_text<<producerOf(i)->index;
                    producerOf(i)->setGeometry(brick_geo(scnt));
                    producerOf(i)->unlock();
                    scnt++;
                }
            }
            /* Show proper widget */
            if(!protect_list.contains(item(i)->db_index)){
                item(i)->setGeometry(brick_geo(scnt));
                item(i)->unlock();
                qDebug()<<"BRICK:"<<mfy()<<brick_geo(scnt)<<scnt;
                scnt++;
                if(item(i)->isProducer())
                    protect_list.append(item(i)->db_index);
                lastVis=i;
            }
        }
        else {
            /* Roll to invisible state not matching widget - if not at list of one of items's producers */
            if(!protect_list.contains(item(i)->db_index))
                item(i)->lock();
        }
    }
    emit filter_done();
    glcontainer->update();
}
void widgetGen::give_event(){
    currItem()->catch_event();
}

QString widgetGen::actContext(){
    return context;
}
bool widgetGen::mclick(){
    if( lastBrick()<0 )return false;
    QString _oid=QString::number(currItem()->oid);
    QString _nf=currItem()->netfile;
    QString _tid = QString::number(currItem()->title_id);
    QString w_desc=currItem()->txt_text;
    QString _time;
    QString _context=currType();
    if( currType()  == "fav_items" ){
        emit go("web",{events[over_brick]});
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
    return (currType()=="videos" || currType()=="fav_scenes");
}
void widgetGen::make_brick( int index, int di,int tid,int itcnt, const QString& up, const QString& dn, const QString& imgurl,
                            const QString& imgfile, const QString& titlemedia, const QString& mod_name,int od){
    bool ft = false;
    if( context  == "videos" ){
        if( vb.count()<=index ){
            vb.append(new gLabel( glcontainer, mod_name ,context ));
            ft = true;
        }
    } else
        if( context  == "fav_items" ){
            if( fib.count()<=index ){
                fib.append(new gLabel( glcontainer,mod_name ,context));
                ft = true;
            }
        } else
            if( context  == "fav_scenes" ){
                if( fsb.count()<=index ){
                    fsb.append(new gLabel( glcontainer, mod_name ,context));
                    ft = true;
                }
            } else
                if( context  == "fav_videos" ){
                    if( fvb.count()<=index ){
                        fvb.append(new gLabel( glcontainer, mod_name ,context));
                        ft = true;
                    }
                }
                else if(context == "prospect"){
                    if( msb.count()<=index ){
                        msb.append(new gLabel( glcontainer, mod_name ,context));
                        ft = true;
                    }
                }
    QPointer<gLabel> gl(item(index));
    gl->lock();
    gl->set_prospect_params(*par);
    if(ft){
        connectSignals(gl);
    } else gl->setContext(context);
    if(gl->db_index!=di || ft){
        gl->setDPI(dpi);
        gl->set_params( index, di, itcnt,tid,imgfile, imgurl, up, dn, titlemedia,od);
    }
    gl->setObjectName(mod_name);
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
        qDebug()<<"Hiding:"<<uilist.at(i)->objectName();
        uilist.at(i)->setVisible(false);
    }
    cw->parentWidget()->update();
}
void widgetGen::connectSignals(gLabel* gl){
    connect( gl, SIGNAL( download( const QString&, const QString&,const int,bool) ), this,
             SIGNAL( download( const QString&, const QString&,const int,bool ) ) );
    connect( gl, SIGNAL( hover( int, int ) ), this,  SLOT( handleEnter( int, int ) ));
    connect( gl, SIGNAL( leaved() ), this, SLOT( handleLeave() ) );
    connect( gl, SIGNAL( add_fav_videos( int ) ), this, SIGNAL( add_fav_videos( int ) ) );
    connect( gl, SIGNAL( del_fav_videos( int, const QString& ) ), this, SIGNAL( del_fav_videos( int, const QString& ) ));
    connect( gl, SIGNAL( click_delete( int ) ), this, SLOT( brick_remove( int )) );
    connect( gl, SIGNAL( hided() ), this,  SIGNAL( hided()) );
    connect( gl, SIGNAL( get_poster(int,bool,const QString&,const QString&,const QString&,const QString&,const QString&,int)),
             this, SLOT( poster_generator(int, bool, const QString&, const QString&,const QString&,const QString&,const QString&,int)),Qt::QueuedConnection);
}
void widgetGen::set_fav_videos_list(const QJsonArray *arr){
    if(debg)qDebug()<<"SET FAV VIDEOS:"<<arr->count()<<*arr;
    int _cnt = arr->count();
    fav_videos_list.clear();
    for( int i = 0;i<_cnt;i++ ){
        int in = arr->at( i ).toObject().value( "id" ).toString().toInt();
        fav_videos_list.append( in );
    }
}
void widgetGen::mark_fav(int i){
    bool m=true,n=true;
    if(isProspect()){
        m=par->prospect_osd;
        n=m*par->prospect_favs;
        if(fav_videos_list.contains( item( i )->db_index ) ){
            item( i )->mark_fav_videos(m);
        } else{
            item( i )->unmark_fav_videos(n);
        }
    }
}
void widgetGen::handleLeave(){
    over_brick=-1;
}
void widgetGen::handleEnter( int e, int f ){
    over_brick = e;
    over_brick_ind = f;
}
void widgetGen::toggleBricks(bool w){
    is_expanded=true;
    lastVis=-1;
    for(int i=0;i<getItemsCount();i++){
        QPointer<gLabel> gl(item(i));
        gl->setGeometry(brick_geo(i));
        gl->toggle(w);
        if(w)lastVis=i;
    }
    QApplication::processEvents();
}
void widgetGen::activate(bool sh){
    unlock();
    if(sh)glcontainer->enableSearch();
    else glcontainer->disableSearch();
    is_expanded=true;
    filter();

}
void widgetGen::setDirector(QWidget *w){
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
void widgetGen::helper(){
    qDebug()<<"Back";
}

QRect widgetGen::brick_geo(int i){
    return QRect(mrgh*brct.width(), mfy() + i * brct.height(),( 1-2*mrgh )*brct.width(), brct.height());
}
QRect widgetGen::get_srect(int i){
    QRect srect=brick_geo(i);
    srect.setHeight(1);
    srect.setY(srect.bottom()-srect.height()+1);
    return srect;
}
QList<QPointer<gLabel>> widgetGen::itemList(){
    if( context  == "videos")return vb;
    else
        if( context  == "fav_videos")return fvb;
        else
            if( context  == "fav_scenes"  )return fsb;
            else
                if( context  == "fav_items"  )return fib;
                else
                    if( isProspect() )return msb;
                    else
                        return QList<QPointer<gLabel>>();
}
void widgetGen::share(){
    QString text = currItem()->txt_text;
    QUrl url( events[over_brick] );
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
    brct.setRect(0,0,rt.width(),rt.width()/par->widget_height_factor);
    w=brct.width()*dpi;
    vtm=0.01*brct.height();
    htm=0.01*brct.width();
    lh=(brct.height()/1.03)*dpi;
    lw=(1-mrgh*2)*w;
    lm=w/1000;
    lmm=lm*2;
    txt_font.setFamily("Apple SD Gothic Neo");
    tim_font.setFamily("Apple Gothic");
    txt_font.setStyleStrategy(QFont::StyleStrategy::PreferQuality);
    tim_font.setStyleStrategy(QFont::StyleStrategy::PreferQuality);
    rct=rt;
    setContainer();
}
bool widgetGen::manageSelectors(){
    //   qDebug()<<marker->mapToGlobal(QPoint(0,0))<<marker_first->mapToGlobal(QPoint(0,0))<<marker_middle->mapToGlobal(QPoint(0,0))<<marker_last->mapToGlobal(QPoint(0,0))<<marker_switch<<marker_middle->mapToGlobal(QPoint(0,0));
    return marker_first->mapToGlobal(QPoint(0,0)).y()>=marker_switch->mapToGlobal( QPoint( 0, 0 ) ).y();
}
QPointer<gLabel> widgetGen::producerOf(int i){
    if(i==-1)
        i=over_brick;
    int oid=item(i)->oid;
    for(int i=0;i<getItemsCount();i++){
        if(item(i)->db_index==oid)
            return item(i);
    }
    return nullptr;
}
QPointer<gLabel> widgetGen::item( int i,QString t){
    QString cn=context;
    if(t!="")cn=t;
    qDebug()<<"ITEM:"<<cn;
    if(cn=="videos")
        if(vb.count()>i)
            return vb.at(i);

    if(cn=="fav_videos")
        if(fvb.count()>i)
            return fvb.at(i);

    if(cn=="fav_scenes")
        if( fsb.count() > i )
            return fsb.at(i);

    if(cn=="fav_items")
        if(fib.count() > i )
            return fib.at(i);

    if(cn=="prospect")
        if(msb.count() > i )
            return msb.at(i);
    return nullptr;
}
QString widgetGen::currType(){
    if(currItem()!=nullptr)
        return currItem()->type;
    else return "";
}
QPointer<gLabel> widgetGen::currItem(){
    if(over_brick!=-1){
        qDebug()<<"OVER BRICK::"<<over_brick;
        return item( over_brick );
    }
    else
    {
        return nullptr;
    }
}

void widgetGen::removeOffscreen(int j){
    int c=itemList().count();
    if(c>0){
        for(int i=0;i<c;i++){
            if(item(i)->db_index==j){
                item(i)->close();
                itemList().removeAt(i);
            }
        }
    }
}
void widgetGen::finish_deletion(){
    emit del_fav_videos(item(br_deleted)->db_index,context);
    qDebug()<<"ITML:"<<itemList();
    itemList().removeAt(br_deleted);
    animating=false;
    if(debg)qDebug()<<"afer delete positioning "<<lastBrick()+1;
    int j;
    for( j = 0;j<lastBrick()+1;j++ ){
        if(debg)qDebug()<<"afer delete positioning brick nr"<<j<<item(j)->index;
        item( j )->index=j;
        item( j )->setGeometry(brick_geo( j ));
    }
    for(int i=0;i<poster_data.count();i++){
        if(poster_data.at(i).index==br_deleted && poster_data.at(i).type==context){
            poster_data.removeAt(i);
            break;
        }
    }
    br_deleted=-1;
}
int widgetGen::lastBrick(){
    return itemList().count()-1;
}


const QString& widgetGen::getContext(){
    return context;
}


/* BRICK & POSTER FACTORY */
void widgetGen::poster_generator(int ind, bool stage, const QString& type, const QString& fn,const QString& up,const QString& dn,const QString& cn,int itcnt){
    poster_data.append({ind,stage,type,fn,up,dn,cn,itcnt});
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
        while(!poster_data.isEmpty()){
            if(poster_data.count()==0){
                if(debg)qDebug()<<"NO DATA POSTER";
                return ;
            }
            /* load poster image */
            PosterData pdata=poster_data.takeFirst();
            const QPixmap* _p=new QPixmap(pdata.file);
            if(_p->isNull()){
                /* remove from external mem if faulty */
                qDebug()<<"P is null:"<<pdata.type<<pdata.context<<pdata.file;
                QFile::remove(pdata.file);
                return ;
            }

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
                txt_font.setPixelSize(16.0*dpi);
                txt_font.setBold(false);
                txt_rect.setRect((_lw+3*htm),vtm,_lw2,lh*.72);
                ppt->fill(QColor(0,0,0,150));
                fct=.8;
            }
            tim_rect.setRect(txt_rect.left(),txt_rect.bottom(),txt_rect.width()*fct,lh-txt_rect.height());
            tim_font.setPixelSize(15*dpi);


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
                painter.drawPixmap(QRect((_lw-ss.width())/2,(lh-ss.height())/2,ss.width(),ss.height()),*_p);
            } else
                painter.drawPixmap(QRect(0,0,_lw,lh),*_p);
            painter.end();
            ppi=new QPixmap(lw,lh);
            ppi->fill(QColor(0,0,0,0));
            painter.begin(ppi);
            painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
            painter.setBrush(*ppt);
            painter.drawRoundedRect(1,1,lw-2,lh-2,10*dpi,10*dpi);
            if(pdata.type=="prospect"){
                painter.setPen(QColor("#ff0"));
            } else
                painter.setPen(QColor("#eef"));
            painter.setFont(txt_font);
            painter.drawText(txt_rect,Qt::TextFlag::TextWordWrap | Qt::AlignCenter,pdata.up);
            if(pdata.type=="prospect"){
                painter.setPen(QColor("#77f"));
            } else
                painter.setPen(QColor("#999"));
            painter.setFont(tim_font);
            painter.drawText(tim_rect,Qt::TextFlag::TextWordWrap | Qt::AlignVCenter | tim_align ,pdata.dn);
            if(pdata.itcnt==0 && pdata.type=="videos"){
                QBrush b(painter.brush());
                painter.setBrush(QBrush(QColor(0,0,0,150)));
                painter.drawRoundedRect(0,0,lw-2,lh-2,10*dpi,10*dpi);
                painter.setBrush(b);
                painter.setBrush(QColor(235,186,26,150));
                painter.save();
                painter.translate(_lw/3.9,lh*.7);
                painter.rotate(-20);
                QRect csrect(0,0,_lw/1.8,lh*0.2);
                painter.drawRoundedRect(csrect,15,15);
                painter.setPen(QColor(0,0,0));
                painter.drawText(csrect,Qt::AlignCenter,"COMING SOON");
                painter.restore();
            }
            painter.end();
            ppi->setDevicePixelRatio(dpi);
            poster_data_ready.append({pdata.index,pdata.stage,pdata.type,ppi,pdata.context});
            delete _p;
            emit send_item(poster_data_ready.count()-1);
        }
    });
}
void widgetGen::put_poster(int i){
    if(back_mutex)return;
    if(item(poster_data_ready.at(i).index,poster_data_ready.at(i).context)){
        item(poster_data_ready.at(i).index,poster_data_ready.at(i).context)->settle_poster(poster_data_ready.at(i).poster,poster_data_ready.at(i).stage);
    }
}


/* MOUSE MANAGEMENT */
bool widgetGen::mouseOver(QWidget* w){
    return QRect(w->mapToGlobal(QPoint(0,0)),w->size()).contains(QCursor::pos());
}
void widgetGen::mousePress(QMouseEvent* e){
    timer.start();
    if(glcontainer->is_search())
        if(!input->keyboardRectangle().contains(e->globalPos()) ){
            if(!glcontainer->edit->geometry().contains(e->globalPos()))
                input->hide();
        }
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
}
bool widgetGen::mouseRelease(QMouseEvent *e){
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
                qDebug()<<"MCLICK:"<<e->globalPos().x()<<currItem()->width()*4/5;
                if( e->globalPos().x() <=currItem()->width()*4/5 ){
                    st=mclick();
                } else if( currType() == "videos" ){
                    if(canToggleFavs()){
                        currItem()->toggle_fav();
                        st=false;
                    }
                } else {
                    if(canToggleShares()){
                        share();
                        st=false;
                    }
                }
            }
        }
    } else {
        if(mouseOver(top_menu_switcher))
            st=top_menu_switcher->get_hover(e->globalPos());
        else
            st=glcontainer->get_hover(e->globalPos());
    }
    swipe_orient=not_set;
    no_click=false;

    return st;
}
void widgetGen::mouseMove(QMouseEvent *e){
    if(over_brick==-1)
        return;
    pos = e->globalPos();
    move = QVector2D( pos-prev_pos );
    diff = QVector2D( pos-touch_pos );
    prev_pos = pos;
    get_swipe_predicter();
    if( swipe_orient == vertical){
        if( lastBrick()>-1 ){
            scrr += double( move.y() );
            append_vscroll( double( move.y() ) );
        } else {
            swipe_orient=not_set;
        }
    }
}
bool widgetGen::isMousePressed(){
    return pressed;
}


/* SWIPE PREDICTER */
void widgetGen::get_swipe_predicter(){
    if( lastBrick()<0 )return;
    if( swipe_orient != not_set )return;
    no_click = false;
    if( abs( move.x() ) <= abs( move.y() ) && abs( diff.y() )>2*dpi ){
        no_click = true;
        if( item( lastBrick() )->geometry().bottom()-marker->geometry().top()>rct.height() ){
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
    if( !pressed ){
        rubberbandscrollspeed = 0;
        int tail = marker_last->pos().y();
        int head = marker->pos().y();
        if(lastBrick()>-1){
            if(lastVis==-1)tail = item(lastBrick())->geometry().bottom();
            else tail=item(lastVis)->geometry().bottom();
        }
        if( head>0 ){
            rubberbandscrollspeed = -head / 2;
            vcatapultscrollspeed /= 2;
            if( abs( rubberbandscrollspeed )<1 ){
                rubberbandscrollspeed = -head;
            }
        }
        if( tail<rct.height()-inpt_h && head<0 ){
            qDebug()<<head<<tail;
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
        if(vcatapultscrollspeed!=0)
            qDebug()<<"vcatapult:"<<vcatapultscrollspeed<<marker_first->y();
    }
    if(br_deleted==-1){
        if(!pressed && scr==0 && scrr==0 ){
            timer.stop();
        }
    }
    scr=scrr;
    scrr=0;
    return scr;
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
    return par->prospect_favs || context!="prospect";
}
bool widgetGen::canToggleShares(){
    return par->prospect_shares || context!="prospect";
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
