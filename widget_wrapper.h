#ifndef WIDGET_WRAPPER_H
#define WIDGET_WRAPPER_H
#include "globals.h"
class widget_wrapper : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit widget_wrapper(QWidget *parent = nullptr);
    ~widget_wrapper() override;
    void set_proxy_widget(QWidget* proxy,qreal op=-1);
    void set_labels(QString,QString);
    void update_container(int c);
    void set_container(QRect,QRect,QRect,QRect);
    QPixmap star;
    void drawBackground(QPainter&);
    void showSearch(bool);
    QString text;
    void set_styles(QLabel*,QLabel*,QLabel*);
    QRect cntr,labr,nicr,cntrw,labrw,nicrw,uprct,starrect;
    QFont cfont,lfont,nfont;
    QPen cpen,npen,lpen;
    int favs_cnt;
    QString label,nick;
    bool get_hover(QPoint);
    QPoint mpos;
    int dpi;
    QString button;
    bool ready_to_paint;
    QOpenGLWidget *marker;
    void showSelectors(bool);
    bool selectors_visible,to_selectors_visible;
  //  QTimer *timer;
    QLabel* favs;
    bool shTop,shMid,frame_pos_setted;
    int moved;
    QPoint diff;
    QLabel* labs;
    QLineEdit *edit;
    void setFrame(QWidget *mid_frame);
    QWidget *midFrame;
    void updateFrameSet();
protected:
    void paintGL() override;
    void initializeGL() override;
    QRect proxy_geo;

    QString pn;
    void resizeEvent(QResizeEvent *e) override;
public slots:
  //  void check_timer();
    void on_text_changed(const QString&);
private:
signals:
    void filter(QString);
};

#endif // WIDGET_WRAPPER_H
