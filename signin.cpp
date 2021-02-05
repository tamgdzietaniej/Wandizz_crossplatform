#include "signin.h"
#include "ui_signin.h"

SignIn::SignIn(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SignIn)
{
    ui->setupUi(this);
    init();
}
void SignIn::init(){
    toggle_auth(false);
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
    ui->verlabel->setText("ver:"+version);
    //  ui->anframe->move(0,height()-ui->anframe->height());
    ui->anframe->setEnabled(toggle);
    ui->anframe->setVisible(toggle);
    //  ui->anframe->move(ui->anframe->x(),height()-ui->anframe->height());
    // ui->logo_google->setGeometry(ui->b_google->x()+30,ui->b_google->y()+5,ui->b_google->height()-10,ui->b_google->height()-10);
    //  shSs();

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
    emit go("profset",{"accrec"});
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


