#include "mhelper.h"


mHelper::mHelper(QObject *parent) : QObject(parent)
{

}
void mHelper::stop(){

}
void mHelper::setParams(QSize s, Qt::AspectRatioMode m){
    msize=s;
    aspect=m;
}
void mHelper::file2image(const int id,const QString& fname){
    if(QFileInfo::exists(fname)){
       const QImage tmp(QImage(fname).scaled(msize,aspect,Qt::SmoothTransformation));
       emit imageReady(id,tmp);
    } else emit noImage();
}
mHelper::~mHelper(){
    qDebug()<<"MHELPER DESTR";
}
