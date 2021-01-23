#include "topmenuswitcher.h"
topMenuSwitcher::topMenuSwitcher(QWidget *parent,int dopacity) : QOpenGLWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents,false);
    //    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_AlwaysStackOnTop,true);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(false);
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    setObjectName("container");
    setMouseTracking(true);
    ready_to_paint=false;
    isEdit=false;
    isEditEn=false;
    sframe=new QFrame(this);
    edit=new QLineEdit(sframe);
    b_search=new QPushButton(sframe);
}
void topMenuSwitcher::initializeGL(){
    glClearColor(0,0,0,0);
}
void topMenuSwitcher::cvtog(){
    cv=!cv;
    update();
}
void topMenuSwitcher::showSelectors(bool s){
    if(selector_visible!=s){
        if(frames.count()>s){
            qDebug()<<"fr:"<<frames.count();
            qDebug()<<selector_visible;
        }
        selector_visible=s;
        update();
    }
}
void topMenuSwitcher::setSearchMode(bool s=false){
    s=s && edit;
    if(s){
    //    edit->setParent(this);
        edit->setEnabled(true);
        edit->show();
        edit->setFocus();
        edit->grabKeyboard();
    } else {
       // edit->setParent(0);
        edit->setEnabled(false);
        edit->hide();
    }
    isEdit=s;
    isEditEn=s;
    ready_to_paint=true;
    update();
}
void topMenuSwitcher::setupSearch(){
    b_search->setIcon(QPixmap(":/gui/icons/mag_glass_black.png"));
    qDebug()<<sframe->geometry()<<b_search->geometry()<<edit->geometry();
    sframe->setStyleSheet("background:#"+par->search_field_color+";color:#"+par->search_text_color+";border:1px solid black;border-radius:15px;");
    QString rstyle="border:none;background:transparent;";
    b_search->setStyleSheet(rstyle);
    edit->setStyleSheet(rstyle);
    edit->setAlignment(Qt::AlignCenter);
    edit->setInputMethodHints(Qt::ImhLowercaseOnly | Qt::ImhNoTextHandles | Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    disconnect(edit,SIGNAL(textChanged(const QString&)),this,SIGNAL(textChanged(const QString&)));
    disconnect(edit,SIGNAL(textChanged(const QString&)),this,SLOT(update()));
    connect(edit,SIGNAL(textChanged(const QString&)),this,SIGNAL(textChanged(const QString&)));
    connect(edit,SIGNAL(textChanged(const QString&)),this,SLOT(update()));
    sframe->show();
}
void topMenuSwitcher::setFrames(QWidget* lfrm_a,QWidget* lfrm_b){
    frames.clear();
    QList<QWidget*> widgets=lfrm_a->findChildren<QWidget *>(QString(),Qt::FindChildOptions(Qt::FindChildrenRecursively));
    lfrm_a->setParent(this);
    lfrm_a->hide();
    lfrm_b->hide();
    sframe->setParent(lfrm_a);
    frames.append(lfrm_a);
    frames.append(lfrm_b);
    int hy=(height()-height()*par->search_height_factor/100)/2;
    qDebug()<<hy<<height()<<par->search_height_factor;
    int ty=height()-hy*2;
    int hx=width()/8;
    int tx=width()-hx*2;
    int sx=30*devicePixelRatio();
    edit->setFont(QFont("Apple Gothic",ty/2));
    sframe->setGeometry(hx,hy,tx,ty);
    sframe->setFixedSize(tx,ty);
    b_search->setGeometry(0,0,sx,ty);
    b_search->setFixedSize(sx,ty);
    edit->setGeometry(sx,0,tx-sx,ty);
    edit->setFixedSize(tx-sx,ty);
    b_search->setIconSize(QSize(sx*8/10,sx*8/10));
}
/* SWITCH BETWEEN WIDGETS : */
/* enum vP:OVER = false; */
/* enum vP:UNDER = true */
bool topMenuSwitcher::get_hover(QPoint pos){
    if(!ready_to_paint){
        return false;
    }
    QList<QPushButton*> wl;
    QLineEdit* le;
    wl.clear();
    wl.append(frames.at(selector_visible)->findChildren<QPushButton*>(QString(),Qt::FindChildOptions(Qt::FindChildrenRecursively)));
    if(isEditEn){
            if(edit->rect().adjusted(0,0,-20,0).contains(pos)){
            edit->setFocus();
            edit->grabKeyboard();
            if(!QApplication::inputMethod()->isVisible())
                QApplication::inputMethod()->show();
            return false;
        }
    }
    if(wl.count()>0){
        for(int i=0;i<wl.count();i++){
            qDebug()<<wl.at(i);
            if(wl.at(i)->rect().contains(wl.at(i)->mapFromGlobal(pos)) && wl.at(i)->isEnabled()){
                wl.at(i)->click();
                return false;
                break;
            }
        }
        wl.clear();
    }
    return true;
}
void topMenuSwitcher::forceUpdate(){
    qDebug()<<"TOPMENUSW GLCONT";
    update();
}
void topMenuSwitcher::resizeEvent(QResizeEvent *e){
    resizeGL(e->size().width(),e->size().height());
}
void topMenuSwitcher::paintGL(){
    if(!ready_to_paint)return;
    QPainter painter(this);
    painter.beginNativePainting();
    glClear(GL_COLOR_BUFFER_BIT);
    painter.endNativePainting();
    QWidget* w=frames.at(selector_visible && !isEditEn);
    w->render(&painter,w->mapFromGlobal(w->mapToGlobal(QPoint(0,0))));
}
topMenuSwitcher::~topMenuSwitcher(){
}
