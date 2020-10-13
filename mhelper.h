#ifndef MHELPER_H
#define MHELPER_H

#include <QObject>
#include <QImage>
#include <QDebug>
#include <QFileInfo>

class mHelper : public QObject
{
    Q_OBJECT
public:
    explicit mHelper(QObject *parent = nullptr);
      ~mHelper();
    void setParams(QSize,Qt::AspectRatioMode);
    void stop();
public slots:
    void file2image(const int,const QString&);
private:
    Qt::AspectRatioMode aspect;
    QSize msize;
signals:
    void imageReady(const int,const QImage&);
    void noImage();

};

#endif // MHELPER_H
