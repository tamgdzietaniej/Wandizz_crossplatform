#ifndef WIDGETGEN_H
#define WIDGETGEN_H

#include <QObject>
#include <QWidget>
#include <QOpenGLWidget>
#include <QtConcurrent/QtConcurrent>
#include "globals.h"
#include "gllabel.h"
#include "topmenuswitcher.h"
#include "appinspector.h"
#include <QCheckBox>
#include<QDesktopWidget>
#if defined(Q_OS_ANDROID)
#include <share_cross.h>
#elif defined (Q_OS_IOS)
#include <shareios.h>
#endif
class widgetGen :  public QOpenGLWidget
{
    Q_OBJECT

public:
    widgetGen(QWidget *parent = nullptr,QString="");
    ~widgetGen();
    void make_brick(int,int,int,int items_cnt,const QString&,const QString&,const QString&,const QString&,const QString&,
                    const QString&,int ind_oid=-1,QString event=no_data_string);
    void update_container(int c);
    QOpenGLWidget* marker_head,*marker_body[2],*marker_tail;
    void shft();
    double compr,prev_compr;
    Prospect* prospect_init_params;
    void corrGeo();
    bool get_hover();
    bool get_hover(QPoint());
    int scr;
    bool debg,ctx;
    QPoint mpos,nmpos;
    int dpi;
    int oldsel;
    void setMarkers();
    int corr_mrb;
    QRect shadow_geo;
    bool showme;
    int oldh,oldt;
    bool ready_to_paint;
    bool selectors_visible,selectors_visible_prev,issrch,nightmode;
    QFrame * tw;
    void forceUpdate();
    //  QTimer *timer;
    int moved,prev_moved;
    QLabel* labs;
    QWidget *midFrame=nullptr;
    void updateFrameSet();
    void showSelectors(bool);
    void setupWrapper(QList<QList<QWidget*>>);
    void setupInfo(QFrame*);
    QFrame* noResults,*prospectInfo;
    QInputMethod* input=QApplication::inputMethod();
    bool clicked(QWidget*);
    void setMrb(bool,int);
    int mrh();
    int mrb();
    int mrt();
    void setTypeID(bool);
    bool back_mutex;
    // lists
    void share();
    void activate();
    void clean();
    bool ftm;
    QImage im;
    enum swipeOrientations{
        not_set,
        horizontal,
        vertical
    } swipe_orient;
    void toggleNavi(bool);
    int oldpy,newpy;
    int inpt_h,brcorr;
    QList<QPointer<gLabel>> getTables();
    QPointer<topMenuSwitcher> top_menu_switcher;
    void setupTopMenuSwitcher(QRect);
    void setFrames(QWidget*,QWidget*);
    QString curr_event;
    void setCoords(QRect, int);
    QStringList getThumbRef(const QString&, QString);
    void setDirector(QWidget*);
    bool setContext(const QString&);
    void setSelectors(bool force=false);
    QPointer<gLabel>mglsearch(const QString lookfor);
    QRect get_srect(int),txt_rect,tim_rect;
    QList<QPointer<gLabel>> tmp_list;
    int lb,curr_oid,curr_title_id,curr_items_cnt;
    int br_deleted,iw,br_deleted_ind;
    const QString& getContext();
    void set_prospect_params(Prospect&);
    QList<QPointer<gLabel>>* itemList();
    QString actContext(),curr_netfile;
    bool sh_titles;
    void shTitles(bool);
    bool isProspect(QString t="");
    bool isVideos(QString t="");
    bool isFVideos(QString t="");
    bool isFScenes(QString t="");
    bool isFItems(QString t="");
    QPointer<gLabel> curr_item,producerOf(int i=-1);
    QString curr_type,curr_ctype;
    void set_fav_mode();
    QPointer<gLabel> item(int,QString t="");
    bool manageSelectors();
    int over_brick,over_brick_ind,antime,over_brick_hold,over_brick_ind_hold;
    QString filtered,prev_context;
    QRect rct,brct;
    int animating;
    qreal lmm,_prop;
    Prospect* par;
    int leased;
    void settleAt(int);
    void toggleBricks(bool);
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
    bool mclick();
    QList<int> fav_videos_list;
    void set_fav_videos_list(const QJsonArray*);
    void start_bg_proc();
    void stop_bg_proc();
    bool isLocked();
    void unlock();
    void lock();
    void freeze(bool);
    int msy();
    void setMRB(bool,int,int,int);
    bool scan;
    bool isNDis(),reinit;
    int  corr_y;
    gLabel* fooLab;
    bool br_updated;
    int wdth,scrr,scroffs;
    void restoreCoords();
    void setProspectInfo(QFrame*);
    void lockToggle(bool);
private:
    QLinearGradient gradient,gradient2;
    QRect tgeo;
    QList<QList<QWidget*>> wlist;
    QList<QList<QPoint>> wpos;
    bool type_id;
    int lastvis;
    bool naviDis;
    QMutex _mutex;
    void connectSignals(gLabel*);
    bool canToggleFavs();
    bool canToggleShares();
    int mfcorr;
    bool isTakingToCarousel();
    bool locked,lasttoggl,freezed;
    QFont txt_font,tim_font;
    int fl,lh,lw,_lw,_lw2,lm,vtm,htm;
    bool is_expanded,gen_busy;
    QString context;
    QFuture<void> future;
    bool fav_mode;
    QTimer ptimer,updtimer;
    QWidget* director;
    QList<QPointer<gLabel>> vb,fib,fsb,fvb,msb,foo;
    Qt::Alignment tim_align;
    QImage *ppi=nullptr, splash_img;
    QPoint pos, prev_pos, touch_pos;
    QVector2D move,diff;
    int tail_offs;
    void append_vscroll(int);
    void get_swipe_predicter();
    QList<int> q_vscroll;
    int get_vscroll(bool e=true);
    bool no_click;
    int old;
    QString br_deleted_type;
    int locker;
    bool wait_to_lock;
    bool need_corr;
    bool empty_queue;
    QTimer timer;
    QString getItemUpperText(int i=-1);
    QString getItemLowerText(int i=-1);
    int getItemOid(int i=-1);
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
    void download(const QString, const QString);
    void hided();
    void back(QString,QStringList={});
    void get_poster(int,bool,const QString&,const QString&,const QString&,const QString&);
    void go(const QString&,const QStringList&);
    void filter_done();
    void set_cnt(int);
    void closeMenu();
    void timershot(int);
    void setLocked(bool);
    void fav_update(int);
private slots:
    void setNextShot(int);
    void  revalidate(QPointer<gLabel>);
    void poster_generator(int, QString, QString, QString, QString, QString, QString,int);
    void make_poster();
    void noticeShow();
    void handlePressed(gLabel*,QMouseEvent*);
    void handleReleased(gLabel*,QMouseEvent*);
    void timerEv(bool force=false);

public slots:
    void finish_deletion();
    void brick_remove(int, int,const QString);
    void filter(const QString&);

protected:
    void paintGL() override;
    void initializeGL() override;
    void resizeEvent(QResizeEvent *e) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
};

#endif // WIDGETGEN_H
