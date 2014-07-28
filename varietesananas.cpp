#include "varietesananas.h"
#include <QDebug>
#include <string>
#include <iostream>
#include <sstream>
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

}

VarietesAnanas::VarietesAnanas(QString appStoragePath, QString imgStoragePath)
{
    this->imageStoragePt = imgStoragePath.toStdString();
}

std::string VarietesAnanas::generateId()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString();

    QString data = QString::fromStdString(nom) + QString::number(this->tFloraison) + QString::number(this->tRecolte) + dateTimeString;
    QByteArray ba = QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Sha1);
    QString out = ba.toHex();

    return out.toStdString();
}

void VarietesAnanas::saveImage()
{
    this->saveImage(this->generateId());
}

void VarietesAnanas::saveImage(std::string varieteId)
{
    QString qsImageStoragePt = QString::fromStdString(this->imageStoragePt);
    QString qsImagePath = QString::fromStdString(this->imagePath);

    //first move image if an image was set
    if(qsImagePath.length() > 1)
    {
        this->newImageName = varieteId;

        QString newImagePath = qsImageStoragePt + "/" + QString::fromStdString(this->newImageName) + ".img";
        qDebug() << " imagePath : " << qsImageStoragePt;
        qDebug() << " newImagePath : " << newImagePath;
        if(QFile::exists(newImagePath))
        {
            QFile::remove(newImagePath);
        }
        QFile::copy(QString::fromStdString(imagePath), newImagePath);
    }
}

/*
 *
 * Getters and setters
 *
 */

void VarietesAnanas::setNom(std::string nom)
{
    this->nom = nom;
}

void VarietesAnanas::setImagePath(std::string path)
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

void VarietesAnanas::setNewImageName(std::string imageName)
{
    this->newImageName = imageName;
}

void VarietesAnanas::setKey(std::string key)
{
    this->key = key;
}

std::string VarietesAnanas::getNom()
{
    return this->nom;
}

std::string VarietesAnanas::getImagePath()
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

std::string VarietesAnanas::getNewImageName()
{
    return this->newImageName;
}

std::string VarietesAnanas::getKey()
{
    return this->key;
}

