#include "favitems.h"
#include "ui_favitems.h"
favItems::favItems( QWidget *parent ) :
    QMainWindow( parent ),
    ui( new Ui::favItems ),
    debg(false)
{
    ui->setupUi( this );
    setAttribute( Qt::WA_AlwaysStackOnTop, false );
    setAttribute( Qt::WA_NoSystemBackground, false );
    setAttribute( Qt::WA_TranslucentBackground, true );
    splash_img.load(":/gui/BANNERS/wandizz_trnsp.png");
    setAttribute( Qt::WA_DeleteOnClose, true );
    glcontainer = new widget_wrapper( this );
    glmapper=new widget_mapper(this);
    mrgh = 0.015;
    max_q = 2;
    antime=90;
    hidden_cnt=0;
    cleanAnimPrepared=false;
    need_correct=false;
    animating=false;
    noanim=true;
    old_scrp=0;
    foo={};
    prev_on="";
    generator_active=false;
    initializing = true;
    ptimer.setSingleShot( true );
    pdimmer.setSingleShot( true );
    connect( &pdimmer, SIGNAL( timeout() ), this, SLOT( hide_top_wrapper() ) );
    group = new QParallelAnimationGroup( this );
    connect(group,SIGNAL(finished()),this,SLOT(finish_deletion()));
    connect(this,SIGNAL(startProcessing()),this,SLOT(start_bg_proc()));
    ui->lab_info_items->hide();
    ui->lab_info_scenes->hide();
    ui->lab_info_videos->hide();
    ui->lab_info_res->hide();
    ui->favs_cnt->hide();
    ui->lab_nick->hide();
    ui->lab_favs_icon->hide();
    offscreenindicator=0;
    setObjectName( "" );
    cleanGroup=new QSequentialAnimationGroup(this);
    connect(cleanGroup,SIGNAL(finished()),this,SLOT(removeClean()));
    ag=new QSequentialAnimationGroup(this);
    connect(ag,SIGNAL(finished()),this,SLOT(noticeShow()));
    on = "";
}
void favItems::enableSearch(){
    glcontainer->showSearch(true);
}
void favItems::init(){
    main_h=width()/2.8;
    QRect r = QApplication::primaryScreen()->virtualGeometry();
#if defined (Q_OS_IOS)
    dpi = devicePixelRatio()*2;
#else
    dpi=devicePixelRatio();
#endif
    glmapper->setGeometry(ui->switch_b_frame_top->geometry());
    glmapper->setFixedSize(ui->switch_b_frame_top->size());
    glcontainer->setGeometry( r );
    glcontainer->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    glcontainer->setFixedSize( r.width(), r.height()+dpi );
    a[0].addPixmap( QPixmap( ":/gui/icons/green_dot_no_dot.png" ), QIcon::Disabled );
    a[1].addPixmap( QPixmap( ":/gui/icons/green_dot.png" ), QIcon::Disabled );
    old_ss = ui->b_options->styleSheet();
    ui->b_items->setFlat( true );
    ui->b_scenes->setFlat( true );
    set_markers();
    timer.setInterval( probe_freq );
    setAttribute( Qt::WA_TransparentForMouseEvents, false );
    setMouseTracking( true );
    setTabletTracking( false );
    connect( &timer, SIGNAL( timeout() ), this, SLOT( timerev() ) );
    glcontainer->set_container(ui->lab_favs_icon->geometry(),
                               ui->favs_cnt->geometry(),
                               ui->lab_info_videos->geometry(),
                               ui->lab_nick->geometry() );

    glcontainer->set_styles( ui->favs_cnt,
                            ui->lab_info_res,
                            ui->lab_nick );
    connect(glcontainer,SIGNAL(filter(QString)),this,SLOT(filter(QString)));
    set_favs_cnt( 0 );
}

void favItems::filter(QString f){
    filtered=f;
    int scnt=0;
    lastVis=-1;
    for(int i=0;i<cnt;i++){
        if(!(item(i)->txt_text.contains(f,Qt::CaseInsensitive) || item(i)->tim_text.contains(f,Qt::CaseInsensitive))){
            if(debg)qDebug()<<"locking"<<i;
            item(i)->lock();
        } else {
            item(i)->unlock();
            item(i)->move(mrgh*width(), marker_first.y() + scnt * main_h);
            scnt++;
            lastVis=i;
        }
    }
}
void favItems::poster_generator(int ind, bool stage, QString type, QString fn,QString up,QString dn){
    poster_data.append({ind,stage,type,fn,up,dn});
    //   if(debg)qDebug()<<"POSTERS WAITING:"<<poster_data.count()<<"ready:"<<poster_data_ready.count();
    if(!generator_active && !future.isRunning()){
        //  if(debg)qDebug()<<"GEN_POSTER:"<<future.isRunning();
        make_poster();
    }
}
void favItems::make_poster(){
    future = QtConcurrent::run([=]() {
        generator_active=true;
        while(!poster_data.isEmpty()){
            if(poster_data.count()==0){
                if(debg)qDebug()<<"NO DATA POSTER";
                return ;
            }
            PosterData pdata=poster_data.takeFirst();
            QString type=pdata.type;
            if(on=="videos"){
                tim_align=Qt::AlignRight;
            } else {
                tim_align=Qt::AlignCenter;
            }
            QImage _p;
            if(pdata.stage==1){
                _p.load(pdata.file);
                _p.convertTo(QImage::Format_ARGB32);
                if(_p.isNull()){
                    QFile::remove(pdata.file);
                    generator_active=false;
                    return ;
                } //else
                  //  splash_img=QImage();
            } else _p=splash_img;
            _p.setDevicePixelRatio(dpi);
            int pdpi=1;
            QString txt_text,tim_text;
            txt_text=pdata.up;
            tim_text=pdata.dn;
            int lh,lw,_lw,lm;
            qreal  _prop;
            QRect txt_rect,tim_rect;
            qreal w=width();
            qreal lmm;
            lh=main_h/1.03;
            lw=(1-mrgh*2)*w;
            lm=w/1000;
            lmm=lm*2;
            on=="fav_items" ? _prop=1 : _prop=1.75;
            _lw=lh*_prop;
            QFont txt_font,tim_font;
            txt_font.setBold(false);
            txt_font.setFamily("Apple SD Gothic Neo");
            tim_font.setFamily("Apple Gothic");
            txt_font.setStyleStrategy(QFont::StyleStrategy::PreferQuality);
            tim_font.setStyleStrategy(QFont::StyleStrategy::PreferQuality);
            tim_font.setPixelSize(15.0);
            txt_font.setPixelSize(16.0);
            int ih=bsize.height();
            int iw=rect().width();
            int vtm=0.05*ih;
            int htm=0.01*iw;
            int _lw2=lw-_lw-(6*htm);
            txt_rect.setRect((_lw+3*htm)*pdpi,vtm*pdpi,_lw2*pdpi,ih*.72*pdpi);
            int fl=main_h*pdpi-txt_rect.height()-vtm*pdpi;
            tim_rect.setRect(txt_rect.left(), txt_rect.bottom(), txt_rect.width()-fl-htm,fl);
            QImage* ppt=new QImage(lw*dpi,lh*dpi,_p.format());
            ppt->fill(QColor(0,0,0,150));
            QPainter painter;
            //   if(debg)qDebug()<<"PPT:"<<ppt.size();
            ppi=new QImage(lw*dpi,lh*dpi,ppt->format());
            ppi->setDevicePixelRatio(dpi);
            painter.begin(ppt);
            painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
            painter.fillRect(ppt->rect(),QBrush(QColor(0,0,0,0)));
            QRect picrect=QRect(0,0,_lw*dpi,lh*dpi);
            if(on=="fav_items"){
                painter.setBrush(QColor(255,255,255,255));
                painter.drawRect(QRect(0,0,_lw*dpi,lh*dpi));
                painter.setBrush(QBrush());
                _p=(_p.scaled(_lw*dpi,lh*dpi,Qt::KeepAspectRatio,Qt::SmoothTransformation));
                painter.drawImage(QRect((_lw/2)*dpi-_p.width()/2,(lh/2)*dpi-_p.height()/2,_p.width(),_p.height()),_p);
            } else
                painter.drawImage(QRect(0,0,_lw*dpi,lh*dpi),_p.scaled(picrect.size(),Qt::IgnoreAspectRatio));
            ppi->fill(QColor(0,0,0,0));
            painter.end();
            ppt->setDevicePixelRatio(dpi);
            painter.begin(ppi);
            painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
            painter.fillRect(ppi->rect(),QColor(0,0,0,0));
            painter.setBrush(*ppt);
            painter.drawRoundedRect(0,0,lw,lh,5*devicePixelRatio(),5*devicePixelRatio());
            painter.setPen(QColor("#eef"));
            painter.setFont(txt_font);
            painter.drawText(txt_rect,Qt::TextFlag::TextWordWrap | Qt::AlignCenter,txt_text);
            painter.setPen(QColor("#999"));
            painter.drawText(tim_rect,Qt::TextFlag::TextWordWrap | Qt::AlignVCenter | tim_align ,tim_text);
            painter.end();
            poster_data_ready.append({pdata.index,pdata.stage,pdata.type,ppi->copy()});
            delete ppi;
            delete ppt;
            emit startProcessing();
        }
        generator_active=false;
    });
}
void favItems::settled(){
    if(debg)qDebug()<<"settled";
}
QRect favItems::brick_geo(int i){
    return   QRect(mrgh*width(), marker_first.y() + i * main_h,( 1-2*mrgh )*width(), main_h);
}
void favItems::reinit( QString o ){
    on=o;
    cnt=0;
    noanim=true;
    animating=false;
    cleaning=false;
    bricks_shown=0;
    lastVis=-1;
    filtered="";
    setObjectName(o);
    initializing=true;
    locked=true;
    set_wrappers_wraps();
    over_brick=-1;
    stop_bg_proc();
    over_brick=-1;
    prev_on=on;
    scrr = 0;
    swipe_orient = not_set;
    move = QVector2D( 0, 0 );
    vcatapultscrollspeed = 0;
    br_deleted = -1;
    probe_freq = 17;
    offline = false;
    pressed = false;
}
int favItems::get_vscroll(){
    int i, vv = 0, zeros = 0;
    if( q_vscroll.count()<max_q )return 0;
    for( i = 0;i<q_vscroll.count();i++ ){
        if( q_vscroll.at( i )  == 0 )zeros++;
        vv += q_vscroll.at( i );
    }
    q_vscroll.clear();
    if(i==0)i=1;
    return ( ( vv / ( i ) ) / dpi ) * 4 ;
}
void favItems::append_vscroll( int s ){
    q_vscroll.append( s );
    if( q_vscroll.count()>max_q )q_vscroll.pop_front();
}
bool favItems::cleanUp(QString type){
    bool anfilled=false;
    locked=false;
    cleaning=true;
    cleanAnimPrepared=false;
    if(lastBrick(type)!=-1){
        int c=lastBrick(type);
        cleanGroup->clear();
        cleanGroup->setDirection(QAbstractAnimation::Forward);
        for( int i = 0;i <= c;i++ )
        {
            if(rect().intersects(brick_geo(i))){
                QPropertyAnimation* an=new QPropertyAnimation(item(i,type),"geometry",this);
                anfilled=true;
                an->setStartValue(item(i)->geometry());
                QRect srect=item(i)->geometry();
                srect.setY(srect.bottom()-2);
                srect.setHeight(1);
                an->setEndValue(srect);
                an->setDuration(antime);
                cleanGroup->addAnimation(an);
                connect(an,SIGNAL(finished()),this,SLOT(lockBrick()));
                cleanAnimPrepared=true;
            } else {
                item(i,type)->lock();
            }
        }
        if(!anfilled){
            cleaning=false;
            if(!animating)locked=false;
        }
    }
    return  false;
}
void favItems::removeClean(){
    cleaning=false;
    if(!animating)locked=false;
}
void favItems::showBricks(){
    animating=true;
    animfilled=false;
    if(cnt>0){
        ag->clear();
        for(int i=0;i<cnt;i++){
            QPointer<gLabel> gl(itemList().at(i));
            if( gl->geometry().bottom()-marker.y()>height() ){
                scfilled = true;
                if(noanim)
                    QApplication::processEvents();
            }
            if(debg)qDebug()<<"NOANI:"<<noanim;
            if(rect().intersects(brick_geo(i)) && !noanim){
                if(debg)qDebug()<<"ANIMATION:"<<i;
                QPropertyAnimation* an=new QPropertyAnimation(gl,"geometry",this);
                QRect sgeo=brick_geo(i);
                sgeo.setHeight(1);
                an->setStartValue(sgeo);
                an->setEndValue(brick_geo(i));
                an->setDuration(antime);
                ag->addAnimation(an);
                gl->setGeometry(sgeo);
                gl->setMinimumSize(sgeo.size());
                gl->setMaximumSize(brick_geo(i).size());
                connect(an,SIGNAL(stateChanged(QAbstractAnimation::State,QAbstractAnimation::State)),this,SLOT(unlockBrick(QAbstractAnimation::State,QAbstractAnimation::State)));
                ag->setDirection(QAbstractAnimation::Forward);
                an->setEasingCurve(QEasingCurve::InCirc);
                gl->raise();
                animfilled=true;
            } else {
                gl->setGeometry(brick_geo(i));
                gl->unlock();
            }
        }
    }
    if(cleanAnimPrepared)
        cleanGroup->start(QAbstractAnimation::KeepWhenStopped);
    if(animfilled)
        ag->start(QAbstractAnimation::KeepWhenStopped);
    else {
        if(cleaning)locked=false;
        animating=false;
    }
    //  });
}
void favItems::unlockBrick(QAbstractAnimation::State newst,QAbstractAnimation::State oldst){
    QPropertyAnimation* a=static_cast<QPropertyAnimation*>(sender());
    gLabel *l=static_cast<gLabel*>(a->targetObject());
    if(newst==QAbstractAnimation::Running){
        l->unlock();
    } else {
        l->setGeometry(brick_geo(l->index));
        l->update();
    }
    a->deleteLater();
}
void favItems::lockBrick(){
    QPropertyAnimation* a=static_cast<QPropertyAnimation*>(sender());
    gLabel *l=static_cast<gLabel*>(a->targetObject());
    l->lock();
    l->setGeometry(brick_geo(l->index));
    a->deleteLater();
}
bool favItems::make_brick( int index, int di, QString up, QString dn, QString imgurl, QString imgfile, QString titlemedia, QString mod_name){
    if(debg)qDebug()<<"IMGFILE:"<<imgfile;
    bool ft = false;
    if( on  == "videos" ){
        if( vb.count()<=index ){
            vb.append(new gLabel( glcontainer, on ));
            ft = true;
        }
    } else
        if( on  == "fav_items" ){
        if( fib.count()<=index ){
            fib.append(new gLabel( glcontainer, on ));
            ft = true;
        }
    } else
        if( on  == "fav_scenes" ){
        if( fsb.count()<=index ){
            fsb.append(new gLabel( glcontainer, on ));
            ft = true;
        }
    } else
        if( on  == "fav_videos" ){
        if( fvb.count()<=index ){
            fvb.append(new gLabel( glcontainer, on ));
            ft = true;
        }
    }
    QPointer<gLabel> gl(itemList().at(index));
    gl->setGeometry(brick_geo(index));
    gl->setFixedSize(brick_geo().size());
    gl->update();
    gl->lock();
    if(ft){
        connect( gl, SIGNAL( download( QString, QString ) ), this, SIGNAL( download( QString, QString ) ) );
        connect( gl, SIGNAL( hover( int, int ) ), this, SLOT( handle_enter( int, int ) ) );
        connect( gl, SIGNAL( add_fav_videos( int ) ), this, SIGNAL( add_fav_videos( int ) ) );
        connect( gl, SIGNAL( del_fav_videos( int, QString ) ), this, SIGNAL( delete_item( int, QString ) ) );
        connect( gl, SIGNAL( click_delete( int, int ) ), this, SLOT( brick_remove( int, int ) ) );
        connect( gl, SIGNAL( leaved() ), this, SLOT( handleLeave() ) );
        connect( gl, SIGNAL( hided() ), this, SLOT( countHidden() ), Qt::QueuedConnection );
        connect( gl, SIGNAL( settled() ),this, SLOT( settled() ),Qt::QueuedConnection);
        connect( gl, SIGNAL( get_poster(int,bool,QString,QString,QString,QString)),
                this, SLOT( poster_generator(int, bool, QString, QString,QString,QString)));
    } else {
        if(noanim)
            noanim=false;
    }
    if(gl->db_index!=di || ft){
        gl->set_params( index, di, mrgh,main_h,imgfile, imgurl, up, dn, &marker_first,titlemedia);
        if(debg)qDebug()<<"IMGFILE@@@@@@@:"<<imgurl;
    }
    if( gl->geometry().bottom()-marker.y()>height() ){
        scfilled = true;
    }
    gl->check_timer();
    cnt++;
    return true;
}
void favItems::countHidden(){
}
void favItems::noticeShow(){
    set_favs_cnt( cnt );
    if(!cleaning)locked=false;
    animating=false;
    update();
}
void favItems::showEvent( QShowEvent*e ){
    e->accept();
    if(!initializing)start_bg_proc();
}
void favItems::handleLeave(){
    over_brick=-1;
}
void favItems::handle_enter( int e, int f ){
    if( initializing ){
        over_brick=-1;
        return;
    }
    over_brick = e;
    over_brick_ind = f;
}
void favItems::manageSelectors(bool force){
    if(!glcontainer->ready_to_paint)glmapper->setFrames(ui->switch_b_frame_top,ui->switch_b_frame);
    tsd = marker_middle.mapToGlobal(QPoint(0,0)).y()>glmapper->topFrame->mapToGlobal( QPoint( 0, 0 ) ).y();
    int dop = !tsd*( !hide_top || glmapper->topFrame->underMouse() );
    if(force || on!="videos"){
        ui->b_back_2->setEnabled(dop);
        ui->b_options_2->setEnabled(dop);
        ui->b_items_2->setEnabled(dop);
        ui->b_scenes_2->setEnabled(dop);
        ui->b_videos_2->setEnabled(dop);
    }
    glmapper->showSelector( dop );
    if( on != "videos" ){
        glcontainer->showSelectors(tsd);
    }
}
void favItems::timerev(){
    if( initializing || !isVisible() ){
        stop_bg_proc();
        return;
    }
    manageSelectors();
    if( !pressed ){
        rubberbandscrollspeed = 0;
        if( marker.pos().y()>0 ){
            rubberbandscrollspeed = -marker.pos().y() / 2;
            vcatapultscrollspeed /= 2;
            if( abs( rubberbandscrollspeed )<1 ){
                rubberbandscrollspeed = -marker.pos().y();
            }
        }
        int marker_tail = marker_first.pos().y();
        if(lastBrick()>-1){
            if(lastVis==-1)marker_tail = itemList().last()->geometry().bottom();
            else marker_tail=item(lastVis)->geometry().bottom();
        }
        if( marker_tail<height() && marker.pos().y()<0 ){
            vcatapultscrollspeed /= 2;
            rubberbandscrollspeed = ( height()-marker_tail )/2;
            if( abs( rubberbandscrollspeed )<1 ){
                rubberbandscrollspeed = height()-marker_tail;
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
    if(!animating && !cleaning)glcontainer->scroll( 0, scrr );
    if( !pressed )old_scr = abs( scrr );
    old_scrp=scrr;
    scrr = 0;
    if(!poster_data_ready.isEmpty() && br_deleted==-1){
        PostersReady poster=poster_data_ready.takeFirst();
        item(poster.index,poster.type)->settle_poster(poster.poster,poster.stage);
    } else if(br_deleted==-1){
        if(!pressed && old_scr==0){
            stop_bg_proc();
            glcontainer->updateFrameSet();
        }
    }
}
int favItems::lastBrick(QString type){
    if(type=="")type=on;
    return itemList(type).count()-1;
}
void favItems::mouseMoveEvent( QMouseEvent*e ){
    if( over_brick!=-1 ){
        pos = e->pos();
        move = QVector2D( pos-prev_pos );
        diff = QVector2D( pos-touch_pos );
        prev_pos = pos;
        get_swipe_predicter();
        if( swipe_orient == vertical){
            if( lastBrick()>-1 ){
                if( scfilled ){
                    scrr += double( move.y() );
                    append_vscroll( double( move.y() ) );
                }
            } else {
                swipe_orient=not_set;
            }
        }
    }
}
void favItems::mousePressEvent( QMouseEvent *e ){
    if( lastBrick()>=0 || over_brick>=0){
        start_bg_proc(17);
        pressed = true;
        diff.setX( 0 );
        diff.setY( 0 );
        ptimer.start( 300 );
        pdimmer.start( 1000 );
        pos = e->pos();
        prev_pos = pos;
        touch_pos = pos;
        vcatapultscrollspeed = 0;
    }
}
void favItems::mouseReleaseEvent( QMouseEvent *e ){
    pressed=false;
    if( lastBrick()>=0 || over_brick>=0){
        vcatapultscrollspeed = get_vscroll();
        if( vcatapultscrollspeed != 0 || swipe_orient  == horizontal || old_scr>1 ){
            no_click = true;
        }
        if( !initializing && !no_click && ptimer.isActive() ){
            if( over_brick>-1 ){
                if( ptimer.isActive() && currItem()->offs  == 0 ){
                    if( e->pos().x() <= width()-currItem()->height() ){
                        click();
                    } else if( on  == "videos" ){
                        currItem()->toggle_fav();
                    } else {
                        share();
                    }
                }
            }
        }
        hide_top = false;
        q_vscroll.clear();   
        over_brick = -1;     
        swipe_orient = not_set;
        ptimer.stop();
        old_scr = 0;
        no_click = false;
    }

    if(over_brick==-1){
        if(!glmapper->get_hover(e->globalPos()))
            glcontainer->get_hover(e->globalPos());

    }
}
void favItems::click(){
    if( lastBrick()<0 )return;
    if( mitems_cnt[over_brick]  == 0 && on  == "videos" ){
        setDisabled( false );
        return;
    }
    if( on  == "fav_items" ){
        emit go("web",{events[over_brick]});
    } else {
        if( on  == "fav_scenes" ){
            emit go("carousel",{currItem()->txt_text, currItem()->tim_text, currItem()->netfile, currItem()->oid});
        } else {
            emit go("carousel",{titles[over_brick], "00:00:00", currItem()->netfile,currItem()->oid});
        }
    }
}
void favItems::give_event(){

    currItem()->catch_event();
}
void favItems::get_swipe_predicter(){

    if( lastBrick()<0 )return;
    if( swipe_orient != not_set )return;
    no_click = false;
    if( abs( move.x() ) <= abs( move.y() ) && abs( diff.y() )>3*dpi ){
        no_click = true;
        if( item( lastBrick() )->geometry().bottom()-marker.geometry().top()>height() ){
            swipe_orient = vertical;
            return;
        }
    } else
        if( abs( move.x() )>abs( move.y() ) && !offline && abs( diff.x() )>3*dpi ){
        swipe_orient = horizontal;
        if( on  == "videos" )return;
        give_event();
        no_click = true;
    }
}
void favItems::brick_remove( int td, int tdi ){
    stop_bg_proc();
    br_deleted = td;
    for(int i=0;i<poster_data.count();i++){
        if(poster_data.at(i).index==br_deleted && poster_data.at(i).type==on){
            poster_data.removeAt(i);
            break;
        }
    }
    pressed=false;
    bool animated = false;
    itemList().takeAt(td)->close();
    if( on != "videos" ){
        set_favs_cnt( lastBrick()+1 );
        swipe_orient=not_set;
        group->clear();
        int acnt = 0;
        if( br_deleted != lastBrick()+1 && lastBrick()>=0 ){
            if(debg)qDebug()<<"LOOP DELETE";
            for( int j = br_deleted;j < lastBrick()+1;j++ ){
                if(debg)qDebug()<<"ADING ANIM DEL:"<<item(j)->geometry()<<brick_geo((j));
                del_anim.append( new QPropertyAnimation( item( j ), "geometry", this ) );
                del_anim.last()->setDuration( antime );
                del_anim.last()->setStartValue( item( j )->geometry() );
                del_anim.last()->setEndValue( brick_geo(j) );
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
    }
    emit delete_item( tdi, on );
    if(filtered!="")filter(filtered);
}
void favItems::finish_deletion(){
    if(debg)qDebug()<<"afer delete positioning "<<lastBrick()+1;
    int j;
    for( j = 0;j<lastBrick()+1;j++ ){
        if(debg)qDebug()<<"afer delete positioning brick nr"<<j<<itemList().at(j)->index;
        item( j )->index=j;
        item( j )->setGeometry(brick_geo( j ));
        item( j )->update();
    }
    start_bg_proc();
    //  set_favs_cnt(itemList().count());
    //  toggle_switchers( true );
}
bool favItems::chk_objectname(QString w){
    return true;
    if(on!=objectName()){
        if(objectName()!="media")
        {
            if(debg)qDebug()<<"WARNING:"<<objectName()<<on<<"at"<<w;
            return false;
        }
        else
        {
            if(debg)qDebug()<<"Object fresh";
        }
    }
    return true;
}
QList<gLabel *> &favItems::itemList(QString o){
    QString _on = o;
    if( _on  == "-" )_on = on;
    if( _on  == "videos" )return vb;
    else
        if( _on  == "fav_videos")return fvb;
    else
        if( _on  == "fav_scenes"  )return fsb;
    else
        if( _on  == "fav_items"  )return fib;
    else
        return foo;
}

QPointer<gLabel> favItems::item( int i, QString o ){
    QString _on = o;
    if( _on  == "-" )_on = on;
    if( _on  == "videos" && vb.count() > i )return vb.at(i);
    else
        if( _on  == "fav_videos" && fvb.count() > i )return fvb.at(i);
    else
        if( _on  == "fav_scenes" && fsb.count() > i )return fsb.at(i);
    else
        if( _on  == "fav_items" && fib.count() > i )return fib.at(i);
    else
        return nullptr;
}
QPointer<gLabel> favItems::currItem(){
    if(over_brick!=-1){
        return item( over_brick );
    }
    else
    {
        return nullptr;
    }
}
void favItems::share(){
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
void favItems::start_bg_proc(int t){
    if(t==-1)t=probe_freq;
    if(timer.isActive())timer.setInterval(t);
    else timer.start(t);
}
void favItems::stop_bg_proc(){
    if( !timer.isActive() )return;
    if(debg)qDebug()<<"STOP FAV TIMER";
    timer.stop();
}
void favItems::set_fav_videos_list( QJsonDocument doc ){
    if(debg)qDebug()<<"SET FAV VIDEOS:"<<doc.array().count()<<doc;
    int _cnt = doc.array().count();
    fav_videos_list.clear();
    int _fv = 0;
    for( int i = 0;i<_cnt;i++ ){
        int in = doc.array().at( i ).toObject().value( "title_id" ).toInt();
        fav_videos_list.append( in );
        _fv++;
    }
}
void favItems::create_fvideos_list(int cfm,int* cnt3,QJsonArray fav_titles, QJsonArray jfm){
    if(debg)qDebug()<<"SHOW FV"<<cfm;
    for(int i=0;i<cfm;i++){
        if(debg)qDebug()<<"CREATING:"<<i;
        int id=fav_titles.at(i).toObject().value(QStringLiteral("id")).toString().toInt();
        QJsonObject obj=jfm.at(i).toObject();
        titles[i] = obj.value("title").toString();
        if(debg)qDebug()<<"FAV_TITLES:"<<i<<id<<titles[i];
        QStringList imData=getThumbRef( obj.value("filename").toString(),titles[i] );
        if(debg)qDebug()<<"Create fav videos list: imData:"<<imData;
        if(!make_brick( i, id, titles[i], QString::number( cnt3[id] )+" items",imData.takeFirst(),imData.takeFirst(),imData.takeFirst(),creating ))return;
        mitems_cnt[i] = cnt3[id];
    }
    set_active();
}
QStringList favItems::getThumbRef(QString imPath,QString title){
    QFileInfo posterFile=QFileInfo( imPath );
    if(debg)qDebug()<<"getThumbsMain:"+imPath<<" postbase:"<<posterFile.baseName()<<title;
    QString poster_basename=posterFile.baseName();
    if(poster_basename==""){
        poster_basename=title.replace(" ","_");
    }
    QString posterFullPath=posterFile.path()+"/"+f_poster_prefix+poster_basename+".png";
    posterFullPath=posterFullPath.replace("/media/","/frames/");
    QString posterUrl=serverRoot+posterFullPath;
    QString posterDir= d_posters+poster_basename+".png";
        if(debg)qDebug()<<"POSTER BASENAME url:"<<posterUrl<<","<<posterDir;
    return {posterUrl,posterDir,serverRoot+imPath};
}
void favItems::mark_fav( int j ){
    if( fav_videos_list.contains( item( j )->db_index ) ){
        item( j )->mark_fav_videos();
    } else{
        item( j )->unmark_fav_videos();
    }
}
void favItems::create_videos_list(QList<FavsTable> list){
    creating="videos";
    int items_cnt=list.count();
    QString lab;
    FavsTable table;
    for(int ind=0;ind<items_cnt;ind++){
        table=list.takeFirst();
        if(table.index!=ind){
            if(debg)qDebug()<<"!!! mismatch Table index:"<<ind<<table.index;
        }
        mitems_cnt[ind] = table.param1.toInt();
        titles[ind] = table.title;
  if(debg)qDebug()<<"createvideos:"<<table.title;
        QStringList imData=getThumbRef( table.image, table.title);
        if(debg)qDebug()<<"Create videos list: imData:"<<imData;
        if(!make_brick(table.index,table.db_index, table.title,  lab, imData.takeFirst(),imData.takeFirst(),imData.takeFirst(),creating))return;
        mark_fav( table.index );
    }
    set_active();
}
void favItems::create_scenes_list(int cfs, QJsonDocument jfs,QJsonArray titles){
    if(debg)qDebug()<<"CREATE SCENES"<<cfs;
    creating="fav_scenes";
    int i;
    for(i=0;i<cfs;i++){
        QString filename;
        QJsonObject obj=jfs.array().at(i).toObject();
        int id=obj.value("id").toString().toInt();
        for(int j=0;j<titles.count();j++){
            if(titles.at(j).toObject().value("title").toString()==obj.value("title").toString()){
                filename=titles.at(j).toObject().value("filename").toString();
                if(!make_brick( i, id,
                                obj.value("title").toString(),
                                obj.value("time").toString(),
                                obj.value("frame").toString(),
                                d_carousel+QUrl(obj.value("frame").toString()).fileName(),
                                serverRoot+filename,creating))
                    break;
            }
        }
        events[i]=obj.value("event").toString();
    }

    set_active();
}
void favItems::create_items_list(int cfi, QJsonDocument jfi){
    creating="fav_items";
    for(int i=0;i<cfi;i++){
        QJsonObject obj=jfi.array().at(i).toObject();
        int id=obj.value("id").toString().toInt();
        if(!make_brick( i, id,
                        obj.value("name").toString()+"\n from \n "+obj.value("title").toString(),
                        "Duration:"+obj.value("time").toString(),
                        obj.value("image").toString(),
                        d_carousel+QUrl( obj.value("image").toString() ).fileName(),
                        "",creating ))return;
        events[i]=obj.value("event").toString();
    }
    set_active();
}
void favItems::toggleSelectors(bool v){
    ui->b_scenes_2->setVisible( v );
    ui->b_items_2->setVisible( v );
    ui->b_videos_2->setVisible( v );
    ui->b_scenes_2->setEnabled( v );
    ui->b_items_2->setEnabled( v );
    ui->b_videos_2->setEnabled( v );
    ui->b_scenes->setEnabled( v );
    ui->b_items->setEnabled( v );
    ui->b_videos->setEnabled( v );
    ui->switch_b_frame->setVisible( v );
}
void favItems::update_ui(){
    if(debg)qDebug()<<"UPDATEUI";
    bool ai = on  == "fav_items";
    bool as = on  == "fav_scenes";
    bool av = on  == "fav_videos";
    bool v = on  == "videos";
    toggleSelectors(!v);
    QString lab;
    if( ai )lab = ui->lab_info_items->text();
    if( as )lab = ui->lab_info_scenes->text();
    if( av )lab = ui->lab_info_videos->text();
    if( v )lab = ui->lab_info_res->text();
    glcontainer->set_labels( lab, nick );
    ui->b_scenes->setStyleSheet( states[as] );
    ui->b_scenes_2->setStyleSheet( states[as] );
    ui->b_scenes->setIcon( a[as] );
    ui->b_scenes_2->setIcon( a[as] );
    ui->b_items->setStyleSheet( states[ai] );
    ui->b_items_2->setStyleSheet( states[ai] );
    ui->b_items->setIcon( a[ai] );
    ui->b_items_2->setIcon( a[ai] );
    ui->b_videos->setStyleSheet( states[av] );
    ui->b_videos_2->setStyleSheet( states[av] );
    ui->b_videos->setIcon( a[av] );
    ui->b_videos_2->setIcon( a[av] );
    ui->favs_cnt->update();
    manageSelectors(true);
    update();
}
void favItems::set_active(){
    initializing=false;
    start_bg_proc();
    if(glcontainer->edit->isVisible())glcontainer->edit->setFocus();
    glcontainer->update();
    update();
    setEnabled(true);
    showBricks();
    if(debg)qDebug()<<"setact:"<<lastBrick()<<cnt;
    if(cnt==0)locked=false;
    if(!animating && !cleaning)locked=false;
    set_favs_cnt(cnt);
}
void favItems::set_wrappers_wraps(){
    if(debg)qDebug()<<"SET WRAPPER WRAPS";
    update_ui();
    glcontainer->setFrame(ui->switch_b_frame);
    glmapper->setFrames(ui->frame,ui->switch_b_frame_top);
    glcontainer->showSearch(false);
}
void favItems::set_markers(){
    marker.setParent(glcontainer);
    marker_first.setParent(glcontainer);
    marker_middle.setParent(glcontainer);
    marker.move(0,0);
    marker.setFixedSize(0,0);
    marker_first.move( ui->switch_b_frame->geometry().bottomLeft());
    marker_middle.move(ui->switch_b_frame->geometry().topLeft());
    marker_first.setFixedSize(0,0);
    marker_middle.setFixedSize(0,0);
    bsize=QSize(( 1-2*mrgh )*width(), main_h );
}
void favItems::hide_temps(){
    ui->frame->hide();
    ui->switch_b_frame->hide();
    ui->switch_b_frame_top->hide();
    ui->switch_b_frame_top->hide();
    ui->switch_b_frame->hide();
    update();
}
void favItems::removeOffscreen(QString o, int j){
    if(o=="videos")o="fav_videos";
    if(debg)qDebug()<<"REMOVE OFFSCREEN:"<<o<<j;
    int c=itemList(o).count();
    if(c>0){
        for(int i=0;i<c;i++){
            if(item(i,o)->db_index==j){
                if(debg)qDebug()<<"Remove"<<i<<"at"<<o;
                itemList(o).at(i)->close();
                itemList().removeAt(i);
            }
        }
    }
}
void favItems::set_favs_cnt( int i ){
    glcontainer->update_container( i );
    cnt = i;
}
void favItems::set_nick( QString ni ){
    ui->lab_nick->setText( ni );
    nick = ni;
}
void favItems::hide_top_wrapper(){
    if( pressed && swipe_orient  == not_set )
        hide_top = true;
}
void favItems::on_b_options_pressed(){
    ui->b_options->setStyleSheet( old_ss+";color:orange" );
}
void favItems::on_b_options_released(){
    ui->b_options->setStyleSheet( old_ss );
}
void favItems::on_b_items_pressed(){
    ui->b_items->setStyleSheet( button_ss_nact );
}
void favItems::on_b_scenes_pressed(){
    ui->b_scenes->setStyleSheet( button_ss_nact );
}
void favItems::on_b_videos_pressed(){
    ui->b_videos->setStyleSheet( button_ss_nact );
}
void favItems::on_b_videos_2_pressed(){
    on_b_videos_pressed();
}
void favItems::on_b_scenes_2_pressed(){
    on_b_scenes_pressed();
}
void favItems::on_b_items_2_pressed(){
    on_b_items_pressed();
}
void favItems::on_b_items_clicked(){
    if(debg)qDebug()<<"CLICKED B_ITEMS"<<locked;
    if( locked || on  == "fav_items")return;
    // toggle_switchers( false );
    //   if( br_deleted != -1 )return;
    emit go( "fav_items",{} );
}
void favItems::on_b_scenes_clicked(){
    if(debg)qDebug()<<"CLICKED B_scenS"<<locked<<on<<br_deleted;
    if( locked || on  == "fav_scenes")return;
    //toggle_switchers( false );
    //    if( br_deleted != -1 )return;
    emit go( "fav_scenes",{} );
}
void favItems::on_b_videos_clicked(){
    //  if(debg)qDebug()<<"CLICKED B_vidS"<<locked;
    if( locked || on  == "fav_videos")return;
    //  toggle_switchers( false );
    //   if( br_deleted != -1 )return;
    emit go( "fav_videos",{} );
}
void favItems::on_b_back_clicked(){
    if( locked)return;
    //  toggle_switchers( false );
    //    if( br_deleted != -1 )return;
    emit go( "back",{} );
}
void favItems::on_b_options_clicked(){
    if( locked)return;
    emit show_menu( ui->b_options->mapToGlobal( QPoint( 0, 0 ) ) );
}
void favItems::on_b_back_2_clicked(){
    on_b_back_clicked();
}
void favItems::on_b_scenes_2_clicked(){
    on_b_scenes_clicked();
}
void favItems::on_b_items_2_clicked(){
    on_b_items_clicked();
}
void favItems::on_b_videos_2_clicked(){
    on_b_videos_clicked();
}
void favItems::on_b_options_2_clicked(){
    on_b_options_clicked();
}
favItems::~favItems(){
    if(future.isRunning())future.waitForFinished();
if(debg)qDebug()<<"DELETING FAVS";
    delete ui;
}

