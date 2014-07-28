#include "varietesananas.h"
#include <QDebug>
#include <string>
#include <iostream>
#include <QCryptographicHash>
#include <QByteArray>
#include <QDateTime>
#include <QFile>


int idVariete;
std::string nom;
std::string imageStoragePt;
std::string imagePath;
std::string newImageName;
float tBase1;
float tFloraison;
float tBase2;
float tRecolte;

VarietesAnanas::VarietesAnanas()
{
    this->idVariete = 0;
}

VarietesAnanas::VarietesAnanas(QString appStoragePath, QString imgStoragePath)
{
    this->imageStoragePt = imgStoragePath;
    this->idVariete = 0;
}

QString VarietesAnanas::generateId()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString();

    QString data = this->nom + QString::number(this->tFloraison) + QString::number(this->tRecolte) + dateTimeString;
    QByteArray ba = QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Sha1);
    QString out = ba.toHex();

    return out;
}

void VarietesAnanas::saveImage()
{
    this->saveImage(this->generateId());
}

void VarietesAnanas::saveImage(QString generatedId)
{
    QString qsImageStoragePt = this->imageStoragePt;
    QString qsImagePath = this->imagePath;

    //first move image if an image was set
    if(qsImagePath.length() > 1)
    {
        this->newImageName = generatedId;

        QString newImagePath = qsImageStoragePt + "/" + this->newImageName + ".img";
        qDebug() << " imagePath : " << qsImageStoragePt;
        qDebug() << " newImagePath : " << newImagePath;
        if(QFile::exists(newImagePath))
        {
            QFile::remove(newImagePath);
        }
        QFile::copy(imagePath, newImagePath);
    }
}

/*
 *
 * Getters and setters
 *
 */

void VarietesAnanas::setNom(QString nom)
{
    this->nom = nom;
}

void VarietesAnanas::setImagePath(QString path)
{
    this->imagePath = path;
}

void VarietesAnanas::setTBase1(float temp)
{
    this->tBase1 = temp;
}

void VarietesAnanas::setTFloraison(float temp)
{
    this->tFloraison = temp;
}

void VarietesAnanas::setTBase2(float temp)
{
    this->tBase2 = temp;
}

void VarietesAnanas::setTRecolte(float temp)
{
    this->tRecolte = temp;
}

void VarietesAnanas::setNewImageName(QString imageName)
{
    this->newImageName = imageName;
}

void VarietesAnanas::setId(int id)
{
    this->idVariete = id;
}

QString VarietesAnanas::getNom()
{
    return this->nom;
}

QString VarietesAnanas::getImagePath()
{
    return this->imagePath;
}

float VarietesAnanas::getTBase1()
{
    return this->tBase1;
}

float VarietesAnanas::getTFloraison()
{
    return this->tFloraison;
}

float VarietesAnanas::getTBase2()
{
    return this->tBase2;
}

float VarietesAnanas::getTRecolte()
{
    return this->tRecolte;
}

QString VarietesAnanas::getNewImageName()
{
    return this->newImageName;
}

int VarietesAnanas::getId()
{
    return this->idVariete;
}

