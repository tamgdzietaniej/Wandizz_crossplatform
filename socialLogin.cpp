#include "socialLogin.h"
#include "ui_webhelper.h"

socialLogin::socialLogin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::webhelper)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground,true);
    setAttribute(Qt::WA_NoSystemBackground,true);
    setStyleSheet("background:rgba(0,0,0,0);");
    ui->messFrame->hide();
    ui->b_close->hide();
    b_close_look=ui->b_close->styleSheet();
    b_again_look=ui->b_again->styleSheet();
    act_url="";
}
void socialLogin::createWebView(){
    w=new browser(ui->container);
    w->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    ui->lay->addWidget(w);
    w->setAttribute(Qt::WA_DeleteOnClose);
    QSize siz=size()-QSize(0,ui->b_close->height());
    w->setGeometry(QRect(QPoint(0,0),siz));
    w->setFixedSize(siz);
    context=w->engine()->rootContext();
    context->setContextProperty(QStringLiteral("w"),w->width());
    context->setContextProperty(QStringLiteral("h"),w->height());
    w->hide();
    w->setAttribute(Qt::WA_AlwaysStackOnTop);
}
void socialLogin::gotAnswer(QString u){
    qDebug()<<"!!!!!!! GOT ANSWER !!!!!!!!!!!!!!!!!!>"<<u<<"<";
    QJsonDocument doc(QJsonDocument::fromJson(u.toLatin1()));
    if(doc.isArray()){
        qDebug()<<"DOC IS ARR:"<<doc.array();
    }
    else if(doc.isObject()){
        QJsonObject ob(doc.object());
        QString status=ob.value("status").toString();
        if(status=="success"){
            QString fname=ob.value("fname").toString();
            QString lname=ob.value("lname").toString();
            QString gender=ob.value("gender").toString();
            QString email=ob.value("email").toString();
            QString token=ob.value("token").toString();
            qDebug()<<"DATA:"<<fname<<lname<<gender<<email;
            emit gotSocialData(fname,lname,gender,email,token);
            deleteLater();
        } else {
            if(ob.contains("error")){
                QString error=ob.value("error").toString();
                qDebug()<<"INSIDE";
                w->close();
                ui->messFrame->show();
                ui->labInfo->setText(error);
                ui->labWait->hide();
                ui->b_close->show();
                ui->container->lower();
                update();
                return;
            }
        }
    }
    if(u=="null"){
        qDebug()<<"U is null";
        {
            ui->messFrame->hide();
            w->show();
        }
    }
    ui->b_close->show();
}
void socialLogin::setUrl(QString url){
    qDebug()<<"WEBVIEW:GEO:"<<w->geometry();
    w->loadPage(url);
    act_url=url;
    QObject* obj=w->rootObject();
    connect(obj,SIGNAL(completed(QString)),this,SLOT(gotAnswer(QString)));
    ui->container->raise();
    w->show();
    w->updateGeometry();
    w->update();
    w->hide();

}
void socialLogin::resizeEvent(QResizeEvent *e){
    w->setGeometry(0,0,e->size().width(),e->size().height());
}
socialLogin::~socialLogin()
{
    delete ui;
}

void socialLogin::on_b_close_clicked()
{
    emit go("back");
}

void socialLogin::on_b_again_clicked()
{
    createWebView();
    ui->messFrame->hide();
    ui->labWait->show();
    ui->b_close->hide();
    setUrl(act_url);
}

void socialLogin::on_container_customContextMenuRequested(const QPoint &pos)
{

}

void socialLogin::on_b_close_pressed()
{
  //  ui->b_close->setStyleSheet(b_close_look+"background:black;");
}

void socialLogin::on_b_again_pressed()
{
    ui->b_again->setStyleSheet(b_again_look+"background:black;");
}

void socialLogin::on_b_again_released()
{
    ui->b_again->setStyleSheet(b_again_look);
}

void socialLogin::on_b_close_released()
{
    ui->b_close->setStyleSheet(b_close_look);
}
