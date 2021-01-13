#ifndef FAVITEMS_H
#define FAVITEMS_H

#include "gllabel.h"
#include "widget_wrapper.h"
#include "topmenuswitcher.h"
#include <QShowEvent>
#include <widgetgen.h>

namespace Ui {
class favItems;
}

class favItems : public QMainWindow
{
    Q_OBJECT


public:
    explicit favItems(widgetGen*, QWidget *parent = nullptr);
    ~favItems() override;
    bool debg;
    widgetGen* wgen;
    QString context;
    void init();
    void reinit();
    void reinit(QString,bool);
    void set_nick(QString);
    void create_prospect_list(int[],QJsonArray*,QJsonArray*);
    void create_videos_list(int[],const QJsonArray*,const QJsonArray*);
    void create_scenes_list(const QJsonArray*);
    void create_items_list(const QJsonArray*);
    void create_fvideos_list(int[],const QJsonArray*);
  void set_favs_cnt(int);
private:
    qreal dpi;
    QString nick;
    bool offline,search_active,prev_es;
    enum swipeOrientations{
        not_set,
        horizontal,
        vertical
    } swipe_orient;

    Ui::favItems *ui;
    QOpenGLWidget marker,marker_first,marker_middle,marker_last,marker_switch;
    QFuture<void> future;
    void append_vscroll(int);
    void get_swipe_predicter();
    void setMarkers();
    void toggleSelectors(bool);
    void updateUi();
    bool setContext(QString);
private slots:
    void on_b_options_clicked();
    void on_b_back_clicked();
    void on_b_items_clicked();
    void on_b_scenes_clicked();
    void on_b_videos_clicked();

    void on_b_options_2_clicked();

    void on_b_back_2_clicked();

    void on_b_items_2_clicked();

    void on_b_scenes_2_clicked();

    void on_b_videos_2_clicked();

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void hideEvent(QHideEvent*) override;

signals:
    void add_fav_videos(int);
    void delete_item(int,QString);
    void go(QString,QStringList);
    void show_menu(QPoint);
    void download(const QString&,const QString&, int);

};

#endif // FAVITEMS_H
