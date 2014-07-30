#include "meteodatabaseinterface.h"
#include <QString>
#include "meteo.h"
#include <QDebug>
#include <QtSql/QtSql>

MeteoDatabaseInterface::MeteoDatabaseInterface()
{

}

void MeteoDatabaseInterface::deleteMeteo(int siteId, int year)
{

}

void MeteoDatabaseInterface::saveMeteo(Meteo* meteo)
{

    qDebug() << "db: " << this->dbPath;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(this->dbPath);

    if(false == db.open())
    {
        qDebug() << "can not open database";
    }

    QString meteoCsv = meteo->exportMeteoAsCsv();

    if(0 != meteo->getId()) //if an id is set we update the meteo entry in the database
    {
        QSqlQuery query;
        query.prepare("UPDATE meteo SET year = :year, id_site = :siteId, meteo_data = :meteoCsv WHERE ID = :id;");
        query.bindValue(":id", meteo->getId());
        query.bindValue(":year", meteo->getYear());
        query.bindValue(":siteId", meteo->getSiteId());
        query.bindValue(":meteoCsv", meteoCsv);
        query.exec();
        if(query.lastError().isValid())
        {
            qDebug() << "SQL ERROR1 : " << query.lastError();
        }
    }
    else
    {
        QSqlQuery query;
        query.prepare("INSERT INTO meteo(ID, year, id_site, meteo_data) VALUES (NULL, :year, :siteId, :meteoCsv);");
        query.bindValue(":year", meteo->getYear());
        query.bindValue(":siteId", meteo->getSiteId());
        query.bindValue(":meteoCsv", meteoCsv);
        query.exec();
        if(query.lastError().isValid())
        {
            qDebug() << "SQL QUERY : " << query.lastQuery();
            qDebug() << "SQL ERROR2 : " << query.lastError();
        }
    }
    db.commit();
    db.close();

}

QList<Meteo*> MeteoDatabaseInterface::getMeteo(int siteId, int year)
{

}

void MeteoDatabaseInterface::setStoragePaths(QString appStoragePath, QString dbPath)
{
    this->appStoragePath = appStoragePath;
    this->dbPath = dbPath;
}
