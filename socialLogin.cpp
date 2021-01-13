#include "socialLogin.h"
#include "ui_webhelper.h"

socialLogin::socialLogin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::webhelper)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setAttribute(Qt::WA_TranslucentBackground,true);
    setAttribute(Qt::WA_NoSystemBackground,true);
    setStyleSheet("background:rgba(0,0,0,0);");
    act_url="";
    portal=FACEBOOK;
}
void socialLogin::setAppleLogin(){
    portal=APPLEID;
}
void socialLogin::setFacebookLogin(){
    portal=FACEBOOK;
}
void socialLogin::createWebView(){
    w=new browser(ui->container);
    w->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    ui->lay->addWidget(w);
    w->setAttribute(Qt::WA_DeleteOnClose);
    QSize siz=size();
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
    if(portal == FACEBOOK){
        if(doc.isObject()){
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
                    show_error(ob.value("error").toString());
                    emit go("back",{});
                }
            }
        }
    } else if(portal == APPLEID){
        if(doc.isObject()){
            QJsonObject ob(doc.object());
            QString email_key="email";
            if(ob.keys().contains(email_key)){
                QString email=ob.value("email").toString();
                QString verified=ob.value("email_verified").toString();
                qDebug()<<"DATA:"<<email<<verified;
                if(verified=="true")
                    emit gotAppleId(email);
                else show_error("Apple ID says Your email address cannot be authorized. Check AppleID");
                deleteLater();
            } else {
                if(ob.contains("error")){
                    show_error(ob.value("error").toString());
                }
            }
        }
    }
    if(u=="null"){
        qDebug()<<"U is null";
        {
            w->show();
        }
    }
}
void socialLogin::show_error(QString error){
    qDebug()<<"ERROR:"<<error;
    w->close();
    ui->container->lower();
    update();
    return;
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



