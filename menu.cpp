#include "menu.h"
#include "ui_menu.h"
#include "QFocusEvent"

menu::menu(QWidget *parent) :
    QDialog (parent),
    ui(new Ui::menu),
    choosed(false),
    is_access(true),
    is_context(true)

{
    ui->setupUi(this);
    setObjectName("menu");
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose,true);
    setAttribute(Qt::WA_AlwaysStackOnTop);
    showing=true;
    hidding=false;
    setFocus();
    opac=0;
    QList<QPushButton*> tw=ui->bar->findChildren<QPushButton*>();
    for(int i=0;i<tw.count();i++)
        connect(tw.at(i),SIGNAL(clicked()),this,SLOT(on_select()));
    set_opt_geo();
}
void menu::setContext(bool c){
    is_context=c;
    set_access(c);
}
void menu::set_access(bool a){
    is_access=a;
    qDebug()<<"NGENisaccess:"<<a;
    QList<QPushButton*> tw=ui->bar->findChildren<QPushButton*>();
    for(int i=0;i<tw.count();i++){
        tw.at(i)->setEnabled(is_access);
        qDebug()<<"MENU:"<<tw.at(i)->objectName()<<tw.at(i)->isEnabled();
    }
    update();
   // qDebug()<<"MENU:"<<a;
}
void menu::deactivate(QString s,QStringList ss){
    if(!is_context)return;
    if(ss.count()>0){
        if(ss.contains("search")){
            s.prepend("s_");
        }
    }
    qDebug()<<"NGEN:MENU:"<<s;
    QList<QPushButton*> tw=ui->bar->findChildren<QPushButton*>();
    for(int i=0;i<tw.count();i++)
        tw.at(i)->setDisabled(s==tw.at(i)->objectName());
    update();
    if(ss.contains("both"))deactivate(s,{"search"});
}
void menu::set_opt_geo(){
    int x=geometry().right()-60*(QApplication::primaryScreen()->geometry().width()/400);
    int y=20*(QApplication::primaryScreen()->geometry().height()/710)-mapFromGlobal(QPoint(0,0)).y();
    ui->options->move(x,y);
    qDebug()<<"MOVE:"<<x,y;
}
void menu::getGeo(){
    qDebug()<<"getgeo"<<geometry()<<ui->frame->geometry()<<ui->bar->geometry();
}
menu::~menu()
{
    delete ui;
}

void menu::resizeEvent(QResizeEvent *e){
    ui->frame->setFixedSize(e->size());
    ui->bar->setFixedSize(e->size());
}
void menu::on_select()
{
    QStringList ons({});
    QString on=sender()->objectName();
    if(on.left(2)=="s_")
        on=on.mid(2);
    ons.append("search");
    emit go(on,ons);
}
