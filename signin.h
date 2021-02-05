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
    void init();
private:
    Ui::SignIn *ui;

private slots:
    void on_b_signup_clicked();
    void on_b_signin_clicked();
    void on_b_facebook_clicked();
    void on_b_twitter_clicked();
    void on_b_create_acc_clicked();

public slots:
    void set_ver(QString ver);
    void on_b_exit_clicked();

public:
    bool dark=false;
    void shSs();
    void toggle_auth(bool);
signals:
    void go(QString,QStringList);
};
#endif


