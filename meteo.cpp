#include "meteo.h"
#include <QtAlgorithms>
#include <QList>
#include <QStringList>
#include <QString>
#include <QMap>

Meteo::Meteo()
{

}

void Meteo::addEntry(QString date, float maxTemp, float avgTemp, float minTemp, bool replace)
{

    if(false == date.mid(0, 4).compare(QString::number(this->year)))
    {
        return;
    }
    QStringList newEntry;
    newEntry.append(date);
    newEntry.append(QString::number(maxTemp));
    newEntry.append(QString::number(avgTemp));
    newEntry.append(QString::number(minTemp));
    foreach(QString key, this->meteo.keys())
    {
        QStringList meteoStringLIst = this->meteo.value(key);
        if(true == meteoStringLIst.at(0).compare(date) && false == replace)
        {
            return;
        }
        else if(true == meteoStringLIst.at(0).compare(date) && true == replace)
        {
            this->meteo.remove(key);
            this->meteo.insert(key, newEntry);
        }
    }
    this->meteo.insert(date, newEntry);
    this->sort();
}

void Meteo::addMeteoWithQMaps(QMap<QString, float> dayMaxTempMap, QMap<QString, float> dayAvgTempMap, QMap<QString, float> dayMinTempMap)
{
    foreach(QString qMapKey, dayMaxTempMap.keys())
    {
        this->addEntry(qMapKey,
                       dayMaxTempMap.value(qMapKey),
                       dayAvgTempMap.value(qMapKey),
                       dayMinTempMap.value(qMapKey),
                       true
                       );
    }
}

void Meteo::removeEntry(QString date)
{
    this->meteo.remove(date);
}

void Meteo::sort()
{
    //nothing todo
}

QString Meteo::exportMeteoAsCsv()
{
    QString csv;

    foreach(QString key, this->meteo.keys())
    {
        csv.append(this->meteo.value(key).at(0) + "," +
                   this->meteo.value(key).at(1) + "," +
                   this->meteo.value(key).at(2) + "," +
                   this->meteo.value(key).at(3) + ";");
    }
    return csv;
}

void loadMeteoFromCSV()
{

}


void Meteo::setYear(int year)
{
    this->year = year;
}

int Meteo::getYear()
{
    return this->year;
}


