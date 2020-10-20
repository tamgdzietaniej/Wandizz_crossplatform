#include "signin.h"
#include "ui_signin.h"

SignIn::SignIn(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SignIn)
{
    ui->setupUi(this);
    ui->b_exit->hide();
    ui->shadow->hide();
    toggle_auth(false);
    ui->logo_facebook->setParent(ui->anframe);
    ui->logo_twitter->setParent(ui->anframe);
    ui->logo_google->setParent(ui->anframe);
    ui->logo_facebook->lower();
    ui->logo_google->lower();
    ui->logo_twitter->lower();
    ui->shadow->setParent(ui->anframe);
}
void SignIn::shSs(){;
    ui->logo_mid->raise();
    ui->logo_mid->show();
}
void SignIn::toggle_auth(bool toggle){
    ui->verlabel->setText("ver:"+QString::number(version));
    ui->anframe->setFixedHeight(ui->b_signin->geometry().bottom());
    ui->anframe->setEnabled(toggle);
    ui->anframe->setVisible(toggle);
    ui->anframe->move(ui->anframe->x(),height()-ui->anframe->height());
    ui->logo_facebook->setGeometry(ui->b_facebook->x()+30,ui->b_facebook->y()+5,ui->b_facebook->height()-10,ui->b_facebook->height()-10);
    ui->logo_twitter->setGeometry(ui->b_twitter->x()+30,ui->b_twitter->y()+5,ui->b_twitter->height()-10,ui->b_twitter->height()-10);
   // ui->logo_google->setGeometry(ui->b_google->x()+30,ui->b_google->y()+5,ui->b_google->height()-10,ui->b_google->height()-10);
    shSs();

}
SignIn::~SignIn()
{
    qDebug()<<"SignIn::~SignIn()";
    delete ui;
}
void SignIn::set_ver(QString ver){
    ui->version->setText("ver:"+ver);
}

void SignIn::on_b_signup_clicked()
{
    emit go("userprof",{});
}
void SignIn::on_b_signin_clicked()
{
    emit go("accrec",{});
}

void SignIn::on_b_exit_clicked()
{
    qApp->quit();
}
void SignIn::on_b_facebook_clicked()
{
     emit go("loginFB",{"login"});
}

void SignIn::on_b_twitter_clicked()
{
    emit go("loginTT",{"login"});
}
void SignIn::on_b_create_acc_clicked()
{
    emit go("profset",{});
}

void SignIn::on_b_twitter_pressed()
{
    animateButton(ui->b_twitter);
}
void SignIn::animateButton(QPushButton *b,bool forw){
    return;
    if(forw){
        ui->shadow->setGeometry(b->geometry().adjusted(0,0,0,-devicePixelRatio()*2));
        ui->shadow->show();
    } else
      ui->shadow->hide();
}
void SignIn::on_b_twitter_released()
{
    animateButton(ui->b_twitter,false);
}

void SignIn::on_b_facebook_pressed()
{
    animateButton(ui->b_facebook);
}

void SignIn::on_b_facebook_released()
{
    animateButton(ui->b_facebook,false);
}

void SignIn::on_b_create_acc_pressed()
{
    animateButton(ui->b_create_acc);
}

void SignIn::on_b_create_acc_released()
{
    animateButton(ui->b_create_acc,false);
}

void SignIn::on_b_signup_pressed()
{
    animateButton(ui->b_signup,true);
}

void SignIn::on_b_signin_pressed()
{
     animateButton(ui->b_signin,true);
}

void SignIn::on_b_signup_released()
{
     animateButton(ui->b_signup,false);
}

void SignIn::on_b_signin_released()
{
     animateButton(ui->b_signin,false);
}
