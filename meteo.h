#ifndef METEO_H
#define METEO_H

#include <iostream>
#include <list>
#include <QList>
#include <QStringList>
#include <QString>
#include <QMap>

class Meteo
{
private:
    int idMeteo;
    int siteId;
    int year;
    QMap<QString, QStringList> meteo;

public:
    Meteo();
    void addEntry(QString date, float maxTemp, float avgTemp, float minTemp, bool replace); //meteo has to be under this format YYYYMMDD
    void addMeteoWithQMaps(QMap<QString, float> maxTempMap, QMap<QString, float> avgTempMap, QMap<QString, float> minTempMap);
    void removeEntry(QString date);
    void sort();
    QString exportMeteoAsCsv();
    void loadMeteoFromCSV();

    //getters and setters
    void setYear(int year);
    int getYear();

    void setId(int id);
    int getId();

    void setSiteId(int id);
    int getSiteId();
};

#endif // METEO_H
