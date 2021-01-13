#ifndef SYNCSCREEN_H
#define SYNCSCREEN_H
#include <QApplication>
#include <QMainWindow>
#include <QMediaRecorder>
#include <QAudio>
#include <QTimer>
#include <QLinearGradient>
#include <QLayout>
#include <QObject>
#include <QMetaType>
#include <QAudioProbe>
#include <audiorecorder.h>
#include <acr_api.h>
#include <audiolevel.h>
#include <aws_api.h>
#include "mypushbutton.h"

namespace Ui {
class SyncScreen;
class acr_api;
}
static qreal getPeakValue(const QAudioFormat &format);
static QVector<qreal> getBufferLevels(const QAudioBuffer &buffer);

template <class T>
static QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels);
class SyncScreen : public QMainWindow
{
    Q_OBJECT

public:
    explicit SyncScreen(QWidget *parent = nullptr);
    ~SyncScreen();
    QString curr_user;
    QString curr_id;
    QTime* internal_process_offset_timer=new QTime();
    QTime *c_show_timer;
    void hide_navs();
    void show_navs();
    int screenw,screenh;
    myPushButton* recordbut;
    void set_rec_button();
    int autorectime;
    void get_in();
    bool car_sh;
    QString old_ss;
    void setCentWGeo(QRect);
    void emitready();
    myPushButton* record;
    void initClasses();
private:
    Ui::SyncScreen *ui;
    qreal oldsadj;
    QList<AudioLevel*> m_audioLevels;
    void clearAudioLevels();
    QAudioProbe *m_probe = nullptr;
    QPropertyAnimation* sanim;
    QRect srect;
    void on_send_cli_clicked();
    void postInit();
    void do_when_test();
    acr_api* acr;
    QString browser_data;
    QString curr_primary_key_id,curr_payload,acr_response;
    bool timer_on,manual_stop,
        timer_active,auto_fp,auto_cli,parser_used,is_newer_version,show_yt,is_signed,must_sign_in,test;
    QTimer* ticker,*meta_ticker, *update_ticker, *synchro_ticker, *fader;
    QFileInfo finfo;
    AudioRecorder* recorder;
    void slide_navs();
    QString content_type,must_sign_in_hwid;
    QString getFile(QString),cli_format,lo_format,videos_format;
    qint64 writeFile(QString,QByteArray);
    bool delPreviousFP(QString);
    QString event_file,last_event;
    QDateTime* monitor_timer_start;
    int offset_ms;
    qint64 checkpoint_ms;
    int cnt,updater_ver,app_version,curr_version;
    QTime checkpoint;
    int attempt;
    QString  artist_and_album,artist,album;
    QString eventer_query="http://wandizz.com/acr/engine/aws/awsCommerceLoader.php"; // amazon loader
    QTimer t1;
    QTimer t2;
    QTimer autorectimer,timer;
    QNetworkAccessManager manager,*tm,img_manager,frame_manager;
    QString curr_title,title,app_version_s;
    int duration;
    void initTimers();
    void setupEncoder();

    bool found=false;
    //    bool  checkUpdate();
    //    bool checkUser();
    void initVars();
    void setupGps();
    QString encodePar(QString,bool);
    // adres WWW serwera
    QString serverRoot;
    // apk welcome screen
    QLinearGradient gradient2;
    void setUrls();
    //  QString check_user_registered();
    bool opt_pressed;
    QTime timer2;

protected:
    // void paintEvent(QPaintEvent*) override;
signals :
    void process();
    void go(const QString,QStringList);
    void here_i_am();
    void start_offset_meter();
    void show_menu(QPoint);
    void ready();
public slots :
    void setTitle2(const QString);
    void setTitle2();
    void setTitle2(const QString, int);
    // void preInit();



private slots:
    void processBuffer(const QAudioBuffer&);
    void on_record_clicked();
    void status(QMediaRecorder::State);
    void getAnswer(QNetworkReply*);
    void updateTicker();
    void startTicker();
    void stopTicker();
    void recTimeout();
    void isRecording();
    void isStopped();
    void carousel(QString);
    void on_youtube_clicked();
    void show_carousel();
    void on_b_options_clicked();
    void on_b_back_clicked();
};
#endif // SYNCSCREEN_H
