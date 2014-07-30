#ifndef METEODATABASEINTERFACE_H
#define METEODATABASEINTERFACE_H

#include <QString>
#include <QList>
#include "meteo.h"

class MeteoDatabaseInterface
{
private:
    QString appStoragePath;
    QString dbPath;

public:
    MeteoDatabaseInterface();
    void deleteMeteo(int siteId, int year);
    void saveMeteo(Meteo* meteo);
    QList<Meteo*> getMeteo(int siteId, int year);

    void setStoragePaths(QString appStoragePath, QString dbPath);
};

#endif // METEODATABASEINTERFACE_H
