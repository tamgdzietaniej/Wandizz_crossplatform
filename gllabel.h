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
    QString event_to_share;
    void setIndex(int);
    bool fresh_run;
    QFuture<void> future;
    bool isLocked();
    void lock();
    void unlock();
    bool isProducerType();
    bool isProducer();
    bool isVideosType();
    bool isVideos();
    bool isFItems();
    bool isFVideos();
    bool isFScenes();
    void show_top_widget();
    void set_params(int index,int db_index,int items_cnt,int title_id,QString poster_file,QString poster_url,QString txt_up,QString txt_dn,QImage&,QString mediafile="",int oid=-1);
    void set_items();
    void set_alignment();
    bool mask=false,hiding;
    QImage* limage=nullptr;
    QImage* timage=nullptr;
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
    void setDPI(int);
    qreal des_opacity;
    int hcatapult;
    int index;
    void setOpacity(qreal o=1);
    int db_index=-1,pixw;
    //  qreal px,py,pw,ph;
    int fav_toggled;
    QRect add_fav_rect;
    QString tim_text,txt_text;
    bool event_owner=false,locked;
    void set_prospect_params(Prospect&);
    void catch_event();
    int dpi;
    QString ctype;
    bool toggle_fav(bool sql);
    void forceUpdate();
private:
    Prospect* par;
    bool sent1,sent2;
    QSizePolicy size_policy;
    bool pressed;
    int pos,prev_pos,press_pos;
    bool anim_active;
    int diff;

public slots:
    void check_timer();
    void settle_poster(const QImage*);

protected:
    void initializeGL() override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void leaveEvent(QEvent*) override;
    void resizeEvent(QResizeEvent *e) override;
    void hideEvent(QHideEvent* e) override;
    void showEvent(QShowEvent* e) override;

signals:
    void shown();
    void hover(int,int);
    void ready_to_switch(int);
    void mouse_move(QPoint,int);
    void entered(int,int,int);
    void click(int,int);
    void toggle_fav_video(int,bool);
    void del_fav(const QString&,int);
    void download(const QString&,const QString&,const int,bool);
    void leaved();
    void hided();
    void get_poster(int,const QString&,const QString&,const QString&,const QString&,const QString&,int);
    void settled(int);
    void chk_timer();
};
#endif // GLLABEL_H
