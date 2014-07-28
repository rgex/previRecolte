#ifndef METEO_H
#define METEO_H

#include <iostream>
#include <list>

class Meteo
{
private:
    int year;
    std::list< std::list<std::string> > meteo; //1st = date MM/DD/YYYY, max temp, avg temp, min temp
public:
    Meteo();
    void addEntry(std::string date, float maxTemp, float avgTemp, float minTemp);
    void removeEntry(std::string date);
    void sort();

    void setYear(float year);
    int getYear();
};

#endif // METEO_H
