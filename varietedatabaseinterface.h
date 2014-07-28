#ifndef DATABASEINTERFACE_H
#define DATABASEINTERFACE_H

#include "varietesananas.h"
#include "leveldb/db.h"

class VarieteDatabaseInterface
{
private:
    QString appStoragePath;
    QString imageStoragePath;
    char* dbPath;
    leveldb::DB* db;
    leveldb::Options options;
    leveldb::ReadOptions readOptions;
    leveldb::WriteOptions writeOptions;
    leveldb::Status status;

public:
    VarieteDatabaseInterface();
    void setStoragePaths(QString appStoragePath, QString imageStoragePath, QString dbPath);
    void saveVariete(VarietesAnanas* variete);
    VarietesAnanas* getVarieteWithId(int id);
    QList<VarietesAnanas*> getAllvarietes();
    void deleteVariete(int id);

    void setdbPath(char* dbPath);
    char* getDbPath();
};

#endif // DATABASEINTERFACE_H
