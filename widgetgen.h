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
    Q_DISABLE_COPY(widgetGen)
public:

    explicit widgetGen(QObject *parent = nullptr);
    ~widgetGen();
    void make_brick(int,int,int,int items_cnt,const QString&,const QString&,const QString&,const QString&,const QString&,
                    const QString&,int ind_oid=-1);
    void set_markers(QOpenGLWidget* mf,QOpenGLWidget* mm,QOpenGLWidget* ml, QOpenGLWidget* me,QOpenGLWidget* ms);
    bool back_mutex;
    // lists
    void share();
    void activate(bool);
    void hideRastered(QWidget*);
    int inpt_h;
    QPointer<topMenuSwitcher> top_menu_switcher;
    void setupTopMenuSwitcher(QRect);
    QInputMethod* input=QApplication::inputMethod();
    QString events[100];
    void setFrames(QList<QWidget*> f1,QList<QWidget*> f2);
    void setCoords(QRect,int);
    QIcon star[2];
    QStringList getThumbRef(const QString&, QString);
    void setDirector(QWidget*);
    bool setContext(const QString&);
    QRect get_srect(int),txt_rect,tim_rect;
    void showSelectors();
    int br_deleted,iw;
    const QString& getContext();
    void setupWrapper(QList<QList<QWidget*>>);
    void set_prospect_params(Prospect&);
    QList<QPointer<gLabel>> itemList();
    QString actContext();
    void helper();
    bool isProspect();
    QPointer<gLabel> currItem(),producerOf(int i=-1);
    QString currType();
    void set_fav_mode();
    QPointer<gLabel> item(int,QString t="");
    bool manageSelectors();
    int over_brick,over_brick_ind,antime;
    QString filtered,prev_context;
    QRect rct,brct;
    int animating;
    qreal dpi,lmm,_prop;
    Prospect* par;
    void settleAt(int);
    void toggleBricks(bool);
    void toggleBricks();
    struct PosterData{
        int index;
        bool stage;
        QString type;
        QString file;
        QString up;
        QString dn;
        QString context;
        int itcnt;
    };
    struct PostersReady{
        int index;
        bool stage;
        QString type;
        QPixmap* poster;
        QString context;
    };
    PostersReady pr;
    QParallelAnimationGroup* group;
    QList<QPropertyAnimation*> del_anim;
    // lists
    QList<PosterData> poster_data;
    QList<PostersReady> poster_data_ready;
    void removeOffscreen(int);
    int lastBrick();
    QRect brick_geo(int i=-1);
    QRect get_new_brick_geo(int i, int h=-1);
    qreal mrgh;
    int lastVis;
    int filter_chars;
    void give_event();
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
    void mark_fav(int);
    QPointer<widget_wrapper> glcontainer;
    void start_bg_proc();
    void stop_bg_proc();
    bool isLocked();
    void unlock();
    void lock();
    int mfy();
private:
    void connectSignals(gLabel*);
    bool canToggleFavs();
    bool canToggleShares();
    int lastscr,mfcorr;
    bool isTakingToCarousel();
    bool locked,lasttoggl;
    void setContainer();
    QFont txt_font,tim_font;
    int fl,lh,lw,_lw,_lw2,lm,vtm,htm;
    bool is_expanded,gen_busy;
    QString context;
    QFuture<void> future,future1;
    bool fav_mode;
    QTimer ptimer;
    QWidget* director;
    bool debg;
    QList<QPointer<gLabel>> vb,fib,fsb,fvb,msb,foo;
    int wdth,scrr,scr;
    QPointer<QOpenGLWidget> marker,marker_first,marker_middle,marker_last,marker_switch;
    Qt::Alignment tim_align;
    QPixmap splash_img,*ppi=nullptr,*ppt=nullptr;
    QPoint pos, prev_pos, touch_pos;
    QVector2D diff,move;
    int tail_offs;
    void append_vscroll(int);
    void get_swipe_predicter();
    QList<int> q_vscroll;
    bool no_click;
    int get_vscroll(bool e=true);
    enum swipeOrientations{
        not_set,
        horizontal,
        vertical
    } swipe_orient;
    QTimer timer;
signals:
    void shown();
    void ready_to_switch(int);
    void mouse_move(QPoint,int);
    void entered(int,int,int);
    void click_delete(int,int);
    void click(int,int);
    void add_fav_videos(int);
    void del_fav_videos(int,const QString&);
    void download(const QString&, const QString&, const int, bool);
    void hided();
    void get_poster(int,bool,const QString&,const QString&,const QString&,const QString&);
    void settled();
    void startProcessing();
    void delete_item(int,const QString&);
    void send_item(int);
    void go(const QString&,const QStringList&);
    void preparing_shutdown(int);
    void filter_done();
    void stopTimer();
    void startTimer();
    void do_poster();
    void closeMenu();

private slots:
    void poster_generator(int, bool,const QString&,const QString&,const QString&,const QString&,const QString&,int);
    void make_poster();
    void noticeShow();
    void put_poster(int);
    void handleEnter(int,int);
    void handleLeave();
    void timerEv(bool force=false);

public slots:
    void finish_deletion();
    void brick_remove(int);
    void filter(const QString&);
    void filter();
};

#endif // WIDGETGEN_H
