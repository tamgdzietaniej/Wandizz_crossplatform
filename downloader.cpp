#include "downloader.h"

downloader::downloader(QObject *parent) : QObject(parent)
{
    t=new QTimer(this);
    connect(t,SIGNAL(timeout()),this,SLOT(begin_download()),Qt::QueuedConnection);
    mthread=new QThread();
    writer=new write_buffers();
    //  connect(writer,SIGNAL(destroyed()),mthread,SLOT(quit()));
    writer->moveToThread(mthread);
    connect(mthread,SIGNAL(started()),writer,SLOT(clear()));
    mthread->start();
    manager=new QNetworkAccessManager();
    connect(mthread, SIGNAL(finished()), writer, SLOT(deleteLater()));
    connect(manager,SIGNAL(finished(QNetworkReply*)),writer,SLOT(get_data(QNetworkReply*)),Qt::QueuedConnection);
    connect(writer, SIGNAL(process()), this, SIGNAL(got_data()),Qt::QueuedConnection);
    connect(writer,SIGNAL(got_data()),this,SIGNAL(got_data()));
    writer->clear();
    dact=false;
}
void downloader::clearList(){
    urllist.clear();
}
void downloader::push_url(QString url,QString fn){
    qDebug()<<"DN:"<<url;
    if(!QUrl(url).isValid()){
        qDebug()<<"WAND:DN:ERROR:WRONG URL FORMAT:"<<url;
        return;
    }
    if(urllist.contains(url)){
        qDebug()<<"LIST CONTAINS:"<<url;
        return;
    }
    urllist.append(url);

    Task* task=new Task;

    task->url=url;
    task->file.setFileName(fn);
    writer->add_task(task);
    total++;
    pending++;
    if(!t->isActive()){
        t->start(50);
    }
}
void downloader::begin_download(){
    if(dact){
        qDebug()<<"DOWNLOADER SKIP BEGIN";
        return;
    }
    dact=true;
    if(write_buffers::tasks.count()>0 && write_buffers::active<max_active){
        write_buffers::active++;
        QUrl url=write_buffers::tasks.first()->url;
        int sys_id=writer->set_task();
        QNetworkRequest req(url);
        qDebug()<<"downloader:"<<url;
        //    req->setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute,true);
        //    req->setRawHeader("Connection","Keep-Alive");
        //     req->setRawHeader("Keep-Alive","timeout=30, max=100;");
        QNetworkReply* reply(manager->get(req));
        reply->setObjectName(QString::number(sys_id));
        if(write_buffers::active<max_active && t->interval()!=5){
            t->setInterval(50);

        }
    } else {
        if(write_buffers::tasks.count()==0){
            t->stop();
        } else {
            t->setInterval(250);
        }
    }
    dact=false;
}
downloader::~downloader(){
    exiting=true;
    qDebug()<<"WAND:DN:DESTR";
    mthread->exit();
    //   mthread->wait(5000);
}
