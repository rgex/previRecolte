#ifndef VARIETESANANAS_H
#define VARIETESANANAS_H

#include <QString>

class VarietesAnanas
{

private:
    int idVariete;
    std::string nom;
    std::string imageStoragePt;
    std::string imagePath;
    std::string newImageName;
    std::string key;
    float tBase1;
    float tFloraison;
    float tBase2;
    float tRecolte;

public:
    VarietesAnanas();
    VarietesAnanas(QString appStoragePath, QString imgStoragePath);
    void saveImage();
    void saveImage(std::string varieteId);
    std::string  generateId();

    void setNom(std::string nom);
    void setImagePath(std::string path);
    void setNewImageName(std::string imageName);
    void setTBase1(float temp);
    void setTFloraison(float temp);
    void setTBase2(float temp);
    void setTRecolte(float temp);
    void setId(int id);

    std::string getNom();
    std::string getImagePath();
    std::string getNewImageName();
    int getId();
    float getTBase1();
    float getTFloraison();
    float getTBase2();
    float getTRecolte();


};

#endif // VARIETESANANAS_H
