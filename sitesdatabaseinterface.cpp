#include "sitesdatabaseinterface.h"
#include <sstream>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <QDebug>

SitesDatabaseInterface::SitesDatabaseInterface()
{

}

void SitesDatabaseInterface::setStoragePaths(QString appStoragePath)
{
    this->appStoragePath = appStoragePath;

    QString dbPath = appStoragePath + "/sites.db";
    this->writeOptions = leveldb::WriteOptions();
    this->readOptions = leveldb::ReadOptions();
    this->options = leveldb::Options();

    this->options.create_if_missing = true;
    this->writeOptions.sync = true;

    this->status = leveldb::DB::Open(this->options, dbPath.toStdString(), &this->db);

    if (false == this->status.ok())
    {
         qDebug() << "Unable to open/create sites database : "  << appStoragePath;
         qDebug() << "Error msg : " << QString::fromStdString(status.ToString());
    }
    else
    {
        qDebug() << "DB status msg: " << QString::fromStdString(status.ToString());
    }
}

void SitesDatabaseInterface::saveSite(Site* site)
{
    std::ostringstream sStream;

    boost::archive::xml_oarchive oarchive(sStream);
    oarchive << BOOST_SERIALIZATION_NVP(site);
    qDebug() << "sStream : " << QString::fromStdString(sStream.str());

    this->db->Put(this->writeOptions, site->getKey(), sStream.str());
}

QList<Site*> SitesDatabaseInterface::getAllSites()
{
    QList<Site*> siteList;

    leveldb::Iterator* it = this->db->NewIterator(this->readOptions);
    for (it->SeekToFirst(); it->Valid(); it->Next())
    {
        leveldb::Slice value = it->value();

        Site* site = new Site();

        std::stringstream ifStream;
        ifStream << value.ToString();

        boost::archive::xml_iarchive iarchive(ifStream);

        iarchive >> BOOST_SERIALIZATION_NVP(site);

        siteList.append(site);
    }
    delete it;
    return siteList;
}
