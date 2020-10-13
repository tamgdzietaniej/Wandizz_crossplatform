#include "downloaderStd.h"
downloaderStd::downloaderStd(QObject *parent) :QObject (parent), offline(false){
    connect(&manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(get_data(QNetworkReply*)),Qt::QueuedConnection);
}
void downloaderStd::get_data(QNetworkReply* reply){
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(error(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
            this, SLOT(ssl_error(QList<QSslError>)));
    if(reply->bytesAvailable()>0){
        downloaded_data.append(reply->readAll());
        offline=false;
    } else {
        if(reply->errorString().toLower()!="unknown error"){
            offline=true;
        }
    }
    emit got_data();
}
QByteArray downloaderStd::download(QString _urli){
    request.setUrl(_urli);
    QEventLoop el;
    connect(this,SIGNAL(got_data()),&el,SLOT(quit()));
    QNetworkReply* rply=manager.get(request);
    connect(rply,SIGNAL(downloadProgress(qint64,qint64)),this,SIGNAL(progress(qint64,qint64)));
    el.exec();
    return downloaded_data;
}
void downloaderStd::set_offline(){
    if(downloaded_data.isEmpty()){
        if(offline){
            qDebug()<<"Sorry, no data in cache!";
            QApplication::exit();
        }
        qDebug()<<"DOWNLOADER:SETTING OFFILNE";
        emit offline_mode(true);
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysCache);
        qDebug()<<"offline, trying retrieve data from cache.";
        offline=true;
    }
}
downloaderStd::~downloaderStd(){
}
void downloaderStd::error(QNetworkReply::NetworkError err){
    qDebug()<<"Downloader error: "<<err;
}
void downloaderStd::ssl_error(QList<QSslError> sslerr){
    qDebug()<<"!!! SSL ERRORS: "<<sslerr;
}
