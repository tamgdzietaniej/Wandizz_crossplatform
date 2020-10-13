#ifndef GLLABEL_H
#define GLLABEL_H

#include "globals.h"
#include <QOpenGLPaintDevice>
#include <QOpenGLContext>


class gLabel : public QOpenGLWidget{
    Q_OBJECT
public:
    explicit gLabel(QWidget *parent = nullptr,QString o="-");
    ~gLabel() override;
    bool sharing;
    void create_item_brick(int,QString,QString,QString,QString,QString,QString);
    void create_scene_brick(int,QString,QString,QString,QString,QString);
    void upper_text(QString);
    void lower_text(QString);
    void updGeo();
    qreal lh3,op_sc;
    QOpenGLPaintDevice pdev;
    bool emitted_once;
    void set_hoffset(int);
    bool scroll_need;
    void show_shares();
    QString on;
    qreal rcol=0;
    QPixmap* favpix;
    QRect actRect(int i=-1,qreal scale=1.0);
    void set_style();
    QSize main_s;
    void show_top_widget();
    QOpenGLWidget* marker_first;
    void set_params(int,int,qreal,int,QString,QString,QString,QString,QOpenGLWidget*,QString nf="" );
    void set_urlimage(QString);
    void set_items();
    QFlags<Qt::AlignmentFlag>  tim_align;
    void set_alignment();
    bool mask=false,hiding;
    QPixmap* lpixmap;
    QString left_borders,filename,fileurl,oid;
    qreal global_hscroll;
    qreal mainh;
    bool prep_remove;
    int mrg;
    bool loaded;
    QPixmap *fav_off=nullptr,*fav_on=nullptr,*sha_pix=nullptr,*del_pix=nullptr;
    QString txt_color,tim_color,nbi,nb;
    int offs;
    bool is_poster,debg;
    QString type;
    int ms;
    QTimer timer;
    qreal mrgh;
    bool need_switch,need_hide,need_update;
    bool sh_del,sh_sha;
    bool chkOffset();
    qreal _prop;
    qreal _lw;
    QString netfile;
    int trip;
    QPen pen;
    int px,py,pw,ph;
    QRect b_share_rect,b_delete_rect,b_back_rect;
    QStringList where_to_share_list;
    QFont font;
    QIcon ic;
    QLabel* poster;
    QSizePolicy sizePolicy;
    int hdur;
    int sdur;
    qreal des_opacity;
    int hcatapult;
    QString item_file;
    bool to_hide=true;
    int w,index,brick_act_shift;
    void setOpacity(qreal o=1);
    int db_index=-1,pixw;
    //  qreal px,py,pw,ph;
    int nv=0;
    qreal main_h;
    qreal lh,lw,lm;
    bool time_to_delete;
    void set_poster(const QString,bool);
    Qt::AspectRatioMode aspect_ratio;
    QRect anrect,anrect_smaller,anrect_bigger,anrect_hor,anrect_ver;
    bool inits;
    bool fav_toggled=false;
    QRect txt_rect;
    QRect tim_rect;
    QRect add_fav_rect,del_fav_rect,add_fav_anim_rect;
    void init();
    QString tim_text,txt_text;
    QFont txt_font,tim_font;
    qreal m;
    QRect picrect;
    QLabel* lpix;
    bool to_delete,anim_needed;
    bool anim_act;
    QString item;
    bool event_owner=false,locked;
    void catch_event();
    int dpi;
    void lock();
    QRect dest_rect;
    void unlock();
    bool acquiring;
    bool under;
    void set_fav_videos(bool);
QImage opimg;
private:
    bool sent1,sent2;
    QSizePolicy size_policy;
    void map_items();
    qreal divider;
    bool pressed=false;
    bool timer_proc;
    qreal qms;
    int pos,prev_pos,press_pos;
    bool need_show,anim_active;
    int diff;
    int stim=400;
    bool final_poster;
    int anset;
    void setAnimRect();
public slots:
    bool toggle_fav();
    void check_timer();
    void mark_fav_videos(bool a=false);
    void unmark_fav_videos(bool a=false);
    void settle_poster(const QImage&,bool);

protected:
    void paintGL() override;
    void initializeGL() override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void leaveEvent(QEvent*) override;
    void resizeEvent(QResizeEvent *e) override;
    void showEvent(QShowEvent* e) override;
    void hideEvent(QHideEvent* e) override;

signals:
    void shown();
    void hover(int,int);
    void ready_to_switch(int);
    void mouse_move(QPoint,int);
    void entered(int,int,int);
    void click_delete(int,int);
    void click(int,int);
    void add_fav_videos(int);
    void del_fav_videos(int,QString);
    void download(QString,QString);
    void leaved();
    void hided();
    void get_poster(int,bool,QString,QString,QString,QString);
    void settled();
};
#endif // GLLABEL_H
