#ifndef SIGNIN_H
#define SIGNIN_H
#include "globals.h"
#include <QPalette>

namespace Ui {
class SignIn;
}
class geometry;
class SignIn : public QMainWindow
{
    Q_OBJECT
public:
    explicit SignIn(QWidget *parent = nullptr);
    ~SignIn() override;

private:
    Ui::SignIn *ui;
    void animateButton(QPushButton*,bool forw=true);
signals :
    void go(QString,QStringList);
private slots:
    void on_b_signup_clicked();
    void on_b_signin_clicked();
    void on_b_facebook_clicked();
    void on_b_twitter_clicked();
    void on_b_create_acc_clicked();
    void on_b_twitter_pressed();
    void on_b_twitter_released();
    void on_b_facebook_pressed();
    void on_b_facebook_released();
    void on_b_create_acc_pressed();
    void on_b_create_acc_released();

    void on_b_signup_pressed();

    void on_b_signin_pressed();

    void on_b_signup_released();

    void on_b_signin_released();

public slots:
    void set_ver(QString ver);
    void on_b_exit_clicked();

public:
    bool dark=false;
    void shSs();
    QString button_released_look="border: 1px solid #ffffff;color:white;border-radius:0.5em;background-color:transparent;border-image:none;";
    QString button_pressed_look= "border: 1px solid #000000;color:white;border-radius:0.5em;background-color:black;border-image:none;";
    QString s_rel1="border: 1px solid rgba(0,0,0,100);color:#fff;border-radius:0.5em;"
                     "background:black;"
                     "border-image:none;";
    QString s_pre1="background:qlineargradient(spread:pad, x1:0.773626, y1:0.017, x2:0.812808, y2:1, stop:0 rgba(150,0,0,20), stop:1 rgba(133,55,151,154));\nborder: 1px solid rgba(140,0,0,100);\ncolor:#000;\nborder-radius:0.5em;\nborder-image:none;";
    QString button_released_look_no_border= "border:none;color:white;background-color:transparent;border-image:none;";
    void toggle_auth(bool);
};
#endif


