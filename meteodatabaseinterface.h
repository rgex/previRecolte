#ifndef METEODATABASEINTERFACE_H
#define METEODATABASEINTERFACE_H

#include <QString>
#include "leveldb/db.h"
#include "meteo.h"

class MeteoDatabaseInterface
{
private:
    QString meteoStoragePath;

public:
    MeteoDatabaseInterface();
    void deleteMeteo(QString siteKey, int year);
    void saveMeteo(QString siteKey, int year, Meteo* meteo);
    QList<Meteo*> getMeteo(QString siteKey, int year);

    void setMeteoStoragePath(QString meteoStoragePath);
};

#endif // METEODATABASEINTERFACE_H
