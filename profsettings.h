#ifndef PROFSETTINGS_H
#define PROFSETTINGS_H
#include "globals.h"

namespace Ui {
class profSettings;
class menu;
}

class profSettings : public QMainWindow
{
    Q_OBJECT

public:
    QQuickWidget* w;
    QQmlContext* context;
    QQmlEngine* engine;
    explicit profSettings(QWidget *parent = nullptr);
    ~profSettings();
    QRegularExpressionValidator* validator;
    bool b_save_click;
    void set_nick(QString,bool to_save=true);
    QString old_ss,old_ss2;
    bool focus_pass=false;
    QString act;
    bool avatar_changed;
    bool ready_to_rec;
    void set_recovery_mode(bool);
    QInputMethod *keyboard;
    void set_pass(QString);
    bool checkEmail(bool);
    void set_avatar(bool to_save=true);
    void set_email(QString,bool to_save=true);
    void set_phone(QString,bool to_save=true);
    QPushButton* disabled;
    bool sent=false;
    QString upd_login,upd_pass,upd_nick,upd_phon;
    Permissions* permission;
    void set_recovery_mode();
    bool recovery_mode;
    QString password;
    bool check_email_conditions();
    QRegularExpression rv;
    QFont f;
    void post_switch();
    QTimer *ktimer;
    bool email_exists;
    QPixmap img,fimg;
    QString prop_login;
    bool must_chk_email;
    bool must_correct_email;
    bool must_chk_cred;
    void setInitVars(bool);
private:

    Ui::profSettings *ui;

    float ar,iw,ih;
    QPushButton* wtemp=nullptr;
    bool check_if_to_recover;
    bool undo;
    QString prev_text;
    bool ret_click;
    void db();
    bool rt;
    void eb();
    QString initial_email;
    QString ess;
    bool validated;
    int dpi;
    QPushButton* vis=nullptr;
    QLineEdit* edi=nullptr;
    bool rot;
    int rotated;
    QTimer* t;
    bool pressed=false;
    QPushButton* mbu=nullptr;
    QString init_email,init_nick,init_phone;
    void animateButtonPress(QObject*);
    void animateButtonRel();
signals:
    void get_foto();
    void ready();
    void edit_nickname(QRect);
    void edit_phone(QRect);
    void edit_email(QRect);
    void edit_pass(QRect);
    void upd_user(QString,QString,QString,QString);
    void rec_user();
    void check_email_exists(QString,bool);
    void go(QString);
    void remind_password(QString);
    void save_profile();
    void show_menu(QPoint);
    void set_waiter();
    void logout();

public slots:
    QPixmap circle_pix(QPixmap,bool rot=false);
    void set_picture(bool rot=false);
    void email_already_registered_info();
    void info(QString);
    void wrong_email_message();
    void on_but_pass_clicked();
    void confirmation_sent_info();
private slots:
    void on_b_add_foto_clicked();
    void on_b_save_clicked();
    void on_b_options_clicked();
    void on_b_options_pressed();
    void on_b_options_released();
    void on_b_add_foto_pressed();
    bool complete_fields();
    void on_b_undo_name_clicked();
    void on_b_undo_email_clicked();
    void on_b_undo_pass_clicked();
    void on_b_undo_phone_clicked();
    void on_b_logout_clicked();
    void gotAnswer(QString);
    void on_but_name_clicked();
    void on_but_email_clicked();
    void on_but_phone_clicked();
    void on_edit_email_returnPressed();
    void on_edit_pass_returnPressed();
    void on_edit_phone_returnPressed();
    void on_edit_name_returnPressed();
    void on_b_add_foto_released();
    void on_b_rotate_clicked();
    void on_but_name_pressed();
    void on_but_email_pressed();
    void on_but_pass_pressed();
    void on_but_phone_pressed();
    void on_b_logout_pressed();
    void on_but_name_released();
    void on_but_email_released();
    void on_but_pass_released();
    void on_b_logout_released();
    void on_b_back_clicked();
    void on_but_phone_released();
};


#endif // PROFSETTINGS_H
