#include "menu.h"
#include "ui_menu.h"
#include "QFocusEvent"

menu::menu(QWidget *parent) :
    QDialog (parent),
    ui(new Ui::menu),
    choosed(false),
    is_access(true)
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

void menu::set_access(bool a){
    is_access=a;
    ui->m_fav_items->setEnabled(a);
    ui->m_fav_scenes->setEnabled(a);
    ui->m_fav_items->setEnabled(a);
    ui->m_fav_videos->setEnabled(a);
    ui->m_gohome->setEnabled(a);
    ui->m_prof_settings->setEnabled(a);
    ui->m_search_scenes->setEnabled(a);
    ui->m_search_items->setEnabled(a);
    ui->m_search_videos->setEnabled(a);
    ui->m_prospect->setEnabled(a);
    ui->m_videorec->setEnabled(a);
    ui->m_videos->setEnabled(a);
    ui->m_share->setEnabled(a);
    qDebug()<<"MENU:"<<a;
}
void menu::deactivate(QString s){
    if(!is_access)return;
    qDebug()<<"Deactivate:"<<s;
    ui->m_videorec->setDisabled(s=="sync");
    ui->m_fav_items->setDisabled(s=="fav_items");
    ui->m_fav_scenes->setDisabled(s=="fav_scenes");
    ui->m_fav_videos->setDisabled(s=="fav_videos");
    ui->m_videos->setDisabled(s=="videos");
    ui->m_prof_settings->setDisabled(s=="profsettings");
    ui->m_gohome->setDisabled(s=="userprof");
    ui->m_prospect->setDisabled(s=="prospect");
    update();
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

void menu::on_m_search_scenes_clicked()
{
    hide();
    emit go("s_fav_scenes");
}

void menu::on_m_prospect_clicked()
{
    emit go("prospect");
}
