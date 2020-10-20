//#putclude "mypassword.h"
#include "profsettings.h"
#include "ui_profsettings.h"
profSettings::profSettings(QWidget *parent) :
    QMainWindow(parent),
    recovery_mode(false),
    avatar_changed(false),
    upd_pass(""),
    ret_click(false),
    ui(new Ui::profSettings),act(""),
    validated(true),
    ready_to_rec(false),
    undo(false),rotated(1)
{
    ui->setupUi(this);
    ui->b_rotate->hide();
    keyboard = QApplication::inputMethod();
    QRegularExpression rx("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b",
                          QRegularExpression::CaseInsensitiveOption);
    ui->edit_email->setValidator(new QRegularExpressionValidator(rx, this));
    f.setPixelSize(18);
    dpi=devicePixelRatio();
    ui->ind_modified->hide();
    ui->edit_name->hide();
    ui->edit_pass->hide();
    ui->edit_email->hide();
    ui->edit_phone->hide();
    ui->b_undo_name->hide();
    ui->b_undo_pass->hide();
    ui->b_undo_email->hide();
    ui->b_undo_phone->hide();
    ess=ui->edit_email->text();
    ui->but_name->raise();
    ui->but_name->show();
    t=new QTimer(this);
    b_save_click=false;
    must_chk_email=false;
    must_correct_email=false;
    must_chk_cred=false;
    pressed=true;
    setAttribute(Qt::WA_DeleteOnClose);
    t->setSingleShot(true);
    qDebug()<<"UI:"<<ui->but_name->isVisible();
    ui->shadow->setParent(ui->verticalWidget);
    ui->b_linked->hide();
    ui->shadow->hide();
}
void profSettings::setInitVars(bool social){
    qDebug()<<"SET INIT VARS:"<<upd_nick<<upd_login<<upd_phon;
    if(social==1)ui->b_linked->show();
    init_nick=upd_nick;
    init_email=upd_login;
    init_phone=upd_phon;
}
void profSettings::wrong_email_message(){
    int reply=QMessageBox::question(this,"WARNING","WRONG PASSWORD\nDO YOU WANT TO SET NEW ONE?",QMessageBox::No | QMessageBox::Yes);
    if(reply==QMessageBox::Yes){
        qDebug()<<"EMIT REC";
        ui->edit_pass->setText("");
        emit remind_password(ui->but_email->text());
    } else {
        if(act=="pass")
            ui->edit_pass->setFocus();
        else ui->but_pass->click();
    }
    ui->b_save->setDisabled(false);
}
void profSettings::info(QString inf){
    QMessageBox::information(this,"",inf);
}
void profSettings::on_b_save_clicked(){
    //   emit set_waiter();
    if(ready_to_rec){
        emit rec_user();
        return;
    }
    QApplication::processEvents();
    b_save_click=true;
    complete_fields();
    qDebug()<<must_correct_email<<must_chk_cred<<must_chk_email;
    if(!must_correct_email && !must_chk_email && !must_chk_cred){
        if(avatar_changed){
            qDebug()<<"AVATAR ChaNGED";
            if(!img.save(f_user_avatar))
                qDebug()<<"SAVE AVATAR LIPA";
        }
        qDebug()<<"AVATAR FILE:"<<f_user_avatar<<","<<QFileInfo(f_user_avatar).exists();
            emit upd_user(upd_login,upd_pass,upd_nick,upd_phon);
        emit go("home");
    } else {
        ui->b_save->setDisabled(false);
        return;
    }
}
void profSettings::confirmation_sent_info(){
    info("- visit Your mailbox -");
    ui->b_save->setDisabled(false);
    on_but_pass_clicked();
}

void profSettings::email_already_registered_info(){
    must_chk_email=false;
    must_chk_cred=false;
    must_chk_email=false;
    int reply=QMessageBox::question(this,"WARNING","EMAIL ALREADY REGISTERED\nDO YOU WANT TO SIGN IN?",QMessageBox::No | QMessageBox::Yes);
    if(reply==QMessageBox::Yes){
        focus_pass=true;
        set_recovery_mode(true);
        if(act=="pass")ui->edit_pass->setFocus();
        else ui->but_pass->click();
    } else {
        upd_login=prop_login;
        qDebug()<<"PREV:"<<upd_login;
        ui->but_email->setText(prop_login);
        ui->edit_email->setText(prop_login);
        if(act=="mail")ui->edit_email->setFocus();
        else ui->but_email->click();
    }
}
bool profSettings::check_email_conditions(){
    if(act=="mail"){
        if(!ui->edit_email->hasAcceptableInput() && ui->edit_email->text()!=""){
            ui->edit_email->setStyleSheet(ess+"color:red;");
            QTimer::singleShot(333,this,[=](){
                check_email_conditions();
            });
            return false;
        } else {
            ui->edit_email->setStyleSheet(ess);
        }
    }
    return true;
}
void profSettings::set_email(QString e,bool to_save){
    qDebug()<<"SET EMAIL:"<<e<<prev_text;
    if(e!=""){
        ui->but_email->setText(e);
        ui->edit_email->setText(e);
        upd_login=e;
    } else {
        ui->but_email->setText("Set Your Email");
        ui->edit_email->setText(upd_login);
        ui->edit_email->hide();
    }
}
void profSettings::set_phone(QString e,bool to_save){
    upd_phon=e;
    ui->edit_phone->setText(upd_phon);
    if(upd_phon=="")ui->but_phone->setText("Phone Number");
    else ui->but_phone->setText(upd_phon);
    ui->edit_phone->hide();
}
void profSettings::set_pass(QString val){
    upd_pass=val;
    //
    ui->but_pass->setText("Password");
    // QTimer::singleShot(2000,this,[=](){
    //     ui->but_pass->setText("Changed");
    // });
    qDebug()<<"Updating pass:"<<upd_pass;
}
void profSettings::set_nick(QString e, bool to_save){
    upd_nick=e;
    if(e!=""){
        upd_nick=e;
        ui->edit_name->setText(e);
        ui->but_name->setText(e);
    }
    else ui->but_name->setText("User Name");
    ui->but_name->show();
    ui->but_name->raise();
    ui->edit_name->hide();
    qDebug()<<"Set nick:"<<upd_nick<<ui->but_name->isVisible()<<ui->but_name->text();
}
bool profSettings::checkEmail(bool ex){
    email_exists=ex;
    if(!recovery_mode && email_exists){
        email_already_registered_info();
        return false;
    } else {
        if(b_save_click)emit save_profile();
        return true;
    }
}
bool profSettings::complete_fields(){
    qDebug()<<"complete fields:"<<act;
    if(!undo){
        if(act=="mail"){
            validated=check_email_conditions();
            if(validated){
                set_email(ui->edit_email->text());
                qDebug()<<"check email1:"<<upd_login;
                if(!recovery_mode){
                    must_chk_email=true;
                    must_chk_cred=false;
                    emit check_email_exists(upd_login,false);
                }
                must_correct_email=false;
            } else {
                qDebug()<<"SETTING EMAIL:"<<upd_login;
                if(mbu!=nullptr){
                    if(mbu->isVisible())
                        mbu->click();
                    else edi->setFocus();
                }
                must_correct_email=true;
            }
        }
        if(act=="pass"){
            if(recovery_mode){
                must_chk_email=false;
                must_chk_cred=false;
                ready_to_rec=true;
            }
            if(ui->edit_pass->text()!=""){
                set_pass(ui->edit_pass->text());
            }
        }
        qDebug()<<"PASS:"<<upd_pass<<ui->edit_pass->text();
        if(act=="nick")set_nick(ui->edit_name->text());
        if(act=="phon")set_phone(ui->edit_phone->text());
        qDebug()<<"IND:"<<init_nick<<upd_nick<<","<<init_email<<upd_login<<","<<init_phone<<upd_phon<<","<<upd_pass;
        if(init_nick!=upd_nick || init_email!=upd_login || init_phone!=upd_phon || upd_pass!="" || avatar_changed){
            qDebug()<<"INIT:"<<init_nick<<upd_nick<<","<<init_email<<upd_login<<","<<init_phone<<upd_phon<<","<<upd_pass<<","<<avatar_changed;
            ui->ind_modified->show();
        }

        else ui->ind_modified->hide();
    } else {
        edi->setText(prev_text);
    }
    qDebug()<<"HIDE FIELD:"<<edi;
    if(edi!=nullptr)edi->hide();
    if(vis!=nullptr)vis->hide();
    if(mbu!=nullptr)mbu->show();
    vis=nullptr;
    mbu=nullptr;
    edi=nullptr;
    act="";
    undo=false;
    bool rc=ret_click;
    ret_click=false;
    return rc;
}
void profSettings::post_switch(){
    vis->show();
    mbu->hide();
    edi->show();
    prev_text=edi->text();
    qDebug()<<"post prev:"<<prev_text;
    if(act=="mail" && ui->edit_email->text().left(5)=="GUEST")ui->edit_email->selectAll();
    if(!edi->hasFocus())edi->setFocus();
    if(!QApplication::inputMethod()->isVisible())
        QApplication::inputMethod()->show();
}
void profSettings::on_but_name_clicked(){
    if(complete_fields()){
        return;
    }
    act="nick";
    qDebug()<<"Nick";
    vis=ui->b_undo_name;
    mbu=ui->but_name;
    edi=ui->edit_name;
    post_switch();
}
void profSettings::on_but_email_clicked(){
    if(complete_fields()){
        return;
    }
    act="mail";
    vis=ui->b_undo_email;
    mbu=ui->but_email;
    edi=ui->edit_email;
    post_switch();
}
void profSettings::on_but_pass_clicked(){
    if(complete_fields()){
        return;
    }
    act="pass";
    vis=ui->b_undo_pass;
    mbu=ui->but_pass;
    edi=ui->edit_pass;
    post_switch();
}
void profSettings::on_but_phone_clicked(){
    if(complete_fields()){
        return;
    }
    act="phon";
    vis=ui->b_undo_phone;
    mbu=ui->but_phone;
    edi=ui->edit_phone;
    post_switch();
}
void profSettings::on_edit_email_returnPressed(){
    qDebug()<<"EDIT EMAIL RET";
    ret_click=true;
    complete_fields();
}
void profSettings::on_edit_pass_returnPressed(){
    qDebug()<<"EDIT PASS RET";
    ret_click=true;
    complete_fields();
}
void profSettings::on_edit_phone_returnPressed(){
    qDebug()<<"EDIT PHONE RET";
    ret_click=true;
    complete_fields();
}
void profSettings::on_edit_name_returnPressed(){
    qDebug()<<"EDIT NAME RET";
    ret_click=true;
    complete_fields();
}
void profSettings::on_b_options_clicked()
{
    emit show_menu(ui->b_options->mapToGlobal(QPoint(0,0)));
}
void profSettings::on_b_options_pressed()
{
    old_ss=ui->b_options->styleSheet();
    ui->b_options->setStyleSheet(old_ss+";color:orange");
}
void profSettings::on_b_options_released()
{
    ui->b_options->setStyleSheet(old_ss);
}
profSettings::~profSettings(){
    qDebug()<<"PROFSETTINGS DESTR";
    delete ui;
}
void profSettings::set_recovery_mode(){
    set_recovery_mode(true);
}
void profSettings::set_recovery_mode(bool rec){
    qDebug()<<"SET REC MODE:"<<rec;
    recovery_mode=rec;
    ui->b_add_foto->setVisible(!rec);
    ui->edit_name->setVisible(!rec);
    ui->edit_phone->setVisible(!rec);
    ui->but_name->setVisible(!rec);
    ui->but_phone->setVisible(!rec);
    ui->icon_phone->setVisible(!rec);
    ui->icon_avatar->setVisible(!rec);
    ui->icon_logout->setVisible(!rec);
    ui->b_logout->setVisible(!rec);
    if(rec)ui->b_save->setText("LOGIN");
    else ui->b_save->setText("SAVE");
    rot=false;
}
void profSettings::on_b_add_foto_clicked(){
    rot=rt;
    rt=false;
#if defined (Q_OS_IOS)
    QString file=QFileDialog::getOpenFileName(this,tr("Selfie pick-up"),pictures_dir);
    gotAnswer(file);
#elif defined (Q_OS_ANDROID)
    w=new QQuickWidget(this);
    w->raise();
    w->setSource(QUrl("qrc:/gallery.qml"));
    QObject* obj=w->rootObject();
    connect(obj,SIGNAL(completed(QString)),this,SLOT(gotAnswer(QString)));
    w->showFullScreen();
#endif
}
void profSettings::gotAnswer(QString fname){
    if(fname==""){
        qDebug()<<"NO FILE!";
        return;
    }
    permission=new Permissions(this);
    permission->requestExternalStoragePermission();
    fname.replace("file://","");
    fimg.load(fname);
    qDebug()<<"loaded pic:"<<fimg.size();
    set_picture();
    ui->b_rotate->show();
}
void profSettings::set_picture(bool rot){
    ui->ind_modified->show();
    img=circle_pix(fimg,rot);
    if(img.isNull())return;
    qDebug()<<"circle dimms:"<<img.size();
    img.setDevicePixelRatio(dpi*2);
    ui->lab_avatar->setPixmap(img);
    avatar_changed=true;
}
QPixmap profSettings::circle_pix(QPixmap p,bool rot){
    int w=p.width();
    int h=p.height();
    if(w>h){
        // rot=false;
        w=h;
    } else {
        //  rot=true;
        h=w;
    }
    int x=p.width()/2-w/2;
    int y=p.height()/2-h/2;
    QPixmap p2(w,h);
    QPainter painter(&p2);
    qDebug()<<"photoo dimms:"<<p.width()<<p.height();
    if(rot){
        //   qDebug()<<"ROTATING!!";
        painter.translate(w/2,h/2);
        painter.rotate(90*rotated++);
        painter.translate(-w/2,-h/2);
        if(rotated==4)rotated=0;
    }
    painter.drawPixmap(0,0,w,h,p,x,y,w,h);
    painter.end();
    QBitmap map(w,h);
    map.fill(Qt::color0);
    painter.begin(&map);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setBrush(QColor(0,0,0,255));
    QPen pen;
    pen.setWidth(3);
    painter.drawEllipse(0,0,w,h);
    p2.setMask(map);
    painter.end();
    painter.begin(&p2);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setPen(pen);
    painter.drawEllipse(0,0,w,h);
    painter.end();
    return p2.scaled(ui->lab_avatar->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
}
void profSettings::set_avatar(bool to_save){
    QPixmap img(f_user_avatar);
    if(!img.isNull()){
        qDebug()<<"SETTING AVA:"<<img.size();
        ui->lab_avatar->setScaledContents(true);
        ui->lab_avatar->clear();
        ui->lab_avatar->setPixmap(img);
        ui->lab_avatar->update();
    }

}
void profSettings::on_b_add_foto_pressed()
{
    pressed=true;
    ui->b_add_foto->setStyleSheet("background-image:none;background-color:transparent;border-image:none;border:none;");
    t->start(1200);
}
void profSettings::on_b_undo_name_clicked()
{
    undo=true;
    complete_fields();
}
void profSettings::on_b_undo_email_clicked()
{
    undo=true;
    complete_fields();
}
void profSettings::on_b_undo_pass_clicked()
{
    undo=true;
    qDebug()<<"UNDO EMAIL"<<prev_text;
    complete_fields();
}
void profSettings::on_b_undo_phone_clicked()
{
    undo=true;
    complete_fields();
}
void profSettings::on_b_logout_clicked()
{
    QMessageBox::StandardButton reply;
    reply=QMessageBox::question(this,"","LOGOUT?");
    if(reply==QMessageBox::Yes){
        qDebug()<<"YES";
        emit logout();
    }
}
void profSettings::on_b_back_clicked(){
    if(ui->ind_modified->isVisible()){
        QMessageBox::StandardButton reply;
        QString tit="CHANGES NOT SAVED, CLICK YES TO CONFIRM";
        reply=QMessageBox::question(this,"WARNING",tit,QMessageBox::No | QMessageBox::Yes);
        qDebug()<<"REPLY"<<reply;
        if(reply==QMessageBox::No){
            return;
        }
    }
    qDebug()<<"GO BACK";
    emit go("back");
}
void profSettings::on_b_add_foto_released(){
    pressed=false;
    qDebug()<<"T:<:"<<t->remainingTime();
    if(t->remainingTime()==-1){
        rt=true;
    } else rt=false;
}
void profSettings::animateButtonPress(QObject* s){
    QPushButton* b=static_cast<QPushButton*>(s);
    QRect g=b->parentWidget()->geometry();
    g.setX(0);
    g.setWidth(width());
    g.setY(g.y());
    ui->shadow->setGeometry(g);
    ui->shadow->show();
}
void profSettings::animateButtonRel(){
    ui->shadow->hide();
}
void profSettings::on_b_rotate_clicked(){
    set_picture(true);
}
void profSettings::on_but_name_pressed(){
    animateButtonPress(sender());
}
void profSettings::on_but_email_pressed(){
    animateButtonPress(sender());
}
void profSettings::on_but_pass_pressed(){
    animateButtonPress(sender());
}
void profSettings::on_but_phone_pressed(){
    animateButtonPress(sender());
}
void profSettings::on_b_logout_pressed(){
    animateButtonPress(sender());
}
void profSettings::on_but_name_released(){
    animateButtonRel();
}
void profSettings::on_but_email_released(){
    ui->shadow->hide();
}
void profSettings::on_but_pass_released(){
    ui->shadow->hide();
}
void profSettings::on_b_logout_released(){
    ui->shadow->hide();
}
void profSettings::on_but_phone_released()
{
    ui->shadow->hide();
}
