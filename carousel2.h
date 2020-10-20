#ifndef CAROUSEL_H
#define CAROUSEL_H
#include "mainwidget.h"

namespace Ui {
class Carousel;
class globals;
}
class Carousel : public QMainWindow
{
    Q_OBJECT

public:
    explicit Carousel(QWidget *parent = nullptr);
    ~Carousel();
    void setCheckPoint(QString);
    QTimer* ctimer;
    QString icon[10];
    void set_sliders();
    void set_params(QString,QString,const int,int,bool);
    int buffered=0;
    int player_offset;
    int cnt=0;
    int scene_cluster[100];
    QString netfile;
    int dcounter;
    int di,ds;
    bool not_loaded_yet,debg;
    QSize ssize,isize;
    QString _event,_image,_dur,_name;
    void timer_start();
    void reinit();
    void set_curr_data(int);
    void setBufferDir(QString);
    bool show_yt;
    void aimParser();
    int hms2ms(QString);
    QList<int> fav_scenes_list,fav_items_list;
    void set_fav_scenes_list(QJsonDocument);
    void set_fav_items_list(QJsonDocument);
    QString curr_title,title,app_version_s;
    bool exiting=false;
    MainWidget* sceneslider;
    MainWidget* itemslider;
    void perform_close();
private:
    Ui::Carousel *ui;
    geofit geo;
    int events_counter,origin_pos;
    QObjectList dn_list;
    QString ms2hms(int),last_event,old_ss;
    MainWidget::event_data ev_data[1000],car_data[100],faced_item_data,faced_scene_data;
    void search_deeper();
    void check_if_carousel_has_all();
    bool chk_buf_dir(QString);
    QString item_urls[1000],scene_data[1000],scene_urls[1000];
    QString  artist_and_album,artist,album;
 //   void correct_offset(int);
    QUrl urls[100];
    bool is_online;
  //  qint64 video_start,video_end,des_start;
    void initClasses();
  //  QVideoWidget*video;
 //   QMediaPlayer*player;
    QNetworkAccessManager* man;
    void setupEncoder();
    bool loading;
//    void init_player();
    void setUrls();
    QTime checkpoint;
    int loaded_items;
    bool render_video;
    QLabel* videolab;
    int duration;
    int paused_time;
    int empties;
    bool roll_to(int,int),pressed,slider_pressed;
    int trip_to;
    QString names[100];
    bool is_scene_fav(int);
    bool is_item_fav(int);
    void set_scene_fav(int);
    void set_item_fav(int);
    bool drawn,master_up;
    bool joinsliders;
    QList<int> irequested,srequested;
    int wind,corr_offset,old_pos;
    int filledup;
    bool first_time;
    bool need_stop;
    void setCarouselScenes(bool);
    QPixmap videopix;
    bool player_enabled;
public slots :

 //   void player_state_changed(QMediaPlayer::State);
  //  void media_buffer_filled_info(int);
 //   void media_info(QMediaPlayer::MediaStatus);
    void parser(QNetworkReply*);
 //   void play();
 //   void stop(bool s=true);
 //   void seek(qint64);
  //  void check_video_pos(qint64);
    void check_mw_fav();
private slots:
    void add_fav_scene_clicked();
    void add_fav_item_clicked();
    void generate_widget();
    void roll_scene();
    void updateMeta();
    void synchronize_scene_to_item(int);
    void synchronize_item_to_scene(int);
    void setTitle(const QString);
    void setTitle2(const QString);
    void setTitle();
    void setTitle2();
    void proceed_web(QString);
    void on_duration_sliderMoved(int position);
    void on_duration_valueChanged(int value);
    void on_b_back_clicked();
    void on_duration_sliderReleased();
    void on_b_options_clicked();
    void on_b_options_pressed();
    void on_b_options_released();
    void on_duration_sliderPressed();

protected:
    void showEvent(QShowEvent*);
private:
    QFuture<void> future;
    void deb(QString);
    QQuickWidget* qvideo;
    QString oldstyleitems,oldstylescenes;
    int frames_ready;
    QQmlContext* context;

    int noi,nos;
    bool need_seek;
    qint64 buffer_size;


signals:
    void sroll(int);
    void offscreen_delete(int,QString);
    void offscreen_add(int,QString);
    void carousel_finish();
    void add_fav_item(int,QString,QString,QString,QString,int);
    void add_fav_scene(int,QString,QString,QString,int);
    void delete_item(int,QString);
    void go(QString,QStringList);
    void go_web(QString);
    void go_user_prof();
    void sdebg(QString);
    void get_wdz(QString);
    void show_menu(QPoint);
    void run_carousel();
    void no_file(int,bool);
    void got_data();
    void download(QString,QString);
    void can_process();
};
#endif // CAROUSEL_H
