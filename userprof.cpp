#include "userprof.h"
#include "ui_userprof2.h"
#include "navigation.h"

userProf::userProf(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::userProf),
    offline(false),
    titles_shown(false)
{
    ui->setupUi(this);
    QPoint bp=ui->titles_cnt->mapToGlobal(ui->b_videos->pos());
    bp.setX(width()*3/5);
    ui->titles_cnt->move(bp);
    setAttribute(Qt::WA_NoSystemBackground,false);
    setAttribute(Qt::WA_TranslucentBackground,true);
    ui->shadow->setParent(ui->verticalWidget);
    ui->shadow->hide();
    setAttribute(Qt::WA_DontShowOnScreen,true);
    //   //vibra=new //vibra();

}
QRect userProf::lab_sidebar_geo(){
    return ui->lab_sidebar->geometry();
}
void userProf::set_equal(){
    QPoint bv=mapToGlobal(ui->b_videos->geometry().topLeft());
    ui->titles_cnt->move(width()*0.75,bv.y());

}
userProf::~userProf()
{
    qDebug()<<"~userProf()";
    delete ui;
}
void userProf::set_mode(bool net,bool favs){
    ui->b_settings->setEnabled(!net);
    ui->b_video_recognition->setEnabled(!net);
    ui->b_favourites->setDisabled(!favs);
    offline=net;
}
void userProf::set_notif(int i){
    ui->titles_cnt->setVisible(true);
    QString tc=QString::number(i);
    ui->titles_cnt->setText(tc);
}
//void userProf::set_mess(int i){
//    if(i==0)ui->lab_mess_cnt->setVisible(false);
//   else {
//      ui->lab_mess_cnt->setVisible(true);
//       ui->lab_mess_cnt->setText(QString::number(i));
//   }
//}
void userProf::set_user(QString n){
    ui->lab_nick->setText(n);
}
void userProf::set_avatar(){
    QPixmap img(f_user_avatar);
    if(!img.isNull()){
        qDebug()<<"SETTING AVATAR PROFSET:"<<img.size();
        ui->lab_avatar->setScaledContents(true);
        ui->lab_avatar->clear();
        ui->lab_avatar->setPixmap(img);
        ui->lab_avatar->update();
    }
}

void userProf::on_b_favourites_clicked(){
    //vibra->vibr(1);
    emit go("favs",{});
}
void userProf::on_b_videos_clicked(){
    //vibra->vibr(1);
    emit go("videos",{});
}
void userProf::on_b_settings_clicked(){
    //vibra->vibr(2);
    if(!offline)
        emit go("profset",{});
}

void userProf::on_b_video_recognition_clicked(){
    //vibra->vibr(2);
    qDebug()<<"gosync";
    if(!offline)
        go("sync",{});
}
void userProf::corr(){
    ui->titles_cnt->move(ui->b_videos->geometry().right()-ui->titles_cnt->width(),ui->b_videos->height()/2-ui->titles_cnt->height()/2);
    ui->titles_cnt->raise();
    update();
}
void userProf::on_b_options_clicked(){
    qDebug()<<"GEO:"<<ui->b_videos->geometry()<<ui->titles_cnt->geometry();
    //vibra->vibr(1);
    emit go("menu",{});
}

void userProf::on_b_exit_clicked(){
    //vibra->vibr(1);
    emit go("exit_app",{});
}
void userProf::animateButtonPress(QObject* s){
    QPushButton* b=static_cast<QPushButton*>(s);
    QRect g=b->parentWidget()->geometry();
    g.setX(0);
    g.setY(g.y()+1);
    g.setWidth(width());
    ui->shadow->setGeometry(g);
    ui->shadow->show();
}
void userProf::animateButtonRel(){
    ui->shadow->hide();
}


void userProf::on_b_video_recognition_pressed()
{
    animateButtonPress(sender());
}

void userProf::on_b_favourites_pressed()
{
    animateButtonPress(sender());
}

void userProf::on_b_videos_pressed()
{
    animateButtonPress(sender());
}

void userProf::on_b_settings_pressed()
{
    animateButtonPress(sender());
}

void userProf::on_b_exit_pressed()
{
    animateButtonPress(sender());
}

void userProf::on_b_video_recognition_released()
{
    animateButtonRel();
}

void userProf::on_b_favourites_released()
{
    animateButtonRel();
}

void userProf::on_b_videos_released()
{
    animateButtonRel();
}

void userProf::on_b_settings_released()
{
    animateButtonRel();
}

void userProf::on_b_exit_released()
{
    animateButtonRel();
}
