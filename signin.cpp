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
    ui->shadow->setParent(ui->anframe);
#if defined (Q_OS_IOS)
    ui->b_twitter->setText("Sign with Apple");
#else
    ui->b_twitter->setText("Sign with Google");
#endif

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
    emit go("userprof",{"silent"});
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
#if defined(Q_OS_IOS)
    emit go("loginApple",{});
#else
    emit go("loginGoogle",{});
#endif
    // emit go("loginTT",{"login"});
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
