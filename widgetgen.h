#ifndef WIDGETGEN_H
#define WIDGETGEN_H

#include <QObject>
#include <QWidget>
#include <QOpenGLWidget>
#include <QtConcurrent/QtConcurrent>
#include "globals.h"
#include "gllabel.h"
#include "widget_wrapper.h"
#include "topmenuswitcher.h"
#if defined(Q_OS_ANDROID)
#include <share_cross.h>
#elif defined (Q_OS_IOS)
#include <shareios.h>
#endif
class widgetGen : public QObject
{
    Q_OBJECT
public:

    widgetGen(QObject *parent = nullptr);
    ~widgetGen();
    void make_brick(int,int,int,int items_cnt,const QString&,const QString&,const QString&,const QString&,const QString&,
                    const QString&,int ind_oid=-1,QString event=no_data_string);
    bool back_mutex;
    // lists
    void share();
    void forceUpdate();
    void activate();
    bool ftm;
    QImage im;
    void toggleNavi(bool);
    int oldpy,newpy;
    int inpt_h,brcorr;
    void setContainer(widget_wrapper*);
    QPointer<topMenuSwitcher> top_menu_switcher;
    void setupTopMenuSwitcher(QRect);
    QInputMethod* input=QApplication::inputMethod();
    void setFrames(QWidget*,QWidget*);
    const QString &currEvent();
    void setCoords(QRect, int);
    double compr;
    QStringList getThumbRef(const QString&, QString);
    void setDirector(QWidget*);
    bool setContext(const QString&);
    void setSelectors(bool force=false);
    QPointer<gLabel>mglsearch(const QString lookfor);
    void setSearch(bool,QFrame* w=nullptr);
    QRect get_srect(int),txt_rect,tim_rect;
    QList<QPointer<gLabel>> tmp_list;
    int lb;
    int br_deleted,iw,br_deleted_ind;
    const QString& getContext();
    void setupWrapper(QList<QList<QWidget*>>);
    void set_prospect_params(Prospect&);
    QList<QPointer<gLabel>>* itemList();
    QString actContext();
    bool sh_titles;
    void shTitles(bool);
    bool isProspect(QString t="");
    bool isVideos(QString t="");
    bool isFVideos(QString t="");
    bool isFScenes(QString t="");
    bool isFItems(QString t="");
    QPointer<gLabel> currItem(),producerOf(int i=-1);
    QString currType();
    void set_fav_mode();
    QPointer<gLabel> item(int,QString t="");
    bool manageSelectors();
    int over_brick,over_brick_ind,antime,last_over_brick;
    QString filtered,prev_context;
    QRect rct,brct;
    int animating;
    qreal dpi,lmm,_prop;
    Prospect* par;
    int leased;
    void settleAt(int);
    void toggleBricks(bool);
    void toggleBricks();
    int tgs;
    bool delmutex;
    int offs;
    PostersReady pr;
    gLabel* fallDown;
    QSequentialAnimationGroup* group;
    QList<QPropertyAnimation*> del_anim;
    // lists
    QList<PosterData> poster_data;
    QList<PostersReady> poster_data_ready;
    bool prev_selector;
    QRect brick_geo(int i=-1);
    void toggle(gLabel*,bool,int i=0);
    QRect get_new_brick_geo(int i, int h=-1);
    qreal mrgh;
    int lastVis();
    gLabel* lastVisibleItem();
    int filter_chars;
    int getItemsCount();
    bool isExpanded(),catapult_brakes;
    bool pressed;
    int vcatapultscrollspeed, rubberbandscrollspeed;
    bool isMousePressed();
    int scrollPerform();
    bool mouseOver(QWidget*);
    void mouseMove(QMouseEvent*);
    void mousePress(QMouseEvent*);
    bool mouseRelease(QMouseEvent*);
    bool menu_opened;
    bool mclick();
    QList<int> fav_videos_list;
    void set_fav_videos_list(const QJsonArray*);
    QPointer<widget_wrapper> glcontainer;
    void start_bg_proc();
    void stop_bg_proc();
    bool isLocked();
    void unlock();
    void lock();
    int mrh();
    int msy();
    int mrt();
    void setMRB(bool,int,int,int);
    bool scan;
    bool isNDis();
    int  corr_y;
    gLabel* fooLab;
    int wdth,scrr,scr;
    void restoreCoords();
private:
    int lastvis;
    bool naviDis;
    QMutex _mutex;
    void connectSignals(gLabel*);
    bool canToggleFavs();
    bool canToggleShares();
    int lastscr,mfcorr;
    bool isTakingToCarousel();
    bool locked,lasttoggl;
    QFont txt_font,tim_font;
    int fl,lh,lw,_lw,_lw2,lm,vtm,htm;
    bool is_expanded,gen_busy;
    QString context;
    QFuture<void> future;
    bool fav_mode;
    QTimer ptimer,updtimer;
    QWidget* director;
    bool debg;
    QList<QPointer<gLabel>> vb,fib,fsb,fvb,msb,foo;
    Qt::Alignment tim_align;
    QImage *ppi=nullptr, splash_img;
    QPoint pos, prev_pos, touch_pos;
    QVector2D diff,move;
    int tail_offs;
    void append_vscroll(int);
    void get_swipe_predicter();
    QList<int> q_vscroll;
    bool no_click;
    int get_vscroll(bool e=true);
    int old;
    enum swipeOrientations{
        not_set,
        horizontal,
        vertical
    } swipe_orient;
    QTimer timer;
signals:
    void shown();
    void reval(QPointer<gLabel>);
    void ready_to_switch(int);
    void mouse_move(QPoint,int);
    void entered(int,int,int);
    void click_delete(int,int);
    void enableMenu();
    void click(int,int);
    void toggle_fav_video(int,bool);
    void del_fav(const QString&,int);
    void download(const QString&, const QString&, const int, bool);
    void hided();
    void back(QString,QStringList={});
    void get_poster(int,bool,const QString&,const QString&,const QString&,const QString&);
    void settled();
    void startProcessing();
    void fav_update(int);
    void send_item(int);
    void go(const QString&,const QStringList&);
    void preparing_shutdown(int);
    void filter_done();
    void stopTimer();
    void set_cnt(int);
    void startTimer();
    void do_poster();
    void closeMenu();
    void timershot(int);
private slots:
    void setNextShot(int);
    void  revalidate(QPointer<gLabel>);
    void poster_generator(int, QString, QString, QString, QString, QString, QString,int);
    void make_poster();
    void noticeShow();
    void handleEnter(int,int);
    void handleLeave();
    void timerEv(bool force=false);

public slots:
    void finish_deletion();
    void brick_remove(const QString, int);
    void filter(const QString&);
    void filter();
};

#endif // WIDGETGEN_H
