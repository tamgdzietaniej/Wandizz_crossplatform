#include "widget_wrapper.h"
#define G_N_ELEMENTS(arr) ((sizeof(arr))/(sizeof(arr[0])))
widget_wrapper::widget_wrapper(QWidget *parent,QString cname) : QOpenGLWidget(parent),selectors_visible(false),
    ready_to_paint(false),show_search(false),type(cname),selectors_visible_prev(false){
    setAttribute(Qt::WA_AlwaysStackOnTop,true);
    setAttribute(Qt::WA_DeleteOnClose,true);
    //  setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    dpi = devicePixelRatio();
#if defined (Q_OS_IOS)
    dpi*=2;
#endif
    star.load(":/gui/ICONS_FAVS/green_star.png");
    star.setDevicePixelRatio(dpi*2);
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
}
void widget_wrapper::initializeGL(){
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0,0,0,0);
}
bool widget_wrapper::get_hover(QPoint pos){
    mpos=pos;
    if(!ready_to_paint){
        qDebug()<<objectName()<<"not ready";
        return false;
    }
    if(edit && show_search){
        if(clicked(edit)){
            if(!input->isVisible()){
                input->setVisible(true);
            }
            return false;
        }
        for(int j=0;j<3;j++){
            for(int i=0;i<wlist.at(j).count();i++){
                QList<QPushButton*> bl = wlist.at(j).at(i)->findChildren<QPushButton *>(QString(),Qt::FindChildOptions(Qt::FindChildrenRecursively));
                if(bl.count()>0){
                    for(int k=0;k<bl.count();k++){
                        if(clicked(bl.at(k))){
                            return false;
                        }
                    }
                }
            }
        }
    }
}
bool widget_wrapper::clicked(QWidget* w){
    if(w->rect().contains(w->mapFromGlobal(mpos))){
        if(QString::fromLatin1(w->metaObject()->className())=="QPushButton"){
            if(w->isEnabled()){
                static_cast<QPushButton*>(w)->click();
            }
        }
        if(QString::fromLatin1(w->metaObject()->className())=="QLineEdit"){
            if(w->isEnabled()){
                static_cast<QLineEdit*>(w)->setFocus();
            }
        }
        return true;
    }
    return false;
}
void widget_wrapper::showSearch(bool sh){
    if(sh)showSearch();
    else hideSearch();
}
void widget_wrapper::showSearch(){
    if(edit==nullptr){
        qDebug()<<"NO EDIT CREATED!!!!!!";
        return;
    }
    show_search=true;
    set_edit();
  if(!edit->isEnabled())edit->setEnabled(true);
    if(!edit->hasFocus())edit->setFocus();
    edit->update();
}
void widget_wrapper::hideSearch(){
    if(edit){
        if(edit->keyboardGrabber())
            edit->releaseKeyboard();
    }
}
void widget_wrapper::showSelectors(bool s){
    if(selectors_visible!=s){
        selectors_visible=s;
        update();
    }
}
void widget_wrapper::make_edit(){
    if(!edit){
        qDebug()<<"AETTING EDIT";
        edit=new QLineEdit(this);
        set_edit();
    }
    edit->setGeometry(width()/10,height()/10,width()*.8,height()/10);
    edit->setFixedSize(width()*.8,height()/20);
    edit->setStyleSheet("border:black;color:rgba(255,255,255,255);background:rgba(0,0,0,20);");
    enableSearch();
}
void widget_wrapper::set_edit(){
    edit->setAlignment(Qt::AlignCenter);
    edit->setInputMethodHints(Qt::ImhLowercaseOnly | Qt::ImhNoTextHandles | Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    connect(edit,SIGNAL(textChanged(const QString&)),this,SIGNAL(textChanged(const QString&)));
    connect(edit,SIGNAL(textChanged(const QString&)),this,SLOT(update()));
    edit->setFocus();
}
void widget_wrapper::setupWrapper(QList<QList<QWidget*>> wdl){
    wlist.clear();
    wlist.append(wdl);
    for(int j=0;j<wdl.count();j++){
        for(int i=0;i<wlist.at(j).count();i++){
            QPoint tpos(wlist.at(j).at(i)->mapToGlobal(QPoint(0,0)));
            if(j==1 || j==2)
                wlist.at(j).at(i)->setParent(this);
            wlist.at(j).at(i)->move(tpos);
            update();
            QList<QLineEdit*> elist=wlist.at(j).at(i)->findChildren<QLineEdit *>(QString(),Qt::FindChildOptions(Qt::FindChildrenRecursively));
            if(elist.count() && edit==nullptr)
                if(QString::fromLatin1(elist.at(0)->metaObject()->className())=="QLineEdit"){
                    edit=static_cast<QLineEdit *>(elist.at(0));
                    qDebug()<<"EDIT:"<<edit;
                }
        }
    }
    ready_to_paint=true;
    if(!isVisible()){
        qDebug()<<"SHOW CONTAINER";
        show();
    }
    update();
    QApplication::processEvents();
}
void widget_wrapper::enableSearch(){
    issrch=true;
    showSearch(true);
}
void widget_wrapper::disableSearch(){
    issrch=false;
    QApplication::inputMethod()->hide();
}
bool widget_wrapper::is_search(){
    return (edit!=nullptr)*issrch;
}
void widget_wrapper::resizeEvent(QResizeEvent *e){
    resizeGL(e->size().width(),e->size().height());
}
void widget_wrapper::paintGL(){
    if(ready_to_paint){
        QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
        f->glClear(GL_COLOR_BUFFER_BIT);
        QPainter painter(this);
        //   painter.setRenderHints(QPainter::HighQualityAntialiasing,QPainter::SmoothPixmapTransform);
        for(int j=0;j<wlist.count();j++){
            if((!selectors_visible && j==0) || (selectors_visible && j==3) || (!selectors_visible && j==2)){
                continue;
            }
            for(int i=0;i<wlist.at(j).count();i++){
                QWidget* w=wlist.at(j).at(i);
                if(w->isEnabled()){
                    if(rect().intersects(QRect(w->pos(),w->size()))){
                        w->render(&painter,w->pos(),w->visibleRegion(),RenderFlags(DrawChildren | DrawWindowBackground));
                    }
                }
            }
        }
        painter.end();
    } else
        qDebug()<<"Container not ready";
}
widget_wrapper::~widget_wrapper(){
    qDebug()<<"DESTROYER:WWRAP";
}
