#include "sitesdatabaseinterface.h"
#include <QDebug>
#include <QtSql/QtSql>

SitesDatabaseInterface::SitesDatabaseInterface()
{

}

void SitesDatabaseInterface::setStoragePaths(QString appStoragePath, QString dbPath)
{
    this->appStoragePath = appStoragePath;
    this->dbPath = dbPath;
}

void SitesDatabaseInterface::saveSite(Site* site)
{

    qDebug() << "db: " << this->dbPath;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(this->dbPath);

    if(false == db.open())
    {
        qDebug() << "can not open database";
    }

    //convert QStringList to CSV
    QString yearsCsv = site->getYears().join(";");

    if(0 != site->getId()) //if an id is set we update the variete in the database
    {
        QSqlQuery query;
        query.prepare("UPDATE sites SET nom = :nom, years = :years WHERE ID = :id;");
        query.bindValue(":id", site->getId());
        query.bindValue(":nom", site->getNom());
        query.bindValue(":years", yearsCsv);
        if(false == query.exec())
        {
            qDebug() << "SQL ERROR : " << query.lastError();
        }
    }
    else
    {
        QSqlQuery query;
        query.prepare("INSERT INTO sites(ID,nom,years) VALUES(NULL, :nom, :years);");
        query.bindValue(":nom", site->getNom());
        query.bindValue(":years", yearsCsv);
        if(false == query.exec())
        {
            qDebug() << "SQL ERROR : " << query.lastError();
        }
    }
    db.commit();
    db.close();
}

QList<Site*> SitesDatabaseInterface::getAllSites()
{
    qDebug() << "db: " << this->dbPath;
    QList<Site*> siteList;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(this->dbPath);

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
        site->setNom(nom);
        site->setYears(yearsList);
        siteList.append(site);
    }
    db.close();

    return siteList;
}
