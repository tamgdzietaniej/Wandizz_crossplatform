#ifndef LOCALIZATION_H
#define LOCALIZATION_H
#include <QGeoLocation>
#include <QGeoCoordinate>
#include <QGeoPositionInfoSource>
#include<QObject>
class localization : public QObject
{
    Q_OBJECT
public:
    explicit localization(QObject *parent = nullptr);
        ~localization();

    QGeoCoordinate getCurrentLocation();
    QGeoCoordinate currentLocation;
    QGeoPositionInfoSource *source;

    QString coords="&lat=&lon=";
    QString prev_coords="";

signals:
    void new_coords(QString,QString,QString);
    public slots:

    void positionUpdated(QGeoPositionInfo info);
};

#endif // LOCALIZATION_H
