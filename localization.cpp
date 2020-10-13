#include "localization.h"
localization::localization(QObject *parent) : QObject(parent)
{
    source = QGeoPositionInfoSource::createDefaultSource(this);
    source->setPreferredPositioningMethods(QGeoPositionInfoSource::AllPositioningMethods);
    source->setUpdateInterval(60000);
  //  qDebug()<<"SOU:"<<source;
    if(source){
        connect(source, SIGNAL (positionUpdated(QGeoPositionInfo)),this, SLOT (positionUpdated(QGeoPositionInfo)));
        source->startUpdates();
    }
}
void localization::positionUpdated(QGeoPositionInfo info)
{
    if(info.isValid() && prev_coords!=coords){
        prev_coords=coords;
        currentLocation = info.coordinate();
        QString lat=QString::number(currentLocation.latitude());
        QString lon=QString::number(currentLocation.longitude());
        coords="&lat="+lat+"&lon="+lon;

     //   qDebug() << "Current Latitude : " << currentLocation.latitude();
   //     qDebug() << "Current Longitude : " << currentLocation.longitude();
        emit new_coords(coords,lat,lon);
    }
}
localization::~localization(){
  //  qDebug()<<"GPS DESTRUCTOR";
}
