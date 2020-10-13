#include "acr_api.h"

acr_api::acr_api(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
    getter=new QNetworkAccessManager(this);
    eol="\n";
    http_method="POST";
    signature_version="1" ;
    root_url=serverRoot.toLatin1()+"/acr/";
    access_key_url=root_url.append("key");
    access_project_url=root_url.append("access_project");
}

void acr_api::sendFile(QString filename, QString data_type,QString http_uri){
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType,this);
    multiPart->setContentType(QHttpMultiPart::FormDataType);
    timestamp=getTimestamp();
    file = new QFile(filename,this);
    if(!file->open(QIODevice::ReadOnly))qDebug() << "Błąd otwarcia pliku";
    qDebug() << "ACR_API: plik: " << filename;
    sample=file->readAll();
    qb_sample_bytes=getSampleSize(sample);
    file->setParent(multiPart);

    if(http_uri=="/v1/identify"){
        downloaderStd* downloader=new downloaderStd(this);
        access_secret_project=downloader->download(serverRoot+"/acr/acr_actions/ac_key");
        downloader->downloaded_data.clear();
        access_key=downloader->download(serverRoot+"/acr/acr_actions/key");
        downloader->downloaded_data.clear();
        ident_serv=downloader->download(serverRoot+"/acr/acr_actions/ident");

            qDebug()<<"key:"<<access_key;
            qDebug()<<"sec:"<<access_secret_project;

        requrl=QUrl(ident_serv.append("/v1/identify"));
        qDebug() << "pobieram dane autoryzacji.";

        //access_key="3d0562a8ff37f92ef4088666ba53dae2";
        //   access_secret_project="rmoFsKnnYn04EzpK2O5izuHNzpCuOcs9ZHKSzof3";
        // access_key=getFile(access_key_url);
        qb_data_type=data_type.toLatin1();
        dts=http_method+eol+http_uri+eol+access_key+eol+data_type+eol+signature_version+eol+timestamp;
        data_to_sign=dts.toLatin1();
        signature=hmacSha1(access_secret_project, data_to_sign ).toBase64();
        multiPart->append(addPart("sample",sample));
        multiPart->append(addPart("sample_bytes",qb_sample_bytes));
        multiPart->append(addPart("access_key",access_key));
        multiPart->append(addPart("data_type",qb_data_type));
        multiPart->append(addPart("signature",signature));
        multiPart->append(addPart("signature_version",signature_version));
        multiPart->append(addPart("timestamp",timestamp));
    }
    qDebug() << "Upload: " <<QUrl::fromPercentEncoding(requrl.toEncoded());
    request.setUrl(requrl);
    networkManager->post(request,multiPart);
}


// TIME * * * getTIMESTAMP
QByteArray acr_api::getTimestamp(){
    return QString::number(currtime.currentMSecsSinceEpoch()).toLatin1();
}

QByteArray acr_api::getSampleSize(QByteArray& s){
    return QString::number(s.length()).toLatin1();
}
QHttpPart acr_api::addPart(QString name, QByteArray data){
    QHttpPart hp;
    hp.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\""+name+"\""));
    hp.setBody(data);
    return hp;
}
QByteArray acr_api::hmacSha1(QByteArray key, QByteArray baseString)
{
    int blockSize = 64; // HMAC-SHA-1 block size, defined in SHA-1 standard
    if (key.length() > blockSize) { // if key is longer than block size (64), reduce key length with SHA-1 compression
        key = QCryptographicHash::hash(key, QCryptographicHash::Sha1);
    }
    QByteArray innerPadding(blockSize, char(0x36)); // initialize inner padding with char "6"
    QByteArray outerPadding(blockSize, char(0x5c)); // initialize outer padding with char "quot;
    for (int i = 0; i < key.length(); i++) {
        innerPadding[i] = innerPadding[i] ^ key.at(i); // XOR operation between every byte in key and innerpadding, of key length
        outerPadding[i] = outerPadding[i] ^ key.at(i); // XOR operation between every byte in key and outerpadding, of key length
    }
    QByteArray total = outerPadding;
    QByteArray part = innerPadding;
    part.append(baseString);
    total.append(QCryptographicHash::hash(part, QCryptographicHash::Sha1));
    QByteArray hashed = QCryptographicHash::hash(total, QCryptographicHash::Sha1);
    return hashed;
}
acr_api::~acr_api(){
    qDebug()<<"~acr_api()";
}
