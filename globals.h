#ifndef GLOBALS_H
#define GLOBALS_H
#include <QObject>
#include <QGuiApplication>
#include <QAudioRecorder>
#include <QBitmap>
#include <QClipboard>
#include <QColor>
#include <QDebug>
#include <QDesktopServices>
#include <QEasingCurve>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFuture>
#include <QGeoCoordinate>
#include <QGeoLocation>
#include <QGraphicsOpacityEffect>
#include <QSurface>
#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>
#include <QHBoxLayout>
#include <QHoverEvent>
#include <QImage>
#include <QImageWriter>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLabel>
#include <QLineEdit>
#include <QLinearGradient>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaRecorder>
#include <QMessageBox>
#include <QtMessageHandler>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QOpenGLContext>
#include <QOpenGLWidget>
#include <QPaintDevice>
#include <QPainter>
#include <QPalette>
#include <QParallelAnimationGroup>
#include <QPicture>
#include <QPixmap>
#include <QPointer>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWidget>
#include <QQuickView>
#include <QRegularExpressionValidator>
#include <QResizeEvent>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QTimer>
#include <QTimerEvent>
#include <QTime>
#include <QToolButton>
#include <QUrl>
#include <QVideoWidget>
#include <QObject>
#include <QWindow>
#include "math.h"
#include "socialLogin.h"
#include "downloaderStd.h"
#include "permissions.h"

/* ANDROID */
#if defined(Q_OS_ANDROID)
#include <QtAndroid>
#include <QtAndroidExtras>
#include <QAndroidJniObject>
#endif
#if defined(Q_OS_ANDROID)
static QString hash_dir=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/WANDIZZ";
#else
static QString hash_dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/WANDIZZ";
#endif
class globals:public QObject{
    Q_OBJECT
public:
    globals(QObject* parent=0);
    static bool osd;

};

static QString version="8.3.6.12";
static QString no_data_string="sorry, bu i haven't got any infrmation about it :(";
static QString serverRoot="https://producer.wandizz.com";
static QString update_url=serverRoot+"/apk/";
static QString chk_update_url = update_url+"version.info";
static QString dwn_update_url = update_url+"downloads/app/";
static QString api_url=serverRoot+"/api/api.php?action=";
static QString get_wdz_url= serverRoot+"/api/listGen.php?action=wdz&title=";
static QString dataRoot=serverRoot+"/producers_data/";
static QString upload_url=serverRoot+"/engine/upload/avatar_upload.php";
static QString uploaded_url=serverRoot+"/custom/avatars/";
static QString ftp_avatars="ftp://producer.wandizz.com";
const static QString coords="&lat=&lon=";
static QString f_app_hash=hash_dir+"/app_hash.info";
static QString f_user_nick=hash_dir+"/user_nick.info";
static QString f_user_login=hash_dir+"/user_login.info";
static QString f_user_phone=hash_dir+"/user_phone.info";
static QString f_user_id=hash_dir+"/user_id.info";
static QString f_user_pass=hash_dir+"/user_id.info";
static QString f_user_fav_s=hash_dir+"/user_fav_s.list";
static QString f_user_fav_i=hash_dir+"/user_fav_i.list";
static QString f_user_fav_v=hash_dir+"/user_fav_m.list";
static QString f_titles_ev_cnt=hash_dir+"/f_titles_ev_cnt.list";
static QString f_user_fav_ml=hash_dir+"/user_fav_ml.list";
static QString f_producers=hash_dir+"/producers.list";
static QString f_user_avatar=hash_dir+"/avatar.png";
static QString f_global_titles=hash_dir+"/titles_global.list";
static QString d_carousel=hash_dir+"/carousel/";
static QString d_recorder=hash_dir+"/recorder_file";
static QString f_recorder=d_recorder+"/tmp";
static QString d_posters=hash_dir+"/posters/";
static QString d_logos=hash_dir+"/logos/";
static QString f_poster_prefix="POSTER_";
static QString d_frames="frames/";
static QString d_media="media/";
static QString f_timeline="timeline/";
static QString button_ss="border:none;background:transparent;background-image:none;";
static QString button_ss_act=button_ss+"color:rgb(120, 180, 255);";
static QString button_ss_nact=button_ss+";color:rgb(80,117, 215);";
static QString states[2]={button_ss_nact,button_ss_act};
static QIcon star[2];
static bool show_even_if_zero=true;
static QString loginFBurl="https://producer.wandizz.com/authorize/facebook/index.php";
static QString loginTWurl="https://producer.wandizz.com/api/twitter_login.php";
static QString loginAPurl="https://appleid.apple.com/auth/authorize";
static QString f_videocut_filename="$fname.$from-$to";
static bool offline=false;
static int readers=0;
static int max_readers=200;
static int max_active=6;
static int probe_interval=17;
static int max_q=2;
static int pmi;
static int pms;
struct FavsTable{
    int index;
    int db_index;
    QString param1;
    QString title;
    QString url;
    QString image;
    QString event;
};
struct STable{
    int index;
    int db_index;
    QString param1;
    QString title;
    QString url;
    QString image;
    QString event;
    QString type;
    int oid;
};
struct Prospect{
    bool prospect_osd;
    int player_offset;
    bool player_enabled;
    bool prospect_favs;
    bool prospect_shares;
    bool menu_context_restrict;
    bool prospect_initial_view;
    int prospect_filter_chars;
    bool prospect_sh_items;
    bool prospect_sh_titles;
    bool search_mag_glass;
    bool search_switch_with_top_bar;
    double widget_height_factor;
    QString videos_coming_soon_text;
    int videos_coming_soon_text_size;
    int videos_coming_soon_picture_dim;
    QString search_text_color;
    QString search_field_color;
    QString search_text_placeholder;
    int search_height_factor;
};
/*
// bool prospect_osd;


struct MediaPlayer{
    bool enabled=false;
    int rev_offset=0;
    int video_duration=4000;
};
struct SCarousel{
    MediaPlayer mediaPlayer;
} carousel;



struct Prospect{
    bool context_has_to_be=false;
    Osd osd;
    Search search;
    VWidget vwidget;
} prospect;
*/
struct Colors{
    QString text;
    QString bg;
    QString buttons;
};
struct Sizes{
    double height_factor;
    double width_factor;
    int height;
    int width;
};
struct Content{
    QString text;
    QString color;
    QString bg_color;
};

struct text_plate{
    Sizes size;
    Content content;
};
struct Search{
    int idle_until;
    bool between_items;
    bool between_titles;
    Sizes size;
    text_plate placeholder;
    text_plate coming_soon_plate;
};
struct VWidget{
    Sizes sizes;
};
struct onScreen{
    bool show_favs=1;
    bool toggle_favs=0;
    bool show_shars=0;
    bool view_all_at_start=0;
    bool zooming_glass=1;
    bool show_all_at_startup=0;
    bool extra_dimmer=true;
};
struct APPL{
    bool context_has_to_be=false;
 //   onScreen oscreen;
  //  Search search;
 //   VWidget vwidget;
};

struct PosterData{
    int index=-1;
    QString type="";
    QString Url="";
    QString file;
    QString up="";
    QString dn="";
    QString context="";
    int itcnt=0;
    bool in_dl_queue=false;
    bool to_dload=false;
    bool is_splash=true;
};
struct PostersReady{
    int index;
    QString type;
    QString file;
    QImage* poster;
    QString context;
    bool ready;
};
enum vP{
    OVER= 0,
    UNDER=1
};

static QList<FavsTable> favs_table;
static QList<STable> sTable;

//static QSize viewport=QApplication::primaryScreen()->virtualSize();
#if defined (Q_OS_IOS)

const QString pictures_dir=QStandardPaths::standardLocations((QStandardPaths::PicturesLocation)).last();
const QString cache_dir=QStandardPaths::standardLocations((QStandardPaths::TempLocation)).last();
#elif defined (Q_OS_ANDROID)
const QString  pictures_dir=QStandardPaths::standardLocations((QStandardPaths::PicturesLocation)).first();
#else
const QString  pictures_dir=QStandardPaths::writableLocation((QStandardPaths::DocumentsLocation));
#endif




#endif // GLOBALS_H
