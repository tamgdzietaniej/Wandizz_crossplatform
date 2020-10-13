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
private:
    QString b_close_look,b_again_look;
    //    QStackedLayout* lay;
    Ui::webhelper *ui;
    QQmlEngine* engine;
    QQmlContext* context;

public slots:
    void gotAnswer(QString);
    void setUrl(QString);

signals:
    void needLogin();
    void go(QString);
    void gotSocialData(QString,QString,QString,QString,QString);

private slots:
    void on_b_close_clicked();
    void on_b_again_clicked();

    void on_container_customContextMenuRequested(const QPoint &pos);

    void on_b_close_pressed();

    void on_b_again_pressed();

    void on_b_again_released();

    void on_b_close_released();

protected:
    void resizeEvent(QResizeEvent* e) override;
};


#endif // SOCIALLOGIN_H
