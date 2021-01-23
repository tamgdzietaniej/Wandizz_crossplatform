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
    void forceUpdate();
    bool ready_to_paint;
    void setSearchMode(bool);
    void setupSearch();
    QLineEdit* edit=nullptr;
    bool isEdit,isEditEn;
    Prospect* par;
private:
    QPointer<QFrame> sframe;
    bool noanim;
    QList<QWidget*> frames;
    QWidget* currFrameSet;
    QPushButton* b_search;
    QRect sgeo;
    bool cv;
protected:
    void paintGL() override;
    void initializeGL() override;
    void resizeEvent(QResizeEvent *e) override;
signals:
    void textChanged(const QString&);
private  slots:
    void   cvtog();
};

#endif // TOPMENUSWITCHER_H
