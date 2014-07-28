#ifndef HTMLCHARTMAKER_H
#define HTMLCHARTMAKER_H

#include <QStringList>
#include <QString>
#include <QList>
#include <string>
#include <QMap>

class HtmlChartMaker
{
private:
    QString getCharJSLib();
    std::string hex_to_string(const std::string& input);

public:
    HtmlChartMaker();
    QString generateHtmlChartWithTempData(QList<QStringList> temperatures);
    QMap<QString, float> calculateDayTempAverage(QList<QStringList> temperatures);
    QMap<QString, float> calculateMonthTempAverage(QMap<QString, float> dayTempAvgMap);

    QMap<QString, float> calculateMaxDayTemp(QList<QStringList> temperatures);
    QMap<QString, float> calculateMaxMonthTemp(QMap<QString, float> dayTempAvgMap);

    QMap<QString, float> calculateMinDayTemp(QList<QStringList> temperatures);
    QMap<QString, float> calculateMinMonthTemp(QMap<QString, float> dayMinTempMap);

    QStringList getYearsWithTempData(QList<QStringList> temperatures);
};

#endif // HTMLCHARTMAKER_H
