#ifndef AWS_API_H
#define AWS_API_H
#include <QCryptographicHash>
#include <QMessageAuthenticationCode>
#include <QDebug>
#include <QTime>

class aws_api : public QObject
{
    Q_OBJECT
public:
    explicit aws_api(QObject *parent = nullptr);
    QString doSignature(QByteArray);
    QString encode_param(QString);
signals:

public slots:

private:
    QByteArray sec_key,service_name,region_name,request_version;
    QByteArray getTimestamp();
    QDateTime currtime;

};
#endif // AWS_API_H
