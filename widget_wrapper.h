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
    QOpenGLWidget* marker_head,*marker_tail;
    int  vcorr;
    void shft();
    double compr,prev_compr;
    Prospect* prospect_init_params;
    bool get_hover(QPoint);
    int* scr;
    void run();
    bool debg;
    QPoint mpos;
    int dpi;
    int oldsel;
    bool showme;
    int oldh,oldt;
    bool ready_to_paint;
    bool selectors_visible,selectors_visible_prev,issrch,nightmode;
    QFrame * tw;
    void forceUpdate();
    //  QTimer *timer;
    int moved,prev_moved;
    QPoint diff;
    QLabel* labs;
    QWidget *midFrame=nullptr;
    void updateFrameSet();
    void showSelectors(bool);
    void setupWrapper(QList<QList<QWidget*>>);
    QInputMethod* input=QApplication::inputMethod();
    bool clicked(QWidget*);
    int mrh();
    int mrt();
private:
    QRect tgeo;

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
