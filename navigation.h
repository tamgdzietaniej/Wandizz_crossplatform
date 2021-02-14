#ifndef NAVIGATION_H
#define NAVIGATION_H
#include "globals.h"
#include "credentials.h"
#include "signin.h"
#include "syncscreen.h"
#include "geofit.h"
#include "favitems.h"
#include "userprof.h"
#include "profsettings.h"
#include "carousel2.h"
#include "credentials.h"
#include "sqlapi.h"
#include "localization.h"
#include "menu.h"
#include "downloader.h"
#include "downloaderStd.h"
#include "socialLogin.h"
#include "userPermissions.h"
class navigation:public QStackedWidget{
    Q_OBJECT
public:
    explicit navigation(QWidget *parent = nullptr);
    ~navigation();

    double opac;
#if defined(Q_OS_ANDROID)
    userPermissions* uperm;
#endif
    QString gone_to;
    QString carousel_title;
    QString url_to_load="";
    QStringList fav_scenes_list,fav_items_list,fav_videos_list;
    QString user_data="";
    bool msearch_in_action;
    bool menuopened,fav_modified;
    QString curr_favs;
    //  QThread mthread;
    bool creds_ok=false;
    bool prevent_click;
    QJsonArray *media,*fav_videos,*producers,*fav_scenes,*fav_items;
    void set_user();
    QFuture<void> future;
    QRect shadow_geo;
    void resetContext();
    bool splash,fresh_run;
    QWidget* parm;


    QString web_url;

    QFrame* msrch;
    bool lock;
    QScreen* gs = QApplication::primaryScreen();
    bool fav_scenes_needs_reload,fav_items_need_reload;
    QPointer<Credentials> credentials=nullptr;
    int mess_notif=0;
    int not_notif=0;
    int curr_social1=0;
    QTimer* shower;
    bool showed;
    int curr_social2=0;
    QThread mythread;
    QStringList lparams;
    bool save_clicked;
    bool shadow;
    QString fav_opened;
    void create_fav_items();
    void create_fav_scenes();
    bool offline_mode,updated;
    QDir d;
    bool guest_login;
    bool fromfavs;
    bool path_once;
    int titles_cnt;

    QPointer<menu> mmenu=nullptr;
    bool login_logged;
    void init_menu();
    QLabel* waiter;
    QPixmap* waiter_anim;
    bool prospect_shown;
    QLinearGradient gradient,gradient2;
    bool debg;
    QWidget* rwidget;
    QStringList prev_cpars,prev_mpars;
    QRect mg;

public slots:
    void set_labels();
    void init_app();
    void info(QString);
    void share();
    void change_geo(const QRect& );
    void erase_module();
    void check_email_exists(QString,bool);
    void save_profile();
    void restart_clean();
    void proceed_sign_in();
    void proceed_webview();
    bool proceed_media(const QString&);
    void proceed_sync();
    void proceed_carousel(const QStringList& params);
    void proceed_userprof();
    void proceed_profsettings(bool acc_recovery=false);
    void update_loc(QString,QString,QString);
    void proceed_gps();
    void proceed_wgen();
    void disableCurrMenuEntry();
    bool menuOpened();

    // CAROUSEL -> MENU
    //   void splash_upgrade();
    void set_credentials();
    void show_web(QString);

private slots:
    void showMenu();
    void hideMenu();
    void setFavModified();
    void setSocialData(QString,QString,QString,QString,QString);
    void rollin_carousel();
    void terminateCreds();
    void remind_password(QString);
    void update_user(QString,QString,QString,QString);
    void go_to(QString,QStringList params={});
    void navigate_upon_user_data();
    void recovery_user();
    void activate_current(int);
    void show_carousel();
    void exit_app();
    void must_revalidate_widget(int);

    void updateFavs();
private:
    QStringList curr_params;
    bool isCWName(QString);
    QPointer<widgetGen> wgen=nullptr;
    downloader* media_queue;
    void show_profsettings();
    void show_module(QWidget* to=nullptr);
    QWidget* to_show;
    int offset_ms;
    geofit geom;
    bool menu_opened;
#if defined(Q_OS_ANDROID)
    void set_splash();
#endif
public:
    QPointer<Carousel> carousel=nullptr;
    QPointer<socialLogin> webview=nullptr;
    QPointer<SyncScreen> sync=nullptr;
    QPointer<SignIn> sign_in=nullptr;
    QPointer<favItems> videos=nullptr;
    QPointer<userProf> userprof=nullptr;
    QPointer<profSettings> profsettings=nullptr;
    QPointer<localization> gps=nullptr;
protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* e) override;
    //void mouseReleaseEvent(QMouseEvent* e) override;

signals:
    void turn_ss_off();
    void go_show_rest();
    void finished();
    void stop_follow();
    void go_resizer(QWidget*);
    void upd_favs(QString,bool);
    void set_url_images(int,QStringList);
    void carousel_item_added();
    void carousel_scene_added();
    void restart();
};




#endif // NAVIGATION_H
