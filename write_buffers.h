#ifndef WRITE_BUFFERS_H
#define WRITE_BUFFERS_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QFile>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QImageReader>
#include <globals.h>

struct Task{
    int sys_id;
    int widget_id;
    QString url;
    QByteArray reply;
    QFile file;
    bool reval;
};

class write_buffers : public QObject
{
    Q_OBJECT
public:
    write_buffers(QObject* parent=nullptr);
    ~write_buffers();
    static QList<Task*> tasks,sent,done;
    int set_task();
    static int active;
    bool busy,debg;
    int add_task(Task*);
    void add_tasks(QList<Task*>);
    int pcnt,npcnt;
    QDateTime remote_stamp,local_stamp;
private:
    QMutex _mutex;
    int loaded,wrote,get_cnt;
signals:
    void stored();
    void process();
    void timerstart();
    void timerstop();
    void got_data();
    void need_revalidate_widget(int);
    void finished();

public slots:
    void process_buffer();
    void get_data(QNetworkReply*);
    void clear();

private:
    QTimer* timer;
    int alldata,done_cnt,sysid;
};

#endif // WRITE_BUFFERS_H
