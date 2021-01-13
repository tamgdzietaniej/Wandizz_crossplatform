#ifndef WIDGET_WRAPPER_H
#define WIDGET_WRAPPER_H
#include "globals.h"
#include "gllabel.h"
#include <QCheckBox>
class widget_wrapper : public QOpenGLWidget
{
    Q_OBJECT

public:
    widget_wrapper(QWidget *parent = nullptr,QString="");
    ~widget_wrapper() override;
    void update_container(int c);
    QPixmap star;
    void showSearch(bool);
    void showSearch();
    bool is_search();
    Prospect* prospect_init_params;
    bool get_hover(QPoint);
    QPoint mpos;
    int dpi;
    bool show_search;
    bool ready_to_paint;
    void make_edit();
    void hideSearch();
    bool selectors_visible,selectors_visible_prev,issrch;
    QFrame * tw;
    //  QTimer *timer;
    void set_edit();
    int moved,prev_moved;
    QPoint diff;
    QLabel* labs;
    void disableSearch();
    void enableSearch();
    QPointer<QLineEdit> edit=nullptr;
    QWidget *midFrame=nullptr;
    void updateFrameSet();
    void showSelectors(bool);
    void setupWrapper(QList<QList<QWidget*>>);
    QInputMethod* input=QApplication::inputMethod();
    bool clicked(QWidget*);
private:
    QList<QList<QWidget*>> wlist;
    QList<QList<QPoint>> wpos;
    QString type;
protected:
    void paintGL() override;
    void initializeGL() override;
    QRect proxy_geo;
    QString pn;
    void resizeEvent(QResizeEvent *e) override;
signals:
    void filter(QString);
    void show_input(bool);
    void textChanged(const QString&);
    void keybh(int);
};

#endif // WIDGET_WRAPPER_H
