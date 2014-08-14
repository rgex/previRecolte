#ifndef HTMLCHARTMAKER_H
#define HTMLCHARTMAKER_H

#include <QStringList>
#include <QString>
#include <QList>
#include <string>
#include <QMap>
#include <QDate>
#include "meteo.h"
#include "mainwindow.h"
#include "site.h"

class HtmlChartMaker
{
private:
    QString getCharJSLib();
    std::string hex_to_string(const std::string& input);
    int height;
    int width;

public:
    HtmlChartMaker();
    QString generateHtmlChartWithMap(QMap<QString, QStringList> tempMap);
    QString generateHtmlChartWithMap(QMap<QString, QStringList> tempMap, bool completeMissingMonth);
    QString generateHtmlChartWithMap(QMap<QString, QStringList> tempMap, int year, bool completeMissingMonth);
    QString generateHtmlChartWithMaps(QMap<QString, float> dayMaxTempMap, QMap<QString, float> dayTempAvgMap, QMap<QString, float> dayMinTempMap);
    QString generateHtmlChartWithMaps(QMap<QString, float> dayMaxTempMap, QMap<QString, float> dayTempAvgMap, QMap<QString, float> dayMinTempMap, int year, bool completeMissingMonth);
    QString generateHtmlChartWithTempData(QList<QStringList> temperatures);

    QMap<QString, float> calculateDayTempAverage(QList<QStringList> temperatures);
    QMap<QString, float> calculateWeekTempAverage(QMap<QString, float> dayTempAvgMap);
    QMap<QString, float> calculateMonthTempAverage(QMap<QString, float> dayTempAvgMap);

    QMap<QString, float> calculateMaxDayTemp(QList<QStringList> temperatures);
    //QMap<QString, float> calculateWeekTempAverage(QMap<QString, float> dayMaxTempMap);
    QMap<QString, float> calculateMaxMonthTemp(QMap<QString, float> dayMaxTempMap);

    QMap<QString, float> calculateMinDayTemp(QList<QStringList> temperatures);
    //QMap<QString, float> calculateWeekTempAverage(QMap<QString, float> dayMinTempMap);
    QMap<QString, float> calculateMinMonthTemp(QMap<QString, float> dayMinTempMap);

    QStringList getYearsWithTempData(QList<QStringList> temperatures);

    QMap<QString, QStringList> calculateAvgOfTempYears(QList<QMap<QString, QStringList> > temps);

    QDate predictDate(QDate selectedDate, int modelYear, MainWindow* mainWindow, Site* site, float base1, float floraison, float base2, float recolte, bool isForRecolte);

    //getters and setters

    void setHeight(int height);
    int getHeight();

    void setWidth(int width);
    int getWidth();
};

#endif // HTMLCHARTMAKER_H
