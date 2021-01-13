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
    bool fresh_run;
    QFuture<void> future;
    bool isLocked();
    bool isProducer();
    void show_top_widget();
    void set_params(int index,int db_index,int items_cnt,int title_id,QString poster_file,QString poster_url,QString txt_up,QString txt_dn,QString mediafile="",int oid=-1);
    void set_items();
    void set_alignment();
    bool mask=false,hiding;
    QPixmap* limage=nullptr;
    QString filename,fileurl;
    int title_id,oid;
    bool loaded;
    QPixmap *fav[2],*sha_pix=nullptr;
    int offs;
    bool is_poster,debg;
    QString type;
    int items_cnt;
    void setContext(const QString&);
    QOpenGLWidget* mfy;
    QTimer timer;
bool need_switch,need_hide,need_update;
    bool chkOffset();
    QString netfile;
    void setDPI(int);
    qreal des_opacity;
    int hcatapult;
    int index;
    void setOpacity(qreal o=1);
    int db_index=-1,pixw;
    //  qreal px,py,pw,ph;
    bool fav_toggled=false;
    QRect add_fav_rect;
    QString tim_text,txt_text;
    bool event_owner=false,locked;
    void set_prospect_params(Prospect&);
    void catch_event();
    int dpi;
    void lock(bool h=true);
    void unlock();
    void paint(QPainter *painter);
private:
    QString ctype;
    Prospect* par;
    bool sent1,sent2;
    QSizePolicy size_policy;
    void map_items();
    bool pressed=false;
    int pos,prev_pos,press_pos;
    bool need_show,anim_active;
    int diff;
    int stim=400;
    bool final_poster;
public slots:
    bool toggle_fav();
    void check_timer();
    void mark_fav_videos(bool);
    void unmark_fav_videos(bool);
    void settle_poster(QPixmap*,bool);
    void toggle(bool);

protected:
    void paintGL() override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void leaveEvent(QEvent*) override;
    void resizeEvent(QResizeEvent *e) override;
    void hideEvent(QHideEvent* e) override;

signals:
    void shown();
    void hover(int,int);
    void ready_to_switch(int);
    void mouse_move(QPoint,int);
    void entered(int,int,int);
    void click_delete(int);
    void click(int,int);
    void add_fav_videos(int);
    void del_fav_videos(int,QString);
    void download(const QString&,const QString&,const int,bool);
    void leaved();
    void hided();
    void get_poster(int,bool,const QString&,const QString&,const QString&,const QString&,const QString&,int);
    void settled();
    void chk_timer();
    void brick_hidden_done(int);
};
#endif // GLLABEL_H
