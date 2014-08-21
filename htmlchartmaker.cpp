#include "htmlchartmaker.h"
#include <string>
#include <iostream>
#include <sstream>
#include <QDateTime>
#include <QDebug>
#include <QMap>
#include "meteodatabaseinterface.h"
#include <QMessageBox>
#include <QFile>

HtmlChartMaker::HtmlChartMaker()
{

}

// @TODO remove when not needed anymore
std::string HtmlChartMaker::hex_to_string(const std::string& input)
{
    static const char* const lut = "0123456789abcdef";
    size_t len = input.length();

    std::string output;
    output.reserve(len / 2);
    for (size_t i = 0; i < len; i += 2)
    {
        char a = input[i];
        const char* p = std::lower_bound(lut, lut + 16, a);

        char b = input[i + 1];
        const char* q = std::lower_bound(lut, lut + 16, b);

        output.push_back(((p - lut) << 4) | (q - lut));
    }
    return output;
}

QDate HtmlChartMaker::predictDate(QDate selectedDate, int modelYear, MainWindow* mainWindow, Site* site, float base1, float floraison, float base2, float recolte, int predictionType)
{
    MeteoDatabaseInterface meteoDatabaseInterface;
    meteoDatabaseInterface.setStoragePaths("", mainWindow->getDbPath());

    QStringList years = site->getYears();

    QList<QMap<QString, QStringList> > finalTmpAvg;
    QMap<QString, QStringList> avgOfTempYears;

    foreach(QString year, years)
    {
        qDebug() << "get site with ID :" << site->getId() << " and with year :" << year.toInt();
        Meteo* meteo = meteoDatabaseInterface.getMeteo(site->getId(), year.toInt());
        finalTmpAvg.append(meteo->getMeteo());
    }

    if(1 == modelYear)
    {
        avgOfTempYears = this->calculateAvgOfTempYears(finalTmpAvg);
    }
    else
    {
        qDebug() << "Model year is : " << modelYear;
        QList<QMap<QString, QStringList> > finalTmp;
        Meteo* meteo = meteoDatabaseInterface.getMeteo(site->getId(), modelYear);
        finalTmp.append(meteo->getMeteo());
        avgOfTempYears = this->calculateAvgOfTempYears(finalTmp);
    }


    float tmpFloraison = floraison;
    float tmpSum = 0;
    float lastAdd = 0;
    int exactValues = 0;
    int prognosedValues = 0;
    float tmpRecolte = recolte;
    QString modelYearString = QString::number(modelYear);

    if(1 == modelYearString.length())
        modelYearString = "000" + modelYearString;

    if(2 == predictionType) //recolte prediction
    {
        mainWindow->insertTextInPrevisionsDebugPlainTextEdit("\n### Récolte ###\n");
        while(tmpFloraison > tmpSum)
        {
            bool foundExactTemp = false; //if we find a temperature record for that day we use it.
            QMap<QString, QStringList> mapListAllYears;

            foreach(mapListAllYears, finalTmpAvg)
            {
                if(mapListAllYears.contains(selectedDate.toString("yyyyMMdd")) && mapListAllYears.value(selectedDate.toString("yyyyMMdd")).at(2).toFloat() > 0)
                {
                    lastAdd = mapListAllYears.value(selectedDate.toString("yyyyMMdd")).at(2).toFloat() - base1;
                    tmpSum += lastAdd;
                    exactValues++;
                    foundExactTemp = true;
                }
            }

            if(foundExactTemp == false)
            {
                if(avgOfTempYears.contains("0001" + selectedDate.toString("MMdd")) || 0 == selectedDate.toString("MMdd").compare(QString("0229")))
                {
                    QStringList tempList;

                    if(0 == selectedDate.toString("MMdd").compare("0229"))
                        tempList = avgOfTempYears.value("0001" + selectedDate.toString("0228"));
                    else
                        tempList = avgOfTempYears.value("0001" + selectedDate.toString("MMdd"));

                    if(tempList.size() > 1)
                    {
                        lastAdd = tempList.at(2).toFloat() - base1;
                        tmpSum += lastAdd;
                        prognosedValues++;
                    }
                }
                else
                {
                    QString errorMsg;
                    if(1 == modelYear)
                        errorMsg = QString("Valeur manquante pour la date du " + selectedDate.toString("d MMMM "));
                    else
                        errorMsg = QString("Valeur manquante pour la date du " + selectedDate.toString("d MMMM ") + modelYearString);
                    QMessageBox msgBox;
                    msgBox.warning(NULL, QString("Erreur"), errorMsg);
                    return QDate::fromString("0001","yyyy");
                }
            }
            selectedDate = selectedDate.addDays(1);
            qDebug() << "date :" << selectedDate.toString("yyyy-MM-dd");
            mainWindow->insertTextInPrevisionsDebugPlainTextEdit("date : " + selectedDate.toString("yyyy-MM-dd") + "   " + QString::number(tmpSum,'f',3) + "   + "  + QString::number(lastAdd,'f',3) + "\n");
        }

        qDebug() << "date de floraison :" << selectedDate.toString("yyyy-MM-dd");
        qDebug() << "exact values :" << exactValues;
        qDebug() << "prognosed values :" << prognosedValues;

    }

    if(1 == predictionType)  //floraison prediction
    {
        mainWindow->insertTextInPrevisionsDebugPlainTextEdit("\n### Floraison ###\n");
        while(tmpRecolte > tmpSum)
        {
            bool foundExactTemp = false; //if we find a temperature record for that day we use it.
            QMap<QString, QStringList> mapListAllYears;
            foreach(mapListAllYears, finalTmpAvg)
            {
                if(mapListAllYears.contains(selectedDate.toString("yyyyMMdd")) && mapListAllYears.value(selectedDate.toString("yyyyMMdd")).at(2).toFloat() > 0)
                {
                    lastAdd = mapListAllYears.value(selectedDate.toString("yyyyMMdd")).at(2).toFloat() - base2;
                    tmpSum += lastAdd;
                    exactValues++;
                    foundExactTemp = true;
                }
            }

            if(foundExactTemp == false)
            {
                if(avgOfTempYears.contains("0001" + selectedDate.toString("MMdd")) || 0 == selectedDate.toString("MMdd").compare(QString("0229")))
                {
                    QStringList tempList;

                    if(0 == selectedDate.toString("MMdd").compare("0229"))
                        tempList = avgOfTempYears.value("0001" + selectedDate.toString("0228"));
                    else
                        tempList = avgOfTempYears.value("0001" + selectedDate.toString("MMdd"));

                    if(tempList.size() > 1)
                    {
                        lastAdd = tempList.at(2).toFloat() - base2;
                        tmpSum += lastAdd;
                        prognosedValues++;
                    }
                }
                else
                {
                    qDebug() << selectedDate.toString("MMdd");
                    QString errorMsg;
                    if(1 == modelYear)
                        errorMsg = QString("Valeur manquante pour la date du " + selectedDate.toString("d MMMM "));
                    else
                        errorMsg = QString("Valeur manquante pour la date du " + selectedDate.toString("d MMMM ") + modelYearString);
                    QMessageBox msgBox;
                    msgBox.warning(NULL, QString("Erreur"), errorMsg);
                    return QDate::fromString("0001","yyyy");
                }
            }
            selectedDate = selectedDate.addDays(1);
            qDebug() << "date :" << selectedDate.toString("yyyy-MM-dd");
            mainWindow->insertTextInPrevisionsDebugPlainTextEdit("date : " + selectedDate.toString("yyyy-MM-dd") + "   " + QString::number(tmpSum,'f',3) + "   + "  + QString::number(lastAdd,'f',3) + "\n");
        }

        qDebug() << "date de récolte :" << selectedDate.toString("yyyy-MM-dd");
        qDebug() << "exact values :" << exactValues;
        qDebug() << "prognosed values :" << prognosedValues;
    }

    if(3 == predictionType)  //calculer TIF
    {
        mainWindow->insertTextInPrevisionsDebugPlainTextEdit("\n### TIF ###\n");
        while(tmpRecolte > tmpSum)
        {
            bool foundExactTemp = false; //if we find a temperature record for that day we use it.
            QMap<QString, QStringList> mapListAllYears;
            foreach(mapListAllYears, finalTmpAvg)
            {
                if(mapListAllYears.contains(selectedDate.toString("yyyyMMdd")) && mapListAllYears.value(selectedDate.toString("yyyyMMdd")).at(2).toFloat() > 0)
                {
                    lastAdd = mapListAllYears.value(selectedDate.toString("yyyyMMdd")).at(2).toFloat() - base2;
                    tmpSum += lastAdd;
                    exactValues++;
                    foundExactTemp = true;
                }
            }

            if(foundExactTemp == false)
            {
                if(avgOfTempYears.contains("0001" + selectedDate.toString("MMdd")) || 0 == selectedDate.toString("MMdd").compare(QString("0229")))
                {
                    QStringList tempList;

                    if(0 == selectedDate.toString("MMdd").compare("0229"))
                        tempList = avgOfTempYears.value("0001" + selectedDate.toString("0228"));
                    else
                        tempList = avgOfTempYears.value("0001" + selectedDate.toString("MMdd"));

                    if(tempList.size() > 1)
                    {
                        lastAdd = tempList.at(2).toFloat() - base2;
                        tmpSum += lastAdd;
                        prognosedValues++;
                    }
                }
                else
                {
                    qDebug() << selectedDate.toString("MMdd");
                    QString errorMsg;
                    if(1 == modelYear)
                        errorMsg = QString("Valeur manquante pour la date du " + selectedDate.toString("d MMMM "));
                    else
                        errorMsg = QString("Valeur manquante pour la date du " + selectedDate.toString("d MMMM ") + modelYearString);
                    QMessageBox msgBox;
                    msgBox.warning(NULL, QString("Erreur"), errorMsg);
                    return QDate::fromString("0001","yyyy");
                }
            }
            selectedDate = selectedDate.addDays(-1);
            qDebug() << "date :" << selectedDate.toString("yyyy-MM-dd");
            mainWindow->insertTextInPrevisionsDebugPlainTextEdit("date : " + selectedDate.toString("yyyy-MM-dd") + "   " + QString::number(tmpSum,'f',3) + "   + "  + QString::number(lastAdd,'f',3) + "\n");
        }
        qDebug() << "date du TIF :" << selectedDate.toString("yyyy-MM-dd");
        qDebug() << "exact values :" << exactValues;
        qDebug() << "prognosed values :" << prognosedValues;


        float tmpFloraison = floraison;
        float tmpSum = 0;
        float lastAdd = 0;
        int exactValues = 0;
        int prognosedValues = 0;

        mainWindow->insertTextInPrevisionsDebugPlainTextEdit("\n### 2ème partie ###\n");

        while(tmpFloraison > tmpSum)
        {
            bool foundExactTemp = false; //if we find a temperature record for that day we use it.
            QMap<QString, QStringList> mapListAllYears;
            foreach(mapListAllYears, finalTmpAvg)
            {
                if(mapListAllYears.contains(selectedDate.toString("yyyyMMdd")) && mapListAllYears.value(selectedDate.toString("yyyyMMdd")).at(2).toFloat() > 0)
                {
                    lastAdd = mapListAllYears.value(selectedDate.toString("yyyyMMdd")).at(2).toFloat() - base1;
                    tmpSum += lastAdd;
                    exactValues++;
                    foundExactTemp = true;
                }
            }

            if(foundExactTemp == false)
            {
                if(avgOfTempYears.contains("0001" + selectedDate.toString("MMdd")) || 0 == selectedDate.toString("MMdd").compare(QString("0229")))
                {
                    QStringList tempList;

                    if(0 == selectedDate.toString("MMdd").compare("0229"))
                        tempList = avgOfTempYears.value("0001" + selectedDate.toString("0228"));
                    else
                        tempList = avgOfTempYears.value("0001" + selectedDate.toString("MMdd"));

                    if(tempList.size() > 1)
                    {
                        lastAdd = tempList.at(2).toFloat() - base1;
                        tmpSum += lastAdd;
                        prognosedValues++;
                    }
                }
                else
                {
                    qDebug() << selectedDate.toString("MMdd");
                    QString errorMsg;
                    if(1 == modelYear)
                        errorMsg = QString("Valeur manquante pour la date du " + selectedDate.toString("d MMMM "));
                    else
                        errorMsg = QString("Valeur manquante pour la date du " + selectedDate.toString("d MMMM ") + modelYearString);
                    QMessageBox msgBox;
                    msgBox.warning(NULL, QString("Erreur"), errorMsg);
                    return QDate::fromString("0001","yyyy");
                }
            }
            selectedDate = selectedDate.addDays(-1);
            qDebug() << "date :" << selectedDate.toString("yyyy-MM-dd");
            mainWindow->insertTextInPrevisionsDebugPlainTextEdit("date : " + selectedDate.toString("yyyy-MM-dd") + "   " + QString::number(tmpSum,'f',3) + "   + "  + QString::number(lastAdd,'f',3) + "\n");
        }
        qDebug() << "date du TIF :" << selectedDate.toString("yyyy-MM-dd");
        qDebug() << "exact values :" << exactValues;
        qDebug() << "prognosed values :" << prognosedValues;

    }

    return selectedDate;
}

QMap<QString, QStringList> HtmlChartMaker::calculateAvgOfTempYears(QList<QMap<QString, QStringList> > temps)
{
    //first get years list
    QStringList yearsList;
    QMap<QString, QStringList> tempsMap;
    QMap<QString, QStringList> avgMap;

    foreach(tempsMap, temps)
    {
        foreach(QStringList temperatures, tempsMap)
        {
            if(temperatures.size() > 0)
            {
                if(false == yearsList.contains(temperatures.at(0).mid(0, 4)))
                {
                    yearsList.append(temperatures.at(0).mid(0, 4));
                }
            }
        }
    }

    QDateTime qDate0 = QDateTime::fromString("0001-01-01","yyyy-MM-dd");

    do //
    {
        //qDebug() << "qDate0.toString(yyyy).toInt() " << qDate0.toString("yyyyMM").totInt();

        QMap<QString, QStringList> tempsMap;
        float avgTempAvg = 0;
        float avgTempMin = 0;
        float avgTempMax = 0;

        float tempsAvgSum = 0;
        float tempsMinSum = 0;
        float tempsMaxSum = 0;

        int tempsCount = 0;
        foreach(tempsMap, temps)
        {
            foreach(QString year, yearsList)
            {
                qDebug() << "qmap key : " << qDate0.toString(year + qDate0.toString("MMdd"));
                if(true == tempsMap.contains(qDate0.toString(year + qDate0.toString("MMdd"))))
                {
                    QStringList tmpList = tempsMap.value(qDate0.toString(year + qDate0.toString("MMdd")));
                    if(tmpList.size() >= 4 && tmpList.at(1).toFloat() > 0 && tmpList.at(2).toFloat() > 0 && tmpList.at(3).toFloat() > 0)
                    {
                        tempsAvgSum += tmpList.at(1).toFloat();
                        tempsMinSum += tmpList.at(2).toFloat();
                        tempsMaxSum += tmpList.at(3).toFloat();
                        tempsCount++;
                    }
                }
            }

        }

        if(tempsCount > 0)
        {
            avgTempAvg = tempsAvgSum / (float)tempsCount;
            avgTempMin = tempsMinSum / (float)tempsCount;
            avgTempMax = tempsMaxSum / (float)tempsCount;

            QStringList resTemps;
            qDebug() << "will insert date : " << qDate0.toString("yyyyMMdd");
            qDebug() << "will insert avg temp : " << QString::number(avgTempAvg);
            resTemps.append(qDate0.toString("yyyyMMdd"));
            resTemps.append(QString::number(avgTempAvg));
            resTemps.append(QString::number(avgTempMin));
            resTemps.append(QString::number(avgTempMax));
            avgMap.insert(qDate0.toString("yyyyMMdd"), resTemps);
        }
        qDate0 = qDate0.addDays(1);
    } while(qDate0.toString("yyyy").toInt() < 2);

    return avgMap;
}

QMap<QString, float> HtmlChartMaker::calculateDayTempAverage(QList<QStringList> temperatures)
{
    QMap<QString, QStringList> dayTempAvgMap;
    QMap<QString, float> dayTempAvgFloatMap;

    foreach(QStringList temperature, temperatures)
    {
        if(temperature.size() >= 4)
        {
            QString date = temperature.at(0);
            QString tmp = temperature.at(1);

            QDateTime qDate = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");
            //qDebug() << "nDate : " << qDate.toString("yyyyMMdd");
            QStringList dayTempsList = dayTempAvgMap.value(qDate.toString("yyyyMMdd"));
            dayTempsList.append(tmp);
            dayTempAvgMap.insert(qDate.toString("yyyyMMdd"), dayTempsList);
        }
        else if(temperature.size() >= 2)
        {
            QString date = temperature.at(0);
            QString tmp = temperature.at(1);

            QDateTime qDate = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");
            //qDebug() << "nDate : " << qDate.toString("yyyyMMdd");
            QStringList dayTempsList = dayTempAvgMap.value(qDate.toString("yyyyMMdd"));
            dayTempsList.append(tmp);
            dayTempAvgMap.insert(qDate.toString("yyyyMMdd"), dayTempsList);
        }
    }

    foreach(QString qMapKey, dayTempAvgMap.keys())
    {
        QStringList qMapElement = dayTempAvgMap.value(qMapKey);
        float sum = 0;
        for(int i=0; i < qMapElement.size(); i++)
        {
            sum += qMapElement.at(i).toFloat();
        }
        float avg = sum / (float)qMapElement.size();

        //QString qsSum;
        //qDebug() << "day avg : " << qsSum.setNum(avg, 'f', 2);

        dayTempAvgFloatMap.insert(qMapKey, avg);
    }

    return dayTempAvgFloatMap;
}


QMap<QString, float> HtmlChartMaker::calculateWeekTempAverage(QMap<QString, float> dayTempAvgMap)
{
    QMap<QString, QStringList> weekTempAvgMap;
    QMap<QString, float> weekTempAvgFloatMap;

    foreach(QString qMapKey, dayTempAvgMap.keys())
    {
        QString date = QString::number(QDate::fromString(qMapKey, "yyyyMMdd").weekNumber());
        QString tmp;
        tmp = tmp.setNum(dayTempAvgMap.value(qMapKey), 'f', 2);
        QStringList tmpStringList = weekTempAvgMap.value(date);
        tmpStringList.append(tmp);
        weekTempAvgMap.insert(date, tmpStringList);
    }

    foreach(QString qMapKey, weekTempAvgMap.keys())
    {
        QStringList tmpStringList = weekTempAvgMap.value(qMapKey);
        float sum = 0;
        for(int i=0; i < tmpStringList.size(); i++)
        {
            sum += tmpStringList.at(i).toFloat();
        }
        float avg = sum / (float)tmpStringList.size();
        weekTempAvgFloatMap.insert(qMapKey, avg);
    }

    return weekTempAvgFloatMap;
}

QMap<QString, float> HtmlChartMaker::calculateMonthTempAverage(QMap<QString, float> dayTempAvgMap)
{
    QMap<QString, QStringList> monthTempAvgMap;
    QMap<QString, float> monthTempAvgFloatMap;

    foreach(QString qMapKey, dayTempAvgMap.keys())
    {
        QString date = QDateTime::fromString(qMapKey, "yyyyMMdd").toString("yyyyMM");
        QString tmp;
        tmp = tmp.setNum(dayTempAvgMap.value(qMapKey), 'f', 2);
        QStringList tmpStringList = monthTempAvgMap.value(date);
        tmpStringList.append(tmp);
        monthTempAvgMap.insert(date, tmpStringList);
    }

    foreach(QString qMapKey, monthTempAvgMap.keys())
    {
        QStringList tmpStringList = monthTempAvgMap.value(qMapKey);
        float sum = 0;
        for(int i=0; i < tmpStringList.size(); i++)
        {
            sum += tmpStringList.at(i).toFloat();
        }
        float avg = sum / (float)tmpStringList.size();
        monthTempAvgFloatMap.insert(qMapKey, avg);
    }

    return monthTempAvgFloatMap;
}

QMap<QString, float> HtmlChartMaker::calculateMaxDayTemp(QList<QStringList> temperatures)
{
    QMap<QString, QStringList> dayTempMap;
    QMap<QString, float> dayTempMaxFloatMap;

    foreach(QStringList temperature, temperatures)
    {
        if(temperature.size() >= 4)
        {
            QString date = temperature.at(0);
            QString tmp = temperature.at(3);

            QDateTime qDate = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");
            QStringList dayTempsList = dayTempMap.value(qDate.toString("yyyyMMdd"));
            dayTempsList.append(tmp);
            dayTempMap.insert(qDate.toString("yyyyMMdd"), dayTempsList);
        }
        else if(temperature.size() >=2)
        {
            QString date = temperature.at(0);
            QString tmp = temperature.at(1);

            QDateTime qDate = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");
            QStringList dayTempsList = dayTempMap.value(qDate.toString("yyyyMMdd"));
            dayTempsList.append(tmp);
            dayTempMap.insert(qDate.toString("yyyyMMdd"), dayTempsList);
        }
    }

    foreach(QString qMapKey, dayTempMap.keys())
    {
        QStringList qMapElement = dayTempMap.value(qMapKey);
        float max = 0;
        for(int i=0; i < qMapElement.size(); i++)
        {
            if(qMapElement.at(i).toFloat() > max)
                max = qMapElement.at(i).toFloat() ;
        }

        dayTempMaxFloatMap.insert(qMapKey, max);
    }

    return dayTempMaxFloatMap;
}

QMap<QString, float> HtmlChartMaker::calculateMaxMonthTemp(QMap<QString, float> dayMaxTempMap)
{
    QMap<QString, QStringList> monthMaxTempMap;
    QMap<QString, float> monthMaxTempFloatMap;

    foreach(QString qMapKey, dayMaxTempMap.keys())
    {
        QString date = QDateTime::fromString(qMapKey, "yyyyMMdd").toString("yyyyMM");
        QString tmp;
        tmp = tmp.setNum(dayMaxTempMap.value(qMapKey), 'f', 2);
        QStringList tmpStringList = monthMaxTempMap.value(date);
        tmpStringList.append(tmp);
        monthMaxTempMap.insert(date, tmpStringList);
    }

    foreach(QString qMapKey, monthMaxTempMap.keys())
    {
        QStringList tmpStringList = monthMaxTempMap.value(qMapKey);
        float max = 0;
        for(int i=0; i < tmpStringList.size(); i++)
        {
            if(tmpStringList.at(i).toFloat() > max)
                max = tmpStringList.at(i).toFloat();
        }
        monthMaxTempFloatMap.insert(qMapKey, max);
    }

    return monthMaxTempFloatMap;
}


QMap<QString, float> HtmlChartMaker::calculateMinDayTemp(QList<QStringList> temperatures)
{
    QMap<QString, QStringList> dayTempMap;
    QMap<QString, float> dayTempMinFloatMap;

    foreach(QStringList temperature, temperatures)
    {
        if(temperature.size() >= 4)
        {
            QString date = temperature.at(0);
            QString tmp = temperature.at(2);
            QDateTime qDate = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");
            QStringList dayTempsList = dayTempMap.value(qDate.toString("yyyyMMdd"));
            dayTempsList.append(tmp);
            dayTempMap.insert(qDate.toString("yyyyMMdd"), dayTempsList);
        }
        else if(temperature.size() >= 2)
        {
            QString date = temperature.at(0);
            QString tmp = temperature.at(1);
            QDateTime qDate = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");
            QStringList dayTempsList = dayTempMap.value(qDate.toString("yyyyMMdd"));
            dayTempsList.append(tmp);
            dayTempMap.insert(qDate.toString("yyyyMMdd"), dayTempsList);
        }
    }

    foreach(QString qMapKey, dayTempMap.keys())
    {
        QStringList qMapElement = dayTempMap.value(qMapKey);
        float min = 10000;
        for(int i=0; i < qMapElement.size(); i++)
        {
            if(qMapElement.at(i).toFloat() < min)
                min = qMapElement.at(i).toFloat();
        }
        dayTempMinFloatMap.insert(qMapKey, min);
    }
    return dayTempMinFloatMap;
}

QMap<QString, float> HtmlChartMaker::calculateMinMonthTemp(QMap<QString, float> dayMinTempMap)
{
    QMap<QString, QStringList> monthMinTempMap;
    QMap<QString, float> monthMinTempFloatMap;

    foreach(QString qMapKey, dayMinTempMap.keys())
    {
        QString date = QDateTime::fromString(qMapKey, "yyyyMMdd").toString("yyyyMM");
        QString tmp;
        tmp = tmp.setNum(dayMinTempMap.value(qMapKey), 'f', 2);
        QStringList tmpStringList = monthMinTempMap.value(date);
        tmpStringList.append(tmp);
        monthMinTempMap.insert(date, tmpStringList);
    }

    foreach(QString qMapKey, monthMinTempMap.keys())
    {
        QStringList tmpStringList = monthMinTempMap.value(qMapKey);
        float min = 10000;
        for(int i=0; i < tmpStringList.size(); i++)
        {
            if(tmpStringList.at(i).toFloat() < min)
                min = tmpStringList.at(i).toFloat();
        }
        monthMinTempFloatMap.insert(qMapKey, min);
    }

    return monthMinTempFloatMap;
}

QStringList HtmlChartMaker::getYearsWithTempData(QList<QStringList> temperatures)
{
    QStringList years;

    foreach(QStringList temperature, temperatures)
    {
        if(temperature.size() > 1)
        {
            QString date = temperature.at(0);
            QDateTime qDate = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");
            QString QsDate = qDate.toString("yyyy");
            if(false == years.contains(QsDate))
                years.append(QsDate);
        }
        qSort(years.begin(),years.end(),qGreater<QString>());
    }
    return years;
}

QString HtmlChartMaker::generateHtmlChartWithTempData(QList<QStringList> temperatures)
{
    QMap<QString, float> dayMaxTempMap = this->calculateMaxDayTemp(temperatures);
    QMap<QString, float> dayTempAvgMap = this->calculateDayTempAverage(temperatures);
    QMap<QString, float> dayMinTempMap = this->calculateMinDayTemp(temperatures);

    return this->generateHtmlChartWithMaps(dayMaxTempMap, dayTempAvgMap, dayMinTempMap);
}

QString HtmlChartMaker::generateHtmlChartWithMap(QMap<QString, QStringList> tempMap)
{
    return this->generateHtmlChartWithMap(tempMap, 0, false);
}

QString HtmlChartMaker::generateHtmlChartWithMap(QMap<QString, QStringList> tempMap, bool completeMissingMonth)
{
    return this->generateHtmlChartWithMap(tempMap, 0, completeMissingMonth);
}

QString HtmlChartMaker::generateHtmlChartWithMap(QMap<QString, QStringList> tempMap, int year, bool completeMissingMonth)
{

    QMap<QString, float> dayMaxTempMap;
    QMap<QString, float> dayTempAvgMap;
    QMap<QString, float> dayMinTempMap;

    foreach(QString qMapKey, tempMap.keys())
    {
        QStringList tempList = tempMap.value(qMapKey);
        ///*
        dayMinTempMap.insert(qMapKey, tempList.at(1).toFloat());
        dayTempAvgMap.insert(qMapKey, tempList.at(2).toFloat());
        dayMaxTempMap.insert(qMapKey, tempList.at(3).toFloat());
        //*/
    }

    return this->generateHtmlChartWithMaps(dayMaxTempMap, dayTempAvgMap, dayMinTempMap, year, completeMissingMonth);
}

QString HtmlChartMaker::generateHtmlChartWithMaps(QMap<QString, float> dayMaxTempMap, QMap<QString, float> dayTempAvgMap, QMap<QString, float> dayMinTempMap)
{
    return this->generateHtmlChartWithMaps(dayMaxTempMap, dayTempAvgMap, dayMinTempMap, 0, false);
}

QString HtmlChartMaker::generateHtmlWeekIntervalle(QMap<int, QStringList> weekList)
{
    QString dataFloraison = "";
    QString dataRecolte = "";
    QString temperatures ="";

    int i = 0;
    foreach(int qMapKey, weekList.keys())
    {
        QStringList list = weekList.value(qMapKey);

        if(0 == i)
        {
            dataFloraison.append("['semaine " + QString::number(i+1) + "',"); //Floraison
            dataFloraison.append("" + list.at(0) + "]");

            dataRecolte.append("['semaine " + QString::number(i+1) + "',"); //Récolte
            dataRecolte.append("" + list.at(1) + "]");

            temperatures.append("['semaine " + QString::number(i+1) + "',"); //Témpératures
            temperatures.append("" + list.at(2) + "]");
        }
        else
        {
            dataFloraison.append(",['semaine " + QString::number(i+1) + "',"); //Floraison
            dataFloraison.append("" + list.at(0) + "]");

            dataRecolte.append(",['semaine " + QString::number(i+1) + "',"); //Récolte
            dataRecolte.append("" + list.at(1) + "]");

            temperatures.append(",['semaine " + QString::number(i+1) + "',"); //Témpératures
            temperatures.append("" + list.at(2) + "]");
        }

        i++;
    }

    QString html = "";

    QFile mFile(":./html/highcharts-all.js");

    if(!mFile.open(QFile::ReadOnly | QFile::Text))
    {
           qDebug() << "could not open file for read";
    }

    QTextStream in(&mFile);
    QString libText = in.readAll();


    mFile.close();

    html.append("<html>\n");

    html.append("<div id='container' style=''></div>\n");

    html.append("<script type='text/javascript'>\n");
    html.append(libText);
    html.append("</script>\n");

    html.append("<script type='text/javascript'>\n");
    //html.append("$(function () {\n");
    html.append("chart = new Highcharts.Chart({\n");
    html.append("chart: {\n");
    html.append("    type: 'spline',\n");
    html.append("    renderTo: 'container'\n");
    html.append("},\n");
    html.append("title: {\n");
    html.append("    text: 'Intervalle TIF / Floraison / Récolte'\n");
    html.append("},\n");
    html.append("xAxis: {\n");
    html.append("   title: {\n");
    html.append("       text: 'semaine n°'\n");
    html.append("   }\n");
    html.append("},\n");
    html.append("yAxis: [\n");
    html.append("{\n");
    html.append("   title: {\n");
    html.append("       text: 'nombre de jours'\n");
    html.append("   },\n");
    //html.append("   min: 0\n");
    html.append("},\n");

    html.append("{\n");
    html.append("   title: {\n");
    html.append("       text: 'Témpérature'\n");
    html.append("   },\n");
    //html.append("   min: 0,\n");
    html.append("   opposite: true\n");
    html.append("}\n");

    html.append("],\n");
    html.append("exporting: { enabled: false },\n");

    //températures
    html.append("series: [{\n");
    html.append("name: 'Températures',\n");
    html.append("tooltip: {\n");
    html.append("   headerFormat: '<b>{series.name}</b><br>',\n");
    html.append("   pointFormat: 'semaine {point.x}: {point.y}°C'\n");
    html.append("},\n");
    html.append("yAxis: 1,\n");
    html.append("data: [\n");
    html.append(temperatures);
    html.append("]\n");

    //récolte
    html.append("}, {\n");
    html.append("name: 'Récolte',\n");
    html.append("tooltip: {\n");
    html.append("   headerFormat: '<b>{series.name}</b><br>',\n");
    html.append("   pointFormat: 'semaine {point.x}: {point.y} jours'\n");
    html.append("},\n");
    html.append("yAxis: 0,\n");
    html.append("data: [\n");
    html.append(dataRecolte);
    html.append("],\n");

    //floraison
    html.append("}, {\n");
    html.append("   name: 'Floraison',\n");
    html.append("   yAxis: 0,\n");
    html.append("tooltip: {\n");
    html.append("   headerFormat: '<b>{series.name}</b><br>',\n");
    html.append("   pointFormat: 'semaine {point.x}: {point.y} jours'\n");
    html.append("},\n");
    html.append("   data: [\n");
    html.append(dataFloraison);
    html.append("]\n");

    //End of Script

    html.append("    }]\n");
    html.append("    });\n");
    //html.append("    });\n");
    html.append("</script>\n");

    html.append("</html>\n");

    qDebug() << "HTML : " << html;
    return html;
}

/*
// OLD chart.js version
QString HtmlChartMaker::generateHtmlWeekIntervalle(QMap<int, QStringList> weekList)
{
    QString labels = "";
    QString floraisonValues = "";
    QString recolteValues = "";

    int i = 0;
    foreach(int qMapKey, weekList.keys())
    {
        QStringList list = weekList.value(qMapKey);

        if(0 == i)
        {
            labels.append("'semaine " + QString::number(i+1) + "'");
            floraisonValues.append("'" + list.at(0) + "'");
            recolteValues.append("'" + list.at(1) + "'");
        }
        else
        {
            labels.append(", 'semaine " + QString::number(i+1) + "'");
            floraisonValues.append(", '" + list.at(0) + "'");
            recolteValues.append(", '" + list.at(1) + "'");
        }

        i++;
    }

    QString html = "";
    html.append("<!doctype html>\n\
                    <html>\n\
                        <head>\n\
                            <title>Line Chart</title>\n\
                            <script>\n");

    html.append(this->getCharJSLib());

    html.append("</script>\n\
                    </head>\n\
                    <body>\n\
                        <div>\n\
                            <div>\n\
                                <canvas id=\"canvas\" height=\"300px\" width=\"400px\"></canvas>\n\
                            </div>\n\
                        </div>\n\
                    <script>\n\
                        var randomScalingFactor = function(){ return Math.round(Math.random()*100)};\n\
                        var lineChartData = {\n\
                            labels : [");

     html.append(labels);
     html.append("],\n\
                     datasets : [\n\
                    {\n\
                        label: \"Température maximale\",\n\
                        fillColor : \"rgba(254,46,46,0.2)\",\n\
                        strokeColor : \"rgba(254,46,46,1)\",\n\
                        pointColor : \"rgba(254,46,46,1)\",\n\
                        pointStrokeColor : \"#fff\",\n\
                        pointHighlightFill : \"#fff\",\n\
                        pointHighlightStroke : \"rgba(254,46,46,1)\",\n\
                        data : [");

    html.append(recolteValues);

    html.append("]\n\
                    }\n\
                    ,");


    html.append("{\n\
                                    label: \"Température moyenne\",\n\
                                    fillColor : \"rgba(160,160,160,0.2)\",\n\
                                    strokeColor : \"rgba(160,160,160,1)\",\n\
                                    pointColor : \"rgba(160,160,160,1)\",\n\
                                    pointStrokeColor : \"#fff\",\n\
                                    pointHighlightFill : \"#fff\",\n\
                                    pointHighlightStroke : \"rgba(160,160,160,1)\",\n\
                                    data : [");

    html.append(floraisonValues);
    html.append("]\n\
                  }");


    html.append("]\n\
                                }\n\
                    window.onload = function(){\n\
                       window.onresize = function(event) {\n\
                                document.getElementById(\"canvas\").style.width = window.innerWidth - 20;\n\
                                document.getElementById(\"canvas\").style.height = window.innerHeight - 120;\n\
                            }\n\
                            document.getElementById(\"canvas\").style.width = window.innerWidth - 20;\n\
                            document.getElementById(\"canvas\").style.height = window.innerHeight - 120;\n\
                        var ctx = document.getElementById(\"canvas\").getContext(\"2d\");\n\
                        window.myLine = new Chart(ctx).Line(lineChartData, {\n\
                            responsive: true,\n\
                            pointHitDetectionRadius : 2,\n\
                            scaleBeginAtZero : false, \n\
                            scaleShowLine : false, \n\
                            datasetFill : false\n\
                        });\n\
                        \n\
                        \n\
                    }\n\
                    </script>\n\
                        <script>\n\
                        </script>\n\
                    </body>\n\
                </html>");

        return html;
}
*/

QString HtmlChartMaker::generateHtmlChartWithMaps(QMap<QString, float> dayMaxTempMap, QMap<QString, float> dayTempAvgMap, QMap<QString, float> dayMinTempMap, int year, bool completeMissingMonth)
{

    QMap<QString, float> monthMaxTempMap = this->calculateMonthTempAverage(dayMaxTempMap);
    QMap<QString, float> monthTempAvgMap = this->calculateMonthTempAverage(dayTempAvgMap);
    QMap<QString, float> monthMinTempMap = this->calculateMonthTempAverage(dayMinTempMap);

    QString avgValues = "";

    if(true == completeMissingMonth && 0 != year)
    for(int i = 1; i <= 12; i++)
    {
        QString newMapKey = QString::number(year);
        if(1 == newMapKey.size())
            newMapKey = "000" + newMapKey;
        QString month = QString::number(i);
        if(1 == month.length())
        {
            month = "0" + month;
        }
        newMapKey = newMapKey + month;

        if(false == monthMaxTempMap.contains(newMapKey))
        {
            monthMaxTempMap.insert(newMapKey, 0);
            monthTempAvgMap.insert(newMapKey, 0);
            monthMinTempMap.insert(newMapKey, 0);
        }
    }

    int i = 0;
    foreach(QString qMapKey, monthTempAvgMap.keys())
    {
        QString qsAvg;
        qsAvg = qsAvg.setNum(monthTempAvgMap.value(qMapKey), 'f', 2);
        if(i == 0)
        {
            if(qsAvg.toFloat() != 0)
                avgValues.append(qsAvg);
            else
                avgValues.append(" 0");
        }
        else
        {
            if(qsAvg.toFloat() != 0)
                avgValues.append("," + qsAvg);
            else
                avgValues.append(", 0");
        }
        i++;
    }

    QString maxValues = "";
    i = 0;
    foreach(QString qMapKey, monthMaxTempMap.keys())
    {
        QString qsMax;
        qsMax = qsMax.setNum(monthMaxTempMap.value(qMapKey), 'f', 2);

        if(i == 0)
            maxValues.append(qsMax);
        else
            maxValues.append("," + qsMax);
        i++;
    }

    QString minValues = "";
    i = 0;
    foreach(QString qMapKey, monthMinTempMap.keys())
    {
        QString qsMin;
        qsMin = qsMin.setNum(monthMinTempMap.value(qMapKey), 'f', 2);
        if(i == 0)
            minValues.append(qsMin);
        else
            minValues.append("," + qsMin);
        i++;
    }

    QString dates = "";
    i = 0;
    foreach(QString qMapKey, monthTempAvgMap.keys())
    {
        QDateTime qDate;
        qDate = qDate.fromString(qMapKey, "yyyyMM");
        QString qsDate;
        if(0 == qDate.toString("yyyy").compare("0001"))
            qsDate = qDate.toString("MMMM");
        else
            qsDate = qDate.toString("MMMM yyyy");

        if(i == 0)
            dates.append("'" + qsDate + "'");
        else
            dates.append(",'" + qsDate + "'");
        i++;
    }

    QString html = "";
    html.append("<!doctype html>\n\
                    <html>\n\
                        <head>\n\
                            <title>Line Chart</title>\n\
                            <script>\n");

    html.append(this->getCharJSLib());

    html.append("</script>\n\
                    </head>\n\
                    <body>\n\
                        <div>\n\
                            <div>\n\
                                <canvas id=\"canvas\" height=\"300px\" width=\"400px\"></canvas>\n\
                            </div>\n\
                        </div>\n\
                    <script>\n\
                        var randomScalingFactor = function(){ return Math.round(Math.random()*100)};\n\
                        var lineChartData = {\n\
                            labels : [");

     html.append(dates);
     html.append("],\n\
                     datasets : [\n\
                    {\n\
                        label: \"Température maximale\",\n\
                        fillColor : \"rgba(254,46,46,0.2)\",\n\
                        strokeColor : \"rgba(254,46,46,1)\",\n\
                        pointColor : \"rgba(254,46,46,1)\",\n\
                        pointStrokeColor : \"#fff\",\n\
                        pointHighlightFill : \"#fff\",\n\
                        pointHighlightStroke : \"rgba(254,46,46,1)\",\n\
                        data : [");

    html.append(maxValues);

    html.append("]\n\
                    }\n\
                    ,");


    html.append("{\n\
                                    label: \"Température moyenne\",\n\
                                    fillColor : \"rgba(160,160,160,0.2)\",\n\
                                    strokeColor : \"rgba(160,160,160,1)\",\n\
                                    pointColor : \"rgba(160,160,160,1)\",\n\
                                    pointStrokeColor : \"#fff\",\n\
                                    pointHighlightFill : \"#fff\",\n\
                                    pointHighlightStroke : \"rgba(160,160,160,1)\",\n\
                                    data : [");

    html.append(avgValues);
    html.append("]\n\
                  },");

    html.append("{\n\
                                    label: \"Température minimale\",\n\
                                    fillColor : \"rgba(46,100,254,0.2)\",\n\
                                    strokeColor : \"rgba(46,100,254,1)\",\n\
                                    pointColor : \"rgba(46,100,254,1)\",\n\
                                    pointStrokeColor : \"#fff\",\n\
                                    pointHighlightFill : \"#fff\",\n\
                                    pointHighlightStroke : \"rgba(46,100,254,1)\",\n\
                                    data : [");

    html.append(minValues);
    html.append("]\n\
                  }");

    html.append("]\n\
                                }\n\
                    window.onload = function(){\n\
                       window.onresize = function(event) {\n\
                                document.getElementById(\"canvas\").style.width = window.innerWidth - 20;\n\
                                document.getElementById(\"canvas\").style.height = window.innerHeight - 120;\n\
                            }\n\
                            document.getElementById(\"canvas\").style.width = window.innerWidth - 20;\n\
                            document.getElementById(\"canvas\").style.height = window.innerHeight - 120;\n\
                        var ctx = document.getElementById(\"canvas\").getContext(\"2d\");\n\
                        window.myLine = new Chart(ctx).Line(lineChartData, {\n\
                            responsive: true,\n\
                            pointHitDetectionRadius : 5,\n\
                            scaleBeginAtZero : false, \n\
                            scaleShowLine : false, \n\
                            datasetFill : false\n\
                        });\n\
                        \n\
                        \n\
                    }\n\
                    </script>\n\
                        <script>\n\
                        </script>\n\
                    </body>\n\
                </html>");

        return html;
}

void HtmlChartMaker::setHeight(int height)
{
    this->height = height;
}

int HtmlChartMaker::getHeight()
{
    return this->height;
}

void HtmlChartMaker::setWidth(int width)
{
    this->width = width;
}

int HtmlChartMaker::getWidth()
{
    return this->width;
}

QString HtmlChartMaker::getCharJSLib()
{

    std::stringstream libHex;
    libHex << std::hex << "2f2a210a202a2043686172742e6a730a202a20687474703a2f2f63686172746a732e6f72672f0a202a0a202a20436f707972696768742032303134204e69636b20446f776e69650a202a2052656c656173656420756e64657220746865204d4954206c6963656e73650a202a2068747470733a2f2f6769746875622e636f6d2f6e6e6e69636b2f43686172742e6a732f626c6f622f6d61737465722f4c4943454e53452e6d640a202a2f0a0a0a2866756e6374696f6e28297b0a0a092275736520737472696374223b0a0a092f2f4465636c61726520726f6f74207661726961626c65202d2077696e646f7720696e207468652062726f777365722c20676c6f62616c206f6e20746865207365727665720a0976617220726f6f74203d20746869732c0a090970726576696f7573203d20726f6f742e43686172743b0a0a092f2f4f63637570792074686520676c6f62616c207661726961626c65206f662043686172742c20616e642063726561746520612073696d706c65206261736520636c6173730a09766172204368617274203d2066756e6374696f6e28636f6e74657874297b0a0909766172206368617274203d20746869733b0a0909746869732e63616e766173203d20636f6e746578742e63616e7661733b0a0a0909746869732e637478203d20636f6e746578743b0a0a09092f2f5661726961626c65732067"
              "6c6f62616c20746f207468652063686172740a0909766172207769647468203d20746869732e7769647468203d20636f6e746578742e63616e7661732e77696474683b0a090976617220686569676874203d20746869732e686569676874203d20636f6e746578742e63616e7661732e6865696768743b0a0909746869732e617370656374526174696f203d20746869732e7769647468202f20746869732e6865696768743b0a09092f2f4869676820706978656c2064656e7369747920646973706c617973202d206d756c7469706c79207468652073697a65206f66207468652063616e766173206865696768742f7769647468206279207468652064657669636520706978656c20726174696f2c207468656e207363616c652e0a090968656c706572732e726574696e615363616c652874686973293b0a0a090972657475726e20746869733b0a097d3b0a092f2f476c6f62616c6c79206578706f7365207468652064656661756c747320746f20616c6c6f7720666f722075736572207570646174696e672f6368616e67696e670a0943686172742e64656661756c7473203d207b0a0909676c6f62616c3a207b0a0909092f2f20426f6f6c65616e202d205768657468657220746f20616e696d617465207468652063686172740a090909616e696d6174696f6e3a20747275652c0a0a0909092f2f204e756d626572"
              "202d204e756d626572206f6620616e696d6174696f6e2073746570730a090909616e696d6174696f6e53746570733a2036302c0a0a0909092f2f20537472696e67202d20416e696d6174696f6e20656173696e67206566666563740a090909616e696d6174696f6e456173696e673a2022656173654f75745175617274222c0a0a0909092f2f20426f6f6c65616e202d2049662077652073686f756c642073686f7720746865207363616c6520617420616c6c0a09090973686f775363616c653a20747275652c0a0a0909092f2f20426f6f6c65616e202d2049662077652077616e7420746f206f7665727269646520776974682061206861726420636f646564207363616c650a0909097363616c654f766572726964653a2066616c73652c0a0a0909092f2f202a2a205265717569726564206966207363616c654f766572726964652069732074727565202a2a0a0909092f2f204e756d626572202d20546865206e756d626572206f6620737465707320696e2061206861726420636f646564207363616c650a0909097363616c6553746570733a206e756c6c2c0a0909092f2f204e756d626572202d205468652076616c7565206a756d7020696e20746865206861726420636f646564207363616c650a0909097363616c655374657057696474683a206e756c6c2c0a0909092f2f204e756d626572202d2054686520"
              "7363616c65207374617274696e672076616c75650a0909097363616c65537461727456616c75653a206e756c6c2c0a0a0909092f2f20537472696e67202d20436f6c6f7572206f6620746865207363616c65206c696e650a0909097363616c654c696e65436f6c6f723a20227267626128302c302c302c2e3129222c0a0a0909092f2f204e756d626572202d20506978656c207769647468206f6620746865207363616c65206c696e650a0909097363616c654c696e6557696474683a20312c0a0a0909092f2f20426f6f6c65616e202d205768657468657220746f2073686f77206c6162656c73206f6e20746865207363616c650a0909097363616c6553686f774c6162656c733a20747275652c0a0a0909092f2f20496e746572706f6c61746564204a5320737472696e67202d2063616e206163636573732076616c75650a0909097363616c654c6162656c3a20223c253d76616c7565253e222c0a0a0909092f2f20426f6f6c65616e202d205768657468657220746865207363616c652073686f756c6420737469636b20746f20696e7465676572732c20616e64206e6f742073686f7720616e7920666c6f617473206576656e2069662064726177696e672073706163652069732074686572650a0909097363616c65496e7465676572734f6e6c793a20747275652c0a0a0909092f2f20426f6f6c65616e202d2057"
              "68657468657220746865207363616c652073686f756c64207374617274206174207a65726f2c206f7220616e206f72646572206f66206d61676e697475646520646f776e2066726f6d20746865206c6f776573742076616c75650a0909097363616c65426567696e41745a65726f3a2066616c73652c0a0a0909092f2f20537472696e67202d205363616c65206c6162656c20666f6e74206465636c61726174696f6e20666f7220746865207363616c65206c6162656c0a0909097363616c65466f6e7446616d696c793a20222748656c766574696361204e657565272c202748656c766574696361272c2027417269616c272c2073616e732d7365726966222c0a0a0909092f2f204e756d626572202d205363616c65206c6162656c20666f6e742073697a6520696e20706978656c730a0909097363616c65466f6e7453697a653a2031322c0a0a0909092f2f20537472696e67202d205363616c65206c6162656c20666f6e7420776569676874207374796c650a0909097363616c65466f6e745374796c653a20226e6f726d616c222c0a0a0909092f2f20537472696e67202d205363616c65206c6162656c20666f6e7420636f6c6f75720a0909097363616c65466f6e74436f6c6f723a202223363636222c0a0a0909092f2f20426f6f6c65616e202d2077686574686572206f72206e6f742074686520636861727420"
              "73686f756c6420626520726573706f6e7369766520616e6420726573697a65207768656e207468652062726f7773657220646f65732e0a090909726573706f6e736976653a2066616c73652c0a0a0909092f2f20426f6f6c65616e202d2044657465726d696e6573207768657468657220746f206472617720746f6f6c74697073206f6e207468652063616e766173206f72206e6f74202d206174746163686573206576656e747320746f20746f7563686d6f76652026206d6f7573656d6f76650a09090973686f77546f6f6c746970733a20747275652c0a0a0909092f2f204172726179202d204172726179206f6620737472696e67206e616d657320746f2061747461636820746f6f6c746970206576656e74730a090909746f6f6c7469704576656e74733a205b226d6f7573656d6f7665222c2022746f7563687374617274222c2022746f7563686d6f7665222c20226d6f7573656f7574225d2c0a0a0909092f2f20537472696e67202d20546f6f6c746970206261636b67726f756e6420636f6c6f75720a090909746f6f6c74697046696c6c436f6c6f723a20227267626128302c302c302c302e3829222c0a0a0909092f2f20537472696e67202d20546f6f6c746970206c6162656c20666f6e74206465636c61726174696f6e20666f7220746865207363616c65206c6162656c0a090909746f6f6c746970466f"
              "6e7446616d696c793a20222748656c766574696361204e657565272c202748656c766574696361272c2027417269616c272c2073616e732d7365726966222c0a0a0909092f2f204e756d626572202d20546f6f6c746970206c6162656c20666f6e742073697a6520696e20706978656c730a090909746f6f6c746970466f6e7453697a653a2031342c0a0a0909092f2f20537472696e67202d20546f6f6c74697020666f6e7420776569676874207374796c650a090909746f6f6c746970466f6e745374796c653a20226e6f726d616c222c0a0a0909092f2f20537472696e67202d20546f6f6c746970206c6162656c20666f6e7420636f6c6f75720a090909746f6f6c746970466f6e74436f6c6f723a202223666666222c0a0a0909092f2f20537472696e67202d20546f6f6c746970207469746c6520666f6e74206465636c61726174696f6e20666f7220746865207363616c65206c6162656c0a090909746f6f6c7469705469746c65466f6e7446616d696c793a20222748656c766574696361204e657565272c202748656c766574696361272c2027417269616c272c2073616e732d7365726966222c0a0a0909092f2f204e756d626572202d20546f6f6c746970207469746c6520666f6e742073697a6520696e20706978656c730a090909746f6f6c7469705469746c65466f6e7453697a653a2031342c0a0a0909"
              "092f2f20537472696e67202d20546f6f6c746970207469746c6520666f6e7420776569676874207374796c650a090909746f6f6c7469705469746c65466f6e745374796c653a2022626f6c64222c0a0a0909092f2f20537472696e67202d20546f6f6c746970207469746c6520666f6e7420636f6c6f75720a090909746f6f6c7469705469746c65466f6e74436f6c6f723a202223666666222c0a0a0909092f2f204e756d626572202d20706978656c207769647468206f662070616464696e672061726f756e6420746f6f6c74697020746578740a090909746f6f6c7469705950616464696e673a20362c0a0a0909092f2f204e756d626572202d20706978656c207769647468206f662070616464696e672061726f756e6420746f6f6c74697020746578740a090909746f6f6c7469705850616464696e673a20362c0a0a0909092f2f204e756d626572202d2053697a65206f6620746865206361726574206f6e2074686520746f6f6c7469700a090909746f6f6c746970436172657453697a653a20382c0a0a0909092f2f204e756d626572202d20506978656c20726164697573206f662074686520746f6f6c74697020626f726465720a090909746f6f6c746970436f726e65725261646975733a20362c0a0a0909092f2f204e756d626572202d20506978656c206f66667365742066726f6d20706f696e74207820"
              "746f20746f6f6c74697020656467650a090909746f6f6c746970584f66667365743a2031302c0a0a0909092f2f20537472696e67202d2054656d706c61746520737472696e6720666f722073696e676c6520746f6f6c746970730a090909746f6f6c74697054656d706c6174653a20223c25696620286c6162656c297b253e3c253d6c6162656c253e3a203c257d253e3c253d2076616c756520253e222c0a0a0909092f2f20537472696e67202d2054656d706c61746520737472696e6720666f722073696e676c6520746f6f6c746970730a0909096d756c7469546f6f6c74697054656d706c6174653a20223c253d2076616c756520253e222c0a0a0909092f2f20537472696e67202d20436f6c6f757220626568696e6420746865206c6567656e6420636f6c6f757220626c6f636b0a0909096d756c7469546f6f6c7469704b65794261636b67726f756e643a202723666666272c0a0a0909092f2f2046756e6374696f6e202d2057696c6c2066697265206f6e20616e696d6174696f6e2070726f6772657373696f6e2e0a0909096f6e416e696d6174696f6e50726f67726573733a2066756e6374696f6e28297b7d2c0a0a0909092f2f2046756e6374696f6e202d2057696c6c2066697265206f6e20616e696d6174696f6e20636f6d706c6574696f6e2e0a0909096f6e416e696d6174696f6e436f6d706c6574653a"
              "2066756e6374696f6e28297b7d0a0a09097d0a097d3b0a0a092f2f43726561746520612064696374696f6e617279206f662063686172742074797065732c20746f20616c6c6f7720666f7220657874656e73696f6e206f66206578697374696e672074797065730a0943686172742e7479706573203d207b7d3b0a0a092f2f476c6f62616c2043686172742068656c70657273206f626a65637420666f72207574696c697479206d6574686f647320616e6420636c61737365730a097661722068656c70657273203d2043686172742e68656c70657273203d207b7d3b0a0a09092f2f2d2d204261736963206a73207574696c697479206d6574686f64730a097661722065616368203d2068656c706572732e65616368203d2066756e6374696f6e286c6f6f7061626c652c63616c6c6261636b2c73656c66297b0a090909766172206164646974696f6e616c41726773203d2041727261792e70726f746f747970652e736c6963652e63616c6c28617267756d656e74732c2033293b0a0909092f2f20436865636b20746f20736565206966206e756c6c206f7220756e646566696e65642066697273746c792e0a090909696620286c6f6f7061626c65297b0a09090909696620286c6f6f7061626c652e6c656e677468203d3d3d202b6c6f6f7061626c652e6c656e677468297b0a090909090976617220693b0a09090909"
              "09666f722028693d303b20693c6c6f6f7061626c652e6c656e6774683b20692b2b297b0a09090909090963616c6c6261636b2e6170706c792873656c662c5b6c6f6f7061626c655b695d2c20695d2e636f6e636174286164646974696f6e616c4172677329293b0a09090909097d0a090909097d0a09090909656c73657b0a0909090909666f722028766172206974656d20696e206c6f6f7061626c65297b0a09090909090963616c6c6261636b2e6170706c792873656c662c5b6c6f6f7061626c655b6974656d5d2c6974656d5d2e636f6e636174286164646974696f6e616c4172677329293b0a09090909097d0a090909097d0a0909097d0a09097d2c0a0909636c6f6e65203d2068656c706572732e636c6f6e65203d2066756e6374696f6e286f626a297b0a090909766172206f626a436c6f6e65203d207b7d3b0a09090965616368286f626a2c66756e6374696f6e2876616c75652c6b6579297b0a09090909696620286f626a2e6861734f776e50726f7065727479286b65792929206f626a436c6f6e655b6b65795d203d2076616c75653b0a0909097d293b0a09090972657475726e206f626a436c6f6e653b0a09097d2c0a0909657874656e64203d2068656c706572732e657874656e64203d2066756e6374696f6e2862617365297b0a090909656163682841727261792e70726f746f747970652e736c6963"
              "652e63616c6c28617267756d656e74732c31292c2066756e6374696f6e28657874656e73696f6e4f626a65637429207b0a090909096561636828657874656e73696f6e4f626a6563742c66756e6374696f6e2876616c75652c6b6579297b0a090909090969662028657874656e73696f6e4f626a6563742e6861734f776e50726f7065727479286b6579292920626173655b6b65795d203d2076616c75653b0a090909097d293b0a0909097d293b0a09090972657475726e20626173653b0a09097d2c0a09096d65726765203d2068656c706572732e6d65726765203d2066756e6374696f6e28626173652c6d6173746572297b0a0909092f2f4d657267652070726f7065727469657320696e206c656674206f626a656374206f76657220746f2061207368616c6c6f7720636c6f6e65206f66206f626a6563742072696768742e0a0909097661722061726773203d2041727261792e70726f746f747970652e736c6963652e63616c6c28617267756d656e74732c30293b0a090909617267732e756e7368696674287b7d293b0a09090972657475726e20657874656e642e6170706c79286e756c6c2c2061726773293b0a09097d2c0a0909696e6465784f66203d2068656c706572732e696e6465784f66203d2066756e6374696f6e286172726179546f5365617263682c206974656d297b0a0909096966202841727261"
              "792e70726f746f747970652e696e6465784f6629207b0a0909090972657475726e206172726179546f5365617263682e696e6465784f66286974656d293b0a0909097d0a090909656c73657b0a09090909666f7220287661722069203d20303b2069203c206172726179546f5365617263682e6c656e6774683b20692b2b29207b0a0909090909696620286172726179546f5365617263685b695d203d3d3d206974656d292072657475726e20693b0a090909097d0a0909090972657475726e202d313b0a0909097d0a09097d2c0a0909696e686572697473203d2068656c706572732e696e686572697473203d2066756e6374696f6e28657874656e73696f6e73297b0a0909092f2f4261736963206a61766173637269707420696e6865726974616e6365206261736564206f6e20746865206d6f64656c206372656174656420696e204261636b626f6e652e6a730a09090976617220706172656e74203d20746869733b0a090909766172204368617274456c656d656e74203d2028657874656e73696f6e7320262620657874656e73696f6e732e6861734f776e50726f70657274792822636f6e7374727563746f72222929203f20657874656e73696f6e732e636f6e7374727563746f72203a2066756e6374696f6e28297b2072657475726e20706172656e742e6170706c7928746869732c20617267756d656e7473"
              "293b207d3b0a0a09090976617220537572726f67617465203d2066756e6374696f6e28297b20746869732e636f6e7374727563746f72203d204368617274456c656d656e743b7d3b0a090909537572726f676174652e70726f746f74797065203d20706172656e742e70726f746f747970653b0a0909094368617274456c656d656e742e70726f746f74797065203d206e657720537572726f6761746528293b0a0a0909094368617274456c656d656e742e657874656e64203d20696e6865726974733b0a0a09090969662028657874656e73696f6e732920657874656e64284368617274456c656d656e742e70726f746f747970652c20657874656e73696f6e73293b0a0a0909094368617274456c656d656e742e5f5f73757065725f5f203d20706172656e742e70726f746f747970653b0a0a09090972657475726e204368617274456c656d656e743b0a09097d2c0a09096e6f6f70203d2068656c706572732e6e6f6f70203d2066756e6374696f6e28297b7d2c0a0909756964203d2068656c706572732e756964203d202866756e6374696f6e28297b0a0909097661722069643d303b0a09090972657475726e2066756e6374696f6e28297b0a0909090972657475726e202263686172742d22202b2069642b2b3b0a0909097d3b0a09097d2928292c0a09097761726e203d2068656c706572732e7761726e203d20"
              "66756e6374696f6e28737472297b0a0909092f2f4d6574686f6420666f72207761726e696e67206f66206572726f72730a0909096966202877696e646f772e636f6e736f6c6520262620747970656f662077696e646f772e636f6e736f6c652e7761726e203d3d202266756e6374696f6e222920636f6e736f6c652e7761726e28737472293b0a09097d2c0a0909616d64203d2068656c706572732e616d64203d2028747970656f6620726f6f742e646566696e65203d3d202766756e6374696f6e2720262620726f6f742e646566696e652e616d64292c0a09092f2f2d2d204d617468206d6574686f64730a090969734e756d626572203d2068656c706572732e69734e756d626572203d2066756e6374696f6e286e297b0a09090972657475726e202169734e614e287061727365466c6f6174286e292920262620697346696e697465286e293b0a09097d2c0a09096d6178203d2068656c706572732e6d6178203d2066756e6374696f6e286172726179297b0a09090972657475726e204d6174682e6d61782e6170706c7928204d6174682c20617272617920293b0a09097d2c0a09096d696e203d2068656c706572732e6d696e203d2066756e6374696f6e286172726179297b0a09090972657475726e204d6174682e6d696e2e6170706c7928204d6174682c20617272617920293b0a09097d2c0a0909636170203d"
              "2068656c706572732e636170203d2066756e6374696f6e2876616c7565546f4361702c6d617856616c75652c6d696e56616c7565297b0a09090969662869734e756d626572286d617856616c75652929207b0a090909096966282076616c7565546f436170203e206d617856616c75652029207b0a090909090972657475726e206d617856616c75653b0a090909097d0a0909097d0a090909656c73652069662869734e756d626572286d696e56616c756529297b0a09090909696620282076616c7565546f436170203c206d696e56616c756520297b0a090909090972657475726e206d696e56616c75653b0a090909097d0a0909097d0a09090972657475726e2076616c7565546f4361703b0a09097d2c0a0909676574446563696d616c506c61636573203d2068656c706572732e676574446563696d616c506c61636573203d2066756e6374696f6e286e756d297b0a090909696620286e756d2531213d3d302026262069734e756d626572286e756d29297b0a0909090972657475726e206e756d2e746f537472696e6728292e73706c697428222e22295b315d2e6c656e6774683b0a0909097d0a090909656c7365207b0a0909090972657475726e20303b0a0909097d0a09097d2c0a0909746f52616469616e73203d2068656c706572732e72616469616e73203d2066756e6374696f6e2864656772656573297b"
              "0a09090972657475726e2064656772656573202a20284d6174682e50492f313830293b0a09097d2c0a09092f2f20476574732074686520616e676c652066726f6d20766572746963616c207570726967687420746f2074686520706f696e742061626f757420612063656e7472652e0a0909676574416e676c6546726f6d506f696e74203d2068656c706572732e676574416e676c6546726f6d506f696e74203d2066756e6374696f6e2863656e747265506f696e742c20616e676c65506f696e74297b0a0909097661722064697374616e636546726f6d5843656e746572203d20616e676c65506f696e742e78202d2063656e747265506f696e742e782c0a0909090964697374616e636546726f6d5943656e746572203d20616e676c65506f696e742e79202d2063656e747265506f696e742e792c0a0909090972616469616c44697374616e636546726f6d43656e746572203d204d6174682e73717274282064697374616e636546726f6d5843656e746572202a2064697374616e636546726f6d5843656e746572202b2064697374616e636546726f6d5943656e746572202a2064697374616e636546726f6d5943656e746572293b0a0a0a09090976617220616e676c65203d204d6174682e5049202a2032202b204d6174682e6174616e322864697374616e636546726f6d5943656e7465722c2064697374616e63"
              "6546726f6d5843656e746572293b0a0a0909092f2f496620746865207365676d656e7420697320696e2074686520746f70206c656674207175616472616e742c207765206e65656420746f2061646420616e6f7468657220726f746174696f6e20746f2074686520616e676c650a0909096966202864697374616e636546726f6d5843656e746572203c20302026262064697374616e636546726f6d5943656e746572203c2030297b0a09090909616e676c65202b3d204d6174682e50492a323b0a0909097d0a0a09090972657475726e207b0a09090909616e676c653a20616e676c652c0a0909090964697374616e63653a2072616469616c44697374616e636546726f6d43656e7465720a0909097d3b0a09097d2c0a0909616c696173506978656c203d2068656c706572732e616c696173506978656c203d2066756e6374696f6e28706978656c5769647468297b0a09090972657475726e2028706978656c576964746820252032203d3d3d203029203f2030203a20302e353b0a09097d2c0a090973706c696e654375727665203d2068656c706572732e73706c696e654375727665203d2066756e6374696f6e284669727374506f696e742c4d6964646c65506f696e742c4166746572506f696e742c74297b0a0909092f2f50726f707320746f20526f62205370656e636572206174207363616c656420696e6e6f"
              "766174696f6e20666f722068697320706f7374206f6e2073706c696e696e67206265747765656e20706f696e74730a0909092f2f687474703a2f2f7363616c6564696e6e6f766174696f6e2e636f6d2f616e616c79746963732f73706c696e65732f61626f757453706c696e65732e68746d6c0a090909766172206430313d4d6174682e73717274284d6174682e706f77284d6964646c65506f696e742e782d4669727374506f696e742e782c32292b4d6174682e706f77284d6964646c65506f696e742e792d4669727374506f696e742e792c3229292c0a090909096431323d4d6174682e73717274284d6174682e706f77284166746572506f696e742e782d4d6964646c65506f696e742e782c32292b4d6174682e706f77284166746572506f696e742e792d4d6964646c65506f696e742e792c3229292c0a0909090966613d742a6430312f286430312b643132292c2f2f207363616c696e6720666163746f7220666f7220747269616e676c652054610a0909090966623d742a6431322f286430312b643132293b0a09090972657475726e207b0a09090909696e6e6572203a207b0a090909090978203a204d6964646c65506f696e742e782d66612a284166746572506f696e742e782d4669727374506f696e742e78292c0a090909090979203a204d6964646c65506f696e742e792d66612a284166746572506f69"
              "6e742e792d4669727374506f696e742e79290a090909097d2c0a090909096f75746572203a207b0a0909090909783a204d6964646c65506f696e742e782b66622a284166746572506f696e742e782d4669727374506f696e742e78292c0a090909090979203a204d6964646c65506f696e742e792b66622a284166746572506f696e742e792d4669727374506f696e742e79290a090909097d0a0909097d3b0a09097d2c0a090963616c63756c6174654f726465724f664d61676e6974756465203d2068656c706572732e63616c63756c6174654f726465724f664d61676e6974756465203d2066756e6374696f6e2876616c297b0a09090972657475726e204d6174682e666c6f6f72284d6174682e6c6f672876616c29202f204d6174682e4c4e3130293b0a09097d2c0a090963616c63756c6174655363616c6552616e6765203d2068656c706572732e63616c63756c6174655363616c6552616e6765203d2066756e6374696f6e2876616c75657341727261792c2064726177696e6753697a652c207465787453697a652c20737461727446726f6d5a65726f2c20696e7465676572734f6e6c79297b0a0a0909092f2f5365742061206d696e696d756d2073746570206f662074776f202d206120706f696e742061742074686520746f70206f66207468652067726170682c20616e64206120706f696e742061742074"
              "686520626173650a090909766172206d696e5374657073203d20322c0a090909096d61785374657073203d204d6174682e666c6f6f722864726177696e6753697a652f287465787453697a65202a20312e3529292c0a09090909736b697046697474696e67203d20286d696e5374657073203e3d206d61785374657073293b0a0a090909766172206d617856616c7565203d206d61782876616c7565734172726179292c0a090909096d696e56616c7565203d206d696e2876616c7565734172726179293b0a0a0909092f2f205765206e65656420736f6d6520646567726565206f662073657065726174696f6e206865726520746f2063616c63756c61746520746865207363616c657320696620616c6c207468652076616c75657320617265207468652073616d650a0909092f2f20416464696e672f6d696e7573696e6720302e352077696c6c206769766520757320612072616e6765206f6620312e0a090909696620286d617856616c7565203d3d3d206d696e56616c7565297b0a090909096d617856616c7565202b3d20302e353b0a090909092f2f20536f20776520646f6e277420656e642075702077697468206120677261706820776974682061206e656761746976652073746172742076616c7565206966207765277665207361696420616c776179732073746172742066726f6d207a65726f0a09090909"
              "696620286d696e56616c7565203e3d20302e352026262021737461727446726f6d5a65726f297b0a09090909096d696e56616c7565202d3d20302e353b0a090909097d0a09090909656c73657b0a09090909092f2f204d616b6520757020612077686f6c65206e756d6265722061626f7665207468652076616c7565730a09090909096d617856616c7565202b3d20302e353b0a090909097d0a0909097d0a0a0909097661720976616c756552616e6765203d204d6174682e616273286d617856616c7565202d206d696e56616c7565292c0a0909090972616e67654f726465724f664d61676e6974756465203d2063616c63756c6174654f726465724f664d61676e69747564652876616c756552616e6765292c0a0909090967726170684d6178203d204d6174682e6365696c286d617856616c7565202f202831202a204d6174682e706f772831302c2072616e67654f726465724f664d61676e6974756465292929202a204d6174682e706f772831302c2072616e67654f726465724f664d61676e6974756465292c0a0909090967726170684d696e203d2028737461727446726f6d5a65726f29203f2030203a204d6174682e666c6f6f72286d696e56616c7565202f202831202a204d6174682e706f772831302c2072616e67654f726465724f664d61676e6974756465292929202a204d6174682e706f772831302c"
              "2072616e67654f726465724f664d61676e6974756465292c0a09090909677261706852616e6765203d2067726170684d6178202d2067726170684d696e2c0a090909097374657056616c7565203d204d6174682e706f772831302c2072616e67654f726465724f664d61676e6974756465292c0a090909096e756d6265724f665374657073203d204d6174682e726f756e6428677261706852616e6765202f207374657056616c7565293b0a0a0909092f2f49662077652068617665206d6f7265207370616365206f6e20746865206772617068207765276c6c2075736520697420746f2067697665206d6f726520646566696e6974696f6e20746f2074686520646174610a0909097768696c6528286e756d6265724f665374657073203e206d61785374657073207c7c20286e756d6265724f665374657073202a203229203c206d61785374657073292026262021736b697046697474696e6729207b0a090909096966286e756d6265724f665374657073203e206d61785374657073297b0a09090909097374657056616c7565202a3d323b0a09090909096e756d6265724f665374657073203d204d6174682e726f756e6428677261706852616e67652f7374657056616c7565293b0a09090909092f2f20446f6e27742065766572206465616c2077697468206120646563696d616c206e756d626572206f6620737465"
              "7073202d2063616e63656c2066697474696e6720616e64206a7573742075736520746865206d696e696d756d206e756d626572206f662073746570732e0a0909090909696620286e756d6265724f6653746570732025203120213d3d2030297b0a090909090909736b697046697474696e67203d20747275653b0a09090909097d0a090909097d0a090909092f2f57652063616e2066697420696e20646f75626c652074686520616d6f756e74206f66207363616c6520706f696e7473206f6e20746865207363616c650a09090909656c73657b0a09090909092f2f4966207573657220686173206465636c6172656420696e7473206f6e6c792c20616e642074686520737465702076616c75652069736e2774206120646563696d616c0a090909090969662028696e7465676572734f6e6c792026262072616e67654f726465724f664d61676e6974756465203e3d2030297b0a0909090909092f2f496620746865207573657220686173207361696420696e746567657273206f6e6c792c207765206e65656420746f20636865636b2074686174206d616b696e6720746865207363616c65206d6f7265206772616e756c617220776f756c646e2774206d616b65206974206120666c6f61740a0909090909096966287374657056616c75652f3220252031203d3d3d2030297b0a090909090909097374657056616c7565"
              "202f3d323b0a090909090909096e756d6265724f665374657073203d204d6174682e726f756e6428677261706852616e67652f7374657056616c7565293b0a0909090909097d0a0909090909092f2f496620697420776f756c64206d616b65206974206120666c6f617420627265616b206f7574206f6620746865206c6f6f700a090909090909656c73657b0a09090909090909627265616b3b0a0909090909097d0a09090909097d0a09090909092f2f496620746865207363616c6520646f65736e2774206861766520746f20626520616e20696e742c206d616b6520746865207363616c65206d6f7265206772616e756c617220616e797761792e0a0909090909656c73657b0a0909090909097374657056616c7565202f3d323b0a0909090909096e756d6265724f665374657073203d204d6174682e726f756e6428677261706852616e67652f7374657056616c7565293b0a09090909097d0a0a090909097d0a0909097d0a0a09090969662028736b697046697474696e67297b0a090909096e756d6265724f665374657073203d206d696e53746570733b0a090909097374657056616c7565203d20677261706852616e6765202f206e756d6265724f6653746570733b0a0909097d0a0a09090972657475726e207b0a090909097374657073203a206e756d6265724f6653746570732c0a09090909737465705661"
              "6c7565203a207374657056616c75652c0a090909096d696e203a2067726170684d696e2c0a090909096d6178093a2067726170684d696e202b20286e756d6265724f665374657073202a207374657056616c7565290a0909097d3b0a0a09097d2c0a09092f2a206a7368696e742069676e6f72653a7374617274202a2f0a09092f2f20426c6f7773207570206a7368696e74206572726f7273206261736564206f6e20746865206e65772046756e6374696f6e20636f6e7374727563746f720a09092f2f54656d706c6174696e67206d6574686f64730a09092f2f4a617661736372697074206d6963726f2074656d706c6174696e67206279204a6f686e205265736967202d20736f7572636520617420687474703a2f2f656a6f686e2e6f72672f626c6f672f6a6176617363726970742d6d6963726f2d74656d706c6174696e672f0a090974656d706c617465203d2068656c706572732e74656d706c617465203d2066756e6374696f6e2874656d706c617465537472696e672c2076616c7565734f626a656374297b0a090909766172206361636865203d207b7d3b0a09090966756e6374696f6e20746d706c287374722c2064617461297b0a090909092f2f20466967757265206f75742069662077652772652067657474696e6720612074656d706c6174652c206f72206966207765206e65656420746f0a09090909"
              "2f2f206c6f6164207468652074656d706c617465202d20616e64206265207375726520746f2063616368652074686520726573756c742e0a0909090976617220666e203d20212f5c572f2e746573742873747229203f0a0909090963616368655b7374725d203d2063616368655b7374725d203a0a0a090909092f2f2047656e65726174652061207265757361626c652066756e6374696f6e20746861742077696c6c20736572766520617320612074656d706c6174650a090909092f2f2067656e657261746f722028616e642077686963682077696c6c20626520636163686564292e0a090909096e65772046756e6374696f6e28226f626a222c0a09090909092276617220703d5b5d2c7072696e743d66756e6374696f6e28297b702e707573682e6170706c7928702c617267756d656e7473293b7d3b22202b0a0a09090909092f2f20496e74726f64756365207468652064617461206173206c6f63616c207661726961626c6573207573696e67207769746828297b7d0a09090909092277697468286f626a297b702e70757368282722202b0a0a09090909092f2f20436f6e76657274207468652074656d706c61746520696e746f2070757265204a6176615363726970740a09090909097374720a0909090909092e7265706c616365282f5b5c725c745c6e5d2f672c20222022290a0909090909092e73706c6974"
              "28223c2522292e6a6f696e28225c7422290a0909090909092e7265706c616365282f28285e7c253e295b5e5c745d2a29272f672c202224315c7222290a0909090909092e7265706c616365282f5c743d282e2a3f29253e2f672c2022272c24312c2722290a0909090909092e73706c697428225c7422292e6a6f696e282227293b22290a0909090909092e73706c69742822253e22292e6a6f696e2822702e70757368282722290a0909090909092e73706c697428225c7222292e6a6f696e28225c5c272229202b0a09090909092227293b7d72657475726e20702e6a6f696e282727293b220a09090909293b0a0a090909092f2f2050726f7669646520736f6d65206261736963206375727279696e6720746f2074686520757365720a0909090972657475726e2064617461203f20666e2820646174612029203a20666e3b0a0909097d0a09090972657475726e20746d706c2874656d706c617465537472696e672c76616c7565734f626a656374293b0a09097d2c0a09092f2a206a7368696e742069676e6f72653a656e64202a2f0a090967656e65726174654c6162656c73203d2068656c706572732e67656e65726174654c6162656c73203d2066756e6374696f6e2874656d706c617465537472696e672c6e756d6265724f6653746570732c67726170684d696e2c7374657056616c7565297b0a09090976617220"
              "6c6162656c734172726179203d206e6577204172726179286e756d6265724f665374657073293b0a090909696620286c6162656c54656d706c617465537472696e67297b0a0909090965616368286c6162656c7341727261792c66756e6374696f6e2876616c2c696e646578297b0a09090909096c6162656c7341727261795b696e6465785d203d2074656d706c6174652874656d706c617465537472696e672c7b76616c75653a202867726170684d696e202b20287374657056616c75652a28696e6465782b312929297d293b0a090909097d293b0a0909097d0a09090972657475726e206c6162656c7341727261793b0a09097d2c0a09092f2f2d2d416e696d6174696f6e206d6574686f64730a09092f2f456173696e672066756e6374696f6e7320616461707465642066726f6d20526f626572742050656e6e6572277320656173696e67206571756174696f6e730a09092f2f687474703a2f2f7777772e726f6265727470656e6e65722e636f6d2f656173696e672f0a0909656173696e6745666665637473203d2068656c706572732e656173696e6745666665637473203d207b0a0909096c696e6561723a2066756e6374696f6e20287429207b0a0909090972657475726e20743b0a0909097d2c0a09090965617365496e517561643a2066756e6374696f6e20287429207b0a0909090972657475726e207420"
              "2a20743b0a0909097d2c0a090909656173654f7574517561643a2066756e6374696f6e20287429207b0a0909090972657475726e202d31202a2074202a202874202d2032293b0a0909097d2c0a09090965617365496e4f7574517561643a2066756e6374696f6e20287429207b0a09090909696620282874202f3d2031202f203229203c2031292072657475726e2031202f2032202a2074202a20743b0a0909090972657475726e202d31202f2032202a2028282d2d7429202a202874202d203229202d2031293b0a0909097d2c0a09090965617365496e43756269633a2066756e6374696f6e20287429207b0a0909090972657475726e2074202a2074202a20743b0a0909097d2c0a090909656173654f757443756269633a2066756e6374696f6e20287429207b0a0909090972657475726e2031202a20282874203d2074202f2031202d203129202a2074202a2074202b2031293b0a0909097d2c0a09090965617365496e4f757443756269633a2066756e6374696f6e20287429207b0a09090909696620282874202f3d2031202f203229203c2031292072657475726e2031202f2032202a2074202a2074202a20743b0a0909090972657475726e2031202f2032202a20282874202d3d203229202a2074202a2074202b2032293b0a0909097d2c0a09090965617365496e51756172743a2066756e6374696f6e202874"
              "29207b0a0909090972657475726e2074202a2074202a2074202a20743b0a0909097d2c0a090909656173654f757451756172743a2066756e6374696f6e20287429207b0a0909090972657475726e202d31202a20282874203d2074202f2031202d203129202a2074202a2074202a2074202d2031293b0a0909097d2c0a09090965617365496e4f757451756172743a2066756e6374696f6e20287429207b0a09090909696620282874202f3d2031202f203229203c2031292072657475726e2031202f2032202a2074202a2074202a2074202a20743b0a0909090972657475726e202d31202f2032202a20282874202d3d203229202a2074202a2074202a2074202d2032293b0a0909097d2c0a09090965617365496e5175696e743a2066756e6374696f6e20287429207b0a0909090972657475726e2031202a202874202f3d203129202a2074202a2074202a2074202a20743b0a0909097d2c0a090909656173654f75745175696e743a2066756e6374696f6e20287429207b0a0909090972657475726e2031202a20282874203d2074202f2031202d203129202a2074202a2074202a2074202a2074202b2031293b0a0909097d2c0a09090965617365496e4f75745175696e743a2066756e6374696f6e20287429207b0a09090909696620282874202f3d2031202f203229203c2031292072657475726e2031202f203220"
              "2a2074202a2074202a2074202a2074202a20743b0a0909090972657475726e2031202f2032202a20282874202d3d203229202a2074202a2074202a2074202a2074202b2032293b0a0909097d2c0a09090965617365496e53696e653a2066756e6374696f6e20287429207b0a0909090972657475726e202d31202a204d6174682e636f732874202f2031202a20284d6174682e5049202f20322929202b20313b0a0909097d2c0a090909656173654f757453696e653a2066756e6374696f6e20287429207b0a0909090972657475726e2031202a204d6174682e73696e2874202f2031202a20284d6174682e5049202f203229293b0a0909097d2c0a09090965617365496e4f757453696e653a2066756e6374696f6e20287429207b0a0909090972657475726e202d31202f2032202a20284d6174682e636f73284d6174682e5049202a2074202f203129202d2031293b0a0909097d2c0a09090965617365496e4578706f3a2066756e6374696f6e20287429207b0a0909090972657475726e202874203d3d3d203029203f2031203a2031202a204d6174682e706f7728322c203130202a202874202f2031202d203129293b0a0909097d2c0a090909656173654f75744578706f3a2066756e6374696f6e20287429207b0a0909090972657475726e202874203d3d3d203129203f2031203a2031202a20282d4d6174682e70"
              "6f7728322c202d3130202a2074202f203129202b2031293b0a0909097d2c0a09090965617365496e4f75744578706f3a2066756e6374696f6e20287429207b0a090909096966202874203d3d3d2030292072657475726e20303b0a090909096966202874203d3d3d2031292072657475726e20313b0a09090909696620282874202f3d2031202f203229203c2031292072657475726e2031202f2032202a204d6174682e706f7728322c203130202a202874202d203129293b0a0909090972657475726e2031202f2032202a20282d4d6174682e706f7728322c202d3130202a202d2d7429202b2032293b0a0909097d2c0a09090965617365496e436972633a2066756e6374696f6e20287429207b0a090909096966202874203e3d2031292072657475726e20743b0a0909090972657475726e202d31202a20284d6174682e737172742831202d202874202f3d203129202a207429202d2031293b0a0909097d2c0a090909656173654f7574436972633a2066756e6374696f6e20287429207b0a0909090972657475726e2031202a204d6174682e737172742831202d202874203d2074202f2031202d203129202a2074293b0a0909097d2c0a09090965617365496e4f7574436972633a2066756e6374696f6e20287429207b0a09090909696620282874202f3d2031202f203229203c2031292072657475726e202d3120"
              "2f2032202a20284d6174682e737172742831202d2074202a207429202d2031293b0a0909090972657475726e2031202f2032202a20284d6174682e737172742831202d202874202d3d203229202a207429202b2031293b0a0909097d2c0a09090965617365496e456c61737469633a2066756e6374696f6e20287429207b0a090909097661722073203d20312e37303135383b0a090909097661722070203d20303b0a090909097661722061203d20313b0a090909096966202874203d3d3d2030292072657475726e20303b0a09090909696620282874202f3d203129203d3d2031292072657475726e20313b0a09090909696620282170292070203d2031202a20302e333b0a090909096966202861203c204d6174682e61627328312929207b0a090909090961203d20313b0a090909090973203d2070202f20343b0a090909097d20656c73652073203d2070202f202832202a204d6174682e504929202a204d6174682e6173696e2831202f2061293b0a0909090972657475726e202d2861202a204d6174682e706f7728322c203130202a202874202d3d20312929202a204d6174682e73696e282874202a2031202d207329202a202832202a204d6174682e504929202f207029293b0a0909097d2c0a090909656173654f7574456c61737469633a2066756e6374696f6e20287429207b0a090909097661722073203d"
              "20312e37303135383b0a090909097661722070203d20303b0a090909097661722061203d20313b0a090909096966202874203d3d3d2030292072657475726e20303b0a09090909696620282874202f3d203129203d3d2031292072657475726e20313b0a09090909696620282170292070203d2031202a20302e333b0a090909096966202861203c204d6174682e61627328312929207b0a090909090961203d20313b0a090909090973203d2070202f20343b0a090909097d20656c73652073203d2070202f202832202a204d6174682e504929202a204d6174682e6173696e2831202f2061293b0a0909090972657475726e2061202a204d6174682e706f7728322c202d3130202a207429202a204d6174682e73696e282874202a2031202d207329202a202832202a204d6174682e504929202f207029202b20313b0a0909097d2c0a09090965617365496e4f7574456c61737469633a2066756e6374696f6e20287429207b0a090909097661722073203d20312e37303135383b0a090909097661722070203d20303b0a090909097661722061203d20313b0a090909096966202874203d3d3d2030292072657475726e20303b0a09090909696620282874202f3d2031202f203229203d3d2032292072657475726e20313b0a09090909696620282170292070203d2031202a2028302e33202a20312e35293b0a09090909"
              "6966202861203c204d6174682e61627328312929207b0a090909090961203d20313b0a090909090973203d2070202f20343b0a090909097d20656c73652073203d2070202f202832202a204d6174682e504929202a204d6174682e6173696e2831202f2061293b0a090909096966202874203c2031292072657475726e202d302e35202a202861202a204d6174682e706f7728322c203130202a202874202d3d20312929202a204d6174682e73696e282874202a2031202d207329202a202832202a204d6174682e504929202f207029293b0a0909090972657475726e2061202a204d6174682e706f7728322c202d3130202a202874202d3d20312929202a204d6174682e73696e282874202a2031202d207329202a202832202a204d6174682e504929202f207029202a20302e35202b20313b0a0909097d2c0a09090965617365496e4261636b3a2066756e6374696f6e20287429207b0a090909097661722073203d20312e37303135383b0a0909090972657475726e2031202a202874202f3d203129202a2074202a20282873202b203129202a2074202d2073293b0a0909097d2c0a090909656173654f75744261636b3a2066756e6374696f6e20287429207b0a090909097661722073203d20312e37303135383b0a0909090972657475726e2031202a20282874203d2074202f2031202d203129202a2074202a2028"
              "2873202b203129202a2074202b207329202b2031293b0a0909097d2c0a09090965617365496e4f75744261636b3a2066756e6374696f6e20287429207b0a090909097661722073203d20312e37303135383b0a09090909696620282874202f3d2031202f203229203c2031292072657475726e2031202f2032202a202874202a2074202a2028282873202a3d2028312e3532352929202b203129202a2074202d207329293b0a0909090972657475726e2031202f2032202a20282874202d3d203229202a2074202a2028282873202a3d2028312e3532352929202b203129202a2074202b207329202b2032293b0a0909097d2c0a09090965617365496e426f756e63653a2066756e6374696f6e20287429207b0a0909090972657475726e2031202d20656173696e67456666656374732e656173654f7574426f756e63652831202d2074293b0a0909097d2c0a090909656173654f7574426f756e63653a2066756e6374696f6e20287429207b0a09090909696620282874202f3d203129203c202831202f20322e37352929207b0a090909090972657475726e2031202a2028372e35363235202a2074202a2074293b0a090909097d20656c7365206966202874203c202832202f20322e37352929207b0a090909090972657475726e2031202a2028372e35363235202a202874202d3d2028312e35202f20322e3735292920"
              "2a2074202b20302e3735293b0a090909097d20656c7365206966202874203c2028322e35202f20322e37352929207b0a090909090972657475726e2031202a2028372e35363235202a202874202d3d2028322e3235202f20322e37352929202a2074202b20302e39333735293b0a090909097d20656c7365207b0a090909090972657475726e2031202a2028372e35363235202a202874202d3d2028322e363235202f20322e37352929202a2074202b20302e393834333735293b0a090909097d0a0909097d2c0a09090965617365496e4f7574426f756e63653a2066756e6374696f6e20287429207b0a090909096966202874203c2031202f2032292072657475726e20656173696e67456666656374732e65617365496e426f756e63652874202a203229202a20302e353b0a0909090972657475726e20656173696e67456666656374732e656173654f7574426f756e63652874202a2032202d203129202a20302e35202b2031202a20302e353b0a0909097d0a09097d2c0a09092f2f5265717565737420616e696d6174696f6e20706f6c7966696c6c202d20687474703a2f2f7777772e7061756c69726973682e636f6d2f323031312f72657175657374616e696d6174696f6e6672616d652d666f722d736d6172742d616e696d6174696e672f0a090972657175657374416e696d4672616d65203d2068656c706572"
              "732e72657175657374416e696d4672616d65203d202866756e6374696f6e28297b0a09090972657475726e2077696e646f772e72657175657374416e696d6174696f6e4672616d65207c7c0a0909090977696e646f772e7765626b697452657175657374416e696d6174696f6e4672616d65207c7c0a0909090977696e646f772e6d6f7a52657175657374416e696d6174696f6e4672616d65207c7c0a0909090977696e646f772e6f52657175657374416e696d6174696f6e4672616d65207c7c0a0909090977696e646f772e6d7352657175657374416e696d6174696f6e4672616d65207c7c0a0909090966756e6374696f6e2863616c6c6261636b29207b0a090909090972657475726e2077696e646f772e73657454696d656f75742863616c6c6261636b2c2031303030202f203630293b0a090909097d3b0a09097d2928292c0a090963616e63656c416e696d4672616d65203d2068656c706572732e63616e63656c416e696d4672616d65203d202866756e6374696f6e28297b0a09090972657475726e2077696e646f772e63616e63656c416e696d6174696f6e4672616d65207c7c0a0909090977696e646f772e7765626b697443616e63656c416e696d6174696f6e4672616d65207c7c0a0909090977696e646f772e6d6f7a43616e63656c416e696d6174696f6e4672616d65207c7c0a0909090977696e646f"
              "772e6f43616e63656c416e696d6174696f6e4672616d65207c7c0a0909090977696e646f772e6d7343616e63656c416e696d6174696f6e4672616d65207c7c0a0909090966756e6374696f6e2863616c6c6261636b29207b0a090909090972657475726e2077696e646f772e636c65617254696d656f75742863616c6c6261636b2c2031303030202f203630293b0a090909097d3b0a09097d2928292c0a0909616e696d6174696f6e4c6f6f70203d2068656c706572732e616e696d6174696f6e4c6f6f70203d2066756e6374696f6e2863616c6c6261636b2c746f74616c53746570732c656173696e67537472696e672c6f6e50726f67726573732c6f6e436f6d706c6574652c6368617274496e7374616e6365297b0a0a0909097661722063757272656e7453746570203d20302c0a09090909656173696e6746756e6374696f6e203d20656173696e67456666656374735b656173696e67537472696e675d207c7c20656173696e67456666656374732e6c696e6561723b0a0a09090976617220616e696d6174696f6e4672616d65203d2066756e6374696f6e28297b0a0909090963757272656e74537465702b2b3b0a090909097661722073746570446563696d616c203d2063757272656e74537465702f746f74616c53746570733b0a090909097661722065617365446563696d616c203d20656173696e6746756e"
              "6374696f6e2873746570446563696d616c293b0a0a0909090963616c6c6261636b2e63616c6c286368617274496e7374616e63652c65617365446563696d616c2c73746570446563696d616c2c2063757272656e7453746570293b0a090909096f6e50726f67726573732e63616c6c286368617274496e7374616e63652c65617365446563696d616c2c73746570446563696d616c293b0a090909096966202863757272656e7453746570203c20746f74616c5374657073297b0a09090909096368617274496e7374616e63652e616e696d6174696f6e4672616d65203d2072657175657374416e696d4672616d6528616e696d6174696f6e4672616d65293b0a090909097d20656c73657b0a09090909096f6e436f6d706c6574652e6170706c79286368617274496e7374616e6365293b0a090909097d0a0909097d3b0a09090972657175657374416e696d4672616d6528616e696d6174696f6e4672616d65293b0a09097d2c0a09092f2f2d2d20444f4d206d6574686f64730a090967657452656c6174697665506f736974696f6e203d2068656c706572732e67657452656c6174697665506f736974696f6e203d2066756e6374696f6e28657674297b0a090909766172206d6f757365582c206d6f757365593b0a0909097661722065203d206576742e6f726967696e616c4576656e74207c7c206576742c0a090909"
              "0963616e766173203d206576742e63757272656e74546172676574207c7c206576742e737263456c656d656e742c0a09090909626f756e64696e6752656374203d2063616e7661732e676574426f756e64696e67436c69656e745265637428293b0a0a09090969662028652e746f7563686573297b0a090909096d6f75736558203d20652e746f75636865735b305d2e636c69656e7458202d20626f756e64696e67526563742e6c6566743b0a090909096d6f75736559203d20652e746f75636865735b305d2e636c69656e7459202d20626f756e64696e67526563742e746f703b0a0a0909097d0a090909656c73657b0a090909096d6f75736558203d20652e636c69656e7458202d20626f756e64696e67526563742e6c6566743b0a090909096d6f75736559203d20652e636c69656e7459202d20626f756e64696e67526563742e746f703b0a0909097d0a0a09090972657475726e207b0a0909090978203a206d6f757365582c0a0909090979203a206d6f757365590a0909097d3b0a0a09097d2c0a09096164644576656e74203d2068656c706572732e6164644576656e74203d2066756e6374696f6e286e6f64652c6576656e74547970652c6d6574686f64297b0a090909696620286e6f64652e6164644576656e744c697374656e6572297b0a090909096e6f64652e6164644576656e744c697374656e657228"
              "6576656e74547970652c6d6574686f64293b0a0909097d20656c736520696620286e6f64652e6174746163684576656e74297b0a090909096e6f64652e6174746163684576656e7428226f6e222b6576656e74547970652c206d6574686f64293b0a0909097d20656c7365207b0a090909096e6f64655b226f6e222b6576656e74547970655d203d206d6574686f643b0a0909097d0a09097d2c0a090972656d6f76654576656e74203d2068656c706572732e72656d6f76654576656e74203d2066756e6374696f6e286e6f64652c206576656e74547970652c2068616e646c6572297b0a090909696620286e6f64652e72656d6f76654576656e744c697374656e6572297b0a090909096e6f64652e72656d6f76654576656e744c697374656e6572286576656e74547970652c2068616e646c65722c2066616c7365293b0a0909097d20656c736520696620286e6f64652e6465746163684576656e74297b0a090909096e6f64652e6465746163684576656e7428226f6e222b6576656e74547970652c68616e646c6572293b0a0909097d20656c73657b0a090909096e6f64655b226f6e22202b206576656e74547970655d203d206e6f6f703b0a0909097d0a09097d2c0a090962696e644576656e7473203d2068656c706572732e62696e644576656e7473203d2066756e6374696f6e286368617274496e7374616e63"
              "652c2061727261794f664576656e74732c2068616e646c6572297b0a0909092f2f2043726561746520746865206576656e7473206f626a6563742069662069742773206e6f7420616c72656164792070726573656e740a09090969662028216368617274496e7374616e63652e6576656e747329206368617274496e7374616e63652e6576656e7473203d207b7d3b0a0a090909656163682861727261794f664576656e74732c66756e6374696f6e286576656e744e616d65297b0a090909096368617274496e7374616e63652e6576656e74735b6576656e744e616d655d203d2066756e6374696f6e28297b0a090909090968616e646c65722e6170706c79286368617274496e7374616e63652c20617267756d656e7473293b0a090909097d3b0a090909096164644576656e74286368617274496e7374616e63652e63686172742e63616e7661732c6576656e744e616d652c6368617274496e7374616e63652e6576656e74735b6576656e744e616d655d293b0a0909097d293b0a09097d2c0a0909756e62696e644576656e7473203d2068656c706572732e756e62696e644576656e7473203d2066756e6374696f6e20286368617274496e7374616e63652c2061727261794f664576656e747329207b0a090909656163682861727261794f664576656e74732c2066756e6374696f6e2868616e646c65722c657665"
              "6e744e616d65297b0a0909090972656d6f76654576656e74286368617274496e7374616e63652e63686172742e63616e7661732c206576656e744e616d652c2068616e646c6572293b0a0909097d293b0a09097d2c0a09096765744d6178696d756d53697a65203d2068656c706572732e6765744d6178696d756d53697a65203d2066756e6374696f6e28646f6d4e6f6465297b0a09090976617220636f6e7461696e6572203d20646f6d4e6f64652e706172656e744e6f64653b0a0909092f2f20544f444f203d20636865636b2063726f73732062726f77736572207374756666207769746820746869732e0a09090972657475726e20636f6e7461696e65722e636c69656e7457696474683b0a09097d2c0a0909726574696e615363616c65203d2068656c706572732e726574696e615363616c65203d2066756e6374696f6e286368617274297b0a09090976617220637478203d2063686172742e6374782c0a090909097769647468203d2063686172742e63616e7661732e77696474682c0a09090909686569676874203d2063686172742e63616e7661732e6865696768743b0a0909092f2f636f6e736f6c652e6c6f67287769647468202b202220782022202b20686569676874293b0a0909096966202877696e646f772e646576696365506978656c526174696f29207b0a090909096374782e63616e7661732e"
              "7374796c652e7769647468203d207769647468202b20227078223b0a090909096374782e63616e7661732e7374796c652e686569676874203d20686569676874202b20227078223b0a090909096374782e63616e7661732e686569676874203d20686569676874202a2077696e646f772e646576696365506978656c526174696f3b0a090909096374782e63616e7661732e7769647468203d207769647468202a2077696e646f772e646576696365506978656c526174696f3b0a090909096374782e7363616c652877696e646f772e646576696365506978656c526174696f2c2077696e646f772e646576696365506978656c526174696f293b0a0909097d0a09097d2c0a09092f2f2d2d2043616e766173206d6574686f64730a0909636c656172203d2068656c706572732e636c656172203d2066756e6374696f6e286368617274297b0a09090963686172742e6374782e636c6561725265637428302c302c63686172742e77696474682c63686172742e686569676874293b0a09097d2c0a0909666f6e74537472696e67203d2068656c706572732e666f6e74537472696e67203d2066756e6374696f6e28706978656c53697a652c666f6e745374796c652c666f6e7446616d696c79297b0a09090972657475726e20666f6e745374796c65202b20222022202b20706978656c53697a652b2270782022202b20666f"
              "6e7446616d696c793b0a09097d2c0a09096c6f6e6765737454657874203d2068656c706572732e6c6f6e6765737454657874203d2066756e6374696f6e286374782c666f6e742c61727261794f66537472696e6773297b0a0909096374782e666f6e74203d20666f6e743b0a090909766172206c6f6e67657374203d20303b0a090909656163682861727261794f66537472696e67732c66756e6374696f6e28737472696e67297b0a0909090976617220746578745769647468203d206374782e6d6561737572655465787428737472696e67292e77696474683b0a090909096c6f6e67657374203d2028746578745769647468203e206c6f6e6765737429203f20746578745769647468203a206c6f6e676573743b0a0909097d293b0a09090972657475726e206c6f6e676573743b0a09097d2c0a090964726177526f756e64656452656374616e676c65203d2068656c706572732e64726177526f756e64656452656374616e676c65203d2066756e6374696f6e286374782c782c792c77696474682c6865696768742c726164697573297b0a0909096374782e626567696e5061746828293b0a0909096374782e6d6f7665546f2878202b207261646975732c2079293b0a0909096374782e6c696e65546f2878202b207769647468202d207261646975732c2079293b0a0909096374782e717561647261746963437572"
              "7665546f2878202b2077696474682c20792c2078202b2077696474682c2079202b20726164697573293b0a0909096374782e6c696e65546f2878202b2077696474682c2079202b20686569676874202d20726164697573293b0a0909096374782e7175616472617469634375727665546f2878202b2077696474682c2079202b206865696768742c2078202b207769647468202d207261646975732c2079202b20686569676874293b0a0909096374782e6c696e65546f2878202b207261646975732c2079202b20686569676874293b0a0909096374782e7175616472617469634375727665546f28782c2079202b206865696768742c20782c2079202b20686569676874202d20726164697573293b0a0909096374782e6c696e65546f28782c2079202b20726164697573293b0a0909096374782e7175616472617469634375727665546f28782c20792c2078202b207261646975732c2079293b0a0909096374782e636c6f73655061746828293b0a09097d3b0a0a0a092f2f53746f72652061207265666572656e636520746f206561636820696e7374616e6365202d20616c6c6f77696e6720757320746f20676c6f62616c6c7920726573697a6520636861727420696e7374616e636573206f6e2077696e646f7720726573697a652e0a092f2f44657374726f79206d6574686f64206f6e2074686520636861727420"
              "77696c6c2072656d6f76652074686520696e7374616e6365206f66207468652063686172742066726f6d2074686973207265666572656e63652e0a0943686172742e696e7374616e636573203d207b7d3b0a0a0943686172742e54797065203d2066756e6374696f6e28646174612c6f7074696f6e732c6368617274297b0a0909746869732e6f7074696f6e73203d206f7074696f6e733b0a0909746869732e6368617274203d2063686172743b0a0909746869732e6964203d2075696428293b0a09092f2f4164642074686520636861727420696e7374616e636520746f2074686520676c6f62616c206e616d6573706163650a090943686172742e696e7374616e6365735b746869732e69645d203d20746869733b0a0a09092f2f20496e697469616c697a6520697320616c776179732063616c6c6564207768656e2061206368617274207479706520697320637265617465640a09092f2f2042792064656661756c742069742069732061206e6f206f702c206275742069742073686f756c6420626520657874656e6465640a0909696620286f7074696f6e732e726573706f6e73697665297b0a090909746869732e726573697a6528293b0a09097d0a0909746869732e696e697469616c697a652e63616c6c28746869732c64617461293b0a097d3b0a0a092f2f436f7265206d6574686f64732074686174276c6c"
              "20626520612070617274206f6620657665727920636861727420747970650a09657874656e642843686172742e547970652e70726f746f747970652c7b0a0909696e697469616c697a65203a2066756e6374696f6e28297b72657475726e20746869733b7d2c0a0909636c656172203a2066756e6374696f6e28297b0a090909636c65617228746869732e6368617274293b0a09090972657475726e20746869733b0a09097d2c0a090973746f70203a2066756e6374696f6e28297b0a0909092f2f2053746f707320616e792063757272656e7420616e696d6174696f6e206c6f6f70206f63637572696e670a09090968656c706572732e63616e63656c416e696d4672616d652e63616c6c28726f6f742c20746869732e616e696d6174696f6e4672616d65293b0a09090972657475726e20746869733b0a09097d2c0a0909726573697a65203a2066756e6374696f6e2863616c6c6261636b297b0a090909746869732e73746f7028293b0a0909097661722063616e766173203d20746869732e63686172742e63616e7661732c0a090909096e65775769647468203d206765744d6178696d756d53697a6528746869732e63686172742e63616e766173292c0a090909096e6577486569676874203d206e65775769647468202f20746869732e63686172742e617370656374526174696f3b0a0a09090963616e7661732e"
              "7769647468203d20746869732e63686172742e7769647468203d206e657757696474683b0a09090963616e7661732e686569676874203d2020746869732e63686172742e686569676874203d206e65774865696768743b0a0a090909726574696e615363616c6528746869732e6368617274293b0a0a09090969662028747970656f662063616c6c6261636b203d3d3d202266756e6374696f6e22297b0a0909090963616c6c6261636b2e6170706c7928746869732c2041727261792e70726f746f747970652e736c6963652e63616c6c28617267756d656e74732c203129293b0a0909097d0a09090972657475726e20746869733b0a09097d2c0a09097265666c6f77203a206e6f6f702c0a090972656e646572203a2066756e6374696f6e287265666c6f77297b0a090909696620287265666c6f77297b0a09090909746869732e7265666c6f7728293b0a0909097d0a09090969662028746869732e6f7074696f6e732e616e696d6174696f6e20262620217265666c6f77297b0a0909090968656c706572732e616e696d6174696f6e4c6f6f70280a0909090909746869732e647261772c0a0909090909746869732e6f7074696f6e732e616e696d6174696f6e53746570732c0a0909090909746869732e6f7074696f6e732e616e696d6174696f6e456173696e672c0a0909090909746869732e6f7074696f6e732e6f"
              "6e416e696d6174696f6e50726f67726573732c0a0909090909746869732e6f7074696f6e732e6f6e416e696d6174696f6e436f6d706c6574652c0a0909090909746869730a09090909293b0a0909097d0a090909656c73657b0a09090909746869732e6472617728293b0a09090909746869732e6f7074696f6e732e6f6e416e696d6174696f6e436f6d706c6574652e63616c6c2874686973293b0a0909097d0a09090972657475726e20746869733b0a09097d2c0a090967656e65726174654c6567656e64203a2066756e6374696f6e28297b0a09090972657475726e2074656d706c61746528746869732e6f7074696f6e732e6c6567656e6454656d706c6174652c74686973293b0a09097d2c0a090964657374726f79203a2066756e6374696f6e28297b0a090909746869732e636c65617228293b0a090909756e62696e644576656e747328746869732c20746869732e6576656e7473293b0a09090964656c6574652043686172742e696e7374616e6365735b746869732e69645d3b0a09097d2c0a090973686f77546f6f6c746970203a2066756e6374696f6e284368617274456c656d656e74732c20666f726365526564726177297b0a0909092f2f204f6e6c7920726564726177207468652063686172742069662077652776652061637475616c6c79206368616e676564207768617420776527726520686f76"
              "6572696e67206f6e2e0a09090969662028747970656f6620746869732e616374697665456c656d656e7473203d3d3d2027756e646566696e6564272920746869732e616374697665456c656d656e7473203d205b5d3b0a0a0909097661722069734368616e676564203d202866756e6374696f6e28456c656d656e7473297b0a09090909766172206368616e676564203d2066616c73653b0a0a0909090969662028456c656d656e74732e6c656e67746820213d3d20746869732e616374697665456c656d656e74732e6c656e677468297b0a09090909096368616e676564203d20747275653b0a090909090972657475726e206368616e6765643b0a090909097d0a0a090909096561636828456c656d656e74732c2066756e6374696f6e28656c656d656e742c20696e646578297b0a090909090969662028656c656d656e7420213d3d20746869732e616374697665456c656d656e74735b696e6465785d297b0a0909090909096368616e676564203d20747275653b0a09090909097d0a090909097d2c2074686973293b0a0909090972657475726e206368616e6765643b0a0909097d292e63616c6c28746869732c204368617274456c656d656e7473293b0a0a090909696620282169734368616e6765642026262021666f726365526564726177297b0a0909090972657475726e3b0a0909097d0a090909656c7365"
              "7b0a09090909746869732e616374697665456c656d656e7473203d204368617274456c656d656e74733b0a0909097d0a090909746869732e6472617728293b0a090909696620284368617274456c656d656e74732e6c656e677468203e2030297b0a090909092f2f2049662077652068617665206d756c7469706c652064617461736574732c2073686f772061204d756c7469546f6f6c74697020666f7220616c6c206f6620746865206461746120706f696e7473206174207468617420696e6465780a0909090969662028746869732e646174617365747320262620746869732e64617461736574732e6c656e677468203e203129207b0a0909090909766172206461746141727261792c0a09090909090964617461496e6465783b0a0a0909090909666f7220287661722069203d20746869732e64617461736574732e6c656e677468202d20313b2069203e3d20303b20692d2d29207b0a090909090909646174614172726179203d20746869732e64617461736574735b695d2e706f696e7473207c7c20746869732e64617461736574735b695d2e62617273207c7c20746869732e64617461736574735b695d2e7365676d656e74733b0a09090909090964617461496e646578203d20696e6465784f66286461746141727261792c204368617274456c656d656e74735b305d293b0a09090909090969662028646174"
              "61496e64657820213d3d202d31297b0a09090909090909627265616b3b0a0909090909097d0a09090909097d0a090909090976617220746f6f6c7469704c6162656c73203d205b5d2c0a090909090909746f6f6c746970436f6c6f7273203d205b5d2c0a0909090909096d656469616e506f736974696f6e203d202866756e6374696f6e28696e64657829207b0a0a090909090909092f2f2047657420616c6c2074686520706f696e7473206174207468617420706172746963756c617220696e6465780a0909090909090976617220456c656d656e7473203d205b5d2c0a090909090909090964617461436f6c6c656374696f6e2c0a090909090909090978506f736974696f6e73203d205b5d2c0a090909090909090979506f736974696f6e73203d205b5d2c0a0909090909090909784d61782c0a0909090909090909794d61782c0a0909090909090909784d696e2c0a0909090909090909794d696e3b0a0909090909090968656c706572732e6561636828746869732e64617461736574732c2066756e6374696f6e2864617461736574297b0a090909090909090964617461436f6c6c656374696f6e203d20646174617365742e706f696e7473207c7c20646174617365742e62617273207c7c20646174617365742e7365676d656e74733b0a09090909090909096966202864617461436f6c6c656374696f6e5b64"
              "617461496e6465785d297b0a090909090909090909456c656d656e74732e707573682864617461436f6c6c656374696f6e5b64617461496e6465785d293b0a09090909090909097d0a090909090909097d293b0a0a0909090909090968656c706572732e6561636828456c656d656e74732c2066756e6374696f6e28656c656d656e7429207b0a090909090909090978506f736974696f6e732e7075736828656c656d656e742e78293b0a090909090909090979506f736974696f6e732e7075736828656c656d656e742e79293b0a0a0a09090909090909092f2f496e636c75646520616e7920636f6c6f757220696e666f726d6174696f6e2061626f75742074686520656c656d656e740a0909090909090909746f6f6c7469704c6162656c732e707573682868656c706572732e74656d706c61746528746869732e6f7074696f6e732e6d756c7469546f6f6c74697054656d706c6174652c20656c656d656e7429293b0a0909090909090909746f6f6c746970436f6c6f72732e70757368287b0a09090909090909090966696c6c3a20656c656d656e742e5f73617665642e66696c6c436f6c6f72207c7c20656c656d656e742e66696c6c436f6c6f722c0a0909090909090909097374726f6b653a20656c656d656e742e5f73617665642e7374726f6b65436f6c6f72207c7c20656c656d656e742e7374726f6b65436f"
              "6c6f720a09090909090909097d293b0a0a090909090909097d2c2074686973293b0a0a09090909090909794d696e203d206d696e2879506f736974696f6e73293b0a09090909090909794d6178203d206d61782879506f736974696f6e73293b0a0a09090909090909784d696e203d206d696e2878506f736974696f6e73293b0a09090909090909784d6178203d206d61782878506f736974696f6e73293b0a0a0909090909090972657475726e207b0a0909090909090909783a2028784d696e203e20746869732e63686172742e77696474682f3229203f20784d696e203a20784d61782c0a0909090909090909793a2028794d696e202b20794d6178292f320a090909090909097d3b0a0909090909097d292e63616c6c28746869732c2064617461496e646578293b0a0a09090909096e65772043686172742e4d756c7469546f6f6c746970287b0a090909090909783a206d656469616e506f736974696f6e2e782c0a090909090909793a206d656469616e506f736974696f6e2e792c0a0909090909097850616464696e673a20746869732e6f7074696f6e732e746f6f6c7469705850616464696e672c0a0909090909097950616464696e673a20746869732e6f7074696f6e732e746f6f6c7469705950616464696e672c0a090909090909784f66667365743a20746869732e6f7074696f6e732e746f6f6c746970"
              "584f66667365742c0a09090909090966696c6c436f6c6f723a20746869732e6f7074696f6e732e746f6f6c74697046696c6c436f6c6f722c0a09090909090974657874436f6c6f723a20746869732e6f7074696f6e732e746f6f6c746970466f6e74436f6c6f722c0a090909090909666f6e7446616d696c793a20746869732e6f7074696f6e732e746f6f6c746970466f6e7446616d696c792c0a090909090909666f6e745374796c653a20746869732e6f7074696f6e732e746f6f6c746970466f6e745374796c652c0a090909090909666f6e7453697a653a20746869732e6f7074696f6e732e746f6f6c746970466f6e7453697a652c0a0909090909097469746c6554657874436f6c6f723a20746869732e6f7074696f6e732e746f6f6c7469705469746c65466f6e74436f6c6f722c0a0909090909097469746c65466f6e7446616d696c793a20746869732e6f7074696f6e732e746f6f6c7469705469746c65466f6e7446616d696c792c0a0909090909097469746c65466f6e745374796c653a20746869732e6f7074696f6e732e746f6f6c7469705469746c65466f6e745374796c652c0a0909090909097469746c65466f6e7453697a653a20746869732e6f7074696f6e732e746f6f6c7469705469746c65466f6e7453697a652c0a090909090909636f726e65725261646975733a20746869732e6f7074696f6e"
              "732e746f6f6c746970436f726e65725261646975732c0a0909090909096c6162656c733a20746f6f6c7469704c6162656c732c0a0909090909096c6567656e64436f6c6f72733a20746f6f6c746970436f6c6f72732c0a0909090909096c6567656e64436f6c6f724261636b67726f756e64203a20746869732e6f7074696f6e732e6d756c7469546f6f6c7469704b65794261636b67726f756e642c0a0909090909097469746c653a204368617274456c656d656e74735b305d2e6c6162656c2c0a09090909090963686172743a20746869732e63686172742c0a0909090909096374783a20746869732e63686172742e6374780a09090909097d292e6472617728293b0a0a090909097d20656c7365207b0a090909090965616368284368617274456c656d656e74732c2066756e6374696f6e28456c656d656e7429207b0a09090909090976617220746f6f6c746970506f736974696f6e203d20456c656d656e742e746f6f6c746970506f736974696f6e28293b0a0909090909096e65772043686172742e546f6f6c746970287b0a09090909090909783a204d6174682e726f756e6428746f6f6c746970506f736974696f6e2e78292c0a09090909090909793a204d6174682e726f756e6428746f6f6c746970506f736974696f6e2e79292c0a090909090909097850616464696e673a20746869732e6f7074696f6e73"
              "2e746f6f6c7469705850616464696e672c0a090909090909097950616464696e673a20746869732e6f7074696f6e732e746f6f6c7469705950616464696e672c0a0909090909090966696c6c436f6c6f723a20746869732e6f7074696f6e732e746f6f6c74697046696c6c436f6c6f722c0a0909090909090974657874436f6c6f723a20746869732e6f7074696f6e732e746f6f6c746970466f6e74436f6c6f722c0a09090909090909666f6e7446616d696c793a20746869732e6f7074696f6e732e746f6f6c746970466f6e7446616d696c792c0a09090909090909666f6e745374796c653a20746869732e6f7074696f6e732e746f6f6c746970466f6e745374796c652c0a09090909090909666f6e7453697a653a20746869732e6f7074696f6e732e746f6f6c746970466f6e7453697a652c0a0909090909090963617265744865696768743a20746869732e6f7074696f6e732e746f6f6c746970436172657453697a652c0a09090909090909636f726e65725261646975733a20746869732e6f7074696f6e732e746f6f6c746970436f726e65725261646975732c0a09090909090909746578743a2074656d706c61746528746869732e6f7074696f6e732e746f6f6c74697054656d706c6174652c20456c656d656e74292c0a0909090909090963686172743a20746869732e63686172740a0909090909097d292e"
              "6472617728293b0a09090909097d2c2074686973293b0a090909097d0a0909097d0a09090972657475726e20746869733b0a09097d2c0a0909746f426173653634496d616765203a2066756e6374696f6e28297b0a09090972657475726e20746869732e63686172742e63616e7661732e746f4461746155524c2e6170706c7928746869732e63686172742e63616e7661732c20617267756d656e7473293b0a09097d0a097d293b0a0a0943686172742e547970652e657874656e64203d2066756e6374696f6e28657874656e73696f6e73297b0a0a090976617220706172656e74203d20746869733b0a0a090976617220436861727454797065203d2066756e6374696f6e28297b0a09090972657475726e20706172656e742e6170706c7928746869732c617267756d656e7473293b0a09097d3b0a0a09092f2f436f7079207468652070726f746f74797065206f626a656374206f6620746865207468697320636c6173730a09094368617274547970652e70726f746f74797065203d20636c6f6e6528706172656e742e70726f746f74797065293b0a09092f2f4e6f77206f766572777269746520736f6d65206f66207468652070726f7065727469657320696e20746865206261736520636c617373207769746820746865206e657720657874656e73696f6e730a0909657874656e64284368617274547970652e70"
              "726f746f747970652c20657874656e73696f6e73293b0a0a09094368617274547970652e657874656e64203d2043686172742e547970652e657874656e643b0a0a090969662028657874656e73696f6e732e6e616d65207c7c20706172656e742e70726f746f747970652e6e616d65297b0a0a0909097661722063686172744e616d65203d20657874656e73696f6e732e6e616d65207c7c20706172656e742e70726f746f747970652e6e616d653b0a0909092f2f41737369676e20616e7920706f74656e7469616c2064656661756c742076616c756573206f6620746865206e657720636861727420747970650a0a0909092f2f4966206e6f6e652061726520646566696e65642c207765276c6c20757365206120636c6f6e65206f662074686520636861727420747970652074686973206973206265696e6720657874656e6465642066726f6d2e0a0909092f2f492e652e20696620776520657874656e642061206c696e652063686172742c207765276c6c20757365207468652064656661756c74732066726f6d20746865206c696e65206368617274206966206f7572206e65772063686172740a0909092f2f646f65736e277420646566696e6520736f6d652064656661756c7473206f66207468656972206f776e2e0a0a090909766172206261736544656661756c7473203d202843686172742e64656661756c"
              "74735b706172656e742e70726f746f747970652e6e616d655d29203f20636c6f6e652843686172742e64656661756c74735b706172656e742e70726f746f747970652e6e616d655d29203a207b7d3b0a0a09090943686172742e64656661756c74735b63686172744e616d655d203d20657874656e64286261736544656661756c74732c657874656e73696f6e732e64656661756c7473293b0a0a09090943686172742e74797065735b63686172744e616d655d203d204368617274547970653b0a0a0909092f2f52656769737465722074686973206e6577206368617274207479706520696e207468652043686172742070726f746f747970650a09090943686172742e70726f746f747970655b63686172744e616d655d203d2066756e6374696f6e28646174612c6f7074696f6e73297b0a0909090976617220636f6e666967203d206d657267652843686172742e64656661756c74732e676c6f62616c2c2043686172742e64656661756c74735b63686172744e616d655d2c206f7074696f6e73207c7c207b7d293b0a0909090972657475726e206e65772043686172745479706528646174612c636f6e6669672c74686973293b0a0909097d3b0a09097d20656c73657b0a0909097761726e28224e616d65206e6f742070726f766964656420666f7220746869732063686172742c20736f206974206861736e2774"
              "206265656e207265676973746572656422293b0a09097d0a090972657475726e20706172656e743b0a097d3b0a0a0943686172742e456c656d656e74203d2066756e6374696f6e28636f6e66696775726174696f6e297b0a0909657874656e6428746869732c636f6e66696775726174696f6e293b0a0909746869732e696e697469616c697a652e6170706c7928746869732c617267756d656e7473293b0a0909746869732e7361766528293b0a097d3b0a09657874656e642843686172742e456c656d656e742e70726f746f747970652c7b0a0909696e697469616c697a65203a2066756e6374696f6e28297b7d2c0a0909726573746f7265203a2066756e6374696f6e2870726f7073297b0a090909696620282170726f7073297b0a09090909657874656e6428746869732c746869732e5f7361766564293b0a0909097d20656c7365207b0a09090909656163682870726f70732c66756e6374696f6e286b6579297b0a0909090909746869735b6b65795d203d20746869732e5f73617665645b6b65795d3b0a090909097d2c74686973293b0a0909097d0a09090972657475726e20746869733b0a09097d2c0a090973617665203a2066756e6374696f6e28297b0a090909746869732e5f7361766564203d20636c6f6e652874686973293b0a09090964656c65746520746869732e5f73617665642e5f73617665643b"
              "0a09090972657475726e20746869733b0a09097d2c0a0909757064617465203a2066756e6374696f6e286e657750726f7073297b0a09090965616368286e657750726f70732c66756e6374696f6e2876616c75652c6b6579297b0a09090909746869732e5f73617665645b6b65795d203d20746869735b6b65795d3b0a09090909746869735b6b65795d203d2076616c75653b0a0909097d2c74686973293b0a09090972657475726e20746869733b0a09097d2c0a09097472616e736974696f6e203a2066756e6374696f6e2870726f70732c65617365297b0a090909656163682870726f70732c66756e6374696f6e2876616c75652c6b6579297b0a09090909746869735b6b65795d203d20282876616c7565202d20746869732e5f73617665645b6b65795d29202a206561736529202b20746869732e5f73617665645b6b65795d3b0a0909097d2c74686973293b0a09090972657475726e20746869733b0a09097d2c0a0909746f6f6c746970506f736974696f6e203a2066756e6374696f6e28297b0a09090972657475726e207b0a0909090978203a20746869732e782c0a0909090979203a20746869732e790a0909097d3b0a09097d0a097d293b0a0a0943686172742e456c656d656e742e657874656e64203d20696e6865726974733b0a0a0a0943686172742e506f696e74203d2043686172742e456c656d656e"
              "742e657874656e64287b0a0909646973706c61793a20747275652c0a0909696e52616e67653a2066756e6374696f6e286368617274582c636861727459297b0a09090976617220686974446574656374696f6e52616e6765203d20746869732e686974446574656374696f6e526164697573202b20746869732e7261646975733b0a09090972657475726e2028284d6174682e706f77286368617274582d746869732e782c2032292b4d6174682e706f77286368617274592d746869732e792c20322929203c204d6174682e706f7728686974446574656374696f6e52616e67652c3229293b0a09097d2c0a090964726177203a2066756e6374696f6e28297b0a09090969662028746869732e646973706c6179297b0a0909090976617220637478203d20746869732e6374783b0a090909096374782e626567696e5061746828293b0a0a090909096374782e61726328746869732e782c20746869732e792c20746869732e7261646975732c20302c204d6174682e50492a32293b0a090909096374782e636c6f73655061746828293b0a0a090909096374782e7374726f6b655374796c65203d20746869732e7374726f6b65436f6c6f723b0a090909096374782e6c696e655769647468203d20746869732e7374726f6b6557696474683b0a0a090909096374782e66696c6c5374796c65203d20746869732e66696c6c43"
              "6f6c6f723b0a0a090909096374782e66696c6c28293b0a090909096374782e7374726f6b6528293b0a0909097d0a0a0a0909092f2f517569636b20646562756720666f722062657a6965722063757276652073706c696e696e670a0909092f2f486967686c696768747320636f6e74726f6c20706f696e747320616e6420746865206c696e65206265747765656e207468656d2e0a0909092f2f48616e647920666f7220646576202d20737472697070656420696e20746865206d696e2076657273696f6e2e0a0a0909092f2f206374782e7361766528293b0a0909092f2f206374782e66696c6c5374796c65203d2022626c61636b223b0a0909092f2f206374782e7374726f6b655374796c65203d2022626c61636b220a0909092f2f206374782e626567696e5061746828293b0a0909092f2f206374782e61726328746869732e636f6e74726f6c506f696e74732e696e6e65722e782c746869732e636f6e74726f6c506f696e74732e696e6e65722e792c20322c20302c204d6174682e50492a32293b0a0909092f2f206374782e66696c6c28293b0a0a0909092f2f206374782e626567696e5061746828293b0a0909092f2f206374782e61726328746869732e636f6e74726f6c506f696e74732e6f757465722e782c746869732e636f6e74726f6c506f696e74732e6f757465722e792c20322c20302c204d617468"
              "2e50492a32293b0a0909092f2f206374782e66696c6c28293b0a0a0909092f2f206374782e6d6f7665546f28746869732e636f6e74726f6c506f696e74732e696e6e65722e782c746869732e636f6e74726f6c506f696e74732e696e6e65722e79293b0a0909092f2f206374782e6c696e65546f28746869732e636f6e74726f6c506f696e74732e6f757465722e782c746869732e636f6e74726f6c506f696e74732e6f757465722e79293b0a0909092f2f206374782e7374726f6b6528293b0a0a0909092f2f206374782e726573746f726528293b0a0a0a0a09097d0a097d293b0a0a0943686172742e417263203d2043686172742e456c656d656e742e657874656e64287b0a0909696e52616e6765203a2066756e6374696f6e286368617274582c636861727459297b0a0a09090976617220706f696e7452656c6174697665506f736974696f6e203d2068656c706572732e676574416e676c6546726f6d506f696e7428746869732c207b0a09090909783a206368617274582c0a09090909793a206368617274590a0909097d293b0a0a0909092f2f436865636b2069662077697468696e207468652072616e6765206f6620746865206f70656e2f636c6f736520616e676c650a090909766172206265747765656e416e676c6573203d2028706f696e7452656c6174697665506f736974696f6e2e616e676c65203e"
              "3d20746869732e7374617274416e676c6520262620706f696e7452656c6174697665506f736974696f6e2e616e676c65203c3d20746869732e656e64416e676c65292c0a0909090977697468696e526164697573203d2028706f696e7452656c6174697665506f736974696f6e2e64697374616e6365203e3d20746869732e696e6e657252616469757320262620706f696e7452656c6174697665506f736974696f6e2e64697374616e6365203c3d20746869732e6f75746572526164697573293b0a0a09090972657475726e20286265747765656e416e676c65732026262077697468696e526164697573293b0a0909092f2f456e737572652077697468696e20746865206f757473696465206f6620746865206172632063656e7472652c2062757420696e7369646520617263206f757465720a09097d2c0a0909746f6f6c746970506f736974696f6e203a2066756e6374696f6e28297b0a0909097661722063656e747265416e676c65203d20746869732e7374617274416e676c65202b202828746869732e656e64416e676c65202d20746869732e7374617274416e676c6529202f2032292c0a0909090972616e676546726f6d43656e747265203d2028746869732e6f75746572526164697573202d20746869732e696e6e657252616469757329202f2032202b20746869732e696e6e65725261646975733b0a09"
              "090972657475726e207b0a0909090978203a20746869732e78202b20284d6174682e636f732863656e747265416e676c6529202a2072616e676546726f6d43656e747265292c0a0909090979203a20746869732e79202b20284d6174682e73696e2863656e747265416e676c6529202a2072616e676546726f6d43656e747265290a0909097d3b0a09097d2c0a090964726177203a2066756e6374696f6e28616e696d6174696f6e50657263656e74297b0a0a09090976617220656173696e67446563696d616c203d20616e696d6174696f6e50657263656e74207c7c20313b0a0a09090976617220637478203d20746869732e6374783b0a0a0909096374782e626567696e5061746828293b0a0a0909096374782e61726328746869732e782c20746869732e792c20746869732e6f757465725261646975732c20746869732e7374617274416e676c652c20746869732e656e64416e676c65293b0a0a0909096374782e61726328746869732e782c20746869732e792c20746869732e696e6e65725261646975732c20746869732e656e64416e676c652c20746869732e7374617274416e676c652c2074727565293b0a0a0909096374782e636c6f73655061746828293b0a0909096374782e7374726f6b655374796c65203d20746869732e7374726f6b65436f6c6f723b0a0909096374782e6c696e655769647468203d"
              "20746869732e7374726f6b6557696474683b0a0a0909096374782e66696c6c5374796c65203d20746869732e66696c6c436f6c6f723b0a0a0909096374782e66696c6c28293b0a0909096374782e6c696e654a6f696e203d2027626576656c273b0a0a09090969662028746869732e73686f775374726f6b65297b0a090909096374782e7374726f6b6528293b0a0909097d0a09097d0a097d293b0a0a0943686172742e52656374616e676c65203d2043686172742e456c656d656e742e657874656e64287b0a090964726177203a2066756e6374696f6e28297b0a09090976617220637478203d20746869732e6374782c0a0909090968616c665769647468203d20746869732e77696474682f322c0a090909096c65667458203d20746869732e78202d2068616c6657696474682c0a09090909726967687458203d20746869732e78202b2068616c6657696474682c0a09090909746f70203d20746869732e62617365202d2028746869732e62617365202d20746869732e79292c0a0909090968616c665374726f6b65203d20746869732e7374726f6b655769647468202f20323b0a0a0909092f2f2043616e76617320646f65736e277420616c6c6f7720757320746f207374726f6b6520696e736964652074686520776964746820736f2077652063616e0a0909092f2f2061646a757374207468652073697a657320"
              "746f206669742069662077652772652073657474696e672061207374726f6b65206f6e20746865206c696e650a09090969662028746869732e73686f775374726f6b65297b0a090909096c65667458202b3d2068616c665374726f6b653b0a09090909726967687458202d3d2068616c665374726f6b653b0a09090909746f70202b3d2068616c665374726f6b653b0a0909097d0a0a0909096374782e626567696e5061746828293b0a0a0909096374782e66696c6c5374796c65203d20746869732e66696c6c436f6c6f723b0a0909096374782e7374726f6b655374796c65203d20746869732e7374726f6b65436f6c6f723b0a0909096374782e6c696e655769647468203d20746869732e7374726f6b6557696474683b0a0a0909092f2f2049742764206265206e69636520746f206b656570207468697320636c61737320746f74616c6c792067656e6572696320746f20616e792072656374616e676c650a0909092f2f20616e642073696d706c79207370656369667920776869636820626f7264657220746f206d697373206f75742e0a0909096374782e6d6f7665546f286c656674582c20746869732e62617365293b0a0909096374782e6c696e65546f286c656674582c20746f70293b0a0909096374782e6c696e65546f287269676874582c20746f70293b0a0909096374782e6c696e65546f287269676874"
              "582c20746869732e62617365293b0a0909096374782e66696c6c28293b0a09090969662028746869732e73686f775374726f6b65297b0a090909096374782e7374726f6b6528293b0a0909097d0a09097d2c0a0909686569676874203a2066756e6374696f6e28297b0a09090972657475726e20746869732e62617365202d20746869732e793b0a09097d2c0a0909696e52616e6765203a2066756e6374696f6e286368617274582c636861727459297b0a09090972657475726e2028636861727458203e3d20746869732e78202d20746869732e77696474682f3220262620636861727458203c3d20746869732e78202b20746869732e77696474682f32292026262028636861727459203e3d20746869732e7920262620636861727459203c3d20746869732e62617365293b0a09097d0a097d293b0a0a0943686172742e546f6f6c746970203d2043686172742e456c656d656e742e657874656e64287b0a090964726177203a2066756e6374696f6e28297b0a0a09090976617220637478203d20746869732e63686172742e6374783b0a0a0909096374782e666f6e74203d20666f6e74537472696e6728746869732e666f6e7453697a652c746869732e666f6e745374796c652c746869732e666f6e7446616d696c79293b0a0a090909746869732e78416c69676e203d202263656e746572223b0a09090974686973"
              "2e79416c69676e203d202261626f7665223b0a0a0909092f2f44697374616e6365206265747765656e207468652061637475616c20656c656d656e742e7920706f736974696f6e20616e6420746865207374617274206f662074686520746f6f6c7469702063617265740a09090976617220636172657450616464696e67203d20323b0a0a09090976617220746f6f6c7469705769647468203d206374782e6d6561737572655465787428746869732e74657874292e7769647468202b20322a746869732e7850616464696e672c0a09090909746f6f6c74697052656374486569676874203d20746869732e666f6e7453697a65202b20322a746869732e7950616464696e672c0a09090909746f6f6c746970486569676874203d20746f6f6c74697052656374486569676874202b20746869732e6361726574486569676874202b20636172657450616464696e673b0a0a09090969662028746869732e78202b20746f6f6c74697057696474682f32203e746869732e63686172742e7769647468297b0a09090909746869732e78416c69676e203d20226c656674223b0a0909097d20656c73652069662028746869732e78202d20746f6f6c74697057696474682f32203c2030297b0a09090909746869732e78416c69676e203d20227269676874223b0a0909097d0a0a09090969662028746869732e79202d20746f6f6c"
              "746970486569676874203c2030297b0a09090909746869732e79416c69676e203d202262656c6f77223b0a0909097d0a0a0a09090976617220746f6f6c74697058203d20746869732e78202d20746f6f6c74697057696474682f322c0a09090909746f6f6c74697059203d20746869732e79202d20746f6f6c7469704865696768743b0a0a0909096374782e66696c6c5374796c65203d20746869732e66696c6c436f6c6f723b0a0a09090973776974636828746869732e79416c69676e290a0909097b0a09090963617365202261626f7665223a0a090909092f2f4472617720612063617265742061626f76652074686520782f790a090909096374782e626567696e5061746828293b0a090909096374782e6d6f7665546f28746869732e782c746869732e79202d20636172657450616464696e67293b0a090909096374782e6c696e65546f28746869732e78202b20746869732e63617265744865696768742c20746869732e79202d2028636172657450616464696e67202b20746869732e636172657448656967687429293b0a090909096374782e6c696e65546f28746869732e78202d20746869732e63617265744865696768742c20746869732e79202d2028636172657450616464696e67202b20746869732e636172657448656967687429293b0a090909096374782e636c6f73655061746828293b0a090909"
              "096374782e66696c6c28293b0a09090909627265616b3b0a09090963617365202262656c6f77223a0a09090909746f6f6c74697059203d20746869732e79202b20636172657450616464696e67202b20746869732e63617265744865696768743b0a090909092f2f4472617720612063617265742062656c6f772074686520782f790a090909096374782e626567696e5061746828293b0a090909096374782e6d6f7665546f28746869732e782c20746869732e79202b20636172657450616464696e67293b0a090909096374782e6c696e65546f28746869732e78202b20746869732e63617265744865696768742c20746869732e79202b20636172657450616464696e67202b20746869732e6361726574486569676874293b0a090909096374782e6c696e65546f28746869732e78202d20746869732e63617265744865696768742c20746869732e79202b20636172657450616464696e67202b20746869732e6361726574486569676874293b0a090909096374782e636c6f73655061746828293b0a090909096374782e66696c6c28293b0a09090909627265616b3b0a0909097d0a0a09090973776974636828746869732e78416c69676e290a0909097b0a0909096361736520226c656674223a0a09090909746f6f6c74697058203d20746869732e78202d20746f6f6c7469705769647468202b2028746869732e"
              "636f726e6572526164697573202b20746869732e6361726574486569676874293b0a09090909627265616b3b0a0909096361736520227269676874223a0a09090909746f6f6c74697058203d20746869732e78202d2028746869732e636f726e6572526164697573202b20746869732e6361726574486569676874293b0a09090909627265616b3b0a0909097d0a0a09090964726177526f756e64656452656374616e676c65286374782c746f6f6c746970582c746f6f6c746970592c746f6f6c74697057696474682c746f6f6c746970526563744865696768742c746869732e636f726e6572526164697573293b0a0a0909096374782e66696c6c28293b0a0a0909096374782e66696c6c5374796c65203d20746869732e74657874436f6c6f723b0a0909096374782e74657874416c69676e203d202263656e746572223b0a0909096374782e74657874426173656c696e65203d20226d6964646c65223b0a0909096374782e66696c6c5465787428746869732e746578742c20746f6f6c74697058202b20746f6f6c74697057696474682f322c20746f6f6c74697059202b20746f6f6c746970526563744865696768742f32293b0a09097d0a097d293b0a0a0943686172742e4d756c7469546f6f6c746970203d2043686172742e456c656d656e742e657874656e64287b0a0909696e697469616c697a65203a206675"
              "6e6374696f6e28297b0a090909746869732e666f6e74203d20666f6e74537472696e6728746869732e666f6e7453697a652c746869732e666f6e745374796c652c746869732e666f6e7446616d696c79293b0a0a090909746869732e7469746c65466f6e74203d20666f6e74537472696e6728746869732e7469746c65466f6e7453697a652c746869732e7469746c65466f6e745374796c652c746869732e7469746c65466f6e7446616d696c79293b0a0a090909746869732e686569676874203d2028746869732e6c6162656c732e6c656e677468202a20746869732e666f6e7453697a6529202b202828746869732e6c6162656c732e6c656e6774682d3129202a2028746869732e666f6e7453697a652f322929202b2028746869732e7950616464696e672a3229202b20746869732e7469746c65466f6e7453697a65202a312e353b0a0a090909746869732e6374782e666f6e74203d20746869732e7469746c65466f6e743b0a0a090909766172207469746c655769647468203d20746869732e6374782e6d6561737572655465787428746869732e7469746c65292e77696474682c0a090909092f2f4c6162656c206861732061206c6567656e64207371756172652061732077656c6c20736f206163636f756e7420666f7220746869732e0a090909096c6162656c5769647468203d206c6f6e6765737454657874"
              "28746869732e6374782c746869732e666f6e742c746869732e6c6162656c7329202b20746869732e666f6e7453697a65202b20332c0a090909096c6f6e67657374546578745769647468203d206d6178285b6c6162656c57696474682c7469746c6557696474685d293b0a0a090909746869732e7769647468203d206c6f6e67657374546578745769647468202b2028746869732e7850616464696e672a32293b0a0a0a0909097661722068616c66486569676874203d20746869732e6865696768742f323b0a0a0909092f2f436865636b20746f20656e7375726520746865206865696768742077696c6c20666974206f6e207468652063616e7661730a0909092f2f54686520746872656520697320746f2062756666657220666f726d2074686520766572790a09090969662028746869732e79202d2068616c66486569676874203c203020297b0a09090909746869732e79203d2068616c664865696768743b0a0909097d20656c73652069662028746869732e79202b2068616c66486569676874203e20746869732e63686172742e686569676874297b0a09090909746869732e79203d20746869732e63686172742e686569676874202d2068616c664865696768743b0a0909097d0a0a0909092f2f446563696465207768657468657220746f20616c69676e206c656674206f7220726967687420626173656420"
              "6f6e20706f736974696f6e206f6e2063616e7661730a09090969662028746869732e78203e20746869732e63686172742e77696474682f32297b0a09090909746869732e78202d3d20746869732e784f6666736574202b20746869732e77696474683b0a0909097d20656c7365207b0a09090909746869732e78202b3d20746869732e784f66667365743b0a0909097d0a0a0a09097d2c0a09096765744c696e65486569676874203a2066756e6374696f6e28696e646578297b0a09090976617220626173654c696e65486569676874203d20746869732e79202d2028746869732e6865696768742f3229202b20746869732e7950616464696e672c0a0909090961667465725469746c65496e646578203d20696e6465782d313b0a0a0909092f2f49662074686520696e646578206973207a65726f2c2077652772652067657474696e6720746865207469746c650a09090969662028696e646578203d3d3d2030297b0a0909090972657475726e20626173654c696e65486569676874202b20746869732e7469746c65466f6e7453697a652f323b0a0909097d20656c73657b0a0909090972657475726e20626173654c696e65486569676874202b202828746869732e666f6e7453697a652a312e352a61667465725469746c65496e64657829202b20746869732e666f6e7453697a652f3229202b20746869732e746974"
              "6c65466f6e7453697a65202a20312e353b0a0909097d0a0a09097d2c0a090964726177203a2066756e6374696f6e28297b0a09090964726177526f756e64656452656374616e676c6528746869732e6374782c746869732e782c746869732e79202d20746869732e6865696768742f322c746869732e77696474682c746869732e6865696768742c746869732e636f726e6572526164697573293b0a09090976617220637478203d20746869732e6374783b0a0909096374782e66696c6c5374796c65203d20746869732e66696c6c436f6c6f723b0a0909096374782e66696c6c28293b0a0909096374782e636c6f73655061746828293b0a0a0909096374782e74657874416c69676e203d20226c656674223b0a0909096374782e74657874426173656c696e65203d20226d6964646c65223b0a0909096374782e66696c6c5374796c65203d20746869732e7469746c6554657874436f6c6f723b0a0909096374782e666f6e74203d20746869732e7469746c65466f6e743b0a0a0909096374782e66696c6c5465787428746869732e7469746c652c746869732e78202b20746869732e7850616464696e672c20746869732e6765744c696e65486569676874283029293b0a0a0909096374782e666f6e74203d20746869732e666f6e743b0a09090968656c706572732e6561636828746869732e6c6162656c732c66756e"
              "6374696f6e286c6162656c2c696e646578297b0a090909096374782e66696c6c5374796c65203d20746869732e74657874436f6c6f723b0a090909096374782e66696c6c54657874286c6162656c2c746869732e78202b20746869732e7850616464696e67202b20746869732e666f6e7453697a65202b20332c20746869732e6765744c696e6548656967687428696e646578202b203129293b0a0a090909092f2f412062697420676e61726c792c2062757420636c656172696e6720746869732072656374616e676c6520627265616b73207768656e207573696e67206578706c6f72657263616e7661732028636c656172732077686f6c652063616e766173290a090909092f2f6374782e636c6561725265637428746869732e78202b20746869732e7850616464696e672c20746869732e6765744c696e6548656967687428696e646578202b203129202d20746869732e666f6e7453697a652f322c20746869732e666f6e7453697a652c20746869732e666f6e7453697a65293b0a090909092f2f496e7374656164207765276c6c206d616b6520612077686974652066696c6c656420626c6f636b20746f2070757420746865206c6567656e64436f6c6f75722070616c65747465206f7665722e0a0a090909096374782e66696c6c5374796c65203d20746869732e6c6567656e64436f6c6f724261636b67726f75"
              "6e643b0a090909096374782e66696c6c5265637428746869732e78202b20746869732e7850616464696e672c20746869732e6765744c696e6548656967687428696e646578202b203129202d20746869732e666f6e7453697a652f322c20746869732e666f6e7453697a652c20746869732e666f6e7453697a65293b0a0a090909096374782e66696c6c5374796c65203d20746869732e6c6567656e64436f6c6f72735b696e6465785d2e66696c6c3b0a090909096374782e66696c6c5265637428746869732e78202b20746869732e7850616464696e672c20746869732e6765744c696e6548656967687428696e646578202b203129202d20746869732e666f6e7453697a652f322c20746869732e666f6e7453697a652c20746869732e666f6e7453697a65293b0a0a0a0909097d2c74686973293b0a09097d0a097d293b0a0a0943686172742e5363616c65203d2043686172742e456c656d656e742e657874656e64287b0a0909696e697469616c697a65203a2066756e6374696f6e28297b0a090909746869732e66697428293b0a09097d2c0a09096275696c64594c6162656c73203a2066756e6374696f6e28297b0a090909746869732e794c6162656c73203d205b5d3b0a0a0909097661722073746570446563696d616c506c61636573203d20676574446563696d616c506c6163657328746869732e73746570"
              "56616c7565293b0a0a090909666f72202876617220693d303b20693c3d746869732e73746570733b20692b2b297b0a09090909746869732e794c6162656c732e707573682874656d706c61746528746869732e74656d706c617465537472696e672c7b76616c75653a28746869732e6d696e202b202869202a20746869732e7374657056616c756529292e746f46697865642873746570446563696d616c506c61636573297d29293b0a0909097d0a090909746869732e794c6162656c5769647468203d2028746869732e646973706c617920262620746869732e73686f774c6162656c7329203f206c6f6e676573745465787428746869732e6374782c746869732e666f6e742c746869732e794c6162656c7329203a20303b0a09097d2c0a0909616464584c6162656c203a2066756e6374696f6e286c6162656c297b0a090909746869732e784c6162656c732e70757368286c6162656c293b0a090909746869732e76616c756573436f756e742b2b3b0a090909746869732e66697428293b0a09097d2c0a090972656d6f7665584c6162656c203a2066756e6374696f6e28297b0a090909746869732e784c6162656c732e736869667428293b0a090909746869732e76616c756573436f756e742d2d3b0a090909746869732e66697428293b0a09097d2c0a09092f2f2046697474696e67206c6f6f7020746f20726f74"
              "6174652078204c6162656c7320616e6420666967757265206f7574207768617420666974732074686572652c20616e6420616c736f2063616c63756c61746520686f77206d616e79205920737465707320746f207573650a09096669743a2066756e6374696f6e28297b0a0909092f2f204669727374207765206e65656420746865207769647468206f662074686520794c6162656c732c20617373756d696e672074686520784c6162656c73206172656e277420726f74617465640a0a0909092f2f20546f20646f2074686174207765206e656564207468652062617365206c696e652061742074686520746f7020616e642062617365206f66207468652063686172742c20617373756d696e67207468657265206973206e6f2078206c6162656c20726f746174696f6e0a090909746869732e7374617274506f696e74203d2028746869732e646973706c617929203f20746869732e666f6e7453697a65203a20303b0a090909746869732e656e64506f696e74203d2028746869732e646973706c617929203f20746869732e686569676874202d2028746869732e666f6e7453697a65202a20312e3529202d2035203a20746869732e6865696768743b202f2f202d3520746f20706164206c6162656c730a0a0909092f2f204170706c792070616464696e672073657474696e677320746f2074686520737461727420"
              "616e6420656e6420706f696e742e0a090909746869732e7374617274506f696e74202b3d20746869732e70616464696e673b0a090909746869732e656e64506f696e74202d3d20746869732e70616464696e673b0a0a0909092f2f20436163686520746865207374617274696e67206865696768742c20736f2063616e2064657465726d696e65206966207765206e65656420746f20726563616c63756c61746520746865207363616c652079417869730a09090976617220636163686564486569676874203d20746869732e656e64506f696e74202d20746869732e7374617274506f696e742c0a09090909636163686564594c6162656c57696474683b0a0a0909092f2f204275696c64207468652063757272656e7420794c6162656c7320736f207765206861766520616e2069646561206f6620776861742073697a652074686579276c6c20626520746f2073746172740a0909092f2a0a090909202a0954686973207365747320776861742069732072657475726e65642066726f6d2063616c63756c6174655363616c6552616e6765206173207374617469632070726f70657274696573206f66207468697320636c6173733a0a090909202a0a09090909746869732e73746570733b0a09090909746869732e7374657056616c75653b0a09090909746869732e6d696e3b0a09090909746869732e6d61783b0a09"
              "0909202a0a090909202a2f0a090909746869732e63616c63756c6174655952616e676528636163686564486569676874293b0a0a0909092f2f20576974682074686573652070726f70657274696573207365742077652063616e206e6f77206275696c6420746865206172726179206f6620794c6162656c730a0909092f2f20616e6420616c736f20746865207769647468206f6620746865206c61726765737420794c6162656c0a090909746869732e6275696c64594c6162656c7328293b0a0a090909746869732e63616c63756c617465584c6162656c526f746174696f6e28293b0a0a0909097768696c652828636163686564486569676874203e20746869732e656e64506f696e74202d20746869732e7374617274506f696e7429297b0a09090909636163686564486569676874203d20746869732e656e64506f696e74202d20746869732e7374617274506f696e743b0a09090909636163686564594c6162656c5769647468203d20746869732e794c6162656c57696474683b0a0a09090909746869732e63616c63756c6174655952616e676528636163686564486569676874293b0a09090909746869732e6275696c64594c6162656c7328293b0a0a090909092f2f204f6e6c7920676f207468726f7567682074686520784c6162656c206c6f6f7020616761696e2069662074686520794c6162656c207769"
              "64746820686173206368616e6765640a0909090969662028636163686564594c6162656c5769647468203c20746869732e794c6162656c5769647468297b0a0909090909746869732e63616c63756c617465584c6162656c526f746174696f6e28293b0a090909097d0a0909097d0a0a09097d2c0a090963616c63756c617465584c6162656c526f746174696f6e203a2066756e6374696f6e28297b0a0909092f2f47657420746865207769647468206f66206561636820677269642062792063616c63756c6174696e672074686520646966666572656e63650a0909092f2f6265747765656e2078206f666673657473206265747765656e203020616e6420312e0a0a090909746869732e6374782e666f6e74203d20746869732e666f6e743b0a0a0909097661722066697273745769647468203d20746869732e6374782e6d6561737572655465787428746869732e784c6162656c735b305d292e77696474682c0a090909096c6173745769647468203d20746869732e6374782e6d6561737572655465787428746869732e784c6162656c735b746869732e784c6162656c732e6c656e677468202d20315d292e77696474682c0a090909096669727374526f74617465642c0a090909096c617374526f74617465643b0a0a0a090909746869732e785363616c6550616464696e675269676874203d206c617374576964"
              "74682f32202b20333b0a090909746869732e785363616c6550616464696e674c656674203d2028666972737457696474682f32203e20746869732e794c6162656c5769647468202b20313029203f20666972737457696474682f32203a20746869732e794c6162656c5769647468202b2031303b0a0a090909746869732e784c6162656c526f746174696f6e203d20303b0a09090969662028746869732e646973706c6179297b0a09090909766172206f726967696e616c4c6162656c5769647468203d206c6f6e676573745465787428746869732e6374782c746869732e666f6e742c746869732e784c6162656c73292c0a0909090909636f73526f746174696f6e2c0a09090909096669727374526f746174656457696474683b0a09090909746869732e784c6162656c5769647468203d206f726967696e616c4c6162656c57696474683b0a090909092f2f416c6c6f77203320706978656c732078322070616464696e6720656974686572207369646520666f72206c6162656c20726561646162696c6974790a090909097661722078477269645769647468203d204d6174682e666c6f6f7228746869732e63616c63756c61746558283129202d20746869732e63616c63756c6174655828302929202d20363b0a0a090909092f2f4d6178206c6162656c20726f746174652073686f756c64206265203930202d2061"
              "6c736f206163742061732061206c6f6f7020636f756e7465720a090909097768696c65202828746869732e784c6162656c5769647468203e207847726964576964746820262620746869732e784c6162656c526f746174696f6e203d3d3d203029207c7c2028746869732e784c6162656c5769647468203e207847726964576964746820262620746869732e784c6162656c526f746174696f6e203c3d20393020262620746869732e784c6162656c526f746174696f6e203e203029297b0a0909090909636f73526f746174696f6e203d204d6174682e636f7328746f52616469616e7328746869732e784c6162656c526f746174696f6e29293b0a0a09090909096669727374526f7461746564203d20636f73526f746174696f6e202a20666972737457696474683b0a09090909096c617374526f7461746564203d20636f73526f746174696f6e202a206c61737457696474683b0a0a09090909092f2f20576527726520726967687420616c69676e696e67207468652074657874206e6f772e0a0909090909696620286669727374526f7461746564202b20746869732e666f6e7453697a65202f2032203e20746869732e794c6162656c5769647468202b2038297b0a090909090909746869732e785363616c6550616464696e674c656674203d206669727374526f7461746564202b20746869732e666f6e7453697a"
              "65202f20323b0a09090909097d0a0909090909746869732e785363616c6550616464696e675269676874203d20746869732e666f6e7453697a652f323b0a0a0a0909090909746869732e784c6162656c526f746174696f6e2b2b3b0a0909090909746869732e784c6162656c5769647468203d20636f73526f746174696f6e202a206f726967696e616c4c6162656c57696474683b0a0a090909097d0a0909090969662028746869732e784c6162656c526f746174696f6e203e2030297b0a0909090909746869732e656e64506f696e74202d3d204d6174682e73696e28746f52616469616e7328746869732e784c6162656c526f746174696f6e29292a6f726967696e616c4c6162656c5769647468202b20333b0a090909097d0a0909097d0a090909656c73657b0a09090909746869732e784c6162656c5769647468203d20303b0a09090909746869732e785363616c6550616464696e675269676874203d20746869732e70616464696e673b0a09090909746869732e785363616c6550616464696e674c656674203d20746869732e70616464696e673b0a0909097d0a0a09097d2c0a09092f2f204e6565647320746f206265206f766572696464656e20696e206561636820436861727420747970650a09092f2f204f7468657277697365207765206e65656420746f207061737320616c6c20746865206461746120"
              "696e746f20746865207363616c6520636c6173730a090963616c63756c6174655952616e67653a206e6f6f702c0a090964726177696e67417265613a2066756e6374696f6e28297b0a09090972657475726e20746869732e7374617274506f696e74202d20746869732e656e64506f696e743b0a09097d2c0a090963616c63756c61746559203a2066756e6374696f6e2876616c7565297b0a090909766172207363616c696e67466163746f72203d20746869732e64726177696e67417265612829202f2028746869732e6d696e202d20746869732e6d6178293b0a09090972657475726e20746869732e656e64506f696e74202d20287363616c696e67466163746f72202a202876616c7565202d20746869732e6d696e29293b0a09097d2c0a090963616c63756c61746558203a2066756e6374696f6e28696e646578297b0a090909766172206973526f7461746564203d2028746869732e784c6162656c526f746174696f6e203e2030292c0a090909092f2f20696e6e65725769647468203d2028746869732e6f6666736574477269644c696e657329203f20746869732e7769647468202d206f66667365744c656674202d20746869732e70616464696e67203a20746869732e7769647468202d20286f66667365744c656674202b2068616c664c6162656c5769647468202a203229202d20746869732e7061646469"
              "6e672c0a09090909696e6e65725769647468203d20746869732e7769647468202d2028746869732e785363616c6550616464696e674c656674202b20746869732e785363616c6550616464696e675269676874292c0a0909090976616c75655769647468203d20696e6e657257696474682f28746869732e76616c756573436f756e74202d202828746869732e6f6666736574477269644c696e657329203f2030203a203129292c0a0909090976616c75654f6666736574203d202876616c75655769647468202a20696e64657829202b20746869732e785363616c6550616464696e674c6566743b0a0a09090969662028746869732e6f6666736574477269644c696e6573297b0a0909090976616c75654f6666736574202b3d202876616c756557696474682f32293b0a0909097d0a0a09090972657475726e204d6174682e726f756e642876616c75654f6666736574293b0a09097d2c0a0909757064617465203a2066756e6374696f6e286e657750726f7073297b0a09090968656c706572732e657874656e6428746869732c206e657750726f7073293b0a090909746869732e66697428293b0a09097d2c0a090964726177203a2066756e6374696f6e28297b0a09090976617220637478203d20746869732e6374782c0a09090909794c6162656c476170203d2028746869732e656e64506f696e74202d20746869"
              "732e7374617274506f696e7429202f20746869732e73746570732c0a09090909785374617274203d204d6174682e726f756e6428746869732e785363616c6550616464696e674c656674293b0a09090969662028746869732e646973706c6179297b0a090909096374782e66696c6c5374796c65203d20746869732e74657874436f6c6f723b0a090909096374782e666f6e74203d20746869732e666f6e743b0a090909096561636828746869732e794c6162656c732c66756e6374696f6e286c6162656c537472696e672c696e646578297b0a090909090976617220794c6162656c43656e746572203d20746869732e656e64506f696e74202d2028794c6162656c476170202a20696e646578292c0a0909090909096c696e65506f736974696f6e59203d204d6174682e726f756e6428794c6162656c43656e746572293b0a0a09090909096374782e74657874416c69676e203d20227269676874223b0a09090909096374782e74657874426173656c696e65203d20226d6964646c65223b0a090909090969662028746869732e73686f774c6162656c73297b0a0909090909096374782e66696c6c54657874286c6162656c537472696e672c785374617274202d2031302c794c6162656c43656e746572293b0a09090909097d0a09090909096374782e626567696e5061746828293b0a090909090969662028696e64"
              "6578203e2030297b0a0909090909092f2f205468697320697320612067726964206c696e6520696e207468652063656e7472652c20736f2064726f7020746861740a0909090909096374782e6c696e655769647468203d20746869732e677269644c696e6557696474683b0a0909090909096374782e7374726f6b655374796c65203d20746869732e677269644c696e65436f6c6f723b0a09090909097d20656c7365207b0a0909090909092f2f205468697320697320746865206669727374206c696e65206f6e20746865207363616c650a0909090909096374782e6c696e655769647468203d20746869732e6c696e6557696474683b0a0909090909096374782e7374726f6b655374796c65203d20746869732e6c696e65436f6c6f723b0a09090909097d0a0a09090909096c696e65506f736974696f6e59202b3d2068656c706572732e616c696173506978656c286374782e6c696e655769647468293b0a0a09090909096374782e6d6f7665546f287853746172742c206c696e65506f736974696f6e59293b0a09090909096374782e6c696e65546f28746869732e77696474682c206c696e65506f736974696f6e59293b0a09090909096374782e7374726f6b6528293b0a09090909096374782e636c6f73655061746828293b0a0a09090909096374782e6c696e655769647468203d20746869732e6c696e6557"
              "696474683b0a09090909096374782e7374726f6b655374796c65203d20746869732e6c696e65436f6c6f723b0a09090909096374782e626567696e5061746828293b0a09090909096374782e6d6f7665546f28785374617274202d20352c206c696e65506f736974696f6e59293b0a09090909096374782e6c696e65546f287853746172742c206c696e65506f736974696f6e59293b0a09090909096374782e7374726f6b6528293b0a09090909096374782e636c6f73655061746828293b0a0a090909097d2c74686973293b0a0a090909096561636828746869732e784c6162656c732c66756e6374696f6e286c6162656c2c696e646578297b0a09090909097661722078506f73203d20746869732e63616c63756c6174655828696e64657829202b20616c696173506978656c28746869732e6c696e655769647468292c0a0909090909092f2f20436865636b20746f20736565206966206c696e652f626172206865726520616e642064656369646520776865726520746f20706c61636520746865206c696e650a0909090909096c696e65506f73203d20746869732e63616c63756c6174655828696e646578202d2028746869732e6f6666736574477269644c696e6573203f20302e35203a20302929202b20616c696173506978656c28746869732e6c696e655769647468292c0a0909090909096973526f746174"
              "6564203d2028746869732e784c6162656c526f746174696f6e203e2030293b0a0a09090909096374782e626567696e5061746828293b0a0a090909090969662028696e646578203e2030297b0a0909090909092f2f205468697320697320612067726964206c696e6520696e207468652063656e7472652c20736f2064726f7020746861740a0909090909096374782e6c696e655769647468203d20746869732e677269644c696e6557696474683b0a0909090909096374782e7374726f6b655374796c65203d20746869732e677269644c696e65436f6c6f723b0a09090909097d20656c7365207b0a0909090909092f2f205468697320697320746865206669727374206c696e65206f6e20746865207363616c650a0909090909096374782e6c696e655769647468203d20746869732e6c696e6557696474683b0a0909090909096374782e7374726f6b655374796c65203d20746869732e6c696e65436f6c6f723b0a09090909097d0a09090909096374782e6d6f7665546f286c696e65506f732c746869732e656e64506f696e74293b0a09090909096374782e6c696e65546f286c696e65506f732c746869732e7374617274506f696e74202d2033293b0a09090909096374782e7374726f6b6528293b0a09090909096374782e636c6f73655061746828293b0a0a0a09090909096374782e6c696e65576964746820"
              "3d20746869732e6c696e6557696474683b0a09090909096374782e7374726f6b655374796c65203d20746869732e6c696e65436f6c6f723b0a0a0a09090909092f2f20536d616c6c206c696e65732061742074686520626f74746f6d206f662074686520626173652067726964206c696e650a09090909096374782e626567696e5061746828293b0a09090909096374782e6d6f7665546f286c696e65506f732c746869732e656e64506f696e74293b0a09090909096374782e6c696e65546f286c696e65506f732c746869732e656e64506f696e74202b2035293b0a09090909096374782e7374726f6b6528293b0a09090909096374782e636c6f73655061746828293b0a0a09090909096374782e7361766528293b0a09090909096374782e7472616e736c6174652878506f732c286973526f746174656429203f20746869732e656e64506f696e74202b203132203a20746869732e656e64506f696e74202b2038293b0a09090909096374782e726f7461746528746f52616469616e7328746869732e784c6162656c526f746174696f6e292a2d31293b0a09090909096374782e666f6e74203d20746869732e666f6e743b0a09090909096374782e74657874416c69676e203d20286973526f746174656429203f2022726967687422203a202263656e746572223b0a09090909096374782e74657874426173656c69"
              "6e65203d20286973526f746174656429203f20226d6964646c6522203a2022746f70223b0a09090909096374782e66696c6c54657874286c6162656c2c20302c2030293b0a09090909096374782e726573746f726528293b0a090909097d2c74686973293b0a0a0909097d0a09097d0a0a097d293b0a0a0943686172742e52616469616c5363616c65203d2043686172742e456c656d656e742e657874656e64287b0a0909696e697469616c697a653a2066756e6374696f6e28297b0a090909746869732e73697a65203d206d696e285b746869732e6865696768742c20746869732e77696474685d293b0a090909746869732e64726177696e6741726561203d2028746869732e646973706c617929203f2028746869732e73697a652f3229202d2028746869732e666f6e7453697a652f32202b20746869732e6261636b64726f7050616464696e675929203a2028746869732e73697a652f32293b0a09097d2c0a090963616c63756c61746543656e7465724f66667365743a2066756e6374696f6e2876616c7565297b0a0909092f2f2054616b6520696e746f206163636f756e742068616c6620666f6e742073697a65202b20746865207950616464696e67206f662074686520746f702076616c75650a090909766172207363616c696e67466163746f72203d20746869732e64726177696e6741726561202f202874"
              "6869732e6d6178202d20746869732e6d696e293b0a0a09090972657475726e202876616c7565202d20746869732e6d696e29202a207363616c696e67466163746f723b0a09097d2c0a0909757064617465203a2066756e6374696f6e28297b0a0909096966202821746869732e6c696e65417263297b0a09090909746869732e7365745363616c6553697a6528293b0a0909097d20656c7365207b0a09090909746869732e64726177696e6741726561203d2028746869732e646973706c617929203f2028746869732e73697a652f3229202d2028746869732e666f6e7453697a652f32202b20746869732e6261636b64726f7050616464696e675929203a2028746869732e73697a652f32293b0a0909097d0a090909746869732e6275696c64594c6162656c7328293b0a09097d2c0a09096275696c64594c6162656c733a2066756e6374696f6e28297b0a090909746869732e794c6162656c73203d205b5d3b0a0a0909097661722073746570446563696d616c506c61636573203d20676574446563696d616c506c6163657328746869732e7374657056616c7565293b0a0a090909666f72202876617220693d303b20693c3d746869732e73746570733b20692b2b297b0a09090909746869732e794c6162656c732e707573682874656d706c61746528746869732e74656d706c617465537472696e672c7b76616c75"
              "653a28746869732e6d696e202b202869202a20746869732e7374657056616c756529292e746f46697865642873746570446563696d616c506c61636573297d29293b0a0909097d0a09097d2c0a090967657443697263756d666572656e6365203a2066756e6374696f6e28297b0a09090972657475726e2028284d6174682e50492a3229202f20746869732e76616c756573436f756e74293b0a09097d2c0a09097365745363616c6553697a653a2066756e6374696f6e28297b0a0909092f2a0a090909202a2052696768742c2074686973206973207265616c6c7920636f6e667573696e6720616e642074686572652069732061206c6f74206f66206d6174687320676f696e67206f6e20686572650a090909202a205468652067697374206f66207468652070726f626c656d20697320686572653a2068747470733a2f2f676973742e6769746875622e636f6d2f6e6e6e69636b2f36393663633963353566346230626562386665390a090909202a0a090909202a205265616374696f6e3a2068747470733a2f2f646c2e64726f70626f7875736572636f6e74656e742e636f6d2f752f33343630313336332f746f6f6d756368736369656e63652e6769660a090909202a0a090909202a20536f6c7574696f6e3a0a090909202a0a090909202a20576520617373756d652074686520726164697573206f662074686520"
              "706f6c79676f6e2069732068616c66207468652073697a65206f66207468652063616e7661732061742066697273740a090909202a206174206561636820696e64657820776520636865636b206966207468652074657874206f7665726c6170732e0a090909202a0a090909202a20576865726520697420646f65732c2077652073746f7265207468617420616e676c6520616e64207468617420696e6465782e0a090909202a0a090909202a2041667465722066696e64696e6720746865206c61726765737420696e64657820616e6420616e676c652077652063616c63756c61746520686f77206d756368207765206e65656420746f2072656d6f76650a090909202a2066726f6d207468652073686170652072616469757320746f206d6f76652074686520706f696e7420696e7761726473206279207468617420782e0a090909202a0a090909202a205765206176657261676520746865206c65667420616e642072696768742064697374616e63657320746f2067657420746865206d6178696d756d2073686170652072616469757320746861742063616e2066697420696e2074686520626f780a090909202a20616c6f6e672077697468206c6162656c732e0a090909202a0a090909202a204f6e6365207765206861766520746861742c2077652063616e2066696e64207468652063656e74726520706f696e"
              "7420666f72207468652063686172742c2062792074616b696e6720746865207820746578742070726f74727573696f6e0a090909202a206f6e206561636820736964652c2072656d6f76696e6720746861742066726f6d207468652073697a652c2068616c76696e6720697420616e6420616464696e6720746865206c65667420782070726f74727573696f6e2077696474682e0a090909202a0a090909202a20546869732077696c6c206d65616e207765206861766520612073686170652066697474656420746f207468652063616e7661732c206173206c617267652061732069742063616e206265207769746820746865206c6162656c730a090909202a20616e6420706f736974696f6e20697420696e20746865206d6f737420737061636520656666696369656e74206d616e6e65720a090909202a0a090909202a2068747470733a2f2f646c2e64726f70626f7875736572636f6e74656e742e636f6d2f752f33343630313336332f79656168736369656e63652e6769660a090909202a2f0a0a0a0909092f2f20476574206d6178696d756d20726164697573206f662074686520706f6c79676f6e2e204569746865722068616c66207468652068656967687420286d696e757320746865207465787420776964746829206f722068616c66207468652077696474682e0a0909092f2f20557365207468697320"
              "746f2063616c63756c61746520746865206f6666736574202b206368616e67652e202d204d616b652073757265204c2f522070726f74727573696f6e206973206174206c65617374203020746f2073746f702069737375657320776974682063656e74726520706f696e74730a090909766172206c617267657374506f737369626c65526164697573203d206d696e285b28746869732e6865696768742f32202d20746869732e706f696e744c6162656c466f6e7453697a65202d2035292c20746869732e77696474682f325d292c0a09090909706f696e74506f736974696f6e2c0a09090909692c0a090909097465787457696474682c0a0909090968616c665465787457696474682c0a0909090966757274686573745269676874203d20746869732e77696474682c0a0909090966757274686573745269676874496e6465782c0a0909090966757274686573745269676874416e676c652c0a0909090966757274686573744c656674203d20302c0a0909090966757274686573744c656674496e6465782c0a0909090966757274686573744c656674416e676c652c0a090909097850726f74727573696f6e4c6566742c0a090909097850726f74727573696f6e52696768742c0a09090909726164697573526564756374696f6e52696768742c0a09090909726164697573526564756374696f6e4c6566742c0a0909"
              "09096d617857696474685261646975733b0a090909746869732e6374782e666f6e74203d20666f6e74537472696e6728746869732e706f696e744c6162656c466f6e7453697a652c746869732e706f696e744c6162656c466f6e745374796c652c746869732e706f696e744c6162656c466f6e7446616d696c79293b0a090909666f722028693d303b693c746869732e76616c756573436f756e743b692b2b297b0a090909092f2f2035707820746f20737061636520746865207465787420736c696768746c79206f7574202d2073696d696c617220746f207768617420776520646f20696e2074686520647261772066756e6374696f6e2e0a09090909706f696e74506f736974696f6e203d20746869732e676574506f696e74506f736974696f6e28692c206c617267657374506f737369626c65526164697573293b0a09090909746578745769647468203d20746869732e6374782e6d656173757265546578742874656d706c61746528746869732e74656d706c617465537472696e672c207b2076616c75653a20746869732e6c6162656c735b695d207d29292e7769647468202b20353b0a090909096966202869203d3d3d2030207c7c2069203d3d3d20746869732e76616c756573436f756e742f32297b0a09090909092f2f20496620776527726520617420696e646578207a65726f2c206f722065786163746c"
              "7920746865206d6964646c652c2077652772652061742065786163746c792074686520746f702f626f74746f6d0a09090909092f2f206f66207468652072616461722063686172742c20736f20746578742077696c6c20626520616c69676e65642063656e7472616c6c792c20736f207765276c6c2068616c6620697420616e6420636f6d706172650a09090909092f2f20772f6c65667420616e6420726967687420746578742073697a65730a090909090968616c66546578745769647468203d207465787457696474682f323b0a090909090969662028706f696e74506f736974696f6e2e78202b2068616c66546578745769647468203e206675727468657374526967687429207b0a09090909090966757274686573745269676874203d20706f696e74506f736974696f6e2e78202b2068616c665465787457696474683b0a09090909090966757274686573745269676874496e646578203d20693b0a09090909097d0a090909090969662028706f696e74506f736974696f6e2e78202d2068616c66546578745769647468203c2066757274686573744c65667429207b0a09090909090966757274686573744c656674203d20706f696e74506f736974696f6e2e78202d2068616c665465787457696474683b0a09090909090966757274686573744c656674496e646578203d20693b0a09090909097d0a090909"
              "097d0a09090909656c7365206966202869203c20746869732e76616c756573436f756e742f3229207b0a09090909092f2f204c657373207468616e2068616c66207468652076616c756573206d65616e73207765276c6c206c65667420616c69676e2074686520746578740a090909090969662028706f696e74506f736974696f6e2e78202b20746578745769647468203e206675727468657374526967687429207b0a09090909090966757274686573745269676874203d20706f696e74506f736974696f6e2e78202b207465787457696474683b0a09090909090966757274686573745269676874496e646578203d20693b0a09090909097d0a090909097d0a09090909656c7365206966202869203e20746869732e76616c756573436f756e742f32297b0a09090909092f2f204d6f7265207468616e2068616c66207468652076616c756573206d65616e73207765276c6c20726967687420616c69676e2074686520746578740a090909090969662028706f696e74506f736974696f6e2e78202d20746578745769647468203c2066757274686573744c65667429207b0a09090909090966757274686573744c656674203d20706f696e74506f736974696f6e2e78202d207465787457696474683b0a09090909090966757274686573744c656674496e646578203d20693b0a09090909097d0a090909097d0a0909"
              "097d0a0a0909097850726f74727573696f6e4c656674203d2066757274686573744c6566743b0a0a0909097850726f74727573696f6e5269676874203d204d6174682e6365696c2866757274686573745269676874202d20746869732e7769647468293b0a0a09090966757274686573745269676874416e676c65203d20746869732e676574496e646578416e676c652866757274686573745269676874496e646578293b0a0a09090966757274686573744c656674416e676c65203d20746869732e676574496e646578416e676c652866757274686573744c656674496e646578293b0a0a090909726164697573526564756374696f6e5269676874203d207850726f74727573696f6e5269676874202f204d6174682e73696e2866757274686573745269676874416e676c65202b204d6174682e50492f32293b0a0a090909726164697573526564756374696f6e4c656674203d207850726f74727573696f6e4c656674202f204d6174682e73696e2866757274686573744c656674416e676c65202b204d6174682e50492f32293b0a0a0909092f2f20456e737572652077652061637475616c6c79206e65656420746f20726564756365207468652073697a65206f66207468652063686172740a090909726164697573526564756374696f6e5269676874203d202869734e756d626572287261646975735265647563"
              "74696f6e52696768742929203f20726164697573526564756374696f6e5269676874203a20303b0a090909726164697573526564756374696f6e4c656674203d202869734e756d62657228726164697573526564756374696f6e4c6566742929203f20726164697573526564756374696f6e4c656674203a20303b0a0a090909746869732e64726177696e6741726561203d206c617267657374506f737369626c65526164697573202d2028726164697573526564756374696f6e4c656674202b20726164697573526564756374696f6e5269676874292f323b0a0a0909092f2f746869732e64726177696e6741726561203d206d696e285b6d617857696474685261646975732c2028746869732e686569676874202d202832202a2028746869732e706f696e744c6162656c466f6e7453697a65202b20352929292f325d290a090909746869732e73657443656e746572506f696e7428726164697573526564756374696f6e4c6566742c20726164697573526564756374696f6e5269676874293b0a0a09097d2c0a090973657443656e746572506f696e743a2066756e6374696f6e286c6566744d6f76656d656e742c2072696768744d6f76656d656e74297b0a0a090909766172206d61785269676874203d20746869732e7769647468202d2072696768744d6f76656d656e74202d20746869732e64726177696e6741"
              "7265612c0a090909096d61784c656674203d206c6566744d6f76656d656e74202b20746869732e64726177696e67417265613b0a0a090909746869732e7843656e746572203d20286d61784c656674202b206d61785269676874292f323b0a0909092f2f20416c7761797320766572746963616c6c7920696e207468652063656e7472652061732074686520746578742068656967687420646f65736e2774206368616e67650a090909746869732e7943656e746572203d2028746869732e6865696768742f32293b0a09097d2c0a0a0909676574496e646578416e676c65203a2066756e6374696f6e28696e646578297b0a09090976617220616e676c654d756c7469706c696572203d20284d6174682e5049202a203229202f20746869732e76616c756573436f756e743b0a0909092f2f2053746172742066726f6d2074686520746f7020696e7374656164206f662072696768742c20736f2072656d6f766520612071756172746572206f662074686520636972636c650a0a09090972657475726e20696e646578202a20616e676c654d756c7469706c696572202d20284d6174682e50492f32293b0a09097d2c0a0909676574506f696e74506f736974696f6e203a2066756e6374696f6e28696e6465782c2064697374616e636546726f6d43656e746572297b0a0909097661722074686973416e676c65203d2074"
              "6869732e676574496e646578416e676c6528696e646578293b0a09090972657475726e207b0a0909090978203a20284d6174682e636f732874686973416e676c6529202a2064697374616e636546726f6d43656e74657229202b20746869732e7843656e7465722c0a0909090979203a20284d6174682e73696e2874686973416e676c6529202a2064697374616e636546726f6d43656e74657229202b20746869732e7943656e7465720a0909097d3b0a09097d2c0a0909647261773a2066756e6374696f6e28297b0a09090969662028746869732e646973706c6179297b0a0909090976617220637478203d20746869732e6374783b0a090909096561636828746869732e794c6162656c732c2066756e6374696f6e286c6162656c2c20696e646578297b0a09090909092f2f20446f6e2774206472617720612063656e7472652076616c75650a090909090969662028696e646578203e2030297b0a090909090909766172207943656e7465724f6666736574203d20696e646578202a2028746869732e64726177696e67417265612f746869732e7374657073292c0a0909090909090979486569676874203d20746869732e7943656e746572202d207943656e7465724f66667365742c0a09090909090909706f696e74506f736974696f6e3b0a0a0909090909092f2f20447261772063697263756c6172206c696e65"
              "732061726f756e6420746865207363616c650a09090909090969662028746869732e6c696e655769647468203e2030297b0a090909090909096374782e7374726f6b655374796c65203d20746869732e6c696e65436f6c6f723b0a090909090909096374782e6c696e655769647468203d20746869732e6c696e6557696474683b0a0a09090909090909696628746869732e6c696e65417263297b0a09090909090909096374782e626567696e5061746828293b0a09090909090909096374782e61726328746869732e7843656e7465722c20746869732e7943656e7465722c207943656e7465724f66667365742c20302c204d6174682e50492a32293b0a09090909090909096374782e636c6f73655061746828293b0a09090909090909096374782e7374726f6b6528293b0a090909090909097d20656c73657b0a09090909090909096374782e626567696e5061746828293b0a0909090909090909666f72202876617220693d303b693c746869732e76616c756573436f756e743b692b2b290a09090909090909097b0a090909090909090909706f696e74506f736974696f6e203d20746869732e676574506f696e74506f736974696f6e28692c20746869732e63616c63756c61746543656e7465724f666673657428746869732e6d696e202b2028696e646578202a20746869732e7374657056616c75652929293b"
              "0a0909090909090909096966202869203d3d3d2030297b0a090909090909090909096374782e6d6f7665546f28706f696e74506f736974696f6e2e782c20706f696e74506f736974696f6e2e79293b0a0909090909090909097d20656c7365207b0a090909090909090909096374782e6c696e65546f28706f696e74506f736974696f6e2e782c20706f696e74506f736974696f6e2e79293b0a0909090909090909097d0a09090909090909097d0a09090909090909096374782e636c6f73655061746828293b0a09090909090909096374782e7374726f6b6528293b0a090909090909097d0a0909090909097d0a090909090909696628746869732e73686f774c6162656c73297b0a090909090909096374782e666f6e74203d20666f6e74537472696e6728746869732e666f6e7453697a652c746869732e666f6e745374796c652c746869732e666f6e7446616d696c79293b0a0909090909090969662028746869732e73686f774c6162656c4261636b64726f70297b0a0909090909090909766172206c6162656c5769647468203d206374782e6d65617375726554657874286c6162656c292e77696474683b0a09090909090909096374782e66696c6c5374796c65203d20746869732e6261636b64726f70436f6c6f723b0a09090909090909096374782e66696c6c52656374280a09090909090909090974686973"
              "2e7843656e746572202d206c6162656c57696474682f32202d20746869732e6261636b64726f7050616464696e67582c0a09090909090909090979486569676874202d20746869732e666f6e7453697a652f32202d20746869732e6261636b64726f7050616464696e67592c0a0909090909090909096c6162656c5769647468202b20746869732e6261636b64726f7050616464696e67582a322c0a090909090909090909746869732e666f6e7453697a65202b20746869732e6261636b64726f7050616464696e67592a320a0909090909090909293b0a090909090909097d0a090909090909096374782e74657874416c69676e203d202763656e746572273b0a090909090909096374782e74657874426173656c696e65203d20226d6964646c65223b0a090909090909096374782e66696c6c5374796c65203d20746869732e666f6e74436f6c6f723b0a090909090909096374782e66696c6c54657874286c6162656c2c20746869732e7843656e7465722c2079486569676874293b0a0909090909097d0a09090909097d0a090909097d2c2074686973293b0a0a090909096966202821746869732e6c696e65417263297b0a09090909096374782e6c696e655769647468203d20746869732e616e676c654c696e6557696474683b0a09090909096374782e7374726f6b655374796c65203d20746869732e616e676c"
              "654c696e65436f6c6f723b0a0909090909666f7220287661722069203d20746869732e76616c756573436f756e74202d20313b2069203e3d20303b20692d2d29207b0a09090909090969662028746869732e616e676c654c696e655769647468203e2030297b0a09090909090909766172206f75746572506f736974696f6e203d20746869732e676574506f696e74506f736974696f6e28692c20746869732e63616c63756c61746543656e7465724f666673657428746869732e6d617829293b0a090909090909096374782e626567696e5061746828293b0a090909090909096374782e6d6f7665546f28746869732e7843656e7465722c20746869732e7943656e746572293b0a090909090909096374782e6c696e65546f286f75746572506f736974696f6e2e782c206f75746572506f736974696f6e2e79293b0a090909090909096374782e7374726f6b6528293b0a090909090909096374782e636c6f73655061746828293b0a0909090909097d0a0909090909092f2f20457874726120337078206f757420666f7220736f6d65206c6162656c2073706163696e670a09090909090976617220706f696e744c6162656c506f736974696f6e203d20746869732e676574506f696e74506f736974696f6e28692c20746869732e63616c63756c61746543656e7465724f666673657428746869732e6d617829202b20"
              "35293b0a0909090909096374782e666f6e74203d20666f6e74537472696e6728746869732e706f696e744c6162656c466f6e7453697a652c746869732e706f696e744c6162656c466f6e745374796c652c746869732e706f696e744c6162656c466f6e7446616d696c79293b0a0909090909096374782e66696c6c5374796c65203d20746869732e706f696e744c6162656c466f6e74436f6c6f723b0a0a090909090909766172206c6162656c73436f756e74203d20746869732e6c6162656c732e6c656e6774682c0a0909090909090968616c664c6162656c73436f756e74203d20746869732e6c6162656c732e6c656e6774682f322c0a09090909090909717561727465724c6162656c73436f756e74203d2068616c664c6162656c73436f756e742f322c0a09090909090909757070657248616c66203d202869203c20717561727465724c6162656c73436f756e74207c7c2069203e206c6162656c73436f756e74202d20717561727465724c6162656c73436f756e74292c0a09090909090909657861637451756172746572203d202869203d3d3d20717561727465724c6162656c73436f756e74207c7c2069203d3d3d206c6162656c73436f756e74202d20717561727465724c6162656c73436f756e74293b0a0909090909096966202869203d3d3d2030297b0a090909090909096374782e74657874416c6967"
              "6e203d202763656e746572273b0a0909090909097d20656c73652069662869203d3d3d2068616c664c6162656c73436f756e74297b0a090909090909096374782e74657874416c69676e203d202763656e746572273b0a0909090909097d20656c7365206966202869203c2068616c664c6162656c73436f756e74297b0a090909090909096374782e74657874416c69676e203d20276c656674273b0a0909090909097d20656c7365207b0a090909090909096374782e74657874416c69676e203d20277269676874273b0a0909090909097d0a0a0909090909092f2f205365742074686520636f7272656374207465787420626173656c696e65206261736564206f6e206f7574657220706f736974696f6e696e670a09090909090969662028657861637451756172746572297b0a090909090909096374782e74657874426173656c696e65203d20276d6964646c65273b0a0909090909097d20656c73652069662028757070657248616c66297b0a090909090909096374782e74657874426173656c696e65203d2027626f74746f6d273b0a0909090909097d20656c7365207b0a090909090909096374782e74657874426173656c696e65203d2027746f70273b0a0909090909097d0a0a0909090909096374782e66696c6c5465787428746869732e6c6162656c735b695d2c20706f696e744c6162656c506f736974"
              "696f6e2e782c20706f696e744c6162656c506f736974696f6e2e79293b0a09090909097d0a090909097d0a0909097d0a09097d0a097d293b0a0a092f2f2041747461636820676c6f62616c206576656e7420746f20726573697a65206561636820636861727420696e7374616e6365207768656e207468652062726f7773657220726573697a65730a0968656c706572732e6164644576656e742877696e646f772c2022726573697a65222c202866756e6374696f6e28297b0a09092f2f204261736963206465626f756e6365206f6620726573697a652066756e6374696f6e20736f20697420646f65736e2774206875727420706572666f726d616e6365207768656e20726573697a696e672062726f777365722e0a09097661722074696d656f75743b0a090972657475726e2066756e6374696f6e28297b0a090909636c65617254696d656f75742874696d656f7574293b0a09090974696d656f7574203d2073657454696d656f75742866756e6374696f6e28297b0a09090909656163682843686172742e696e7374616e6365732c66756e6374696f6e28696e7374616e6365297b0a09090909092f2f2049662074686520726573706f6e7369766520666c61672069732073657420696e2074686520636861727420696e7374616e636520636f6e6669670a09090909092f2f20436173636164652074686520726573"
              "697a65206576656e7420646f776e20746f207468652063686172742e0a090909090969662028696e7374616e63652e6f7074696f6e732e726573706f6e73697665297b0a090909090909696e7374616e63652e726573697a6528696e7374616e63652e72656e6465722c2074727565293b0a09090909097d0a090909097d293b0a0909097d2c203530293b0a09097d3b0a097d292829293b0a0a0a0969662028616d6429207b0a0909646566696e652866756e6374696f6e28297b0a09090972657475726e2043686172743b0a09097d293b0a097d20656c73652069662028747970656f66206d6f64756c65203d3d3d20276f626a65637427202626206d6f64756c652e6578706f72747329207b0a09096d6f64756c652e6578706f727473203d2043686172743b0a097d0a0a09726f6f742e4368617274203d2043686172743b0a0a0943686172742e6e6f436f6e666c696374203d2066756e6374696f6e28297b0a0909726f6f742e4368617274203d2070726576696f75733b0a090972657475726e2043686172743b0a097d3b0a0a7d292e63616c6c2874686973293b0a2866756e6374696f6e28297b0a092275736520737472696374223b0a0a0976617220726f6f74203d20746869732c0a09094368617274203d20726f6f742e43686172742c0a090968656c70657273203d2043686172742e68656c706572733b0a"
              "0a0a097661722064656661756c74436f6e666967203d207b0a09092f2f426f6f6c65616e202d205768657468657220746865207363616c652073686f756c64207374617274206174207a65726f2c206f7220616e206f72646572206f66206d61676e697475646520646f776e2066726f6d20746865206c6f776573742076616c75650a09097363616c65426567696e41745a65726f203a20747275652c0a0a09092f2f426f6f6c65616e202d20576865746865722067726964206c696e6573206172652073686f776e206163726f7373207468652063686172740a09097363616c6553686f77477269644c696e6573203a20747275652c0a0a09092f2f537472696e67202d20436f6c6f7572206f66207468652067726964206c696e65730a09097363616c65477269644c696e65436f6c6f72203a20227267626128302c302c302c2e303529222c0a0a09092f2f4e756d626572202d205769647468206f66207468652067726964206c696e65730a09097363616c65477269644c696e655769647468203a20312c0a0a09092f2f426f6f6c65616e202d2049662074686572652069732061207374726f6b65206f6e2065616368206261720a090962617253686f775374726f6b65203a20747275652c0a0a09092f2f4e756d626572202d20506978656c207769647468206f662074686520626172207374726f6b650a090962"
              "61725374726f6b655769647468203a20322c0a0a09092f2f4e756d626572202d2053706163696e67206265747765656e2065616368206f662074686520582076616c756520736574730a090962617256616c756553706163696e67203a20352c0a0a09092f2f4e756d626572202d2053706163696e67206265747765656e206461746120736574732077697468696e20582076616c7565730a09096261724461746173657453706163696e67203a20312c0a0a09092f2f537472696e67202d2041206c6567656e642074656d706c6174650a09096c6567656e6454656d706c617465203a20223c756c20636c6173733d5c223c253d6e616d652e746f4c6f776572436173652829253e2d6c6567656e645c223e3c2520666f72202876617220693d303b20693c64617461736574732e6c656e6774683b20692b2b297b253e3c6c693e3c7370616e207374796c653d5c226261636b67726f756e642d636f6c6f723a3c253d64617461736574735b695d2e66696c6c436f6c6f72253e5c223e3c2f7370616e3e3c2569662864617461736574735b695d2e6c6162656c297b253e3c253d64617461736574735b695d2e6c6162656c253e3c257d253e3c2f6c693e3c257d253e3c2f756c3e220a0a097d3b0a0a0a0943686172742e547970652e657874656e64287b0a09096e616d653a2022426172222c0a090964656661756c7473"
              "203a2064656661756c74436f6e6669672c0a0909696e697469616c697a653a202066756e6374696f6e2864617461297b0a0a0909092f2f4578706f7365206f7074696f6e7320617320612073636f7065207661726961626c65206865726520736f2077652063616e2061636365737320697420696e20746865205363616c65436c6173730a090909766172206f7074696f6e73203d20746869732e6f7074696f6e733b0a0a090909746869732e5363616c65436c617373203d2043686172742e5363616c652e657874656e64287b0a090909096f6666736574477269644c696e6573203a20747275652c0a0909090963616c63756c61746542617258203a2066756e6374696f6e2864617461736574436f756e742c2064617461736574496e6465782c20626172496e646578297b0a09090909092f2f5265757361626c65206d6574686f6420666f722063616c63756c6174696e67207468652078506f736974696f6e206f66206120676976656e20626172206261736564206f6e2064617461736574496e6465782026207769647468206f6620746865206261720a090909090976617220785769647468203d20746869732e63616c63756c61746542617365576964746828292c0a090909090909784162736f6c757465203d20746869732e63616c63756c6174655828626172496e64657829202d20287857696474682f32"
              "292c0a0909090909096261725769647468203d20746869732e63616c63756c61746542617257696474682864617461736574436f756e74293b0a0a090909090972657475726e20784162736f6c757465202b20286261725769647468202a2064617461736574496e64657829202b202864617461736574496e646578202a206f7074696f6e732e6261724461746173657453706163696e6729202b2062617257696474682f323b0a090909097d2c0a0909090963616c63756c617465426173655769647468203a2066756e6374696f6e28297b0a090909090972657475726e2028746869732e63616c63756c61746558283129202d20746869732e63616c63756c6174655828302929202d2028322a6f7074696f6e732e62617256616c756553706163696e67293b0a090909097d2c0a0909090963616c63756c6174654261725769647468203a2066756e6374696f6e2864617461736574436f756e74297b0a09090909092f2f5468652070616464696e67206265747765656e20646174617365747320697320746f20746865207269676874206f662065616368206261722c2070726f766964696e67207468617420746865726520617265206d6f7265207468616e203120646174617365740a090909090976617220626173655769647468203d20746869732e63616c63756c6174654261736557696474682829202d2028"
              "2864617461736574436f756e74202d203129202a206f7074696f6e732e6261724461746173657453706163696e67293b0a0a090909090972657475726e2028626173655769647468202f2064617461736574436f756e74293b0a090909097d0a0909097d293b0a0a090909746869732e6461746173657473203d205b5d3b0a0a0909092f2f53657420757020746f6f6c746970206576656e7473206f6e207468652063686172740a09090969662028746869732e6f7074696f6e732e73686f77546f6f6c74697073297b0a0909090968656c706572732e62696e644576656e747328746869732c20746869732e6f7074696f6e732e746f6f6c7469704576656e74732c2066756e6374696f6e28657674297b0a09090909097661722061637469766542617273203d20286576742e7479706520213d3d20276d6f7573656f75742729203f20746869732e6765744261727341744576656e742865767429203a205b5d3b0a0a0909090909746869732e65616368426172732866756e6374696f6e28626172297b0a0909090909096261722e726573746f7265285b2766696c6c436f6c6f72272c20277374726f6b65436f6c6f72275d293b0a09090909097d293b0a090909090968656c706572732e6561636828616374697665426172732c2066756e6374696f6e28616374697665426172297b0a090909090909616374697665"
              "4261722e66696c6c436f6c6f72203d206163746976654261722e686967686c6967687446696c6c3b0a0909090909096163746976654261722e7374726f6b65436f6c6f72203d206163746976654261722e686967686c696768745374726f6b653b0a09090909097d293b0a0909090909746869732e73686f77546f6f6c7469702861637469766542617273293b0a090909097d293b0a0909097d0a0a0909092f2f4465636c6172652074686520657874656e73696f6e206f66207468652064656661756c7420706f696e742c20746f20636174657220666f7220746865206f7074696f6e732070617373656420696e20746f2074686520636f6e7374727563746f720a090909746869732e426172436c617373203d2043686172742e52656374616e676c652e657874656e64287b0a090909097374726f6b655769647468203a20746869732e6f7074696f6e732e6261725374726f6b6557696474682c0a0909090973686f775374726f6b65203a20746869732e6f7074696f6e732e62617253686f775374726f6b652c0a09090909637478203a20746869732e63686172742e6374780a0909097d293b0a0a0909092f2f49746572617465207468726f7567682065616368206f66207468652064617461736574732c20616e64206275696c64207468697320696e746f20612070726f7065727479206f662074686520636861"
              "72740a09090968656c706572732e6561636828646174612e64617461736574732c66756e6374696f6e28646174617365742c64617461736574496e646578297b0a0a0909090976617220646174617365744f626a656374203d207b0a09090909096c6162656c203a20646174617365742e6c6162656c207c7c206e756c6c2c0a090909090966696c6c436f6c6f72203a20646174617365742e66696c6c436f6c6f722c0a09090909097374726f6b65436f6c6f72203a20646174617365742e7374726f6b65436f6c6f722c0a090909090962617273203a205b5d0a090909097d3b0a0a09090909746869732e64617461736574732e7075736828646174617365744f626a656374293b0a0a0909090968656c706572732e6561636828646174617365742e646174612c66756e6374696f6e2864617461506f696e742c696e646578297b0a09090909096966202868656c706572732e69734e756d6265722864617461506f696e7429297b0a0909090909092f2f4164642061206e657720706f696e7420666f722065616368207069656365206f6620646174612c2070617373696e6720616e79207265717569726564206461746120746f20647261772e0a090909090909646174617365744f626a6563742e626172732e70757368286e657720746869732e426172436c617373287b0a0909090909090976616c7565203a2064"
              "617461506f696e742c0a090909090909096c6162656c203a20646174612e6c6162656c735b696e6465785d2c0a09090909090909646174617365744c6162656c3a20646174617365742e6c6162656c2c0a090909090909097374726f6b65436f6c6f72203a20646174617365742e7374726f6b65436f6c6f722c0a0909090909090966696c6c436f6c6f72203a20646174617365742e66696c6c436f6c6f722c0a09090909090909686967686c6967687446696c6c203a20646174617365742e686967686c6967687446696c6c207c7c20646174617365742e66696c6c436f6c6f722c0a09090909090909686967686c696768745374726f6b65203a20646174617365742e686967686c696768745374726f6b65207c7c20646174617365742e7374726f6b65436f6c6f720a0909090909097d29293b0a09090909097d0a090909097d2c74686973293b0a0a0909097d2c74686973293b0a0a090909746869732e6275696c645363616c6528646174612e6c6162656c73293b0a0a090909746869732e426172436c6173732e70726f746f747970652e62617365203d20746869732e7363616c652e656e64506f696e743b0a0a090909746869732e65616368426172732866756e6374696f6e286261722c20696e6465782c2064617461736574496e646578297b0a0909090968656c706572732e657874656e64286261722c20"
              "7b0a09090909097769647468203a20746869732e7363616c652e63616c63756c617465426172576964746828746869732e64617461736574732e6c656e677468292c0a0909090909783a20746869732e7363616c652e63616c63756c6174654261725828746869732e64617461736574732e6c656e6774682c2064617461736574496e6465782c20696e646578292c0a0909090909793a20746869732e7363616c652e656e64506f696e740a090909097d293b0a090909096261722e7361766528293b0a0909097d2c2074686973293b0a0a090909746869732e72656e64657228293b0a09097d2c0a0909757064617465203a2066756e6374696f6e28297b0a090909746869732e7363616c652e75706461746528293b0a0909092f2f20526573657420616e7920686967686c6967687420636f6c6f757273206265666f7265207570646174696e672e0a09090968656c706572732e6561636828746869732e616374697665456c656d656e74732c2066756e6374696f6e28616374697665456c656d656e74297b0a09090909616374697665456c656d656e742e726573746f7265285b2766696c6c436f6c6f72272c20277374726f6b65436f6c6f72275d293b0a0909097d293b0a0a090909746869732e65616368426172732866756e6374696f6e28626172297b0a090909096261722e7361766528293b0a0909097d293b"
              "0a090909746869732e72656e64657228293b0a09097d2c0a09096561636842617273203a2066756e6374696f6e2863616c6c6261636b297b0a09090968656c706572732e6561636828746869732e64617461736574732c66756e6374696f6e28646174617365742c2064617461736574496e646578297b0a0909090968656c706572732e6561636828646174617365742e626172732c2063616c6c6261636b2c20746869732c2064617461736574496e646578293b0a0909097d2c74686973293b0a09097d2c0a09096765744261727341744576656e74203a2066756e6374696f6e2865297b0a09090976617220626172734172726179203d205b5d2c0a090909096576656e74506f736974696f6e203d2068656c706572732e67657452656c6174697665506f736974696f6e2865292c0a09090909646174617365744974657261746f72203d2066756e6374696f6e2864617461736574297b0a09090909096261727341727261792e7075736828646174617365742e626172735b626172496e6465785d293b0a090909097d2c0a09090909626172496e6465783b0a0a090909666f7220287661722064617461736574496e646578203d20303b2064617461736574496e646578203c20746869732e64617461736574732e6c656e6774683b2064617461736574496e6465782b2b29207b0a09090909666f72202862617249"
              "6e646578203d20303b20626172496e646578203c20746869732e64617461736574735b64617461736574496e6465785d2e626172732e6c656e6774683b20626172496e6465782b2b29207b0a090909090969662028746869732e64617461736574735b64617461736574496e6465785d2e626172735b626172496e6465785d2e696e52616e6765286576656e74506f736974696f6e2e782c6576656e74506f736974696f6e2e7929297b0a09090909090968656c706572732e6561636828746869732e64617461736574732c20646174617365744974657261746f72293b0a09090909090972657475726e206261727341727261793b0a09090909097d0a090909097d0a0909097d0a0a09090972657475726e206261727341727261793b0a09097d2c0a09096275696c645363616c65203a2066756e6374696f6e286c6162656c73297b0a0909097661722073656c66203d20746869733b0a0a0909097661722064617461546f74616c203d2066756e6374696f6e28297b0a090909097661722076616c756573203d205b5d3b0a0909090973656c662e65616368426172732866756e6374696f6e28626172297b0a090909090976616c7565732e70757368286261722e76616c7565293b0a090909097d293b0a0909090972657475726e2076616c7565733b0a0909097d3b0a0a090909766172207363616c654f7074696f6e"
              "73203d207b0a0909090974656d706c617465537472696e67203a20746869732e6f7074696f6e732e7363616c654c6162656c2c0a09090909686569676874203a20746869732e63686172742e6865696768742c0a090909097769647468203a20746869732e63686172742e77696474682c0a09090909637478203a20746869732e63686172742e6374782c0a0909090974657874436f6c6f72203a20746869732e6f7074696f6e732e7363616c65466f6e74436f6c6f722c0a09090909666f6e7453697a65203a20746869732e6f7074696f6e732e7363616c65466f6e7453697a652c0a09090909666f6e745374796c65203a20746869732e6f7074696f6e732e7363616c65466f6e745374796c652c0a09090909666f6e7446616d696c79203a20746869732e6f7074696f6e732e7363616c65466f6e7446616d696c792c0a0909090976616c756573436f756e74203a206c6162656c732e6c656e6774682c0a09090909626567696e41745a65726f203a20746869732e6f7074696f6e732e7363616c65426567696e41745a65726f2c0a09090909696e7465676572734f6e6c79203a20746869732e6f7074696f6e732e7363616c65496e7465676572734f6e6c792c0a0909090963616c63756c6174655952616e67653a2066756e6374696f6e2863757272656e74486569676874297b0a09090909097661722075706461"
              "74656452616e676573203d2068656c706572732e63616c63756c6174655363616c6552616e6765280a09090909090964617461546f74616c28292c0a09090909090963757272656e744865696768742c0a090909090909746869732e666f6e7453697a652c0a090909090909746869732e626567696e41745a65726f2c0a090909090909746869732e696e7465676572734f6e6c790a0909090909293b0a090909090968656c706572732e657874656e6428746869732c207570646174656452616e676573293b0a090909097d2c0a09090909784c6162656c73203a206c6162656c732c0a09090909666f6e74203a2068656c706572732e666f6e74537472696e6728746869732e6f7074696f6e732e7363616c65466f6e7453697a652c20746869732e6f7074696f6e732e7363616c65466f6e745374796c652c20746869732e6f7074696f6e732e7363616c65466f6e7446616d696c79292c0a090909096c696e655769647468203a20746869732e6f7074696f6e732e7363616c654c696e6557696474682c0a090909096c696e65436f6c6f72203a20746869732e6f7074696f6e732e7363616c654c696e65436f6c6f722c0a09090909677269644c696e655769647468203a2028746869732e6f7074696f6e732e7363616c6553686f77477269644c696e657329203f20746869732e6f7074696f6e732e7363616c6547"
              "7269644c696e655769647468203a20302c0a09090909677269644c696e65436f6c6f72203a2028746869732e6f7074696f6e732e7363616c6553686f77477269644c696e657329203f20746869732e6f7074696f6e732e7363616c65477269644c696e65436f6c6f72203a20227267626128302c302c302c3029222c0a0909090970616464696e67203a2028746869732e6f7074696f6e732e73686f775363616c6529203f2030203a2028746869732e6f7074696f6e732e62617253686f775374726f6b6529203f20746869732e6f7074696f6e732e6261725374726f6b655769647468203a20302c0a0909090973686f774c6162656c73203a20746869732e6f7074696f6e732e7363616c6553686f774c6162656c732c0a09090909646973706c6179203a20746869732e6f7074696f6e732e73686f775363616c650a0909097d3b0a0a09090969662028746869732e6f7074696f6e732e7363616c654f76657272696465297b0a0909090968656c706572732e657874656e64287363616c654f7074696f6e732c207b0a090909090963616c63756c6174655952616e67653a2068656c706572732e6e6f6f702c0a090909090973746570733a20746869732e6f7074696f6e732e7363616c6553746570732c0a09090909097374657056616c75653a20746869732e6f7074696f6e732e7363616c65537465705769647468"
              "2c0a09090909096d696e3a20746869732e6f7074696f6e732e7363616c65537461727456616c75652c0a09090909096d61783a20746869732e6f7074696f6e732e7363616c65537461727456616c7565202b2028746869732e6f7074696f6e732e7363616c655374657073202a20746869732e6f7074696f6e732e7363616c65537465705769647468290a090909097d293b0a0909097d0a0a090909746869732e7363616c65203d206e657720746869732e5363616c65436c617373287363616c654f7074696f6e73293b0a09097d2c0a090961646444617461203a2066756e6374696f6e2876616c75657341727261792c6c6162656c297b0a0909092f2f4d6170207468652076616c75657320617272617920666f722065616368206f66207468652064617461736574730a09090968656c706572732e656163682876616c75657341727261792c66756e6374696f6e2876616c75652c64617461736574496e646578297b0a09090909096966202868656c706572732e69734e756d6265722876616c756529297b0a0909090909092f2f4164642061206e657720706f696e7420666f722065616368207069656365206f6620646174612c2070617373696e6720616e79207265717569726564206461746120746f20647261772e0a090909090909746869732e64617461736574735b64617461736574496e6465785d2e62"
              "6172732e70757368286e657720746869732e426172436c617373287b0a0909090909090976616c7565203a2076616c75652c0a090909090909096c6162656c203a206c6162656c2c0a09090909090909783a20746869732e7363616c652e63616c63756c6174654261725828746869732e64617461736574732e6c656e6774682c2064617461736574496e6465782c20746869732e7363616c652e76616c756573436f756e742b31292c0a09090909090909793a20746869732e7363616c652e656e64506f696e742c0a090909090909097769647468203a20746869732e7363616c652e63616c63756c617465426172576964746828746869732e64617461736574732e6c656e677468292c0a0909090909090962617365203a20746869732e7363616c652e656e64506f696e742c0a090909090909097374726f6b65436f6c6f72203a20746869732e64617461736574735b64617461736574496e6465785d2e7374726f6b65436f6c6f722c0a0909090909090966696c6c436f6c6f72203a20746869732e64617461736574735b64617461736574496e6465785d2e66696c6c436f6c6f720a0909090909097d29293b0a09090909097d0a0909097d2c74686973293b0a0a090909746869732e7363616c652e616464584c6162656c286c6162656c293b0a0909092f2f5468656e2072652d72656e64657220746865206368"
              "6172742e0a090909746869732e75706461746528293b0a09097d2c0a090972656d6f766544617461203a2066756e6374696f6e28297b0a090909746869732e7363616c652e72656d6f7665584c6162656c28293b0a0909092f2f5468656e2072652d72656e646572207468652063686172742e0a09090968656c706572732e6561636828746869732e64617461736574732c66756e6374696f6e2864617461736574297b0a09090909646174617365742e626172732e736869667428293b0a0909097d2c74686973293b0a090909746869732e75706461746528293b0a09097d2c0a09097265666c6f77203a2066756e6374696f6e28297b0a09090968656c706572732e657874656e6428746869732e426172436c6173732e70726f746f747970652c7b0a09090909793a20746869732e7363616c652e656e64506f696e742c0a0909090962617365203a20746869732e7363616c652e656e64506f696e740a0909097d293b0a090909766172206e65775363616c6550726f7073203d2068656c706572732e657874656e64287b0a09090909686569676874203a20746869732e63686172742e6865696768742c0a090909097769647468203a20746869732e63686172742e77696474680a0909097d293b0a090909746869732e7363616c652e757064617465286e65775363616c6550726f7073293b0a09097d2c0a090964"
              "726177203a2066756e6374696f6e2865617365297b0a09090976617220656173696e67446563696d616c203d2065617365207c7c20313b0a090909746869732e636c65617228293b0a0a09090976617220637478203d20746869732e63686172742e6374783b0a0a090909746869732e7363616c652e6472617728656173696e67446563696d616c293b0a0a0909092f2f4472617720616c6c20746865206261727320666f72206561636820646174617365740a09090968656c706572732e6561636828746869732e64617461736574732c66756e6374696f6e28646174617365742c64617461736574496e646578297b0a0909090968656c706572732e6561636828646174617365742e626172732c66756e6374696f6e286261722c696e646578297b0a09090909096261722e62617365203d20746869732e7363616c652e656e64506f696e743b0a09090909092f2f5472616e736974696f6e207468656e20647261770a09090909096261722e7472616e736974696f6e287b0a09090909090978203a20746869732e7363616c652e63616c63756c6174654261725828746869732e64617461736574732e6c656e6774682c2064617461736574496e6465782c20696e646578292c0a09090909090979203a20746869732e7363616c652e63616c63756c61746559286261722e76616c7565292c0a090909090909776964"
              "7468203a20746869732e7363616c652e63616c63756c617465426172576964746828746869732e64617461736574732e6c656e677468290a09090909097d2c20656173696e67446563696d616c292e6472617728293b0a090909097d2c74686973293b0a0a0909097d2c74686973293b0a09097d0a097d293b0a0a0a7d292e63616c6c2874686973293b0a2866756e6374696f6e28297b0a092275736520737472696374223b0a0a0976617220726f6f74203d20746869732c0a09094368617274203d20726f6f742e43686172742c0a09092f2f43616368652061206c6f63616c207265666572656e636520746f2043686172742e68656c706572730a090968656c70657273203d2043686172742e68656c706572733b0a0a097661722064656661756c74436f6e666967203d207b0a09092f2f426f6f6c65616e202d20576865746865722077652073686f756c642073686f772061207374726f6b65206f6e2065616368207365676d656e740a09097365676d656e7453686f775374726f6b65203a20747275652c0a0a09092f2f537472696e67202d2054686520636f6c6f7572206f662065616368207365676d656e74207374726f6b650a09097365676d656e745374726f6b65436f6c6f72203a202223666666222c0a0a09092f2f4e756d626572202d20546865207769647468206f662065616368207365676d656e74"
              "207374726f6b650a09097365676d656e745374726f6b655769647468203a20322c0a0a09092f2f5468652070657263656e74616765206f6620746865206368617274207468617420776520637574206f7574206f6620746865206d6964646c652e0a090970657263656e74616765496e6e65724375746f7574203a2035302c0a0a09092f2f4e756d626572202d20416d6f756e74206f6620616e696d6174696f6e2073746570730a0909616e696d6174696f6e5374657073203a203130302c0a0a09092f2f537472696e67202d20416e696d6174696f6e20656173696e67206566666563740a0909616e696d6174696f6e456173696e67203a2022656173654f7574426f756e6365222c0a0a09092f2f426f6f6c65616e202d205768657468657220776520616e696d6174652074686520726f746174696f6e206f662074686520446f7567686e75740a0909616e696d617465526f74617465203a20747275652c0a0a09092f2f426f6f6c65616e202d205768657468657220776520616e696d617465207363616c696e672074686520446f7567686e75742066726f6d207468652063656e7472650a0909616e696d6174655363616c65203a2066616c73652c0a0a09092f2f537472696e67202d2041206c6567656e642074656d706c6174650a09096c6567656e6454656d706c617465203a20223c756c20636c6173733d5c"
              "223c253d6e616d652e746f4c6f776572436173652829253e2d6c6567656e645c223e3c2520666f72202876617220693d303b20693c7365676d656e74732e6c656e6774683b20692b2b297b253e3c6c693e3c7370616e207374796c653d5c226261636b67726f756e642d636f6c6f723a3c253d7365676d656e74735b695d2e66696c6c436f6c6f72253e5c223e3c2f7370616e3e3c256966287365676d656e74735b695d2e6c6162656c297b253e3c253d7365676d656e74735b695d2e6c6162656c253e3c257d253e3c2f6c693e3c257d253e3c2f756c3e220a0a097d3b0a0a0a0943686172742e547970652e657874656e64287b0a09092f2f50617373696e6720696e2061206e616d6520726567697374657273207468697320636861727420696e20746865204368617274206e616d6573706163650a09096e616d653a2022446f7567686e7574222c0a09092f2f50726f766964696e6720612064656661756c74732077696c6c20616c736f207265676973746572207468652064656166756c747320696e20746865206368617274206e616d6573706163650a090964656661756c7473203a2064656661756c74436f6e6669672c0a09092f2f496e697469616c697a65206973206669726564207768656e2074686520636861727420697320696e697469616c697a6564202d2044617461206973207061737365642069"
              "6e206173206120706172616d657465720a09092f2f436f6e666967206973206175746f6d61746963616c6c79206d65726765642062792074686520636f7265206f662043686172742e6a732c20616e6420697320617661696c61626c6520617420746869732e6f7074696f6e730a0909696e697469616c697a653a202066756e6374696f6e2864617461297b0a0a0909092f2f4465636c617265207365676d656e74732061732061207374617469632070726f706572747920746f2070726576656e7420696e6865726974696e67206163726f73732074686520436861727420747970652070726f746f747970650a090909746869732e7365676d656e7473203d205b5d3b0a090909746869732e6f75746572526164697573203d202868656c706572732e6d696e285b746869732e63686172742e77696474682c746869732e63686172742e6865696768745d29202d09746869732e6f7074696f6e732e7365676d656e745374726f6b6557696474682f32292f323b0a0a090909746869732e5365676d656e74417263203d2043686172742e4172632e657874656e64287b0a09090909637478203a20746869732e63686172742e6374782c0a0909090978203a20746869732e63686172742e77696474682f322c0a0909090979203a20746869732e63686172742e6865696768742f320a0909097d293b0a0a0909092f2f53"
              "657420757020746f6f6c746970206576656e7473206f6e207468652063686172740a09090969662028746869732e6f7074696f6e732e73686f77546f6f6c74697073297b0a0909090968656c706572732e62696e644576656e747328746869732c20746869732e6f7074696f6e732e746f6f6c7469704576656e74732c2066756e6374696f6e28657674297b0a0909090909766172206163746976655365676d656e7473203d20286576742e7479706520213d3d20276d6f7573656f75742729203f20746869732e6765745365676d656e747341744576656e742865767429203a205b5d3b0a0a090909090968656c706572732e6561636828746869732e7365676d656e74732c66756e6374696f6e287365676d656e74297b0a0909090909097365676d656e742e726573746f7265285b2266696c6c436f6c6f72225d293b0a09090909097d293b0a090909090968656c706572732e65616368286163746976655365676d656e74732c66756e6374696f6e286163746976655365676d656e74297b0a0909090909096163746976655365676d656e742e66696c6c436f6c6f72203d206163746976655365676d656e742e686967686c69676874436f6c6f723b0a09090909097d293b0a0909090909746869732e73686f77546f6f6c746970286163746976655365676d656e7473293b0a090909097d293b0a0909097d0a0909"
              "09746869732e63616c63756c617465546f74616c2864617461293b0a0a09090968656c706572732e6561636828646174612c66756e6374696f6e2864617461706f696e742c20696e646578297b0a09090909746869732e616464446174612864617461706f696e742c20696e6465782c2074727565293b0a0909097d2c74686973293b0a0a090909746869732e72656e64657228293b0a09097d2c0a09096765745365676d656e747341744576656e74203a2066756e6374696f6e2865297b0a090909766172207365676d656e74734172726179203d205b5d3b0a0a090909766172206c6f636174696f6e203d2068656c706572732e67657452656c6174697665506f736974696f6e2865293b0a0a09090968656c706572732e6561636828746869732e7365676d656e74732c66756e6374696f6e287365676d656e74297b0a09090909696620287365676d656e742e696e52616e6765286c6f636174696f6e2e782c6c6f636174696f6e2e792929207365676d656e747341727261792e70757368287365676d656e74293b0a0909097d2c74686973293b0a09090972657475726e207365676d656e747341727261793b0a09097d2c0a090961646444617461203a2066756e6374696f6e287365676d656e742c206174496e6465782c2073696c656e74297b0a09090976617220696e646578203d206174496e646578207c7c"
              "20746869732e7365676d656e74732e6c656e6774683b0a090909746869732e7365676d656e74732e73706c69636528696e6465782c20302c206e657720746869732e5365676d656e74417263287b0a0909090976616c7565203a207365676d656e742e76616c75652c0a090909096f75746572526164697573203a2028746869732e6f7074696f6e732e616e696d6174655363616c6529203f2030203a20746869732e6f757465725261646975732c0a09090909696e6e6572526164697573203a2028746869732e6f7074696f6e732e616e696d6174655363616c6529203f2030203a2028746869732e6f757465725261646975732f31303029202a20746869732e6f7074696f6e732e70657263656e74616765496e6e65724375746f75742c0a0909090966696c6c436f6c6f72203a207365676d656e742e636f6c6f722c0a09090909686967686c69676874436f6c6f72203a207365676d656e742e686967686c69676874207c7c207365676d656e742e636f6c6f722c0a0909090973686f775374726f6b65203a20746869732e6f7074696f6e732e7365676d656e7453686f775374726f6b652c0a090909097374726f6b655769647468203a20746869732e6f7074696f6e732e7365676d656e745374726f6b6557696474682c0a090909097374726f6b65436f6c6f72203a20746869732e6f7074696f6e732e7365676d"
              "656e745374726f6b65436f6c6f722c0a090909097374617274416e676c65203a204d6174682e5049202a20312e352c0a0909090963697263756d666572656e6365203a2028746869732e6f7074696f6e732e616e696d617465526f7461746529203f2030203a20746869732e63616c63756c61746543697263756d666572656e6365287365676d656e742e76616c7565292c0a090909096c6162656c203a207365676d656e742e6c6162656c0a0909097d29293b0a090909696620282173696c656e74297b0a09090909746869732e7265666c6f7728293b0a09090909746869732e75706461746528293b0a0909097d0a09097d2c0a090963616c63756c61746543697263756d666572656e6365203a2066756e6374696f6e2876616c7565297b0a09090972657475726e20284d6174682e50492a32292a2876616c7565202f20746869732e746f74616c293b0a09097d2c0a090963616c63756c617465546f74616c203a2066756e6374696f6e2864617461297b0a090909746869732e746f74616c203d20303b0a09090968656c706572732e6561636828646174612c66756e6374696f6e287365676d656e74297b0a09090909746869732e746f74616c202b3d207365676d656e742e76616c75653b0a0909097d2c74686973293b0a09097d2c0a0909757064617465203a2066756e6374696f6e28297b0a090909746869"
              "732e63616c63756c617465546f74616c28746869732e7365676d656e7473293b0a0a0909092f2f20526573657420616e7920686967686c6967687420636f6c6f757273206265666f7265207570646174696e672e0a09090968656c706572732e6561636828746869732e616374697665456c656d656e74732c2066756e6374696f6e28616374697665456c656d656e74297b0a09090909616374697665456c656d656e742e726573746f7265285b2766696c6c436f6c6f72275d293b0a0909097d293b0a0a09090968656c706572732e6561636828746869732e7365676d656e74732c66756e6374696f6e287365676d656e74297b0a090909097365676d656e742e7361766528293b0a0909097d293b0a090909746869732e72656e64657228293b0a09097d2c0a0a090972656d6f7665446174613a2066756e6374696f6e286174496e646578297b0a09090976617220696e646578546f44656c657465203d202868656c706572732e69734e756d626572286174496e6465782929203f206174496e646578203a20746869732e7365676d656e74732e6c656e6774682d313b0a090909746869732e7365676d656e74732e73706c69636528696e646578546f44656c6574652c2031293b0a090909746869732e7265666c6f7728293b0a090909746869732e75706461746528293b0a09097d2c0a0a09097265666c6f77203a"
              "2066756e6374696f6e28297b0a09090968656c706572732e657874656e6428746869732e5365676d656e744172632e70726f746f747970652c7b0a0909090978203a20746869732e63686172742e77696474682f322c0a0909090979203a20746869732e63686172742e6865696768742f320a0909097d293b0a090909746869732e6f75746572526164697573203d202868656c706572732e6d696e285b746869732e63686172742e77696474682c746869732e63686172742e6865696768745d29202d09746869732e6f7074696f6e732e7365676d656e745374726f6b6557696474682f32292f323b0a09090968656c706572732e6561636828746869732e7365676d656e74732c2066756e6374696f6e287365676d656e74297b0a090909097365676d656e742e757064617465287b0a09090909096f75746572526164697573203a20746869732e6f757465725261646975732c0a0909090909696e6e6572526164697573203a2028746869732e6f757465725261646975732f31303029202a20746869732e6f7074696f6e732e70657263656e74616765496e6e65724375746f75740a090909097d293b0a0909097d2c2074686973293b0a09097d2c0a090964726177203a2066756e6374696f6e2865617365446563696d616c297b0a09090976617220616e696d446563696d616c203d202865617365446563696d61"
              "6c29203f2065617365446563696d616c203a20313b0a090909746869732e636c65617228293b0a09090968656c706572732e6561636828746869732e7365676d656e74732c66756e6374696f6e287365676d656e742c696e646578297b0a090909097365676d656e742e7472616e736974696f6e287b0a090909090963697263756d666572656e6365203a20746869732e63616c63756c61746543697263756d666572656e6365287365676d656e742e76616c7565292c0a09090909096f75746572526164697573203a20746869732e6f757465725261646975732c0a0909090909696e6e6572526164697573203a2028746869732e6f757465725261646975732f31303029202a20746869732e6f7074696f6e732e70657263656e74616765496e6e65724375746f75740a090909097d2c616e696d446563696d616c293b0a0a090909097365676d656e742e656e64416e676c65203d207365676d656e742e7374617274416e676c65202b207365676d656e742e63697263756d666572656e63653b0a0a090909097365676d656e742e6472617728293b0a0909090969662028696e646578203d3d3d2030297b0a09090909097365676d656e742e7374617274416e676c65203d204d6174682e5049202a20312e353b0a090909097d0a090909092f2f436865636b20746f20736565206966206974277320746865206c6173"
              "74207365676d656e742c206966206e6f742067657420746865206e65787420616e64207570646174652074686520737461727420616e676c650a0909090969662028696e646578203c20746869732e7365676d656e74732e6c656e6774682d31297b0a0909090909746869732e7365676d656e74735b696e6465782b315d2e7374617274416e676c65203d207365676d656e742e656e64416e676c653b0a090909097d0a0909097d2c74686973293b0a0a09097d0a097d293b0a0a0943686172742e74797065732e446f7567686e75742e657874656e64287b0a09096e616d65203a2022506965222c0a090964656661756c7473203a2068656c706572732e6d657267652864656661756c74436f6e6669672c7b70657263656e74616765496e6e65724375746f7574203a20307d290a097d293b0a0a7d292e63616c6c2874686973293b0a2866756e6374696f6e28297b0a092275736520737472696374223b0a0a0976617220726f6f74203d20746869732c0a09094368617274203d20726f6f742e43686172742c0a090968656c70657273203d2043686172742e68656c706572733b0a0a097661722064656661756c74436f6e666967203d207b0a0a09092f2f2f426f6f6c65616e202d20576865746865722067726964206c696e6573206172652073686f776e206163726f7373207468652063686172740a0909736361"
              "6c6553686f77477269644c696e6573203a20747275652c0a0a09092f2f537472696e67202d20436f6c6f7572206f66207468652067726964206c696e65730a09097363616c65477269644c696e65436f6c6f72203a20227267626128302c302c302c2e303529222c0a0a09092f2f4e756d626572202d205769647468206f66207468652067726964206c696e65730a09097363616c65477269644c696e655769647468203a20312c0a0a09092f2f426f6f6c65616e202d205768657468657220746865206c696e6520697320637572766564206265747765656e20706f696e74730a090962657a6965724375727665203a20747275652c0a0a09092f2f4e756d626572202d2054656e73696f6e206f66207468652062657a696572206375727665206265747765656e20706f696e74730a090962657a696572437572766554656e73696f6e203a20302e342c0a0a09092f2f426f6f6c65616e202d205768657468657220746f2073686f77206120646f7420666f72206561636820706f696e740a0909706f696e74446f74203a20747275652c0a0a09092f2f4e756d626572202d20526164697573206f66206561636820706f696e7420646f7420696e20706978656c730a0909706f696e74446f74526164697573203a20342c0a0a09092f2f4e756d626572202d20506978656c207769647468206f6620706f696e7420646f"
              "74207374726f6b650a0909706f696e74446f745374726f6b655769647468203a20312c0a0a09092f2f4e756d626572202d20616d6f756e7420657874726120746f2061646420746f207468652072616469757320746f20636174657220666f722068697420646574656374696f6e206f7574736964652074686520647261776e20706f696e740a0909706f696e74486974446574656374696f6e526164697573203a2032302c0a0a09092f2f426f6f6c65616e202d205768657468657220746f2073686f772061207374726f6b6520666f722064617461736574730a0909646174617365745374726f6b65203a20747275652c0a0a09092f2f4e756d626572202d20506978656c207769647468206f662064617461736574207374726f6b650a0909646174617365745374726f6b655769647468203a20322c0a0a09092f2f426f6f6c65616e202d205768657468657220746f2066696c6c2074686520646174617365742077697468206120636f6c6f75720a09096461746173657446696c6c203a20747275652c0a0a09092f2f537472696e67202d2041206c6567656e642074656d706c6174650a09096c6567656e6454656d706c617465203a20223c756c20636c6173733d5c223c253d6e616d652e746f4c6f776572436173652829253e2d6c6567656e645c223e3c2520666f72202876617220693d303b20693c646174"
              "61736574732e6c656e6774683b20692b2b297b253e3c6c693e3c7370616e207374796c653d5c226261636b67726f756e642d636f6c6f723a3c253d64617461736574735b695d2e7374726f6b65436f6c6f72253e5c223e3c2f7370616e3e3c2569662864617461736574735b695d2e6c6162656c297b253e3c253d64617461736574735b695d2e6c6162656c253e3c257d253e3c2f6c693e3c257d253e3c2f756c3e220a0a097d3b0a0a0a0943686172742e547970652e657874656e64287b0a09096e616d653a20224c696e65222c0a090964656661756c7473203a2064656661756c74436f6e6669672c0a0909696e697469616c697a653a202066756e6374696f6e2864617461297b0a0909092f2f4465636c6172652074686520657874656e73696f6e206f66207468652064656661756c7420706f696e742c20746f20636174657220666f7220746865206f7074696f6e732070617373656420696e20746f2074686520636f6e7374727563746f720a090909746869732e506f696e74436c617373203d2043686172742e506f696e742e657874656e64287b0a090909097374726f6b655769647468203a20746869732e6f7074696f6e732e706f696e74446f745374726f6b6557696474682c0a09090909726164697573203a20746869732e6f7074696f6e732e706f696e74446f745261646975732c0a090909096469"
              "73706c61793a20746869732e6f7074696f6e732e706f696e74446f742c0a09090909686974446574656374696f6e526164697573203a20746869732e6f7074696f6e732e706f696e74486974446574656374696f6e5261646975732c0a09090909637478203a20746869732e63686172742e6374782c0a09090909696e52616e6765203a2066756e6374696f6e286d6f75736558297b0a090909090972657475726e20284d6174682e706f77286d6f757365582d746869732e782c203229203c204d6174682e706f7728746869732e726164697573202b20746869732e686974446574656374696f6e5261646975732c3229293b0a090909097d0a0909097d293b0a0a090909746869732e6461746173657473203d205b5d3b0a0a0909092f2f53657420757020746f6f6c746970206576656e7473206f6e207468652063686172740a09090969662028746869732e6f7074696f6e732e73686f77546f6f6c74697073297b0a0909090968656c706572732e62696e644576656e747328746869732c20746869732e6f7074696f6e732e746f6f6c7469704576656e74732c2066756e6374696f6e28657674297b0a090909090976617220616374697665506f696e7473203d20286576742e7479706520213d3d20276d6f7573656f75742729203f20746869732e676574506f696e747341744576656e742865767429203a205b"
              "5d3b0a0909090909746869732e65616368506f696e74732866756e6374696f6e28706f696e74297b0a090909090909706f696e742e726573746f7265285b2766696c6c436f6c6f72272c20277374726f6b65436f6c6f72275d293b0a09090909097d293b0a090909090968656c706572732e6561636828616374697665506f696e74732c2066756e6374696f6e28616374697665506f696e74297b0a090909090909616374697665506f696e742e66696c6c436f6c6f72203d20616374697665506f696e742e686967686c6967687446696c6c3b0a090909090909616374697665506f696e742e7374726f6b65436f6c6f72203d20616374697665506f696e742e686967686c696768745374726f6b653b0a09090909097d293b0a0909090909746869732e73686f77546f6f6c74697028616374697665506f696e7473293b0a090909097d293b0a0909097d0a0a0909092f2f49746572617465207468726f7567682065616368206f66207468652064617461736574732c20616e64206275696c64207468697320696e746f20612070726f7065727479206f66207468652063686172740a09090968656c706572732e6561636828646174612e64617461736574732c66756e6374696f6e2864617461736574297b0a0a0909090976617220646174617365744f626a656374203d207b0a09090909096c6162656c203a206461"
              "74617365742e6c6162656c207c7c206e756c6c2c0a090909090966696c6c436f6c6f72203a20646174617365742e66696c6c436f6c6f722c0a09090909097374726f6b65436f6c6f72203a20646174617365742e7374726f6b65436f6c6f722c0a0909090909706f696e74436f6c6f72203a20646174617365742e706f696e74436f6c6f722c0a0909090909706f696e745374726f6b65436f6c6f72203a20646174617365742e706f696e745374726f6b65436f6c6f722c0a0909090909706f696e7473203a205b5d0a090909097d3b0a0a09090909746869732e64617461736574732e7075736828646174617365744f626a656374293b0a0a0a0909090968656c706572732e6561636828646174617365742e646174612c66756e6374696f6e2864617461506f696e742c696e646578297b0a09090909092f2f426573742077617920746f20646f20746869733f206f7220696e20647261772073657175656e63652e2e2e3f0a09090909096966202868656c706572732e69734e756d6265722864617461506f696e7429297b0a09090909092f2f4164642061206e657720706f696e7420666f722065616368207069656365206f6620646174612c2070617373696e6720616e79207265717569726564206461746120746f20647261772e0a090909090909646174617365744f626a6563742e706f696e74732e70757368"
              "286e657720746869732e506f696e74436c617373287b0a0909090909090976616c7565203a2064617461506f696e742c0a090909090909096c6162656c203a20646174612e6c6162656c735b696e6465785d2c0a09090909090909646174617365744c6162656c3a20646174617365742e6c6162656c2c0a090909090909097374726f6b65436f6c6f72203a20646174617365742e706f696e745374726f6b65436f6c6f722c0a0909090909090966696c6c436f6c6f72203a20646174617365742e706f696e74436f6c6f722c0a09090909090909686967686c6967687446696c6c203a20646174617365742e706f696e74486967686c6967687446696c6c207c7c20646174617365742e706f696e74436f6c6f722c0a09090909090909686967686c696768745374726f6b65203a20646174617365742e706f696e74486967686c696768745374726f6b65207c7c20646174617365742e706f696e745374726f6b65436f6c6f720a0909090909097d29293b0a09090909097d0a090909097d2c74686973293b0a0a09090909746869732e6275696c645363616c6528646174612e6c6162656c73293b0a0a0a09090909746869732e65616368506f696e74732866756e6374696f6e28706f696e742c20696e646578297b0a090909090968656c706572732e657874656e6428706f696e742c207b0a090909090909783a2074"
              "6869732e7363616c652e63616c63756c6174655828696e646578292c0a090909090909793a20746869732e7363616c652e656e64506f696e740a09090909097d293b0a0909090909706f696e742e7361766528293b0a090909097d2c2074686973293b0a0a0909097d2c74686973293b0a0a0a090909746869732e72656e64657228293b0a09097d2c0a0909757064617465203a2066756e6374696f6e28297b0a090909746869732e7363616c652e75706461746528293b0a0909092f2f20526573657420616e7920686967686c6967687420636f6c6f757273206265666f7265207570646174696e672e0a09090968656c706572732e6561636828746869732e616374697665456c656d656e74732c2066756e6374696f6e28616374697665456c656d656e74297b0a09090909616374697665456c656d656e742e726573746f7265285b2766696c6c436f6c6f72272c20277374726f6b65436f6c6f72275d293b0a0909097d293b0a090909746869732e65616368506f696e74732866756e6374696f6e28706f696e74297b0a09090909706f696e742e7361766528293b0a0909097d293b0a090909746869732e72656e64657228293b0a09097d2c0a090965616368506f696e7473203a2066756e6374696f6e2863616c6c6261636b297b0a09090968656c706572732e6561636828746869732e64617461736574732c66"
              "756e6374696f6e2864617461736574297b0a0909090968656c706572732e6561636828646174617365742e706f696e74732c63616c6c6261636b2c74686973293b0a0909097d2c74686973293b0a09097d2c0a0909676574506f696e747341744576656e74203a2066756e6374696f6e2865297b0a09090976617220706f696e74734172726179203d205b5d2c0a090909096576656e74506f736974696f6e203d2068656c706572732e67657452656c6174697665506f736974696f6e2865293b0a09090968656c706572732e6561636828746869732e64617461736574732c66756e6374696f6e2864617461736574297b0a0909090968656c706572732e6561636828646174617365742e706f696e74732c66756e6374696f6e28706f696e74297b0a090909090969662028706f696e742e696e52616e6765286576656e74506f736974696f6e2e782c6576656e74506f736974696f6e2e79292920706f696e747341727261792e7075736828706f696e74293b0a090909097d293b0a0909097d2c74686973293b0a09090972657475726e20706f696e747341727261793b0a09097d2c0a09096275696c645363616c65203a2066756e6374696f6e286c6162656c73297b0a0909097661722073656c66203d20746869733b0a0a0909097661722064617461546f74616c203d2066756e6374696f6e28297b0a0909090976"
              "61722076616c756573203d205b5d3b0a0909090973656c662e65616368506f696e74732866756e6374696f6e28706f696e74297b0a090909090976616c7565732e7075736828706f696e742e76616c7565293b0a090909097d293b0a0a0909090972657475726e2076616c7565733b0a0909097d3b0a0a090909766172207363616c654f7074696f6e73203d207b0a0909090974656d706c617465537472696e67203a20746869732e6f7074696f6e732e7363616c654c6162656c2c0a09090909686569676874203a20746869732e63686172742e6865696768742c0a090909097769647468203a20746869732e63686172742e77696474682c0a09090909637478203a20746869732e63686172742e6374782c0a0909090974657874436f6c6f72203a20746869732e6f7074696f6e732e7363616c65466f6e74436f6c6f722c0a09090909666f6e7453697a65203a20746869732e6f7074696f6e732e7363616c65466f6e7453697a652c0a09090909666f6e745374796c65203a20746869732e6f7074696f6e732e7363616c65466f6e745374796c652c0a09090909666f6e7446616d696c79203a20746869732e6f7074696f6e732e7363616c65466f6e7446616d696c792c0a0909090976616c756573436f756e74203a206c6162656c732e6c656e6774682c0a09090909626567696e41745a65726f203a2074686973"
              "2e6f7074696f6e732e7363616c65426567696e41745a65726f2c0a09090909696e7465676572734f6e6c79203a20746869732e6f7074696f6e732e7363616c65496e7465676572734f6e6c792c0a0909090963616c63756c6174655952616e6765203a2066756e6374696f6e2863757272656e74486569676874297b0a0909090909766172207570646174656452616e676573203d2068656c706572732e63616c63756c6174655363616c6552616e6765280a09090909090964617461546f74616c28292c0a09090909090963757272656e744865696768742c0a090909090909746869732e666f6e7453697a652c0a090909090909746869732e626567696e41745a65726f2c0a090909090909746869732e696e7465676572734f6e6c790a0909090909293b0a090909090968656c706572732e657874656e6428746869732c207570646174656452616e676573293b0a090909097d2c0a09090909784c6162656c73203a206c6162656c732c0a09090909666f6e74203a2068656c706572732e666f6e74537472696e6728746869732e6f7074696f6e732e7363616c65466f6e7453697a652c20746869732e6f7074696f6e732e7363616c65466f6e745374796c652c20746869732e6f7074696f6e732e7363616c65466f6e7446616d696c79292c0a090909096c696e655769647468203a20746869732e6f7074696f6e"
              "732e7363616c654c696e6557696474682c0a090909096c696e65436f6c6f72203a20746869732e6f7074696f6e732e7363616c654c696e65436f6c6f722c0a09090909677269644c696e655769647468203a2028746869732e6f7074696f6e732e7363616c6553686f77477269644c696e657329203f20746869732e6f7074696f6e732e7363616c65477269644c696e655769647468203a20302c0a09090909677269644c696e65436f6c6f72203a2028746869732e6f7074696f6e732e7363616c6553686f77477269644c696e657329203f20746869732e6f7074696f6e732e7363616c65477269644c696e65436f6c6f72203a20227267626128302c302c302c3029222c0a0909090970616464696e673a2028746869732e6f7074696f6e732e73686f775363616c6529203f2030203a20746869732e6f7074696f6e732e706f696e74446f74526164697573202b20746869732e6f7074696f6e732e706f696e74446f745374726f6b6557696474682c0a0909090973686f774c6162656c73203a20746869732e6f7074696f6e732e7363616c6553686f774c6162656c732c0a09090909646973706c6179203a20746869732e6f7074696f6e732e73686f775363616c650a0909097d3b0a0a09090969662028746869732e6f7074696f6e732e7363616c654f76657272696465297b0a0909090968656c706572732e6578"
              "74656e64287363616c654f7074696f6e732c207b0a090909090963616c63756c6174655952616e67653a2068656c706572732e6e6f6f702c0a090909090973746570733a20746869732e6f7074696f6e732e7363616c6553746570732c0a09090909097374657056616c75653a20746869732e6f7074696f6e732e7363616c655374657057696474682c0a09090909096d696e3a20746869732e6f7074696f6e732e7363616c65537461727456616c75652c0a09090909096d61783a20746869732e6f7074696f6e732e7363616c65537461727456616c7565202b2028746869732e6f7074696f6e732e7363616c655374657073202a20746869732e6f7074696f6e732e7363616c65537465705769647468290a090909097d293b0a0909097d0a0a0a090909746869732e7363616c65203d206e65772043686172742e5363616c65287363616c654f7074696f6e73293b0a09097d2c0a090961646444617461203a2066756e6374696f6e2876616c75657341727261792c6c6162656c297b0a0909092f2f4d6170207468652076616c75657320617272617920666f722065616368206f66207468652064617461736574730a0a09090968656c706572732e656163682876616c75657341727261792c66756e6374696f6e2876616c75652c64617461736574496e646578297b0a09090909096966202868656c706572732e69"
              "734e756d6265722876616c756529297b0a09090909092f2f4164642061206e657720706f696e7420666f722065616368207069656365206f6620646174612c2070617373696e6720616e79207265717569726564206461746120746f20647261772e0a090909090909746869732e64617461736574735b64617461736574496e6465785d2e706f696e74732e70757368286e657720746869732e506f696e74436c617373287b0a0909090909090976616c7565203a2076616c75652c0a090909090909096c6162656c203a206c6162656c2c0a09090909090909783a20746869732e7363616c652e63616c63756c6174655828746869732e7363616c652e76616c756573436f756e742b31292c0a09090909090909793a20746869732e7363616c652e656e64506f696e742c0a090909090909097374726f6b65436f6c6f72203a20746869732e64617461736574735b64617461736574496e6465785d2e706f696e745374726f6b65436f6c6f722c0a0909090909090966696c6c436f6c6f72203a20746869732e64617461736574735b64617461736574496e6465785d2e706f696e74436f6c6f720a0909090909097d29293b0a09090909097d0a0909097d2c74686973293b0a0a090909746869732e7363616c652e616464584c6162656c286c6162656c293b0a0909092f2f5468656e2072652d72656e64657220746865"
              "2063686172742e0a090909746869732e75706461746528293b0a09097d2c0a090972656d6f766544617461203a2066756e6374696f6e28297b0a090909746869732e7363616c652e72656d6f7665584c6162656c28293b0a0909092f2f5468656e2072652d72656e646572207468652063686172742e0a09090968656c706572732e6561636828746869732e64617461736574732c66756e6374696f6e2864617461736574297b0a09090909646174617365742e706f696e74732e736869667428293b0a0909097d2c74686973293b0a090909746869732e75706461746528293b0a09097d2c0a09097265666c6f77203a2066756e6374696f6e28297b0a090909766172206e65775363616c6550726f7073203d2068656c706572732e657874656e64287b0a09090909686569676874203a20746869732e63686172742e6865696768742c0a090909097769647468203a20746869732e63686172742e77696474680a0909097d293b0a090909746869732e7363616c652e757064617465286e65775363616c6550726f7073293b0a09097d2c0a090964726177203a2066756e6374696f6e2865617365297b0a09090976617220656173696e67446563696d616c203d2065617365207c7c20313b0a090909746869732e636c65617228293b0a0a09090976617220637478203d20746869732e63686172742e6374783b0a0a09"
              "0909746869732e7363616c652e6472617728656173696e67446563696d616c293b0a0a0a09090968656c706572732e6561636828746869732e64617461736574732c66756e6374696f6e2864617461736574297b0a0a090909092f2f5472616e736974696f6e206561636820706f696e7420666972737420736f207468617420746865206c696e6520616e6420706f696e742064726177696e672069736e2774206f7574206f662073796e630a090909092f2f57652063616e207573652074686973206578747261206c6f6f7020746f2063616c63756c6174652074686520636f6e74726f6c20706f696e7473206f662074686973206461746173657420616c736f20696e2074686973206c6f6f700a0a0909090968656c706572732e6561636828646174617365742e706f696e74732c66756e6374696f6e28706f696e742c696e646578297b0a0909090909706f696e742e7472616e736974696f6e287b0a09090909090979203a20746869732e7363616c652e63616c63756c6174655928706f696e742e76616c7565292c0a09090909090978203a20746869732e7363616c652e63616c63756c6174655828696e646578290a09090909097d2c20656173696e67446563696d616c293b0a0a090909097d2c74686973293b0a0a0a090909092f2f20436f6e74726f6c20706f696e7473206e65656420746f206265206361"
              "6c63756c6174656420696e2061207365706572617465206c6f6f702c2062656361757365207765206e65656420746f206b6e6f77207468652063757272656e7420782f79206f662074686520706f696e740a090909092f2f205468697320776f756c6420636175736520697373756573207768656e207468657265206973206e6f20616e696d6174696f6e2c2062656361757365207468652079206f6620746865206e65787420706f696e7420776f756c6420626520302c20736f2062657a6965727320776f756c6420626520736b657765640a0909090969662028746869732e6f7074696f6e732e62657a6965724375727665297b0a090909090968656c706572732e6561636828646174617365742e706f696e74732c66756e6374696f6e28706f696e742c696e646578297b0a0909090909092f2f496620776527726520617420746865207374617274206f7220656e642c20776520646f6e2774206861766520612070726576696f75732f6e65787420706f696e740a0909090909092f2f42792073657474696e67207468652074656e73696f6e20746f203020686572652c207468652063757276652077696c6c207472616e736974696f6e20746f2073747261696768742061742074686520656e640a09090909090969662028696e646578203d3d3d2030297b0a09090909090909706f696e742e636f6e74726f6c"
              "506f696e7473203d2068656c706572732e73706c696e65437572766528706f696e742c706f696e742c646174617365742e706f696e74735b696e6465782b315d2c30293b0a0909090909097d0a090909090909656c73652069662028696e646578203e3d20646174617365742e706f696e74732e6c656e6774682d31297b0a09090909090909706f696e742e636f6e74726f6c506f696e7473203d2068656c706572732e73706c696e65437572766528646174617365742e706f696e74735b696e6465782d315d2c706f696e742c706f696e742c30293b0a0909090909097d0a090909090909656c73657b0a09090909090909706f696e742e636f6e74726f6c506f696e7473203d2068656c706572732e73706c696e65437572766528646174617365742e706f696e74735b696e6465782d315d2c706f696e742c646174617365742e706f696e74735b696e6465782b315d2c746869732e6f7074696f6e732e62657a696572437572766554656e73696f6e293b0a0909090909097d0a09090909097d2c74686973293b0a090909097d0a0a0a090909092f2f4472617720746865206c696e65206265747765656e20616c6c2074686520706f696e74730a090909096374782e6c696e655769647468203d20746869732e6f7074696f6e732e646174617365745374726f6b6557696474683b0a090909096374782e7374726f6b"
              "655374796c65203d20646174617365742e7374726f6b65436f6c6f723b0a090909096374782e626567696e5061746828293b0a0909090968656c706572732e6561636828646174617365742e706f696e74732c66756e6374696f6e28706f696e742c696e646578297b0a090909090969662028696e6465783e30297b0a090909090909696628746869732e6f7074696f6e732e62657a6965724375727665297b0a090909090909096374782e62657a6965724375727665546f280a0909090909090909646174617365742e706f696e74735b696e6465782d315d2e636f6e74726f6c506f696e74732e6f757465722e782c0a0909090909090909646174617365742e706f696e74735b696e6465782d315d2e636f6e74726f6c506f696e74732e6f757465722e792c0a0909090909090909706f696e742e636f6e74726f6c506f696e74732e696e6e65722e782c0a0909090909090909706f696e742e636f6e74726f6c506f696e74732e696e6e65722e792c0a0909090909090909706f696e742e782c0a0909090909090909706f696e742e790a09090909090909293b0a0909090909097d0a090909090909656c73657b0a090909090909096374782e6c696e65546f28706f696e742e782c706f696e742e79293b0a0909090909097d0a0a09090909097d0a0909090909656c73657b0a0909090909096374782e6d6f766554"
              "6f28706f696e742e782c706f696e742e79293b0a09090909097d0a090909097d2c74686973293b0a090909096374782e7374726f6b6528293b0a0a0a0909090969662028746869732e6f7074696f6e732e6461746173657446696c6c297b0a09090909092f2f526f756e64206f666620746865206c696e6520627920676f696e6720746f207468652062617365206f66207468652063686172742c206261636b20746f207468652073746172742c207468656e2066696c6c2e0a09090909096374782e6c696e65546f28646174617365742e706f696e74735b646174617365742e706f696e74732e6c656e6774682d315d2e782c20746869732e7363616c652e656e64506f696e74293b0a09090909096374782e6c696e65546f28746869732e7363616c652e63616c63756c617465582830292c20746869732e7363616c652e656e64506f696e74293b0a09090909096374782e66696c6c5374796c65203d20646174617365742e66696c6c436f6c6f723b0a09090909096374782e636c6f73655061746828293b0a09090909096374782e66696c6c28293b0a090909097d0a0a090909092f2f4e6f7720647261772074686520706f696e7473206f76657220746865206c696e650a090909092f2f41206c6974746c6520696e656666696369656e7420646f75626c65206c6f6f70696e672c20627574206265747465722074"
              "68616e20746865206c696e650a090909092f2f6c616767696e6720626568696e642074686520706f696e7420706f736974696f6e730a0909090968656c706572732e6561636828646174617365742e706f696e74732c66756e6374696f6e28706f696e74297b0a0909090909706f696e742e6472617728293b0a090909097d293b0a0a0909097d2c74686973293b0a09097d0a097d293b0a0a0a7d292e63616c6c2874686973293b0a2866756e6374696f6e28297b0a092275736520737472696374223b0a0a0976617220726f6f74203d20746869732c0a09094368617274203d20726f6f742e43686172742c0a09092f2f43616368652061206c6f63616c207265666572656e636520746f2043686172742e68656c706572730a090968656c70657273203d2043686172742e68656c706572733b0a0a097661722064656661756c74436f6e666967203d207b0a09092f2f426f6f6c65616e202d2053686f772061206261636b64726f7020746f20746865207363616c65206c6162656c0a09097363616c6553686f774c6162656c4261636b64726f70203a20747275652c0a0a09092f2f537472696e67202d2054686520636f6c6f7572206f6620746865206c6162656c206261636b64726f700a09097363616c654261636b64726f70436f6c6f72203a202272676261283235352c3235352c3235352c302e373529222c0a"
              "0a09092f2f20426f6f6c65616e202d205768657468657220746865207363616c652073686f756c6420626567696e206174207a65726f0a09097363616c65426567696e41745a65726f203a20747275652c0a0a09092f2f4e756d626572202d20546865206261636b64726f702070616464696e672061626f766520262062656c6f7720746865206c6162656c20696e20706978656c730a09097363616c654261636b64726f7050616464696e6759203a20322c0a0a09092f2f4e756d626572202d20546865206261636b64726f702070616464696e6720746f207468652073696465206f6620746865206c6162656c20696e20706978656c730a09097363616c654261636b64726f7050616464696e6758203a20322c0a0a09092f2f426f6f6c65616e202d2053686f77206c696e6520666f7220656163682076616c756520696e20746865207363616c650a09097363616c6553686f774c696e65203a20747275652c0a0a09092f2f426f6f6c65616e202d205374726f6b652061206c696e652061726f756e642065616368207365676d656e7420696e207468652063686172740a09097365676d656e7453686f775374726f6b65203a20747275652c0a0a09092f2f537472696e67202d2054686520636f6c6f7572206f6620746865207374726f6b65206f6e206561636820736567656d656e742e0a09097365676d656e74"
              "5374726f6b65436f6c6f72203a202223666666222c0a0a09092f2f4e756d626572202d20546865207769647468206f6620746865207374726f6b652076616c756520696e20706978656c730a09097365676d656e745374726f6b655769647468203a20322c0a0a09092f2f4e756d626572202d20416d6f756e74206f6620616e696d6174696f6e2073746570730a0909616e696d6174696f6e5374657073203a203130302c0a0a09092f2f537472696e67202d20416e696d6174696f6e20656173696e67206566666563742e0a0909616e696d6174696f6e456173696e67203a2022656173654f7574426f756e6365222c0a0a09092f2f426f6f6c65616e202d205768657468657220746f20616e696d6174652074686520726f746174696f6e206f66207468652063686172740a0909616e696d617465526f74617465203a20747275652c0a0a09092f2f426f6f6c65616e202d205768657468657220746f20616e696d617465207363616c696e67207468652063686172742066726f6d207468652063656e7472650a0909616e696d6174655363616c65203a2066616c73652c0a0a09092f2f537472696e67202d2041206c6567656e642074656d706c6174650a09096c6567656e6454656d706c617465203a20223c756c20636c6173733d5c223c253d6e616d652e746f4c6f776572436173652829253e2d6c6567656e64"
              "5c223e3c2520666f72202876617220693d303b20693c7365676d656e74732e6c656e6774683b20692b2b297b253e3c6c693e3c7370616e207374796c653d5c226261636b67726f756e642d636f6c6f723a3c253d7365676d656e74735b695d2e66696c6c436f6c6f72253e5c223e3c2f7370616e3e3c256966287365676d656e74735b695d2e6c6162656c297b253e3c253d7365676d656e74735b695d2e6c6162656c253e3c257d253e3c2f6c693e3c257d253e3c2f756c3e220a097d3b0a0a0a0943686172742e547970652e657874656e64287b0a09092f2f50617373696e6720696e2061206e616d6520726567697374657273207468697320636861727420696e20746865204368617274206e616d6573706163650a09096e616d653a2022506f6c617241726561222c0a09092f2f50726f766964696e6720612064656661756c74732077696c6c20616c736f207265676973746572207468652064656166756c747320696e20746865206368617274206e616d6573706163650a090964656661756c7473203a2064656661756c74436f6e6669672c0a09092f2f496e697469616c697a65206973206669726564207768656e2074686520636861727420697320696e697469616c697a6564202d20446174612069732070617373656420696e206173206120706172616d657465720a09092f2f436f6e66696720697320"
              "6175746f6d61746963616c6c79206d65726765642062792074686520636f7265206f662043686172742e6a732c20616e6420697320617661696c61626c6520617420746869732e6f7074696f6e730a0909696e697469616c697a653a202066756e6374696f6e2864617461297b0a090909746869732e7365676d656e7473203d205b5d3b0a0909092f2f4465636c617265207365676d656e7420636c617373206173206120636861727420696e7374616e636520737065636966696320636c6173732c20736f2069742063616e2073686172652070726f707320666f72207468697320696e7374616e63650a090909746869732e5365676d656e74417263203d2043686172742e4172632e657874656e64287b0a0909090973686f775374726f6b65203a20746869732e6f7074696f6e732e7365676d656e7453686f775374726f6b652c0a090909097374726f6b655769647468203a20746869732e6f7074696f6e732e7365676d656e745374726f6b6557696474682c0a090909097374726f6b65436f6c6f72203a20746869732e6f7074696f6e732e7365676d656e745374726f6b65436f6c6f722c0a09090909637478203a20746869732e63686172742e6374782c0a09090909696e6e6572526164697573203a20302c0a0909090978203a20746869732e63686172742e77696474682f322c0a0909090979203a207468"
              "69732e63686172742e6865696768742f320a0909097d293b0a090909746869732e7363616c65203d206e65772043686172742e52616469616c5363616c65287b0a09090909646973706c61793a20746869732e6f7074696f6e732e73686f775363616c652c0a09090909666f6e745374796c653a20746869732e6f7074696f6e732e7363616c65466f6e745374796c652c0a09090909666f6e7453697a653a20746869732e6f7074696f6e732e7363616c65466f6e7453697a652c0a09090909666f6e7446616d696c793a20746869732e6f7074696f6e732e7363616c65466f6e7446616d696c792c0a09090909666f6e74436f6c6f723a20746869732e6f7074696f6e732e7363616c65466f6e74436f6c6f722c0a0909090973686f774c6162656c733a20746869732e6f7074696f6e732e7363616c6553686f774c6162656c732c0a0909090973686f774c6162656c4261636b64726f703a20746869732e6f7074696f6e732e7363616c6553686f774c6162656c4261636b64726f702c0a090909096261636b64726f70436f6c6f723a20746869732e6f7074696f6e732e7363616c654261636b64726f70436f6c6f722c0a090909096261636b64726f7050616464696e6759203a20746869732e6f7074696f6e732e7363616c654261636b64726f7050616464696e67592c0a090909096261636b64726f705061646469"
              "6e67583a20746869732e6f7074696f6e732e7363616c654261636b64726f7050616464696e67582c0a090909096c696e6557696474683a2028746869732e6f7074696f6e732e7363616c6553686f774c696e6529203f20746869732e6f7074696f6e732e7363616c654c696e655769647468203a20302c0a090909096c696e65436f6c6f723a20746869732e6f7074696f6e732e7363616c654c696e65436f6c6f722c0a090909096c696e654172633a20747275652c0a0909090977696474683a20746869732e63686172742e77696474682c0a090909096865696768743a20746869732e63686172742e6865696768742c0a090909097843656e7465723a20746869732e63686172742e77696474682f322c0a090909097943656e7465723a20746869732e63686172742e6865696768742f322c0a09090909637478203a20746869732e63686172742e6374782c0a0909090974656d706c617465537472696e673a20746869732e6f7074696f6e732e7363616c654c6162656c2c0a0909090976616c756573436f756e743a20646174612e6c656e6774680a0909097d293b0a0a090909746869732e7570646174655363616c6552616e67652864617461293b0a0a090909746869732e7363616c652e75706461746528293b0a0a09090968656c706572732e6561636828646174612c66756e6374696f6e287365676d656e"
              "742c696e646578297b0a09090909746869732e61646444617461287365676d656e742c696e6465782c74727565293b0a0909097d2c74686973293b0a0a0909092f2f53657420757020746f6f6c746970206576656e7473206f6e207468652063686172740a09090969662028746869732e6f7074696f6e732e73686f77546f6f6c74697073297b0a0909090968656c706572732e62696e644576656e747328746869732c20746869732e6f7074696f6e732e746f6f6c7469704576656e74732c2066756e6374696f6e28657674297b0a0909090909766172206163746976655365676d656e7473203d20286576742e7479706520213d3d20276d6f7573656f75742729203f20746869732e6765745365676d656e747341744576656e742865767429203a205b5d3b0a090909090968656c706572732e6561636828746869732e7365676d656e74732c66756e6374696f6e287365676d656e74297b0a0909090909097365676d656e742e726573746f7265285b2266696c6c436f6c6f72225d293b0a09090909097d293b0a090909090968656c706572732e65616368286163746976655365676d656e74732c66756e6374696f6e286163746976655365676d656e74297b0a0909090909096163746976655365676d656e742e66696c6c436f6c6f72203d206163746976655365676d656e742e686967686c69676874436f6c6f"
              "723b0a09090909097d293b0a0909090909746869732e73686f77546f6f6c746970286163746976655365676d656e7473293b0a090909097d293b0a0909097d0a0a090909746869732e72656e64657228293b0a09097d2c0a09096765745365676d656e747341744576656e74203a2066756e6374696f6e2865297b0a090909766172207365676d656e74734172726179203d205b5d3b0a0a090909766172206c6f636174696f6e203d2068656c706572732e67657452656c6174697665506f736974696f6e2865293b0a0a09090968656c706572732e6561636828746869732e7365676d656e74732c66756e6374696f6e287365676d656e74297b0a09090909696620287365676d656e742e696e52616e6765286c6f636174696f6e2e782c6c6f636174696f6e2e792929207365676d656e747341727261792e70757368287365676d656e74293b0a0909097d2c74686973293b0a09090972657475726e207365676d656e747341727261793b0a09097d2c0a090961646444617461203a2066756e6374696f6e287365676d656e742c206174496e6465782c2073696c656e74297b0a09090976617220696e646578203d206174496e646578207c7c20746869732e7365676d656e74732e6c656e6774683b0a0a090909746869732e7365676d656e74732e73706c69636528696e6465782c20302c206e657720746869732e53"
              "65676d656e74417263287b0a0909090966696c6c436f6c6f723a207365676d656e742e636f6c6f722c0a09090909686967686c69676874436f6c6f723a207365676d656e742e686967686c69676874207c7c207365676d656e742e636f6c6f722c0a090909096c6162656c3a207365676d656e742e6c6162656c2c0a0909090976616c75653a207365676d656e742e76616c75652c0a090909096f757465725261646975733a2028746869732e6f7074696f6e732e616e696d6174655363616c6529203f2030203a20746869732e7363616c652e63616c63756c61746543656e7465724f6666736574287365676d656e742e76616c7565292c0a0909090963697263756d666572656e63653a2028746869732e6f7074696f6e732e616e696d617465526f7461746529203f2030203a20746869732e7363616c652e67657443697263756d666572656e636528292c0a090909097374617274416e676c653a204d6174682e5049202a20312e350a0909097d29293b0a090909696620282173696c656e74297b0a09090909746869732e7265666c6f7728293b0a09090909746869732e75706461746528293b0a0909097d0a09097d2c0a090972656d6f7665446174613a2066756e6374696f6e286174496e646578297b0a09090976617220696e646578546f44656c657465203d202868656c706572732e69734e756d62657228"
              "6174496e6465782929203f206174496e646578203a20746869732e7365676d656e74732e6c656e6774682d313b0a090909746869732e7365676d656e74732e73706c69636528696e646578546f44656c6574652c2031293b0a090909746869732e7265666c6f7728293b0a090909746869732e75706461746528293b0a09097d2c0a090963616c63756c617465546f74616c3a2066756e6374696f6e2864617461297b0a090909746869732e746f74616c203d20303b0a09090968656c706572732e6561636828646174612c66756e6374696f6e287365676d656e74297b0a09090909746869732e746f74616c202b3d207365676d656e742e76616c75653b0a0909097d2c74686973293b0a090909746869732e7363616c652e76616c756573436f756e74203d20746869732e7365676d656e74732e6c656e6774683b0a09097d2c0a09097570646174655363616c6552616e67653a2066756e6374696f6e2864617461706f696e7473297b0a0909097661722076616c7565734172726179203d205b5d3b0a09090968656c706572732e656163682864617461706f696e74732c66756e6374696f6e287365676d656e74297b0a0909090976616c75657341727261792e70757368287365676d656e742e76616c7565293b0a0909097d293b0a0a090909766172207363616c6553697a6573203d2028746869732e6f7074696f"
              "6e732e7363616c654f7665727269646529203f0a090909097b0a090909090973746570733a20746869732e6f7074696f6e732e7363616c6553746570732c0a09090909097374657056616c75653a20746869732e6f7074696f6e732e7363616c655374657057696474682c0a09090909096d696e3a20746869732e6f7074696f6e732e7363616c65537461727456616c75652c0a09090909096d61783a20746869732e6f7074696f6e732e7363616c65537461727456616c7565202b2028746869732e6f7074696f6e732e7363616c655374657073202a20746869732e6f7074696f6e732e7363616c65537465705769647468290a090909097d203a0a0909090968656c706572732e63616c63756c6174655363616c6552616e6765280a090909090976616c75657341727261792c0a090909090968656c706572732e6d696e285b746869732e63686172742e77696474682c20746869732e63686172742e6865696768745d292f322c0a0909090909746869732e6f7074696f6e732e7363616c65466f6e7453697a652c0a0909090909746869732e6f7074696f6e732e7363616c65426567696e41745a65726f2c0a0909090909746869732e6f7074696f6e732e7363616c65496e7465676572734f6e6c790a09090909293b0a0a09090968656c706572732e657874656e64280a09090909746869732e7363616c652c0a09"
              "0909097363616c6553697a65732c0a090909097b0a090909090973697a653a2068656c706572732e6d696e285b746869732e63686172742e77696474682c20746869732e63686172742e6865696768745d292c0a09090909097843656e7465723a20746869732e63686172742e77696474682f322c0a09090909097943656e7465723a20746869732e63686172742e6865696768742f320a090909097d0a090909293b0a0a09097d2c0a0909757064617465203a2066756e6374696f6e28297b0a090909746869732e63616c63756c617465546f74616c28746869732e7365676d656e7473293b0a0a09090968656c706572732e6561636828746869732e7365676d656e74732c66756e6374696f6e287365676d656e74297b0a090909097365676d656e742e7361766528293b0a0909097d293b0a090909746869732e72656e64657228293b0a09097d2c0a09097265666c6f77203a2066756e6374696f6e28297b0a09090968656c706572732e657874656e6428746869732e5365676d656e744172632e70726f746f747970652c7b0a0909090978203a20746869732e63686172742e77696474682f322c0a0909090979203a20746869732e63686172742e6865696768742f320a0909097d293b0a090909746869732e7570646174655363616c6552616e676528746869732e7365676d656e7473293b0a09090974686973"
              "2e7363616c652e75706461746528293b0a0a09090968656c706572732e657874656e6428746869732e7363616c652c7b0a090909097843656e7465723a20746869732e63686172742e77696474682f322c0a090909097943656e7465723a20746869732e63686172742e6865696768742f320a0909097d293b0a0a09090968656c706572732e6561636828746869732e7365676d656e74732c2066756e6374696f6e287365676d656e74297b0a090909097365676d656e742e757064617465287b0a09090909096f75746572526164697573203a20746869732e7363616c652e63616c63756c61746543656e7465724f6666736574287365676d656e742e76616c7565290a090909097d293b0a0909097d2c2074686973293b0a0a09097d2c0a090964726177203a2066756e6374696f6e2865617365297b0a09090976617220656173696e67446563696d616c203d2065617365207c7c20313b0a0909092f2f436c65617220262064726177207468652063616e7661730a090909746869732e636c65617228293b0a09090968656c706572732e6561636828746869732e7365676d656e74732c66756e6374696f6e287365676d656e742c20696e646578297b0a090909097365676d656e742e7472616e736974696f6e287b0a090909090963697263756d666572656e6365203a20746869732e7363616c652e676574436972"
              "63756d666572656e636528292c0a09090909096f75746572526164697573203a20746869732e7363616c652e63616c63756c61746543656e7465724f6666736574287365676d656e742e76616c7565290a090909097d2c656173696e67446563696d616c293b0a0a090909097365676d656e742e656e64416e676c65203d207365676d656e742e7374617274416e676c65202b207365676d656e742e63697263756d666572656e63653b0a0a090909092f2f2049662077652776652072656d6f76656420746865206669727374207365676d656e74207765206e65656420746f2073657420746865206669727374206f6e6520746f0a090909092f2f2073746172742061742074686520746f702e0a0909090969662028696e646578203d3d3d2030297b0a09090909097365676d656e742e7374617274416e676c65203d204d6174682e5049202a20312e353b0a090909097d0a0a090909092f2f436865636b20746f20736565206966206974277320746865206c617374207365676d656e742c206966206e6f742067657420746865206e65787420616e64207570646174652074686520737461727420616e676c650a0909090969662028696e646578203c20746869732e7365676d656e74732e6c656e677468202d2031297b0a0909090909746869732e7365676d656e74735b696e6465782b315d2e7374617274416e67"
              "6c65203d207365676d656e742e656e64416e676c653b0a090909097d0a090909097365676d656e742e6472617728293b0a0909097d2c2074686973293b0a090909746869732e7363616c652e6472617728293b0a09097d0a097d293b0a0a7d292e63616c6c2874686973293b0a2866756e6374696f6e28297b0a092275736520737472696374223b0a0a0976617220726f6f74203d20746869732c0a09094368617274203d20726f6f742e43686172742c0a090968656c70657273203d2043686172742e68656c706572733b0a0a0a0a0943686172742e547970652e657874656e64287b0a09096e616d653a20225261646172222c0a090964656661756c74733a7b0a0909092f2f426f6f6c65616e202d205768657468657220746f2073686f77206c696e657320666f722065616368207363616c6520706f696e740a0909097363616c6553686f774c696e65203a20747275652c0a0a0909092f2f426f6f6c65616e202d20576865746865722077652073686f772074686520616e676c65206c696e6573206f7574206f66207468652072616461720a090909616e676c6553686f774c696e654f7574203a20747275652c0a0a0909092f2f426f6f6c65616e202d205768657468657220746f2073686f77206c6162656c73206f6e20746865207363616c650a0909097363616c6553686f774c6162656c73203a2066616c73"
              "652c0a0a0909092f2f20426f6f6c65616e202d205768657468657220746865207363616c652073686f756c6420626567696e206174207a65726f0a0909097363616c65426567696e41745a65726f203a20747275652c0a0a0909092f2f537472696e67202d20436f6c6f7572206f662074686520616e676c65206c696e650a090909616e676c654c696e65436f6c6f72203a20227267626128302c302c302c2e3129222c0a0a0909092f2f4e756d626572202d20506978656c207769647468206f662074686520616e676c65206c696e650a090909616e676c654c696e655769647468203a20312c0a0a0909092f2f537472696e67202d20506f696e74206c6162656c20666f6e74206465636c61726174696f6e0a090909706f696e744c6162656c466f6e7446616d696c79203a202227417269616c27222c0a0a0909092f2f537472696e67202d20506f696e74206c6162656c20666f6e74207765696768740a090909706f696e744c6162656c466f6e745374796c65203a20226e6f726d616c222c0a0a0909092f2f4e756d626572202d20506f696e74206c6162656c20666f6e742073697a6520696e20706978656c730a090909706f696e744c6162656c466f6e7453697a65203a2031302c0a0a0909092f2f537472696e67202d20506f696e74206c6162656c20666f6e7420636f6c6f75720a090909706f696e744c61"
              "62656c466f6e74436f6c6f72203a202223363636222c0a0a0909092f2f426f6f6c65616e202d205768657468657220746f2073686f77206120646f7420666f72206561636820706f696e740a090909706f696e74446f74203a20747275652c0a0a0909092f2f4e756d626572202d20526164697573206f66206561636820706f696e7420646f7420696e20706978656c730a090909706f696e74446f74526164697573203a20332c0a0a0909092f2f4e756d626572202d20506978656c207769647468206f6620706f696e7420646f74207374726f6b650a090909706f696e74446f745374726f6b655769647468203a20312c0a0a0909092f2f4e756d626572202d20616d6f756e7420657874726120746f2061646420746f207468652072616469757320746f20636174657220666f722068697420646574656374696f6e206f7574736964652074686520647261776e20706f696e740a090909706f696e74486974446574656374696f6e526164697573203a2032302c0a0a0909092f2f426f6f6c65616e202d205768657468657220746f2073686f772061207374726f6b6520666f722064617461736574730a090909646174617365745374726f6b65203a20747275652c0a0a0909092f2f4e756d626572202d20506978656c207769647468206f662064617461736574207374726f6b650a0909096461746173657453"
              "74726f6b655769647468203a20322c0a0a0909092f2f426f6f6c65616e202d205768657468657220746f2066696c6c2074686520646174617365742077697468206120636f6c6f75720a0909096461746173657446696c6c203a20747275652c0a0a0909092f2f537472696e67202d2041206c6567656e642074656d706c6174650a0909096c6567656e6454656d706c617465203a20223c756c20636c6173733d5c223c253d6e616d652e746f4c6f776572436173652829253e2d6c6567656e645c223e3c2520666f72202876617220693d303b20693c64617461736574732e6c656e6774683b20692b2b297b253e3c6c693e3c7370616e207374796c653d5c226261636b67726f756e642d636f6c6f723a3c253d64617461736574735b695d2e7374726f6b65436f6c6f72253e5c223e3c2f7370616e3e3c2569662864617461736574735b695d2e6c6162656c297b253e3c253d64617461736574735b695d2e6c6162656c253e3c257d253e3c2f6c693e3c257d253e3c2f756c3e220a0a09097d2c0a0a0909696e697469616c697a653a2066756e6374696f6e2864617461297b0a090909746869732e506f696e74436c617373203d2043686172742e506f696e742e657874656e64287b0a090909097374726f6b655769647468203a20746869732e6f7074696f6e732e706f696e74446f745374726f6b6557696474682c"
              "0a09090909726164697573203a20746869732e6f7074696f6e732e706f696e74446f745261646975732c0a09090909646973706c61793a20746869732e6f7074696f6e732e706f696e74446f742c0a09090909686974446574656374696f6e526164697573203a20746869732e6f7074696f6e732e706f696e74486974446574656374696f6e5261646975732c0a09090909637478203a20746869732e63686172742e6374780a0909097d293b0a0a090909746869732e6461746173657473203d205b5d3b0a0a090909746869732e6275696c645363616c652864617461293b0a0a0909092f2f53657420757020746f6f6c746970206576656e7473206f6e207468652063686172740a09090969662028746869732e6f7074696f6e732e73686f77546f6f6c74697073297b0a0909090968656c706572732e62696e644576656e747328746869732c20746869732e6f7074696f6e732e746f6f6c7469704576656e74732c2066756e6374696f6e28657674297b0a090909090976617220616374697665506f696e7473436f6c6c656374696f6e203d20286576742e7479706520213d3d20276d6f7573656f75742729203f20746869732e676574506f696e747341744576656e742865767429203a205b5d3b0a0a0909090909746869732e65616368506f696e74732866756e6374696f6e28706f696e74297b0a0909090909"
              "09706f696e742e726573746f7265285b2766696c6c436f6c6f72272c20277374726f6b65436f6c6f72275d293b0a09090909097d293b0a090909090968656c706572732e6561636828616374697665506f696e7473436f6c6c656374696f6e2c2066756e6374696f6e28616374697665506f696e74297b0a090909090909616374697665506f696e742e66696c6c436f6c6f72203d20616374697665506f696e742e686967686c6967687446696c6c3b0a090909090909616374697665506f696e742e7374726f6b65436f6c6f72203d20616374697665506f696e742e686967686c696768745374726f6b653b0a09090909097d293b0a0a0909090909746869732e73686f77546f6f6c74697028616374697665506f696e7473436f6c6c656374696f6e293b0a090909097d293b0a0909097d0a0a0909092f2f49746572617465207468726f7567682065616368206f66207468652064617461736574732c20616e64206275696c64207468697320696e746f20612070726f7065727479206f66207468652063686172740a09090968656c706572732e6561636828646174612e64617461736574732c66756e6374696f6e2864617461736574297b0a0a0909090976617220646174617365744f626a656374203d207b0a09090909096c6162656c3a20646174617365742e6c6162656c207c7c206e756c6c2c0a0909090909"
              "66696c6c436f6c6f72203a20646174617365742e66696c6c436f6c6f722c0a09090909097374726f6b65436f6c6f72203a20646174617365742e7374726f6b65436f6c6f722c0a0909090909706f696e74436f6c6f72203a20646174617365742e706f696e74436f6c6f722c0a0909090909706f696e745374726f6b65436f6c6f72203a20646174617365742e706f696e745374726f6b65436f6c6f722c0a0909090909706f696e7473203a205b5d0a090909097d3b0a0a09090909746869732e64617461736574732e7075736828646174617365744f626a656374293b0a0a0909090968656c706572732e6561636828646174617365742e646174612c66756e6374696f6e2864617461506f696e742c696e646578297b0a09090909092f2f426573742077617920746f20646f20746869733f206f7220696e20647261772073657175656e63652e2e2e3f0a09090909096966202868656c706572732e69734e756d6265722864617461506f696e7429297b0a09090909092f2f4164642061206e657720706f696e7420666f722065616368207069656365206f6620646174612c2070617373696e6720616e79207265717569726564206461746120746f20647261772e0a09090909090976617220706f696e74506f736974696f6e3b0a0909090909096966202821746869732e7363616c652e616e696d6174696f6e297b"
              "0a09090909090909706f696e74506f736974696f6e203d20746869732e7363616c652e676574506f696e74506f736974696f6e28696e6465782c20746869732e7363616c652e63616c63756c61746543656e7465724f66667365742864617461506f696e7429293b0a0909090909097d0a090909090909646174617365744f626a6563742e706f696e74732e70757368286e657720746869732e506f696e74436c617373287b0a0909090909090976616c7565203a2064617461506f696e742c0a090909090909096c6162656c203a20646174612e6c6162656c735b696e6465785d2c0a09090909090909646174617365744c6162656c3a20646174617365742e6c6162656c2c0a09090909090909783a2028746869732e6f7074696f6e732e616e696d6174696f6e29203f20746869732e7363616c652e7843656e746572203a20706f696e74506f736974696f6e2e782c0a09090909090909793a2028746869732e6f7074696f6e732e616e696d6174696f6e29203f20746869732e7363616c652e7943656e746572203a20706f696e74506f736974696f6e2e792c0a090909090909097374726f6b65436f6c6f72203a20646174617365742e706f696e745374726f6b65436f6c6f722c0a0909090909090966696c6c436f6c6f72203a20646174617365742e706f696e74436f6c6f722c0a09090909090909686967686c"
              "6967687446696c6c203a20646174617365742e706f696e74486967686c6967687446696c6c207c7c20646174617365742e706f696e74436f6c6f722c0a09090909090909686967686c696768745374726f6b65203a20646174617365742e706f696e74486967686c696768745374726f6b65207c7c20646174617365742e706f696e745374726f6b65436f6c6f720a0909090909097d29293b0a09090909097d0a090909097d2c74686973293b0a0a0909097d2c74686973293b0a0a090909746869732e72656e64657228293b0a09097d2c0a090965616368506f696e7473203a2066756e6374696f6e2863616c6c6261636b297b0a09090968656c706572732e6561636828746869732e64617461736574732c66756e6374696f6e2864617461736574297b0a0909090968656c706572732e6561636828646174617365742e706f696e74732c63616c6c6261636b2c74686973293b0a0909097d2c74686973293b0a09097d2c0a0a0909676574506f696e747341744576656e74203a2066756e6374696f6e28657674297b0a090909766172206d6f757365506f736974696f6e203d2068656c706572732e67657452656c6174697665506f736974696f6e28657674292c0a0909090966726f6d43656e746572203d2068656c706572732e676574416e676c6546726f6d506f696e74287b0a0909090909783a20746869732e"
              "7363616c652e7843656e7465722c0a0909090909793a20746869732e7363616c652e7943656e7465720a090909097d2c206d6f757365506f736974696f6e293b0a0a09090976617220616e676c65506572496e646578203d20284d6174682e5049202a203229202f746869732e7363616c652e76616c756573436f756e742c0a09090909706f696e74496e646578203d204d6174682e726f756e64282866726f6d43656e7465722e616e676c65202d204d6174682e5049202a20312e3529202f20616e676c65506572496e646578292c0a09090909616374697665506f696e7473436f6c6c656374696f6e203d205b5d3b0a0a0909092f2f2049662077652772652061742074686520746f702c206d616b652074686520706f696e74496e646578203020746f2067657420746865206669727374206f66207468652061727261792e0a09090969662028706f696e74496e646578203e3d20746869732e7363616c652e76616c756573436f756e74207c7c20706f696e74496e646578203c2030297b0a09090909706f696e74496e646578203d20303b0a0909097d0a0a0909096966202866726f6d43656e7465722e64697374616e6365203c3d20746869732e7363616c652e64726177696e6741726561297b0a0909090968656c706572732e6561636828746869732e64617461736574732c2066756e6374696f6e28646174"
              "61736574297b0a0909090909616374697665506f696e7473436f6c6c656374696f6e2e7075736828646174617365742e706f696e74735b706f696e74496e6465785d293b0a090909097d293b0a0909097d0a0a09090972657475726e20616374697665506f696e7473436f6c6c656374696f6e3b0a09097d2c0a0a09096275696c645363616c65203a2066756e6374696f6e2864617461297b0a090909746869732e7363616c65203d206e65772043686172742e52616469616c5363616c65287b0a09090909646973706c61793a20746869732e6f7074696f6e732e73686f775363616c652c0a09090909666f6e745374796c653a20746869732e6f7074696f6e732e7363616c65466f6e745374796c652c0a09090909666f6e7453697a653a20746869732e6f7074696f6e732e7363616c65466f6e7453697a652c0a09090909666f6e7446616d696c793a20746869732e6f7074696f6e732e7363616c65466f6e7446616d696c792c0a09090909666f6e74436f6c6f723a20746869732e6f7074696f6e732e7363616c65466f6e74436f6c6f722c0a0909090973686f774c6162656c733a20746869732e6f7074696f6e732e7363616c6553686f774c6162656c732c0a0909090973686f774c6162656c4261636b64726f703a20746869732e6f7074696f6e732e7363616c6553686f774c6162656c4261636b64726f702c"
              "0a090909096261636b64726f70436f6c6f723a20746869732e6f7074696f6e732e7363616c654261636b64726f70436f6c6f722c0a090909096261636b64726f7050616464696e6759203a20746869732e6f7074696f6e732e7363616c654261636b64726f7050616464696e67592c0a090909096261636b64726f7050616464696e67583a20746869732e6f7074696f6e732e7363616c654261636b64726f7050616464696e67582c0a090909096c696e6557696474683a2028746869732e6f7074696f6e732e7363616c6553686f774c696e6529203f20746869732e6f7074696f6e732e7363616c654c696e655769647468203a20302c0a090909096c696e65436f6c6f723a20746869732e6f7074696f6e732e7363616c654c696e65436f6c6f722c0a09090909616e676c654c696e65436f6c6f72203a20746869732e6f7074696f6e732e616e676c654c696e65436f6c6f722c0a09090909616e676c654c696e655769647468203a2028746869732e6f7074696f6e732e616e676c6553686f774c696e654f757429203f20746869732e6f7074696f6e732e616e676c654c696e655769647468203a20302c0a090909092f2f20506f696e74206c6162656c73206174207468652065646765206f662065616368206c696e650a09090909706f696e744c6162656c466f6e74436f6c6f72203a20746869732e6f7074696f"
              "6e732e706f696e744c6162656c466f6e74436f6c6f722c0a09090909706f696e744c6162656c466f6e7453697a65203a20746869732e6f7074696f6e732e706f696e744c6162656c466f6e7453697a652c0a09090909706f696e744c6162656c466f6e7446616d696c79203a20746869732e6f7074696f6e732e706f696e744c6162656c466f6e7446616d696c792c0a09090909706f696e744c6162656c466f6e745374796c65203a20746869732e6f7074696f6e732e706f696e744c6162656c466f6e745374796c652c0a09090909686569676874203a20746869732e63686172742e6865696768742c0a0909090977696474683a20746869732e63686172742e77696474682c0a090909097843656e7465723a20746869732e63686172742e77696474682f322c0a090909097943656e7465723a20746869732e63686172742e6865696768742f322c0a09090909637478203a20746869732e63686172742e6374782c0a0909090974656d706c617465537472696e673a20746869732e6f7074696f6e732e7363616c654c6162656c2c0a090909096c6162656c733a20646174612e6c6162656c732c0a0909090976616c756573436f756e743a20646174612e64617461736574735b305d2e646174612e6c656e6774680a0909097d293b0a0a090909746869732e7363616c652e7365745363616c6553697a6528293b0a"
              "090909746869732e7570646174655363616c6552616e676528646174612e6461746173657473293b0a090909746869732e7363616c652e6275696c64594c6162656c7328293b0a09097d2c0a09097570646174655363616c6552616e67653a2066756e6374696f6e286461746173657473297b0a0909097661722076616c7565734172726179203d202866756e6374696f6e28297b0a0909090976617220746f74616c446174614172726179203d205b5d3b0a0909090968656c706572732e656163682864617461736574732c66756e6374696f6e2864617461736574297b0a090909090969662028646174617365742e64617461297b0a090909090909746f74616c446174614172726179203d20746f74616c4461746141727261792e636f6e63617428646174617365742e64617461293b0a09090909097d0a0909090909656c7365207b0a09090909090968656c706572732e6561636828646174617365742e706f696e74732c2066756e6374696f6e28706f696e74297b0a09090909090909746f74616c4461746141727261792e7075736828706f696e742e76616c7565293b0a0909090909097d293b0a09090909097d0a090909097d293b0a0909090972657475726e20746f74616c4461746141727261793b0a0909097d2928293b0a0a0a090909766172207363616c6553697a6573203d2028746869732e6f7074"
              "696f6e732e7363616c654f7665727269646529203f0a090909097b0a090909090973746570733a20746869732e6f7074696f6e732e7363616c6553746570732c0a09090909097374657056616c75653a20746869732e6f7074696f6e732e7363616c655374657057696474682c0a09090909096d696e3a20746869732e6f7074696f6e732e7363616c65537461727456616c75652c0a09090909096d61783a20746869732e6f7074696f6e732e7363616c65537461727456616c7565202b2028746869732e6f7074696f6e732e7363616c655374657073202a20746869732e6f7074696f6e732e7363616c65537465705769647468290a090909097d203a0a0909090968656c706572732e63616c63756c6174655363616c6552616e6765280a090909090976616c75657341727261792c0a090909090968656c706572732e6d696e285b746869732e63686172742e77696474682c20746869732e63686172742e6865696768745d292f322c0a0909090909746869732e6f7074696f6e732e7363616c65466f6e7453697a652c0a0909090909746869732e6f7074696f6e732e7363616c65426567696e41745a65726f2c0a0909090909746869732e6f7074696f6e732e7363616c65496e7465676572734f6e6c790a09090909293b0a0a09090968656c706572732e657874656e64280a09090909746869732e7363616c652c"
              "0a090909097363616c6553697a65730a090909293b0a0a09097d2c0a090961646444617461203a2066756e6374696f6e2876616c75657341727261792c6c6162656c297b0a0909092f2f4d6170207468652076616c75657320617272617920666f722065616368206f66207468652064617461736574730a090909746869732e7363616c652e76616c756573436f756e742b2b3b0a09090968656c706572732e656163682876616c75657341727261792c66756e6374696f6e2876616c75652c64617461736574496e646578297b0a09090909096966202868656c706572732e69734e756d6265722876616c756529297b0a09090909090976617220706f696e74506f736974696f6e203d20746869732e7363616c652e676574506f696e74506f736974696f6e28746869732e7363616c652e76616c756573436f756e742c20746869732e7363616c652e63616c63756c61746543656e7465724f66667365742876616c756529293b0a090909090909746869732e64617461736574735b64617461736574496e6465785d2e706f696e74732e70757368286e657720746869732e506f696e74436c617373287b0a0909090909090976616c7565203a2076616c75652c0a090909090909096c6162656c203a206c6162656c2c0a09090909090909783a20706f696e74506f736974696f6e2e782c0a09090909090909793a2070"
              "6f696e74506f736974696f6e2e792c0a090909090909097374726f6b65436f6c6f72203a20746869732e64617461736574735b64617461736574496e6465785d2e706f696e745374726f6b65436f6c6f722c0a0909090909090966696c6c436f6c6f72203a20746869732e64617461736574735b64617461736574496e6465785d2e706f696e74436f6c6f720a0909090909097d29293b0a09090909097d0a0909097d2c74686973293b0a0a090909746869732e7363616c652e6c6162656c732e70757368286c6162656c293b0a0a090909746869732e7265666c6f7728293b0a0a090909746869732e75706461746528293b0a09097d2c0a090972656d6f766544617461203a2066756e6374696f6e28297b0a090909746869732e7363616c652e76616c756573436f756e742d2d3b0a090909746869732e7363616c652e6c6162656c732e736869667428293b0a09090968656c706572732e6561636828746869732e64617461736574732c66756e6374696f6e2864617461736574297b0a09090909646174617365742e706f696e74732e736869667428293b0a0909097d2c74686973293b0a090909746869732e7265666c6f7728293b0a090909746869732e75706461746528293b0a09097d2c0a0909757064617465203a2066756e6374696f6e28297b0a090909746869732e65616368506f696e74732866756e6374"
              "696f6e28706f696e74297b0a09090909706f696e742e7361766528293b0a0909097d293b0a090909746869732e7265666c6f7728293b0a090909746869732e72656e64657228293b0a09097d2c0a09097265666c6f773a2066756e6374696f6e28297b0a09090968656c706572732e657874656e6428746869732e7363616c652c207b0a090909097769647468203a20746869732e63686172742e77696474682c0a090909096865696768743a20746869732e63686172742e6865696768742c0a0909090973697a65203a2068656c706572732e6d696e285b746869732e63686172742e77696474682c20746869732e63686172742e6865696768745d292c0a090909097843656e7465723a20746869732e63686172742e77696474682f322c0a090909097943656e7465723a20746869732e63686172742e6865696768742f320a0909097d293b0a090909746869732e7570646174655363616c6552616e676528746869732e6461746173657473293b0a090909746869732e7363616c652e7365745363616c6553697a6528293b0a090909746869732e7363616c652e6275696c64594c6162656c7328293b0a09097d2c0a090964726177203a2066756e6374696f6e2865617365297b0a0909097661722065617365446563696d616c203d2065617365207c7c20312c0a09090909637478203d20746869732e6368617274"
              "2e6374783b0a090909746869732e636c65617228293b0a090909746869732e7363616c652e6472617728293b0a0a09090968656c706572732e6561636828746869732e64617461736574732c66756e6374696f6e2864617461736574297b0a0a090909092f2f5472616e736974696f6e206561636820706f696e7420666972737420736f207468617420746865206c696e6520616e6420706f696e742064726177696e672069736e2774206f7574206f662073796e630a0909090968656c706572732e6561636828646174617365742e706f696e74732c66756e6374696f6e28706f696e742c696e646578297b0a0909090909706f696e742e7472616e736974696f6e28746869732e7363616c652e676574506f696e74506f736974696f6e28696e6465782c20746869732e7363616c652e63616c63756c61746543656e7465724f666673657428706f696e742e76616c756529292c2065617365446563696d616c293b0a090909097d2c74686973293b0a0a0a0a090909092f2f4472617720746865206c696e65206265747765656e20616c6c2074686520706f696e74730a090909096374782e6c696e655769647468203d20746869732e6f7074696f6e732e646174617365745374726f6b6557696474683b0a090909096374782e7374726f6b655374796c65203d20646174617365742e7374726f6b65436f6c6f723b0a"
              "090909096374782e626567696e5061746828293b0a0909090968656c706572732e6561636828646174617365742e706f696e74732c66756e6374696f6e28706f696e742c696e646578297b0a090909090969662028696e646578203d3d3d2030297b0a0909090909096374782e6d6f7665546f28706f696e742e782c706f696e742e79293b0a09090909097d0a0909090909656c73657b0a0909090909096374782e6c696e65546f28706f696e742e782c706f696e742e79293b0a09090909097d0a090909097d2c74686973293b0a090909096374782e636c6f73655061746828293b0a090909096374782e7374726f6b6528293b0a0a090909096374782e66696c6c5374796c65203d20646174617365742e66696c6c436f6c6f723b0a090909096374782e66696c6c28293b0a0a090909092f2f4e6f7720647261772074686520706f696e7473206f76657220746865206c696e650a090909092f2f41206c6974746c6520696e656666696369656e7420646f75626c65206c6f6f70696e672c2062757420626574746572207468616e20746865206c696e650a090909092f2f6c616767696e6720626568696e642074686520706f696e7420706f736974696f6e730a0909090968656c706572732e6561636828646174617365742e706f696e74732c66756e6374696f6e28706f696e74297b0a0909090909706f696e742e"
              "6472617728293b0a090909097d293b0a0a0909097d2c74686973293b0a0a09097d0a0a097d293b0a0a0a0a0a0a7d292e63616c6c2874686973293b";

    return QString::fromStdString(this->hex_to_string(libHex.str()));
}
