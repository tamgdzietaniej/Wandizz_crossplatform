#include "menu.h"
#include "ui_menu.h"
#include "QFocusEvent"

menu::menu(QWidget *parent) :
    QDialog (parent),
    ui(new Ui::menu),
    choosed(false)
{
    ui->setupUi(this);
    setObjectName("menu");
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_AlwaysStackOnTop);
    showing=true;
    hidding=false;
    setFocus();
    opac=0;
}
/*
void menu::paintEvent(QPaintEvent *){
    QPainter painter(this);
    opac+=.1*(showing && opac<1.0)-.1*(hidding && opac>0.0);
    painter.end();
    setWindowOpacity(opac);
    if(showing || hidding){
    if(opac>1.0){
            opac=1;
            showing=false;
        }
        if(opac<=0.0){
            opac=0.0;
            hidding=false;
            deleteLater();
        }
        if(opac!=1.0 && opac !=0.0)qDebug()<<"SETTING MENU OPAC"<<opac;
            update();
    }
}
*/
//void menu::focusOutEvent(QFocusEvent*e){
//    emit menu_on(true);
//}
void menu::deactivate(QString s){
    qDebug()<<"Deactivate:"<<s;
    ui->m_videorec->setDisabled(s=="sync");
    ui->m_fav_items->setDisabled(s=="fav_items");
    ui->m_fav_scenes->setDisabled(s=="fav_scenes");
    ui->m_fav_videos->setDisabled(s=="fav_videos");
    ui->m_videos->setDisabled(s=="videos");
    ui->m_prof_settings->setDisabled(s=="profsettings");
    ui->m_gohome->setDisabled(s=="userprof");
}
void menu::on_m_exit_clicked()
{
    emit exit_app();
}
void menu::on_m_fav_videos_clicked()
{
    hide();
    ////vibr.//vibr(1);
    //vibr.//vibr(1);
    emit go("fav_videos");

}
void menu::on_m_fav_scenes_clicked()
{
    hide();
    //vibr.//vibr(1);
    emit go("fav_scenes");
}
void menu::on_m_fav_items_clicked()
{
    hide();
    //vibr.//vibr(1);
    emit go("fav_items");
}

void menu::on_m_search_videos_clicked()
{
    hide();
    //vibr.//vibr(1);
    emit go("s_videos");
}
void menu::on_m_prof_settings_clicked()
{
    hide();
    //vibr.//vibr(1);
    emit go("profset");
}
void menu::on_m_videorec_clicked()
{
    hide();
    //vibr.//vibr(1);
    emit go("sync");
}
void menu::on_m_gohome_clicked()
{
    hide();
    //vibr.//vibr(1);
    emit go("userprof");
}
void menu::set_opt_geo(QPoint r){
    //vibr.//vibr(1);
    ui->options->move(mapFromGlobal(r));

}
void menu::on_options_clicked()
{
    hide();
}
void menu::enable(){
    ui->m_fav_scenes->setDisabled(false);
    ui->m_fav_items->setDisabled(false);
    ui->m_fav_videos->setDisabled(false);
    //  ui->m_search_items->setDisabled(false);
    ui->m_videorec->setDisabled(false);
    ui->m_prof_settings->setDisabled(false);
    ui->m_search_videos->setDisabled(false);
}
void menu::disable(QString t){
    if(t=="fav_scenes")ui->m_fav_scenes->setDisabled(true);
    if(t=="fav_items")ui->m_fav_items->setDisabled(true);
    if(t=="fav_videos")ui->m_fav_videos->setDisabled(true);
    //  if(t=="videos")ui->m_search_items->setDisabled(true);
    if(t=="sync")ui->m_videorec->setDisabled(true);
    if(t=="profsettings")ui->m_prof_settings->setDisabled(true);
    if(t=="cinema")ui->m_search_videos->setDisabled(true);
}
menu::~menu()
{
    delete ui;
}

void menu::resizeEvent(QResizeEvent *e){
    ui->frame->setFixedSize(e->size());
    ui->bar->setFixedSize(e->size());
}

void menu::on_m_search_items_clicked()
{
    hide();
    ////vibr.////vibr(1);
    emit go("s_fav_items");
}

void menu::on_m_share_clicked()
{
    hide();
    emit go("share");
}

void menu::on_m_videos_clicked()
{
    hide();
    emit go("videos");
}
