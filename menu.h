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
    void set_opt_geo(QPoint);
    double opac;
    bool showing,hidding;
    bool choosed;
    void deactivate(QString);
    void set_access(bool);
    bool is_access;
private slots:
    void on_m_fav_videos_clicked();
    void on_m_fav_scenes_clicked();
    void on_m_fav_items_clicked();
    void on_m_search_videos_clicked();
    void on_m_prof_settings_clicked();
    void on_m_videorec_clicked();
    void on_options_clicked();
    void on_m_gohome_clicked();
    void on_m_exit_clicked();
    void on_m_search_items_clicked();
    void on_m_share_clicked();
    void on_m_videos_clicked();
    void on_m_search_scenes_clicked();
    void on_m_prospect_clicked();

public slots:

protected:
    void resizeEvent(QResizeEvent*) override;
    //void focusOutEvent(QFocusEvent*e) override;

private:
    Ui::menu *ui;
    QString style;
signals:
    void logout();
    void go(QString);
    void exit_app();
    void menu_on(bool);
};
#endif // MENU_H
