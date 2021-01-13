#ifndef USERPROF_H
#define USERPROF_H
#include "globals.h"
#include <QInputMethod>
namespace Ui {
class userProf;
}

class userProf : public QMainWindow
{
    Q_OBJECT

public:
    explicit userProf(QWidget *parent = nullptr);
    ~userProf();
    void set_user(QString);
    void set_notif(int);
    void set_avatar();
    void set_mode(bool,bool);
    void set_equal();
#if defined(Q_OS_ANDROID)
    QString path=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/WANDIZZ";
#else
    QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/WANDIZZ";
#endif
    QLabel* lab_sb;
    QRect lab_sidebar_geo();
    bool offline;
    //   void set_mess(int);
    QWidget* aw;
    void animateButtonPress(QObject*);
    void animateButtonRel();
private:
    Ui::userProf *ui;
    bool titles_shown=false;
signals:
    void go(QString,QStringList);
    void show_menu(QPoint);
    void exit_app();

private slots:
    void on_b_favourites_clicked();
    void on_b_settings_clicked();
    void on_b_video_recognition_clicked();
    void on_b_videos_clicked();
    void on_b_options_clicked();
    void on_b_exit_clicked();
    void on_b_search_clicked();
    void on_b_video_recognition_pressed();
    void on_b_favourites_pressed();
    void on_b_videos_pressed();
    void on_b_search_pressed();
    void on_b_settings_pressed();
    void on_b_exit_pressed();
    void on_b_video_recognition_released();
    void on_b_favourites_released();
    void on_b_videos_released();
    void on_b_search_released();
    void on_b_settings_released();
    void on_b_exit_released();
};
#endif // USERPROF_H
