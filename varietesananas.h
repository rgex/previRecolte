#ifndef VARIETESANANAS_H
#define VARIETESANANAS_H

#include <QString>

class VarietesAnanas
{

private:
    int idVariete;
    QString nom;
    QString imageStoragePt;
    QString imagePath;
    QString newImageName;
    QString key;
    float tBase1;
    float tFloraison;
    float tBase2;
    float tRecolte;

public:
    VarietesAnanas();
    VarietesAnanas(QString appStoragePath, QString imgStoragePath);
    void saveImage();
    void saveImage(QString generatedId);
    QString  generateId();

    void setNom(QString nom);
    void setImagePath(QString path);
    void setNewImageName(QString imageName);
    void setTBase1(float temp);
    void setTFloraison(float temp);
    void setTBase2(float temp);
    void setTRecolte(float temp);
    void setId(int id);

    QString getNom();
    QString getImagePath();
    QString getNewImageName();
    int getId();
    float getTBase1();
    float getTFloraison();
    float getTBase2();
    float getTRecolte();


};

#endif // VARIETESANANAS_H
