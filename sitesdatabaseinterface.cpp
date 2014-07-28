#include "sitesdatabaseinterface.h"
#include <QDebug>
#include <sqlite3.h>
#include <QtSql/QtSql>

SitesDatabaseInterface::SitesDatabaseInterface()
{

}

void SitesDatabaseInterface::setStoragePaths(QString appStoragePath, QString dbPath)
{
    this->appStoragePath = appStoragePath;
    char* nDbPath = new char [dbPath.size()+1];
    strcpy(nDbPath, dbPath.toStdString().c_str());
    this->dbPath = nDbPath;
}

void SitesDatabaseInterface::saveSite(Site* site)
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

    //convert QStringList to CSV

    QString yearsCsv = site->getYears().join(";");

    if(0 != site->getId()) //if an id is set we update the variete in the database
    {
        sql = sqlite3_mprintf("UPDATE sites SET nom = '%q', years = '%q' WHERE ID = '%q';",
                              site->getNom().c_str(),
                              yearsCsv.toStdString().c_str(),
                              site->getId()
                              );
    }
    else
    {
        sql = sqlite3_mprintf("INSERT INTO sites(ID,nom,years) VALUES(NULL,'%q','%q');",
                              site->getNom().c_str(),
                              yearsCsv.toStdString().c_str()
                              );
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

QList<Site*> SitesDatabaseInterface::getAllSites()
{

    QList<Site*> siteList;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QString::fromUtf8(this->dbPath));

    if(false == db.open())
    {
        qDebug() << "can not open database";
    }

    QSqlQuery query= db.exec("SELECT * FROM sites");
    while (query.next())
    {
        int id = query.value(0).toInt();
        QString nom = query.value(1).toString();
        QString years = query.value(2).toString();
        QStringList yearsList = years.split(";");
        Site* site = new Site();

        site->setId(id);
        site->setNom(nom.toStdString());
        site->setYears(yearsList);
        siteList.append(site);
    }
    db.close();

    return siteList;
}
