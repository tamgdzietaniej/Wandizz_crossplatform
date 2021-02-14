#include "mainwidget.h"
#include "math.h"
#include "globals.h"


MainWidget::MainWidget(int tp,QWidget *parent):
    QOpenGLWidget(parent),scr(0),last_created(-1),type(tp),dodebug(false),pdodebug(true)
{
    shwd=false;
    scrolling=false;
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_AlwaysStackOnTop,true);
    // setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_DeleteOnClose,true);
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    setMouseTracking(true);
    prev_buffer2=-1;
    lock=false;
    dpi=devicePixelRatio();
    //  dpi=1;
    if(debg)qDebug()<<"DPI"<<dpi;
    player_inited=false;
    lw=-1;
    rw=-1;
    playing=false;
    total_loaded=0;
    seeked=false;
    item_seeked=-1;
    buffer_load=-1;
    processing=0;
    shplayer=false;
    debg=false;
    items=0;
    is_video_frame=false;
    updated=false;
    omw=-2;
    need_update=false;
    all_loaded=false;
    mw=-1;
    curr_in_front=0;
    clustered=false;
    trip=0;
    pos=QPoint(0,0);
    img.load(":/gui/BANNERS/wandizz_trnsp.png");
    prev_pos=QPoint(0,0);
    diff.setX(0);
    pressed=false;
    hscroll_autopilot=0;
    wsize=QSize();
    start_time=0;
    end_time=0;
    player_enabled=false;
    emited=-1;
    left_offset=0;
    sscroling=false;
    hscroll_iner=0;
    slave=true;
    settin=false;
    first_frame=true;
    timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerev()),Qt::QueuedConnection);
    connect(this,SIGNAL(go_update()),this,SLOT(update()),Qt::QueuedConnection);
    connect(this,SIGNAL(timershot(int)),this,SLOT(setNextShot(int)),Qt::QueuedConnection);
    rotate_to_element=-1;
    pressed=false;
    marker_left=new QWidget(this);
    forbid.load(":/gui/APP ICONS/forbid.png");
    forbid.setDevicePixelRatio(dpi);
    type=tp;
    img=img.scaled(msize*.8*dpi);
    QSize _bsize(20*dpi*2,20*dpi*2);
    b_fav_image[0]=QImage(":/gui/ICONS_FAVS/fav5grays.png").scaled(_bsize,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    b_fav_image[1]=QImage(":/gui/ICONS_FAVS/fav5.png").scaled(_bsize,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    b_play_image[0]=QImage(":/gui/buttons/play_3.png").scaled(_bsize,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    b_play_image[1]=QImage(":/gui/buttons/play_2_d.png").scaled(_bsize,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    b_pause_image=QImage(":/gui/APP ICONS/pause.png").scaled(_bsize,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    b_volume_image[0]=QImage(":/gui/buttons/unmute.png").scaled(_bsize,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    b_volume_image[1]=QImage(":/gui/buttons/mute.png").scaled(_bsize,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
}

void MainWidget::set_slave(){
    if(!scrolling)
        slave=true;
}
void MainWidget::unset_slave(){
    slave=false;
}
void MainWidget::resizeEvent(QResizeEvent *e){
    resizeGL(e->size().width()*dpi,e->size().height()*dpi);
    if(!type)msize=QSize(e->size().height()*1.76,e->size().height());
    else msize=QSize(e->size().height()+2,e->size().height());
    update();
    e->accept();
}
void MainWidget::prepare(struct event_data ed[],int len,QSize siz,QString netf,QList<int> favss){
    favs=favss;
    netfile=netf;
    left_offset=rect().width()/2-siz.width()/2;
    marker_left->setGeometry(0,0,left_offset,siz.height());
    marker_left->setFixedSize(left_offset,siz.height());
    prev_time=-1;
    par_idx=0;
    msize=siz;
    bw=msize.width()*.3;
    bh=msize.height()*.3;
    by=msize.height()*.7;
    bx=left_offset+msize.width()*.7;

    button_rect.setRect(left_offset+msize.width()-30,msize.height()-30,25,25);
    button2_rect.setRect(bx,by,bw,bh);
    volume_rect.setRect(left_offset+msize.width()-70,msize.height()-27,17,18);
    volume2_rect.setRect(left_offset+msize.width()-90,msize.height()-50,50,50);
    ww=siz.width();
    play_rect.setRect(left_offset+5,msize.height()-27,20,20);
    play2_rect.setRect(left_offset+0,msize.height()-50,50,50);

    forbid_rect=play_rect.adjusted(-4,-2,0,2);
    forbid=forbid.scaled(forbid_rect.size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ww=siz.width();
    for(int i=0;i<len;i++){
        if(ed[i].etime==prev_time && type==0){
            cluster[i]=par_idx-1;
            clustered=true;
        } else {
            if(type==0){
                if(clustered)last_assigned[par_idx-1]=i-1;
            }
            if(type)
                filename[par_idx]=ed[i].item_file;
            else
                filename[par_idx]=ed[i].frame_file;
            slide[par_idx].image=&img;
            slide[par_idx].image->setDevicePixelRatio(dpi);
            slide[par_idx].shown=false;
            slide[par_idx].waiting_swap=false;
            cluster[i]=par_idx;
            last_assigned[par_idx]=i;
            if(type==0){
                first_assigned[par_idx]=i;
            }
            ev_data[par_idx]=ed[i];
            prev_time=ed[i].etime;

            if(objectName()=="sceneframe")
                slide[par_idx].is_fav=favs.contains(ev_data[par_idx].scene_id);
            else {
                if(debg)qDebug()<<"CHECK:"<<ed[i].item_id<<ev_data[par_idx].item_id<<favs.contains(ev_data[par_idx].item_id);
                slide[par_idx].is_fav=favs.contains(ev_data[par_idx].item_id);
            }
            last_created=par_idx;
            par_idx++;
            items=par_idx;
            if(debg)qDebug()<<"make widget:"<<par_idx;
        }
        aspect=Qt::KeepAspectRatio;
        if(!type)aspect=Qt::IgnoreAspectRatio;
    }
    qDebug()<<"ITEMS:"<<len<<items<<par_idx;
}
void MainWidget::set_favs(){
    for(int i=0;i<items;i++){
        if(objectName()=="sceneframe")slide[i].is_fav=favs.contains(ev_data[i].scene_id);
        if(objectName()=="itemframe")slide[i].is_fav=favs.contains(ev_data[i].item_id);
    }
}
void MainWidget::mouseMoveEvent(QMouseEvent *e){
    if(items==0 || mw<0)
        return;
    if(!sscroling && !scroll_locked){
        if(abs(abs(pos.x())+abs(press_pos.x()))>5*dpi){
            sscroling=true;
        } else {
            return;
        }
    }
    if(sscroling){
        pos=e->pos();
        diff.setX((pos.x()-prev_pos.x())*devicePixelRatio()/2);
        prev_pos=pos;
        if(!scroll_locked)
            scr+=diff.x();
        last_zeros=0;
        diffs.append(diff.x());
        if(diffs.count()>3)diffs.removeFirst();
        no_move=false;
        if(debg)qDebug()<<"scr:"<<diffs;
    }
    if(!p_stop_emitted && !volume2_rect.contains(e->pos()) && !button2_rect.contains(e->pos()) && !play2_rect.contains(e->pos())){
        if(debg)qDebug()<<"P_STOP";
        p_stop_emitted=true;
        if(type==0)
            stop();
        else
            emit stop_p();
    }
}
void MainWidget::close_video(){
#if defined (Q_OS_ANDROID)
    if(is_playing()){
        player->stop();
        update();
    }
#endif
}
void MainWidget::initializeGL(){
    glClearColor(1,1,1,0);
}
void MainWidget::paintGL(){
    if(items==0)return;
    QPainter painter(this);
    painter.beginNativePainting();
    glClear(GL_COLOR_BUFFER_BIT);
    painter.endNativePainting();
    painter.setBackgroundMode(Qt::TransparentMode);

    double sc=double(mw_geo.center().x()-geometry().x())/double(width()/2);
    int offs=marker_left->geometry().right();
    QRect geo=QRect(mw*msize.width()+offs+left_offset,0,wsize.width(),wsize.height());
    for(int i=0;i<=last_created;i++){
        if(is_video_frame && i==mw ){
            if(!vim.isNull())
                painter.drawImage(mw_geo.topLeft(),vim,slide[mw].image->rect());
            else {
                if(debg)qDebug()<<"NULL IMAGE!!";
                is_video_frame=false;
            }
        }
        /* WIDGET PAINTING */
        if(!is_video_frame){
            if(slide[i].waiting_swap){
                //  qDebug()<<"PAINT SWAP:"<<slide[i].image<<slide[i].swap_img<<type;
                slide[i].image=static_cast<QImage*>(slide[i].swap_img);
                slide[i].image->setDevicePixelRatio(dpi);
                slide[i].waiting_swap=false;
            }
            //      ox=(msize.width()-slide[i].image->width()/dpi)/2;
            ox=0;
            y=(msize.height()-slide[i].image->height()/dpi)/2;
            y=0;
            //    w=slide[i].image->width()/dpi;
            w=msize.width();
            h=msize.height();
            //     h=slide[i].image->height()/dpi;
            x=i*msize.width()+offs;//+ox;

            geo=QRect(x,y,w,h);
            QRect srect=slide[i].image->rect();

            if(geo.right()>0 && geo.left()<width()){
                if(geo.left()<0){
                    srect.adjust(geo.left(),0,0,0);
                    geo.adjust(geo.left(),0,0,0);
                }
                if(geo.right()>width()){
                    srect.adjust(0,0,width()-geo.right(),0);
                    geo.adjust(0,0,width()-geo.right(),0);
                }
                int l=i*msize.width()+offs;
                int r=(i+1)*msize.width()+offs;
                painter.drawImage(geo.topLeft(),*slide[i].image,srect);
                painter.setBrush(QColor(0,0,0,0));
                painter.setPen(QColor(0,0,0,255));
                if(l>0 && l<width())painter.drawLine(l,0,l,height());
                if(r>0 && r<width())painter.drawLine(r,0,r,height());
            }
        }
    }
    //  if( b_opacity ){
    if( !type && player_inited ){
        if(!player_enabled && !pressed){
            painter.setPen(QPen(QColor(255,255,255,255)));
            painter.setFont(QFont("Apple Gothic",8));
            QString pinfo="Video not available";
            if(player_url!="")
                pinfo="BUFFERING MEDIA:"+buff_perc;
            painter.drawText(play_rect.center(),pinfo);
        } else {
            if(is_playing()){
                painter.drawImage(play_rect,b_pause_image,b_play_image[0].rect());
                painter.drawImage(volume_rect,b_volume_image[player->isMuted()],b_volume_image[0].rect());
            }
            else {
                painter.drawImage(play_rect,b_play_image[1],b_play_image[0].rect());
            }

        }
    }
    painter.drawImage(button_rect,b_fav_image[slide[mw].is_fav],b_fav_image[slide[mw].is_fav].rect());
    //}
    painter.end();
    need_update=false;
}

void MainWidget::mouseReleaseEvent(QMouseEvent *e){
    p_stop_emitted=false;
    if(items==0 || mw<0)return;
    pressed=false;
    if(abs(get_trip_to(mw))>2){
        scrolled=true;
    }
    if(scroll_locked){
        /* PLAYER MANAGEMENT */
        if(type==0 && player_enabled){
            if(debg)qDebug()<<"VOL:"<<volume2_rect.contains(e->pos())<<volume2_rect<<e->pos();
            if(is_playing()){
                if(volume2_rect.contains(e->pos())){
                    player->setMuted(!player->isMuted());
                }
                if(!volume2_rect.contains(e->pos())){
                    if(debg)qDebug()<<"STOP";
                    stop();
                }
            } else {
                if(play2_rect.contains(e->pos())){
                    if(debg)qDebug()<<"Play";
                    play();
                    scroll_locked=true;
                }
            }
        }
    } else{
        if(scrolled){
            int i,v=0,ii=0;
            int vv=0;
            if(diffs.count()>0){
                for(i=0;i<diffs.count();i++){
                    if(abs(v)<abs(diffs.at(i))){
                        v=diffs.at(i);
                    }
                    vv+=diffs.at(i);
                    if(diffs.at(i)!=0)ii++;
                }
                if(ii!=0)
                    hscroll_iner=vv/ii;
                diffs.clear();
            }
            scrolled=false;
            if(abs(get_trip_to(mw))<2 && hscroll_iner==0.0 && type && !scroll_locked){
                emit go("web",{ev_data[mw].event});
            } else {
                scrolling=true;
            }
        } else {
            /* ADD FAV BUTTON CLICK */
            if(button2_rect.contains(e->pos())){
                if(dodebug)if(debg)qDebug()<<"ADD FAV CLICK";
                emit add_fav_click();
                slide[mw].is_fav=!slide[mw].is_fav;
                scroll_locked=true;
                update();
            }
        }
    }
    scroll_locked=false;
    sscroling=false;
}
void MainWidget::mousePressEvent(QMouseEvent *e){
    if(items==0 || mw<0)return;
    prev_pos=e->pos();
    press_pos=prev_pos;
    pos=prev_pos;
    hscroll_iner=0;
    hscroll_autopilot=0;
    slave=false;
    diff.setX(0);
    diffs.clear();
    p_stop_emitted=false;
    unset_slave();
    if(objectName()=="itemframe")emit stop_p();
    pressed=true;
    catapult=0;
    scr=0;
    rotate_to_element=-1;
    scroll_locked=false;
    scrolled=scrolling;
    start_bg_proc();
}
void MainWidget::timerev(bool st){
    if(items==0 || lock==true){
        return;
    }
    get_coords();
    if(!all_loaded)
        if(no_move)last_zeros++;
    if(last_zeros>=10){
        diffs.clear();
        last_zeros=0;
    }
    no_move=true;
    curr_in_front=mw;
    if(!pressed){
        if(rotate_to_element==-1){
            // fit to cell
            if(abs(hscroll_iner)<12){
                int gt=get_trip_to(mw);
                if(hscroll_iner<0 && gt>0){
                    rotate_to_element=rw+(1*(-hscroll_iner>gt));
                } else
                    if(hscroll_iner>0 && gt<0){
                    rotate_to_element=lw-(1*(hscroll_iner>-gt));
                } else {
                    rotate_to_element=mw;
                }
            } else {
                hscroll_iner*=0.94+(fabs(catapult)/right)*0.04;
                scr+=hscroll_iner;
            }
        } else hscroll_iner=0;
        if(bleft+left_offset<0 || (last_created)*msize.width()<bleft+left_offset){
            hscroll_iner/=4;
            rotate_to_element=mw;
        }

        int trip=get_trip_to(mw);
        if(abs(trip)<40){
            if(rotate_to_element!=-1 && trip!=prev_dist){
                prev_dist=trip;
            }
        }
        // Autorotation
        if(rotate_to_element!=-1){
            trip=get_trip_to(rotate_to_element);
            hscroll_iner=0;
            hscroll_autopilot=asqrt(trip);//*(fabs(asqrt(trip)/5));
            if(abs(trip)<=4)
                hscroll_autopilot=trip;
            if(trip==0){
                rotate_to_element=-1;
            }
            scr+=hscroll_autopilot;
        }
    }
    scrolling=abs(scr)>2;
    if(scr!=0){
        scroll(scr,0);
        seeked=false;
    } else {
        scrolling=false;
        //     if(b_opacity!=1 && !pressed){
        if(!pressed){
            //       b_opacity=1;
            //      needs_update();
            if( emited!=mw && !slave ){
                emited=mw;
                emit spin_friend( mw );
                if(!type)seek(ev_data[mw].etime);
            }
            if(!type && !seeked){
                seek(ev_data[mw].etime);
            }
        }
    }
    scr=0;
    hscroll_autopilot=0;
    if(need_update){
        update();
    }
    if(st){
        if(!timer->isActive())timer->start(17);
    } else {
        if(!need_update && scr==0 && !pressed && rotate_to_element==-1 && timer->isActive() && !scrolling){
            timer->stop();
        }
    }
}
qreal MainWidget::get_trip_to(int tr){
    if(tr<0 || tr>last_created)return 0;
    return left+bleft-tr*msize.width();
}
bool MainWidget::set_trip_to(int t){
    if(mw<0)return false;;
    int tr=t;
    if(type==0){
        tr=cluster[t];
    }
    if(tr==mw || tr==rotate_to_element)return false;
    rotate_to_element=tr;
    start_bg_proc();
    return true;
}

void MainWidget::get_coords(){
    // lw, [mw], rw - left, middle, right indexes of visible widgets ( 0 -> first)
    // [mw] - nearest middle position
    // lw, rw - neighbours
    left=mapFromGlobal(QPoint(0,0)).x()+left_offset;
    right=width();
    bleft=-marker_left->geometry().right();
    bright=bleft+right;
    bmid=bleft+right/2;
    lw=bleft/ww;
    mw=bmid/ww;
    rw=bright/ww;
    prl=lw-double(bmid)/double(ww);
    prm=mw-double(bmid)/double(ww);
    prp=rw-double(bmid)/double(ww);

    if(rw>last_created)rw=last_created;
    if(mw>last_created)mw=last_created;
    if(lw>last_created)lw=last_created;
    curr_in_front=mw;
    if(mw<0)mw=0;
    if(lw<0)lw=0;
    if(rw<0)rw=0;
}
void MainWidget::getPics(){
    future=QtConcurrent::run([=]() {
        QPainter p;
        for(int genw=0;genw<items;genw++){
            if(!slide[genw].shown){
                if(QFileInfo::exists(filename[genw] + ".lock"))qDebug()<<"LOCK!!!";
                if(QFileInfo::exists(filename[genw]) && !QFileInfo::exists(filename[genw] + ".lock")){
                    //   if(type)qDebug()<<"loading:all_loaded:"<<all_loaded<<total_loaded<<items<<last_created;
                    QImage i(filename[genw]);
                    i.setDevicePixelRatio(dpi);
                    i=i.scaled(msize,aspect,Qt::SmoothTransformation);
                    QImage it(msize,i.format());
                    it.setDevicePixelRatio(dpi);
                    QBrush br(i);
                    p.begin(&it);
                    p.setRenderHint(QPainter::Antialiasing);
                    p.fillRect(it.rect(),QColor(255,255,255,255*type));
                    int xa=(it.width()-i.width())/2;
                    int ya=(it.height()-i.height())/2;
                    p.drawImage(it.rect().adjusted(xa,ya,-xa,-ya),i);
                    p.end();
                    /*
                    QImage ii(it.size()-QSize(1,0),it.format());
                    p.begin(&ii);
                    p.setPen(QColor(255,255,255,150*type));
                    p.setBrush(it);
                    p.drawRoundedRect(ii.rect(),5,5);
                    p.end();
*/
                    slide[genw].swap_img=new QImage(it);
                    slide[genw].image->setDevicePixelRatio(dpi);
                    qDebug()<<"Loaded image:"<<slide[genw].swap_img->size()<<msize;
                    need_update=true;
                    total_loaded++;
                    qDebug()<<all_loaded<<total_loaded<<last_created+1;
                    all_loaded=total_loaded==last_created+1;

                    //  qDebug()<<"Loaded image:"<<total_loaded<<last_created+1;
                    slide[genw].shown=true;
                    slide[genw].waiting_swap=true;
                    if(isVisible())emit go_update();
                    if(all_loaded)qDebug()<<"ALL LOADED!";
                    break;
                }
            }
        }
        if(!all_loaded)
            emit timershot(50);
    });
}
void MainWidget::setNextShot(int t){
    QTimer::singleShot(t, this, SLOT(getPics()));
}

MainWidget::~MainWidget(){
    timer->stop();
    if(is_playing())player->stop();
    future.cancel();
    future.waitForFinished();
    qDebug()<<"WAND:MAINWIDGET:DESTRuctor mainwidget";
}

/*  PLAYER SECTION */
/* *************** */
void MainWidget::set_player(){
    if(!shplayer)return;
    player=new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    video=new MyVideoSurface(this);
    player->setNotifyInterval(200);
    video->setState(true);
    player->setVideoOutput(video);
    catch_stop=true;
    if("" == QUrl(player_url).fileName()){
        player_enabled=false;
        return;
    } else {
        QMediaContent mc((QUrl(player_url)));
        player->setMedia(mc);
        player_enabled=true;
    }
    // if(player_enabled)player->setPosition(0);
    end_time=player_offset;
    player->setMuted(true);
    player->pause();
    if(player_enabled){
        connect(video,SIGNAL(hasImage(const QImage&,bool)),this,SLOT(setImage(const QImage&,bool)));
        connect(player,SIGNAL(bufferStatusChanged(int)),this,SLOT(media_buffer_filled_info(int)));
        connect(player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(media_info(QMediaPlayer::MediaStatus)));
        connect(player,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(state_changed(QMediaPlayer::State)));
        connect(player, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
                [=](QMediaPlayer::Error error){ if(debg)qDebug()<<"TADAAA: erooor:"<<error; });
    }
    if(debg)qDebug()<<"PLAYER:"<<player->state();
}
void MainWidget::init_player(QString url){
    if(!shplayer)return;
    player_url=url;
    //  player_url=player_url.replace("https://","http://");
    if(debg)qDebug()<<"INIT PLAYER: "<<QUrl(player_url).fileName();
    buff_perc="0%";
    player_inited=true;
    if(0 == type)set_player();
}
void MainWidget::play(){
    if(!player_enabled)return;
    playing=true;
    is_video_frame=false;
    int offs=marker_left->geometry().right();
    x=(msize.width()-slide[mw].image->width()/dpi)/2;
    y=(msize.height()-slide[mw].image->height()/dpi)/2;
    w=slide[mw].image->width()/dpi;
    h=slide[mw].image->height()/dpi;
    mw_geo=QRect(mw*msize.width()+offs+x,y,w,h);
    last_frame=false;
    catch_stop=false;
    //  video->setState(true);
    frames_cnt=0;
    player->play();
    start_bg_proc();
}
void MainWidget::start_bg_proc(){
    if(!all_loaded)getPics();
    if(!timer->isActive())timerev(true);
}
void MainWidget::seek(qint64 curr_pos,bool){
    if(!player_enabled)return;
    seeked=true;
    end_time=curr_pos;
    start_time=curr_pos-player_offset;
    if(start_time<0)start_time=0;
    if(player_enabled)
        if(player->position()==start_time){
            if(pdodebug)if(debg)qDebug()<<"Seek not needed!";
        } else {
            player->setPosition(start_time);
            if(pdodebug)if(debg)qDebug()<<"Media PLAYER:TRIGGER SEEK TO "<<start_time<<player->position()<<player->state()<<player_offset;
        }
}
void MainWidget::stop(){
    if(!player_enabled){
        return;
    }
    if(pdodebug)if(debg)qDebug()<<"Media PLAYER:TRIGGER STOP AT "<<player->position()<<player->state();
    if(player->state()==QMediaPlayer::PausedState){
        if(pdodebug)if(debg)qDebug()<<"Already stopped";
    } else {
        player->pause();
    }
    //  video->setState(false);
    is_video_frame=false;
    catch_stop=false;
    playing=false;
    rewind();
    needs_update();
}
void MainWidget::rewind(){
    seek(end_time);
}
void MainWidget::setImage(const QImage& im,bool swrgb){
    if(im.isNull()){
        if(debg)qDebug()<<"null im";
        return;;
    }
    if(debg)qDebug()<<"SET IMAGE";
    frames_cnt++;
    if(first_frame){
        if(pdodebug)
            if(debg)qDebug()<<"Media PLAYER:Transport START - IMG PARAMS:"<<im.size()<<","<<im.size()<<","<<im.depth();
        first_frame=false;
    }
    vim=im.scaled(msize,Qt::IgnoreAspectRatio);
    if(player->position()>end_time || !is_playing() || catch_stop){
        if(catch_stop){
            if(pdodebug)if(debg)qDebug()<<"CATCH STOP LAST FRAME";
        }
        if(pdodebug)if(debg)qDebug()<<"Media PLAYER:End of media catch avg "<<(frames_cnt*1000/player_offset)<<"fps ["<<frames_cnt<<" frames at "<<player_offset<<"ms]";
        stop();
        return;
    }
    is_video_frame=true;
    needs_update();
}
void MainWidget::needs_update(){
    need_update=true;
    start_bg_proc();
}
void MainWidget::media_info(QMediaPlayer::MediaStatus s){
    if(pdodebug)if(debg)qDebug()<<"Media PLAYER STATUS:"<<s;
    if(QMediaPlayer::InvalidMedia == s || QMediaPlayer::BufferingMedia == s)needs_update();
}

void MainWidget::state_changed(QMediaPlayer::State state){
    if(pdodebug)if(debg)qDebug()<<"Media PLAYER STATE:"<<state;
    if(state==QMediaPlayer::PausedState && is_video_frame){
        if(pdodebug)if(debg)qDebug()<<"Media PLAYER:Flush overtime frame";
        playing=false;
        needs_update();
    } else
        if(state==QMediaPlayer::PlayingState ){
        playing=true;
        if(pdodebug)if(debg)qDebug()<<"Media PLAYER:Re-init start flag and counter";
        first_frame=true;
        frames_cnt=0;
    }
}
void MainWidget::media_buffer_filled_info(int perc){
    if(prev_buffer!=perc){
        if(pdodebug)if(debg)qDebug()<<"Media PLAYER BUFFER"<<perc<<player->position()<<player->state();
        prev_buffer=perc;
        buff_perc=QString::number(perc)+"%";
        if(buff_perc>0)player_enabled=true;
        needs_update();
    }
}
bool MainWidget::is_playing(){
    return playing;
    //  return  player->state()==QMediaPlayer::PlayingState;
    //  if(debg)qDebug()<<"is_playing_stop";
}
bool MainWidget::perform_close(){
    if(!player_enabled)return true;
    qDebug()<<"Performing close:"<<player->state()<<video->isActive();
    while(player->state()!=QMediaPlayer::StoppedState){
        player->stop();
        if(debg)qDebug()<<"Stopping player";
        //  QApplication::processEvents();
    }
    while(video->isActive()){
        video->stop();
        if(debg)qDebug()<<"Stopping surface";
        QApplication::processEvents();
    }
    future.cancel();
    future.waitForFinished();
    return true;
}
