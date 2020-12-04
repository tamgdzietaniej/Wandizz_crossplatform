#ifndef SOCIALLOGIN_H
#define SOCIALLOGIN_H

#include "globals.h"
#include <QtQuick>
#include <QQmlContext>
#include <QQmlEngine>
#include "browser.h"
namespace Ui {
class webhelper;
}

class socialLogin : public QMainWindow
{
    Q_OBJECT

public:
    explicit socialLogin(QWidget *parent = nullptr);
    ~socialLogin() override;
    void resize_win(int);
    browser* w;
    QString act_url;
    void createWebView();
    void setAppleLogin();
    void setFacebookLogin();
private:
    QString b_close_look,b_again_look;
    //    QStackedLayout* lay;
    Ui::webhelper *ui;
    QQmlEngine* engine;
    QQmlContext* context;
    enum Portal{
        FACEBOOK,
        APPLEID,
        TWITTER,
        GOOGLE
    } portal;
   void show_error(QString);
public slots:
    void gotAnswer(QString);
    void setUrl(QString);

signals:
    void needLogin();
    void go(QString);
    void gotSocialData(QString,QString,QString,QString,QString);
    void gotAppleId(QString);


protected:
    void resizeEvent(QResizeEvent* e) override;
};


#endif // SOCIALLOGIN_H
