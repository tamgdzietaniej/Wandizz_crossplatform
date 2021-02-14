
#include "navigation.h"

void navigation::restart_clean(){
    //   go_to("loginFB",{"logout"});
    QFile(f_app_hash).remove();
    QFile(f_user_avatar).remove();
    creds_ok=false;
    updated=false;
    shadow=false;
    prospect_shown=false;
    int wc=count();
    for(int i=0;i<wc;i++){
        if(widget(i)!=sign_in){
            widget(i)->deleteLater();
        }
    }
    init_app();
    //   go_to("sign_in");
}
void navigation::check_email_exists(QString email,bool){
    profsettings->must_chk_email=false;
    if(email==credentials->curr_user){
        if(debg)qDebug()<<"email=curr_user"<<credentials->curr_user;
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
    if(debg)qDebug()<<"SIGN IN";
    if(sign_in==nullptr){
        sign_in=new SignIn();
        sign_in->setObjectName("sign_in");
        geom.resizer(sign_in);
        connect(sign_in,SIGNAL(go(QString,QStringList)),this,SLOT(go_to(QString,QStringList)),Qt::QueuedConnection);

    }
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
bool navigation::isCWName(QString n){
    if(!count())return false;
    return currentWidget()->objectName()==n;
}
// mrk::go_to
void navigation::go_to(QString to,QStringList params){
    qDebug()<<"TO:"<<to<<params<<prospect_shown;
    curr_params=params;
    shadow=(to!="sign_in" && to!="userprof");
    update();
    /* LAUNCH MENU */
    //   if(menu_opened && senatyder()!=mmenu){
    //       qDebug()<<"MENU VISIBLE:"<<menu_opened;
    //      hideMenu();
    //      return;
    //  }
    if(to=="menu"){  // mmenu
        if(QApplication::inputMethod()->isVisible())
            QApplication::inputMethod()->hide();
        if(menu_opened)hideMenu();
        else
            showMenu();
        return;
    }
    if(menu_opened)hideMenu();
    if(to=="menu_hide")
        return;
    /* SHARE */
    if(to=="share"){ //share
        share();
        return;
    }
    if(to=="m_share"){
        go_to("share");
        return;
    }
    if(to=="legal"){
        QMessageBox::information(this,"Legal","Legal info soon");
        return;
    }
    if(to=="security"){
        QMessageBox::information(this,"Security","");
        return;
    }
    //   qDebug()<<"Pars:"<<params<<curr_params;
    if(count()>0)
        if(to==currentWidget()->objectName() && !params.contains("back") && params==lparams){
            qDebug()<<"Already here";
            return;
        }
    if(to=="web"){
        QDesktopServices::openUrl(QUrl(params.takeFirst()));
        return;
    } else if(to=="exit_app"){
        exit_app();
        return;
    }
    /* RE-USABLE STUFF */
    /* " ACTION:Back  */
    if(!wgen->isLocked()){
        wgen->clean();
    }
    if(to.contains("back")){
        removeWidget(currentWidget());
        if(currentWidget()==videos){
            wgen->unlock();
            update();
        }
        if(count()>0)
            go_to(currentWidget()->objectName(),{"last","back"});
        else go_to("userprof",{});
        return;
    }
    /* FULL WANDIZZ */
    if(to=="full"){
        credentials->context="";
        credentials->context_id=-1;
        updateFavs();
        go_to("userprof");
        return;
    }
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
        userprof->corr();
        // proper destination -> profile [home] screen
    }
    /* ACCOUNT RECOVERY */
    if(to=="profset"){  //profile page
        proceed_profsettings(params.contains("accrec"));
        show_module(profsettings);
    } else
        /* SYNC-SCREEN */
        if(to=="sync"){  // videorec
        proceed_sync();
        show_module(sync);
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
        show_module(videos);
        videos->create_videos_list(credentials->timeline_count_by_title_id,media);
        return;
    }
    else
        if(to=="prospect"){
        prospect_shown=true;
        proceed_media("prospect");
        show_module(videos);
        videos->create_prospect_list(credentials->timeline_count_by_title_id,media);
        return;
    }
    else
        if(to=="carousel"){
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
            show_module(videos);
            videos->create_scenes_list(credentials->timeline_count_by_title_id,fav_scenes);
            return;
        }
    } else
        if(to=="fav_items"){
        if(fav_items->count() || show_even_if_zero){
            proceed_media("fav_items");
            show_module(videos);
            videos->create_items_list(fav_items);
            return;
        }
    } else
        if(to=="fav_videos"){
        if(fav_videos->count() || show_even_if_zero){
            proceed_media("fav_videos");
            show_module(videos);
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
    if(to=="sign_in"){
        if(webview!=nullptr){
            removeWidget(webview);
            qDebug()<<"Deleting webview";
            webview->deleteLater();
        }
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
    lparams=curr_params;
    if(to!=nullptr)
    {
        addWidget(to);
        //qDebug()<<"SHOW MODUULE:"<<count();
        //    if(count()==2 && to->objectName()!="prospect" && to->objectName()!="profset"){
        if(!prospect_shown && to!=profsettings && to!=sign_in && to!=webview){
            go_to("prospect",{"search"});
            return;
        }
        setCurrentWidget(to);
        currentWidget()->raise();
        currentWidget()->showFullScreen();
        if(currentWidget()!=videos)
            currentWidget()->activateWindow();
        else
        {
            if(wgen){
                qDebug()<<"WGEN:"<<wgen;
                wgen->activateWindow();
            }
        }
    }
    disableCurrMenuEntry();
}
void navigation::mousePressEvent(QMouseEvent* e){
    if(mmenu->isVisible() || menu_opened)
        hideMenu();
}
void navigation::resetContext(){
    credentials->context="";
    credentials->context_id=-1;
    updateFavs();
}
void navigation::updateFavs(){
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
            //  else qDebug()<<"!!! set_favs_cnt:"<<wgen->getContext();
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
        geom.resizer(sync);
        sync->initClasses();
        sync->screenw=geom.nw;
        sync->screenh=geom.nh;
        connect(sync,SIGNAL(go(QString,QStringList)),this,SLOT(go_to(QString,QStringList)));
    }
    sync->curr_user=credentials->curr_user;
    sync->curr_id=QString::number(credentials->curr_id);
}
void navigation::proceed_webview(){
    if(webview!=nullptr){
        webview->close();
    }
    webview=new socialLogin();
    webview->setObjectName("webview");
    geom.resizer(webview);
    webview->createWebView();
    webview->setAttribute(Qt::WA_AlwaysStackOnTop);
    connect(webview,SIGNAL(go(QString,QStringList)),this,SLOT(go_to(QString,QStringList)));
    connect(webview,SIGNAL(gotSocialData(QString,QString,QString,QString,QString)),this,SLOT(setSocialData(QString,QString,QString,QString,QString)));
    connect(webview,SIGNAL( gotAppleId(QString)),credentials,SLOT(get_user_by_email(QString)));
    webview->show();
    sign_in->toggle_auth(false);
}
void navigation::setSocialData(QString fname,QString,QString,QString email,QString token){
    if(credentials->curr_user!=email){
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
    if(debg)qDebug()<<"params:"<<pars;
    QStringList params(pars);
    QString title=params.takeFirst();
    QString stime=params.takeFirst();
    QString netfile=params.takeFirst();
    QString f_oid=params.takeFirst();
    int title_id=params.takeFirst().toInt();
    if(carousel!=nullptr){
        if(carousel->title_id==title_id && !fav_modified)
            return;
        else {
            qDebug()<<"Carousel destroy";
            carousel->close();
        }
    }
    carousel=new Carousel();
    carousel->hide();
    fav_modified=false;
    update();
    offset_ms=0;
    connect(carousel,SIGNAL(download(const QString&,const QString&)),media_queue,SLOT(push_url(const QString&,const QString&)),Qt::QueuedConnection);
    carousel->setObjectName("carousel");
    geom.resizer(carousel);
    carousel->set_fav_items_list(fav_items);
    carousel->set_fav_scenes_list(fav_scenes);
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
        geom.resizer(videos,true);
        videos->init();
        rwidget->show();
        rwidget->hide();
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
    if(wgen!=nullptr){
        qDebug()<<"WGEN:OLD";
        return;
    }
    wgen=new widgetGen();
    wgen->set_prospect_params(credentials->prospect_settings);
    wgen->setCoords(geom.m_geo, devicePixelRatio());
    connect( wgen, SIGNAL( download( const QString, const QString ) ), media_queue,SLOT(push_url(const QString,const QString)),Qt::QueuedConnection);
    connect( wgen, SIGNAL( toggle_fav_video(int,bool)),credentials,SLOT(toggle_fav_video(int,bool)),Qt::QueuedConnection);
    connect( wgen, SIGNAL( del_fav(const QString&, int)),credentials,SLOT(del_fav(const QString&,int)),Qt::QueuedConnection);
    connect( wgen, SIGNAL( del_fav(const QString&, int)),this,SLOT(setFavModified()),Qt::QueuedConnection);
    connect( wgen, SIGNAL( go(const QString&,const QStringList&)),this,SLOT( go_to(const QString&,const QStringList&)));
}
void navigation::setFavModified(){
    fav_modified=true;
}
void navigation::erase_module(){
    QWidget* s=static_cast<QWidget*>(sender());
    s->close();
}
void navigation::proceed_userprof(){
    // USER PROFILE
    if(userprof==nullptr){
        userprof=new userProf();
        userprof->setObjectName("userprof");
        connect(userprof,SIGNAL(go(QString,QStringList)),this,SLOT(go_to(QString,QStringList)),Qt::DirectConnection);
        geom.resizer(userprof);
    }
    set_labels();
}
void navigation::exit_app(){
    QMessageBox::StandardButton reply;
    reply=QMessageBox::question(this,"","Leave now?");
    if(reply==QMessageBox::Yes){
        setUpdatesEnabled(false);
        while(count()>0){
            currentWidget()->setAttribute(Qt::WA_DeleteOnClose,true);
            currentWidget()->close();
            removeWidget(currentWidget());
            QApplication::processEvents();
        }
        mmenu->close();
        credentials->deleteLater();
        qDebug()<<qApp->allWidgets()<<qApp->allWindows()<<qApp->findChildren<QImage*>();

        QApplication::quit();
    }
}
void navigation::proceed_profsettings(bool acc_recovery){
    if(profsettings==nullptr){
        profsettings=new profSettings();
        geom.resizer(profsettings);
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
    set_labels();
    profsettings->prop_login=credentials->curr_user;
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
        userprof->set_user(credentials->curr_nick);
        userprof->set_notif(media->count());
        userprof->corr();
    }
    if(profsettings!=nullptr){
        profsettings->set_email(credentials->curr_user,false);
        profsettings->set_phone(credentials->curr_phone,false);
        profsettings->set_nick(credentials->curr_nick,false);
        profsettings->set_avatar(false);
    }
}
void navigation::init_menu(){
    mmenu=new menu(this);
    //  mmenu->setAttribute(Qt::WA_AlwaysStackOnTop,true);
    mmenu->setFixedSize(mg.width()*0.5,mg.height()-40);
    mmenu->setGeometry(width()*0.5,20,mg.width()*0.5,mg.height()-20);
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
    menu_opened=true;
}

void navigation::hideMenu(){
    if(!mmenu)return;
    mmenu->setVisible(false);
    menu_opened=false;
    if(count()>0)
        currentWidget()->activateWindow();
}
void navigation::navigate_upon_user_data(){
    creds_ok=!credentials->curr_user.contains("GUEST");
    proceed_wgen();
    updateFavs();
    go_to("sign_in");
    qDebug()<<"AUTH:"<<creds_ok;
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
navigation::navigation(QWidget *parent) : QStackedWidget(parent),splash(true),menuopened(false),fresh_run(true),fav_modified(false),
    prospect_shown(false),rwidget(new QWidget){
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
    debg=false;
    menu_opened=false;
    media_queue=new downloader(this);
    setObjectName("core");
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
    if(wgen)
        if(wgen->top_menu_switcher)
            qDebug()<<"NAVI:PAINTEEVENT"<<shadow<<wgen->top_menu_switcher->edit->text();
    int marg=15*width()/400;
    shadow_geo.setRect(marg,geom.gmarg+marg,geom.m_geo.width()-2*marg,geom.m_geo.height()-2*marg);
    QPainter p(this);
    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColor(0,0,0,0));
    p.setRenderHints(QPainter::HighQualityAntialiasing);
    p.setPen(pen);
    p.setBrush(gradient);
    p.fillRect(geom.a_geo,p.brush());
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
    shadow=false;
    showFullScreen();
    QRect m1=geom.m_geo;
    QRect m2=geom.a_geo;
    mg=m2;
    if(m1.height()>m2.height())mg=m1;
    setGeometry(mg);
    setFixedSize(mg.size());
    update();
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
    if(credentials)credentials->deleteLater();
    if(wgen)wgen->deleteLater();
    if(debg)qDebug()<<"WAND:NAVI:~navigation";
    qApp->quit();
}



