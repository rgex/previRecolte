#ifndef SITE_H
#define SITE_H

#include <string>
#include "boost/serialization/serialization.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/serialization/list.hpp"

class Site
{
private:
    std::string nom;
    std::string key;
    std::list<int> years;

public:
    Site();
    std::string generateId();

    std::string getNom();
    std::string getKey();
    std::list<int> getYears();

    void setNom(std::string nom);
    void setKey(std::string key);
    void addYear(int year);
    void deleteYear(int year);
    void setYears(std::list<int> years);

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version){
                ar &
                BOOST_SERIALIZATION_NVP(nom) &
                BOOST_SERIALIZATION_NVP(key) &
                BOOST_SERIALIZATION_NVP(years);
    }
};

#endif // SITE_H
