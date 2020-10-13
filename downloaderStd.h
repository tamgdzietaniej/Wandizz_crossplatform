#ifndef DOWNLOADERSTD_H
#define DOWNLOADERSTD_H
#include <QApplication>
#include <QTimer>
#include <QObject>
#include <QtNetwork>
#include <QNetworkDiskCache>
#include <QImageWriter>
#include <QImage>
class downloaderStd : public QObject
{
    Q_OBJECT
private:
    Q_DISABLE_COPY(downloaderStd)
public:
    explicit downloaderStd(QObject *parent = nullptr);
    ~downloaderStd();
    QNetworkAccessManager manager;
    QNetworkRequest request;
    QNetworkReply* reply;
    QByteArray download(QString);
    QString stats;
    bool connecting;
    bool offline=false;;
    QByteArray downloaded_data;
    QString url;

signals:
    void progress(qint64,qint64);
    void finish(const QByteArray&);
    void got_data();
    void offline_mode(bool);
    void no_response();
    void stop_el();

public slots:
    void set_offline();
    void get_data(QNetworkReply* reply);
    void error(QNetworkReply::NetworkError);
    void ssl_error(QList<QSslError>);
};

#endif // DOWNLOADER_H
