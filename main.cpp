#include "mainwindow.h"
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <iostream>
#include <sstream>
#include <string>
#include "leveldb/db.h"

using namespace std;

int main(int argc, char *argv[])
{

    //on launch create application folder to store images and databases

    QString appStoragePath = QDir::homePath() + "/ananas-cirad";
    QDir().mkdir(appStoragePath); //create app folder
    qDebug() << "created folder : " + appStoragePath;
    QString imageStoragePath = appStoragePath + "/images";
    QDir().mkdir(imageStoragePath); //create image folder
    qDebug() << "created image folder : " + imageStoragePath;

    QString meteoStoragePath = appStoragePath + "/meteo";
    QDir().mkdir(meteoStoragePath); //create image folder
    qDebug() << "created meteo folder : " + imageStoragePath;


    QApplication a(argc, argv);

    MainWindow w;
    w.setAppStoragePath(appStoragePath);
    w.setImageStoragePath(imageStoragePath);
    w.initWindow();
    w.show();

    return a.exec();
}
