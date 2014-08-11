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
    //QString appStoragePath = QDir::homePath() + "/sql-ananas-cirad";
    QString appStoragePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/sql-ananas-cirad";
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
                  "newImageName   TEXT," \
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
                  "years   TEXT     NULL" \
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

    QApplication a(argc, argv);

    MainWindow w;
    w.setAppStoragePath(appStoragePath);
    w.setImageStoragePath(imageStoragePath);
    w.setDbPath(databasePath);
    w.initWindow();
    w.show();

    return a.exec();
}
