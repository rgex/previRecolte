#include "mainwindow.h"
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

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

    sqlite3 *db;
    char *zErrMsg = 0;
    int  rc;
    char *sql;

    /* Open database */
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
          "newImageName   CHAR(80)     NOT NULL," \
          "tBase1         FLOAT," \
          "tFloraison     FLOAT," \
          "tBase2         FLOAT," \
          "tRecolte       FLOAT" \
          ");";

    /* Execute SQL statement */
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

    QApplication a(argc, argv);

    MainWindow w;
    w.setAppStoragePath(appStoragePath);
    w.setImageStoragePath(imageStoragePath);
    w.setDbPath(databasePath);
    w.initWindow();
    w.show();

    return a.exec();
}
