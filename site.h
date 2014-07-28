#ifndef SITE_H
#define SITE_H

#include <string>
#include <QStringList>

class Site
{
private:
    int idSite;
    std::string nom;
    QStringList years;

public:
    Site();

    std::string getNom();
    QStringList getYears();
    int getId();

    void setNom(std::string nom);
    void addYear(int year);
    void deleteYear(int year);
    void setYears(QStringList years);
    void setId(int id);

};

#endif // SITE_H
