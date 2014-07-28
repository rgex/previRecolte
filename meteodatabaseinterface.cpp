#include "meteodatabaseinterface.h"
#include <QString>
#include <ostream>
#include "leveldb/db.h"
#include "meteo.h"
#include <sstream>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <QDebug>

MeteoDatabaseInterface::MeteoDatabaseInterface()
{

}

void MeteoDatabaseInterface::deleteMeteo(QString siteKey, int year)
{

}

void MeteoDatabaseInterface::saveMeteo(QString siteKey, int year, Meteo* meteo)
{
    QString dbPath = meteoStoragePath + "/" + siteKey + "-" + QString::number(year) + ".db";

    leveldb::DB* db;
    leveldb::WriteOptions writeOptions = leveldb::WriteOptions();
    leveldb::Options options = leveldb::Options();
    options.create_if_missing = true;

    leveldb::Status status = leveldb::DB::Open(options, dbPath.toStdString(), &db);

    if (false == status.ok())
    {
         qDebug() << "Unable to open/create sites database : "  << meteoStoragePath;
         qDebug() << "Error msg : " << QString::fromStdString(status.ToString());
    }
    else
    {
        qDebug() << "DB status msg: " << QString::fromStdString(status.ToString());
    }

    std::ostringstream sStream;

    boost::archive::xml_oarchive oarchive(sStream);
    oarchive << BOOST_SERIALIZATION_NVP(meteo);
    qDebug() << "sStream : " << QString::fromStdString(sStream.str());

    db->Put(writeOptions, QString::number(year).toStdString(), sStream.str());
}

QList<Meteo*> MeteoDatabaseInterface::getMeteo(QString siteKey, int year)
{
    QString dbPath = meteoStoragePath + "/" + siteKey + "-" + QString::number(year) + ".db";

    leveldb::DB* db;
    leveldb::ReadOptions readOptions = leveldb::ReadOptions();
    leveldb::Options options = leveldb::Options();
    options.create_if_missing = true;

    leveldb::Status status = leveldb::DB::Open(options, dbPath.toStdString(), &db);

    if (false == status.ok())
    {
         qDebug() << "Unable to open/create sites database : "  << meteoStoragePath;
         qDebug() << "Error msg : " << QString::fromStdString(status.ToString());
    }
    else
    {
        qDebug() << "DB status msg: " << QString::fromStdString(status.ToString());
    }
}

void MeteoDatabaseInterface::setMeteoStoragePath(QString meteoStoragePath)
{
    this->meteoStoragePath = meteoStoragePath;
}
