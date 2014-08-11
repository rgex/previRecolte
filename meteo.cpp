#include "meteo.h"
#include <QtAlgorithms>
#include <QList>
#include <QStringList>
#include <QString>
#include <QMap>
#include <QDebug>
#include <QDateTime>

Meteo::Meteo()
{
    this->idMeteo = 0;
}

void Meteo::addEntry(QString date, float maxTemp, float avgTemp, float minTemp, bool replace)
{
    if(0 != date.mid(0, 4).compare(QString::number(this->year)) && (date.toFloat() > 1))
    {   // 1 is a joker that allows to add new meteos entries even if the date is not matching
        return;
    }
    QStringList newEntry;
    newEntry.append(date);
    newEntry.append(QString::number(maxTemp));
    newEntry.append(QString::number(avgTemp));
    newEntry.append(QString::number(minTemp));
    foreach(QString key, this->meteo.keys())
    {
        QStringList meteoStringList = this->meteo.value(key);
        if(true == meteoStringList.at(0).compare(date) && false == replace)
        {
            return;
        }
        else if(true == meteoStringList.at(0).compare(date) && true == replace)
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
    this->addMeteoWithQMaps(dayMaxTempMap, dayAvgTempMap, dayMinTempMap, true);
}

void Meteo::addMeteoWithQMaps(QMap<QString, float> dayMaxTempMap, QMap<QString, float> dayAvgTempMap, QMap<QString, float> dayMinTempMap, bool replace)
{
    foreach(QString qMapKey, dayMaxTempMap.keys())
    {
        this->addEntry(qMapKey,
                       dayMaxTempMap.value(qMapKey),
                       dayAvgTempMap.value(qMapKey),
                       dayMinTempMap.value(qMapKey),
                       replace
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

QString Meteo::exportMeteoAsCsv2(bool includeHeaders)
{
    QString csv;
    if(true == includeHeaders)
    {
        csv.append(QString("Date,") +
                   QString("Températures moyenne,") +
                   QString("Températures minimale,") +
                   QString("Températures maximale") +
                   QString("\n")
                   );
    }
    int i = 0;
    foreach(QString key, this->meteo.keys())
    {
        qDebug() << "date : " << this->meteo.value(key).at(0);
        QDateTime date = QDateTime::fromString(this->meteo.value(key).at(0),"yyyyMMdd");
        QString qsDate = date.toString("yyyy-MM-dd hh:mm:ss");
        csv.append(
                   qsDate + "," +
                   QString::number(this->meteo.value(key).at(2).toFloat(), 'f', 3) + "," +
                   QString::number(this->meteo.value(key).at(1).toFloat(), 'f', 3) + "," +
                   QString::number(this->meteo.value(key).at(3).toFloat(), 'f', 3) + "\n");
        i++;
    }
    return csv;
}

void Meteo::importMeteoFromCsv(QString meteoCsv)
{

     QStringList meteoList = meteoCsv.split(";");

     foreach(QString meteo, meteoList)
     {
         QStringList meteoElements = meteo.split(",");
         if(meteoElements.size()> 3)
         {
             if(meteoElements.at(1).toFloat() > 0 && meteoElements.at(2).toFloat() > 0 && meteoElements.at(3).toFloat() > 0)
             {
                 this->addEntry(meteoElements.at(0),
                                meteoElements.at(1).toFloat(),
                                meteoElements.at(2).toFloat(),
                                meteoElements.at(3).toFloat(),
                                true);
            }
         }
     }

}


void Meteo::setYear(int year)
{
    this->year = year;
}

int Meteo::getYear()
{
    return this->year;
}

void Meteo::setId(int id)
{
    this->idMeteo = id;
}

int Meteo::getId()
{
    return this->idMeteo;
}

void Meteo::setSiteId(int id)
{
    this->siteId = id;
}

int Meteo::getSiteId()
{
    return this->siteId;
}

void Meteo::setMeteo(QMap<QString, QStringList> meteo)
{
    this->meteo = meteo;
}

QMap<QString, QStringList> Meteo::getMeteo()
{
    return this->meteo;
}
