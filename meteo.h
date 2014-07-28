#ifndef METEO_H
#define METEO_H

#include <iostream>
#include "boost/serialization/serialization.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/serialization/list.hpp"

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

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version){
                ar &
                BOOST_SERIALIZATION_NVP(year) &
                BOOST_SERIALIZATION_NVP(meteo);
    }

    void setYear(float year);
    int getYear();
};

#endif // METEO_H
