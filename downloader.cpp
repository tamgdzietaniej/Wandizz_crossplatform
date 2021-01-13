#include "downloader.h"

downloader::downloader(QObject *parent) : QObject(parent)
{
    t=new QTimer(this);
 //   connect(t,SIGNAL(timeout()),this,SLOT(begin_download()),Qt::QueuedConnection);
    mthread=new QThread();
    writer=new write_buffers();
    //  connect(writer,SIGNAL(destroyed()),mthread,SLOT(quit()));
    writer->moveToThread(mthread);
    connect(mthread,SIGNAL(started()),writer,SLOT(clear()));
    mthread->start();
    manager=new QNetworkAccessManager(this);
    request=new QNetworkRequest();
    request->setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute,true);
    connect(mthread, SIGNAL(finished()), writer, SLOT(deleteLater()));
    connect(manager,SIGNAL(finished(QNetworkReply*)),writer,SLOT(get_data(QNetworkReply*)),Qt::QueuedConnection);
    connect(writer, SIGNAL(process()), this, SIGNAL(got_data()),Qt::QueuedConnection);
    connect(writer,SIGNAL(got_data()),this,SIGNAL(got_data()));
    dact=false;
}
/*
void downloader::mid_process(){
    total--;
    qDebug()<<"GOTDATA:TOTAL LEFT:"<<total;
    //     QTimer::singleShot(150,this,[=](){
    if(total==0){
        if(files_watchdog){
            QTimer::singleShot(15000,this,[=](){
                run_watchdog();
            });
        } else {
            run_watchdog();
        }
    }

}

void downloader::run_watchdog(){
    clearList();
    files_watchdog=true;
    for(int i=0;i<task_helper.count();i++){
        push_url(task_helper.at(i).url,task_helper.at(i).fname,task_helper.at(i).id,bool);
    }
    qDebug()<<"mytasks:"<<mytasks;
    t->start();

}
*/
void downloader::clearList(){
    urllist.clear();
}
void downloader::push_url(const QString& url, const QString& fn){
    push_url(url,fn,-1,0);
}
void downloader::push_url(const QString& url,const QString& fn, int index,bool rev){
    //  qDebug()<<"DN:"<<url;
    if(!QUrl(url).isValid()){
        qDebug()<<"WAND:DN:ERROR:WRONG URL FORMAT:"<<url;
        return;
    }
  //  if(urllist.contains(url)){
  //      qDebug()<<"LIST CONTAINS:"<<url;
  //      return;
  //  }
    urllist.append(url);

    Task* task=new Task;
    task->widget_id=index;
    task->url=url;
    task->file.setFileName(fn);
    task->reval=rev;
    writer->add_task(task);
    total++;
    pending++;
    if(!dact)begin_download();
   // if(!t->isActive()){
   //     t->start(50);
  //  }
}
void downloader::begin_download(){
    if(dact){
        qDebug()<<"DOWNLOADER SKIP BEGIN";
        return;
    }
    dact=true;
    while(write_buffers::tasks.count()>0 && write_buffers::active<max_active){
        write_buffers::active++;
        QUrl url=write_buffers::tasks.first()->url;
        int sys_id=writer->set_task();
        request->setUrl(url);
        //   qDebug()<<"downloader:"<<url;
        request->setRawHeader("Connection","Keep-Alive");
        request->setRawHeader("Keep-Alive","timeout=30, max=100;");
        QNetworkReply* reply(manager->get(*request));
        reply->setObjectName(QString::number(sys_id));
        QApplication::processEvents();
    //    if(write_buffers::active<max_active && t->interval()!=5){
     //       t->setInterval(50);
     //   }
   // } else {
    //    if(write_buffers::tasks.count()==0){
    //        t->stop();
    //    }
   }
    if(write_buffers::tasks.count()>0 && write_buffers::active<max_active){
        qDebug()<<"MARAUDER!"<<write_buffers::tasks.count();
        begin_download();
    }

    dact=false;
}
downloader::~downloader(){
    exiting=true;
    qDebug()<<"WAND:DN:DESTR";
    mthread->exit();
    //   mthread->wait(5000);
}
