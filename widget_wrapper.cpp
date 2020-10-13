#include "widget_wrapper.h"
widget_wrapper::widget_wrapper(QWidget *parent) : QOpenGLWidget(parent){
    setAttribute(Qt::WA_AlwaysStackOnTop,true);
    setMouseTracking(true);
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    setObjectName("container");
    dpi=devicePixelRatio();
    ready_to_paint=false;
//    timer=new QTimer(this);
 //   connect(timer,SIGNAL(timeout()),this,SLOT(check_timer()));
    favs_cnt=0;
    selectors_visible=false;
    to_selectors_visible=false;
    nick="";
    frame_pos_setted=false;
    label="";
    star.load(":/gui/ICONS_FAVS/green_star.png");
    star.setDevicePixelRatio(dpi*2);
    marker=new QOpenGLWidget(this);
    marker->setGeometry(0,0,0,0);
}
void widget_wrapper::initializeGL(){
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
bool widget_wrapper::get_hover(QPoint pos){
    if(!ready_to_paint){
        qDebug()<<objectName()<<"not ready";
        return false;
    }
    QList<QPushButton*> wl=midFrame->findChildren<QPushButton *>();
    if(wl.count()==0)qDebug()<<"WL0";
    if(wl.count()>0){
        for(int i=0;i<wl.count();i++){
            if(wl.at(i)->rect().contains(wl.at(i)->mapFromGlobal(pos)) && wl.at(i)->isEnabled()){
                wl.at(i)->click();
                return wl.at(i)->isEnabled();
            }
        }
    }
    return false;
}
void widget_wrapper::showSearch(bool sh){
    edit->setVisible(sh);
    edit->setEnabled(sh);
    edit->setFocus();
    edit->grabKeyboard();
    update();
}
void widget_wrapper::showSelectors(bool s){
    if(selectors_visible!=s){
        selectors_visible=s;
        update();
    }
}
void widget_wrapper::set_container(QRect sr,QRect cr,QRect lr,QRect nr){
    cntr=cr;
    labr=lr;
    nicr=nr;
    cntrw=cr;
    labrw=lr;
    nicrw=nr;
    starrect=sr;
    edit=new QLineEdit(this);
    edit->setGeometry(width()/10,height()/10,width()*.8,height()/10);
    edit->setFixedSize(width()*.8,height()/20);
    edit->setAttribute(Qt::WA_AlwaysStackOnTop);
    edit->setStyleSheet("border:black;color:rgba(255,255,255,255);background:rgba(0,0,0,20);");
    edit->setAlignment(Qt::AlignCenter);
    edit->setInputMethodHints(Qt::ImhLowercaseOnly | Qt::ImhNoTextHandles | Qt::InputMethodHint::ImhNoAutoUppercase);
    // edit->show();
    qDebug()<<"EDIT:"<<edit->geometry();
    connect(edit,SIGNAL(textChanged(const QString&)),this,SLOT(on_text_changed(const QString&)));
}
void widget_wrapper::on_text_changed(const QString &arg1)
{
    qDebug()<<"TEXT:"<<arg1;
    text=arg1;
    update();
    emit filter(text);
}
void widget_wrapper::set_styles(QLabel* clab,QLabel* llab,QLabel* nlab){
    cfont=clab->font();
    lfont=llab->font();
    nfont=nlab->font();
}
void widget_wrapper::set_labels(QString lm, QString ln){
    label=lm;
    nick=ln;
   update();
}
void widget_wrapper::update_container(int c){
    if(c!=-1){
        favs_cnt=c;
    } else favs_cnt=0;
    update();
}
void widget_wrapper::resizeEvent(QResizeEvent *e){
    resizeGL(e->size().width(),e->size().height());
}
void widget_wrapper::setFrame(QWidget *mid_frame){
    midFrame=static_cast<QWidget*>(mid_frame);
    mid_frame->hide();
    QPoint sp=midFrame->mapToGlobal(QPoint(0,0));
    if(!frame_pos_setted)
        mpos=mapFromGlobal(sp);
    frame_pos_setted=true;
    ready_to_paint=true;
    update();
}
void widget_wrapper::paintGL(){
    if(ready_to_paint){
        moved=marker->y()*2;
        QPainter painter(this);
        painter.beginNativePainting();
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        painter.endNativePainting();
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::Qt4CompatiblePainting);
        painter.setFont(cfont);      
        painter.setPen(QPen(QColor(39, 217, 255, 255)));
        painter.drawPixmap(starrect.adjusted(0,moved/2,0,moved/2),star);
        painter.drawText(cntrw.adjusted(0,moved,0,0),Qt::AlignCenter,QString::number(favs_cnt));
        painter.setFont(lfont);
        painter.setPen(QColor(255,255,255,255));
        painter.drawText(labrw.adjusted(0,moved,0,0),Qt::AlignCenter,label);
        painter.setFont(nfont);
        painter.drawText(nicrw.adjusted(0,moved,0,0),Qt::AlignCenter,nick);
        if(edit->isVisible()){
            edit->render(&painter,edit->pos());
            qDebug()<<"EDIT ENABLED";
        }
        if(selectors_visible)
            midFrame->render(&painter,mpos+QPoint(0,moved / 2));
        painter.end();
    }
}
void widget_wrapper::updateFrameSet(){
    midFrame->move(mpos+QPoint(0,moved/2));
}
widget_wrapper::~widget_wrapper(){
}
