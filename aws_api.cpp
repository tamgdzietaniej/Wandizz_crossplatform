#include "aws_api.h"

aws_api::aws_api(QObject *parent) : QObject(parent)
{
    qDebug()<<"AWS_API::CONSTRUCT::>";
    sec_key="i+STkvBvbA5HrJeNAWZ1XtR7zkOTgKjw+NBC8iTj";
    //sec_key = "AWS4wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY";
    //QString key="AKIAJBNDA5UKRCAZ5JQQ";
    //QString key="AKIAJBNDA5UKRCAZ5JQQ";
    region_name="us-east-1";
    service_name = "AWSECommerceService";
    request_version="aws4_request";

}

QString aws_api::doSignature(QByteArray timestamp) {
    QByteArray kDate = QMessageAuthenticationCode::hash(timestamp, sec_key, QCryptographicHash::Sha256);
 //   qDebug() << "kdate:" << kDate.toHex();
    QByteArray kRegion = QMessageAuthenticationCode::hash(region_name, kDate, QCryptographicHash::Sha256);
 //   qDebug()<<"kregion:"<<kRegion.toHex();
    QByteArray kService = QMessageAuthenticationCode::hash(service_name, kRegion, QCryptographicHash::Sha256);
  //  qDebug()<<"kService:"<<kService.toHex();
    QByteArray kSigning = QMessageAuthenticationCode::hash(request_version,kService, QCryptographicHash::Sha256);
    qDebug()<<"kSigning aws_api return:"<<kSigning;
    return kSigning;
}

QByteArray aws_api::getTimestamp(){

    return QString::number(currtime.currentMSecsSinceEpoch()).toLatin1();
}

