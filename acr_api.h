#ifndef ACR_API_H
#define ACR_API_H
#include <QFile>
#include <QtNetwork>
#include <QUrl>
#include <QDebug>
#include "downloaderStd.h"
#include "globals.h"
class acr_api : public QObject
{
public:
    explicit acr_api(QObject *parent = nullptr);
    ~acr_api();
    void sendFile(QString,QString,QString);
    QHttpPart addPart(QString,QByteArray);
    QByteArray getTimestamp();
    QByteArray getSampleSize(QByteArray&);
    QHttpMultiPart *multiPart=nullptr;

    QString finger_data_type,finger_uri;
public slots:
        void getPayload(QNetworkReply*);
        void get_secrets();
        void no_connection();
private:
    QUrlQuery postData;
    QByteArray hmacSha1(QByteArray,QByteArray);
    QByteArray access_key;
    QByteArray access_secret, access_secret_project, access_url, root_url;
    QByteArray timestamp;
    QByteArray signature;
    QByteArray qb_sample_bytes;
    QByteArray qb_data_type;
    QByteArray sample;
    QByteArray data_to_sign;
    QDateTime currtime;
    QByteArray signature_version;
    QString extra_data_if_cli,access_key_url, access_project_url;
    QString dts,eol,http_method,ident_serv;
    QFile* file,*tmp_file;
    QNetworkReply *reply;
    QNetworkRequest request;
    QUrl requrl;
signals:
    void finished();
public:
    QNetworkAccessManager *networkManager, *getter;
    QByteArray glo_payload;
    bool is_getter_finished;

};

#endif // ACR_API_H
