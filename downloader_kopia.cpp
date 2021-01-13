/*
#include "downloader_kopia.h"

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
    manager=new QNetworkAccessManager(this);
    request=new QNetworkRequest();
    connect(mthread, SIGNAL(finished()), writer, SLOT(deleteLater()));
    connect(manager,SIGNAL(finished(QNetworkReply*)),writer,SLOT(get_data(QNetworkReply*)),Qt::QueuedConnection);
    connect(writer,SIGNAL(finished()),this,SLOT(mid_process()),Qt::QueuedConnection);
    connect(writer, SIGNAL(process()), this, SIGNAL(got_data()),Qt::QueuedConnection);
    connect(writer,SIGNAL(got_data()),this,SIGNAL(got_data()));
    connect(writer,SIGNAL(need_revalidate_widget(int)),this,SIGNAL(need_revalidate_widget(int)),Qt::QueuedConnection);
    writer->clear();
    dact=false;
    files_watchdog=false;
}
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
void downloader::clearList(){
    urllist.clear();
    total=0;
}
void downloader::run_watchdog(){
    clearList();
    files_watchdog=true;
    for(int i=0;i<task_helper.count();i++){
        push_url(task_helper.at(i).url,task_helper.at(i).fname,task_helper.at(i).id);
    }
    qDebug()<<"mytasks:"<<mytasks;
    t->start();

}
void downloader::push_url(const QString& url, const QString& fn){
    push_url(url,fn,-1);
}
void downloader::push_url(const QString& url,const QString& fn, int index){
    if(!QUrl(url).isValid()){
        qDebug()<<"WAND:DN:ERROR:WRONG URL FORMAT:"<<url;
        return;
    }
    if(urllist.contains(url)){
        qDebug()<<"LIST CONTAINS:"<<url<< "continuing anyway";
    }
    urllist.append(url);
    Task task;
    task.url=url;
    task.file.setFileName(fn);
    task.widget_id=index;
    writer->add_task(&task);
    if(!files_watchdog){
        TaskHelper th;
        th.url=url;
        th.fname=fn;
        th.id=index;
        task_helper.append(th);
    }
    total++;
    pending++;
    if(!t->isActive()){
        t->start(2000);
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
        if(files_watchdog){
            qDebug()<<"WB:"<<write_buffers::tasks.count();
            qDebug()<<"WB:"<<write_buffers::tasks;

        }
        QUrl url=write_buffers::tasks.first()->url;
        int sys_id=writer->set_task();
        qDebug()<<"Url:"<<url;
        request->setUrl(url);
        request->setRawHeader("Connection","keep-alive");
        request->setRawHeader("Keep-Alive","timeout=30, max=100;");
        request->setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute,true);
        QNetworkReply* reply(manager->get(*request));
        reply->setObjectName(QString::number(sys_id));
        if(write_buffers::active<max_active && t->interval()!=5){
            t->setInterval(25);
        } else {
            t->setInterval(500);
        }
    }
    dact=false;
}
void downloader::stop_watchdog(){
    files_watchdog=false;
    urllist.clear();
    task_helper.clear();
    qDebug()<<"Stop watchdog";
}
downloader::~downloader(){
    exiting=true;
    qDebug()<<"WAND:DN:DESTR";
    mthread->exit();
    //   mthread->wait(5000);
}
*/
