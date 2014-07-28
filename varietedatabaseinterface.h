#ifndef DATABASEINTERFACE_H
#define DATABASEINTERFACE_H

#include "varietesananas.h"
#include "leveldb/db.h"

class VarieteDatabaseInterface
{
private:
    QString appStoragePath;
    QString imageStoragePath;
    leveldb::DB* db;
    leveldb::Options options;
    leveldb::ReadOptions readOptions;
    leveldb::WriteOptions writeOptions;
    leveldb::Status status;

public:
    VarieteDatabaseInterface();
    void setStoragePaths(QString appStoragePath, QString imageStoragePath);
    void saveVariete(VarietesAnanas* variete);
    VarietesAnanas* getVarieteWithKey(QString key);
    QList<VarietesAnanas*> getAllvarietes();
    void deleteVariete(QString key);
};

#endif // DATABASEINTERFACE_H
