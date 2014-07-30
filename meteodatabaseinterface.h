#ifndef METEODATABASEINTERFACE_H
#define METEODATABASEINTERFACE_H

#include <QString>
#include <QList>
#include "meteo.h"

class MeteoDatabaseInterface
{
private:
    QString meteoStoragePath;
    QString dbPath;

public:
    MeteoDatabaseInterface();
    void deleteMeteo(int siteId, int year);
    void saveMeteo(Meteo* meteo);
    QList<Meteo*> getMeteo(int siteId, int year);

    void setMeteoStoragePath(QString meteoStoragePath);
};

#endif // METEODATABASEINTERFACE_H
