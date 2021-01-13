#include "navigation.h"

void navigation::set_favs(){
    set_favs("");
}
void navigation::set_favs(QString){
    if(currentWidget()){
        if(currentWidget()==carousel){
            qDebug()<<currentWidget();
            qDebug()<<currentWidget()->objectName();
            QWidget* fo=widget(currentIndex()-1);
            fav_opened=fo->objectName();
        }
        if(currentWidget()!=carousel && carousel!=nullptr){
            carousel->set_fav_items_list(credentials->jfi);
            carousel->set_fav_scenes_list(credentials->jfs);
            carousel->check_mw_fav();
        }
    }
    videos=&credentials->jv;
    fav_videos=&credentials->jfv;
    fav_scenes=&credentials->jfs;
    fav_items=&credentials->jfi;
    producers=&credentials->jp;
}
void navigation::restart_clean(){
    go_to("loginFB",{"logout"});
    QFile(f_app_hash).remove();
    QFile(f_user_avatar).remove();
    creds_ok=false;
    updated=false;
    shadow=false;
    context="";
    int wc=count();
    for(int i=0;i<wc;i++){
        if(widget(i)!=sign_in){
            // widget(i)->deleteLater();
            delete widget(i);
        }
    }
    init_app();
    show_module(sign_in);
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
// mrk::go_to
void navigation::go_to(QString to,QStringList params){
    //  if(wgen->isExpanded())
    //      wgen->toggleBricks(false);
    silent=false;
    qDebug()<<"PARAM:"<<credentials->prospect_settings.prospect_sh_items<<credentials->prospect_settings.prospect_sh_titles;
    media_queue->clearList();
    qDebug()<<"SENDER:"<<sender()<<to;
    if(sender())
        if(sender()!=credentials)qDebug()<<"BACK:SENDER:"<<sender();
    if(gone_to==to && to!="back"){
        if(debg)qDebug()<<"!!!!! PRESSED AGAIN!!!!!!";
        return;
    }
    gone_to=to;
    // closing menu
    if(mmenu->isVisible() && sender()!=mmenu){
        currentWidget()->setEnabled(true);
        if(debg)qDebug()<<"HIDING MENU"<<sender()<<to;
        mmenu->hide();
        update();
        if(to=="back"){
            gone_to="";
            return;
        }
    }
    if(debg)qDebug()<<"GOTO:"<<to;
    if(to=="loginFB"){
        proceed_webview();
        webview->setFacebookLogin();
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
    else if(to=="loginGoogle"){

    }
    else if(to=="loginApple"){
        QByteArray secret(credentials->get_jwt());
        QString url=loginAPurl+"?response_type=code&client_id=com.wandizz.producer&scope=email%20name&response_mode=form_post&usePopup=false&redirect_uri=https://producer.wandizz.com/authorize/apple/check.php";
        proceed_webview();
        webview->setAppleLogin();
        show_module(webview);
        webview->setUrl(url);
        return;
    }
    if(to.contains("back")){
        if(count()>1){
            QPointer<QWidget> dw=currentWidget();
            removeWidget(currentWidget());
            //carousel
            if(dw==carousel){
                if(!credentials->prospect_settings.player_enabled){
                    qDebug()<<"CLOSING:"<<dw->objectName();
                    dw->close();
                } else {
                    carousel->perform_close();
                }
            }
            QWidget *on=currentWidget();
            // shadows
            if(on==userprof || on==sign_in){
                shadow=false;
            }
            qDebug()<<"Currentw:"<<currentWidget();
            if(currentWidget()==msearch){
            go_to("prospect");
              return;
            }
            if(currentWidget()==video){
              go_to(video->context);
              return;
            }
            currentWidget()->raise();
            currentWidget()->showFullScreen();
        }
        gone_to="";
        mmenu->deactivate(currentWidget()->objectName());
        return;
    }
    if(to=="share"){
        share();
        return;
    }
    if(to=="s_fav_items"){
        to="fav_items";
        params.append("search");
    } else
        if(to=="s_videos"){
            to="videos";
            params.append("search");
        } else
            if(to=="userprof"){
                if(params.count()>0){
                    if(params.count()==1){
                        if(params.at(0)=="silent"){
                            silent=true;
                        }
                    } else
                        if(params.count()==3){
                            if(params.at(0)=="context"){
                                params.takeFirst();
                                context=params.takeFirst();
                                context_id=params.takeFirst().toInt();
                                if(video!=nullptr && credentials->prospect_settings.menu_context_restrict){
                                    removeWidget(video);
                                    video->close();
                                }
                                credentials->context_id=context_id;
                                credentials->context=context;
                                qDebug()<<"Context:"<<context<<context_id;
                                credentials->prepare_titles();
                                get_favs(true);
                            }

                        }
                }
                proceed_userprof();
                userprof->set_equal();
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
                                    to="fav_videos";
                                    if(!fav_videos->count()){
                                        if(fav_scenes->count()){
                                            to="fav_scenes";
                                        } else if(fav_items->count()){
                                            to="fav_items";
                                        }
                                    }
                                }
    if(to=="videos"){
        proceed_media("videos",params);
        shadow=true;
        show_module(video);
        video->create_videos_list(credentials->timeline_count_by_title_id,videos,fav_videos);
    }
    else
        if(to=="prospect"){
            proceed_msearch();
            shadow=true;
            show_module(msearch);
            msearch->create_prospect_list(credentials->timeline_count_by_title_id,videos,fav_videos);
        }
        else
            if(to=="carousel"){
                shadow=true;
                proceed_carousel(params);
                // show_module(carousel);
            } else
                if(to=="fav_scenes"){
                    if(fav_scenes->count() || show_even_if_zero){
                        proceed_media("fav_scenes");
                        shadow=true;
                        show_module(video);
                        video->create_scenes_list(fav_scenes);

                    }
                } else
                    if(to=="fav_items"){
                        if(fav_items->count() || show_even_if_zero){
                            proceed_media("fav_items",params);
                            shadow=true;
                            show_module(video);
                            video->create_items_list(fav_items);
                        }
                    } else
                        if(to=="fav_videos"){
                            if(fav_videos->count() || show_even_if_zero){
                                proceed_media("fav_videos");
                                shadow=true;
                                show_module(video);
                                qDebug()<<"FAVTI:"<<fav_videos;
                                video->create_fvideos_list(credentials->timeline_count_by_title_id,fav_videos);
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
    if(count()>0)qDebug()<<"GONE TO:"<<to;

}
// mrk:show_module
void navigation::show_module(QWidget* to){
    if(to!=nullptr)
    {
        if(currentWidget()!=to)
            addWidget(to);
        set_labels();
        if(silent){
            silent=false;
            go_to("prospect");
            return;
        }
        to->setEnabled(true);
        setCurrentWidget(to);
        currentWidget()->raise();
        if(!currentWidget()->isVisible()){
            currentWidget()->showFullScreen();
        }
        if(!currentWidget()->isActiveWindow()){
            currentWidget()->activateWindow();
        }
        update();
    }
    unlock();
    mmenu->deactivate(currentWidget()->objectName());
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
        connect(sync,SIGNAL(show_menu(QPoint)),this,SLOT(show_menu(QPoint)));
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
void navigation::proceed_carousel(const QStringList& pars)
{// title, QString time, QString});){
    qDebug()<<"params:"<<pars;
    if(carousel!=nullptr)
    {
        qDebug()<<"Carousel destroy";
        carousel->close();
    }
    QStringList params(pars);
    QString title=params.takeFirst();
    QString stime=params.takeFirst();
    QString netfile=params.takeFirst();
    QString f_oid=params.takeFirst();
    int title_id=params.takeFirst().toInt();
    carousel=new Carousel();
    carousel->hide();
    update();
    offset_ms=0;
    connect(carousel,SIGNAL(download(QString,QString)),media_queue,SLOT(push_url(QString,QString)),Qt::QueuedConnection);
    carousel->setObjectName("carousel");
    geom->resizer(carousel);
    carousel->set_fav_items_list(credentials->jfi);
    carousel->set_fav_scenes_list(credentials->jfs);
    carousel->set_sliders();
    if(stime!=""){
        offset_ms=carousel->hms2ms(stime);
        stime="";
    }
    carousel->set_params(title,title_id,netfile,offset_ms,credentials->player_offset,credentials->prospect_settings.player_enabled);
    connect(carousel,SIGNAL(go(QString,QStringList)),this,SLOT(go_to(QString,QStringList)));
    connect(carousel,SIGNAL(add_fav_item(int,QString,int,QString,QString,QString,int)),credentials,SLOT(add_fav_item(int,QString,int,QString,QString,QString,int)),Qt::QueuedConnection);
    connect(carousel,SIGNAL(add_fav_scene(int,QString,int,QString,QString,int)),credentials,SLOT(add_fav_scene(int,QString,int,QString,QString,int)),Qt::QueuedConnection);
    connect(carousel,SIGNAL(delete_item(int,QString)),this,SLOT(delete_item(int,QString)),Qt::QueuedConnection);
    connect(carousel,SIGNAL(show_menu(QPoint)),this,SLOT(show_menu(QPoint)));
    connect(carousel,SIGNAL(can_process()),this,SLOT(show_carousel()));
    connect(carousel,SIGNAL(cant_process()),this,SLOT(rollin_carousel()));
    carousel->aimParser();
}
void navigation::rollin_carousel(){
    carousel->close();
}
void navigation::get_favs(bool write){
    credentials->get_favs(write);

}
void navigation::proceed_msearch(){
    if(wgen==nullptr)
        proceed_wgen();
    if(!msearch){
        msearch=new metasearch(wgen);
        connect(msearch,SIGNAL(go(const QString&, const QStringList&)),this,SLOT(go_to(const QString&,const QStringList&)));
        connect(msearch,SIGNAL(show_menu(QPoint)),this,SLOT(show_menu(QPoint)));
        geom->resizer(msearch);
        msearch->init();
    }
    msearch->reinit(is_context());
}
bool navigation::proceed_media(const QString& on,const QStringList& srch){
    if(!video){
        video=new favItems(wgen);
        connect(video,SIGNAL(go(QString, QStringList)),this,SLOT(go_to(QString,QStringList)));
        connect(video,SIGNAL(show_menu(QPoint)),this,SLOT(show_menu(QPoint)));
        geom->resizer(video,true);
        video->init();
    }
    video->reinit(on,srch.contains("search"));
    return true;
}
void navigation::proceed_wgen(){
    qDebug()<<"WGEN PROC:";
    wgen=new widgetGen();
    wgen->set_prospect_params(credentials->prospect_settings);
    wgen->setCoords(rect(),devicePixelRatio()*2);
    connect(wgen,SIGNAL(closeMenu()),this,SLOT(hide_menu()));
    connect( wgen, SIGNAL( download( const QString&, const QString&, int,bool ) ), media_queue,SLOT(push_url(const QString&,const QString&, int,bool)),Qt::QueuedConnection);
    connect( wgen, SIGNAL( add_fav_videos( int ) ), credentials,SLOT(add_fav_videos(int)),Qt::DirectConnection);
    connect( wgen, SIGNAL( del_fav_videos( int, const QString& ) ),this,SLOT(delete_item(int,const QString&)));
    connect( wgen, SIGNAL( go(const QString&,const QStringList&)),this,SLOT( go_to(const QString&,const QStringList&)));
    qDebug()<<"DONE";
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
        connect(userprof,SIGNAL(exit_app()),this,SLOT(exit_app()));
        connect(userprof,SIGNAL(show_menu(QPoint)),this,SLOT(show_menu(QPoint)));
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
        connect(profsettings,SIGNAL(go(QString,QStringList)),this,SLOT(go_to(QString,QStringList)),Qt::QueuedConnection);
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
    if(video!=nullptr && (currentWidget()!=video || (currentWidget()==video && wgen->actContext()!=ri))){
        wgen->removeOffscreen(i_db);
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
        userprof->set_notif(videos->count());
    }
    if(profsettings!=nullptr){
        profsettings->set_email(curr_user,false);
        profsettings->set_phone(curr_phone,false);
        profsettings->set_nick(curr_nick,false);
        profsettings->set_avatar(false);
    }
    if(video!=nullptr){
        video->set_nick(curr_nick);
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
bool navigation::is_context(){
    qDebug()<<"COntextid:"<<credentials->prospect_settings.menu_context_restrict<<context_id;
    if(credentials->prospect_settings.menu_context_restrict)
        return (context_id!=-1);
    else return true;
}
void navigation::show_menu(QPoint r){
    if(!(fav_items->count()) && !show_even_if_zero)mmenu->deactivate("fav_items");
    if(!(fav_scenes->count()) && !show_even_if_zero)mmenu->deactivate("fav_scenes");
    if(!(fav_videos->count()) && !show_even_if_zero)mmenu->deactivate("fav_videos");
    mmenu->deactivate(currentWidget()->objectName());
    qDebug()<<"Context:"<<context_id;
    mmenu->set_access(is_context());
    mmenu->set_opt_geo(r);
    mmenu->setVisible(true);
    mmenu->raise();
    wgen->menu_opened=true;
    update();

}
void navigation::hide_menu(){
    mmenu->hide();
    wgen->menu_opened=false;
}
void navigation::navigate_upon_user_data(){
    get_current_user_data();
    creds_ok=!curr_user.contains("GUEST");
    get_favs(true);
    go_to("sign_in");
    sign_in->toggle_auth(true);
    if(creds_ok){
        go_to("userprof",{"silent"});
    }
}
void navigation::set_credentials(){
    if(credentials==nullptr){
        credentials=new Credentials();
        connect(credentials,SIGNAL(got_user_data()),this,SLOT(navigate_upon_user_data()),Qt::QueuedConnection);
        connect(credentials,SIGNAL(got_favs()),this,SLOT(set_favs()),Qt::QueuedConnection);
        connect(credentials,SIGNAL(got_titles()),this,SLOT(set_favs()),Qt::QueuedConnection);
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
navigation::navigation(QWidget *parent) : QStackedWidget(parent),splash(true),menuopened(false),fresh_run(true),context_id(-1){
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
    media_queue=new downloader(this);
    setObjectName("core");
    geom=new geofit(this);
    geom->resizer(this);
    connect(qApp->primaryScreen(),SIGNAL( availableGeometryChanged(const QRect& )),this,SLOT(change_geo(const QRect& )));
    //  connect(media_queue,SIGNAL(need_revalidate_widget(int)),this,SLOT(must_revalidate_widget(int)),Qt::QueuedConnection);
    //  connect(this,SIGNAL(widgetRemoved(int)),this,SLOT(activate_current(int)));
    setAttribute(Qt::WA_TransparentForMouseEvents,true);
    init_menu();

#if defined (Q_OS_ANDROID)
    uperm=new userPermissions(this);
    uperm->requestPermission();
#endif
    init_app();
}
void navigation::must_revalidate_widget(int i){
    if(count()==0) return;
    if(currentWidget()==video)
        wgen->item(i)->revalidate();
    else if(currentWidget()==msearch)
        wgen->item(i)->revalidate();
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
    login_logged=false;
    showed=false;
    lock=false;
#if defined (Q_OS_ANDROID) || defined (Q_OS_IOS)
    proceed_gps();
#endif
    set_credentials();
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



