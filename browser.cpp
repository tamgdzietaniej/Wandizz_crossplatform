#include "browser.h"

browser::browser(QWidget *parent):QWidget(parent)
{

    viewer=new QQuickView();
    viewer->setResizeMode(QQuickView::SizeRootObjectToView);
    setAttribute(Qt::WA_DeleteOnClose);
   // setAttribute(Qt::WA_TranslucentBackground);
   // setClearColor(QColor(0,0,0,0));
    update();
    qDebug()<<"BROWSER";
    setStyleSheet("background:rgba(0,0,0,0);color:rgba(255,255,255,255);");
    context=viewer->engine()->rootContext();
}
void browser::loadPage(QString url){
    context->setContextProperty(QStringLiteral("ref"),url);
    context->setContextProperty(QStringLiteral("w"),width());
    context->setContextProperty(QStringLiteral("h"),height());
    update();
    updateGeometry();
    viewer->setSource(QUrl("qrc:/main.qml"));
}
