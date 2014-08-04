#ifndef SITE_H
#define SITE_H

#include <string>
#include <QStringList>
#include <QString>

class Site
{
private:
    int idSite;
    QString nom;
    QStringList years;

public:
    Site();

    QString getNom();
    QStringList getYears();
    int getId();

    void setNom(QString nom);
    void addYear(int year);
    void deleteYear(int year);
    void setYears(QStringList years);
    void setId(int id);
    QString getYearsCsv();

};

#endif // SITE_H
