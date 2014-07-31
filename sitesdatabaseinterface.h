#ifndef SITESDATABASEINTERFACE_H
#define SITESDATABASEINTERFACE_H

#include "site.h"
#include <QList>
#include <QString>

class SitesDatabaseInterface
{
private:
    QString appStoragePath;
    QString dbPath;

public:
    SitesDatabaseInterface();
    void setStoragePaths(QString appStoragePath, QString dbPath);
    void saveSite(Site* site);
    QList<Site*> getAllSites();
    void deleteSite(int id);
    int lastInsertedRowId();
};

#endif // SITESDATABASEINTERFACE_H
