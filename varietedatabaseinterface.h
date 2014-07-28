#ifndef DATABASEINTERFACE_H
#define DATABASEINTERFACE_H

#include "varietesananas.h"

class VarieteDatabaseInterface
{
private:
    QString appStoragePath;
    QString imageStoragePath;
    char* dbPath;

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
