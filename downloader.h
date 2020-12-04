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
    QStringList urllist;
    void clearList();
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

signals:
    void error(QString);
    void got_data();
    void got_reply();

public slots:
    void push_url(QString,QString);

private slots:
    void begin_download();

};



#endif // DOWNLOADER_H
