﻿#include "topmenuswitcher.h"
topMenuSwitcher::topMenuSwitcher(QWidget *parent,int dopacity) : QOpenGLWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents,false);
    //   setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_AlwaysStackOnTop,true);
    //  setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(false);
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    setObjectName("container");
    setMouseTracking(true);
    ready_to_paint=false;
    isEdit=false;
    isEditEn=false;
    srchinited=false;
    sframe=new QFrame(this);
    //    sframe->setAttribute(Qt::WA_NoSystemBackground);
    sframe->setAttribute(Qt::WA_AlwaysStackOnTop,true);
    // sframe->setAttribute(Qt::WA_OpaquePaintEvent);
    edit=new QLineEdit(sframe);
    b_search=new QPushButton(sframe);
    b_clear=new QPushButton(sframe);
    b_search->setIcon(QPixmap(":/gui/icons/mag_glass_black.png"));
    b_clear->setIcon(QPixmap(":/gui/APP ICONS/clear.png"));
    connect(edit,SIGNAL(textChanged(const QString&)),this,SIGNAL(textChanged(const QString&)));
    connect(edit,SIGNAL(textChanged(const QString&)),this,SLOT(doUpdate(const QString&)));
        connect(edit,SIGNAL(textEdited(const QString&)),this,SLOT(doUpdate2(const QString&)));

    connect(b_clear,SIGNAL(clicked()),this,SLOT(clearEdit()));
    sframe->setVisible(false);
    edit->grabKeyboard();
    connect(&t,SIGNAL(timeout()),this,SLOT(chkkeyb()));
    QApplication::setCursorFlashTime(0);

}
void topMenuSwitcher::mousePressEvent(QMouseEvent* e){
    get_hover(e->pos());
}
void topMenuSwitcher::chkkeyb(){
    //  qDebug()<<"edit.key"<<keyboardGrabber();
    if(QApplication::inputMethod()->isVisible()){
        //    qDebug()<<"EDIT:"<<edit->keyboardGrabber();
        update();
    } else t.stop();
}
void topMenuSwitcher::initializeGL(){
    glClearColor(0,0,0,0);
}
void topMenuSwitcher::cvtog(){
    cv=!cv;
    update();
}
void topMenuSwitcher::showKB(bool s){
    qApp->inputMethod()->setVisible(s);
}
void topMenuSwitcher::showKB(){
    if(isEditEn){
        edit->setFocus();
        edit->grabKeyboard();
        if(!qApp->inputMethod()->isVisible())qApp->inputMethod()->setVisible(true);
    } else {
        edit->releaseKeyboard();
        sframe->setEnabled(isEditEn);
        sframe->setVisible(isEditEn);
    }

    ready_to_paint=true;
    update();
}
void topMenuSwitcher::showSelectors(bool s){
    if(selector_visible!=s){
        //  qDebug()<<"SHOW SELECTORS";
        selector_visible=s;
        update();
    }
}
void topMenuSwitcher::setSearchMode(bool s=false){
    s=s && edit;
    bool ie=isEdit==s;
    isEdit=s;
    isEditEn=s;
    edit->setFocus();
    edit->grabKeyboard();
    t.start(500);
    if(!ie)QApplication::inputMethod()->setVisible(s);
    //  edit->setFocus(Qt::FocusReason::MouseFocusReason);
    update();
}
void topMenuSwitcher::setupSearch(){
    sframe->setStyleSheet("background:transparent;color:#"+par->search_text_color+";border:1px solid black;border-radius:15px;");
    QString rstyle="border:none;background:transparent;color:#"+par->search_text_color+";border-radius:15px;";
    edit->setStyleSheet(rstyle);
    b_search->setStyleSheet(rstyle);
    b_clear->setStyleSheet(rstyle);
    edit->setAlignment(Qt::AlignCenter);
    edit->setInputMethodHints(Qt::ImhLowercaseOnly | Qt::ImhNoTextHandles | Qt::ImhNoAutoUppercase |
                              Qt::ImhNoPredictiveText);
}
void topMenuSwitcher::setFrames(QWidget* lfrm_a,QWidget* lfrm_b){
    frames.clear();
    QList<QWidget*> widgets=lfrm_a->findChildren<QWidget *>(QString(),Qt::FindChildOptions(Qt::FindChildrenRecursively));
    lfrm_a->setParent(this);
    lfrm_b->setParent(this);
    lfrm_a->hide();
    lfrm_b->hide();
    // sframe->setParent(lfrm_a);
    frames.append(lfrm_a);
    frames.append(lfrm_b);
    if(!srchinited){
        int hy=(height()-height()*par->search_height_factor/100)/2;
        int ty=height()-hy*2;
        int hx=width()/8;
        int tx=width()-hx*2;
        int sx=30*devicePixelRatio();
        edit->setFont(QFont("Apple Gothic",ty/2));
        sframe->setGeometry(hx,hy,tx,ty);
        //    qDebug()<<"SFRAME:"<<sframe->geometry()<<rect();
        sframe->setFixedSize(tx,ty);
        b_search->setGeometry(5,0,sx,ty);
        b_clear->setGeometry(tx-5-sx,0,tx-5,ty);
        b_search->setFixedSize(sx,ty);
        b_clear->setFixedSize(sx,ty);
        edit->setGeometry(sframe->rect());
        edit->setFixedSize(sframe->size());
        b_search->setIconSize(QSize(23,18));
        b_clear->setIconSize(QSize(35,15));
        //  qDebug()<<"EDIT"<<b_search->iconSize()<<sx;
        srchinited=true;
    }
}
void topMenuSwitcher::doUpdate2(const QString& txt){
    edit->setFocusPolicy(Qt::StrongFocus);
    edit->setClearButtonEnabled(true);
    edit->grabKeyboard();
    edit->activateWindow();
    qDebug()<<"EDIT UPDATE2:"<<txt;
    edit->ensurePolished();
    edit->repaint();
}

void topMenuSwitcher::doUpdate(const QString& txt){
    edit->setFocusPolicy(Qt::StrongFocus);
    edit->setClearButtonEnabled(true);
    edit->grabKeyboard();
    qDebug()<<"EDIT UPDATE:"<<txt;
    update();
}
void topMenuSwitcher::clearEdit(){
    edit->setText("");
    emit need_filter();
}
/* SWITCH BETWEEN WIDGETS : */
/* enum vP:OVER = false; */
/* enum vP:UNDER = true */
bool topMenuSwitcher::get_hover(QPoint pos){
    if(!ready_to_paint){
        return false;
    }
    pos=mapFromGlobal(pos);
    QList<QPushButton*> wl;
    wl.clear();
    wl.append(frames.at(0)->findChildren<QPushButton*>(QString(),Qt::FindChildOptions(Qt::FindChildrenRecursively)));
    if(isEditEn){
        if(sframe->geometry().contains(pos)){
            edit->setFocus(Qt::FocusReason::MouseFocusReason);
#if defined(Q_OS_IOS)
            edit->grabKeyboard();
            if(!QApplication::inputMethod()->isVisible())
                QApplication::inputMethod()->show();
#endif
            t.start(500);
            return false;
        }
    }
    if(wl.count()>0){
        for(int i=0;i<wl.count();i++){
            qDebug()<<"WL"<<wl.at(i)->objectName()<<wl.at(i)->geometry()<<pos<<wl.at(i),isEnabled();
            if(wl.at(i)->geometry().contains(pos) && wl.at(i)->isEnabled()){
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
void topMenuSwitcher::setPaintCanvas(QImage &i){
    *canvas=i;
    paintEngine()->setPaintDevice(canvas);
}
void topMenuSwitcher::paintGL(){
    //  qDebug()<<"SENDER:"<<sender();
    //  qDebug()<<"SEARCH:"<<isEditEn<<ready_to_paint;
    if(!ready_to_paint)return;
    QPainter painter(this);
    painter.beginNativePainting();
    glClear(GL_COLOR_BUFFER_BIT);
    painter.endNativePainting();
    QWidget* w=static_cast<QWidget*>(frames.at(selector_visible && !isEditEn));
    w->render(&painter,w->mapFromGlobal(w->mapToGlobal(QPoint(0,0))));
    if(isEditEn){
        painter.setBrush(QColor("#"+par->search_field_color));
        painter.setPen(QColor("#90333333"));
        painter.drawRoundedRect(sframe->geometry(),15,15);
        sframe->render(&painter,sframe->pos());
    }
    //  qDebug()<<"PAINTED TOPMENU";
}
topMenuSwitcher::~topMenuSwitcher(){qDebug()<<"Deleting tompenu";}
