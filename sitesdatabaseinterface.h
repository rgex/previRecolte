#ifndef SITESDATABASEINTERFACE_H
#define SITESDATABASEINTERFACE_H

#include "site.h"
#include <QList>
#include "leveldb/db.h"
#include <QString>

class SitesDatabaseInterface
{
private:
    QString appStoragePath;
    leveldb::DB* db;
    leveldb::Options options;
    leveldb::ReadOptions readOptions;
    leveldb::WriteOptions writeOptions;
    leveldb::Status status;

public:
    SitesDatabaseInterface();
    void setStoragePaths(QString appStoragePath);
    void saveSite(Site* site);
    QList<Site*> getAllSites();
};

#endif // SITESDATABASEINTERFACE_H
