#ifndef MAINWIDGET_H
#define MAINWIDGET_H


#include "globals.h"
#include "math.h"
#include "stdlib.h"
#include "myvideosurface.h"
#include <QtConcurrent/QtConcurrentRun>
class MainWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit MainWidget(int,QWidget* parent=nullptr);
    ~MainWidget() override;
    bool loop=false;
    bool player_inited;
    bool player_enabled;
    bool is_video_frame;
    bool is_playing();;
    bool playing;
    bool shplayer;
    bool show_buffer;
    bool catch_stop;
    int player_offset;
    int cluster[99];
    bool debg;
    int first_assigned[99];
    int last_assigned[99];
    bool clustered,all_loaded;
    int prev_buffer2;
    int item_seeked;
    bool seeked;
    int total_loaded;
    int x,y,w,h,ox;
    int items;
    int beg_h,beg_y;
    QThread mthread;
    bool dodebug,pdodebug,first_frame;
    int fav_button,frames_cnt;
    QPixmap vframe;
   // qreal b_opacity;
    QPixmap vimage;
    QPixmap b_fav_image[2],b_play_image[2],*pix,b_pause_image,b_volume_image[2];
    int omw,prev_dist,oomw;
    bool last_frame;
    int lw,rw;
    void set_favs();
    QList<int> favs;
    void close_video();
    int srotate_to_element;
    int buffer_load;
    QPointF button_pos;
    QSize viewport=QApplication::primaryScreen()->virtualSize();
    bool vscrolling,scroll_locked,sscroling;
    struct event_data{
        QString event; // sklep
        QString image; // item image
        QString frame; // scene image
        QString name; //nazwa itemu
        QString frame_file; // plik
        QString item_file; // j.w.
        int dur; // duration

        int etime; // start
        int scene_id;
        int item_id;
        int tline_id;
        qreal offset;
    } ev_data[99];
    QString filename[99];
    bool scrolling,scrolled;
    QWidget* marker_left;
    bool animating=false;
    QPointF pos,prev_pos,press_pos;
    int catapult;
    void rewind();
    int index,ww,emited,faded;
    qint64 prev_time;
    int par_idx;
    QString player_url;
    int last_zeros;
    bool no_move;
    bool lock;
    qreal scr;
    int bleft,bmid,bright;
    qint64 start_time,video_end,end_time;
    void set_player();
    bool perform_close();
    enum swipe{
        LEFT,
        RIGHT,
        ZOOM,
        DOUBLE,
        PRESS,
        CLICK
    };
    QPixmap img;
    bool updated,p_stop_emitted;
    void prepare(struct event_data[],int,QSize,QString,QList<int>);
    bool is_autorotating=false;
    event_data edata[999];
    void get_coords();
    void unset_slave();
    int prev_buffer;
    void show_buttons(int);
    bool released;
    bool enabled;
    bool slave;
    int curr_in_front,left_offset;
    int last_created;
    QRect mw_geo;
    int processing,pr;
    QList<int> diffs;
    inline qreal fabs(qreal l){ if(l<0)return -l;else return l; }
    inline qreal asqrt(qreal l){ if(l<0)return -sqrt(-qreal(l));else return sqrt(qreal(l));}
    bool set_trip_to(int);
    int settinf_ind;
    int dpi,type,ol1,ol2,ol3,ol4,mw,rotate_to_element;
    bool need_update;
    int wind,corr_offset,old_pos;
    QSize wsize,msize;
    QTimer* timer;
    QString buff_perc;
    struct Widget{
        QPixmap* image;
        bool is_fav;
        int loaded;
    } widget[1000];
    void takeImage(int,QString);
    int rf;
    MyVideoSurface* video=nullptr;
    QMediaPlayer*player=nullptr;
    QMutex _mutex;
public slots:
    void init_player(QString);
    void setImage(const QImage&,bool);
    void set_slave();
    void state_changed(QMediaPlayer::State);
    void media_info(QMediaPlayer::MediaStatus);
    void media_buffer_filled_info(int);
    void play();
    void stop();
    void seek(qint64, bool play=false);
    void start_bg_proc();
private:
    QFuture<void> future;
    QString num(int),netfile;
    void needs_update();
    swipe swipe_dir;
    qreal hscroll_autopilot=0;
    qreal hscroll_iner=0;
    qreal trip,left,mid,right;
    QPixmap vim;
    qreal corr1;
    qreal corr2;
    void get_offs(qreal);
    QRect button_rect,play_rect,button2_rect,play2_rect,forbid_rect,volume_rect,volume2_rect;
    qreal get_trip_to(int);
    int closer_neighbour;
    QPixmap button_pix[2],forbid;
    bool all_rendered;
    qreal get_trip_home();
    bool shwd;
    qreal resc,tresc;
    QVector2D diff;
    bool pressed=false,unblocking_facer;
    Qt::AspectRatioMode aspect;
    void block_native_surface();
    QList<QFuture<void>> futures;
    QMutex _mutex5;

private slots:
    void timerev(bool st=false);
signals:
    void set_top(int);
    void wshow(int);
    void stop_p();
    void spin_friend(int);
    void go_web(QString);
    void set_favs_look_on_start(int);
    void add_fav_click();
    void got_image();
    void setFace(int,const QString&);
    void go_update();
protected:
    void paintGL() override;
    void initializeGL() override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
};

#endif // MAINWIDGET_H
