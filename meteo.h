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
    void addEntry(QString date, float maxTemp, float avgTemp, float minTemp, float pluvioMetry, bool replace); //meteo has to be under this format YYYYMMDD
    void addMeteoWithQMaps(QMap<QString, float> maxTempMap, QMap<QString, float> avgTempMap, QMap<QString, float> minTempMap, QMap<QString, float> dayPluvioMap);
    void addMeteoWithQMaps(QMap<QString, float> dayMaxTempMap, QMap<QString, float> dayAvgTempMap, QMap<QString, float> dayMinTempMap, QMap<QString, float> dayPluvioMap, bool replace);
    void removeEntry(QString date);
    void sort();
    QString exportMeteoAsCsv();
    QString exportMeteoAsCsv2(bool includeHeaders);
    void importMeteoFromCsv(QString meteoCsv);

    //getters and setters
    void setYear(int year);
    int getYear();

    void setId(int id);
    int getId();

    void setSiteId(int id);
    int getSiteId();

    void setMeteo(QMap<QString, QStringList> meteo);
    QMap<QString, QStringList> getMeteo();
};

#endif // METEO_H
