#include "varietedatabaseinterface.h"
#include "leveldb/db.h"
#include "varietesananas.h"
#include <sstream>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <QDebug>


VarieteDatabaseInterface::VarieteDatabaseInterface()
{
}

void VarieteDatabaseInterface::setStoragePaths(QString appStoragePath, QString imageStoragePath)
{
    this->appStoragePath = appStoragePath;
    this->imageStoragePath = imageStoragePath;

    QString dbPath = appStoragePath + "/varietesAnanas.db";
    this->writeOptions = leveldb::WriteOptions();
    this->readOptions = leveldb::ReadOptions();
    this->options = leveldb::Options();

    this->options.create_if_missing = true;
    this->writeOptions.sync = true;

    this->status = leveldb::DB::Open(this->options, dbPath.toStdString(), &this->db);

    if (false == this->status.ok())
    {
         qDebug() << "Unable to open/create varietesAnanas database : "  << appStoragePath;
         qDebug() << "Error msg : " << QString::fromStdString(status.ToString());
    }
    else
    {
        qDebug() << "DB status msg: " << QString::fromStdString(status.ToString());
    }
}

VarietesAnanas* VarieteDatabaseInterface::getVarieteWithKey(QString key)
{
    std::string value;

    this->db->Get(this->readOptions, key.toStdString(), &value);

    qDebug() << "LevelDb value : " << QString::fromStdString(value);
    qDebug() << "for key : " << key;

    VarietesAnanas* variete = new VarietesAnanas(appStoragePath, imageStoragePath);

    if(value.length() > 1)
    {
        std::stringstream ifStream;
        ifStream << value;

        boost::archive::xml_iarchive iarchive(ifStream);
        iarchive >> BOOST_SERIALIZATION_NVP(variete);
    }
    else
    {
        qDebug() << "something went wrong while recovering data from DB : ";
    }

    return variete;
}

void VarieteDatabaseInterface::deleteVariete(QString key)
{
    this->db->Delete(this->writeOptions, key.toStdString());
}

void VarieteDatabaseInterface::saveVariete(VarietesAnanas* variete)
{
    std::ostringstream sStream;

    boost::archive::xml_oarchive oarchive(sStream);
    oarchive << BOOST_SERIALIZATION_NVP(variete);
    qDebug() << "sStream : " << QString::fromStdString(sStream.str());

    this->db->Put(this->writeOptions, variete->getKey(), sStream.str());
}

QList<VarietesAnanas*> VarieteDatabaseInterface::getAllvarietes()
{
    QList<VarietesAnanas*> varieteList;

    leveldb::Iterator* it = this->db->NewIterator(this->readOptions);
    for (it->SeekToFirst(); it->Valid(); it->Next())
    {
        leveldb::Slice value = it->value();

        VarietesAnanas* variete = new VarietesAnanas(this->appStoragePath, this->imageStoragePath);

        std::stringstream ifStream;
        ifStream << value.ToString();

        boost::archive::xml_iarchive iarchive(ifStream);

        iarchive >> BOOST_SERIALIZATION_NVP(variete);

        varieteList.append(variete);
    }
    delete it;
    return varieteList;
}
