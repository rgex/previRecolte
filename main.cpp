#include "mainwindow.h"
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <iostream>
#include <string>
#include <QtSql/QtSql>

using namespace std;

int main(int argc, char *argv[])
{
    //on launch create application folder to store images and databases
    QString appStoragePath = QDir::homePath() + "/sql-ananas-cirad";
    QString databasePath = appStoragePath + "/ananas.db";

    QDir().mkdir(appStoragePath); //create app folder
    qDebug() << "created folder : " + appStoragePath;
    QString imageStoragePath = appStoragePath + "/images";
    QDir().mkdir(imageStoragePath); //create image folder
    qDebug() << "created image folder : " + imageStoragePath;

    QString meteoStoragePath = appStoragePath + "/backup";
    QDir().mkdir(meteoStoragePath); //create backup folder
    qDebug() << "created meteo folder : " + imageStoragePath;



    qDebug() << "db: " << databasePath;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(databasePath);

    if(false == db.open())
    {
        qDebug() << "can not open database";
    }

    QSqlQuery query;
    query.prepare("CREATE TABLE IF NOT EXISTS varietes("  \
                  "ID INTEGER PRIMARY KEY," \
                  "nom            TEXT    NOT NULL," \
                  "newImageName   CHAR(80)," \
                  "tBase1         FLOAT," \
                  "tFloraison     FLOAT," \
                  "tBase2         FLOAT," \
                  "tRecolte       FLOAT" \
                  ");");
    if(false == query.exec())
    {
        qDebug() << "SQL ERROR : " << query.lastError();
    }
    db.commit();

    query.prepare("CREATE TABLE IF NOT EXISTS sites("  \
                  "ID INTEGER PRIMARY KEY," \
                  "nom            TEXT    NOT NULL," \
                  "years   CHAR(500)     NOT NULL" \
                  ");");
    if(false == query.exec())
    {
        qDebug() << "SQL ERROR : " << query.lastError();
    }
    db.commit();

    query.prepare("CREATE TABLE IF NOT EXISTS meteo("  \
                  "ID INTEGER PRIMARY KEY," \
                  "year INTEGER    NOT NULL," \
                  "id_site INTEGER   NOT NULL," \
                  "meteo_data TEXT NULL" \
                  ");");
    if(false == query.exec())
    {
        qDebug() << "SQL ERROR : " << query.lastError();
    }
    db.commit();
    db.close();




    QSqlDatabase db2 = QSqlDatabase::addDatabase("QSQLITE");
    db2.setDatabaseName("/home/jan/sql-ananas-cirad/ananas.db");

    if(false == db2.open())
    {
        qDebug() << "can not open database";
    }

        QSqlQuery query2;
        query2.prepare("INSERT INTO meteo(ID, year, id_site, meteo_data) VALUES (NULL, :year, :siteId, :meteoCsv);");
        query2.bindValue(":year", 1222);
        query2.bindValue(":siteId", 12);
        query2.bindValue(":meteoCsv", "text");
        /*
        query.bindValue(":year", meteo->getYear());
        query.bindValue(":siteId", meteo->getSiteId());
        query.bindValue(":meteoCsv", meteoCsv);*/
        query2.exec();
        if(query2.lastError().isValid())
        {
            qDebug() << "SQL QUERY : " << query2.lastQuery();
            qDebug() << "SQL ERROR2 : " << query2.lastError();
        }

    db2.commit();
    db2.close();





    QApplication a(argc, argv);

    MainWindow w;
    w.setAppStoragePath(appStoragePath);
    w.setImageStoragePath(imageStoragePath);
    w.setDbPath(databasePath);
    w.initWindow();
    w.show();

    return a.exec();
}
