#ifndef GLLABEL_H
#define GLLABEL_H

#include "globals.h"
#include <QOpenGLPaintDevice>
#include <QOpenGLContext>


class gLabel : public QOpenGLWidget{
    Q_OBJECT
public:
    explicit gLabel(QWidget *parent = nullptr,QString o="",QString t="");
    ~gLabel() override;
    void create_item_brick(int,QString,QString,QString,QString,QString,QString);
    void create_scene_brick(int,QString,QString,QString,QString,QString);
    qreal op_sc;
    void revalidate();
    qreal rcol=0;
    void setGesture(bool);
    void movEvent(QMouseEvent*);
    void relEvent(QMouseEvent*);
    void setIndex(int);
    bool fresh_run;
    bool accept;
    bool need_lock;
    int *over_me_clicked,*act_oid,*act_title_id,*act_items_cnt;
    QRect mrrect;
    bool *click_update;
    QString event_to_share;
    gLabel* act_item;
    QString* act_type,*act_ctype,*act_event_to_share,*act_netfile;
    void setWatcher(QString& curr_type,QString& curr_ctype,QString& curr_event,QString& nf,int& oid,int& c_title_id,int& ind,int& c_it_cnt,bool& isnew);
    void reindex(int,QRect r=QRect());
    int brcorr,mrb;
    bool reindexing;
    bool need_unlock;
    double lsp;
    double rt;
    QFuture<void> future;
    bool isLocked();
    bool del;
    void lock();
    void unlock();
    bool isProducerType();
    bool need_show;
    bool isProducer();
    bool isVideosType();
    bool isVideos();
    bool isFItems();
    bool isFVideos();
    bool isFScenes();
    void show_top_widget();
    void set_params(int index,int db_index,int items_cnt,int title_id,QString poster_file,QString poster_url,QString txt_up,QString txt_dn,QImage*,QString mediafile="",int oid=-1);
    void set_items();
    void set_alignment();
    bool mask=false,hiding;
    QImage* limage=nullptr;
    QImage* timage=nullptr;
    void setScaled(bool);
    QString filename,fileurl;
    int title_id,oid;
    bool loaded;
    QPixmap *fav[2],*sha_pix=nullptr;
    int offs;
    void setSilent(int,const QString&,const QString&);
    bool is_poster,debg;
    QString type;
    int items_cnt;
    QOpenGLWidget* mfy;
    QTimer timer;
    bool need_update;
    bool chkOffset();
    QString netfile;
    bool is_gesture;
    void setDPI(int);
    QRect vrect,*pvrect;
  //  QGraphicsOpacityEffect *opacity;
 //   QPropertyAnimation* anim;
   // qreal des_opacity,sw_opacity;
    double hcatapult;
    int index;
    int db_index=-1,pixw;
    //  qreal px,py,pw,ph;
    Prospect* par;

    int fav_toggled;
    QRect add_fav_rect;
    QString tim_text,txt_text;
    bool propagate=false;
    void set_prospect_params(Prospect&);
    int dpi;
    QString ctype;
    bool toggle_fav(bool sql);
    void forceUpdate();
    int nde;
        QList<PostersReady>* poster_data_ready;
private:
    bool locked;
    double scale;
    bool sent1,sent2;
    QSizePolicy size_policy;
    bool pressed;
    int position,prev_position,press_position;
    void start_bg_proc(bool ns=false);
    bool anim_active;
    bool need_hide;
public slots:
    void endAnim();
    void check_timer();

protected:
    void initializeGL() override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent*) override;
   // void mouseReleaseEvent(QMouseEvent*) override;
   // void mouseMoveEvent(QMouseEvent*) override;
    void resizeEvent(QResizeEvent *e) override;
   // void hideEvent(QHideEvent* e) override;
  //  void showEvent(QShowEvent* e) override;

signals:
    void shown();
    void mpressed(gLabel*,QMouseEvent*);
    void mreleased(gLabel*,QMouseEvent*);
    void ready_to_switch(int);
    void mouse_move(QPoint,int);
    void click(int,int);
    void toggle_fav_video(int,bool);
    void del_fav(int,int,const QString);
    void download(const QString,const QString);
    void hided();
    void get_poster(int,const QString&,const QString&,const QString&,const QString&,const QString&,const QString&,int);
    void settled(int);
    void chk_timer();
};
#endif // GLLABEL_H
