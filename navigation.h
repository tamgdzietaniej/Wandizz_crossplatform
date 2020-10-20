#ifndef NAVIGATION_H
#define NAVIGATION_H
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
#include "myvideosurface.h"

class navigation:public QStackedWidget{
    Q_OBJECT
public:
    explicit navigation(QWidget *parent = nullptr);
    ~navigation();

    double opac;
    QString gone_to;
    QPixmap*vpix;
    QPixmap bimg;
    QTimer*ktimer;
    QString carousel_title;
    QString s_curr_id,curr_user,curr_nick,curr_phone;
    QString url_to_load="";
    QString stime;
    QStringList fav_scenes_list,fav_items_list,fav_videos_list;
    QString
    title,netfile;
    QString user_data="";
    bool menuopened;
    QString curr_favs;
    //  QThread mthread;
    bool creds_ok=false;
    bool prevent_click;
    QJsonArray titles,fav_titles;
    void set_user();
    QFuture<void> future;
    QRect shadow_geo;
    bool splash;
    QWidget* parm;
    bool navlock();
    bool unlock();
    QString web_url;
    bool lock;
    QScreen* gs = QApplication::primaryScreen();
    bool fav_scenes_needs_reload,fav_items_need_reload;
    QPointer<Credentials> credentials=nullptr;
    int mess_notif=0;
    int not_notif=0;
    int curr_social1=0;
    int cfi,cfs,cfm;
    QTimer* shower;
    bool showed;
    int curr_social2=0;
    void set_labels();
    QThread mythread;
    bool registering=false;
    QWidget* tohide=nullptr;
    void set_favs_cnt(int);
    QTimer* vt;
    QTimer* timer=new QTimer(this);
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
    QLinearGradient gradient,gradient2;
    bool debg;
public slots:
    void init_app();
    void info(QString);
    void share();
    void change_geo(const QRect& );
    void erase_module();
    void show_menu(QPoint);
    void set_favs();
    void set_favs(QString);
    void check_email_exists(QString,bool);
    void save_profile();
    void toggle_input(bool);
    void restart_clean();
    void proceed_sign_in();
    void proceed_webview();
    bool proceed_media(QString,QStringList srch={});
    void proceed_sync();
    void proceed_carousel(QStringList params);
    void proceed_userprof();
    void proceed_profsettings(bool acc_recovery=false);
    void update_loc(QString,QString,QString);
    void proceed_gps();

    // CAROUSEL -> MENU
    //   void splash_upgrade();
    void set_credentials();
    void show_waiter();
    void hide_waiter();
    void show_web(QString);

private slots:
    void setSocialData(QString,QString,QString,QString,QString);
    void terminateCreds();
    void show_rest();
    void get_favs(bool);
    void set_titles(QJsonArray);
    void remind_password(QString);
    void update_user(QString,QString,QString,QString);
    void get_current_user_data();
    void delete_item(int,QString);
    void go_to(QString,QStringList params={});
    // void go_to(QString);
    void show_fav_scenes();
    void show_fav_items();
    void show_fav_videos();
    void show_videos();
    void prepare_favs(QString);
    void navigate_upon_user_data();
    void recovery_user();
    void activate_current(int);
    void exit_app();
private:
    downloader* media_queue;
    void show_profsettings();
    void show_module(QWidget* to=nullptr);
    QWidget* to_show;
    int offset_ms;
    geofit* geom;
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
