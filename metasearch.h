#ifndef METASEARCH_H
#define METASEARCH_H

#include "gllabel.h"
#include "widget_wrapper.h"
#include "topmenuswitcher.h"
#include <QtConcurrent/QtConcurrentRun>
#include <QShowEvent>
#include <widgetgen.h>


namespace Ui {
class metasearch;
}

class metasearch : public QMainWindow
{
    Q_OBJECT

public:
    metasearch(widgetGen* ,QWidget *parent = nullptr);
    ~metasearch() override;
    bool debg;
    widgetGen* wgen;
    bool setContext(QString);
    void init();
    void create_prospect_list(int[],QJsonArray*,QJsonArray*);
    void reinit( bool );
    QString getContext();

private:
    Ui::metasearch *ui;
    qreal dpi;
    bool offline;
    QOpenGLWidget marker,marker_first,marker_middle,marker_last,marker_switch;
    QFuture<void> future;
    QInputMethod* input=QApplication::inputMethod();
    void hide_temps();
    void setMarkers();
    void set_fav_videos_list(const QJsonArray*);
    void setGenerator();

private slots:
    void on_b_options_2_clicked();
    void on_b_options_clicked();
    void on_b_back_clicked();
    void on_b_back_2_clicked();
    void on_b_home_clicked();
    void on_b_filters_clicked();
    void on_b_sh_titles_stateChanged(int arg1);

    void on_b_sh_titles_clicked();

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
    void download(const QString&,const QString&, int, bool);
};

#endif // METASEARCH_H
