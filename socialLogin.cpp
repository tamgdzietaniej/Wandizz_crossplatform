#include "socialLogin.h"
#include "ui_webhelper.h"

socialLogin::socialLogin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::webhelper)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setAttribute(Qt::WA_TranslucentBackground,true);
 //   setAttribute(Qt::WA_NoSystemBackground,true);

 //   setAttribute(Qt::WA_OpaquePaintEvent);
    setStyleSheet("background:rgba(0,0,0,0);color:rgba(255,255,255,255);");
    portal=FACEBOOK;
}
void socialLogin::setAppleLogin(){
    portal=APPLEID;
}
void socialLogin::setFacebookLogin(){
    portal=FACEBOOK;
}
void socialLogin::createWebView(){
    viewer=new QQuickView();
    viewer->setResizeMode(QQuickView::SizeRootObjectToView);
    viewer->setGeometry(QRect(QPoint(0,0),size()));
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setAlphaBufferSize(8);
    viewer->setFormat(surfaceFormat);
    viewer->setClearBeforeRendering(true);
    viewer->setColor(QColor(Qt::transparent));

    /* CONTAINER FOR OPENGL WEBVIEW */
    QWidget *container = QWidget::createWindowContainer(viewer);
    container->setMinimumSize(size());
    container->setMaximumSize(size());
    container->setFocusPolicy(Qt::TabFocus);
    ui->container->layout()->addWidget(container);
    ui->container->raise();
    /* LOOK'n'FEEL */
    viewer->setDefaultAlphaBuffer(true);
    viewer->setColor(QColor(0,0,0,0));

    /* SETUP */
    context=viewer->engine()->rootContext();
    context=viewer->engine()->rootContext();
    context->setContextProperty(QStringLiteral("w"),viewer->width());
    context->setContextProperty(QStringLiteral("h"),viewer->height());
    ui->container->hide();
}
void socialLogin::gotAnswer(QString u){
    ui->container->hide();
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
                return;
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
                if(verified=="true"){
                    emit gotAppleId(email);
                    return;
                }
                else show_error("Apple says: Your email address cannot be authorized. Check AppleID");
            } else {
                if(ob.contains("error")){
                    show_error(ob.value("error").toString());
                    emit go("back",{});
                }
            }
        }
    }
    if(u=="null"){
        qDebug()<<"U is null"<<u;
        {
            ui->container->show();
        }
    }
}
void socialLogin::show_error(QString error){
    qDebug()<<"ERROR:"<<error;
    QMessageBox::information(this,"","error");
    viewer->close();
    ui->container->lower();
    update();
    return;
}
void socialLogin::setUrl(QString url){
    context->setContextProperty(QStringLiteral("ref"),url);
    viewer->setSource(QUrl("qrc:/main.qml"));;
    QObject* obj=viewer->rootObject();
    connect(obj,SIGNAL(completed(QString)),this,SLOT(gotAnswer(QString)));
    connect(obj,SIGNAL(undef()),ui->container,SLOT(show()));
}
void socialLogin::resizeEvent(QResizeEvent *e){
    viewer->setGeometry(0,0,e->size().width(),e->size().height());
}
socialLogin::~socialLogin()
{
    delete ui;
}



