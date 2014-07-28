#ifndef SITESDATABASEINTERFACE_H
#define SITESDATABASEINTERFACE_H

#include "site.h"
#include <QList>
#include <QString>

class SitesDatabaseInterface
{
private:
    QString appStoragePath;
    char* dbPath;

public:
    SitesDatabaseInterface();
    void setStoragePaths(QString appStoragePath, QString dbPath);
    void saveSite(Site* site);
    QList<Site*> getAllSites();
};

#endif // SITESDATABASEINTERFACE_H
