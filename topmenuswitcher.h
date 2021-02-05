
#ifndef TOPMENUSWITCHER_H
#define TOPMENUSWITCHER_H
#include "globals.h"
class topMenuSwitcher : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit topMenuSwitcher(QWidget *parent = nullptr,int dopacity=1);
    ~topMenuSwitcher() override;
    void setFrames(QWidget* f1,QWidget* f2);
    bool get_hover(QPoint);
    void showSelectors(bool);
    bool selector_visible;
    void setPaintCanvas(QImage&);
    bool srchinited;
    void forceUpdate();
    void showKB();
    bool ready_to_paint;
    void setSearchMode(bool);
    void setupSearch();
    QLineEdit* edit=nullptr;
    bool isEdit,isEditEn;
    Prospect* par;
    QPointer<QFrame> sframe;
private:
    QImage* canvas;
    bool noanim;
    QList<QWidget*> frames;
    QWidget* currFrameSet;
    QPushButton* b_search;
    QPushButton* b_clear;
    QRect sgeo;
    bool cv;
protected:
    void paintGL() override;
    void initializeGL() override;
    void resizeEvent(QResizeEvent *e) override;
signals:
    void textChanged(const QString&);
    void need_filter();
private  slots:
    void cvtog();
    void clearEdit();
    void doUpdate();
};

#endif // TOPMENUSWITCHER_H
