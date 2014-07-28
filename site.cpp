#include "site.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QString>

Site::Site()
{
    this->idSite = 0;
}

/*
 *
 * Getters and setters
 *
 */

QString Site::getNom()
{
    return nom;
}

QStringList Site::getYears()
{
    return years;
}

void Site::setNom(QString nom)
{
    this->nom = nom;
}

void Site::setYears(QStringList years)
{
    this->years = years;
}

void Site::addYear(int year)
{
    if(false == this->years.contains(QString::number(year)))
    {
        this->years.append(QString::number(year));
        qSort(this->years.begin(),this->years.end(),qGreater<QString>());
    }
}

void Site::deleteYear(int year)
{
    this->years.removeOne(QString::number(year));
}

int Site::getId()
{
    return this->idSite;
}

void Site::setId(int id)
{
    this->idSite = id;
}
