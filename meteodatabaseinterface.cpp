#include "meteodatabaseinterface.h"
#include <QString>
#include "meteo.h"
#include <QDebug>

MeteoDatabaseInterface::MeteoDatabaseInterface()
{

}

void MeteoDatabaseInterface::deleteMeteo(QString siteKey, int year)
{

}

void MeteoDatabaseInterface::saveMeteo(QString siteKey, int year, Meteo* meteo)
{

}

QList<Meteo*> MeteoDatabaseInterface::getMeteo(QString siteKey, int year)
{

}

void MeteoDatabaseInterface::setMeteoStoragePath(QString meteoStoragePath)
{
    this->meteoStoragePath = meteoStoragePath;
}
