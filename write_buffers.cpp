#include "write_buffers.h"
QList<Task*> write_buffers::sent;
QList<Task*> write_buffers::done;
QList<Task*> write_buffers::tasks;
int write_buffers::active;

write_buffers::write_buffers(QObject* parent) :
    QObject(parent),
    sysid(0),
    alldata(0),
    get_cnt(0),
    done_cnt(0),
    busy(false)
{
    timer=new QTimer();
    timer->start(100);
    connect(timer,SIGNAL(timeout()),this,SLOT(process_buffer()),Qt::QueuedConnection);
    connect(this,SIGNAL(process()),this,SLOT(process_buffer()));
    connect(this,SIGNAL(timerstop()),timer,SLOT(stop()),Qt::QueuedConnection);
    connect(this,SIGNAL(timerstart()),timer,SLOT(start()),Qt::QueuedConnection);

}
void write_buffers::clear(){
    write_buffers::tasks.clear();
    write_buffers::sent.clear();
    write_buffers::done.clear();
}
int write_buffers::add_task(Task* task){
    write_buffers::tasks.append(task);
    return write_buffers::tasks.count();
}
int write_buffers::set_task(){
    if(sysid>1000)sysid=0;
    write_buffers::tasks.first()->sys_id=sysid;
    write_buffers::sent.append(write_buffers::tasks.takeFirst());
    return sysid++;
}
void write_buffers::get_data(QNetworkReply* rep){
    alldata+=rep->bytesAvailable();
    int sys_id=rep->objectName().toInt();
    for(int i=0;i<write_buffers::sent.count();i++){
        if(write_buffers::sent.at(i)->sys_id==sys_id){
            if(rep->objectName()=="" || rep->bytesAvailable()==0 || rep->error()!=QNetworkReply::NoError){
                qDebug()<<"WB:ERROR:Network:ID:"<<rep->objectName()<<",error:"<<rep->errorString()<<",bytes:"<<rep->bytesAvailable();
                write_buffers::sent.removeAt(i);
            } else {
                write_buffers::sent.at(i)->reply=rep->readAll();
                write_buffers::done.append(write_buffers::sent.takeAt(i));
                rep->close();
                rep->deleteLater();
                if(!timer->isActive())emit timerstart();
            }
            break;
        }
    }
    write_buffers::active--;
}
void write_buffers::process_buffer(){
    if(done.count()==0){
        emit timerstop();
        return;
    }
    if(get_cnt>3){
        qDebug()<<"Write delay";
        return;
    }
    get_cnt++;
    Task* t=write_buffers::done.takeFirst();
    if(QFileInfo::exists(t->file.fileName())){
        qDebug()<<"FILE "<<t->file.fileName()<<"exists!!!!!!!!!!!!!";

    }
    if(t->reply.left(8).toHex(':')!="89:50:4e:47:0d:0a:1a:0a"){
        qDebug()<<"!!!!!!!! FILE FORMAT ERROR:"<<t->reply.left(8).toHex(':')<<" - "<<t->file.fileName()<<t->url;
    } else {
        QFile* tmpf=new QFile(t->file.fileName()+".lock");
        tmpf->open(QIODevice::WriteOnly);
        tmpf->write(QByteArray());
        tmpf->close();
        t->file.open(QFile::WriteOnly);
        t->file.write(t->reply);
        t->file.close();
        QFile::remove(t->file.fileName()+".lock");
    }
    delete t;
    get_cnt--;
}

write_buffers::~write_buffers(){

    qDebug()<<"WB:DESTR";
}
