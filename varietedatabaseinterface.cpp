#include "varietedatabaseinterface.h"
#include "varietesananas.h"
#include <string>
#include <QDebug>
#include <sqlite3.h>


VarieteDatabaseInterface::VarieteDatabaseInterface()
{
}

void VarieteDatabaseInterface::setStoragePaths(QString appStoragePath, QString imageStoragePath, QString dbPath)
{
    this->appStoragePath = appStoragePath;
    this->imageStoragePath = imageStoragePath;

    char* nDbPath = new char [dbPath.size()+1];;
    strcpy(nDbPath, dbPath.toStdString().c_str());

    qDebug() << "db: " << dbPath;

    this->dbPath = nDbPath;

    qDebug() << "db: " << this->dbPath;

}

VarietesAnanas* VarieteDatabaseInterface::getVarieteWithId(int id)
{
    VarietesAnanas* variete = new VarietesAnanas(appStoragePath, imageStoragePath);

    qDebug() << "db: " << this->dbPath;
    sqlite3 *db;
    const char *zErrMsg = 0;
    int  rc;
    char *sql;
    sqlite3_stmt* stmt;
    QList<VarietesAnanas*> varieteList;

    /* Open database */
    rc = sqlite3_open(this->dbPath, &db);
    if( rc ){
       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
       exit(0);
    }else{
       fprintf(stdout, "Opened database successfully\n");
    }

    sql = sqlite3_mprintf("SELECT * FROM varietes WHERE ID =  '%q'", QString::number(id).toStdString().c_str());

    qDebug() << "sql request : " << sql;

    /* Execute SQL statement */
    rc = sqlite3_prepare_v2(db, sql, strlen(sql)+1, &stmt, &zErrMsg);
    if( rc != SQLITE_OK )
    {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
    }
    else
    {
       int result;
       int ID;
       QString nom;
       QString newImageName;
       float tBase1;
       float tFloraison;
       float tBase2;
       float tRecolte;
       do {
            result = sqlite3_step(stmt);
            if (result == SQLITE_ROW)
            { /* can read data */
                ID = sqlite3_column_int(stmt,0);
                nom = QString::fromUtf8((char *)sqlite3_column_text(stmt,1));
                newImageName = QString::fromUtf8((char *)sqlite3_column_text(stmt,2));
                tBase1 = (float)sqlite3_column_double(stmt,3);
                tFloraison = (float)sqlite3_column_double(stmt,4);
                tBase2 = (float)sqlite3_column_double(stmt,5);
                tRecolte = (float)sqlite3_column_double(stmt,6);

                variete->setId(ID);
                variete->setNom(nom.toStdString());
                variete->setNewImageName(newImageName.toStdString());
                variete->setTBase1(tBase1);
                variete->setTFloraison(tFloraison);
                variete->setTBase2(tBase2);
                variete->setTRecolte(tRecolte);
                varieteList.append(variete);
            }
       } while (result == SQLITE_ROW);
    }
    sqlite3_close(db);

    return variete;
}

void VarieteDatabaseInterface::deleteVariete(int id)
{
    qDebug() << "db: " << this->dbPath;
    sqlite3 *db;
    char *zErrMsg = 0;
    int  rc;
    char *sql;

    /* Open database */
    rc = sqlite3_open(this->dbPath, &db);
    if( rc ){
       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
       exit(0);
    }else{
       fprintf(stdout, "Opened database successfully\n");
    }

    sql = sqlite3_mprintf("DELETE FROM varietes WHERE ID = '%q';",
                          QString::number(id).toStdString().c_str()
                          );

    qDebug() << "sql request : " << sql;

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if( rc != SQLITE_OK )
    {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
       fprintf(stdout, "a new variete was added in the database\n");
    }
    sqlite3_close(db);
}

void VarieteDatabaseInterface::saveVariete(VarietesAnanas* variete)
{

    qDebug() << "db: " << this->dbPath;
    sqlite3 *db;
    char *zErrMsg = 0;
    int  rc;
    char *sql;

    /* Open database */
    rc = sqlite3_open(this->dbPath, &db);
    if( rc ){
       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
       exit(0);
    }else{
       fprintf(stdout, "Opened database successfully\n");
    }

    if(variete->getId()) //if an id is set we update the variete in the database
    {
        sql = sqlite3_mprintf("UPDATE varietes SET nom = '%q',newImageName = '%q',tBase1 = '%q',tFloraison = '%q',tBase2 = '%q',tRecolte = '%q' WHERE ID = '%q';",
                              variete->getNom().c_str(),
                              variete->getNewImageName().c_str(),
                              QString::number(variete->getTBase1()).toStdString().c_str(),
                              QString::number(variete->getTFloraison()).toStdString().c_str(),
                              QString::number(variete->getTBase2()).toStdString().c_str(),
                              QString::number(variete->getTRecolte()).toStdString().c_str(),
                              QString::number(variete->getId()).toStdString().c_str()
                              );
    }
    else
    {
        sql = sqlite3_mprintf("INSERT INTO varietes(ID,nom,newImageName,tBase1,tFloraison,tBase2,tRecolte) VALUES(NULL,'%q','%q','%q','%q','%q','%q');",
                              variete->getNom().c_str(),
                              variete->getNewImageName().c_str(),
                              QString::number(variete->getTBase1()).toStdString().c_str(),
                              QString::number(variete->getTFloraison()).toStdString().c_str(),
                              QString::number(variete->getTBase2()).toStdString().c_str(),
                              QString::number(variete->getTRecolte()).toStdString().c_str());
    }

    qDebug() << "sql request : " << sql;

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if( rc != SQLITE_OK )
    {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
       fprintf(stdout, "a new variete was added in the database\n");
    }
    sqlite3_close(db);
}

QList<VarietesAnanas*> VarieteDatabaseInterface::getAllvarietes()
{

    qDebug() << "db: " << this->dbPath;
    sqlite3 *db;
    const char *zErrMsg = 0;
    int  rc;
    char *sql;
    sqlite3_stmt* stmt;
    QList<VarietesAnanas*> varieteList;

    /* Open database */
    rc = sqlite3_open(this->dbPath, &db);
    if( rc ){
       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
       exit(0);
    }else{
       fprintf(stdout, "Opened database successfully\n");
    }

    sql = "SELECT * FROM varietes";

    qDebug() << "sql request : " << sql;

    /* Execute SQL statement */
    rc = sqlite3_prepare_v2(db, sql, strlen(sql)+1, &stmt, &zErrMsg);
    if( rc != SQLITE_OK )
    {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
    }
    else
    {
       int result = sqlite3_step(stmt);
       int ID;
       QString nom;
       QString newImageName;
       float tBase1;
       float tFloraison;
       float tBase2;
       float tRecolte;
       do {
            result = sqlite3_step (stmt);
            if (result == SQLITE_ROW)
            { /* can read data */
                ID = sqlite3_column_int(stmt,0);
                nom = QString::fromUtf8((char *)sqlite3_column_text(stmt,1));
                newImageName = QString::fromUtf8((char *)sqlite3_column_text(stmt,2));
                tBase1 = (float)sqlite3_column_double(stmt,3);
                tFloraison = (float)sqlite3_column_double(stmt,4);
                tBase2 = (float)sqlite3_column_double(stmt,5);
                tRecolte = (float)sqlite3_column_double(stmt,6);

                VarietesAnanas* variete = new VarietesAnanas(this->appStoragePath, this->imageStoragePath);
                variete->setId(ID);
                variete->setNom(nom.toStdString());
                variete->setNewImageName(newImageName.toStdString());
                variete->setTBase1(tBase1);
                variete->setTFloraison(tFloraison);
                variete->setTBase2(tBase2);
                variete->setTRecolte(tRecolte);
                varieteList.append(variete);
            }
       } while (result == SQLITE_ROW);
    }
    sqlite3_close(db);
    return varieteList;
}

void VarieteDatabaseInterface::setdbPath(char* dbPath)
{
    this->dbPath = dbPath;
}

char* VarieteDatabaseInterface::getDbPath()
{
    return this->dbPath;
}
