#include "navigation.h"

void navigation::set_titles(QJsonArray t){
    titles=t;
    fav_titles=credentials->jfm.array();
    titles_cnt=titles.count();
    gone_to="";
}
void navigation::set_favs(){
    set_favs("");
}
void navigation::set_favs(QString){
    cfi=credentials->jfi.array().count();
    cfs=credentials->jfs.array().count();
    cfm=credentials->jfm.array().count();
    if(currentWidget()==carousel)fav_opened=videos->objectName();
    if(currentWidget()!=carousel && carousel!=nullptr){
        carousel->set_fav_items_list(credentials->jfi);
        carousel->set_fav_scenes_list(credentials->jfs);
        carousel->check_mw_fav();
    }
    fav_titles=credentials->jfm.array();
    set_labels();
}
void navigation::restart_clean(){
    go_to("loginFB",{"logout"});
    QFile(f_app_hash).remove();
    QFile(f_user_avatar).remove();
    creds_ok=false;
    updated=false;
    shadow=false;
    /*
     *
     *     if(credentials->recov){
        credentials->recov=false;
        int cc=count();
        for(int i=0;i<cc;i++){
            setCurrentIndex(i);
            if(debg)qDebug()<<"currentindex"<<i<<currentWidget();
            if(currentWidget()!=sign_in){
                QWidget *w=currentWidget();
                removeWidget(currentWidget());
                if(debg)qDebug()<<"REMONING:"<<w;
                w->deleteLater();
            }
        }
    } else
     *
     * */
    int wc=count();
    for(int i=0;i<wc;i++){
        if(widget(i)!=sign_in){
            widget(i)->deleteLater();
        }
    }
    init_app();
}
void navigation::get_current_user_data(){
    offline_mode=credentials->offline_m;
    if(!offline){
        s_curr_id=QString::number(credentials->curr_id);
        curr_user=credentials->curr_user;
        curr_nick=credentials->curr_nick;
        curr_phone=credentials->curr_phone;
        get_favs(true);
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
void navigation::check_email_exists(QString email,bool rec){
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

void navigation::show_fav_videos(){
    videos->create_fvideos_list(cfm, credentials->timeline_count_by_title_id,fav_titles,credentials->jfm.array());
}
void navigation::show_videos(){
    prepare_favs("videos");
}
void navigation::prepare_favs(QString type){
    QString url,media_file,titl,file_name;
    QJsonObject obj;
    FavsTable table;
    favs_table.clear();
    int ev_cnt,i,id,cnt;
    cnt=titles.count();
    videos->set_fav_videos_list(credentials->fvl);
    for(i=0;i<cnt;i++){
        id=titles.at(i).toObject().value(QStringLiteral("id")).toInt();
        ev_cnt=credentials->timeline_count_by_title_id[id];
        titl=titles.at(i).toObject().value(QStringLiteral("title")).toString();
        media_file=titles.at(i).toObject().value(QStringLiteral("filename")).toString();
        table={i,id,QString::number(ev_cnt),titl,url,media_file,""};
        favs_table.append(table);
    }
    videos->create_videos_list(favs_table);
}
void navigation::show_fav_scenes(){
    videos->create_scenes_list(cfs,credentials->jfs,titles);
}
void navigation::show_fav_items(){
    videos->create_items_list(cfi,credentials->jfi);
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
            if(debg)qDebug()<<"Unlock:"<<currentWidget();
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
// mrk::go_to
void navigation::go_to(QString to,QStringList params){
    if(gone_to==to){
        if(debg)qDebug()<<"!!!!! PRESSED AGAIN!!!!!!";
        return;
    }
    if(debg)qDebug()<<"GOTO:"<<to;
    if(to=="loginFB"){
        proceed_webview();
        show_module(webview);
        webview->setUrl(loginFBurl+"?action="+params.takeFirst());
        return;
    }
    else if(to=="loginTT"){
        proceed_webview();
        show_module(webview);
        webview->setUrl(loginTWurl+"?action="+params.takeFirst());
        return;
    }

    if(mmenu->isVisible()){
        if(debg)qDebug()<<"HIDING MENU"<<sender()<<to;
        mmenu->hide();
        if(to=="back")
            return;
    }
    if(count()>0)
    if(sender()==mmenu)currentWidget()->setEnabled(true);
    if(to.contains("back")){
        if(count()>1){
            QPointer<QWidget> dw=currentWidget();
            removeWidget(currentWidget());
            if(dw!=carousel){
                dw->setAttribute(Qt::WA_DeleteOnClose,true);
                dw->close();
            } else {
                carousel->perform_close();
            }
            //    if(!(sender()==webview && dw!=webview)){
            //    }
            QString on=currentWidget()->objectName();
            shadow=true;
            if(on=="userprof" || on=="sign_in"){
                shadow=false;
                set_labels();
            }
            unlock();
            if(currentWidget()==videos){
                to=videos->on;
            }
        }
    }
    if(to=="home"){
        currentWidget()->close();
        go_to("userprof");
        return;
    }
    if(to=="share"){
        share();
        return;
    }
    if(to=="s_fav_items"){
        to="fav_items";
        params.append("search");
    }
    if(to=="s_videos"){
        to="videos";
        params.append("search");
    }

    if(to=="userprof"){
        proceed_userprof();
        userprof->set_equal();
        shadow=false;
        show_module(userprof);
    } else
        if(to=="profset"){
            proceed_profsettings();
            shadow=true;
            show_module(profsettings);
        } else
            if(to=="accrec"){
                proceed_profsettings();
                shadow=true;
                profsettings->set_recovery_mode(true);
                show_module(profsettings);
            } else
                if(to=="sync"){
                    proceed_sync();
                    shadow=true;
                    show_module(sync);
                } else
                    if(to=="menu"){
                        emit show_menu(QPoint());
                    } else
                        if(to=="favs"){
                            if(cfm>0 || ((cfi==0) && cfs==0)){
                                to="fav_videos";
                            } else if(cfi>0){
                                to="fav_items";
                            } else if(cfs>0){
                                to="fav_scenes";
                            }
                        }
    if(to=="videos"){
        proceed_media("videos",params);
        shadow=true;
        show_module(videos);
        show_videos();
    }
    else
        if(to=="carousel"){
            shadow=true;
            proceed_carousel(params);
            show_module(carousel);
        } else
            if(to=="fav_scenes"){
                if(cfs>0 || show_even_if_zero){
                    proceed_media("fav_scenes");
                    shadow=true;
                    show_module(videos);
                    show_fav_scenes();

                }
            } else
                if(to=="fav_items"){
                    if(cfi>0 || show_even_if_zero){
                        proceed_media("fav_items",params);
                        shadow=true;
                        show_module(videos);
                        show_fav_items();
                    }
                } else
                    if(to=="fav_videos"){
                        if(cfm>0 || show_even_if_zero){
                            proceed_media("fav_videos");
                            shadow=true;
                            show_module(videos);
                            show_fav_videos();
                        }
                    } else
                        if(to=="sign_in"){
                            shadow=false;
                            proceed_sign_in();
                            show_module(sign_in);
#if defined(Q_OS_ANDROID)
                            if(splash)
                                QtAndroid::hideSplashScreen();
                            splash=false;
#endif
                        }
                        else
                            if(to=="web"){
                                currentWidget()->setEnabled(true);
                                QDesktopServices::openUrl(QUrl(params.takeFirst()));
                                show_module();
                            }
    gone_to=currentWidget()->objectName();
    if(count()>0)qDebug()<<"GONE TO:"<<gone_to;

}
// mrk:show_module
void navigation::show_module(QWidget* to){
    if(to!=nullptr)
    {
        to->setEnabled(true);
        addWidget(to);
        setCurrentWidget(to);
        if(debg)qDebug()<<"Currentwidget:"<<currentWidget();
        //     currentWidget()->raise();
        //    currentWidget()->showFullScreen();
        //     currentWidget()->activateWindow();
        //     update();
    }
    unlock();
}
void navigation::proceed_sync(){
    if(sync==nullptr){
        sync=new SyncScreen();
        sync->setObjectName("sync");
        geom->resizer(sync);
        sync->initClasses();
        sync->screenw=geom->nw;
        sync->screenh=geom->nh;
        connect(sync,SIGNAL(go_carousel(const QString,QString)),this,SLOT(go_carousel(const QString, QString)));
        connect(sync,SIGNAL(show_menu(QPoint)),this,SLOT(show_menu(QPoint)));
        connect(sync,SIGNAL(go_web(QString)),this,SLOT(show_web(QString)));
        connect(sync,SIGNAL(go(QString)),this,SLOT(go_to(QString)));
    }
    sync->curr_user=curr_user;
    sync->curr_id=s_curr_id;
}
void navigation::proceed_webview(){
    if(webview!=nullptr){
        webview->deleteLater();
    }
    webview=new socialLogin();
    webview->setObjectName("webview");
    geom->resizer(webview);
    webview->createWebView();
    connect(webview,SIGNAL(go(QString)),this,SLOT(go_to(QString)));
    connect(webview,SIGNAL(gotSocialData(QString,QString,QString,QString,QString)),this,SLOT(setSocialData(QString,QString,QString,QString,QString)));
}
void navigation::setSocialData(QString fname,QString lname,QString gender,QString email,QString token){
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
void navigation::proceed_carousel(QStringList params)
{// title, QString time, QString});){
    if(carousel!=nullptr)
    {
        carousel->deleteLater();
    }
    title=params.takeFirst();
    stime=params.takeFirst();
    netfile=params.takeFirst();
    QString f_oid=params.takeFirst();
    carousel=new Carousel();
    connect(carousel,SIGNAL(download(QString,QString)),media_queue,SLOT(push_url(QString,QString)),Qt::QueuedConnection);
    carousel->setObjectName("carousel");
    geom->resizer(carousel);
    carousel->set_fav_items_list(credentials->jfi);
    carousel->set_fav_scenes_list(credentials->jfs);
    carousel->set_sliders();
    if(stime!=""){
        offset_ms=carousel->hms2ms(stime);
        stime="";
        carousel->set_params(title,netfile,offset_ms,credentials->player_offset,credentials->player_enabled);
    }
    connect(carousel,SIGNAL(go(QString,QStringList)),this,SLOT(go_to(QString,QStringList)));
    connect(carousel,SIGNAL(add_fav_item(int,QString,QString,QString,QString,int)),credentials,SLOT(add_fav_item(int,QString,QString,QString,QString,int)),Qt::QueuedConnection);
    connect(carousel,SIGNAL(add_fav_scene(int,QString,QString,QString,int)),credentials,SLOT(add_fav_scene(int,QString,QString,QString,int)),Qt::QueuedConnection);
    connect(carousel,SIGNAL(delete_item(int,QString)),this,SLOT(delete_item(int,QString)),Qt::QueuedConnection);
    connect(carousel,SIGNAL(show_menu(QPoint)),this,SLOT(show_menu(QPoint)));
    carousel->aimParser();
}
void navigation::get_favs(bool write){
    credentials->get_favs(write);
}
bool navigation::proceed_media(QString on,QStringList srch){
    if(videos!=nullptr){
        if(currentWidget()!=videos){
            setCurrentWidget(videos);
        }
        videos->cleanUp(videos->objectName());
    }
    else
    {
        videos=new favItems();
        connect(videos,SIGNAL(download(QString,QString)),media_queue,SLOT(push_url(QString,QString)),Qt::QueuedConnection);
        connect(videos,SIGNAL(go(QString, QStringList)),this,SLOT(go_to(QString,QStringList)));
        connect(videos,SIGNAL(show_menu(QPoint)),this,SLOT(show_menu(QPoint)));
        connect(videos,SIGNAL(add_fav_videos(int)),credentials,SLOT(add_fav_videos(int)),Qt::DirectConnection);
        connect(videos,SIGNAL(delete_item(int,QString)),this,SLOT(delete_item(int,QString)));
        geom->resizer(videos,true);
        videos->init();
    }
    videos->reinit(on);
    videos->set_fav_videos_list(credentials->fvl);
    if(srch.contains("search"))videos->enableSearch();
    return true;
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
        connect(userprof,SIGNAL(exit_app()),this,SLOT(exit_app()));
        connect(userprof,SIGNAL(show_menu(QPoint)),this,SLOT(show_menu(QPoint)));
        geom->resizer(userprof);
    }
    set_labels();
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
        mmenu->setAttribute(Qt::WA_DeleteOnClose,true);
        mmenu->close();
        QApplication::quit();
    }
}
void navigation::show_waiter(){
    waiter->setAttribute(Qt::WA_AlwaysStackOnTop);
    waiter->showFullScreen();
    waiter->raise();
}
void navigation::hide_waiter(){
    waiter->setAttribute(Qt::WA_AlwaysStackOnTop,false);
    waiter->hide();
    waiter->lower();
}
void navigation::proceed_profsettings(bool acc_recovery){
    if(profsettings==nullptr){
        profsettings=new profSettings();
        geom->resizer(profsettings);
        profsettings->setObjectName("profsettings");
        connect(profsettings,SIGNAL(save_profile()),this,SLOT(save_profile()));
        connect(profsettings,SIGNAL(upd_user(QString,QString,QString,QString)),this,SLOT(update_user(QString,QString,QString,QString)),Qt::QueuedConnection);
        connect(profsettings,SIGNAL(go(QString)),this,SLOT(go_to(QString)),Qt::QueuedConnection);
        connect(profsettings,SIGNAL(rec_user()),this,SLOT(recovery_user()),Qt::QueuedConnection);
        connect(credentials,SIGNAL(email_already_registered()),profsettings,SLOT(email_already_registered_info()),Qt::QueuedConnection);
        connect(profsettings,SIGNAL(check_email_exists(QString,bool)),this,SLOT(check_email_exists(QString,bool)),Qt::QueuedConnection);
        connect(profsettings,SIGNAL(remind_password(QString)),this,SLOT(remind_password(QString)));
        connect(credentials,SIGNAL(rec_acc_not_found()),profsettings,SLOT(wrong_email_message()),Qt::QueuedConnection);
        connect(profsettings,SIGNAL(show_menu(QPoint)),this,SLOT(show_menu(QPoint)));
        connect(profsettings,SIGNAL(logout()),this,SLOT(restart_clean()),Qt::QueuedConnection);
        //  connect(profsettings,SIGNAL(set_waiter()),this,SLOT(show_waiter()));
    }
    profsettings->prop_login=curr_user;
    profsettings->set_recovery_mode(acc_recovery);
    set_labels();
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
void navigation::delete_item(int i_db, QString ri){
    if(videos!=nullptr && (currentWidget()!=videos || (currentWidget()==videos && videos->on!=ri))){
        videos->removeOffscreen(ri,i_db);
    }
    credentials->remove_from_favs(i_db,ri,sender()->objectName());
    if(debg)qDebug()<<ri<<sender()->objectName();
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
        userprof->set_notif(titles_cnt);
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
    mmenu->setAttribute(Qt::WA_AlwaysStackOnTop,true);
    mmenu->setFixedSize(width()*0.6,height()*0.9);
    mmenu->setGeometry(width()*0.4,height()*0.05,width()*0.6,height()*0.9);
    connect(mmenu,SIGNAL(go(QString)),this,SLOT(go_to(QString)));
    connect(mmenu,SIGNAL(logout()),qApp,SLOT(quit()));
    connect(mmenu,SIGNAL(exit_app()),this,SLOT(exit_app()));
    connect(mmenu,SIGNAL(menu_on(bool)),this,SLOT(toggle_input(bool)));
}
void navigation::toggle_input(bool t){
    mmenu->setVisible(!t);
    currentWidget()->setFocus();
}
void navigation::show_menu(QPoint r){
    mmenu->set_opt_geo(r);
    mmenu->setVisible(true);
    mmenu->enable();
    if(cfi==0 && !show_even_if_zero)mmenu->disable("fav_items");
    if(cfs==0 && !show_even_if_zero)mmenu->disable("fav_scenes");
    if(cfm==0 && !show_even_if_zero)mmenu->disable("fav_videos");
    mmenu->deactivate(currentWidget()->objectName());
    //   mmenu->activateWindow();
}
void navigation::navigate_upon_user_data(){
    //   if(profsettings!=nullptr){
    //      profsettings->on_b_back_clicked();
    //   }
    get_current_user_data();
    titles_cnt=titles.count();
    creds_ok=!curr_user.contains("GUEST");
    sign_in->toggle_auth(true);
    if(creds_ok){
        if(currentWidget()!=userprof) go_to("userprof");
    }


    emit go_show_rest();
}
void navigation::show_rest(){
    //show_fav_items(false);
    //show_fav_scenes(false);
    // show_fav_videos(false);
}
void navigation::set_credentials(){
    if(credentials==nullptr){
        credentials=new Credentials();
        connect(credentials,SIGNAL(got_user_data()),this,SLOT(navigate_upon_user_data()),Qt::QueuedConnection);
        connect(this,SIGNAL(go_show_rest()),this,SLOT(show_rest()));
        connect(credentials,SIGNAL(got_favs()),this,SLOT(set_favs()),Qt::QueuedConnection);
        connect(credentials,SIGNAL(got_titles(QJsonArray)),this,SLOT(set_titles(QJsonArray)),Qt::QueuedConnection);
        connect(credentials,SIGNAL(inform(QString)),this,SLOT(info(QString)),Qt::QueuedConnection);
        connect(credentials,SIGNAL(go_get_user_data()),credentials,SLOT(get_user_data()),Qt::QueuedConnection);
        connect(credentials,SIGNAL(finished()),this,SLOT(terminateCreds()),Qt::QueuedConnection);
    }
    //    credentials->moveToThread(&mthread);
    //    connect(&mthread,SIGNAL(started()),credentials,SLOT(process()));
    //    mthread.start();
    credentials->process();
}
void navigation::change_geo(const QRect& r){
    resize(r.size());
    updateGeometry();
    setGeometry(r);
    setFixedSize(r.size());
    update();
}
navigation::navigation(QWidget *parent) : QStackedWidget(parent),splash(true),menuopened(false){
    /*
    if(debg)qDebug()<<"WINDOW SIZE:"<<geometry();
    //  showFullScreen();
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowState(Qt::WindowFullScreen);
    setGeometry(QApplication::primaryScreen()->availableVirtualGeometry());
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
  //  if(debg)qDebug()<<"ANDROID VER:"<<QtAndroid::androidSdkVersion();
    QPainter painter;
    bimg=QPixmap(rect().size());
    bimg.fill(QColor(0,0,0,0));
    QBrush brush(gradient);
    painter.begin(&bimg);
    painter.fillRect(rect(),brush);
    painter.end();
    splash=true;
    //  setAttribute(Qt::WA_TranslucentBackground,true);
    // setAttribute(Qt::WA_NoSystemBackground,true);
    media_queue=new downloader(this);
    setObjectName("core");
    geom=new geofit(this);
    connect(qApp->primaryScreen(),SIGNAL( availableGeometryChanged(const QRect& )),this,SLOT(change_geo(const QRect& )));
    //   connect(this,SIGNAL(widgetRemoved(int)),this,SLOT(activate_current(int)));
    setAttribute(Qt::WA_TransparentForMouseEvents,true);
    init_menu();
    //  waiter_anim=new QPixmap(":/gui/BANNERS/wait.gif");
    //  waiter=new QLabel(this);
    //  waiter->setAlignment(Qt::AlignCenter);
    //  waiter->setStyleSheet("background-color:rgba(100,100,100,100);border:none;");
    //  waiter->hide();
    //  waiter->move(0,0);
    //  waiter->setFixedSize(size());
    //   waiter->setPixmap(*waiter_anim);
    go_to("sign_in");
  //  init_app();
    */
    showFullScreen();
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowState(Qt::WindowFullScreen);
    setGeometry(QApplication::primaryScreen()->availableVirtualGeometry());
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
    QPainter painter;
    bimg=QPixmap(rect().size());
    bimg.fill(QColor(0,0,0,0));
    QBrush brush(gradient);
    painter.begin(&bimg);
    painter.fillRect(rect(),brush);
    painter.end();
    debg=true;
    //  setAttribute(Qt::WA_TranslucentBackground,true);
    // setAttribute(Qt::WA_NoSystemBackground,true);
    media_queue=new downloader(this);
    setObjectName("core");
    geom=new geofit(this);
    connect(qApp->primaryScreen(),SIGNAL( availableGeometryChanged(const QRect& )),this,SLOT(change_geo(const QRect& )));
    //  connect(this,SIGNAL(widgetRemoved(int)),this,SLOT(activate_current(int)));
    setAttribute(Qt::WA_TransparentForMouseEvents,true);
    init_menu();
    //  waiter_anim=new QPixmap(":/gui/BANNERS/wait.gif");
    //  waiter=new QLabel(this);
    //  waiter->setAlignment(Qt::AlignCenter);
    //  waiter->setStyleSheet("background-color:rgba(100,100,100,100);border:none;");
    //  waiter->hide();
    //  waiter->move(0,0);
    //  waiter->setFixedSize(size());
    //   waiter->setPixmap(*waiter_anim);
    go_to("sign_in");
    init_app();
    set_credentials();
}

void navigation::activate_current(int i){
    if(count()==0)return;
    if(debg)qDebug()<<"Widget removed now is:"<<currentIndex()<<currentWidget()<<i;
    currentWidget()->activateWindow();
    currentWidget()->raise();
}
void navigation::paintEvent(QPaintEvent *){
    QRect r=QApplication::primaryScreen()->availableVirtualGeometry();
    QPixmap p;
    shadow_geo.setRect(15*r.width()/400,r.y()+20*r.height()/710,r.width()-30*r.width()/400,r.height()-40*r.height()/710);
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

void navigation::init_app(){
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
    cfi=0;
    cfs=0;
    cfm=0;
    login_logged=false;
    showed=false;
    stime="";
    lock=false;
#if defined (Q_OS_ANDROID) || defined (Q_OS_IOS)
    proceed_gps();
#endif
}
void navigation::terminateCreds(){
    if(debg)qDebug()<<"TERMINATE CREDS";
    //   mthread.quit();
    if(debg)qDebug()<<"thread-del";
    //   mthread.deleteLater();
}
void navigation::info(QString inf){
    QMessageBox::information(this,"",inf);
}
navigation::~navigation(){
    credentials->deleteLater();
    if(debg)qDebug()<<"WAND:NAVI:~navigation";
    qApp->quit();
}



