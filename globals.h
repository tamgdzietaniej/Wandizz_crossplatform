#ifndef GLOBALS_H
#define GLOBALS_H
#include <QApplication>
#include <QAudioRecorder>
#include <QBitmap>
#include <QtConcurrent/QtConcurrentRun>
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
#include <QScreen>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QThread>
#include <QTimer>
#include <QTimerEvent>
#include <QTime>
#include <QToolButton>
#include <QUrl>
#include <QVideoWidget>
#include "math.h"
#include "geofit.h"
#include "socialLogin.h"
#include "downloaderStd.h"
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
static int version=2348;
static QString serverRoot="https://producer.wandizz.com";
static QString update_url=serverRoot+"/apk/";
static QString chk_update_url = update_url+"version.info";
static QString dwn_update_url = update_url+"downloads/app/";
static QString api_url=serverRoot+"/api/api.php?action=";
static QString get_wdz_url= serverRoot+"/api/listGen.php?action=wdz&title=";
static QString dataRoot=serverRoot+"/producers_data/";
static QString upload_url=serverRoot+"/engine/upload/avatar_upload.php";
static QString uploaded_url=serverRoot+"/custom/avatars/";
const static QString coords="&lat=&lon=";
static QString f_app_hash=hash_dir+"/app_hash.info";
static QString f_user_nick=hash_dir+"/user_nick.info";
static QString f_user_login=hash_dir+"/user_login.info";
static QString f_user_phone=hash_dir+"/user_phone.info";
static QString f_user_id=hash_dir+"/user_id.info";
static QString f_user_pass=hash_dir+"/user_id.info";
static QString f_user_fav_s=hash_dir+"/user_fav_s.list";
static QString f_user_fav_i=hash_dir+"/user_fav_i.list";
static QString f_user_fav_m=hash_dir+"/user_fav_m.list";
static QString f_user_fav_ml=hash_dir+"/user_fav_ml.list";
static QString f_user_avatar=hash_dir+"/avatar.png";
static QString f_global_titles=hash_dir+"/titles_global.list";
static QString d_carousel=hash_dir+"/carousel/";
static QString d_recorder=hash_dir+"/recorder_file";
static QString f_recorder=d_recorder+"/tmp";
static QString d_posters=hash_dir+"/posters/";
static QString f_poster_prefix="POSTER_";
static QString d_frames="frames/";
static QString d_media="media/";
static QString button_ss="border:none;background:transparent;background-image:none;";
static QString button_ss_act=button_ss+"color:rgb(120, 180, 255);";
static QString button_ss_nact=button_ss+";color:rgb(80,117, 215);";
static QString states[2]={button_ss_nact,button_ss_act};
static bool show_even_if_zero=true;
static QString loginFBurl="https://wandizz.com/api/index.php";
static QString loginTWurl="https://wandizz.com/api/twitter_login.php";
static QString f_videocut_filename="$fname.$from-$to";
static bool offline=false;
static int readers=0;
static int max_readers=200;
static int max_active=10;
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
static QList<FavsTable> favs_table;

//static QSize viewport=QApplication::primaryScreen()->virtualSize();
#if defined (Q_OS_IOS)

const QString pictures_dir=QStandardPaths::standardLocations((QStandardPaths::PicturesLocation)).last();
#elif defined (Q_OS_ANDROID)
const QString  pictures_dir=QStandardPaths::standardLocations((QStandardPaths::PicturesLocation)).first();
#else
const QString  pictures_dir=QStandardPaths::writableLocation((QStandardPaths::DocumentsLocation));
#endif




#endif // GLOBALS_H
