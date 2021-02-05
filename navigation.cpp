
#include "navigation.h"

void navigation::restart_clean(){
    //   go_to("loginFB",{"logout"});
    QFile(f_app_hash).remove();
    QFile(f_user_avatar).remove();
    creds_ok=false;
    updated=false;
    shadow=false;
    int wc=count();
    for(int i=0;i<wc;i++){
        if(widget(i)!=sign_in){
            // widget(i)->deleteLater();
            delete widget(i);
        }
    }
    init_app();
    go_to("sign_in");
}
void navigation::get_current_user_data(){
    offline_mode=credentials->offline_m;
    if(!offline){
        s_curr_id=QString::number(credentials->curr_id);
        curr_user=credentials->curr_user;
        curr_nick=credentials->curr_nick;
        curr_phone=credentials->curr_phone;
    } else {
        if(QFileInfo::exists(f_user_login) && QFileInfo::exists(f_user_id) && QFileInfo::exists(f_user_nick)){
            curr_user=credentials->getFile(f_user_login);
            s_curr_id=credentials->getFile(f_user_id);
            curr_nick=credentials->getFile(f_user_nick);
            curr_phone=credentials->getFile(f_user_phone);

        } else {
            info("Sorry, but to use WANDIZZ, it must be initialized online once.\n Application will now exit.");
            QApplication::exit();
        }
    }
}
void navigation::check_email_exists(QString email,bool){
    profsettings->must_chk_email=false;
    if(email==curr_user){
        if(debg)qDebug()<<"email=curr_user"<<curr_user;
        return;
    }
    profsettings->checkEmail(credentials->check_user(email)!=0);
}
void navigation::save_profile(){
    if(debg)qDebug()<<profsettings->upd_login<<profsettings->upd_pass<<profsettings->upd_phon;
    update_user(profsettings->upd_login,profsettings->upd_pass,profsettings->upd_nick,profsettings->upd_phon);
    go_to("back");
}
void navigation::update_user(QString login,QString pass,QString nick,QString phone){

    credentials->update_user(login,pass,nick,phone);
    updated=true;
    curr_user=credentials->curr_user=login;
    curr_nick=credentials->curr_nick=nick;
    curr_phone=credentials->curr_phone=phone;
    credentials->curr_pass=pass;
    if(debg)qDebug()<<curr_user<<curr_nick<<curr_phone;
    credentials->upload_file(f_user_avatar);
}
void navigation::recovery_user(){
    credentials->get_user_data(profsettings->upd_login,profsettings->upd_pass);
}

void navigation::show_web(QString url){
    go_to("web",{url});
}

void navigation::proceed_gps(){
    if(gps!=nullptr)return;
    gps=new localization(this);
    connect(gps,SIGNAL(new_coords(QString,QString,QString)),this,SLOT(update_loc(QString,QString,QString)),Qt::QueuedConnection);
}

// FORMS INIT
void navigation::proceed_sign_in(){
    if(sign_in==nullptr){
        sign_in=new SignIn();
        sign_in->setObjectName("sign_in");
        geom->resizer(sign_in);
        connect(sign_in,SIGNAL(go(QString,QStringList)),this,SLOT(go_to(QString,QStringList)),Qt::QueuedConnection);

    }
}
bool navigation::navlock(){
    if(lock){
        if(debg)qDebug()<<"Locked already";
        return true;
    }
    else
    {
        lock=true;
        if(count()>0){
            currentWidget()->setEnabled(false);
        }
        return false;
    }
}
bool navigation::unlock(){
    bool test=lock;
    lock=false;
    if(count()>0){
        if(!currentWidget()->isEnabled())
            currentWidget()->setEnabled(true);
    }
    return test;
}
void navigation::share(){
    QString text = "WANDIZZ-HAVE to HAVE";
    QUrl url( "https://wandizz.com" );
#if defined( Q_OS_ANDROID )
    share_cross* share = new share_cross( this );
    share->share( text, url );
#elif defined ( Q_OS_IOS )
    shareios* share = new shareios();
    share->share( text, url );
#endif
}
void navigation::implodeContent(){
    if(wgen!=nullptr){
        if(wgen->isExpanded()){
            wgen->lock();
        }
    }
}
bool navigation::isCWName(QString n){
    if(!count())return false;
    return currentWidget()->objectName()==n;
}
// mrk::go_to
void navigation::go_to(QString to,QStringList params){
    curr_params=params;
    menu_opened=false;
    if(count()==0){
        to="sign_in";
    }
    else{
        if(to=="web"){
            QDesktopServices::openUrl(QUrl(params.takeFirst()));
            return;
        } else if(to=="exit_app"){
            deleteLater();
            return;
        }
        if(mmenu!=nullptr){
            if(to=="options" && sender()==mmenu){
                hideMenu();
            }
            menu_opened=mmenu->isVisible();
        }
        if(menu_opened){
            mmenu->hide();
            menu_opened=false;
        }
        /* RE-USABLE STUFF */
        qDebug()<<"goto:"<<to<<menu_opened;

        if(videos!=nullptr)
            if(currentWidget()==videos)
                videos->wgen->toggleBricks(false);


        // silent=false;
        //  media_queue->gclearList();
        /* " ACTION:Back  */
        if(to.contains("back")){
            //    if(count()>0){
            qDebug()<<"gotob:"<<to;
            if(menu_opened){
                hideMenu();
                currentWidget()->setEnabled(true);
                currentWidget()->activateWindow();
                if(currentWidget()==videos){
                    videos->activateContainer();
                }
            } else {
                int cn=count();
                removeWidget(currentWidget());
                update();
                qDebug()<<currentWidget()->objectName();
                qDebug()<<"BACK -> redirect to:"<<currentWidget();
                if(count()>1)
                    go_to(currentWidget()->objectName(),{"last"});
                else go_to("userprof");
            }
            return;
        } else
            currentWidget()->hide();
        QApplication::processEvents();
        /* FULL WANDIZZ */
        if(to=="full"){
            credentials->context="";
            credentials->context_id=-1;
            updateFavs();
            go_to("userprof");
            qDebug()<<"FULL";
            return;
        } else
            /* ADMINISTRATION REDIR */
            // user profile screen
            if(to=="userprof"){  //user-profile
            if(params.count()==3){  // set context form prospect
                /* MAKE CONTEXT */
                if(params.at(0)=="context"){
                    params.takeFirst();
                    credentials->context=params.takeFirst();
                    credentials->context_id=params.takeFirst().toInt();
                    if(mmenu!=nullptr)
                        mmenu->setContext(true);
                    updateFavs();
                }
            }
            proceed_userprof();
            show_module(userprof);
            // proper destination -> profile [home] screen
        }
        /* SHARE */
        if(to=="share"){ //share
            share();
            return;
        } else
            /* ACCOUNT RECOVERY */
            if(to=="profset"){  //profile page
            proceed_profsettings(params.contains("accrec"));
            shadow=true;
            show_module(profsettings);
        } else
            /* SYNC-SCREEN */
            if(to=="sync"){  // videorec
            proceed_sync();
            shadow=true;
            show_module(sync);
        } else
            /* LAUNCH MENU */
            if(to=="menu"){  // mmenu
            showMenu();
            return;
        } else
            /* GO "FAVS" BUTTON PRESSED [JUST FAVS - not PRECISED WHICH] */
            if(to=="favs"){
            to="fav_videos";
            // determine which are not empty
            if(!fav_videos->count()){
                if(fav_scenes->count()){
                    to="fav_scenes";
                } else if(fav_items->count()){
                    to="fav_items";
                }
            }
        }
        if(to=="videos"){
            proceed_media("videos");
            shadow=true;
            show_module(videos);
            videos->create_videos_list(credentials->timeline_count_by_title_id,media);
            return;
        }
        else
            if(to=="prospect"){
            proceed_media("prospect");
            shadow=true;
            show_module(videos);
            videos->create_prospect_list(credentials->timeline_count_by_title_id,media);
            return;
        }
        else
            if(to=="carousel"){
            shadow=true;
            if(params.count()==5){
                prev_cpars.clear();
                prev_cpars.append(params);
            }
            proceed_carousel(prev_cpars);
            show_module(carousel);
        } else
            if(to=="fav_scenes"){
            if(fav_scenes->count() || show_even_if_zero){
                proceed_media("fav_scenes");
                shadow=true;
                show_module(videos);
                videos->create_scenes_list(credentials->timeline_count_by_title_id,fav_scenes);
                return;
            }
        } else
            if(to=="fav_items"){
            if(fav_items->count() || show_even_if_zero){
                proceed_media("fav_items");
                shadow=true;
                show_module(videos);
                videos->create_items_list(fav_items);
                return;
            }
        } else
            if(to=="fav_videos"){
            if(fav_videos->count() || show_even_if_zero){
                proceed_media("fav_videos");
                shadow=true;
                show_module(videos);
                qDebug()<<fav_videos;
                videos->create_fvideos_list(credentials->timeline_count_by_title_id,fav_videos);
                return;
            }
        } else
            /* SOCIAL LOGINS */
            if(to=="loginFB"){
            proceed_webview();
            webview->setFacebookLogin();
            webview->setUrl(loginFBurl+"?action="+params.takeFirst());
            show_module(webview);
        }
        else if(to=="loginTT"){
            proceed_webview();
            webview->setUrl(loginTWurl+"?action="+params.takeFirst());
            show_module(webview);
        }
        else if(to=="loginGoogle"){

        }
        else if(to=="loginApple"){
            QByteArray secret(credentials->get_jwt());
            QString url=loginAPurl+"?response_type=code&client_id=com.wandizz.producer&scope=email%20name&response_mode=form_post&usePopup=false&redirect_uri=https://producer.wandizz.com/authorize/apple/check.php";
            proceed_webview();
            webview->setAppleLogin();
            webview->setUrl(url);
            show_module(webview);
        }
    }
    if(to=="sign_in"){
        shadow=false;
        proceed_sign_in();
        show_module(sign_in);
    }
}
// mrk:show_module
void navigation::show_module(QWidget* to){
#if defined(Q_OS_ANDROID)
    if(splash)
        QtAndroid::hideSplashScreen();
    splash=false;
#endif
    unlock();
    if(to!=nullptr)
    {
        if(currentWidget()!=to)
            addWidget(to);
        set_labels();
        if(count()==2 && to->objectName()!="prospect"){
            go_to("prospect",{"search"});
            return;
        }
        to->setEnabled(true);
        setCurrentWidget(to);
        currentWidget()->raise();
        currentWidget()->showFullScreen();
        currentWidget()->activateWindow();
        update();
        rwidget->show();
        QApplication::processEvents();
        rwidget->hide();
    }
    disableCurrMenuEntry();
}
void navigation::resetContext(){
    credentials->context="";
    credentials->context_id=-1;
    updateFavs();
}
void navigation::updateFavs(){
    qDebug()<<"UpdateFavs";
    credentials->get_favs(true);
    credentials->get_stats(true);
    credentials->prepare_titles();
    media=&credentials->jtit;
    fav_videos=&credentials->jfv;
    fav_scenes=&credentials->jfs;
    fav_items=&credentials->jfi;
    producers=&credentials->jp;
    if(wgen)
        wgen->set_fav_videos_list(fav_videos);
    if(carousel!=nullptr){
        carousel->set_fav_items_list(fav_items);
        carousel->set_fav_scenes_list(fav_scenes);
        carousel->check_mw_fav();
    }
    if(count()>0)
        if(videos!=nullptr){
            int c=0;
            if(wgen->getContext().contains("fav_videos"))c=fav_videos->count();
            else if(wgen->getContext().contains("fav_scenes"))c=fav_scenes->count();
            else if(wgen->getContext().contains("fav_items"))c=fav_items->count();
            else if(wgen->getContext().contains("videos"))c=titles_cnt;
            else qDebug()<<"!!! set_favs_cnt:"<<wgen->getContext();
            if(wgen->getContext()!="prospect") videos->set_favs_cnt(c);
        }
}

void navigation::show_carousel(){
    show_module(carousel);
}
void navigation::proceed_sync(){
    if(sync==nullptr){
        sync=new SyncScreen();
        sync->setObjectName("sync");
        geom->resizer(sync);
        sync->initClasses();
        sync->screenw=geom->nw;
        sync->screenh=geom->nh;
        connect(sync,SIGNAL(go(QString,QStringList)),this,SLOT(go_to(QString,QStringList)));
    }
    sync->curr_user=curr_user;
    sync->curr_id=s_curr_id;
}
void navigation::proceed_webview(){
    if(webview!=nullptr){
        webview->close();
    }
    webview=new socialLogin();
    webview->setObjectName("webview");
    geom->resizer(webview);
    webview->createWebView();
    connect(webview,SIGNAL(go(QString)),this,SLOT(go_to(QString)));
    connect(webview,SIGNAL(gotSocialData(QString,QString,QString,QString,QString)),this,SLOT(setSocialData(QString,QString,QString,QString,QString)));
    connect(webview,SIGNAL( gotAppleId(QString)),credentials,SLOT(get_user_by_email(QString)));
}
void navigation::setSocialData(QString fname,QString,QString,QString email,QString token){
    if(curr_user!=email){
        int id=credentials->check_user(email);
        if(id!=0){
            if(debg)qDebug()<<"RECOVERY FROM FACEBOOK";
            credentials->get_user_data(email,"",true);
        } else {
            if(debg)qDebug()<<"NEW USER:"<<email;
            credentials->add_new_user(email,email,fname,token,true);
            credentials->get_user_data();
        }
    }
}
void navigation::proceed_carousel(const QStringList& pars){
    qDebug()<<"params:"<<pars;
    QStringList params(pars);
    QString title=params.takeFirst();
    QString stime=params.takeFirst();
    QString netfile=params.takeFirst();
    QString f_oid=params.takeFirst();
    int title_id=params.takeFirst().toInt();
    if(carousel!=nullptr){
        if(carousel->title_id==title_id)
            return;
        else {
            qDebug()<<"Carousel destroy";
            carousel->close();
        }
    }
    carousel=new Carousel();
    carousel->hide();
    update();
    offset_ms=0;
    connect(carousel,SIGNAL(download(const QString&,const QString&)),media_queue,SLOT(push_url(const QString&,const QString&)),Qt::QueuedConnection);
    carousel->setObjectName("carousel");
    geom->resizer(carousel);
    carousel->set_fav_items_list(fav_items);
    carousel->set_fav_scenes_list(fav_scenes);
    qDebug()<<"CARL:"<<fav_items;
    carousel->set_sliders();
    if(stime!=""){
        offset_ms=carousel->hms2ms(stime);
        stime="";
    }
    carousel->set_params(title,title_id,netfile,offset_ms,credentials->player_offset,credentials->prospect_settings.player_enabled);
    connect(carousel,SIGNAL(go(QString,QStringList)),this,SLOT(go_to(QString,QStringList)));
    connect(carousel,SIGNAL(add_fav_item(int)),credentials,SLOT(add_fav_item(int)),Qt::QueuedConnection);
    connect(carousel,SIGNAL(add_fav_scene(int)),credentials,SLOT(add_fav_scene(int)),Qt::QueuedConnection);
    connect(carousel,SIGNAL(del_fav(const QString&,int)),credentials,SLOT(del_fav(const QString&,int)),Qt::QueuedConnection);
    connect(carousel,SIGNAL(can_process()),this,SLOT(show_carousel()));
    connect(carousel,SIGNAL(cant_process()),this,SLOT(rollin_carousel()));
    carousel->aimParser();
}
void navigation::rollin_carousel(){
    carousel->close();
}
bool navigation::proceed_media(const QString& on){
    if(!videos){
        videos=new favItems(wgen);
        connect(videos,SIGNAL(go(QString, QStringList)),this,SLOT(go_to(QString,QStringList)));
        geom->resizer(videos,true);
        videos->init();
    }
    if(on=="prospect"){
        if(credentials->context_id!=-1){
            resetContext();
        }
    }
    if(debg)qDebug()<<"FAV VIDEOS:"<<fav_videos->toVariantList();
    videos->reinit(on,curr_params,credentials->is_context());
    videos->activateContainer();
    //   if(srch.contains("search"))videos->setObjectName("S_"+on);
    return true;
}
void navigation::proceed_wgen(){
    wgen=new widgetGen();
    wgen->set_prospect_params(credentials->prospect_settings);
    wgen->setCoords(geom->m_geo, devicePixelRatio());
    connect( wgen, SIGNAL( download( const QString&, const QString&, int,bool ) ), media_queue,SLOT(push_url(const QString&,const QString&, int,bool)),Qt::QueuedConnection);
    connect( wgen, SIGNAL( toggle_fav_video(int,bool)),credentials,SLOT(toggle_fav_video(int,bool)),Qt::QueuedConnection);
    connect( wgen, SIGNAL( del_fav(const QString&, int)),credentials,SLOT(del_fav(const QString&,int)),Qt::QueuedConnection);
    connect( wgen, SIGNAL( go(const QString&,const QStringList&)),this,SLOT( go_to(const QString&,const QStringList&)));
}
void navigation::erase_module(){
    QWidget* s=static_cast<QWidget*>(sender());
    s->close();
}
void navigation::proceed_userprof(){
    // USER PROFILE
    shadow=false;
    if(userprof==nullptr){
        userprof=new userProf();
        userprof->setObjectName("userprof");
        connect(userprof,SIGNAL(go(QString,QStringList)),this,SLOT(go_to(QString,QStringList)),Qt::DirectConnection);
        geom->resizer(userprof);
    }
}
void navigation::exit_app(){
    QMessageBox::StandardButton reply;
    reply=QMessageBox::question(this,"","Leave now?");
    if(reply==QMessageBox::Yes){
        //   mthread.exit();
        while(count()>0){
            currentWidget()->setAttribute(Qt::WA_DeleteOnClose,true);
            currentWidget()->close();
            removeWidget(currentWidget());
            QApplication::processEvents();
        }
        mmenu->close();
        QApplication::quit();
    }
}
void navigation::proceed_profsettings(bool acc_recovery){
    if(profsettings==nullptr){
        profsettings=new profSettings();
        geom->resizer(profsettings);
        profsettings->setObjectName("profsettings");
        connect(profsettings,SIGNAL(save_profile()),this,SLOT(save_profile()));
        connect(profsettings,SIGNAL(upd_user(QString,QString,QString,QString)),this,SLOT(update_user(QString,QString,QString,QString)),Qt::QueuedConnection);
        connect(profsettings,SIGNAL(go(QString,QStringList)),this,SLOT(go_to(QString,QStringList)),Qt::QueuedConnection);
        connect(profsettings,SIGNAL(rec_user()),this,SLOT(recovery_user()),Qt::QueuedConnection);
        connect(credentials,SIGNAL(email_already_registered()),profsettings,SLOT(email_already_registered_info()),Qt::QueuedConnection);
        connect(profsettings,SIGNAL(check_email_exists(QString,bool)),this,SLOT(check_email_exists(QString,bool)),Qt::QueuedConnection);
        connect(profsettings,SIGNAL(remind_password(QString)),this,SLOT(remind_password(QString)));
        connect(credentials,SIGNAL(rec_acc_not_found()),profsettings,SLOT(wrong_email_message()),Qt::QueuedConnection);
        connect(profsettings,SIGNAL(logout()),this,SLOT(restart_clean()),Qt::QueuedConnection);
        //  connect(profsettings,SIGNAL(set_waiter()),this,SLOT(show_waiter()));
    }
    profsettings->prop_login=curr_user;
    profsettings->set_recovery_mode(acc_recovery);
    profsettings->setInitVars(credentials->social_type);
}
void navigation::remind_password(QString mail){
    QString res_email=serverRoot+"/api/checkSigned.php?remind_password=1&id="+mail;
    profsettings->sent=true;
    QNetworkAccessManager* mr=new QNetworkAccessManager(this);
    mr->get(QNetworkRequest(QUrl(res_email)));
    if(debg)qDebug()<<res_email;
    profsettings->confirmation_sent_info();
    profsettings->act="pass";
}
void navigation::update_loc(QString,QString lat,QString lon){
    credentials->lat=lat;
    credentials->lon=lon;
    if(!login_logged){
        login_logged=true;
        credentials->update_loc();
    }
}
void navigation::set_labels(){
    if(userprof!=nullptr){
        userprof->set_avatar();
        userprof->set_user(curr_nick);
        userprof->set_notif(media->count());
    }
    if(profsettings!=nullptr){
        profsettings->set_email(curr_user,false);
        profsettings->set_phone(curr_phone,false);
        profsettings->set_nick(curr_nick,false);
        profsettings->set_avatar(false);
    }
    if(videos!=nullptr){
        videos->set_nick(curr_nick);
    }
}
void navigation::init_menu(){
    mmenu=new menu(this);
    //  mmenu->setAttribute(Qt::WA_AlwaysStackOnTop,true);
    QRect r=QApplication::primaryScreen()->geometry();
    mmenu->setFixedSize(r.width()*0.6,r.height()*0.9);
    mmenu->setGeometry(width()*0.4,r.height()*0.05,r.width()*0.6,r.height()*0.9);
    mmenu->getGeo();
    connect(mmenu,SIGNAL(go(QString,QStringList)),this,SLOT(go_to(QString,QStringList)));
    mmenu->setContext(credentials->is_context());
}
bool navigation::menuOpened(){
    if(mmenu==nullptr)return false;
    return mmenu->isVisible();
}
void navigation::disableCurrMenuEntry(){
    if(mmenu!=nullptr && count()>0){
        mmenu->deactivate(currentWidget()->objectName(),curr_params);
    }
}
void navigation::showMenu(){
    if(mmenu==nullptr || count()==0)return;
    mmenu->setVisible(true);
    mmenu->raise();
    wgen->menu_opened=true;
    update();
}

void navigation::hideMenu(){
    if(!mmenu)return;
    mmenu->hide();
    wgen->menu_opened=false;
}
void navigation::navigate_upon_user_data(){
    get_current_user_data();
    creds_ok=!curr_user.contains("GUEST");
    proceed_wgen();
    updateFavs();
    go_to("sign_in");
    sign_in->toggle_auth(!creds_ok);
    if(creds_ok){
        go_to("userprof",{});
    }
}
void navigation::set_credentials(){
    if(credentials==nullptr){
        credentials=new Credentials();
        connect(credentials,SIGNAL(got_user_data()),this,SLOT(navigate_upon_user_data()),Qt::QueuedConnection);
        connect(credentials,SIGNAL(got_favs()),this,SLOT(updateFavs()),Qt::QueuedConnection);
        connect(credentials,SIGNAL(inform(QString)),this,SLOT(info(QString)),Qt::QueuedConnection);
        connect(credentials,SIGNAL(finished()),this,SLOT(terminateCreds()),Qt::QueuedConnection);
    }
    credentials->process();
}
void navigation::change_geo(const QRect& r){
    resize(r.size());
    updateGeometry();
    setGeometry(r);
    setFixedSize(r.size());
    update();
}
navigation::navigation(QWidget *parent) : QStackedWidget(parent),splash(true),menuopened(false),fresh_run(true),
    rwidget(new QWidget){
    //  setWindowFlag(Qt::FramelessWindowHint);
    //  setWindowState(Qt::WindowFullScreen);
    //   setGeometry(QApplication::primaryScreen()->availableVirtualGeometry());
    gradient.setCoordinateMode(QLinearGradient::ObjectMode);
    gradient.setStart(0,0);
    gradient.setFinalStop(QPoint(1,0));// diagonal gradient from top-left to bottom-right
    gradient.setColorAt(0, QColor(59,34,100));
    gradient.setColorAt(0.5, QColor(150,2,170));
    gradient.setColorAt(1, QColor(59,34,100));
    gradient2.setCoordinateMode(QLinearGradient::ObjectMode);
    gradient2.setStart(0,0);
    gradient2.setFinalStop(QPoint(0,1));
    gradient2.setColorAt(0, QColor(0,0,0,80));
    gradient2.setColorAt(0.5, QColor(0,0,0,60));
    gradient2.setColorAt(1, QColor(0,0,0,80));
    debg=false;;
    media_queue=new downloader(this);
    setObjectName("core");
    geom=new geofit(this);
    connect(qApp->primaryScreen(),SIGNAL( availableGeometryChanged(const QRect& )),this,SLOT(change_geo(const QRect& )));
    //  connect(media_queue,SIGNAL(need_revalidate_widget(int)),this,SLOT(must_revalidate_widget(int)),Qt::QueuedConnection);
    setAttribute(Qt::WA_TransparentForMouseEvents,true);
    rwidget->setGeometry(0,0,1,1);
    rwidget->setAttribute(Qt::WA_AlwaysStackOnTop);
    rwidget->hide();

#if defined (Q_OS_ANDROID)
    uperm=new userPermissions(this);
    uperm->requestPermission();
#endif
    init_app();
}
void navigation::must_revalidate_widget(int i){
    if(count()==0) return;
    if(currentWidget()==videos)
        wgen->item(i)->revalidate();
}
void navigation::activate_current(int i){
    if(count()==0)return;
    if(debg)qDebug()<<"Widget removed now is:"<<currentIndex()<<currentWidget()<<i;
    currentWidget()->activateWindow();
    currentWidget()->raise();
}
void navigation::paintEvent(QPaintEvent *){
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
    opac+=.25*(shadow && opac<1.0)-.25*(!shadow && opac>0.0);
    p.setOpacity(opac);
    p.drawRoundedRect(shadow_geo,5,5);
    if(opac!=0 && opac!=1){
        if(opac>=1.0)opac=1;
        if(opac<=0.0)opac=0.0;
        update();
    }
}
/*
void navigation::paintEvent(QPaintEvent *){
    QRect r=QApplication::primaryScreen()->availableVirtualGeometry();
    QPixmap p;

QPainter painter(this);
painter.setRenderHints(QPainter::Antialiasing);
painter.drawPixmap(r,bimg);
opac+=.25*(shadow && opac<1.0)-.25*(!shadow && opac>0.0);
painter.setOpacity(opac);
painter.setBrush(gradient2);
painter.drawRoundedRect(shadow_geo,5,5);
painter.end();
if(opac!=0 && opac!=1){
    if(opac>=1.0)opac=1;
    if(opac<=0.0)opac=0.0;
    update();
}
}
*/
    void navigation::init_app(){
    showFullScreen();
    update();
    geom->resizer(this);
    int marg=15*width()/400;
    qDebug()<<"COORDS:"<<width()<<height();

    shadow_geo.setRect(marg,marg,width()-2*marg,height()-2*marg);
    curr_user="";
    s_curr_id="";
    curr_nick="";
    curr_phone="";
    offline_mode=false;
    updated=false;
    guest_login=true;
    path_once=true;
    carousel_title="!?!?*&";
    prevent_click=false;
    login_logged=false;
    showed=false;
    lock=false;
#if defined (Q_OS_ANDROID) || defined (Q_OS_IOS)
    proceed_gps();
#endif
    set_credentials();
    init_menu();
}
void navigation::terminateCreds(){
    if(debg)qDebug()<<"TERMINATE CREDS";
    if(debg)qDebug()<<"thread-del";
}
void navigation::info(QString inf){
    QMessageBox::information(this,"",inf);
}
navigation::~navigation(){
    credentials->deleteLater();
    wgen->deleteLater();
    if(debg)qDebug()<<"WAND:NAVI:~navigation";
    qApp->quit();
}



