#ifndef SHARE_CROSS_H
#define SHARE_CROSS_H

#include <QObject>
#include <QUrl>
#include <QtAndroidExtras/QAndroidJniObject>


class share_cross : public QObject
{
    Q_OBJECT
public:
    explicit share_cross(QObject *parent = nullptr);

signals:

public slots:
    void share(QString,QUrl);
};

#endif // SHARE_CROSS_H
