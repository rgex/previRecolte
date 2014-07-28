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
    query.exec();
    db.commit();

    query.prepare("CREATE TABLE IF NOT EXISTS sites("  \
                  "ID INTEGER PRIMARY KEY," \
                  "nom            TEXT    NOT NULL," \
                  "years   CHAR(500)     NOT NULL" \
                  ");");
    query.exec();


    db.commit();
    db.close();

    /*
    sqlite3 *db;
    char *zErrMsg = 0;
    int  rc;
    char *sql;


    rc = sqlite3_open(databasePath.toStdString().c_str(), &db);
    if( rc ){
       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
       exit(0);
    }else{
       fprintf(stdout, "Opened database successfully\n");
    }

    sql = "CREATE TABLE IF NOT EXISTS varietes("  \
          "ID INTEGER PRIMARY KEY," \
          "nom            TEXT    NOT NULL," \
          "newImageName   CHAR(80)," \
          "tBase1         FLOAT," \
          "tFloraison     FLOAT," \
          "tBase2         FLOAT," \
          "tRecolte       FLOAT" \
          ");";


    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if( rc != SQLITE_OK )
    {
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    }
    else
    {
       fprintf(stdout, "Table created successfully\n");
    }

    sql = "CREATE TABLE IF NOT EXISTS sites("  \
          "ID INTEGER PRIMARY KEY," \
          "nom            TEXT    NOT NULL," \
          "years   CHAR(500)     NOT NULL" \
          ");";

    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if( rc != SQLITE_OK )
    {
       fprintf(stderr, "SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
    }
    else
    {
       fprintf(stdout, "Table created successfully\n");
    }


    sqlite3_close(db);
    */

    QApplication a(argc, argv);

    MainWindow w;
    w.setAppStoragePath(appStoragePath);
    w.setImageStoragePath(imageStoragePath);
    w.setDbPath(databasePath);
    w.initWindow();
    w.show();

    return a.exec();
}
