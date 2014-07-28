#include "site.h"
#include <QCryptographicHash>
#include <QDateTime>
#include "boost/foreach.hpp"

Site::Site()
{

}


std::string Site::generateId()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString();

    QString data = QString::fromStdString(nom) + dateTimeString;
    QByteArray ba = QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Sha1);
    QString out = ba.toHex();

    return out.toStdString();
}

/*
 *
 * Getters and setters
 *
 */

std::string Site::getNom()
{
    return nom;
}

std::string Site::getKey()
{
    return key;
}

std::list<int> Site::getYears()
{
    return years;
}

void Site::setNom(std::string nom)
{
    this->nom = nom;
}

void Site::setKey(std::string key)
{
    this->key = key;
}

void Site::setYears(std::list<int> years)
{
    this->years = years;
}

void Site::addYear(int year)
{
    bool yearAllReadyExists = false;

    BOOST_FOREACH( int y, this->years)
    {
        if(y == year)
            yearAllReadyExists = true;
    }

    if(false == yearAllReadyExists)
        this->years.push_back(year);
}

void Site::deleteYear(int year)
{

}
