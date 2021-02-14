#ifndef MENU_H
#define MENU_H
#include "globals.h"

namespace Ui {
class menu;
}

class menu : public QDialog {
    Q_OBJECT
public:
    QWidget* parm;
    explicit menu(QWidget *parent = nullptr);
    ~menu() override;
    void set_opt_geo();
    double opac;
    void getGeo();
    bool showing,hidding;
    bool choosed;
    void deactivate(QString,QStringList);
    void set_access(bool);
    bool is_access,is_context;
    void setContext(bool);
private slots:
    void on_select();


    void on_menu_hide_clicked();

protected:
    void resizeEvent(QResizeEvent*) override;
    //void focusOutEvent(QFocusEvent*e) override;

private:
    Ui::menu *ui;
    QString style;
signals:
    void logout();
    void go(QString,QStringList);
    void exit_app();
    void menu_on(bool);
};
#endif // MENU_H
