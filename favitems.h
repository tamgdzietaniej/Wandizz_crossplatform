#ifndef FAVITEMS_H
#define FAVITEMS_H

#include "gllabel.h"
#include "widget_wrapper.h"
#include "widget_mapper.h"
#include <QShowEvent>
#include <QHideEvent>
#include <QAnimationGroup>
#include <QSequentialAnimationGroup>
#if defined(Q_OS_ANDROID)
#include <share_cross.h>
#elif defined (Q_OS_IOS)
#include <shareios.h>
#endif

namespace Ui {
class favItems;
}

class favItems : public QMainWindow
{
    Q_OBJECT


public:
    explicit favItems(QWidget *parent = nullptr);
    ~favItems() override;
    QString events[100],on,prev_on;
    int tmp_last;
    void init();
    bool cleaning;
    bool cleanUp(QString type);
    bool switchView(bool);
    void set_nick(QString);
    void set_favs_cnt(int);
    QString creating;
    void mark_fav(int);
    int bricks_shown;
    bool cleanAnimPrepared,fresh_run;
    QSequentialAnimationGroup* cleanGroup;
    QSequentialAnimationGroup* ag;
    bool show_videos(int,int,int,QString,QString);
    bool show_fav_videos(int,int,int,QString,QString);
    bool show_item(int,int,QString,QString,QString,QString);
    bool show_scene(int,int,QString,QString,QString,QString);
    void create_videos_list(QList<FavsTable>);
    void create_scenes_list(int,QJsonDocument,QJsonArray);
    void create_items_list(int,QJsonDocument);
    void create_fvideos_list(int,int[],QJsonArray,QJsonArray);
    bool debg;
    void make_poster();
    QRect brects[100];
    struct PosterData{
        int index;
        bool stage;
        QString type;
        QString file;
        QString up;
        QString dn;
    };
    struct PostersReady{
        int index;
        bool stage;
        QString type;
        QImage poster;
    };
    QList<PosterData> poster_data;
    QList<PostersReady> poster_data_ready;
    void showBricks();
    int lastBrick(QString type="");
    // void unvalidate(int,int,int);
    void removeOffscreen(QString,int);
    void animate(bool);
private:
    QStringList getThumbRef(QString,QString);
    Ui::favItems *ui;
    bool generator_active;
    bool chk_objectname(QString);
    bool offline,tsd,initializing,locked,scfilled, no_click, pressed, hide_top;

    int max_q, mitems_cnt[1000], scrr, old_scr,old_scrp;
    int main_h, cnt, br_deleted, over_brick=-1, over_brick_ind=-1, dpi,lastVis;
    QString filtered;
    int probe_freq, tmrc, tmrp, tmrm, tmrs;
    QString nick, titles[100], old_ss;
    QPoint pos, prev_pos, touch_pos;
    qreal mrgv ,mrgh, vcatapultscrollspeed, rubberbandscrollspeed;
    enum swipeOrientations{
        not_set,
        horizontal,
        vertical
    } swipe_orient;

    QOpenGLWidget marker,marker_first,marker_middle;
    QVector2D diff,move,swipe_action_predict;
    QTimer timer;
    QParallelAnimationGroup* group;
    // lists
    QList<QPropertyAnimation*> del_anim;
    QList<int> fav_videos_list;
    QList<int> q_vscroll;

    // icons
    QIcon a[2]={QIcon(":/gui/green_dot_no_dot.png"),QIcon(":/gui/green_dot.png")};

    QList<gLabel*>& itemList(QString o="-");
    QFuture<void> future;
    QTimer ptimer,pdimmer;
    QJsonDocument favlist;
    QSize bsize;
    QRect brick_geo(int i=-1);
    bool offscreenanim;
    int offscreenindicator;
    // functions
    bool animfilled;
    void share();
    void toggleSelectors(bool);
    void append_vscroll(int);
    void get_swipe_predicter();
    int get_vscroll();
    void hide_temps();
    QPointer<gLabel> currItem();
    QPointer<gLabel> item(int,QString o="-");
    void set_proxy(widget_wrapper*,QWidget*w,QWidget*p);
    void set_wrappers();
    void give_event();
    bool need_clean;
    bool animating;
    // widget_wrapper* top_frame_switch_wrapper=nullptr,*bg_wrapper=nullptr,*selectors_wrapper=nullptr,*top_frame_wrapper=nullptr,
    widget_wrapper *glcontainer=nullptr;
    widget_mapper* glmapper=nullptr;
    gLabel* curr_item,*settle_into;
    void set_markers();
    bool postering;
    int antime;
    int hidden_cnt;
    QImage splash_img,*ppi=nullptr;
    bool need_correct;
    bool noanim;
    // classes
    //  gLabel* vb[100],*fib[100],*fsb[100],*fvb[100];
    QList<gLabel*> vb,fib,fsb,fvb,foo;
    Qt::Alignment tim_align;
    int corr;

public slots:
    void removeClean();
    void poster_generator(int, bool,QString,QString,QString,QString);
    void handle_enter(int,int);
    void hide_top_wrapper();
    void stop_bg_proc();
    void start_bg_proc(int t=-1);
    void reinit(QString);
    bool make_brick(int,int,QString,QString,QString,QString,QString,QString);
    void click();
    void set_fav_videos_list(QJsonDocument);
    void set_active();
    void update_ui();
    void countHidden();
    void enableSearch();

private slots:
    void settled();
    void unlockBrick(QAbstractAnimation::State,QAbstractAnimation::State);
    void lockBrick();
    void brick_remove(int,int);
    void finish_deletion();
    void on_b_items_clicked();
    void set_wrappers_wraps();
    void on_b_scenes_clicked();
    void on_b_options_clicked();
    void on_b_options_pressed();
    void on_b_options_released();
    void on_b_scenes_pressed();
    void on_b_items_pressed();
    void on_b_scenes_2_clicked();
    void on_b_items_2_clicked();
    void on_b_videos_clicked();
    void on_b_videos_2_clicked();
    void on_b_videos_pressed();
    void on_b_videos_2_pressed();
    void on_b_scenes_2_pressed();
    void on_b_items_2_pressed();
    void on_b_options_2_clicked();
    void on_b_back_clicked();
    void timerev();
    void on_b_back_2_clicked();
    void handleLeave();
    void manageSelectors(bool force=false);
    void filter(QString);
    void noticeShow();

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void showEvent(QShowEvent*) override;

signals:
    void add_fav_videos(int);
    void delete_item(int,QString);
    void go(QString,QStringList);
    void show_menu(QPoint);
    void download(QString,QString);
    void clean();
    void startProcessing();
    void settle_poster(int,QString,const QImage&,bool);

};

#endif // FAVITEMS_H
