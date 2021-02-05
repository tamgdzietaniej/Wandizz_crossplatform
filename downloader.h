#ifndef DOWNLOADER_H
#define DOWNLOADER_H
#include <QApplication>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QImage>
#include <QList>
#include <QThread>
#include "write_buffers.h"
#include "globals.h"
#include "gllabel.h"
class downloader : public QObject
{
    Q_OBJECT
public:
    explicit downloader(QObject *parent = nullptr);
    ~downloader();
    void start();
    //  bool curr_manager;
    bool exiting;
    QTimer* timer;
    QStringList urllist,surllist;
    void clearList();
    void stop_watchdog();
    struct TaskHelper{
        int id;
        QString fname;
        QString url;
    };
    QList<TaskHelper> task_helper;

private:
    QNetworkAccessManager *manager;
    QNetworkRequest* request;
    QTimer* t;
    int pending;
    int total;
    int processed;
    int processing;
    bool vr=false,dact;
    qint64 alldata;
    QThread twriter;
    write_buffers* writer;
    QThread* mthread;
    QList<Task*> mytasks;
    bool files_watchdog;

signals:
    void error(QString);
    void got_data();
    void got_reply();
    void  need_revalidate_widget(int);
public slots:
    void push_url(const QString&,const QString&);
    void push_url(const QString&, const QString&, int,bool);
  //  void run_watchdog();
private slots:
    void begin_download();
 //   void mid_process();

};



#endif // DOWNLOADER_H
