#include "write_buffers.h"
QList<Task*> write_buffers::sent;
QList<Task*> write_buffers::done;
QList<Task*> write_buffers::tasks;
int write_buffers::active;

write_buffers::write_buffers(QObject* parent) :
    QObject(parent),
    debg(false),
    pcnt(0),
    npcnt(0),
    loaded(0),
    wrote(0),
    get_cnt(0),
    alldata(0),
    done_cnt(0),
    sysid(0)
{
    timer=new QTimer();
    connect(this,SIGNAL(timerstop()),timer,SLOT(stop()),Qt::QueuedConnection);
    connect(this,SIGNAL(timerstart()),timer,SLOT(start()),Qt::QueuedConnection);
    connect(timer,SIGNAL(timeout()),this,SLOT(process_buffer()),Qt::QueuedConnection);
    timer->start(200);
}

void write_buffers::clear(){
    write_buffers::tasks.clear();
    write_buffers::sent.clear();
    write_buffers::done.clear();
}
void write_buffers::add_tasks(QList<Task*> t){
    write_buffers::tasks.clear();
    write_buffers::tasks.append(t);
}
int write_buffers::add_task(Task* task){
    write_buffers::tasks.append(task);
    return write_buffers::tasks.count();
}
int write_buffers::set_task(){
    QMutexLocker {&_mutex};
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
                qDebug()<<"Reply:"<<rep->bytesAvailable()<<rep->request().url();
                QFile &local_file=sent.at(i)->file;
                QVariant pip= rep->attribute(QNetworkRequest::HttpPipeliningWasUsedAttribute);
                bool file_exists(local_file.exists());
                bool file_expired(false);
                if(pip.toBool())pcnt++;
                else npcnt++;
                if(file_exists){
                    if(local_file.size()!=0){
                        local_stamp=QDateTime(QFileInfo(local_file.fileName()).lastModified()).toUTC();
                        remote_stamp=rep->header(QNetworkRequest::KnownHeaders::LastModifiedHeader).toDateTime();
                        qDebug()<<"TIME:"<<rep->request().url()<<local_stamp<<remote_stamp;
                        file_expired=(remote_stamp>local_stamp);
                        if(file_expired)qDebug()<<"----------------->Must revalidate<------------------------"<<rep->request().url()<<sent.at(i)->reval;
                    } else {
                        if(local_file.size()==0 || file_expired){
                            qDebug()<<"----------------->Must remove zero bytes:"<<local_file.fileName()<<"<------------------------"<<rep->request().url()<<sent.at(i)->reval;
                            local_file.remove();
                        }
                    }
                }
                write_buffers::sent.at(i)->reply=rep->readAll();
                write_buffers::done.append(write_buffers::sent.takeAt(i));
                if(file_expired){
                    emit need_revalidate_widget(sent.at(i)->widget_id);
                }
                rep->close();
                rep->deleteLater();
                emit timerstart();
            }
            break;
        }
    }
    write_buffers::active--;
    if(active==0){
        qDebug()<<"------------------------------------------------------------------";
        qDebug()<<"Writer finished summary:"<<alldata<<" bytes ("<<alldata/1024<<"kB";
        qDebug()<<"-- Pipelinded:"<<pcnt<<" / not pipelined: "<<npcnt<<"--";
        qDebug()<<"------------------------------------------------------------------";
        npcnt=0;
        pcnt=0;
        //  emit finished();
    }
}
void write_buffers::process_buffer(){
    if(write_buffers::done.count()==0){
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
        if(debg)qDebug()<<"WRITE FILE "<<t->file.fileName()<<"exists!!!!!!!!!!!!!";
    }
    if(t->reply.left(8).toHex(':')!="89:50:4e:47:0d:0a:1a:0a"){
        qDebug()<<"!!!!!!!! FILE FORMAT ERROR:"<<t->reply.left(8).toHex(':')<<" - "<<t->file.fileName()<<t->url;
        t->file.remove();
    } else {
        QFile* tmpf=new QFile(t->file.fileName()+".lock");
        tmpf->open(QIODevice::WriteOnly);
        tmpf->write(QByteArray());
        tmpf->close();
        t->file.open(QFile::WriteOnly);
        t->file.write(t->reply);
        t->file.close();
        QFile::remove(t->file.fileName()+".lock");
        qDebug()<<"Wrote:"<<t->file.fileName()<<t->file.size();
    }
    delete t;
    get_cnt--;
}

write_buffers::~write_buffers(){
    qDebug()<<"WB:DESTR";
}
